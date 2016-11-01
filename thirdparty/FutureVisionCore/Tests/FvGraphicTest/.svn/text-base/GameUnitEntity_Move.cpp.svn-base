#include "PlayerEntity.h"

#include "VisualDefine.h"

#include <FvLogBus.h>
#include <FvPhysicsBody.h>
#include <FvGameUnitDefine.h>
#include <FvGameTimer.h>
#include <FvAvatar.h>
#include <FvParticleAttachment.h>
#include <FvAvatarAttachment.h>
#include <FvLogicDebug.h>
#include "ScriptEntityDefine.h"

//+----------------------------------------------------------------------------------------------------------------------------------------------------------

ActDir::Idx GetDiretion(const FvUInt32 uiMoveFlag)
{
	if(FvMask::HasAny(uiMoveFlag, FvMoveState::MOVING_TO))
	{
		return ActDir::FRONT;
	}
	if(FvMask::HasAny(uiMoveFlag, FvMoveState::MOVING_FRONT))
	{
		if (FvMask::HasAny(uiMoveFlag, FvMoveState::MOVING_LEFT))
		{
			return ActDir::FRONT_LEFT;
		}
		else if(FvMask::HasAny(uiMoveFlag, FvMoveState::MOVING_RIGHT))
		{
			return ActDir::FRONT_RIGHT;
		}
		else
		{
			return ActDir::FRONT;
		}
	}
	else if(FvMask::HasAny(uiMoveFlag, FvMoveState::MOVING_BACK))
	{
		if (FvMask::HasAny(uiMoveFlag, FvMoveState::MOVING_LEFT))
		{
			return ActDir::BACK_LEFT;
		}
		else if(FvMask::HasAny(uiMoveFlag, FvMoveState::MOVING_RIGHT))
		{
			return ActDir::BACK_RIGHT;
		}
		else
		{
			return ActDir::BACK;
		}
	}
	else if (FvMask::HasAny(uiMoveFlag, FvMoveState::MOVING_LEFT))
	{
		return ActDir::LEFT;
	}
	else if(FvMask::HasAny(uiMoveFlag, FvMoveState::MOVING_RIGHT))
	{
		return ActDir::RIGHT;
	}
	else
	{
		return ActDir::DIR_TOTAL;
	}
}
MoveType::Idx GameUnitEntity::_GetMoveType(const FvUInt32 uiMoveFlag)const
{
	//QY
	//FV_ASSERT(_PhysicsBody());
	//if(_PhysicsBody()->IsSwinning())
	//{
	//	return MoveType::SWINING;
	//}
	//else
	//{
	//	const float fSpd = GetMovSpd();
	//	switch(m_kMoveAnimScaleController.GetState())
	//	{
	//	case GxMoveAnimScale::WALK:
	//		return MoveType::WALKING;
	//		break;
	//	case GxMoveAnimScale::RUN:
	//		return MoveType::RUNING;
	//		break;
	//	}
	//}
	return MoveType::RUNING;
}

void GameUnitEntity::_UpdateMoveFlagWithTurn(const FvUInt32 uiMoveFlag)
{
//	if(HasActionStateMask(_MASK_(ActionState::DIS_TURN)))
//		return;

	if(FvMask::HasAny(uiMoveFlag, FvMoveState::TURNING_LEFT))
	{
		if(!FvMask::HasAny(_PhysicsBody()->GetMask(), FvPhysicsBody::ROTATING))
		{
		//	OnTurnStart();
		}
		_PhysicsBody()->Turn(3.0f);
	}
	else if(FvMask::HasAny(uiMoveFlag, FvMoveState::TURNING_RIGHT))
	{
		if(!FvMask::HasAny(_PhysicsBody()->GetMask(), FvPhysicsBody::ROTATING))
		{
//			OnTurnStart();
		}
		_PhysicsBody()->Turn(-3.0f);
	}
	else
	{
		if(FvMask::HasAny(_PhysicsBody()->GetMask(), FvPhysicsBody::ROTATING))
		{
//			OnTurnEnd();
		}
		_PhysicsBody()->StopTurn();
	}
}
void GameUnitEntity::SetMoveScale(const float fScale, bool bMount)
{
	{
		GameUnitAvatar().BodyAnimController().SetScale("Run", fScale);
		GameUnitAvatar().BodyAnimController().SetScale("Walk", fScale);
	}
}

