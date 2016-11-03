#include "FvZoneTest.h"
#include "TestFun.h"


static const char* ACCOUNT_EMBODIMENT_NAME[ACCOUNT_EMBODIMENT_NUMBER] = 
{
	"Character/Human/Male/PC_M_Base01.avr",
	"Character/Human/Female/PC_F_Base01.avr"
};

class DecalMRTListener : public FvGraphicsCommon::MrtTextureListener
{
public:
	void TickMrtTexture(const FvString& kMrt0, const FvString& kMrt1, const FvString& kMrt2, const FvString& kMrt3)
	{
		ParticleUniverse::DecalSet::setMRTTexture(kMrt2,kMrt3);
		ParticleUniverse::ParticleRenderer::updateDepthTexture(kMrt3);
	}
};
FvGraphicsCommon::MrtTextureListener::MrtTextureListener()
{
}
static DecalMRTListener s_kDecalMRTListener;
TestApplication::TestApplication()
:m_pkRenderManagerFactory(NULL)
{
	m_fAvatarRadian=0.0f;
 k_fAvatarRadianX=0.0f;
	 k_fAvatarRadianY=0.0f;
	m_BodyScale.m_fX=2.0f;
	m_BodyScale.m_fY=2.0f;
	m_BodyScale.m_fZ=2.0f; 
	ms_bEnabledOC = false;
	ms_uiGraphicFlag = 0;
	m_hKernelDLL = NULL;
	m_pkCurrentEmbodiment=NULL;
	//m_pkGameAvatar=NULL;
	r=1.0f;
	g=1.0f;
	b=1.0f;
	Past_Time=0.01f;
	pkSceneManager=NULL;
	m_iSpaceID=-1;
	::memset(Key,false,sizeof(bool)*1000);
	::memset(MouseKey,false,sizeof(bool)*100);
	::memset(m_aspEmbodiments,NULL,sizeof(m_aspEmbodiments));
}


bool TestApplication::CreateSpace(const char* strAnimShell ,const char* WorkMap)
{
	if(m_iSpaceID == -1)
	{
		Test_ClearAllSpaces(true);
		FvCamera* pkCamera = FvClientCamera::Instance().GetCamera();
		pkCamera->setPosition(kPos);
		pkCamera->setOrientation(Ogre::Quaternion(0.0f,1.0f,0.0f));
		
		m_iSpaceID = Test_CreateSpace();
		m_GameUnit.m_SpaceID=m_iSpaceID;
		m_uiSpaceHandle = Test_AddSpaceGeometryMapping(m_iSpaceID,WorkMap);
		
		Test_CameraSpaceID(m_iSpaceID);

		while (Test_SpaceLoadStatus() < 0.99999)
		{
			FvZoneManager::Instance().Tick( 0.05f );
			FvBGTaskManager::Instance().Tick();
		}
	}


	for(int i = 0; i < ACCOUNT_EMBODIMENT_NUMBER; i++)
	{
		if(m_aspEmbodiments[i] == NULL)
		{
			char acTemp[4];
			sprintf_s(acTemp,4,"AE%d",i);
			AvatarResourceFinishListener kAvatarListener;
			
			FvAvatarAttachment* pkAvatar = new FvAvatarAttachment(acTemp,ACCOUNT_EMBODIMENT_NAME[i], strAnimShell);
			m_aspEmbodiments[i] = new FvZoneAttachment(pkAvatar);
			pkAvatar->SetFinishLoadCallBack(&kAvatarListener);

			while(!kAvatarListener.IsComplete())
			{
				FvBGTaskManager::Instance().Tick();
				FvAvatarAttachment::Tick();
				//Ogre::ResourceBackgroundQueue::getSi ngleton()._fireOnFrameCallbacks();
			}
			pkAvatar->EnterWorld();
			m_akEquipItems[i].resize(MAX_EMBODIMENT_PART_TYPE,NULL);
		}

		pkAttachment1[i] = (FvZoneAttachment*)m_aspEmbodiments[i].Get();
		pkAttachment1[i]->SetPosition(FvVector3(kPos.x,kPos.y,kPos.z));
		pkAttachment1[i]->SetQuaternion(m_kQuaternion);
	}
	return true;
}
TestApplication::~TestApplication()
{

}

