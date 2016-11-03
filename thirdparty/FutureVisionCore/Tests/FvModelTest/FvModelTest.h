//{future header message}
#ifndef __FvModelTest_H__
#define __FvModelTest_H__

#include <Ogre.h>
#include <Samples/Common/include/ExampleApplication.h>
#include <RenderSystems/Direct3D9/include/OgreD3D9Plugin.h>

#ifdef FV_DEBUG
#include <vld.h>
#endif

#include <FvModel.h>
#include <FvAvatarResource.h>
#include <FvAvatar.h>
#include <FvAnimationShellResource.h>
#include <FvAnimationShell.h>
#include <FvDebug.h>
#include <FvAnimatedEntity.h>
#include <FvAnimatedCharacter.h>
#include <FvAnimatedCharacterResource.h>

using namespace Ogre;

SceneNode* g_pkCameraNode;

AnimationState* g_pkIdle;
FvAnimatedEntityFactory* m_pkFactory;
FvAnimatedEntity* ent;
FvComponentEntityFactory* m_pkComponentFactory;
FvComponentEntity* torsoEnt;
FvComponentEntity* faceEnt;
FvComponentEntity* hairEnt;

FvAnimatedCharacter* m_kAnimatedCharacter;

class TestNode : public FvAnimatedCharacterNode
{
public:
	virtual bool LoadFinishAll()
	{
		return false;
	}
};

TestNode g_kTorso;

TestNode g_kFace;

FvAnimatedCharacterNode* g_pkHair = NULL;

FvUInt32 g_au32IdlePriority[7] =
{
	20,20,20,20,20,20,20
};

FvUInt32 g_au32WalkPriority[7] =
{
	10,10,10,10,10,10,10
};

FvUInt32 g_au32AttackPriority[7] =
{
	5,15,5,5,5,5,5
};

float g_afWeight[7] =
{
	1,1,1,1,1,1,1
};

FvUInt32 g_u32Partition = 0x7F;

FvUInt32 g_u32Idle;
FvUInt32 g_u32Walk;
FvUInt32 g_u32Attack;

FvUInt32 g_u32Walk_1;
FvUInt32 g_u32Walk_2;

FvUInt32 g_u32Hit;

class TestKeyListener : public FvAnimationState::Callback
{
public:
	virtual void KeyEventCome(FvAnimationState* pkAnimationState,
		const char* pcMessage)
	{
		FV_TRACE_MSG("Animation: %s\tKey: %s\n", pkAnimationState->GetName(), pcMessage);
		//pkAnimationState->SetTimePosition(100);
	}

	//virtual ~TestKeyListener() {}
};

TestKeyListener g_Listener;

class FvModelTestListener : public ExampleFrameListener, public OIS::KeyListener
{
public:
	FvModelTestListener(RenderWindow* win, Camera* cam)
		: ExampleFrameListener(win, cam)
	{
		mMoveSpeed = 10;
		mKeyboard->setEventCallback(this);
		m_bIsQuit = false;
		m_u32Flag = 0;
	}

	bool frameRenderingQueued(const FrameEvent& evt)
	{
		FvBGTaskManager::Instance().Tick();
		
		FvAnimatedCharacterBase::Tick();

		if(m_kAnimatedCharacter)
			m_kAnimatedCharacter->AddTime(evt.timeSinceLastFrame);

		//g_pkIdle->addTime(evt.timeSinceLastFrame);

		if(m_bIsQuit)
			return false;

		return ExampleFrameListener::frameRenderingQueued(evt);
	}