void GameUnitEntity::_UpdateMoveFlagWithMove(const FvUInt32 uiMoveFlag, const float fSpd)
{
//	if(HasActionStateMask(_MASK_(ActionState::DIS_MOVE)))
//		return;

	static float fMoveYaw[ActDir::DIR_TOTAL] = {0.0f, -3.14f, 3.14f*0.5f, -3.14f*0.5f, 3.14f*0.25f, -3.14f*0.25f, 3.14f*0.75f, -3.14f*0.75f};
	static float fActYaw[ActDir::DIR_TOTAL] = {0.0f, -0.0f, 3.14f*0.5f, -3.14f*0.5f, 3.14f*0.25f, -3.14f*0.25f, -3.14f*0.25f, 3.14f*0.25f};
//	static FvScriptConstValue<float> S_MoveBackScale("MoveBackScale", 0.30f);
	float fSpdBackScale =0.30f;
	static float fSpdScale[ActDir::DIR_TOTAL] = {1.0f, fSpdBackScale, 0.9f, 0.9f, 0.9f, 0.9f, fSpdBackScale, fSpdBackScale};
	//
	const ActDir::Idx eMoveDir = GetDiretion(uiMoveFlag);
	MoveType::Idx eMoveType = _GetMoveType(uiMoveFlag);
	FvUInt32 uiAct = 0;
	//QY//bool bIsMount = IsMount();
	bool bIsMount = false;
	if(Act::GetMoveDirAct(eMoveDir, eMoveType, uiAct))
	{
		FV_ASSERT(eMoveDir < ActDir::DIR_TOTAL);
		//
		bool bUpdated = false;
		if(!FvMask::HasAll(uiMoveFlag, FvMoveState::MOVING_TO))
		{
			_PhysicsBody()->Move(fSpd*fSpdScale[eMoveDir]);
			bUpdated = true;
		}
		else
		{
			FV_ASSERT_ERROR(m_spMoveToPos);
			bUpdated = _PhysicsBody()->MoveTo(m_spMoveToPos, fSpd, 6.0f);
		}
		//
		if(bUpdated)
		{
			_PhysicsBody()->SetMoveYaw(fMoveYaw[eMoveDir]);
			m_kYawCursor.SetRotDirection(fActYaw[eMoveDir]);
			//SetAct(GameUnitAvatar().AnimDict().GetAnim(Act::GetJumpDirAct(m_eLandDir, Act::JUMP_LAND)), FvAnimatedCharacter::REMOVE, bIsMount);
			//SetMoveScale(m_kMoveAnimScaleController.GetAnimScale(), bIsMount);
			//SetAct(GameUnitAvatar().AnimDict().GetAnim(uiAct), FvAnimatedCharacter::DEFAULT, bIsMount);
			//QY//GameUnitAvatar().ActionState().Add(ActLayer::LAYER_MOVE);
		}
	}
	else
	{
		m_kYawCursor.SetRotDirection(0.0f);
		StopMove(FvPhysicsBody::MOVING);
	}
}
bool GameUnitEntity::StopMove(const FvUInt32 eMask)
{
	if(_PhysicsBody()->Stop(FvPhysicsBody::_MoveMask(eMask)))
	{
		//QY//CloseActGroup("Move",IsMount());
		//QY//GameUnitAvatar().ActionState().Del(ActLayer::LAYER_MOVE);
		return true;
	}
	else
		return false;
}
void GameUnitEntity::_UpdateMoveFlag(const FvUInt32 uiMoveFlag)
{	
	_UpdateMoveFlagWithTurn(uiMoveFlag);
	_UpdateMoveFlagWithMove(uiMoveFlag, GetMovSpd());
}
void GameUnitEntity::OnMove(const FvUInt32 uiMoveFlag, const FvVector3& kFrom, const float fYaw)
{
	const FvVector3& pos = _PhysicsBody()->Position();
	const float fDiff= FvMath2DTool::Distance(pos.x, pos.y, kFrom.x, kFrom.y);
	FV_ASSERT(_PhysicsBody() && "_PhysicsBody() 不能为空");

	FvMask::Del(m_uiMoveFlag, FvMoveState::MOVING_DIR_MASK);
	FvMask::Add(m_uiMoveFlag, uiMoveFlag&FvMoveState::MOVING_DIR_MASK);
	FvMask::Del(m_uiMoveFlag, FvMoveState::TURNING_DIR_MASK);
	FvMask::Add(m_uiMoveFlag, uiMoveFlag&FvMoveState::TURNING_DIR_MASK);
	FvMask::Del(m_uiMoveFlag, FvMoveState::JUMPED);
	FvMask::Add(m_uiMoveFlag, uiMoveFlag&FvMoveState::JUMPED);

	if(!FvMask::HasAny(uiMoveFlag, FvMoveState::MOVING_TO))
	{
		FvMask::Del(m_uiMoveFlag, FvMoveState::MOVING_TO);
		StopMove(FvPhysicsBody::MOVING_TO);
	}

	//QY//if(!HasActionStateMask(_MASK_(ActionState::DIS_MOVE)))
	{
		_UpdateMoveFlagWithTurn(m_uiMoveFlag);
		if (_PhysicsBody()->GetState() != FvPhysicsBody::IN_SKY)
		{
			_UpdateMoveFlagWithMove(m_uiMoveFlag, GetMovSpd());
		}
		else 
		{
			const FvVector3& kVel = _PhysicsBody()->Velocity();
			const float fHorizVel = FvMath2DTool::Length2(kVel.x, kVel.y);
			if(fHorizVel < 1.0f)
			{
				//QY//static FvScriptConstValue<float> S_SkySpdPlus("SkySpdPlus", 2.0f);
				_UpdateMoveFlagWithMove(m_uiMoveFlag, 2.0f);
				_PhysicsBody()->UpdateMoveDirection(true);
			}
		}
		if(FvMask::HasAny(uiMoveFlag, FvMoveState::JUMPED) == true)
		{
			//! 更新标记
			Jump(0.3f, 1.0f, GetDiretion(uiMoveFlag));
		}
	}
	//! 纠正朝向和位置
	_PhysicsBody()->SetYaw(fYaw);
	const float fIgnoreDiff = 1.0f;
	if(FvDistance2(GetPos(), kFrom) >= fIgnoreDiff)
	{
		_PhysicsBody()->SetPosition(kFrom.x, kFrom.y);
	}
}	

