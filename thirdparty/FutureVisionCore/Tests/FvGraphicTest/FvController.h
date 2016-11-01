#pragma once
#include <FvWeightList.h>
#include "FvCliEntity.h"
#include "FvCursorCamera.h"
#include "PlayerEntity.h"
#include "KeyboardState.h"
#include "GxMouseHandler.h"
class FvController
{
public:
	KeyboardState m_kKeyboardState;
	FvController(void);
	~FvController(void);
	void Start(PlayerEntity* kLocalPlayer);
	FvCamera* pkCamera;
	PlayerEntity* m_kLocalPlayer;
	FvCursorCamera* pkCursorCamera;
	//MoveToOperator m_kMoveToOperator;
	void OnKeyPressed(const FvUInt32 eKeyCode);
	void OnKeyReleased(const FvUInt32 eKeyCode);
	//PlayerDirectionOperator m_kPlayerDirectionOperator;
public:
	void _AddCameraFilter(const FvSmartPointer<FvDirectionFilter>& spFilter, const FvUInt32 uiWeight);
	void _SendMoveFlag(const FvUInt32 uiMoveFlag);
public:
	void _AdjustCameraDistancFromPrivot(const float deltaValue);
	//GxMouseController m_kLeftMouseController;
	//GxMouseController m_kRightMouseController;

	virtual void OnLeftMousePressed();
	virtual void OnLeftMouseReleased();
	virtual void OnRightMousePressed();
	virtual void OnRightMouseReleased();

	virtual void OnMouseMove(const OIS::MouseState& kMouseState);

	void _OnLeftMoving2(const OIS::MouseState& kMs);
	void _OnRightMoving2(const OIS::MouseState& kMs);

	void _DelCameraFilter(const FvSmartPointer<FvDirectionFilter>& spFilter);
	float k_fAvatarRadianX;
public:
	//FvWeightList<FvDirectionFilter*> m_kDirectionFilterList;
	//FvSmartPointer<DirectionFilter_Accelerate> m_spAccelerateFilter;
	//FvSmartPointer<DirectionFilter_Free> m_spFreeFilter;
	//FvSmartPointer<DirectionFilter_Attach> m_spAttachFilter;
};
