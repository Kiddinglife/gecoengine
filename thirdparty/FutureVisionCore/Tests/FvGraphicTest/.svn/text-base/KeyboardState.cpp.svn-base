#include "KeyboardState.h"

#include "ScriptEntityDefine.h"

#include <FvLogBus.h>

KeyboardState::KeyboardState(void)
:m_uiMoveFlag(0)
,m_bActive(true)
{

	RegisterAction(OIS::KC_W, MOVE_FRONT);
	RegisterAction(OIS::KC_S, MOVE_BACK);
	RegisterAction(OIS::KC_Q, MOVE_LEFT);
	RegisterAction(OIS::KC_E, MOVE_RIGHT);
	RegisterAction(OIS::KC_A, TURN_LEFT);
	RegisterAction(OIS::KC_D, TURN_RIGHT);

	m_KeyMoveFunction[MOVE_FRONT].m_Content = MOVE_FRONT;
	m_KeyMoveFunction[MOVE_BACK].m_Content	= MOVE_BACK;
	m_KeyMoveFunction[MOVE_LEFT].m_Content	= MOVE_LEFT;
	m_KeyMoveFunction[MOVE_RIGHT].m_Content	= MOVE_RIGHT;
	m_KeyMoveFunction[TURN_LEFT].m_Content	= TURN_LEFT;
	m_KeyMoveFunction[TURN_RIGHT].m_Content	= TURN_RIGHT;

	//!
	m_ActAddMask[MOVE_FRONT]	= FvMoveState::MOVING_FRONT;
	m_ActAddMask[MOVE_BACK]	= FvMoveState::MOVING_BACK;
	m_ActAddMask[MOVE_LEFT]	= FvMoveState::MOVING_LEFT;
	m_ActAddMask[MOVE_RIGHT]	= FvMoveState::MOVING_RIGHT;
	m_ActAddMask[TURN_LEFT]		= FvMoveState::TURNING_LEFT;
	m_ActAddMask[TURN_RIGHT]	= FvMoveState::TURNING_RIGHT;
	//
	m_ActDelMask[MOVE_FRONT]	= FvMoveState::MOVING_BACK;
	m_ActDelMask[MOVE_BACK]		= FvMoveState::MOVING_FRONT;
	m_ActDelMask[MOVE_LEFT]		= FvMoveState::MOVING_RIGHT;
	m_ActDelMask[MOVE_RIGHT]	= FvMoveState::MOVING_LEFT;
	m_ActDelMask[TURN_LEFT]		= FvMoveState::TURNING_RIGHT;
	m_ActDelMask[TURN_RIGHT]	= FvMoveState::TURNING_LEFT;
}

KeyboardState::~KeyboardState(void)
{
}

void KeyboardState::SetActive(const bool bActive)
{
	m_bActive = bActive;
}
void KeyboardState::RegisterAction(const FvUInt32 uiKeyIdx, const FvUInt32 uiAction)
{
	if(uiAction < ACTION_TOTAL)
	{
		m_KeyActs[uiKeyIdx] = uiAction;
	}
	else
	{
		FvLogBus::Warning("KeyboardState::RegisterAction Òì³£");
	}
}
bool KeyboardState::IsMoveKey(const FvUInt32 uiKeyIdx)const
{
	std::map<FvUInt32, FvUInt32>::const_iterator iter = m_KeyActs.find(uiKeyIdx);
	return (m_KeyActs.end() != iter);
}
bool KeyboardState::IsActive(const Action uiAction)const
{
	if(uiAction < ACTION_TOTAL)
	{
		return m_KeyMoveFunction[uiAction].IsAttach();
	}
	else
		return false;
}
bool KeyboardState::IsAnyActive()const
{
	return (m_PressedMoveKeyList.Size() != 0);
}
void KeyboardState::SetTurnFlag(const bool bTurn)
{
	if(bTurn)
	{
		m_KeyMoveFunction[TURN_LEFT].m_Content = TURN_LEFT;
		m_KeyMoveFunction[TURN_RIGHT].m_Content = TURN_RIGHT;
	}
	else
	{
		m_KeyMoveFunction[TURN_LEFT].m_Content = MOVE_LEFT;
		m_KeyMoveFunction[TURN_RIGHT].m_Content = MOVE_RIGHT;
	}
	_ReviveKeys(m_uiMoveFlag);
}

//+----------------------------------------------------------------------------------------------------


bool KeyboardState::OnKeyPressed(const OIS::KeyCode& eKeyCode)
{
	if(m_bActive == false)
	{
		return false;
	}
	FvUInt32 uiAct = 0;
	if(_TranslateKey(eKeyCode, uiAct))
	{
		FV_ASSERT(uiAct < ACTION_TOTAL);
		m_PressedMoveKeyList.AttachBack(m_KeyMoveFunction[uiAct]);
		_ReviveKeys(m_uiMoveFlag);
		return true;
	}
	else
		return false;
}
bool KeyboardState::OnKeyReleased(const OIS::KeyCode& eKeyCode)
{
	if(m_bActive == false)
	{
		return false;
	}
	FvUInt32 uiAct = 0;
	if(_TranslateKey(eKeyCode, uiAct))
	{
		FV_ASSERT(uiAct < ACTION_TOTAL);
		m_KeyMoveFunction[uiAct].Detach();
		_ReviveKeys(m_uiMoveFlag);
		return true;
	}
	else
		return false;
}


bool KeyboardState::_TranslateKey(const FvUInt32 uiKey, FvUInt32& uiAct)const
{
	std::map<FvUInt32, FvUInt32>::const_iterator iter = m_KeyActs.find(uiKey);
	if(iter != m_KeyActs.end())
	{
		uiAct = (*iter).second;
		return true;
	}
	else
	{
		uiAct = ACTION_TOTAL;
		return false;
	}
}

void KeyboardState::_ReviveKeys(FvUInt32& uiMoveFlag)const
{
	uiMoveFlag = 0;
	FvRefList<FvUInt32>::iterator iter = m_PressedMoveKeyList.GetHead();
	while (!m_PressedMoveKeyList.IsEnd(iter))
	{
		const FvUInt32 uiAct = (*iter).m_Content;
		FV_ASSERT(uiAct < ACTION_TOTAL);
		FvRefList<FvUInt32>::Next(iter);
		FvMask::Add(uiMoveFlag, m_ActAddMask[uiAct]);
		FvMask::Del(uiMoveFlag, m_ActDelMask[uiAct]);
	}
}

FvUInt32 KeyboardState::GetMoveFlag()const
{
	return m_uiMoveFlag;
}
bool KeyboardState::ClearState()
{
	bool bResult = (m_PressedMoveKeyList.Size() != 0);
	m_PressedMoveKeyList.Clear();
	m_uiMoveFlag = 0;
	return bResult;
}
void KeyboardState::AddMoveTo()
{
	FvMask::Add(m_uiMoveFlag, FvMoveState::MOVING_TO);
}
void KeyboardState::DelMoveTo()
{
	FvMask::Del(m_uiMoveFlag, FvMoveState::MOVING_TO);
}