void GameUnitEntity::Jump(const float fJumpStartTime, const float fLandHeight, const ActDir::Idx eDir /* = Act::DIR_TOTAL */)
{
	FV_ASSERT(_PhysicsBody());
	if(m_eJumpState == JUMP_2)
	{
		return;
	}
	if(_PhysicsBody()->IsSwinning())
	{
		return;
	}
	//m_eLandDir = eDir;
	const float fJumpSpd = (eDir == ActDir::FRONT)? 5.0f: 4.8f;
	bool bIsMount = false;
	if(m_eJumpState == JUMP_NONE)
	{
		m_eJumpState = JUMP_1;
	/*	SetAct(GameUnitAvatar().AnimDict().GetAnim(Act::GetJumpDirAct(eDir, Act::JUMP_START)), FvAnimatedCharacter::BREAK, bIsMount);
		SetAct(GameUnitAvatar().AnimDict().GetAnim(Act::GetJumpDirAct(eDir, Act::JUMP_CONTINUE)), FvAnimatedCharacter::INSERT, bIsMount);
		GameUnitAvatar().ActionState().Add(ActLayer::LAYER_JUMP);*/
		_PhysicsBody()->SetSpeedV(fJumpSpd);
		//_PhysicsBody()->GetCaller().Attach(FvPhysicsEvent::LAND, m_kPhysicsLandListener);
	}
	else if(m_eJumpState == JUMP_1)
	{
		//! 只有下落速度够小的时候才能二段跳
		if(_PhysicsBody()->Velocity().z >= -10.0f)
		{
			m_eJumpState = JUMP_2;
		//	static FvScriptConstValue<FvInt32> S_DisplayEffect_AirHike("DisplayEffect_AirHike", 156);
		/*	DisplayEffect(S_DisplayEffect_AirHike.Value());
			SetAct(GameUnitAvatar().AnimDict().GetAnim(Act::GetJumpDirAct(eDir, Act::JUMP_SECOND)), FvAnimatedCharacter::BREAK, bIsMount);
			SetAct(GameUnitAvatar().AnimDict().GetAnim(Act::GetJumpDirAct(eDir, Act::JUMP_CONTINUE)), FvAnimatedCharacter::INSERT, bIsMount);
			GameUnitAvatar().ActionState().Add(ActLayer::LAYER_JUMP);*/
			_PhysicsBody()->SetSpeedV(fJumpSpd);
		//	_PhysicsBody()->GetCaller().Attach(FvPhysicsEvent::LAND, m_kPhysicsLandListener);

			_UpdateMoveFlagWithMove(m_uiMoveFlag, GetMovSpd());
			_PhysicsBody()->UpdateMoveDirection(true);
		}
	}
}	
//+----------------------------------------------------------------------------------------------------------------------------------------------------------
bool GameUnitEntity::_OnMoveTo(const FvVector3& kDest)
{
	StopMove();
	//QY//if(HasActionStateMask(_MASK_(ActionState::DIS_MOVE)))
	{
	//QY//	return false;
	}
	return _OnMoveTo(kDest, GetMovSpd());
}
bool GameUnitEntity::_OnMoveTo(const FvVector3& kDest, const float fSpd)
{
	FV_ASSERT(_PhysicsBody());
	FV_ASSERT_ERROR(m_spMoveToPos);
	m_spMoveToPos->Detach0();
	m_spMoveToPos->SetValue0(kDest);
	FvMask::Add(m_uiMoveFlag, FvMoveState::MOVING_TO);
	_UpdateMoveFlagWithMove(m_uiMoveFlag, fSpd);
	_PhysicsBody()->UpdateMoveDirection(true);
	//
	if(_PhysicsBody()->IsMoving())
	{
		_PhysicsBody()->GetCaller().Attach(FvPhysicsEvent::GET_DEST, m_kPhysicsGetDestListener);	
		return true;	
	}
	else
	{
		return false;
	}
}
void GameUnitEntity::_OnGetDest(const FvUInt32)
{
	FvMask::Del(m_uiMoveFlag, FvMoveState::MOVING_TO);
	StopMove();
//	m_kPhysicsGetDestListener.Detach();
}
////+----------------------------------------------------------------------------------------------------------------------------------------------------------
//void GameUnitEntity::OnChase(const GameUnitEntity& kTarget, const float fInterv)
//{
//
//}	