bool TestApplication::Initialize(const FvString &kConfigFile,
								const FvString &kDataPath,
								const FvString &kCursorFile,
								bool bCreate, 
								InitializeListener *pkListener, 
								const char *pcLogFile)
{
	
	return FvApplication::Initialize(kConfigFile, kDataPath, kCursorFile, bCreate, pkListener, pcLogFile);
}

void TestApplication::Terminate()
{

}

bool TestApplication::CreateAppWindow()
{
	if(!FvApplication::CreateAppWindow())
		return false;

	Ogre::SceneManager *pkSceneManager = m_pkOgreRoot->_getCurrentSceneManager();
	IDirect3DDevice9* pkDevice = 
		((Ogre::D3D9RenderSystem*)(pkSceneManager->getDestinationRenderSystem()))->getDevice();
	FV_ASSERT(pkDevice);

	ms_uiGraphicFlag = FvGraphicsCommon::CheckSuitableGraphicsLevel(
		m_spConfiguration, pkDevice, &ms_bEnabledOC);

	switch(ms_uiGraphicFlag & FV_GRAPHICS_LEVEL_MASK)
	{
	case FV_GRAPHICS_LEVEL_LOW:
	case FV_GRAPHICS_LEVEL_MIDDLE:
		ParticleUniverse::DecalRendererFactory::setType(ParticleUniverse::DecalRendererFactory::TYPE_BILLBOARD);
		break;
	case FV_GRAPHICS_LEVEL_HIGH:
		ParticleUniverse::DecalRendererFactory::setType(ParticleUniverse::DecalRendererFactory::TYPE_DECAL);
		break;
	}
	return true;
}
bool TestApplication::CreateResourceSystem()
{
	if(!FvApplication::CreateResourceSystem())
		return false;

	if(ms_bEnabledOC)
		FvGraphicsCommon::EnableOC((FvRenderManager*)m_pkOgreRoot->_getCurrentSceneManager());

	return true;
}

void TestApplication::createScene(void)
{
}
bool TestApplication::CreateRendererSystem()
{
	if(!FvApplication::CreateRendererSystem())
		return false;

	FV_ASSERT(!m_pkRenderManagerFactory);
	m_pkRenderManagerFactory = new FvRenderManagerFactory;
	m_pkOgreRoot->addSceneManagerFactory(m_pkRenderManagerFactory);
	pkSceneManager = static_cast<FvRenderManager*>(m_pkOgreRoot->
		createSceneManager("FvRenderManager", "FvRenderManager"));
	m_pkOgreRoot->_setCurrentSceneManager(pkSceneManager);

	return true;
}
bool TestApplication::CreateModuleSystem()
{
	m_MoveControl=new FvCamera(FvString("Move"),pkSceneManager);
	m_pkAppWindow->setDeactivateOnFocusChange(false);
	
	FvAnimatedCharacter::Init();
	
	FvBGTaskManager::Instance().StartThreads(1);
	FvInstance::Init();
	FvTerrainRenderable::Init();
	FvBSPProxyManager::Create();
	FvZoneManager::Create();
	FvZoneManager::Instance().Init();
	FvLocalPlayer::Init();
	FvClientCamera::Init();
	FvOcean::Init();
	CEGUI::OgreRenderer *kRenderer = static_cast<CEGUI::OgreRenderer*>(CEGUI::System::getSingleton().getRenderer());
	kRenderer->setRenderingEnabled(true);

	FvGraphicsCommon::SetGraphicsLevel(ms_uiGraphicFlag);
	FvGraphicsCommon::Init();
	
	ParticleUniverse::DecalSet::init();
	
	FvGraphicsCommon::AddListener(&s_kDecalMRTListener);

	ScriptGameData::Create();
	if(!FvApplication::CreateModuleSystem())
		return false;

	return true;
}