	virtual bool processUnbufferedKeyInput(const FrameEvent& evt)
	{
		bool bRes = ExampleFrameListener::processUnbufferedKeyInput(evt);

		{
			static bool s_bIsPressed = false;

			if(mKeyboard->isKeyDown(OIS::KC_F1))
			{
				if(!s_bIsPressed)
				{
					m_kAnimatedCharacter->Active(!m_kAnimatedCharacter->Active());
					s_bIsPressed = true;
				}
			}
			else
			{
				s_bIsPressed = false;
			}
		}

		{
			static bool s_bIsPressed = false;

			if(mKeyboard->isKeyDown(OIS::KC_F2))
			{
				if(!s_bIsPressed)
				{
					m_kAnimatedCharacter->PlayAnimation(g_u32Walk, FvAnimatedCharacter::INSERT);

					s_bIsPressed = true;
				}
			}
			else
			{
				s_bIsPressed = false;
			}
		}

		{
			static bool s_bIsPressed = false;

			if(mKeyboard->isKeyDown(OIS::KC_F3))
			{
				if(!s_bIsPressed)
				{
					m_kAnimatedCharacter->RemoveLevel("Move");

					s_bIsPressed = true;
				}
			}
			else
			{
				s_bIsPressed = false;
			}
		}

		{
			static bool s_bIsPressed = false;

			if(mKeyboard->isKeyDown(OIS::KC_F4))
			{
				if(!s_bIsPressed)
				{
					m_kAnimatedCharacter->PutOff(g_kTorso);
					m_kAnimatedCharacter->PutOff(g_kFace);
					//m_kAnimatedCharacter->PutOff(g_kHair);

					FV_SAFE_DELETE(g_pkHair);

					s_bIsPressed = true;
				}
			}
			else
			{
				s_bIsPressed = false;
			}
		}

		{
			static bool s_bIsPressed = false;

			if(mKeyboard->isKeyDown(OIS::KC_F5))
			{
				if(!s_bIsPressed)
				{
					m_kAnimatedCharacter->LinkCacheNodes();

					s_bIsPressed = true;
				}
			}
			else
			{
				s_bIsPressed = false;
			}
		}

		{
			static bool s_bIsPressed = false;

			if(mKeyboard->isKeyDown(OIS::KC_F6))
			{
				if(!s_bIsPressed)
				{
					g_pkHair->m_kLinkEntityDatas[0].m_pcNameOfLink = "Back";
					g_pkHair->RefreshLink();

					s_bIsPressed = true;
				}
			}
			else
			{
				s_bIsPressed = false;
			}
		}

		{
			static bool s_bIsPressed = false;

			if(mKeyboard->isKeyDown(OIS::KC_J))
			{
				if(!s_bIsPressed)
				{
					//m_kAnimatedCharacter->PlayAnimation(g_u32Idle, FvAnimatedCharacter::INSERT);
					/*m_kAnimatedCharacter->PlayAnimation("Bounce_Forward", FvAnimatedCharacter::BREAK);
					m_kAnimatedCharacter->PlayAnimation("Jump_F_Air", FvAnimatedCharacter::INSERT);
					m_kAnimatedCharacter->PlayAnimation("Death", FvAnimatedCharacter::BREAK);*/
					//m_kAnimatedCharacter->PlayAnimation("Jump_F_Land", FvAnimatedCharacter::REMOVE);
					//m_kAnimatedCharacter->PlayAnimation("Run", FvAnimatedCharacter::DEFAULT);
					//m_kAnimatedCharacter->PlayAnimation("Turn_R", FvAnimatedCharacter::DEFAULT);
					////m_kAnimatedCharacter->PlayAnimation("Turn_R", FvAnimatedCharacter::REMOVE);
					//m_kAnimatedCharacter->PlayAnimation("Turn_L", FvAnimatedCharacter::DEFAULT);
					////m_kAnimatedCharacter->PlayAnimation("Turn_L", FvAnimatedCharacter::REMOVE);
					//m_kAnimatedCharacter->PlayAnimation("Idle_2", FvAnimatedCharacter::BREAK);

					m_kAnimatedCharacter->PlayAnimation("Death", FvAnimatedCharacter::DEFAULT, 30, false);
					
					s_bIsPressed = true;
				}
			}
			else
			{
				s_bIsPressed = false;
			}
		}

		{
			static bool s_bIsPressed = false;

			if(mKeyboard->isKeyDown(OIS::KC_K))
			{
				if(!s_bIsPressed)
				{
					m_kAnimatedCharacter->PlayAnimation("Run", FvAnimatedCharacter::REMOVE);
					//m_kAnimatedCharacter->RemoveLevel("Default");

					s_bIsPressed = true;
				}
			}
			else
			{
				s_bIsPressed = false;
			}
		}

		{
			static bool s_bIsPressed = false;

			if(mKeyboard->isKeyDown(OIS::KC_L))
			{
				if(!s_bIsPressed)
				{
					//m_kAnimatedCharacter->PlayAnimation(g_u32Idle, FvAnimatedCharacter::REMOVE);
					m_kAnimatedCharacter->PlayAnimation(g_u32Walk, FvAnimatedCharacter::BREAK);

					s_bIsPressed = true;
				}
			}
			else
			{
				s_bIsPressed = false;
			}
		}

		{
			static bool s_bIsPressed = false;

			if(mKeyboard->isKeyDown(OIS::KC_I))
			{
				if(!s_bIsPressed)
				{
					m_kAnimatedCharacter->PlayAnimation(g_u32Walk_1, FvAnimatedCharacter::BREAK);

					s_bIsPressed = true;
				}
			}
			else
			{
				s_bIsPressed = false;
			}
		}

		{
			static bool s_bIsPressed = false;

			if(mKeyboard->isKeyDown(OIS::KC_O))
			{
				if(!s_bIsPressed)
				{
					m_kAnimatedCharacter->PlayAnimation(g_u32Walk_2, FvAnimatedCharacter::BREAK);

					s_bIsPressed = true;
				}
			}
			else
			{
				s_bIsPressed = false;
			}
		}

		return bRes;
	}

