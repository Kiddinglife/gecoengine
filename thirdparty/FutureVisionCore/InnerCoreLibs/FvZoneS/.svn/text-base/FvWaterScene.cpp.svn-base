#include "FvWaterScene.h"
#include "FvRenderManager.h"

std::vector<FvWaterScene*> FvWaterScene::ms_kWaterScenes;
FvUInt32 FvWaterScene::ms_uiRenderTargetIdentify = 0;
FvCamera *FvWaterScene::ms_pkCamera = NULL;
Ogre::PixelFormat FvWaterScene::ms_eTextureFormat = Ogre::PF_R8G8B8;
bool FvWaterScene::ms_bInit = false;

Ogre::Entity* FvWaterScene::ms_pkSky = NULL;
Ogre::Entity* FvWaterScene::ms_pkCloud = NULL;

static const int FV_REFLECTION_TEXTURE_WIDTH = 512;
static const int FV_REFLECTION_TEXTURE_HEIGHT = 512;

FvWaterScene::FvWaterScene() :
m_pkCamera(NULL),
m_bEnable(false),
m_kReflectionPlane(Ogre::Vector3::UNIT_Z,0)
{
	this->RecreateTexture();
}

FvWaterScene::~FvWaterScene()
{
	this->ClearTexture();
}

void FvWaterScene::Init(Ogre::PixelFormat eFormat,int iNumTex)
{
	FV_ASSERT(ms_pkCamera);
	if(ms_bInit) return;

	ms_eTextureFormat = eFormat;
	ms_kWaterScenes.reserve(iNumTex);
	for(int i = 0; i < iNumTex; i++)
	{
		FvWaterScene *pkWaterScene = new FvWaterScene;
		ms_kWaterScenes.push_back(pkWaterScene);
	}
	ms_bInit = true;
}

void FvWaterScene::Fini()
{
	if(!ms_bInit) return;

	for(size_t i = 0; i < ms_kWaterScenes.size(); i++)
		delete ms_kWaterScenes[i];

	ms_kWaterScenes.clear();
	ms_bInit = false;
}

void FvWaterScene::Render()
{
	FV_ASSERT(ms_bInit);
	std::vector<FvWaterScene*>::iterator kIt = ms_kWaterScenes.begin();
	for(; kIt != ms_kWaterScenes.end(); ++kIt)
	{
		if(!(*kIt)->m_bEnable) continue;
		FV_ASSERT(!(*kIt)->m_spTexture.isNull());
		(*kIt)->m_pkCamera->setPosition(ms_pkCamera->getRealPosition());
		(*kIt)->m_pkCamera->setOrientation(ms_pkCamera->getRealOrientation());
		(*kIt)->m_spTexture->getBuffer()->getRenderTarget()->update();
		(*kIt)->m_bEnable = false;
	}
}

void FvWaterScene::SetTextureFormat(Ogre::PixelFormat eFormat)
{
	if(ms_eTextureFormat == eFormat || !ms_bInit) return;

	ms_eTextureFormat = eFormat;
	for(size_t i = 0; i < ms_kWaterScenes.size(); i++)
		ms_kWaterScenes[i]->RecreateTexture();
}

Ogre::Texture *FvWaterScene::AllocateTexture(FvUInt32 uiIndex,float fHeight)
{
	FV_ASSERT(ms_bInit);
	FV_ASSERT(uiIndex < ms_kWaterScenes.size())
	FvWaterScene *pkScene = ms_kWaterScenes[uiIndex];
	pkScene->m_kReflectionPlane.d = -fHeight;
	pkScene->m_bEnable = true;
	return pkScene->m_spTexture.get();
}

Ogre::Texture *FvWaterScene::GetTexture(FvUInt32 uiIndex)
{
	FV_ASSERT(ms_bInit);
	FV_ASSERT(uiIndex < ms_kWaterScenes.size())
	return ms_kWaterScenes[uiIndex]->m_spTexture.get();
}

