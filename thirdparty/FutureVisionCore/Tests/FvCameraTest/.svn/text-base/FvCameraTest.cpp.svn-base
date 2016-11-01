#include <FvZone.h>
#include <FvZoneSpace.h>
#include <FvMatrix.h>
#include <FvZoneManager.h>
#include <FvTerrainLodController.h>
#include <FvZoneSerializerXMLImpl.h>
#include <FvTimeStamp.h>
#include <FvBSPProxy.h>
#include <FvModel.h>
#include <FvZoneLight.h>
#include <FvCursorCamera.h>
#include <FvDirectionCursor.h>

#include <OgreDataStream.h>
#include <OgrePanelOverlayElement.h>
#include <Samples/Common/include/ExampleApplication.h>
#include <RenderSystems/Direct3D9/include/OgreD3D9Plugin.h>

#include <fstream>

#ifdef FV_DEBUG
#include <vld.h>
#endif

#include "FvRenderManager.h"

FvZoneSpacePtr m_spZoneSpace;
FvRenderManagerFactory *ms_pkRenderFactory;
Ogre::Camera *ms_pkCamera;
FvCursorCamera *ms_pkCameraController;
FvSmartPointer<FvDirectionCursor> ms_spDirectionCursor;

extern int iZoneTerrainToken;
extern int iZoneModelToken;
extern int iZoneParticlesToken;
extern int iZoneLightToken;

static int iZoneTokenSet = iZoneTerrainToken | iZoneModelToken | iZoneParticlesToken | iZoneLightToken;

//! 如果Target的生存周期无法与camera相同或者大于, 这样Target需要保证Target消失的时候 Camera.setTarget(NULL)
class Target: public FvTranslateRotateProvider
{
public:
	Target():m_kPostion(0,0,0)
	{
		m_kPostion = FvVector3(50,50,3);
	}

	virtual const FvAngle& Yaw()const{ return m_kYaw;}
	virtual const FvAngle& Pitch()const{ return m_kPitch;}
	virtual const FvAngle& Roll()const{ return m_kRoll;}
	const FvVector3& Translate()const{return m_kPostion;}

	FvVector3 m_kPostion;
	FvAngle m_kYaw;
	FvAngle m_kPitch;
	FvAngle m_kRoll;// 
};



typedef FvSmartPointer<Target> TargetPtr;

TargetPtr ms_spTarget;

class FvZontTestListener : public ExampleFrameListener , public OIS::KeyListener
{
protected:
public:
	FvZontTestListener(RenderWindow* win, Camera* cam)
		: ExampleFrameListener(win, cam,true)
	{
		m_kMatrix = FvMatrix::IDENTITY;
		mMoveSpeed = 15.0f;
		mKeyboard->setEventCallback(this);
	}

	virtual bool keyPressed( const OIS::KeyEvent &arg )
	{
		FvVector3 kPos = ms_spTarget->Translate();
		switch(arg.key)
		{
		case OIS::KC_A:
			kPos.y -= 1;
			break;
		case OIS::KC_D:
			kPos.y += 1;
			break;
		case OIS::KC_W:
			kPos.x -= 1;
			break;		
		case OIS::KC_S:
			kPos.x += 1;
			break;
		case OIS::KC_1:
			ms_pkCameraController->Shake(3.0f,FvVector3::UNIT_X );
			break;
		case OIS::KC_2:
			ms_pkCameraController->Shake(3.0f,FvVector3::UNIT_Y);
			break;
		case OIS::KC_3:
			ms_pkCameraController->Shake(3.0f,FvVector3::UNIT_Z);
			break;
		case OIS::KC_R:
			{
				if(mCamera->getPolygonMode() == PM_WIREFRAME)
					mCamera->setPolygonMode(PM_SOLID);
				else
					mCamera->setPolygonMode(PM_WIREFRAME);
			}
		case OIS::KC_4:
				if(ms_pkCameraController->Collision())
					ms_pkCameraController->Collision(false);
				else
					ms_pkCameraController->Collision(true);
			break;
		}

		ms_spTarget->m_kPostion = kPos;

		//FvUInt32 uiEvent = FvFreeCamera::EVENT_ID_NONE;
		//switch(arg.key)
		//{
		//case OIS::KC_A:
		//	uiEvent = FvFreeCamera::EVENT_ID_RIGHTARROW;
		//	break;
		//case OIS::KC_D:
		//	uiEvent = FvFreeCamera::EVENT_ID_LEFTARROW;
		//	break;
		//case OIS::KC_W:
		//	uiEvent = FvFreeCamera::EVENT_ID_UPARROW;
		//	break;		
		//case OIS::KC_S:
		//	uiEvent = FvFreeCamera::EVENT_ID_DOWNARROW;
		//	break;
		//case OIS::KC_R:
		//	uiEvent = FvFreeCamera::EVENT_ID_PGUP;
		//	break;
		//case OIS::KC_F:
		//	uiEvent = FvFreeCamera::EVENT_ID_PGDN;
		//	break;
		//case OIS::KC_PGUP:
		//	uiEvent = FvFreeCamera::EVENT_ID_VOLUMEUP;
		//	break;
		//case OIS::KC_PGDOWN:
		//	uiEvent = FvFreeCamera::EVENT_ID_VOLUMEDOWN;
		//	break;
		//}

		//ms_pkCameraController->HandleControlEvent(
		//	uiEvent,true);	
		return true;
	}