	virtual bool keyPressed(const OIS::KeyEvent &arg)
	{		
		return true;
	}

	virtual bool keyReleased(const OIS::KeyEvent &arg)
	{
		return true;
	}

	bool m_bIsQuit;

	FvUInt32 m_u32Flag;
};

class FvModelTest : public ExampleApplication
{
public:
	FvModelTest()
	{ 
		m_pkPlugin = new D3D9Plugin;
	}
	~FvModelTest()
	{

	}

protected:

	virtual void createCamera(void)
	{
		mCamera = mSceneMgr->createCamera("PlayerCam");
		g_pkCameraNode = static_cast<SceneNode*>(mSceneMgr->getRootSceneNode()->createChild());
		g_pkCameraNode->pitch(Degree(90));
		g_pkCameraNode->attachObject(mCamera);
		g_pkCameraNode->setPosition(0,0,0);

		mCamera->setPosition(Vector3(0, 3, 5));	

		mCamera->lookAt(0, 3, 0);
		mCamera->setNearClipDistance(0.5);
		mCamera->setFarClipDistance(400);
	}

	virtual bool configure(void)
	{
		mRoot->installPlugin(m_pkPlugin);
		mRoot->restoreConfig();
		mWindow = mRoot->initialise(true,"FvModelTest");
		return true;
	}

