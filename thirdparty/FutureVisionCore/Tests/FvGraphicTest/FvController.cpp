#include "FvController.h"
#include <FvApplication.h>
#include "FvMouseCursor.h"
#include "FvLocalPlayer.h"
#include "FvClientCamera.h"
FvController::FvController(void)
:m_kLocalPlayer(NULL)
{
	
}

FvController::~FvController(void)
{
}
void FvController::Start(PlayerEntity* kLocalPlayer)
{
	m_kLocalPlayer = kLocalPlayer;
	pkCamera = FvClientCamera::Instance().GetCamera();
	FV_ASSERT(pkCamera && "FvClientCamera::Instance().GetCamera() Òì³£");
	pkCursorCamera = new FvCursorCamera(pkCamera);
	pkCursorCamera->Collision(true);
	FvClientCamera::Instance().SetController(pkCursorCamera);

	const FvSmartPointer<FvProvider1<FvDirection3>>& spCameraDirection = FvClientCamera::Instance().DiretionCursor().Direction();
	pkCursorCamera->SetTarget(m_kLocalPlayer->GetTrackPos());
	pkCursorCamera->SetDirection(spCameraDirection);
}
void FvController::OnKeyPressed(const FvUInt32 eKeyCode)
{
	switch(eKeyCode)
	{
	case OIS::KC_SPACE:
		FvUInt32 uiMoveFlag = m_kKeyboardState.GetMoveFlag();
		FvMask::Add(uiMoveFlag, 0X800);
		_SendMoveFlag(uiMoveFlag);
		break;
	}
	//
	if(m_kKeyboardState.IsMoveKey(eKeyCode))
	{
		{
			const FvVector3& kPos = m_kLocalPlayer->GetPos();
			const float fYaw = m_kLocalPlayer->GetDir().Yaw();
			if(m_kKeyboardState.OnKeyPressed(OIS::KeyCode(eKeyCode)))
			{
				m_kKeyboardState.DelMoveTo();
				//m_kMoveToOperator.End(*m_kLocalPlayer, false);
				_SendMoveFlag(m_kKeyboardState.GetMoveFlag());
				//_AddCameraFilter(m_spAccelerateFilter, m_spAccelerateFilter->Weight());
			}
		}
		//!
	}
}
void FvController::OnKeyReleased(const FvUInt32 eKeyCode)
{

	if(m_kKeyboardState.IsMoveKey(eKeyCode))
	{
		const FvVector3& kPos = m_kLocalPlayer->Position();
		const float fYaw = m_kLocalPlayer->Yaw();
		if(m_kKeyboardState.OnKeyReleased(OIS::KeyCode(eKeyCode)))
		{
			m_kKeyboardState.DelMoveTo();
			//m_kMoveToOperator.End(*m_kLocalPlayer, false);
			//if(m_kLocalPlayer.IsMoveLocked() == false)
			{
				_SendMoveFlag(m_kKeyboardState.GetMoveFlag());	
			}
			if(m_kKeyboardState.IsAnyActive() == false)
			{
			//	_DelCameraFilter(m_spAccelerateFilter);
			}
		}
	}
	//
}
void FvController::_SendMoveFlag(const FvUInt32 uiMoveFlag)
{
	const FvVector3& kPos = m_kLocalPlayer->Position();
	FvCamera* pkCamera = FvClientCamera::Instance().GetCamera();
	const float fYaw = k_fAvatarRadianX;
	const double fCurTime = FvGameTimer::LocalAccumulateTime();
	m_kLocalPlayer->OnMove(uiMoveFlag, kPos, fYaw);
}
void FvController::_AddCameraFilter(const FvSmartPointer<FvDirectionFilter>& spFilter, const FvUInt32 uiWeight)
{
//	m_kDirectionFilterList.Add(uiWeight, &*spFilter);
	FvDirectionFilter* pkTopFilter = NULL;
//	if(m_kDirectionFilterList.GetTop(pkTopFilter))
	{
		FV_ASSERT(pkTopFilter);
		FvClientCamera::Instance().DiretionCursor().SetFilter(pkTopFilter);		
	}
}
static bool s_bIsCursorPushed = false;
void FvController::OnLeftMousePressed()
{
	CEGUI::Window *pkWindow = CEGUI::System::getSingleton().getGUISheet();
	if(pkWindow)
	{
		POINT kPos = FvMouseCursor::Instance().Position();
		CEGUI::Window *pkChild = pkWindow->getTargetChildAtPosition(CEGUI::Vector2(kPos.x,kPos.y));
		if(pkChild && 
			pkChild->isVisible() && 
			!pkChild->isMousePassThroughEnabled())
		{
			s_bIsCursorPushed = false;
			return;
		}
	}
	s_bIsCursorPushed = true;
	if(FvMouseCursor::Instance().IsClientRect() == false)
	{
		return;
	}

	//if(m_kRightMouseController.IsExclusive() == false)
	{
		//m_kLeftMouseController.OnPressed();
	}
}
void FvController::OnLeftMouseReleased()
{
	CEGUI::Window *pkWindow = CEGUI::System::getSingleton().getGUISheet();
	if(pkWindow)
	{
		POINT kPos = FvMouseCursor::Instance().Position();
		CEGUI::Window *pkChild = pkWindow->getTargetChildAtPosition(CEGUI::Vector2(kPos.x,kPos.y));
		if(pkChild && 
			pkChild->isVisible() && 
			!pkChild->isMousePassThroughEnabled())
		{
			//m_kLeftMouseController.ClearPress();
			s_bIsCursorPushed = false;
			return;
		}
	}

	if(s_bIsCursorPushed)
	{

		s_bIsCursorPushed = false;
	}

	//m_kLeftMouseController.OnReleased();
}
void FvController::OnRightMousePressed()
{
	FvLogBus::CritOk("SaintController::OnRightMousePressed1");

	//ScriptMouseCursor::Instance().SetItem(ScriptMouseCursor::ITEM_TYPE_NONE);

	CEGUI::Window *pkWindow = CEGUI::System::getSingleton().getGUISheet();
	if(pkWindow)
	{
		POINT kPos = FvMouseCursor::Instance().Position();
		CEGUI::Window *pkChild = pkWindow->getTargetChildAtPosition(CEGUI::Vector2(kPos.x,kPos.y));
		if(pkChild)
			return;
	}
	if(FvMouseCursor::Instance().IsClientRect() == false)
	{
		return;
	}
	FvLogBus::CritOk("SaintController::OnRightMousePressed2");
	//if(m_kLeftMouseController.IsExclusive() == false)
	{
	//	FvLogBus::CritOk("SaintController::OnRightMousePressed3");
	//	m_kRightMouseController.OnPressed();
	}
}
void FvController::_OnRightMoving2(const OIS::MouseState& kMs)
{
	//static FvScriptConstValue<float> S_CameraHorizontalSpdScale("CameraHorizontalSpdScale", 0.015f);
	//static FvScriptConstValue<float> S_CameraVerticalSpdScale("CameraVerticalSpdScale", 0.015f);
	const float fHorizontalSpdScale = 0.007f;//S_CameraHorizontalSpdScale.Value();
	const float fVertiaclSpdScale = 0.007f;//S_CameraVerticalSpdScale.Value();
	FvClientCamera::Instance().DiretionCursor().ModYaw(-kMs.X.rel*fHorizontalSpdScale);
	FvClientCamera::Instance().DiretionCursor().ModPitch(-kMs.Y.rel*fVertiaclSpdScale);

	//const bool bResult = m_kPlayerDirectionOperator.Mod(*m_kLocalPlayer, -kMs.X.rel*fHorizontalSpdScale, -kMs.Y.rel*fVertiaclSpdScale);
	//if( bResult == false)
	{
		//_DelCameraFilter(m_spAttachFilter);
		FvClientCamera::Instance().DiretionCursor().ModYaw(-kMs.X.rel*fHorizontalSpdScale);
		FvClientCamera::Instance().DiretionCursor().ModPitch(-kMs.Y.rel*fVertiaclSpdScale);
	}
}
void FvController::_OnLeftMoving2(const OIS::MouseState& kMs)
{
	//static FvScriptConstValue<float> S_CameraHorizontalSpdScale("CameraHorizontalSpdScale", 0.015f);
	//static FvScriptConstValue<float> S_CameraVerticalSpdScale("CameraVerticalSpdScale", 0.015f);
	FvClientCamera::Instance().DiretionCursor().ModYaw(-kMs.X.rel*0.007f);//S_CameraHorizontalSpdScale.Value());
	FvClientCamera::Instance().DiretionCursor().ModPitch(-kMs.Y.rel*0.007f);//S_CameraVerticalSpdScale.Value());
}
void FvController::OnRightMouseReleased()
{
	FvLogBus::CritOk("SaintController::OnRightMouseReleased1");
	CEGUI::Window *pkWindow = CEGUI::System::getSingleton().getGUISheet();
	if(pkWindow)
	{
		POINT kPos = FvMouseCursor::Instance().Position();
		CEGUI::Window *pkChild = pkWindow->getTargetChildAtPosition(CEGUI::Vector2(kPos.x,kPos.y));
		if(pkChild)
		{
		//	m_kRightMouseController.ClearPress();
			return;
		}
	}
	FvLogBus::CritOk("SaintController::OnRightMouseReleased2");
//	m_kRightMouseController.OnReleased();
}
void FvController::OnMouseMove(const OIS::MouseState& kState)
{
	CEGUI::Window *pkWindow = CEGUI::System::getSingleton().getGUISheet();
	if(pkWindow)
	{
		POINT kPos = FvMouseCursor::Instance().Position();
		CEGUI::Window *pkChild = pkWindow->getTargetChildAtPosition(CEGUI::Vector2(kPos.x,kPos.y));
		if(pkChild)
		{
			//m_kLeftMouseController.ClearPress();
			//m_kRightMouseController.ClearPress();
			return;
		}
	}
	if(kState.Z.rel)
	{
		_AdjustCameraDistancFromPrivot(kState.Z.rel);
	}
}
void FvController::_AdjustCameraDistancFromPrivot(const float deltaValue)
{
	FvCameraControllerPtr pkCameraController = FvClientCamera::Instance().GetController();
	FvCursorCamera* pkCursorCamera = static_cast<FvCursorCamera*>(&*pkCameraController);
	if(pkCameraController)
	{
		//static FvScriptConstValue<float> S_CameraDistanceSpdScale("CameraDistanceSpdScale", 0.015f);
		float fDistance = pkCursorCamera->TargetMaxDistanceFromPivot() + -0.015f * deltaValue;
		if(fDistance <= 1.0f)
			fDistance = 1.0f;
		pkCursorCamera->MaxDistanceFromPivot(fDistance);
	}
}
void FvController::_DelCameraFilter(const FvSmartPointer<FvDirectionFilter>& spFilter)
{
//	m_kDirectionFilterList.Del(&*spFilter);
	FvDirectionFilter* pkTopFilter = NULL;
//	if(m_kDirectionFilterList.GetTop(pkTopFilter))
	{
		FV_ASSERT(pkTopFilter);
		FvClientCamera::Instance().DiretionCursor().SetFilter(pkTopFilter);		
	}
}