	virtual bool keyReleased( const OIS::KeyEvent &arg )
	{
		//FvUInt32 uiEvent = FvFreeCamera::EVENT_ID_NONE;
		//switch(arg.key)
		//{
		//case OIS::KC_A:
		//	uiEvent = FvFreeCamera::EVENT_ID_RIGHTARROW;
		//	break;
		//case OIS::KC_D:
		//	uiEvent = FvFreeCamera::EVENT_ID_LEFTARROW;
		//	break;
		//case OIS::KC_W:
		//	uiEvent = FvFreeCamera::EVENT_ID_UPARROW;
		//	break;		
		//case OIS::KC_S:
		//	uiEvent = FvFreeCamera::EVENT_ID_DOWNARROW;
		//	break;
		//case OIS::KC_R:
		//	uiEvent = FvFreeCamera::EVENT_ID_PGUP;
		//	break;
		//case OIS::KC_F:
		//	uiEvent = FvFreeCamera::EVENT_ID_PGDN;
		//	break;
		//case OIS::KC_PGUP:
		//	uiEvent = FvFreeCamera::EVENT_ID_VOLUMEUP;
		//	break;
		//case OIS::KC_PGDOWN:
		//	uiEvent = FvFreeCamera::EVENT_ID_VOLUMEDOWN;
		//	break;
		//}

		//ms_pkCameraController->HandleControlEvent(
		//	uiEvent,false);
		return true;
	}

	virtual bool processUnbufferedKeyInput(const FrameEvent& evt)
	{
		return true;
	}

	virtual bool processUnbufferedMouseInput(const FrameEvent& evt)
	{
		const OIS::MouseState &ms = mMouse->getMouseState();
		if( ms.buttonDown( OIS::MB_Right ) )
			ms_spDirectionCursor->CursorMove(ms.X.rel,ms.Y.rel);

		ms_pkCameraController->MaxDistanceFromPivot(ms_pkCameraController->MaxDistanceFromPivot() + ms.Z.rel);
		return true;
	}

	bool frameRenderingQueued(const FrameEvent& evt)
	{
		if( ExampleFrameListener::frameRenderingQueued(evt) == false )
			return false;

		FvZoneManager::Instance().Tick(evt.timeSinceLastFrame);
		return true;
	}

    bool frameStarted(const FrameEvent& evt) 
	{ 
		Ogre::Vector3 kPos = mCamera->getRealPosition();
		m_kMatrix.SetTranslate(kPos.x, kPos.y, kPos.z);
		FvZoneManager::Instance().Camera(m_kMatrix , m_spZoneSpace);
		ms_pkCameraController->Update(evt.timeSinceLastFrame * 10);
		return true; 
	}