//+----------------------------------------------------------------------------------------------------------------------------------------------------------
void GameUnitEntity::SetPos(const FvVector3& kPos)
{
	FvEntity::SetPos(kPos);
	const float fDist = FvGeographicObject::GetDistance(Position(), kPos);
	if(fDist > 10.0f)
	{
		ms_CheckSurroundList.AttachBack(m_kCheckSurroundNode);
	}
}
void GameUnitEntity::_OnMotorEnd(const FvUInt32)
{
//	m_kMotorEndListener.Detach();
	_PhysicsBody()->DetachMotor();
}
//void GameUnitEntity::LockMove(const float fDurTime)
//{
//	FvGameTimer::SetTime(fDurTime, m_kMoveLockTime);
//}
//void GameUnitEntity::UnlockMove()
//{
//	m_kMoveLockTime.Detach();
//}
//bool GameUnitEntity::IsMoveLocked()const
//{
//	return m_kMoveLockTime.IsAttach();
//}
//void GameUnitEntity::_OnMoveLockEndTime(const FvUInt32, FvBaseTimeEventNode&)
//{
//	_UpdateMoveFlag(m_uiMoveFlag);
//}
////+----------------------------------------------------------------------------------------------------------------------------------------------------------
//void GameUnitEntity::_OnJumpStartEnd(const FvUInt32, FvBaseTimeEventNode&)
//{
//	//FvLogBus::CritOk("GameUnitEntity::OnJumpStartEnd");	
//}
bool GameUnitEntity::IsMoving()const
{
	//FV_ASSERT(_PhysicsBody());
	return _PhysicsBody()->IsMoving();
}
void GameUnitEntity::_OnLand(const FvUInt32)
{
	FV_ASSERT(_PhysicsBody());
	_UpdateMoveFlag(m_uiMoveFlag);
	//bool bIsMount = IsMount();
	//if(IsMoving() == false)
	//{
	//	SetAct(GameUnitAvatar().AnimDict().GetAnim(Act::GetJumpDirAct(m_eLandDir, Act::JUMP_START)), FvAnimatedCharacter::REMOVE, bIsMount);
	//	SetAct(GameUnitAvatar().AnimDict().GetAnim(Act::GetJumpDirAct(m_eLandDir, Act::JUMP_LAND)), FvAnimatedCharacter::BREAK, bIsMount);
	//}
	//else
	//{
	//	SetAct(GameUnitAvatar().AnimDict().GetAnim(Act::GetJumpDirAct(m_eLandDir, Act::JUMP_START)), FvAnimatedCharacter::REMOVE, bIsMount);
	//	CloseActGroup("Jump",bIsMount);
	//}
	//GameUnitAvatar().ActionState().Add(ActLayer::LAYER_JUMP, 0.5f);
	m_kPhysicsLandListener.Detach();
	FvMask::Del(m_uiMoveFlag, FvMoveState::JUMPED);
	m_eJumpState = JUMP_NONE;
}

