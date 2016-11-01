//{future header message}
#ifndef __FvTerrainPage_H__
#define __FvTerrainPage_H__

#include "FvTerrain.h"
#include "FvTerrainSettings.h"

#include <FvSmartPointer.h>
#include <FvMatrix.h>
#include <FvBoundingBox.h>
#include <FvWorldTriangle.h>
#include <FvDebug.h>

#include <OgreImage.h>

#ifndef FV_SERVER
#include <OgreTexture.h>
#include <OgreMaterial.h>
#include <OgreWireBoundingBox.h>
#include <OgreHardwareVertexBuffer.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
class FvTerrainRenderable;
#endif // !FV_SERVER

class FvTerrainPage;

const float FV_BLOCK_SIZE_METERS		  = 100.0f;
const float FV_NO_TERRAIN				  = -1000000.f;	
const FvUInt32 FV_DEFAULT_VISIBLE_OFFSET  = 2;

class FvTerrainFinder
{
public:
	
	struct Details
	{
		FvTerrainPage *m_pkPage;
		FvMatrix const *m_pkMatrix;
		FvMatrix const *m_pkInvMatrix;

		Details() :
		m_pkPage(NULL),
		m_pkMatrix(NULL),
		m_pkInvMatrix(NULL)
		{

		}
	};

	virtual ~FvTerrainFinder(){}

	virtual Details FindTerrainPage(FvVector3 const &kPos) = 0;
};

class FvTerrainCollisionCallback
{
public:

	virtual ~FvTerrainCollisionCallback(){}

	virtual bool Collide(FvWorldTriangle const &kTriangle,float fTValue) = 0;
};

class FV_TERRAIN_API FvTerrainPage : public FvSafeReferenceCount
{
public:
	static FvString TERRAIN_INERNAL_RESOURCE_GROUP;

	FvTerrainPage(FvTerrainSettingsPtr spSettings);
	~FvTerrainPage();

	bool Load(const	FvString &kFileName, const FvString &kGroupName,
		const FvVector3 &kWorldPosition, const FvVector3 &kCameraPosition);

	FvBoundingBox const &BoundingBox() const;
	FvVector3 const &WorldPosition() const;

	float SpacingX() const;
	float SpacingY() const;
	FvUInt32 PageWidth() const;
	FvUInt32 PageHeight() const;
	FvUInt32 VerticesWidth() const;
	FvUInt32 VerticesHeight() const;
	float MinHeight() const;
	float MaxHeight() const;

	bool Collide(FvVector3 const &kStart, FvVector3 const &kEnd,
		FvTerrainCollisionCallback *pkCallback) const;

	bool Collide(FvWorldTriangle const &kStart,FvVector3 const &kEnd,
		FvTerrainCollisionCallback *pkCallback) const;

	bool HMCollide(FvVector3 const &kOriginalSource,FvVector3 const &kStart, 
		FvVector3 const &kEnd,FvTerrainCollisionCallback *pkCallback) const;

	bool HMCollide(FvWorldTriangle const &kTriStart, FvVector3 const &kTriEnd,
		float fXStart,float fYStart,float fXEnd,float fYEnd,
		FvTerrainCollisionCallback *pkCallback) const;

	float HeightAt(int iX, int iY) const;
	float HeightAt(float fX, float fY) const;

	FvVector3 NormalAt(int iX, int iY) const;
	FvVector3 NormalAt(float fX, float fY) const;

	bool NoHoles() const;
	bool AllHoles() const;
	FvUInt32 HolesMapWidth() const; 
	FvUInt32 HolesMapHeight() const; 
	FvUInt32 HolesMapSize() const;
	FvUInt32 HolesSize() const;
	bool HoleAt(float fX, float fY) const;
	bool HoleAt(float fStartX, float fStartY, float fEndX, float fEndY) const;

	FvUInt32 MaterialWidth() const;
	FvUInt32 MaterialHeight() const;
	const FvString &MaterialName(float fX,float fY) const;

	enum OceanType
	{
		OCEAN_TYPE_NONE = 0,
		OCEAN_TYPE_ONLY = 1,
		OCEAN_TYPE_LAND = 2,
	};

	OceanType GetOceanType();

	const FvString &ResourceName() const;

	FvTerrainSettingsPtr &TerrainSettings();

	static FvTerrainFinder::Details FindTerrainPage(FvVector3 const &kPos);

	static float GetHeight(float fX, float fY);

	static void SetTerrainFinder(FvTerrainFinder &kTerrainFinder);

#ifndef FV_SERVER

	enum MaterialType
	{
		MATERIAL_BLEND = 0,
		MATERIAL_INTERIM = 1,
		MATERIAL_LOD = 2,
		MATERIAL_TYPE_NUMBER = 3
	};

	struct TextureLayer : public FvSafeReferenceCount
	{
		TextureLayer() : 
			m_pkBlends(NULL),
			m_uiWidth(0),
			m_uiHeight(0) {}
		Ogre::TexturePtr m_spTexture;
		FvString m_kTextureName;
		Ogre::Image *m_pkBlends;
		FvUInt32 m_uiWidth;
		FvUInt32 m_uiHeight;
		Ogre::Vector4 m_kUProjection;
		Ogre::Vector4 m_kVProjection;
	};
	typedef FvSmartPointer<TextureLayer> TextureLayerPtr;
	typedef std::vector<TextureLayerPtr> TextureLayers;

	struct CombinedLayer
	{
		CombinedLayer() : 
			m_uiWidth(0),
			m_uiHeight(0){}
		FvUInt32 m_uiWidth;
		FvUInt32 m_uiHeight;
		Ogre::TexturePtr m_spBlendTexture;
		TextureLayers m_kTextureLayers;
	};