	bool frameEnded(const FrameEvent& evt)
	{
		static Real timeUntilNextToggle = 0;
		static const Real MAX_TIME_INCREMENT = 0.5f;
		if (evt.timeSinceLastEvent > MAX_TIME_INCREMENT)
		{
			return true;
		}

		if (timeUntilNextToggle >= 0) 
			timeUntilNextToggle -= evt.timeSinceLastFrame; 

		return ExampleFrameListener::frameEnded(evt);
	}
	FvMatrix m_kMatrix;
};

class FvZoneTest : public ExampleApplication
{
protected:
	VertexDeclaration* patchDecl;
	float* patchCtlPoints;

public:
	FvZoneTest()
	{ 
		m_pkPlugin = new D3D9Plugin;
	}
	~FvZoneTest()
	{
		ResourceBackgroundQueue::getSingleton().shutdown();
		FvZoneManager::Instance().Fini();
		FvBSPProxyManager::Destory();
	}

protected:

	virtual void createCamera(void)
	{
		// Create the camera
		mCamera = mSceneMgr->createCamera("PlayerCam");
		ms_pkCamera = mCamera;

		Vector3 pos = mCamera->getRealPosition();

		mCamera->setPosition(Vector3(50,50,50));
		mCamera->lookAt(0,0,0);
		mCamera->setNearClipDistance(0.5);
		mCamera->setFarClipDistance(500);

		ms_spDirectionCursor = new FvDirectionCursor;
		ms_spTarget = new Target;
		ms_pkCameraController = new FvCursorCamera((FvCamera*)mCamera);
		ms_pkCameraController->SetSource(ms_spDirectionCursor);
		ms_pkCameraController->SetTarget(ms_spTarget);
		//ms_pkCameraController->Shake(100,FvVector3(0.0,0.0,0.1));
	}

	virtual void chooseSceneManager(void)
	{
		mSceneMgr = mRoot->createSceneManager("FvRenderManager", "FvRenderManager");
		mRoot->_setCurrentSceneManager(mSceneMgr);
		FvModel::SetSceneManager(mSceneMgr);
		FvZoneLight::SetSceneManager(mSceneMgr);
		FvTerrainRenderable::SetSceneManager(mSceneMgr);
		mSceneMgr->setAmbientLight(Ogre::ColourValue(0.0f,0.0f,0.0f,1.0f));
	}

	virtual bool configure(void)
	{
		mRoot->installPlugin(m_pkPlugin);
		mRoot->addSceneManagerFactory(ms_pkRenderFactory);
		mRoot->restoreConfig();
		mWindow = mRoot->initialise(true,"FvZoneTest");
		return true;
	}

	void createScene(void)
	{
		FvBSPProxyManager::Create();
		FvZoneManager::Instance().Init();
		m_spZoneSpace = FvZoneManager::Instance().Space(1);

		FvMatrix* nonConstIdentity = const_cast<FvMatrix*>(&FvMatrix::IDENTITY);
		m_spZoneSpace->AddMapping(1,(float*)nonConstIdentity,"General","spaces/test_2_2/");
		FvMatrix kMat = FvMatrix::IDENTITY;
		FvZoneManager::Instance().Camera(kMat , m_spZoneSpace);
		FvZoneManager::Instance().Tick(0.f);
	}
	void destroyScene(void)
	{
		m_spZoneSpace = NULL;
		FvZoneSerializerXMLImpl::Fini();
	}
	void createFrameListener(void)
	{
		mFrameListener= new FvZontTestListener(mWindow, mCamera);
		mRoot->addFrameListener(mFrameListener);
	}
	Ogre::Light* mSunLight;
	Ogre::D3D9Plugin *m_pkPlugin;
};

void main()
{
	ms_pkRenderFactory = new FvRenderManagerFactory;
	FvZoneManager::Create();
	FvZoneTest *pkApp = new FvZoneTest;
	try 
	{
		pkApp->go();

	} 
	catch(Ogre::Exception& e)
	{
		std::cerr << "An exception has occured:" <<
			e.getFullDescription().c_str() << std::endl;
	}
	delete pkApp;
	FvZoneManager::Destory();
	delete ms_pkRenderFactory;
}
