#include "FvTerrainSettings.h"
#include "FvTerrainPage.h"

#include <FvDebug.h>

namespace
{
	const float FV_LAST_LOD_DIST = 1000000.f;

	FvUInt32 Log2RoundUp(FvUInt32 uiX)
	{
		FvUInt32 uiResult = 0;
		while ((uiX >> uiResult) > 1)
			uiResult++;
		return uiResult;
	}
}

FvTerrainSettings::FvTerrainSettings() :
m_uiHeightMapSize(132),
m_uiNormalMapSize(128),
m_uiHoleMapSize(128),
m_uiShadowMapSize(32),
m_uiBlendMapSize(128),
m_bUVProjections(true),	
m_uiNumTitles(2),
m_fLodStartBias(0.800000),
m_fLodEndBias(0.990000),
m_bUseLodTexture(true),
m_fLodTextureStart(200),
m_fLodTextureDistance(100),
m_fLodNormalStart(300),
m_fLodNormalDistance(100),
m_uiDefaultHeightMapLod(0)
{
	m_kTextureGroup = "General";
	m_uiNumLods = Log2RoundUp(m_uiHeightMapSize/m_uiNumTitles);
}

FvTerrainSettings::~FvTerrainSettings()
{

}

bool FvTerrainSettings::Init( FvXMLSectionPtr spSettings )
{
	m_uiHeightMapSize = spSettings->ReadInt("heightMapSize",m_uiHeightMapSize);
	m_uiNormalMapSize = spSettings->ReadInt("normalMapSize",m_uiNormalMapSize);
	m_uiHoleMapSize = spSettings->ReadInt("holeMapSize",m_uiHoleMapSize);
	m_uiShadowMapSize = spSettings->ReadInt("shadowMapSize",m_uiShadowMapSize);
	m_uiBlendMapSize = spSettings->ReadInt("blendMapSize",m_uiBlendMapSize);

	m_uiNumLods = Log2RoundUp(m_uiHeightMapSize/m_uiNumTitles);

	FV_ASSERT((1 << m_uiNumLods) == m_uiHeightMapSize/m_uiNumTitles);

	m_fLodStartBias = spSettings->ReadFloat("lodInfo/startBias",m_fLodStartBias);
	m_fLodEndBias = spSettings->ReadFloat("lodInfo/endBias",m_fLodEndBias);

	m_fLodTextureStart = spSettings->ReadFloat("lodInfo/lodTextureStart",m_fLodTextureStart);
	m_fLodTextureDistance = spSettings->ReadFloat("lodInfo/lodTextureDistance",m_fLodTextureDistance);

	m_fLodNormalStart = spSettings->ReadFloat("lodInfo/lodNormalStart",m_fLodNormalStart);
	m_fLodNormalDistance = spSettings->ReadFloat("lodInfo/lodNormalDistance",m_fLodNormalDistance);

	m_kLodDistances.clear();
	const FvUInt32 uiNumDistances = m_uiNumLods - 1;

	for ( FvUInt32 i = 0; i < uiNumDistances; i++ )
	{
		char acBuffer[64];
		_snprintf( acBuffer, (sizeof(acBuffer) / sizeof(acBuffer[0])), 
			"lodInfo/lodDistances/distance%u", i );
		m_kLodValues.push_back( spSettings->ReadFloat( acBuffer, 0.0f ) );
	}

	m_kTextureGroup = spSettings->ReadString("texutreGroup",m_kTextureGroup);

	this->ApplyLodModifier(1.0f);

	std::vector<FvXMLSectionPtr> kMaps;
	spSettings->OpenSections("ocean/map",kMaps);
	std::vector<FvXMLSectionPtr>::iterator kIt = kMaps.begin();
	for(; kIt != kMaps.end(); ++kIt)
	{
		FvString kMapName = (*kIt)->AsString();
		Ogre::StringUtil::toLowerCase(kMapName);
		m_kOcreanMaps.insert(kMapName);
	}

	return true;
}

