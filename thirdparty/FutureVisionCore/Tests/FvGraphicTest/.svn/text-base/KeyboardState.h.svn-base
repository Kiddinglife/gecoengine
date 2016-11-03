#ifndef __KeyboardState_H__
#define __KeyboardState_H__

#include <OISKeyboard.h>
#include <FvRefList.h>

class KeyboardState
{
public:
	enum Action
	{
		MOVE_FRONT,
		MOVE_BACK,
		MOVE_LEFT,
		MOVE_RIGHT,
		TURN_LEFT,
		TURN_RIGHT,
		ACTION_TOTAL,
	};

	KeyboardState(void);
	~KeyboardState(void);

	void SetActive(const bool bActive);

	bool OnKeyPressed(const OIS::KeyCode& eKeyCode);
	bool OnKeyReleased(const OIS::KeyCode& eKeyCode);

	void RegisterAction(const FvUInt32 uiKeyIdx, const FvUInt32 uiAction);
	void SetTurnFlag(const bool bTurn);
	bool IsMoveKey(const FvUInt32 uiKeyIdx)const;
	bool IsActive(const Action uiAction)const;
	bool IsAnyActive()const;

	void AddMoveTo();
	void DelMoveTo();
	bool ClearState();
	FvUInt32 GetMoveFlag()const;

private:


	bool _TranslateKey(const FvUInt32 uiKey, FvUInt32& uiAct)const;
	void _ReviveKeys(FvUInt32& uiMoveFlag)const;

	FvUInt32 m_uiMoveFlag;

	FvUInt32 m_ActAddMask[ACTION_TOTAL];
	FvUInt32 m_ActDelMask[ACTION_TOTAL];

	std::map<FvUInt32, FvUInt32> m_KeyActs;

	//! KeyState
	FvRefNode2<FvUInt32> m_KeyMoveFunction[ACTION_TOTAL ];
	FvRefList<FvUInt32> m_PressedMoveKeyList;

	bool m_bActive;
};

#endif //__KeyboardState_H__