	void createScene(void)
	{
		FvAnimatedCharacterResourceManager::Create("General");
		FvAnimationSetupResourceManager::Create("General");

		m_pkFactory = OGRE_NEW FvAnimatedEntityFactory();
		FV_ASSERT(m_pkFactory);
		Root::getSingleton().addMovableObjectFactory(m_pkFactory);
		m_pkComponentFactory = OGRE_NEW FvComponentEntityFactory();
		FV_ASSERT(m_pkComponentFactory);
		Root::getSingleton().addMovableObjectFactory(m_pkComponentFactory);

		mRoot->_setCurrentSceneManager(mSceneMgr);

		FvBGTaskManager::Instance().StartThreads(1);



		FvAnimatedCharacterResourcePtr spRes = FvAnimatedCharacterResourceManager::getSingleton().load("PC_M_Base01.xml", "General");
		FvAnimationSetupResourcePtr spASRes = FvAnimationSetupResourceManager::getSingleton().load("Human.xml", "General");
		//FvAnimatedCharacterResourcePtr spRes = FvAnimatedCharacterResourceManager::getSingleton().load("Deer.xml", "General");
		
		m_kAnimatedCharacter = new FvAnimatedCharacter(spRes, "Test", spASRes);
		m_kAnimatedCharacter->AttachToNode(((SceneNode*)(mSceneMgr->getRootSceneNode()->createChild())));

		g_u32Idle = m_kAnimatedCharacter->GetAnimation("Idle");
		g_u32Walk = m_kAnimatedCharacter->GetAnimation("Walk");
		g_u32Walk_1 = m_kAnimatedCharacter->GetAnimation("Walk_L");
		g_u32Walk_2 = m_kAnimatedCharacter->GetAnimation("Walk_R");
		g_u32Attack = m_kAnimatedCharacter->GetAnimation("Attack");
		g_u32Hit = m_kAnimatedCharacter->GetAnimation("Hit");

		{
			g_kTorso.m_kPartEntityDatas.resize(1);

			FvAnimatedCharacterNode::PartEntityData& kTorsoData = g_kTorso.m_kPartEntityDatas[0];

			for(FvUInt32 i(0); i < 2; ++i)
			{			
				kTorsoData.m_apcMeshes[i] = "PC_M_Base01_Torso.mesh";
				kTorsoData.m_apcMaterials[i] = "Torso";
				kTorsoData.m_kParts.resize(13);
				kTorsoData.m_u32Priority = 5;

				for(FvUInt32 j(0); j < kTorsoData.m_kParts.size(); ++j)
				{
					kTorsoData.m_kParts[j] = j;
				}
			}

			kTorsoData.m_apcNames[0] = "Body0";
			kTorsoData.m_apcNames[1] = "Body1";
		}

		{
			g_kFace.m_kPartEntityDatas.resize(1);

			FvAnimatedCharacterNode::PartEntityData& kFaceData = g_kFace.m_kPartEntityDatas[0];

			for(FvUInt32 i(0); i < 2; ++i)
			{			
				kFaceData.m_apcMeshes[i] = "PC_M_Base01_Face01.mesh";
				kFaceData.m_apcMaterials[i] = "Face";
				kFaceData.m_kParts.resize(1);
				kFaceData.m_u32Priority = 5;

				kFaceData.m_kParts[0] = 13;
			}

			kFaceData.m_apcNames[0] = "Face0";
			kFaceData.m_apcNames[1] = "Face1";
		}

		g_pkHair = new FvAnimatedCharacterNode();
		FvAnimatedCharacterNode& g_kHair = *g_pkHair;

		{
			g_kHair.m_kPartEntityDatas.resize(1);

			FvAnimatedCharacterNode::PartEntityData& kHairData = g_kHair.m_kPartEntityDatas[0];

			for(FvUInt32 i(0); i < 2; ++i)
			{			
				kHairData.m_apcMeshes[i] = "PC_M_Base01_Hair01.mesh";
				kHairData.m_apcMaterials[i] = "Hair";
				kHairData.m_kParts.resize(1);
				kHairData.m_u32Priority = 5;

				kHairData.m_kParts[0] = 14;
			}

			kHairData.m_apcNames[0] = "Hair0";
			kHairData.m_apcNames[1] = "Hair1";

			g_kHair.m_kLinkEntityDatas.resize(1);

			FvAnimatedCharacterNode::LinkEntityData& kSwordData = g_kHair.m_kLinkEntityDatas[0];

			for(FvUInt32 i(0); i < 2; ++i)
			{			
				kSwordData.m_apcMeshes[i] = "Wep_Sword01.mesh";
				kSwordData.m_apcMaterials[i] = "Wep_Sword01";

				kSwordData.m_pcNameOfLink = "HandR";
			}

			kSwordData.m_apcNames[0] = "Sword0";
			kSwordData.m_apcNames[1] = "Sword1";
		}

		m_kAnimatedCharacter->PutOn(g_kTorso);
		m_kAnimatedCharacter->PutOn(g_kFace);
		m_kAnimatedCharacter->PutOn(g_kHair);

		//m_kAnimatedCharacter->SetScale("Walk", 2.0f);
		//m_kAnimatedCharacter->PutOff(g_kTorso);

		m_kAnimatedCharacter->PlayAnimation("Idle", FvAnimatedCharacter::DEFAULT);
		//m_kAnimatedCharacter->PlayAnimation("Run", FvAnimatedCharacter::DEFAULT);
		m_kAnimatedCharacter->PlayAnimation("Head_L_R", FvAnimatedCharacter::BREAK);
		m_kAnimatedCharacter->PlayAnimation("Body_L_R", FvAnimatedCharacter::BREAK);

		//m_kAnimatedCharacter->EnablePartitionAnimaiton(g_u32Idle, g_au32IdlePriority, true);
		//m_kAnimatedCharacter->EnablePartitionAnimaiton(g_u32Walk, g_au32WalkPriority, true);
		//m_kAnimatedCharacter->EnablePartitionAnimaiton(g_u32Attack, g_au32AttackPriority);
		//m_kAnimatedCharacter->EnableExtraAnimaiton(g_u32Hit, true);

		////m_kAnimatedCharacter->PlayAnimation(g_u32Idle);

		//m_kAnimatedCharacter->GetAnimationState(g_u32Hit)->SetCallback(&g_Listener);
		//m_kAnimatedCharacter->GetAnimationState(g_u32Hit)->EnableBlendMask("UpperBody");
		//m_kAnimatedCharacter->GetAnimationState(g_u32Hit)->SetTimeScale(2);

		//FvAnimatedCharacterResourcePtr spRes = FvAnimatedCharacterResourceManager::getSingleton().load("PC_M_Base01.xml", "General");

		/*NameValuePairList kParams;
		kParams["mesh"] = "PC_M_Base01.mesh";

		ent = (FvAnimatedEntity*)(mSceneMgr->createMovableObject("Root", "AnimatedEntity", &kParams));*/
		//ent->SetShow(false);
		////ent->SetBoundingBoxTargetBone(4, Ogre::Vector3(0.1f,0.1f,0.1f), Ogre::Vector3(-0.1f,-0.1f,-0.1f));
		//
		//kParams["mesh"] = "PC_M_Base01_Torso.mesh";
		//kParams["material"] = "Torso";
		//torsoEnt = (FvComponentEntity*)(mSceneMgr->createMovableObject("Torso", "ComponentEntity", &kParams));
		//torsoEnt->SetParent(ent);
		////torsoEnt->setMaterialName("Torso");
		//torsoEnt->SetDisplayFlags(0xFFFFFFFF);
		//
		//kParams["mesh"] = "PC_M_Base01_Face01.mesh";
		//kParams["material"] = "Face";
		//faceEnt = (FvComponentEntity*)(mSceneMgr->createMovableObject("Face", "ComponentEntity", &kParams));
		//faceEnt->SetParent(ent);
		////faceEnt->setMaterialName("Face");
		//faceEnt->SetDisplayFlags(0xFFFFFFFF);

		//kParams["mesh"] = "PC_M_Base01_Hair01.mesh";
		//kParams["material"] = "Hair";
		//hairEnt = (FvComponentEntity*)(mSceneMgr->createMovableObject("Hair", "ComponentEntity", &kParams));
		//hairEnt->SetParent(ent);
		////hairEnt->setMaterialName("Hair");
		//hairEnt->SetDisplayFlags(0xFFFFFFFF);

		/*g_pkIdle = ent->getAnimationState("Walk");
		g_pkIdle->setLoop(true);
		g_pkIdle->setEnabled(true);
		g_pkIdle->GetTargetAnimation()->UpdateTrackList(spRes->GetBonesPartTable(0));
		g_pkIdle->SetWeightArray(g_afWeight);
		g_pkIdle->SetExtraData(&g_u32Partition);

		((SceneNode*)(mSceneMgr->getRootSceneNode()->createChild()))->attachObject(ent);*/

		

		
	}

	void destroyScene(void)
	{
		FV_SAFE_DELETE(g_pkHair);
		//mSceneMgr->destroyMovableObject(ent);
		/*mSceneMgr->destroyMovableObject(torsoEnt);
		mSceneMgr->destroyMovableObject(faceEnt);
		mSceneMgr->destroyMovableObject(hairEnt);*/

		delete m_kAnimatedCharacter;

		Root* pkRoot = Root::getSingletonPtr();
		if(pkRoot)
		{
			pkRoot->removeMovableObjectFactory(m_pkFactory);
			pkRoot->removeMovableObjectFactory(m_pkComponentFactory);
		}

		OGRE_DELETE m_pkFactory;
		OGRE_DELETE m_pkComponentFactory;

		FvAnimationSetupResourceManager::Destory();
		FvAnimatedCharacterResourceManager::Destory();

		FvBGTaskManager::Instance().StopAll();
		FvBGTaskManager::Fini();
	}

	void createFrameListener(void)
	{
		mFrameListener= new FvModelTestListener(mWindow, mCamera);
		mRoot->addFrameListener(mFrameListener);
	}

	Ogre::D3D9Plugin *m_pkPlugin;
	
	
};

#endif // __FvModelTest_H__