float FvTerrainSettings::LodDistance( FvUInt32 uiLod ) const
{
	float fDist = FV_LAST_LOD_DIST;
	if (uiLod < m_kLodDistances.size())
	{
		fDist = m_kLodDistances[uiLod];
	}
	return fDist;
}

void FvTerrainSettings::LodDistance( FvUInt32 uiLod, float fValue )
{
	if (uiLod < m_kLodDistances.size())
	{
		m_kLodDistances[uiLod] = fValue;
	}
}

FvUInt32 FvTerrainSettings::CalculateLodLevel( float fBlockDistance ) const
{
	const FvUInt32 uiLods = m_kLodDistances.size();

	FvUInt32 uiLod = 0;

	for ( ; uiLod < uiLods; uiLod++ )
	{
		if ( fBlockDistance < m_kLodDistances[uiLod] )
		{
			break;
		}
	}

	return (uiLod < this->NumLods()) ? uiLod :(this->NumLods() - 1);
}

FvVector2 FvTerrainSettings::CalcMorphRanges( FvUInt32 uiLodLevel ) const
{
	float fLodDelta = LodDistance(uiLodLevel);
	float fLodStart = 0;

	if (uiLodLevel != 0)
		fLodStart = LodDistance(uiLodLevel - 1);

	fLodDelta -= fLodStart;

	FvVector2 kMorphRange( fLodStart + fLodDelta * m_fLodStartBias,
		fLodStart + fLodDelta * m_fLodEndBias );

	kMorphRange.y = 1.0f / (kMorphRange.y - kMorphRange.x);

	return kMorphRange;
}

void FvTerrainSettings::MinMaxXZDistance( const FvVector3& kRelativeCameraPos,
							 const float fBlockSize, float &fMinDistance, float &fMaxDistance )
{
	FvVector2 kMin(kRelativeCameraPos.x, kRelativeCameraPos.y);
	FvVector2 kMax = kMin;

	float fHalfBlockSize = fBlockSize * 0.5f;

	if (kRelativeCameraPos.x > fBlockSize)
	{
		kMin.x -= fBlockSize;
	}
	else if (kRelativeCameraPos.x > 0.f)
	{
		kMin.x = 0.f;
		kMax.x = fabsf(kMax.x - fHalfBlockSize) + fHalfBlockSize;
	}
	else
	{
		kMax.x -= fBlockSize;
	}

	if (kRelativeCameraPos.y > fBlockSize)
	{
		kMin.y -= fBlockSize;
	}
	else if (kRelativeCameraPos.y > 0.f)
	{
		kMin.y = 0.f;
		kMax.y = fabsf(kMax.y - fHalfBlockSize) + fHalfBlockSize;
	}
	else
	{
		kMax.y -= fBlockSize;
	}

	fMinDistance = kMin.Length();
	fMaxDistance = kMax.Length();
}

bool FvTerrainSettings::IsOceanMap(const FvString &kTexName)
{	
	return m_kOcreanMaps.find(kTexName) != m_kOcreanMaps.end();
}

void FvTerrainSettings::ApplyLodModifier(float fModifier)
{
	m_kLodDistances.clear();

	float fSubBlockSizeMeters = FV_BLOCK_SIZE_METERS/m_uiNumTitles;
	const float fMinDistance = 
		ceilf(sqrtf(fSubBlockSizeMeters * fSubBlockSizeMeters * 2) / 
		m_fLodStartBias);

	float fLastValue = -fMinDistance;

	for (FvUInt32 i = 0; i < m_kLodValues.size(); i++)
	{
		float fValue = floorf(m_kLodValues[i] * fModifier);
		if ((fValue - fLastValue) < fMinDistance)
			fValue = fLastValue + fMinDistance;

		m_kLodDistances.push_back( fValue );
		fLastValue = fValue;
	}
}
