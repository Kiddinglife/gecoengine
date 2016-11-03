//#include <FvZone.h>
//#include <FvZoneSpace.h>
//#include <FvMatrix.h>
//#include <FvZoneManager.h>
//#include <FvTerrainLodController.h>
//#include <FvZoneSerializerXMLImpl.h>
//#include <FvTimeStamp.h>
//#include <FvBSPProxy.h>
//#include <FvZoneModel.h>
//#include <FvZoneTerrain.h>
//#include <FvZoneParticles.h>
//#include <FvZoneLight.h>
//
//#include <OgreDataStream.h>
//#include <OgrePanelOverlayElement.h>
//#include <Samples/Common/include/ExampleApplication.h>
//#ifdef FV_AS_STATIC_LIBS
//#include <RenderSystems/Direct3D9/include/OgreD3D9Plugin.h>
//#include <ParticleUniversePlugin.h>
//#endif FV_AS_STATIC_LIBS
//
//#include <fstream>
//
//#include <FvAngle.h>
//
////#ifdef FV_DEBUG
////#include <vld.h>
////#endif
//
//#include <FvGpuProgramParams.h>
//
//#include "FvSceneManager.h"
//#include "FvRenderManager.h"
//
////FvSceneManager *ms_pkZoneManager;
//FvZoneSpacePtr m_spZoneSpace;
//SceneNode* m_pkCameraNode;
//FvSceneManagerFactory *ms_pkSceneFactory;
//FvRenderManagerFactory *ms_pkRenderFactory;
//Ogre::Camera *ms_pkCamera;
//
//#ifdef FV_AS_STATIC_LIBS
//extern int iZoneTerrainToken;
//extern int iZoneModelToken;
//extern int iZoneParticlesToken;
//extern int iZoneLightToken;
//
//static int iZoneTokenSet = iZoneTerrainToken | iZoneModelToken | iZoneParticlesToken | iZoneLightToken;
//#endif // FV_AS_STATIC_LIBS
//
//Ogre::Light* g_pkSunLight = NULL;
//
//
//class FvZontTestListener : public ExampleFrameListener
//{
//protected:
//public:
//	FvZontTestListener(RenderWindow* win, Camera* cam)
//		: ExampleFrameListener(win, cam)
//	{
//		m_kMatrix = FvMatrix::IDENTITY;
//		mMoveSpeed = 15.0f;
//	}
//
//	virtual bool processUnbufferedKeyInput(const FrameEvent& evt)
//	{
//		bool bResult = ExampleFrameListener::processUnbufferedKeyInput(evt);
//
//		return bResult;
//	}
//
//	bool frameRenderingQueued(const FrameEvent& evt)
//	{
//		static FvAngle kAngle = 0;
//
//		kAngle += evt.timeSinceLastFrame * FV_MATH_PI_2_F * 0.125f;
//
//		//printf_s("%f\n", kAngle.m_fAngle);
//
//		Vector3 kLightDirection(FvCosf(kAngle.GetAngle()), 0, -FvFabsf(FvSinf(kAngle.GetAngle())));
//		g_pkSunLight->setDirection(kLightDirection);
//
//		float f = 0.5f;
//		f = -atan2f(-kLightDirection.z,
//			-kLightDirection.x );
//		f = float( ( f + ( FV_MATH_PI_F * 1.5 ) ) / ( FV_MATH_PI_F * 2 ) );
//		f = fmodf( f, 1 );
//		f = min( 1.f, max( (f * 2.f ) - 0.5f, 0.f ) );
//		FvGpuProgramParams::SetHorizonShadowParams(Vector4(f, f, 5.0, 5.0));
//		
//
//		if( ExampleFrameListener::frameRenderingQueued(evt) == false )
//			return false;
//
//		FvZoneManager::Instance().Tick(evt.timeSinceLastFrame);
//		return true;
//	}
//
//    bool frameStarted(const FrameEvent& evt) 
//	{ 
//		Ogre::Vector3 kPos = mCamera->getRealPosition();
//		m_kMatrix.SetTranslate(kPos.x, kPos.y, kPos.z);
//		FvZoneManager::Instance().Camera(m_kMatrix , m_spZoneSpace);
//		return true; 
//	}
//
//	bool frameEnded(const FrameEvent& evt)
//	{
//		static Real timeUntilNextToggle = 0;
//		static const Real MAX_TIME_INCREMENT = 0.5f;
//		if (evt.timeSinceLastEvent > MAX_TIME_INCREMENT)
//		{
//			return true;
//		}
//
//		if (timeUntilNextToggle >= 0) 
//			timeUntilNextToggle -= evt.timeSinceLastFrame; 
//
//		float fDist = 1000.0f;
//		Ray kPickRay = ms_pkCamera->getCameraToViewportRay(0.5, 0.5);
//		Vector3 kStartPoint = kPickRay.getOrigin();
//		Vector3 kDirection = kPickRay.getDirection();
//		Vector3 kEndPoint = kStartPoint + kDirection * fDist;
//
//		FvTestZoneCallback kCallback;
//		//FvUInt64 uiStartTime,uiEndTime;
//		bool bCollision = false;
//
//		//if(m_spZoneSpace && ms_pkZoneManager->m_pkTriangleRenderable)
//		//{
//		//	uiStartTime = TimeStamp();
//		//	bCollision = m_spZoneSpace->Collide(
//		//		FvVector3(kStartPoint.x,kStartPoint.y,kStartPoint.z),
//		//		FvVector3(kEndPoint.x,kEndPoint.y,kEndPoint.z),kCallback);
//		//	uiEndTime = TimeStamp();
//
//		//	ms_pkZoneManager->m_pkTriangleRenderable->m_bRenderer = bCollision;
//		//	if(bCollision)
//		//	{
//		//		ms_pkZoneManager->m_pkTriangleRenderable->m_bRenderer = true;
//		//		((FvMatrix*)&ms_pkZoneManager->m_pkTriangleRenderable->m_kMatrix)->Transpose(kCallback.m_kMatrix);
//		//		ms_pkZoneManager->m_pkTriangleRenderable->SetTriangle(kCallback.m_kTriangle);
//		//	}
//		//}
//
//		//if (mKeyboard->isKeyDown(OIS::KC_SPACE) && timeUntilNextToggle <= 0)
//		//{
//		//	timeUntilNextToggle = 2;
//
//		//	ms_pkZoneManager->m_pkLineRenderable->m_bRenderer = true;
//		//	ms_pkZoneManager->m_pkLineRenderable->SetLine(FvVector3(kStartPoint.x,kStartPoint.y,kStartPoint.z),
//		//		FvVector3(kEndPoint.x,kEndPoint.y,kEndPoint.z));
//		//	printf_s("START:%f,%f,%f\tEND:%f,%f,%f\n", kStartPoint.x, kStartPoint.y, kStartPoint.z,
//		//		kEndPoint.x,kEndPoint.y,kEndPoint.z);
//
//		//	std::cout << "===================================" << std::endl;
//		//	std::cout << "collision distance:" << kCallback.m_fDist << std::endl;
//		//	std::cout << "collision time:" <<  uiEndTime - uiStartTime << std::endl;
//		//	std::cout << "StampsPerSecond:" << StampsPerSecond() << std::endl;
//		//	std::cout << "===================================" << std::endl;
//		//}
//
//		return ExampleFrameListener::frameEnded(evt);
//	}
//	FvMatrix m_kMatrix;
//};
//
//class FvZoneTest : public ExampleApplication
//{
//protected:
//	VertexDeclaration* patchDecl;
//	float* patchCtlPoints;
//
//public:
//	FvZoneTest()
//	{ 
//#ifdef FV_AS_STATIC_LIBS
//		m_pkPlugin = new D3D9Plugin;
//#endif // FV_AS_STATIC_LIBS
//	}
//	~FvZoneTest()
//	{
//		ResourceBackgroundQueue::getSingleton().shutdown();
//		FvZoneManager::Instance().Fini();
//		FvBSPProxyManager::Destory();
//	}
//
//protected:
//
//	virtual void createCamera(void)
//	{
//		// Create the camera
//		mCamera = mSceneMgr->createCamera("PlayerCam");
//		ms_pkCamera = mCamera;
//		m_pkCameraNode = static_cast<SceneNode*>(mSceneMgr->getRootSceneNode()->createChild());
//		m_pkCameraNode->pitch(Degree(90));
//		m_pkCameraNode->attachObject(mCamera);
//		m_pkCameraNode->setPosition(0,0,0);
//
//		Vector3 pos = mCamera->getRealPosition();
//		
//		//// Position it at 500 in Z direction
//		mCamera->setPosition(Vector3(0,0,100));
//		//// Look back along -Z
//		mCamera->lookAt(Vector3(0,0,-300));
//		//mCamera->setNearClipDistance(5);
//		//mCamera->setPosition(0,0,100);
//		mCamera->lookAt(0,0,0);
//		mCamera->setNearClipDistance(0.05);
//		mCamera->setFarClipDistance(500);
//
//		
//
//		//const Ogre::Vector3& kPos0 = m_pkCameraNode->getWorldTransforms()
//		//const Ogre::Vector3& kPos1 = mCamera->getWorldTransforms()
//	}
//
//	virtual void chooseSceneManager(void)
//	{
//		mSceneMgr = mRoot->createSceneManager("FvRenderManager", "FvRenderManager");
//		mRoot->_setCurrentSceneManager(mSceneMgr);
//	}
//
//	virtual bool configure(void)
//	{
//		//RenderSystem *pkRenderSystem = OGRE_NEW Ogre::D3D9RenderSystem(NULL);
//		//mRoot->addRenderSystem(pkRenderSystem);
//#ifdef FV_AS_STATIC_LIBS
//		mRoot->installPlugin(m_pkPlugin);
//#else // FV_AS_STATIC_LIBS
//		mRoot->loadPlugin("RenderSystem_Direct3D9");
//#endif // !FV_AS_STATIC_LIBS
//		//mRoot->installPlugin(m_pkParticlePlugin);
//		mRoot->addSceneManagerFactory(ms_pkSceneFactory);
//		mRoot->addSceneManagerFactory(ms_pkRenderFactory);
//		mRoot->restoreConfig();
//		mWindow = mRoot->initialise(true,"FvZoneTest");
//		return true;
//	}
//
//	virtual void createViewports(void)
//	{
//		// Create one viewport, entire window
//		Viewport* vp = mWindow->addViewport(mCamera);
//		vp->setBackgroundColour(ColourValue(0,0,0));
//
//		// Alter the camera aspect ratio to match the viewport
//		mCamera->setAspectRatio(
//			Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
//
//		m_pkViewPort = vp;
//	}
//
//	void createScene(void)
//	{
//		FvTerrainRenderable::Init();
//		FvBSPProxyManager::Create();
//		FvZoneManager::Instance().Init();
//		m_spZoneSpace = FvZoneManager::Instance().Space(1);
//
//		FvMatrix* nonConstIdentity = const_cast<FvMatrix*>(&FvMatrix::IDENTITY);
//		FvZoneSpaceEntryID kEntryID;
//		m_spZoneSpace->AddMapping(kEntryID,(float*)nonConstIdentity,"General","World/Maps/Balkan/");
//		//FvZoneManager::Instance().LoadZoneNow("00000000o",m_spZoneSpace->GetMapping(1));
//		FvMatrix kMat = FvMatrix::IDENTITY;
//		FvZoneManager::Instance().Camera(kMat , m_spZoneSpace);
//		FvZoneManager::Instance().Tick(0.f);
//
//		mSceneMgr->setAmbientLight(ColourValue(0.478f * 0.5f, 0.61f * 0.5f, 0.74f * 0.5f));
//
//		FvGpuProgramParams::SetSurfaceAmbientColor(0, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
//		FvGpuProgramParams::SetSurfaceDiffuseColor(0, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
//
//		mSunLight = mSceneMgr->createLight("SunLight");
//		g_pkSunLight = mSunLight;
//		mSunLight->setType(Light::LT_DIRECTIONAL);
//		mSunLight->setDiffuseColour(ColourValue(0.9647f,0.7647f,0.6157f, 1));
//		mSunLight->setDirection(Vector3(0,0,-1));
//
//		mSceneMgr->setFog(FOG_NONE, ColourValue(0.549f, 0.835, 1.0), 0/*0.00318f * 0.00318f*/);
//
//		FvGpuProgramParams::SetHorizonShadowParams(Vector4(0.5, 0.5, 5.0, 5.0));
//
//		CompositorManager::getSingleton().addCompositor(m_pkViewPort,"BaseRendering_level_0");
//		//CompositorManager::getSingleton().addCompositor(m_pkViewPort,"BaseRendering_level_1");
//		//CompositorManager::getSingleton().addCompositor(m_pkViewPort,"BaseRendering_level_2");
//		//CompositorManager::getSingleton().addCompositor(m_pkViewPort,"DeferredShading");
//		
//		//CompositorManager::getSingleton().setCompositorEnabled(m_pkViewPort,"BaseRendering_level_2",true);
//		//CompositorManager::getSingleton().setCompositorEnabled(m_pkViewPort,"DeferredShading",true);
//
//		CompositorManager::getSingleton().setCompositorEnabled(m_pkViewPort,"BaseRendering_level_0",true);
//
//		/*Entity* pkGlobalLighting = mSceneMgr->createEntity("GlobalLightingEntity", "ScreenQuad.mesh");
//		pkGlobalLighting->setMaterialName("GlobalLighting");
//		pkGlobalLighting->setRenderQueueGroup(19);
//		((FvRenderManager*)mSceneMgr)->AttachExtraEntity(pkGlobalLighting);*/
//	}
//	void destroyScene(void)
//	{
//		m_spZoneSpace = NULL;
//		FvZoneSerializerXMLImpl::Fini();
//		FvTerrainRenderable::Fini();
//	}
//	void createFrameListener(void)
//	{
//		mFrameListener= new FvZontTestListener(mWindow, mCamera);
//		mRoot->addFrameListener(mFrameListener);
//	}
//	Ogre::Light* mSunLight;
//#ifdef FV_AS_STATIC_LIBS
//	Ogre::D3D9Plugin *m_pkPlugin;
//#endif //FV_AS_STATIC_LIBS
//	Ogre::Viewport* m_pkViewPort;
//};
//
//void main()
//{
//	ms_pkSceneFactory = new FvSceneManagerFactory;	
//	ms_pkRenderFactory = new FvRenderManagerFactory;
//	FvZoneManager::Create();
//	FvZoneTest *pkApp = new FvZoneTest;
//	try 
//	{
//		pkApp->go();
//
//	} 
//	catch(Ogre::Exception& e)
//	{
//		std::cerr << "An exception has occured:" <<
//			e.getFullDescription().c_str() << std::endl;
//	}
//	delete pkApp;
//	FvZoneManager::Destory();
//	delete ms_pkRenderFactory;
//	delete ms_pkSceneFactory;
//}
#include <FvApplication.h>

#include <windows.h>

#include "vld.h"

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE hOtherInst, LPSTR pcCmdLine, INT iMode)
{
	FvApplication::SetInstanceReference(hInst);
	FvApplication::SetAcceleratorReference(hOtherInst);
	FvApplication::SetCommandLine(pcCmdLine);

	FvApplication *pkApp = new FvApplication;
	if(pkApp->Initialize("Config.fvc","F:/SaintsPro/Binaries/Data"))
	{
		pkApp->MainLoop();
	}

	pkApp->Terminate();
	delete pkApp;
}