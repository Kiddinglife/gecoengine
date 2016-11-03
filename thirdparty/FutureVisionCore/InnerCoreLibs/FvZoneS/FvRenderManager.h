//{future header message}
#ifndef __FvRenderManager_H__
#define __FvRenderManager_H__

#include <FvBaseTypes.h>
#include <FvVector3.h>

#include <OgreRenderQueue.h>
#include <OgreSceneManager.h>
#include <OgreMesh.h>
#include <OgreMaterial.h>
#include <FvRefList.h>
#include "FvModelDefines.h"

#include "FvCamera.h"
#include "FvZoneDefine.h"
#include "FvZoneVisitor.h"
#include <FvRefList.h>

class FvZone;
class FvCamera;
class FvBillboard;
class FvOcclusionQueryAABB;
struct IDirect3DTexture9;
struct IDirect3DSurface9;

class FV_ZONE_API FvRenderManager : public Ogre::SceneManager,
	public Ogre::RenderQueue::RenderableListener,
	public FvZoneVisitor
{
public:

	enum
	{
		MAX_OUTSIDE_ZONES	= 400,
		MAX_INSIDE_ZONES	= 10,
		MAX_VISIBLE_ZONES	= MAX_OUTSIDE_ZONES + MAX_INSIDE_ZONES,

		FIRST_GROUP_ID		= 30,

		COLOR_EFFECT_GROUP_ID = 51,
		FADE_OUT_GROUP_ID = 52,
		TREE_GROUP_ID = 53,
		EDGE_HIGH_LIGHT = 54,
		EDGE_HIGH_LIGHT_BLEND = 55,
		EDGE_HIGH_LIGHT_COLOR_EFFECT = 56,
		BLEND_GROUP_ID_ANIM = 87,
		BLEND_GROUP_ID = 88,
		
		OCEAN_GROUP_ID = 91,
		TRANSPARENT_GROUP_ID = 92,
	};

	class OutsideRenderingListener
	{
	public:
		virtual void Callback(bool bHasOutsideRendering) = 0;
	};

	FvRenderManager();
	~FvRenderManager();

	const Ogre::String &getTypeName() const;

	virtual void _findVisibleObjects( Ogre::Camera *pkCam, 
		Ogre::VisibleObjectsBoundsInfo *pkVisibleBounds, bool bOnlyShadowCasters );

	void _FindVisibleTerrains(FvCamera* pkCamera);

	void AddVisibleFinder(FvCamera::VisibleObjectFinder *pkFinder);
	void RemoveVisibleFinder(FvCamera::VisibleObjectFinder *pkFinder);

	virtual Ogre::Camera *createCamera(const Ogre::String &kName);

	virtual Ogre::SceneNode *createSceneNodeImpl(void);
	virtual Ogre::SceneNode *createSceneNodeImpl(const Ogre::String &kName);

	virtual bool renderableQueued(Ogre::Renderable *pkRend, FvUInt8 uiGroupID, 
		FvUInt16 uiPriority, Ogre::Technique **ppTech, Ogre::RenderQueue *pkQueue);

    virtual Ogre::RenderQueue *getRenderQueue(void);

	virtual void _populateLightList(const Ogre::Vector3 &kPosition, float fRadius, 
		Ogre::LightList &kDestList, FvUInt32 uiLightMask = 0xFFFFFFFF);
	virtual void _populateLightList(const Ogre::SceneNode *pkNode, float fRadius, 
		Ogre::LightList &kDestList, FvUInt32 uiLightMask = 0xFFFFFFFF);

	virtual bool validatePassForRendering(const Ogre::Pass* pass);

	virtual bool HasFlag(FvZoneVisitor::VisitFlag eFlag){return (m_uiVisitFlag & FvUInt16(eFlag));}

	virtual bool Visit(FvZone *pkZone);
	virtual bool Visit(FvZoneItem *pkItem){return true;}
	virtual bool Visit(FvZoneBoundary::Portal *pkPortal){return true;}

	virtual bool VisitPointLight(FvZoneOmniLight::Params* pkPointLight, bool bGlobal, bool bSpecular);

	virtual bool EnableVisitLight() {return m_bDeferredShadingEnable;}

	virtual Ogre::RenderQueue *GetRenderQueue(){return this->getRenderQueue();}
	virtual Ogre::LightList *GetLightList(){return NULL;}
	virtual Ogre::VisibleObjectsBoundsInfo *GetVisibleObjectsBoundsInfo(){return m_pkBoundsInfo;}

	void AttachExtraEntity(Ogre::Entity* pkEntity);
	void RemoveExtraEntity(Ogre::Entity* pkEntity);

	void SetDeferredShadingEnable(bool bEnable);
	bool GetDeferredShadingEnable();

	void SetDirLightInShellsEnable(bool bEnable);
	bool GetDirLightInShellsEnable();

	void SetHDREnable(bool bEnable);
	bool GetHDREnable();

	void SetShadowEnable(bool bEnable);
	bool GetShadowEnable();

	void SetInstanceEnable(bool bEnable);
	bool GetInstanceEnable();

	void SetManualCastDirShadow(bool bEnable);
	bool GetManualCastDirShadow();

	void SetDetailBendingEnable(bool bEnable);
	bool GetDetailBendingEnable();

	virtual void renderVisibleObjectsDefaultSequence(void);

	void SetDeferredLightingQuad(Ogre::Entity* pkEntity);

	void SetDeferredLightingMaterial(const Ogre::MaterialPtr& kMat);

	void SetOutsideRenderingListener(OutsideRenderingListener* pkListener);

	void SetSunMoonLight(Ogre::Light* pkSunLight);

	void SetGlobalLightPower(float fSunMoonLightPower, float fAmbientPower);

	void SetAdaptedQuad(Ogre::Entity* pkEntity);

	void SwapAdaptedQueue();

	void SetIlluminationStage(Ogre::SceneManager::IlluminationRenderStage eStage);

	Ogre::SceneManager::IlluminationRenderStage GetIlluminationStage();

	void SetPointLightEntity(Ogre::Entity* pkPointLight);

	virtual bool IsOnlyShadowCasters();

	void SetOutdoorExtraParams(const FvVector4& kParams);

	const FvVector4& GetOutdoorExtraParams();

	void AddBillboard(FvBillboard* pkBillboard);

	void RemoveBillboard(FvBillboard* pkBillboard);

	FvUInt8 GetCurrentGroupID();

	static FvVector2& GetLodDistance(FvUInt32 u32Level);

	void SetOcclusionMeshMaterial(const Ogre::MeshPtr& spMesh,
		const Ogre::MaterialPtr& spMaterial);

	const Ogre::MeshPtr& GetOcclusionMesh();

	const Ogre::MaterialPtr& GetOcclusionMaterial();

	void AddOcclusionQueryAABB(FvOcclusionQueryAABB* pkQuery);

	static bool IsOCStart();

	virtual void _renderScene(Ogre::Camera* camera, Ogre::Viewport* vp, bool includeOverlays);

	void SetVisitFlag(FvUInt16 uiFlag) { m_uiVisitFlag = uiFlag; }
	FvUInt16 GetVisitFlag() { return m_uiVisitFlag; }

	void SetShadowMap(FvUInt32 u32Level, IDirect3DTexture9* pkTexture);

	void SetShadowReceiverPass(FvUInt32 u32Pass);

	void _RenderSingleObject(Ogre::Renderable* rend, const Ogre::Pass* pass,
		bool lightScissoringClipping, bool doLightIteration,
		const Ogre::LightList* manualLightList = 0);

	float* GetRotMatrixBuffer();

	float* GetShadowMapParams();

	float* GetTerrainShadowMatrix();

	void SetDepthTarget(IDirect3DSurface9* pkTarget);

	IDirect3DSurface9* GetDepthTarget();

	void SetPointLightMaterial(const Ogre::MaterialPtr& kMat);

	void SetSceneRT(const Ogre::TexturePtr& spSceneRT);

	void SetRefractionRT(const Ogre::TexturePtr& spRefractionRT);

	void SetDepthMap(const FvString& kDepthMap);

	void ResetOC();

protected:

	IDirect3DTexture9* m_apkShadowMaps[4];

	virtual bool fireRenderQueueStarted(FvUInt8 uiID, const Ogre::String &kInvocation);
	virtual bool fireRenderQueueEnded(FvUInt8 uiID, const Ogre::String &kInvocation);

	void FindZoneItemLight(FvZone *pkZone, const FvVector3 &kPosition, 
		float fRadius, Ogre::LightList &kDestList);

	FvZone *VisitIndexToZone(FvUInt32 uiIndex);

	typedef std::vector<FvZone*> ZoneList;
	ZoneList m_kOutsideZones;
	ZoneList m_kInsideZones;
	FvUInt8 m_uiCurrentGroupID;

	FvUInt16 m_uiVisitFlag;
	Ogre::VisibleObjectsBoundsInfo *m_pkBoundsInfo;

	std::list<Ogre::Entity*> m_kExtraEntityList;
	
	std::list<FvCamera::VisibleObjectFinder*> m_kVisibleObjectFinders;

	bool m_bDeferredShadingEnable;
	bool m_bDirLightInShellsEnable;
	bool m_bHasOutsideRendering;
	bool m_bHasDirLighting;
	bool m_bHDREnable;
	bool m_bShadowEnable;
	bool m_bInstanceEnable;
	bool m_bManualCastDirShadow;
	bool m_bDetailBendingEnable;

	float m_fSunMoonLightPower;
	float m_fAmbientPower;

	OutsideRenderingListener* m_pkOutsideRenderingListener;
	Ogre::MaterialPtr m_spDeferredLightingMaterial;
	Ogre::MaterialPtr m_spHDRAdaptedMaterial;
	Ogre::MaterialPtr m_spHDRFinalMaterial;
	Ogre::MaterialPtr m_spHDRBrightPassMaterial;
	Ogre::MaterialPtr m_spHDRLumInitial;

	Ogre::Entity* m_pkDeferredLightingQuad;

	Ogre::TexturePtr m_spManualGlobalLighting;

	Ogre::Light* m_pkSunMoonLight;	

	FvUInt32 m_au32AmbientCache[32];
	FvUInt32 m_au32DirectionCache[32];

	Ogre::Entity* m_pkAdaptedQuad;
	FvUInt32 m_u32AdaptedQuadQueue;

	Ogre::Entity* m_pkPointLight;

	FvZoneOmniLight::Params* m_apkPointLight[200];
	FvUInt32 m_u32PointLightNumber;

	FvZoneOmniLight::Params* m_apkGlobalPointLight[10];
	FvUInt32 m_u32GlobalPointLightNumber;

	FvZoneOmniLight::Params* m_apkPointLightSpec[200];
	FvUInt32 m_u32PointLightSpecNumber;

	FvZoneOmniLight::Params* m_apkGlobalPointLightSpec[10];
	FvUInt32 m_u32GlobalPointLightSpecNumber;

	FvVector4 m_kOutdoorExtraParams;

	bool m_bOnlyShadowCasters;

	FvRefList<FvBillboard*, FvRefNode1> m_kBillboardList;

	static FvVector2 ms_kLodDistance[FV_MAX_LOD_LEVEL];
	static bool ms_bIsOCStart;

	Ogre::MeshPtr m_spOcclusionAABBMesh;
	Ogre::MaterialPtr m_spOcclusionAABBMaterial;

	struct OcclusionQueryList
	{
		FvOcclusionQueryAABB* m_apkOcclusions[128];
		FvUInt32 m_u32Number;
	} m_akOcclusionAABBCache[2];

	FvUInt32 m_u32FreeListPoint;

	FvUInt32 m_u32ShadowReceiverPass;

	float m_afShadowPCFParams[16];
	float m_afShadowMapParams[4];

	float m_afTerrainShadowMatrix[16];

	IDirect3DSurface9* m_pkDepthTarget;

	Ogre::MaterialPtr m_spPointLightMaterial;

	Ogre::TexturePtr m_spSceneRT;
	Ogre::TexturePtr m_spRefractionRT;
	FvString m_kDepthMap;
};

class FV_ZONE_API FvRenderManagerFactory : public Ogre::SceneManagerFactory
{
public:
	FvRenderManagerFactory(){}
	~FvRenderManagerFactory(){}

	static const Ogre::String FACTORY_TYPE_NAME;
	Ogre::SceneManager *createInstance(const Ogre::String &kInstanceName);
	void destroyInstance(Ogre::SceneManager *pkInstance);

protected:

	void initMetaData() const;
};

#endif // __FvRenderManager_H__
