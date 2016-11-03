#include "PlayerEntity.h"
#include "FvLocalPlayer.h"
#include <FvZoneManager.h>
FvRefList<GameUnitEntity*> GameUnitEntity::ms_CheckSurroundList;
GameUnitEntity::GameUnitEntity()
#pragma warning(push)
#pragma warning(disable: 4355)
:m_kPhysicsGetDestListener(*this)
,m_kPhysicsLandListener(*this)
#pragma warning(pop)
,m_uiMoveFlag(0)
//:m_kAvatarFinishLoadCallBack(this)
{
	m_spMoveToPos = new FvProvider1<FvVector3>();
	m_kAvatarFinishLoadCallBack.SetListener(this);
}
GameUnitEntity::~GameUnitEntity()
{

}
void GameUnitEntity::OnAvatarFinishLoad()
{

}
void GameUnitEntity::OnEnterWorld()
{
	m_kCheckSurroundNode.m_Content = this;
	ms_CheckSurroundList.AttachBack(m_kCheckSurroundNode);
	m_kGameUnitAvatar.SetSpace(FvZoneManager::Instance().Space(m_SpaceID));
}
void GameUnitEntity::SetAvatar(const FvString& strAvatarRes, const FvString& strAnimShell)
{
	static char pcName[64+1] = {0};
	sprintf_s(pcName, 64, "GameUnitEntity::SetAvatar%d", ClientAvatarNode::GenKey());
	pkAvatar = new FvAvatarAttachment(pcName, strAvatarRes.c_str(), strAnimShell.c_str());
	AvatarResourceFinishListener kAvatarListener;
	pkAvatar->SetFinishLoadCallBack(&kAvatarListener);
	while(!kAvatarListener.IsComplete())
	{
		FvBGTaskManager::Instance().Tick();
		FvAvatarAttachment::Tick();
	}
	m_Test=new FvZoneAttachment(pkAvatar);
	m_MyPhBody = new FvPhysicsBody();
	FvLocalPlayer::Instance().SetPlayer(this);

	FvEntity::SetPos(FvVector3(20,20,30));
	m_Test->EnterSpace(FvZoneManager::Instance().Space(m_SpaceID),true);
	m_kGameUnitAvatar.SetAvatarAttachment(*pkAvatar);

}
void GameUnitEntity::Initialize()
{
	FvEntity::Initialize();
	FvPhysicsBody* pkPhysics = new FvPhysicsBody();
	pkPhysics->SetPosition(GetPos());
	pkPhysics->SetYaw(GetDir().Yaw());
	m_kYawCursor.SetDirection(GetDir().Yaw());
	_AddPhysics(*pkPhysics);
}
inline void CalculateQuaternionNormal(const float fYaw, FvQuaternion& kQuat)
{
	kQuat.FromAngleAxis(fYaw, FvVector3::UNIT_Z);
}


void GameUnitEntity::_UpdateMatrix(const float fDeltaTime)
{
	FvVector3 kPos = GetPos();
	bool bIsMount = false;

	bool bYawUpdate = m_kYawCursor.Update(fDeltaTime,_PhysicsBody()->GetYaw());
	FvMotor* pkMotor = _PhysicsBody()->GetMotor();

	bool bDirty = false;
	FvQuaternion kQuat;

	{
		FvZoneAttachment* pkAttachment = static_cast<FvZoneAttachment*>(m_spPrimaryEmbodiment.Get());
		if(pkAttachment)
		{
			if(!FvAlmostEqual(pkAttachment->GetPosition(), kPos))
				pkAttachment->SetPosition(kPos);
			CalculateQuaternionNormal(m_kYawCursor.Direction(),kQuat);
			//if( bDirty )
			{
				if(pkAttachment->GetQuaternion() != kQuat)
					pkAttachment->SetQuaternion(kQuat);
			}

		//	static_cast<FvAvatarAttachment*>(pkAttachment->GetAttachment())->TickAnimation(fDeltaTime);
		}
	}
}
PlayerEntity::PlayerEntity(void)
{
}

PlayerEntity::~PlayerEntity(void)
{
}
