//{future header message}
#ifndef __FvWaterScene_H__
#define __FvWaterScene_H__

#include "FvZoneDefine.h"
#include "FvCamera.h"

#include <FvVector3.h>

#include <OgreSceneManager.h>
#include <OgreRenderTarget.h>
#include <OgreRenderQueueListener.h>
#include <OgreRenderTargetListener.h>

class FvCamera;
class FvWaterScene;

class FV_ZONE_API FvWaterScene : public Ogre::RenderTargetListener,
	public FvCamera::VisibleObjectFinder
{
public:
	FvWaterScene();
	~FvWaterScene();

	static void Init(
		Ogre::PixelFormat eFormat = Ogre::PF_R8G8B8,
		int iNumTex = 1);
	static void Fini();
	static void Render();
	static void SetTextureFormat(Ogre::PixelFormat eFormat);
	static Ogre::Texture *AllocateTexture(FvUInt32 uiIndex,float fHeight);
	static Ogre::Texture *GetTexture(FvUInt32 uiIndex);
	static FvUInt32 GetTextureNumber(){ return ms_kWaterScenes.size(); }

	void preRenderTargetUpdate(const Ogre::RenderTargetEvent &kEvent);
	void postRenderTargetUpdate(const Ogre::RenderTargetEvent &kEvent);

	void FindVisibleObject( 
		Ogre::RenderQueue *pkQueue,
		FvCamera *pkCamera, 
		Ogre::VisibleObjectsBoundsInfo *pkVisibleBounds, 
		bool bOnlyShadowCasters );

	static void SetSkyAndCloud(Ogre::Entity* pkSky, Ogre::Entity* pkCloud);

private:
	void ClearTexture();
	void RecreateTexture();

	FvCamera *m_pkCamera;
	Ogre::TexturePtr m_spTexture;
	FvUInt16 m_uiVisitFlag;
	Ogre::SceneManager::IlluminationRenderStage m_eSavedStage;

	bool m_bEnable;
	Ogre::Plane m_kReflectionPlane;

	static std::vector<FvWaterScene*> ms_kWaterScenes;
	static FvUInt32 ms_uiRenderTargetIdentify;
	static FvCamera *ms_pkCamera;
	static Ogre::PixelFormat ms_eTextureFormat;
	static bool ms_bInit;

	static Ogre::Entity* ms_pkSky;
	static Ogre::Entity* ms_pkCloud;

	friend class FvWaters;
};

#endif // __FvWaterScene_H__