void FvWaterScene::preRenderTargetUpdate(const Ogre::RenderTargetEvent &kEvent)
{
	m_eSavedStage = ((FvRenderManager*)Ogre::Root::getSingleton()._getCurrentSceneManager())->GetIlluminationStage();
	m_uiVisitFlag = ((FvRenderManager*)Ogre::Root::getSingleton()._getCurrentSceneManager())->GetVisitFlag();
	((FvRenderManager*)Ogre::Root::getSingleton()._getCurrentSceneManager())->SetIlluminationStage(
		Ogre::SceneManager::IRS_RENDER_TO_TEXTURE);
	((FvRenderManager*)Ogre::Root::getSingleton()._getCurrentSceneManager())->SetVisitFlag(FvZoneVisitor::VISIT_FLAG_WATER_REFLECTION);
	m_pkCamera->enableReflection(m_kReflectionPlane);
}

void FvWaterScene::postRenderTargetUpdate(const Ogre::RenderTargetEvent &kEvent)
{
	m_pkCamera->disableReflection();
	((FvRenderManager*)Ogre::Root::getSingleton()._getCurrentSceneManager())->SetVisitFlag(m_uiVisitFlag);
	((FvRenderManager*)Ogre::Root::getSingleton()._getCurrentSceneManager())->SetIlluminationStage(m_eSavedStage);
}

void FvWaterScene::FindVisibleObject( 
					   Ogre::RenderQueue *pkQueue,
					   FvCamera *pkCamera, 
					   Ogre::VisibleObjectsBoundsInfo *pkVisibleBounds, 
					   bool bOnlyShadowCasters )
{
	((FvRenderManager*)Ogre::Root::getSingleton()._getCurrentSceneManager())->_FindVisibleTerrains(m_pkCamera);

	if(ms_pkSky) ms_pkSky->_updateRenderQueue(pkQueue);

	if(ms_pkCloud) ms_pkCloud->_updateRenderQueue(pkQueue);
}

void FvWaterScene::ClearTexture()
{
	if(m_spTexture.isNull()) return;
	FV_ASSERT(m_pkCamera);
	Ogre::Root::getSingleton().
		_getCurrentSceneManager()->destroyCamera(m_pkCamera);
	m_pkCamera = NULL;

	m_spTexture->getBuffer()->getRenderTarget()->removeListener(this);
	Ogre::TextureManager::getSingleton().remove(m_spTexture->getHandle());
	m_spTexture.setNull();
}

void FvWaterScene::RecreateTexture()
{
	FV_ASSERT(ms_pkCamera);

	this->ClearTexture();

	ms_uiRenderTargetIdentify++;
	char pcTexName[32];
	char pcCamName[32];
	sprintf_s(pcTexName,32,"FvWaterSceneRTTex_%I32x",FvInt32(ms_uiRenderTargetIdentify));
	sprintf_s(pcCamName,32,"FvWaterSceneCam_%I32x",FvInt32(ms_uiRenderTargetIdentify));
	m_spTexture = Ogre::TextureManager::getSingleton().createManual( pcTexName, 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 
		FV_REFLECTION_TEXTURE_WIDTH, FV_REFLECTION_TEXTURE_HEIGHT, 0, ms_eTextureFormat, Ogre::TU_RENDERTARGET );
	m_pkCamera = static_cast<FvCamera*>(Ogre::Root::getSingleton().
		_getCurrentSceneManager()->createCamera(pcCamName));
	m_pkCamera->setFOVy(ms_pkCamera->getFOVy());
	m_pkCamera->setAspectRatio(ms_pkCamera->getAspectRatio());
	m_pkCamera->setNearClipDistance(ms_pkCamera->getNearClipDistance());
	m_pkCamera->setFarClipDistance(ms_pkCamera->getFarClipDistance());
	m_pkCamera->setLodCamera(ms_pkCamera);
	m_pkCamera->SetVisibleFinder(this);

	Ogre::RenderTarget *pkRenderTarget = m_spTexture->getBuffer()->getRenderTarget();
	pkRenderTarget->setAutoUpdated(false);
	pkRenderTarget->addListener(this);

	Ogre::Viewport *pkViewport = pkRenderTarget->addViewport(m_pkCamera);
	pkViewport->setMaterialScheme("level_2_lighting");
	pkViewport->setClearEveryFrame(true);
	pkViewport->setOverlaysEnabled(false);
	pkViewport->setBackgroundColour(Ogre::ColourValue(0.47f,0.706f,0.937f,1.f));
}

void FvWaterScene::SetSkyAndCloud( Ogre::Entity* pkSky, Ogre::Entity* pkCloud )
{
	ms_pkSky = pkSky;
	ms_pkCloud = pkCloud;
}