#ifndef __FvTest_H__
#define __FvTest_H__
#include <FvApplication.h>
#include "FvLocalPlayer.h"
#include "FvClientCamera.h"
//#include "FvEntityPicker.h"
//#include "FvGraphicsCommon.h"
#include "FvUIAddonManager.h"
#include "FvUIAddonFunctions.h"
//#include "FvCliEntityManager.h"
//#include "FvConnectionControl.h"
//#include "FvExpressManagerInstance.h"

#include <FvAvatar.h>
#include <FvAnimatedCharacter.h>
#include <FvBSPProxy.h>
#include <FvTimestamp.h>
//#include <FvGameTimer.h>
#include <FvDebugFilter.h>
#include <FvZoneManager.h>
#include <FvModuleManager.h>
#include <FvBGTaskManager.h>
#include <FvInstanceEntity.h>
//#include <FvDestroyManager.h>
#include <FvTerrainRenderable.h>
//#include <FvFDBStorageManager.h>
#include <FvZoneSerializerXMLImpl.h>
#include <FvAnimationShellResource.h>

#include <CEGUIWindow.h>
#include <CEGUISystem.h>
#include <FvRenderManager.h>
#include <OgreScriptCompiler.h>
#include <CEGUIWindowManager.h>
#include <elements/CEGUIPopupMenu.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>
#include <ParticleRenderers/ParticleUniverseDecalRendererFactory.h>
//#include "vld.h"
#include "FvAvatarAttachment.h"
#include "FvZoneEmbodiment.h"
#include "FvZoneAttachment.h"
#include "AvatarAssisstant.h"
//#include "ScriptItemDefine.h"
#include "ScriptGameData.h"
#include <windows.h>
//#include "FvAnimatedEntity.h"
#include "PlayerEntity.h"
#include "FvController.h"

typedef FvInt32 EntityID;
typedef FvInt64 EntityDBID;
typedef FvUInt64 Time64;
typedef FvUInt16 Time16;
typedef FvUInt32 ItemTemplate;
typedef FvInt16 ItemStack;
typedef FvUInt64 ItemID;
typedef FvUInt32 SpellTemplate;
typedef FvUInt16 SpellEffectIdx;
struct EquipDisplayStruct
{
	ItemTemplate m_u32ItemTemplate;
	FvUInt8 m_u8ItemState;
	ItemID m_u64ItemId;
};
static const FvUInt32 ACCOUNT_EMBODIMENT_NUMBER = 2;

struct AvatarInfo
{
	FvUInt32 m_uiSex;
	FvUInt32 m_uiFace;
	FvUInt32 m_uiHair;
	float m_fBody;
	FvUInt32 m_uiSet;
	std::vector<EquipDisplayStruct> m_kDispList;
};
class TestNode : public FvAnimatedCharacterNode
{
public:
	virtual bool LoadFinishAll()
	{
		return false;
	}
};
class TestApplication : public FvApplication
//	,public ExampleApplication
{
public:
	void  CreateController();
	FvController* m_Controller;


	enum EmbodimentPartType
	{
		EMBODIMENT_PART_TYPE_BODY,
		EMBODIMENT_PART_TYPE_HAIR,
		EMBODIMENT_PART_TYPE_FACE,
		EMBODIMENT_PART_TYPE_SET,
		EMBODIMENT_PART_TYPE_ITEM_BEGIN,
		EMBODIMENT_PART_TYPE_ITEM_END = EMBODIMENT_PART_TYPE_ITEM_BEGIN,
		MAX_EMBODIMENT_PART_TYPE,
	};
	TestApplication();
	~TestApplication();

	bool Initialize(const FvString &kConfigFile = "Config.fvc",
		const FvString &kDataPath = "./Data/",
		const FvString &kCursorFile = "MouseCursors.xml",
		bool bCreate = false, 
		InitializeListener *pkListener = NULL, 
		const char *pcLogFile = NULL);

	void Terminate();
	bool CreateSpace(const char* strAnimShell ,const char* WorkMap);
	bool keyReleased( const FvKeyEvent &kEvent );
	bool keyPressed( const FvKeyEvent &kEvent );
	bool frameStarted(const FvFrameEvent &kEvent);
	bool mouseMoved( const FvMouseEvent &kEvent );
	float Past_Time;
	Ogre::SceneManager *pkSceneManager;
	float r;
	float g;
	float b;
	void PlayAnimation(const char* pcAnimation,const FvAnimatedCharacter::PlayType eType=FvAnimatedCharacter::DEFAULT);
	
	FvZoneEmbodiment* m_pkCurrentEmbodiment;
	void RefreshPlayerAvatar(unsigned int uiSex, unsigned int uiFace, 
		unsigned int uiHair, float fBody, unsigned int uiSet);
	void SetCurrentAvatar(const AvatarInfo& kInfo,
		const FvString& kAnimation = "Idle");
	void _EquipItem(FvUInt32 uiSex, FvUInt32 uiIndex, FvUInt32 uiVisualID);
	FvAnimatedCharacter* m_pkCurrentAvatar;
	float m_fAvatarRadian;
	typedef std::vector<ClientAvatarNode*> Parts;
	Parts m_akEquipItems[ACCOUNT_EMBODIMENT_NUMBER];
	void _UnquipItem(FvUInt32 uiSex, FvUInt32 uiIndex);
	TestApplication::Parts& _SexToEquipItems(FvUInt32 uiSex);
	FvAnimatedCharacter* _SexToAvatar(FvUInt32 uiIndex);

	PlayerEntity m_GameUnit;
	bool mousePressed( const FvMouseEvent &kEvent, FvMouseButtonID eID );
	bool mouseReleased( const FvMouseEvent &kEvent, FvMouseButtonID eID );
	FvAvatarAttachment* pkAttachment;
	FvCamera* m_MoveControl;
	void CreateModleTest();
	bool _GetBottom(const FvVector3& kPos, const float fLen, const float fTestGround, float& fHeight);
	float k_fAvatarRadianX;
	float k_fAvatarRadianY;
protected:
	bool Key[1000];
	bool MouseKey[100];


	bool CreateRendererSystem();
	bool CreateAppWindow();
	bool CreateModuleSystem();
	bool CreateResourceSystem();

	FvRenderManagerFactory *m_pkRenderManagerFactory;


	int m_iSpaceID;
	unsigned int m_uiSpaceHandle;
	FvQuaternion m_kQuaternion;
	Ogre::SceneNode* m_pkCameraNode;
	FvZoneEmbodimentPtr m_aspEmbodiments[ACCOUNT_EMBODIMENT_NUMBER];
	
	bool ms_bEnabledOC ;
	FvUInt32 ms_uiGraphicFlag ;
	HINSTANCE m_hKernelDLL ;
	
	FvZoneAttachment* pkAttachment1[2];
public:
	float MouseSpd;
	Vector3 kPos;
	FvBodyScale m_BodyScale;
	void createScene(void);
	void UpCamera();

	float m_Time;
	bool FreeCamera;
};

static std::vector<FvZoneSpacePtr> s_kClientSpaces;




#endif 