void TestApplication::RefreshPlayerAvatar(unsigned int uiSex, unsigned int uiFace, 
						 unsigned int uiHair, float fBody, unsigned int uiSet)
{
	AvatarInfo kAvatar;
	kAvatar.m_uiSex = uiSex;
	kAvatar.m_uiFace = uiFace;
	kAvatar.m_uiHair = uiHair;
	kAvatar.m_fBody = fBody;
	kAvatar.m_uiSet = uiSet;
	SetCurrentAvatar(kAvatar);
}
void TestApplication::_EquipItem(FvUInt32 uiSex, FvUInt32 uiIndex, FvUInt32 uiVisualID)
{
	Parts& kParts = m_akEquipItems[uiSex-1];
	while(uiIndex > (kParts.size() - 1))
	{
		kParts.push_back(NULL);
	}

	FvAnimatedCharacter* pkAvatar = ((FvAvatarAttachment*)((FvZoneAttachment*)m_aspEmbodiments[(uiSex-1)].Get())->GetAttachment())->GetCharacter();

	ClientAvatarNode*& pkPart = kParts[uiIndex];
	ClientAvatarNode* pkOldPart = NULL;
	if(pkPart)
	{
		if(pkPart->Info()->m_iIdx == uiVisualID)
			return;

		pkOldPart = pkPart;
		pkPart = NULL;
	}

	const VisualItem* pkItemVisual = ScriptGameData::GetData<VisualItem>(uiVisualID);
	if(pkItemVisual)
	{
		pkPart = new ClientAvatarNode();
		pkPart->Load(*pkItemVisual, *pkAvatar, uiSex);
		pkPart->AttachOldNode(pkOldPart);
		pkAvatar->PutOn(*pkPart);
	}
}
TestApplication::Parts& TestApplication::_SexToEquipItems(FvUInt32 uiSex)
{
	return  m_akEquipItems[uiSex-1];;
}
FvAnimatedCharacter* TestApplication::_SexToAvatar(FvUInt32 uiIndex)
{
	FV_ASSERT(uiIndex < ACCOUNT_EMBODIMENT_NUMBER);
	return ((FvAvatarAttachment*)((FvZoneAttachment*)m_aspEmbodiments[uiIndex].Get())->GetAttachment())->GetCharacter();
}
void TestApplication::_UnquipItem(FvUInt32 uiSex, FvUInt32 uiIndex)
{
	FV_ASSERT(uiIndex < MAX_EMBODIMENT_PART_TYPE);
	Parts& kParts = _SexToEquipItems(uiSex);
	if(uiIndex > (kParts.size() - 1))
		return;

	ClientAvatarNode*& pkPart = kParts[uiIndex];
	if(!pkPart)
		return;

	FvAnimatedCharacter* pkAvatar = _SexToAvatar(uiSex);
	pkAvatar->PutOff(*pkPart);
	delete pkPart;
	pkPart = NULL;
}
bool TestApplication::_GetBottom(const FvVector3& kPos, const float fLen, const float fTestGround, float& fHeight)
{
	FvVector3 kAcc(0,0,0);
	FvVector3 m_kNormal(0.0f,0.0f,0.0f);
	Test_CalculateSlide(m_kNormal, 7.0f, kAcc);


	
	FV_ASSERT_WARNING(fLen <= 0.0f);
	bool bHitCollide = false;
	bool bGroundCollide = false;
	FvVector3 kNewPos = kPos;
	const float bottomHold = m_BodyScale.m_fZ*0.2f;
	kNewPos.z += bottomHold;

	FvZone * space = FvZoneManager::Instance().CameraZone();
	if(space && space->Space())
	{
		float fCanDropLen = fLen - bottomHold - fTestGround;
		const FvUInt16 uiIgnoreMask = 0;
		bHitCollide = Test_VerticalCollision(*space->Space(), kNewPos, fCanDropLen, m_BodyScale.m_fX*0.5f, m_kNormal, uiIgnoreMask);
		//if(bHitCollide == false)
		//{
		//	m_kNormal = FvVector3::UNIT_Z;
		//}
		m_kNormal.Normalise();
		fHeight = kNewPos.z + fCanDropLen;
	}
	return bHitCollide;
}
void TestApplication::SetCurrentAvatar(const AvatarInfo& kInfo, const FvString& kAnimation)
{
	FvZoneEmbodimentPtr& spEmbodiment = m_aspEmbodiments[kInfo.m_uiSex-1];
	if(spEmbodiment == NULL)
		return;

	if(m_pkCurrentEmbodiment != spEmbodiment.Get())
	{
		if(m_pkCurrentEmbodiment)
		{
			m_pkCurrentEmbodiment->LeaveSpace();
			m_pkCurrentAvatar = NULL;
			m_pkCurrentEmbodiment = NULL;
		}

		pkAttachment = ((FvAvatarAttachment*)((FvZoneAttachment*)m_aspEmbodiments[kInfo.m_uiSex-1].Get())->GetAttachment());
		m_pkCurrentAvatar = pkAttachment->GetCharacter();
		m_pkCurrentEmbodiment = spEmbodiment.Get();
		m_fAvatarRadian = 0.f;
		pkAttachment->SetPosition(FvVector3(20,30 , 30));
		m_MoveControl->setPosition(Vector3(20,30 , 30));
		FvQuaternion kQuat;
		kQuat.FromAngleAxis(m_fAvatarRadian,FvVector3::UNIT_Z);
		((FvZoneAttachment*)m_pkCurrentEmbodiment)->SetQuaternion(m_kQuaternion * kQuat);

		//if(m_GameUnit.GameUnitAvatar())
		//	m_GameUnit.GameUnitAvatar().Clear();

		FV_ASSERT(FvZoneManager::Instance().CameraSpace());
		spEmbodiment->EnterSpace(FvZoneManager::Instance().CameraSpace());
	}

	FV_ASSERT(kInfo.m_uiSex != -1);
	_EquipItem(kInfo.m_uiSex, EMBODIMENT_PART_TYPE_BODY, 0);

	FV_ASSERT(kInfo.m_uiHair != -1);
	_EquipItem(kInfo.m_uiSex, EMBODIMENT_PART_TYPE_HAIR, 6 + kInfo.m_uiHair);

	FV_ASSERT(kInfo.m_uiFace != -1);
	_EquipItem(kInfo.m_uiSex, EMBODIMENT_PART_TYPE_FACE, 1 + kInfo.m_uiFace);

	FvVector3 kScale(m_BodyScale.m_fX,m_BodyScale.m_fY,m_BodyScale.m_fZ);
	kScale *= FvMathTool::Clamp(kInfo.m_fBody, 0.95f, 1.05f);
	((FvAvatarAttachment*)((FvZoneAttachment*)spEmbodiment.Get())->GetAttachment())->SetScale(kScale);

	if(kInfo.m_uiSet == -1)
		_UnquipItem(kInfo.m_uiSex,EMBODIMENT_PART_TYPE_SET);
	else
		_EquipItem(kInfo.m_uiSex, EMBODIMENT_PART_TYPE_SET, kInfo.m_uiSet);

	Parts kParts = _SexToEquipItems(kInfo.m_uiSex);
	FvUInt32 uiPartItemCount = kParts.size() - EMBODIMENT_PART_TYPE_ITEM_BEGIN;
	FvUInt32 i = 0;
	
}
void TestApplication::PlayAnimation(const char* pcAnimation,const FvAnimatedCharacter::PlayType eType)
{


	if(m_pkCurrentEmbodiment == NULL)
		return;

	FvZoneSpacePtr spSpace = FvZoneManager::Instance().CameraSpace();
	if(spSpace == NULL)
		return;

	m_GameUnit.GameUnitAvatar().Clear();
	//m_GameUnit.GameUnitAvatar().StartRibbonTail();
	m_GameUnit.GameUnitAvatar().SetAvatarAttachment(
		*((FvAvatarAttachment*)((FvZoneAttachment*)m_pkCurrentEmbodiment)->GetAttachment()));
	m_GameUnit.GameUnitAvatar().SetSpace(spSpace);
	
	//spSpace->Focus(FvVector3(0.0f,0.0f,0.0f));
	m_GameUnit.GameUnitAvatar().SetAnim(pcAnimation,eType);
}

void  TestApplication::CreateController()
{
	m_Controller = new FvController();
	m_Controller->Start(&m_GameUnit);
}