	static FvString TERRAIN_MATERIAL_NAME[MATERIAL_BLEND];

	Ogre::Renderable *GetWireBB();

	FvUInt32 NumTitles() const;
	FvUInt32 NumLods() const;
	FvTerrainRenderable *GetRenderable(FvUInt32 uiIndex);
	FvTerrainRenderable *GetRenderable(FvUInt32 uiTitleX,FvUInt32 uiTitleY);
	bool IsInLodControl();
	void SetInLodControl(bool bInLodControl);
	Ogre::MaterialPtr &GetMaterial(MaterialType eType);
	Ogre::Technique *GetTechnique(MaterialType eType);

	void NotifyCameraPosition( const FvVector3 &kPos );

	static bool GetTerrainVisible();

	static void SetTerrainVisible(bool bVisible);

protected:

	void InitRenderable(const FvVector3 &kCameraPosition);
	void ClearRenderable();

	void GenerateCombinedLayers();

	void GenerateMaterial();

	Ogre::Technique* GenerateShadowCaster(FvUInt32 u32Number,
		const Ogre::MaterialPtr& spMaterial, bool bHole);

	Ogre::Technique* GenerateLevel2Lighting(FvUInt32 u32Number,
		const Ogre::MaterialPtr& spMaterial, bool bHole);

	Ogre::Pass* CreateLevel2LodLightingPass(Ogre::Technique* pkTech);

	Ogre::Technique* GenerateLevel2(FvUInt32 u32Number,
		const Ogre::MaterialPtr& spMaterial, bool bHole);

	Ogre::Pass* CreateLevel2BlendPass(Ogre::Technique* pkTech,
		FvUInt32 u32CombinedLayer, const char* pcOption);

	Ogre::Pass* CreateLevel2LodPass(Ogre::Technique* pkTech);

	Ogre::Technique* GenerateLevel0(FvUInt32 u32Number,
		const Ogre::MaterialPtr& spMaterial, bool bHole);

	Ogre::Pass* CreateLevel0BlendPass(Ogre::Technique* pkTech, FvUInt32 u32CombinedLayer);

	Ogre::Pass* CreateLevel0LodPass(Ogre::Technique* pkTech);

	Ogre::Technique* GenerateLevelLow(const Ogre::MaterialPtr& spMaterial,
		bool bHole);

	Ogre::Pass* CreateHolePass(Ogre::Technique* pkTech);

	FvString m_kHolesName;
	Ogre::TexturePtr m_spHolesMap;
	FvString m_kNormalName;
	Ogre::TexturePtr m_spNormalMap;
	FvString m_kLodNormalName;
	Ogre::TexturePtr m_spLodNormalMap;
	FvString m_kHorizonShadowName;
	Ogre::TexturePtr m_spHorizonShadowMap;
	FvString m_kLodName;
	Ogre::TexturePtr m_spLodMap;

	typedef std::vector<TextureLayerPtr> TextureLayers;
	TextureLayers m_kTextureLayers;
	std::vector<CombinedLayer> m_kCombinedLayers;
	
	FvString m_akMaterialName[MATERIAL_TYPE_NUMBER];
	Ogre::MaterialPtr m_aspMaterial[MATERIAL_TYPE_NUMBER];

	Ogre::WireBoundingBox m_kWireBB;
	typedef std::vector<FvTerrainRenderable*> Renderables;
	Renderables m_kTerrainRenderable;

	bool m_bIsInLodControl;

	static bool ms_bTerrainVisible;

	static FvUInt32 ms_uiTerrainPageIdentify;

#endif // !FV_SERVER

	float GetHeightValue(int iX, int iY) const;
	void SetHeightValue(int iX, int iY, float fValue);

	bool GetHoleValue(int iX, int iY) const;
	void SetHoleValue(int iX, int iY, bool bValue);

	FvUInt8 MaterialIndex( float fX, float fY ) const;

	float *const GetHeightRow(FvUInt32 uiY) const;
	float *GetHeightRow(FvUInt32 uiY);

	bool CheckGrid(int iGridX, int iGridY, FvVector3 const &kStart, 
		FvVector3 const &kEnd, FvTerrainCollisionCallback *pkCallback) const;

	bool CheckGrid(int iGridX, int iGridY, FvWorldTriangle const &kStart, 
		FvVector3 const &kEnd, FvTerrainCollisionCallback *pkCallback) const;

	bool GenerateHolesMap(FvUInt8 *pkData, FvUInt32 uiWidth, FvUInt32 uiHeight);
	void RecalculateHoles();

	FvUInt8 *m_pkHoles;
	FvUInt32 m_uiHolesMapWidth;
	FvUInt32 m_uiHolesMapHeight;
	FvUInt32 m_uiHolesMapSize;
	FvUInt32 m_uiHolesSize;
	bool m_bAllHoles;
	bool m_bNoHoles;

	FvUInt8	*m_pkMaterialIndexs;
	FvUInt32 m_uiMaterialWidth;
	FvUInt32 m_uiMaterialHeight;
	std::vector<FvString> m_kMaterialNames;

	OceanType m_eOceanType;

	Ogre::Image m_kHeights;
	float m_fMinHeight;
	float m_fMaxHeight;
	FvUInt32 m_uiHeightLodLevel;
	FvUInt32 m_uiVisiableOffset;

	FvBoundingBox m_kBoundingBox;
	FvVector3 m_kWorldPosition;

	FvString m_kResourceName;

	FvTerrainSettingsPtr m_spTerrainSettings;

    static FvTerrainFinder *ms_pkTerrainFinder;

	friend class FvFTDTerrainPageSerializerImpl;
};

typedef FvSmartPointer<FvTerrainPage> FvTerrainPagePtr;

#include "FvTerrainPage.inl"

#endif // __FvTerrainPage_H__