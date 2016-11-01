#include "FvOcean.h"
#include <OgreMaterialManager.h>
#include <FvDebug.h>
#include <FvMathPower.h>
#include <OgreGpuProgram.h>
#include <OgreTechnique.h>

Ogre::MaterialPtr FvOcean::ms_spOceanMaterial;
FvOcean::WaveDesc FvOcean::m_GeoWaves[FV_OCEAN_WAVE_NUM];
FvOcean::GeoState FvOcean::m_GeoState;

//----------------------------------------------------------------------------
void FvOcean::Init()
{
	ms_spOceanMaterial = Ogre::MaterialManager::getSingleton().load("FvOcean", "General");
	FV_ASSERT(!ms_spOceanMaterial.isNull());
	ResetWater();
}
//----------------------------------------------------------------------------
void FvOcean::Term()
{
	ms_spOceanMaterial.setNull();
}
//----------------------------------------------------------------------------
void FvOcean::Tick(float fDeltaTime)
{
	float fRand0 = FvSymmetricRandomf();
	float fRand1 = FvSymmetricRandomf();

	FvVector2 kDir = m_GeoState.m_WindDir + FvVector2(fRand0, fRand1);
	kDir.Normalise();
	kDir *= m_GeoState.m_TexVec * fDeltaTime;

	m_GeoState.m_Tex0Phase.x += kDir.x;
	m_GeoState.m_Tex0Phase.y += kDir.y;

	m_GeoState.m_Tex1Phase.x -= kDir.x;
	m_GeoState.m_Tex1Phase.y -= kDir.y;

	UpdateGeoWaves(fDeltaTime);
	SetParams();
}
//----------------------------------------------------------------------------
Ogre::Technique* FvOcean::GetTechnique()
{
	FV_ASSERT(!ms_spOceanMaterial.isNull());
	return ms_spOceanMaterial->getTechnique(0);
}
//----------------------------------------------------------------------------
void FvOcean::ResetWater()
{
	InitGeoState();
	InitWaves();
}
//----------------------------------------------------------------------------
void FvOcean::InitWaves()
{
	InitGeoWaves();
}
//----------------------------------------------------------------------------
void FvOcean::InitGeoState()
{
	m_GeoState.m_Chop = 2.5f;
	m_GeoState.m_AngleDeviation = 30.f;
	m_GeoState.m_WindDir.x = 0.5f;
	m_GeoState.m_WindDir.y = 0.5f;

	m_GeoState.m_MinLength = 1.f;
	m_GeoState.m_MaxLength = 5.f;
	m_GeoState.m_AmpOverLen = 0.1f;

	m_GeoState.m_EnvHeight = -50.f;
	m_GeoState.m_EnvRadius = 100.f;
	m_GeoState.m_WaterLevel = -2.f;

	m_GeoState.m_TransIdx = 0;
	m_GeoState.m_TransDel = -1.f / 6.f;

	m_GeoState.m_SpecAtten = 1.f;
	m_GeoState.m_SpecEnd = 200.f;
	m_GeoState.m_SpecTrans = 100.f;

	m_GeoState.m_Tex0Phase = FvVector4(0,0,0.5f,0.5f);
	m_GeoState.m_Tex1Phase = FvVector4(0,0,0.3f,0.3f);
	m_GeoState.m_TexVec = 0.5f;
}
//----------------------------------------------------------------------------
void FvOcean::InitGeoWaves()
{
	for(FvUInt32 i = 0; i < FV_OCEAN_WAVE_NUM; ++i)
		InitGeoWave(i);
}
//----------------------------------------------------------------------------
void FvOcean::InitGeoWave(FvInt32 i)
{
	m_GeoWaves[i].m_Phase = FvUnitRandomf() * FV_MATH_PI_F * 2.f;
	m_GeoWaves[i].m_Len = m_GeoState.m_MinLength + FvUnitRandomf() * (m_GeoState.m_MaxLength - m_GeoState.m_MinLength);
	m_GeoWaves[i].m_Amp = m_GeoWaves[i].m_Len * m_GeoState.m_AmpOverLen / float(FV_OCEAN_WAVE_NUM);
	m_GeoWaves[i].m_Freq = 2.f * FV_MATH_PI_F / m_GeoWaves[i].m_Len;
	m_GeoWaves[i].m_Fade = 1.f;

	float rotBase = m_GeoState.m_AngleDeviation * FV_MATH_PI_F / 180.f;

	float rads = rotBase * FvSymmetricRandomf();
	float rx = float(cosf(rads));
	float ry = float(sinf(rads));

	float x = m_GeoState.m_WindDir.x;
	float y = m_GeoState.m_WindDir.y;
	m_GeoWaves[i].m_Dir.x = x * rx + y * ry;
	m_GeoWaves[i].m_Dir.y = x * -ry + y * rx;
}
//----------------------------------------------------------------------------
void FvOcean::UpdateGeoWaves(float fDeltaTime)
{
	for(FvUInt32 i = 0; i < FV_OCEAN_WAVE_NUM; i++ )
		UpdateGeoWave(i, fDeltaTime);
}
//----------------------------------------------------------------------------
static const float kGravConst = 30.f;
void FvOcean::UpdateGeoWave(FvInt32 i, float dt)
{
	if( i == m_GeoState.m_TransIdx )
	{
		m_GeoWaves[i].m_Fade += m_GeoState.m_TransDel * dt;
		if( m_GeoWaves[i].m_Fade < 0 )
		{
			// This wave is faded out. Re-init and fade it back up.
			InitGeoWave(i);
			m_GeoWaves[i].m_Fade = 0;
			m_GeoState.m_TransDel = -m_GeoState.m_TransDel;
		}
		else if( m_GeoWaves[i].m_Fade > 1.f )
		{
			// This wave is faded back up. Start fading another down.
			m_GeoWaves[i].m_Fade = 1.f;
			m_GeoState.m_TransDel = -m_GeoState.m_TransDel;
			if( ++m_GeoState.m_TransIdx >= FV_OCEAN_WAVE_NUM )
				m_GeoState.m_TransIdx = 0;
		}
	}

	const float speed = float(1.0 / sqrt(m_GeoWaves[i].m_Len / (2.f * FV_MATH_PI_F * kGravConst)));

	m_GeoWaves[i].m_Phase += speed * dt;
	m_GeoWaves[i].m_Phase = float(fmod(m_GeoWaves[i].m_Phase, 2.f*FV_MATH_PI_F));

	m_GeoWaves[i].m_Amp = m_GeoWaves[i].m_Len * m_GeoState.m_AmpOverLen / float(FV_OCEAN_WAVE_NUM) * m_GeoWaves[i].m_Fade;
}
//----------------------------------------------------------------------------
void FvOcean::SetParams()
{
	Ogre::Technique* pkTechnique = GetTechnique();

	Ogre::GpuProgramParametersSharedPtr spParams = pkTechnique->getPass(0)->getVertexProgramParameters();

	Ogre::Vector4 dirX(m_GeoWaves[0].m_Dir.x, m_GeoWaves[1].m_Dir.x, m_GeoWaves[2].m_Dir.x, m_GeoWaves[3].m_Dir.x);
	Ogre::Vector4 dirY(m_GeoWaves[0].m_Dir.y, m_GeoWaves[1].m_Dir.y, m_GeoWaves[2].m_Dir.y, m_GeoWaves[3].m_Dir.y);

	Ogre::Vector4 amp(m_GeoWaves[0].m_Amp, m_GeoWaves[1].m_Amp, m_GeoWaves[2].m_Amp, m_GeoWaves[3].m_Amp);

	Ogre::Vector4 freq(m_GeoWaves[0].m_Freq, m_GeoWaves[1].m_Freq, m_GeoWaves[2].m_Freq, m_GeoWaves[3].m_Freq);

	Ogre::Vector4 phase(m_GeoWaves[0].m_Phase, m_GeoWaves[1].m_Phase, m_GeoWaves[2].m_Phase, m_GeoWaves[3].m_Phase);

	Ogre::Vector4 lengths(m_GeoWaves[0].m_Len, m_GeoWaves[1].m_Len, m_GeoWaves[2].m_Len, m_GeoWaves[3].m_Len);

	spParams->setConstant(12, dirX);
	spParams->setConstant(13, dirY);
	spParams->setConstant(14, amp);
	spParams->setConstant(15, freq);
	spParams->setConstant(16, phase);
	spParams->setConstant(17, lengths);

	float K = 5.f;
	if( m_GeoState.m_AmpOverLen > m_GeoState.m_Chop / (2.f * FV_MATH_PI_F * FV_OCEAN_WAVE_NUM * K) )
		K = m_GeoState.m_Chop / (2.f*FV_MATH_PI_F* m_GeoState.m_AmpOverLen * FV_OCEAN_WAVE_NUM);
	Ogre::Vector4 dirXK(m_GeoWaves[0].m_Dir.x * K, 
		m_GeoWaves[1].m_Dir.x * K, 
		m_GeoWaves[2].m_Dir.x * K, 
		m_GeoWaves[3].m_Dir.x * K);
	Ogre::Vector4 dirYK(m_GeoWaves[0].m_Dir.y * K, 
		m_GeoWaves[1].m_Dir.y * K, 
		m_GeoWaves[2].m_Dir.y * K, 
		m_GeoWaves[3].m_Dir.y * K);
	
	spParams->setConstant(18, dirXK);
	spParams->setConstant(19, dirYK);

	Ogre::Vector4 dirXSqKW(m_GeoWaves[0].m_Dir.x * m_GeoWaves[0].m_Dir.x * K * m_GeoWaves[0].m_Freq,
		m_GeoWaves[1].m_Dir.x * m_GeoWaves[1].m_Dir.x * K * m_GeoWaves[1].m_Freq,
		m_GeoWaves[2].m_Dir.x * m_GeoWaves[2].m_Dir.x * K * m_GeoWaves[2].m_Freq,
		m_GeoWaves[3].m_Dir.x * m_GeoWaves[3].m_Dir.x * K * m_GeoWaves[3].m_Freq);

	Ogre::Vector4 dirYSqKW(m_GeoWaves[0].m_Dir.y * m_GeoWaves[0].m_Dir.y * K * m_GeoWaves[0].m_Freq,
		m_GeoWaves[1].m_Dir.y * m_GeoWaves[1].m_Dir.y * K * m_GeoWaves[1].m_Freq,
		m_GeoWaves[2].m_Dir.y * m_GeoWaves[2].m_Dir.y * K * m_GeoWaves[2].m_Freq,
		m_GeoWaves[3].m_Dir.y * m_GeoWaves[3].m_Dir.y * K * m_GeoWaves[3].m_Freq);

	Ogre::Vector4 dirXdirYKW(m_GeoWaves[0].m_Dir.y * m_GeoWaves[0].m_Dir.x * K * m_GeoWaves[0].m_Freq,
		m_GeoWaves[1].m_Dir.x * m_GeoWaves[1].m_Dir.y * K * m_GeoWaves[1].m_Freq,
		m_GeoWaves[2].m_Dir.x * m_GeoWaves[2].m_Dir.y * K * m_GeoWaves[2].m_Freq,
		m_GeoWaves[3].m_Dir.x * m_GeoWaves[3].m_Dir.y * K * m_GeoWaves[3].m_Freq);

	Ogre::Vector4 dirXW(m_GeoWaves[0].m_Dir.x * m_GeoWaves[0].m_Freq, 
		m_GeoWaves[1].m_Dir.x * m_GeoWaves[1].m_Freq, 
		m_GeoWaves[2].m_Dir.x * m_GeoWaves[2].m_Freq, 
		m_GeoWaves[3].m_Dir.x * m_GeoWaves[3].m_Freq);
	Ogre::Vector4 dirYW(m_GeoWaves[0].m_Dir.y * m_GeoWaves[0].m_Freq, 
		m_GeoWaves[1].m_Dir.y * m_GeoWaves[1].m_Freq, 
		m_GeoWaves[2].m_Dir.y * m_GeoWaves[2].m_Freq, 
		m_GeoWaves[3].m_Dir.y * m_GeoWaves[3].m_Freq);

	Ogre::Vector4 KW(K * m_GeoWaves[0].m_Freq,
		K * m_GeoWaves[1].m_Freq,
		K * m_GeoWaves[2].m_Freq,
		K * m_GeoWaves[3].m_Freq);

	spParams->setConstant(20, dirXSqKW);
	spParams->setConstant(21, dirXdirYKW);
	spParams->setConstant(22, dirYSqKW);
	spParams->setConstant(23, dirXW);
	spParams->setConstant(24, dirYW);
	spParams->setConstant(25, KW);

	spParams->setConstant(26, *(Ogre::Vector4*)&m_GeoState.m_Tex0Phase);
	spParams->setConstant(27, *(Ogre::Vector4*)&m_GeoState.m_Tex1Phase);

	spParams = pkTechnique->getPass(0)->getFragmentProgramParameters();

	{
		float fWidth = (float)1024;
		float fHeight = (float)768;		
		float fPixelX = 1.f / fWidth;
		float fPixelY = 1.f / fHeight;
		float fOffsetX = fPixelX*0.25f;
		float fOffsetY = fPixelY*0.25f + 1.f;
		spParams->setConstant(2, Ogre::Vector4(fOffsetX, fOffsetY, fOffsetY, fOffsetX));
	}
}
//----------------------------------------------------------------------------
void FvOcean::SetRefractionMap(const FvString& kName)
{
	GetTechnique()->getPass(0)->getTextureUnitState(1)->setTextureName(kName);
}
//----------------------------------------------------------------------------
void FvOcean::SetSunDirection( const Ogre::Vector4& kSunDir )
{
	Ogre::GpuProgramParametersSharedPtr spParams = GetTechnique()->getPass(0)->getFragmentProgramParameters();
	spParams->setConstant(5, kSunDir);
}
//----------------------------------------------------------------------------
void FvOcean::SetSunColor( const Ogre::Vector4& kSunColor )
{
	Ogre::GpuProgramParametersSharedPtr spParams = GetTechnique()->getPass(0)->getFragmentProgramParameters();
	spParams->setConstant(6, kSunColor);
}
//----------------------------------------------------------------------------