void GameUnitEntity::_OnStartSwin(const FvUInt32)
{
	FvMask::Add(m_uiMoveFlag, FvMoveState::SWAMMING);

	/*if(IsDead())
		return;*/

	bool bIsMount =false;
	FV_ASSERT(_PhysicsBody());
	
	_UpdateMoveFlag(m_uiMoveFlag);
	//m_kPhysicsLandListener.Detach();
	FvMask::Del(m_uiMoveFlag, FvMoveState::JUMPED);
	m_eJumpState = JUMP_NONE;

	//CloseActGroup("Jump",bIsMount);
	//SetAct(GameUnitAvatar().AnimDict().GetAnim(Act::SWIN), FvAnimatedCharacter::DEFAULT, bIsMount);

}
void GameUnitEntity::_OnEndSwin(const FvUInt32)
{
	FvMask::Del(m_uiMoveFlag, FvMoveState::SWAMMING);

//	if(IsDead())
		return;

	_UpdateMoveFlag(m_uiMoveFlag);
//	SetAct(GameUnitAvatar().AnimDict().GetAnim(Act::IDEL), FvAnimatedCharacter::DEFAULT, IsMount());
}
void GameUnitEntity::_OnDrop(const FvUInt32)
{
	FV_ASSERT(_PhysicsBody());
	m_eJumpState = JUMP_1;
	//const ActDir::Idx eDir = GetDiretion(m_uiMoveFlag);
	//m_eLandDir = eDir;
	//SetAct(GameUnitAvatar().AnimDict().GetAnim(Act::GetJumpDirAct(m_eLandDir, Act::JUMP_CONTINUE)), FvAnimatedCharacter::BREAK, IsMount());
	//GameUnitAvatar().ActionState().Add(ActLayer::LAYER_JUMP);
	_PhysicsBody()->GetCaller().Attach(FvPhysicsEvent::LAND, m_kPhysicsLandListener);
}

void GameUnitEntity::OnBreakMoveTo(const FvVector3& kPos)
{
	FV_ASSERT(_PhysicsBody());
//	m_kPhysicsGetDestListener.Detach();
	if(StopMove(FvPhysicsBody::MOVING_TO))
	{
		FvMask::Del(m_uiMoveFlag, FvMoveState::MOVING_TO);
	}
	//! 纠正位置
	const float fIgnoreDiff = 1.0f;
	if(FvDistance2(GetPos(), kPos) >= fIgnoreDiff)
	{
		_PhysicsBody()->SetPosition(kPos.x, kPos.y);
	}
}

//+----------------------------------------------------------------------------------------------------------------------------------------------------
//void GameUnitEntity::StartAimRotate(const FvSmartPointer<FvProvider1<FvVector3>>& spTrackBall, const float fRotSpd /* = 3.0f */)
//{
//	//m_kAimRotate.Start(spTrackBall, fRotSpd);
//}

//void GameUnitEntity::EndAimRotate()
//{
//	//m_kAimRotate.End();
//}
////+----------------------------------------------------------------------------------------------------------------------------------------------------
//
//void GameUnitEntity::OnTurnStart()
//{
//	//m_kRotateCursorFlow.Start();
//}
//void GameUnitEntity::OnTurnEnd()
//{
//	//m_kRotateCursorFlow.Stop();
//	GameUnitAvatar().RotateCursor().ForceAccord();
//	//RotateCursor().SetAllowAngle(0.0f);
//}
////+----------------------------------------------------------------------------------------------------------------------------------------------------
