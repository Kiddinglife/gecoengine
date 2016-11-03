//{future header message}
#ifndef __FvTerrainSettings_H__
#define __FvTerrainSettings_H__

#include "FvTerrain.h"

#include <FvBaseTypes.h>
#include <FvGlobalMacros.h>
#include <FvSmartPointer.h>
#include <FvVector2.h>
#include <FvVector3.h>
#include <FvXMLSection.h>

#include <vector>

struct FV_TERRAIN_API FvTerrainSettings : public FvSafeReferenceCount
{
public:
	FvTerrainSettings();
	~FvTerrainSettings(); 	

	bool Init( FvXMLSectionPtr spSettings );

	FvUInt32 HeightMapSize() const;
	FvUInt32 NormalMapSize() const;
	FvUInt32 HoleMapSize() const;
	FvUInt32 ShadowMapSize() const;
	FvUInt32 BlendMapSize() const;

	bool UVProjections() const;

	FvUInt32 NumTitles() const;
	FvUInt32 NumLods() const;

	float LodDistance( FvUInt32 uiLod ) const;
	void LodDistance( FvUInt32 uiLod, float fValue );

	float LodStartBias() const;
	float LodEndBias() const;

	bool UseLodTexture();

	float LodTextureStart() const;
	float LodTextureDistance() const;
	float LodNormalStart() const;
	float LodNormalDistance() const;

	FvUInt32 DefaultHeightMapLod() const;

	FvUInt32 CalculateLodLevel( float fBlockDistance ) const;
	FvVector2 CalcMorphRanges( FvUInt32 uiLodLevel ) const;

	FvString TextureGroup() const;

	bool IsOceanMap(const FvString &kTexName);

	static void MinMaxXZDistance( const FvVector3& kRelativeCameraPos,
		const float fBlockSize, float &fMinDistance, float &fMaxDistance );

	void ApplyLodModifier(float modifier);

private:

	FvUInt32 m_uiHeightMapSize;
	FvUInt32 m_uiNormalMapSize;
	FvUInt32 m_uiHoleMapSize;
	FvUInt32 m_uiShadowMapSize;
	FvUInt32 m_uiBlendMapSize;

	bool m_bUVProjections;

	FvUInt32 m_uiNumTitles;
	FvUInt32 m_uiNumLods;

	float m_fLodStartBias;
	float m_fLodEndBias;

	bool m_bUseLodTexture;

	float m_fLodTextureStart;
	float m_fLodTextureDistance;

	float m_fLodNormalStart;
	float m_fLodNormalDistance;

	typedef std::vector<float> LodDistances;
	LodDistances m_kLodDistances;

	typedef std::vector<float> LodValues;
	LodValues m_kLodValues;

	FvUInt32 m_uiDefaultHeightMapLod;

	FvString m_kTextureGroup;

	std::set<FvString> m_kOcreanMaps;
};

typedef FvSmartPointer<FvTerrainSettings> FvTerrainSettingsPtr;

#include "FvTerrainSettings.inl"

#endif // __FvTerrainSettings_H__