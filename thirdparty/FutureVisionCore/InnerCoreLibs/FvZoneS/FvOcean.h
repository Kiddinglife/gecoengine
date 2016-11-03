//{future header message}
#ifndef __FvOcean_H__
#define __FvOcean_H__

#include <OgreMaterial.h>
#include <FvVector2.h>
#include <FvVector4.h>

#include "FvZoneDefine.h"

namespace Ogre
{
	class Technique;
};

#define FV_OCEAN_WAVE_NUM 4

class FV_ZONE_API FvOcean
{
public:
	static void Init();

	static void Term();

	static void Tick(float fDeltaTime);

	static Ogre::Technique* GetTechnique();

	static void SetRefractionMap(const FvString& kName);

	static void SetSunDirection(const Ogre::Vector4& kSunDir);

	static void SetSunColor(const Ogre::Vector4& kSunColor);

protected:
	static Ogre::MaterialPtr ms_spOceanMaterial;

	static void ResetWater();

	static void InitWaves();

	static void InitGeoState();

	static void InitGeoWaves();

	static void InitGeoWave(FvInt32 i);

	static void UpdateGeoWaves(float fDeltaTime);

	static void UpdateGeoWave(FvInt32 i, float dt);

	static void SetParams();

	struct WaveDesc
	{
		float		m_Phase;
		float		m_Amp;
		float		m_Len;
		float		m_Freq;
		FvVector2	m_Dir;
		float		m_Fade;
	};

	static WaveDesc m_GeoWaves[FV_OCEAN_WAVE_NUM];

	struct GeoState
	{
		float		m_Chop;
		float		m_AngleDeviation;
		FvVector2	m_WindDir;
		float		m_MinLength;
		float		m_MaxLength;
		float		m_AmpOverLen;

		float		m_SpecAtten;
		float		m_SpecEnd;
		float		m_SpecTrans;

		float		m_EnvHeight;
		float		m_EnvRadius;
		float		m_WaterLevel;

		FvInt32		m_TransIdx;
		float		m_TransDel;

		FvVector4   m_Tex0Phase;
		FvVector4   m_Tex1Phase;
		float       m_TexVec;
		
	};

	static GeoState m_GeoState;

};

#endif // __FvOcean_H__
