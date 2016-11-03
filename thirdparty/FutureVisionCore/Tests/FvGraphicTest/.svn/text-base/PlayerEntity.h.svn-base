#pragma once
#include "VisualStruct.h"
#include "FvCliEntity.h"
#include "GxVisualAvatar.h"
#include "AvatarAssisstant.h"
#include "VisualDefine.h"
#include <FvAngleCursor.h>
#include "FvGeographicObject.h"
class AvatarResourceFinishListener : public FvAvatarFinishLoadCallBack
{
public:
	AvatarResourceFinishListener() : m_bComplete(false){}
	void CallBack() { m_bComplete = true; }
	bool IsComplete() { return m_bComplete; }
protected:
	bool m_bComplete;
};
class GameUnitEntity:public FvEntity
	,public FvGeographicObject
{
public:
	GameUnitEntity();
	~GameUnitEntity();
	void Initialize();
	void OnEnterWorld();
	FvAvatarAttachment* pkAvatar;
	ClientGameUnitAvatar m_kGameUnitAvatar;
	void OnAvatarFinishLoad();
	ClientGameUnitAvatar& GameUnitAvatar(){return m_kGameUnitAvatar;}
	void SetAvatar(const FvString& strAvatarRes, const FvString& strAnimShell);
	AvatarFinishLoadCallBack m_kAvatarFinishLoadCallBack;

	static FvRefList<GameUnitEntity*> ms_CheckSurroundList;
	FvRefNode2<GameUnitEntity*> m_kCheckSurroundNode;

	int m_SpaceID;
	FvPhysicsBody* m_MyPhBody;
	//PropertyValueList<GameUnitEntity, ClientAuraVisual, VisualAuraStructObj, VisualAuraStruct> m_AuraVisualList;
	FV_REFERENCE_HEAD;
public:
	float m_spd;
	enum _JumpState
	{
		JUMP_NONE,
		JUMP_1,
		JUMP_2,
	};
	_JumpState m_eJumpState;
	const FvVector3& Position()const
	{
		
		return GetPos();
	}

	MoveType::Idx _GetMoveType(const FvUInt32 uiMoveFlag)const;

	void _SetTangencyType(const GroundWinger::Type eType, const bool bForceNotify = false);
	void SetPos(const FvVector3& kPos);
	bool _OnMoveTo(const FvVector3& kDest);
	bool _OnMoveTo(const FvVector3& kDest, const float fSpd);

	void _UpdateMoveFlag(const FvUInt32 uiMoveFlag);
	void _UpdateMoveFlagWithTurn(const FvUInt32 uiMoveFlag);
	void _UpdateMoveFlagWithMove(const FvUInt32 uiMoveFlag, const float fSpd);

	void Jump(const float fJumpStartTime, const float fLandHeight, const ActDir::Idx eDir = ActDir::DIR_TOTAL);

	void SetMoveScale(const float fScale, bool bMount);
	bool StopMove(const FvUInt32 eMask= 3);
	float GetMovSpd()const			{return m_spd;}
	void OnMove(const FvUInt32 uiMoveFlag, const FvVector3& kFrom, const float fYaw);

	void _OnGetDest(const FvUInt32);
	FvAsynCallback0<FvPhysicsBody, GameUnitEntity, &_OnGetDest> m_kPhysicsGetDestListener;
	
	void _OnMotorEnd(const FvUInt32);
//	FvAsynCallback0<FvPhysicsBody, GameUnitEntity, &_OnMotorEnd> m_kMotorEndListener;
	void _OnLand(const FvUInt32);
	FvAsynCallback0<FvPhysicsBody, GameUnitEntity, &_OnLand> m_kPhysicsLandListener;
	void _OnDrop(const FvUInt32);
//	FvAsynCallback0<FvPhysicsBody, GameUnitEntity, &_OnDrop> m_kPhysicsDropListener;

	void _OnStartSwin(const FvUInt32);
	//FvAsynCallback0<FvPhysicsBody, GameUnitEntity, &_OnStartSwin> m_kStartSwinListener;
	void _OnEndSwin(const FvUInt32);
	//FvAsynCallback0<FvPhysicsBody, GameUnitEntity, &_OnEndSwin> m_kEndSwinListener;
	
	bool GameUnitEntity::IsMoving()const;
	void OnBreakMoveTo(const FvVector3& kPos);
	FvAngleCursor m_kYawCursor;
	FvUInt32 m_uiMoveFlag;
	FvSmartPointer<FvProvider1<FvVector3>> m_spMoveToPos;
	void _UpdateMatrix(const float fDeltaTime);

	FvSmartPointer<FvZoneAttachment> m_Test;
};
class PlayerEntity:public GameUnitEntity
{
public:
	PlayerEntity(void);
	~PlayerEntity(void);
public:
	//MoveToOperator m_kMoveToOperator;
};
