#include "FvTerrainTest.h"
#include "Ogre.h"
#include "OgreImage.h"
#include "Samples/Common/include/ExampleApplication.h"
#include "RenderSystems/Direct3D9/include/OgreD3D9Plugin.h"
#include "PlugIns/CgProgramManager/include/OgreCgPlugin.h"
#include "FvTerrainPageSerializer.h"
#include <FvTimeStamp.h>

#ifdef FV_DEBUG
#include <vld.h>
#endif

FvTerrainTestManager *ms_pkTerrainManager;
Ogre::Camera *ms_pkCamera;

class FvTerrainTestListener : public ExampleFrameListener
{
protected:
public:
	FvTerrainTestListener(RenderWindow* win, Camera* cam)
		: ExampleFrameListener(win, cam)
	{
		mMoveSpeed = 50;
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

		float fDist = 1.0f;
		Ray kPickRay = ms_pkCamera->getCameraToViewportRay(0.5, 0.5);
		Vector3 kStartPoint = kPickRay.getOrigin();
		Vector3 kDirection = kPickRay.getDirection();
		Vector3 kEndPoint = kStartPoint + kDirection * 1000.0f;

		FvTestTerrainCallback kCallback;
		FvUInt64 uiStartTime,uiEndTime;
		bool bCollision = false;

		FvTerrainPagePtr spTerrainPage = ms_pkTerrainManager->m_kTerrainPage[5][5];
		if(spTerrainPage)
		{
			uiStartTime = TimeStamp();
			bCollision = spTerrainPage->Collide(
				FvVector3(kStartPoint.x,kStartPoint.y,kStartPoint.z),
				FvVector3(kEndPoint.x,kEndPoint.y,kEndPoint.z),&kCallback);
			uiEndTime = TimeStamp();

			ms_pkTerrainManager->m_pkTriangleRenderable->m_bRenderer = bCollision;
			if(bCollision)
			{
				ms_pkTerrainManager->m_pkTriangleRenderable->m_bRenderer = true;
				ms_pkTerrainManager->m_pkTriangleRenderable->m_kMatrix = Ogre::Matrix4::IDENTITY;
				ms_pkTerrainManager->m_pkTriangleRenderable->SetTriangle(kCallback.m_kTriangle);
			}
		}

		if (mKeyboard->isKeyDown(OIS::KC_SPACE) && timeUntilNextToggle <= 0)
		{
			timeUntilNextToggle = 2;

			ms_pkTerrainManager->m_pkLineRenderable->m_bRenderer = true;
			ms_pkTerrainManager->m_pkLineRenderable->SetLine(FvVector3(kStartPoint.x,kStartPoint.y,kStartPoint.z),
				FvVector3(kEndPoint.x,kEndPoint.y,kEndPoint.z));
			printf_s("START:%f,%f,%f\tEND:%f,%f,%f\n", kStartPoint.x, kStartPoint.y, kStartPoint.z,
				kEndPoint.x,kEndPoint.y,kEndPoint.z);

			std::cout << "===================================" << std::endl;
			std::cout << "collision distance:" << kCallback.m_fValue << std::endl;
			std::cout << "collision time:" <<  uiEndTime - uiStartTime << std::endl;
			std::cout << "StampsPerSecond:" << StampsPerSecond() << std::endl;
			std::cout << "===================================" << std::endl;
		}

		return ExampleFrameListener::frameEnded(evt);
	}
};

FvTerrainTestManagerFactory *ms_pkZoneFactory;

class FvTerrainTest : public ExampleApplication
{
public:
	FvTerrainTest()
	{ 
		m_pkPlugin = new D3D9Plugin;
		m_pkCgPlugin = new CgPlugin;
	}
	~FvTerrainTest()
	{

	}

protected:

	virtual void createCamera(void)
	{
		mCamera = mSceneMgr->createCamera("PlayerCam");
		ms_pkCamera = mCamera;
		mCamera->setPosition(0,0,200);
		mCamera->lookAt(100,100,0);
		mCamera->setNearClipDistance(1);
		mCamera->setFarClipDistance(10000);
	}

	virtual void chooseSceneManager(void)
	{
		mSceneMgr = mRoot->createSceneManager("FvTerrainTestManager", "FvTerrainTestManager");
		FvTerrainRenderable::SetSceneManager(mSceneMgr);
		ms_pkTerrainManager = (FvTerrainTestManager*)mSceneMgr;
	}

	virtual bool configure(void)
	{
		mRoot->installPlugin(m_pkPlugin);
		mRoot->installPlugin(m_pkCgPlugin);
		mRoot->addSceneManagerFactory(ms_pkZoneFactory);
		mRoot->restoreConfig();
		mWindow = mRoot->initialise(true,"FvTerrainTest");
		return true;
	}

	void createScene(void)
	{
		mSceneMgr->setAmbientLight(ColourValue(0.467f, 0.702f, 0.933f));
		mRedLight = mSceneMgr->createLight("Light0");
		mRedLight->setType(Light::LT_SPOTLIGHT);
		mRedLight->setDirection(Vector3(0,0,-1));
		mRedLight->setDiffuseColour(ColourValue(1,0.9,0.443,2));
		mRedLight->setPosition(Vector3(50,50,50));
		mRedLight->setAttenuation(100, 1,0.05,0);
		mRedLight->setSpotlightInnerAngle(Radian(FvCosf(FvDegreeToRadiantf(10))));
		mRedLight->setSpotlightOuterAngle(Radian(FvCosf(FvDegreeToRadiantf(70))));
		mRedLight->setSpotlightFalloff(1);

		Vector3 kDir(0.4, 0.5, -0.3);
		kDir.normalise();
		/*mRedLight->setDirection(kDir);
		mRedLight->setDiffuseColour(ColourValue(1,0.9,0.443,1));
		mRedLight->setPosition(Vector3(50,50,50));
		mRedLight->setAttenuation(100, 1,0.1,0);*/

		Light* Light2 = mSceneMgr->createLight("Light1");
		Light2->setType(Light::LT_DIRECTIONAL);
		Light2->setDirection(kDir);
		Light2->setDiffuseColour(ColourValue(1,0,0,0));

		Light* Light3 = mSceneMgr->createLight("Light2");
		Light3->setType(Light::LT_POINT);
		Light3->setDiffuseColour(ColourValue(0,0.9,0,1));
		Light3->setPosition(Vector3(0,0,50));
		Light3->setAttenuation(100, 1,0.05,0);

		mSceneMgr->setFog(FOG_NONE, ColourValue(0.5, 0.5, 0.5), 0.00318f * 0.00318f);
	}
	void destroyScene(void)
	{

	}
	void createFrameListener(void)
	{
		mFrameListener= new FvTerrainTestListener(mWindow, mCamera);
		mRoot->addFrameListener(mFrameListener);
	}

	Ogre::Light* mRedLight;
	Ogre::CgPlugin *m_pkCgPlugin;
	Ogre::D3D9Plugin *m_pkPlugin;
};

void main()
{
	ms_pkZoneFactory = new FvTerrainTestManagerFactory;
	FvTerrainTest *pkApp = new FvTerrainTest;
	pkApp->go();
	delete pkApp;
	delete ms_pkZoneFactory;
}
