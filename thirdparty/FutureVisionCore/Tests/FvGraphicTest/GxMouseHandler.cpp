#include "GxMouseHandler.h"


GxMouseHandlerInterface::GxMouseHandlerInterface(void)
{}
GxMouseHandlerInterface::~GxMouseHandlerInterface(void)
{}
void GxMouseHandlerInterface::End()
{
	Detach();
}

//+----------------------------------------------------------------------------------------------------
void GxMouseController::AttachFront(GxMouseHandlerInterface& kHandler)
{
	m_kWorkList.AttachFront(kHandler);
}
void GxMouseController::AttachBack(GxMouseHandlerInterface& kHandler)
{
	m_kWorkList.AttachBack(kHandler);
}
GxMouseHandlerInterface* GxMouseController::_GetTop()const
{
	if(m_kWorkList.Size())
	{
		FvRefList<GxMouseHandlerInterface*, FvRefNode1>::iterator iter = m_kWorkList.GetHead();
		FV_ASSERT(iter);
		return static_cast<GxMouseHandlerInterface*>(iter);
	}
	else
		return NULL;
}

void GxMouseController::OnPressed()
{
	GxMouseHandlerInterface* pkHandler = _GetTop();
	if(pkHandler)
		pkHandler->OnPressed();
}
void GxMouseController::OnReleased()
{
	GxMouseHandlerInterface* pkHandler = _GetTop();
	if(pkHandler)
		pkHandler->OnReleased();
}
void GxMouseController::OnMoved(const OIS::MouseState& kEvent)
{
	GxMouseHandlerInterface* pkHandler = _GetTop();
	if(pkHandler)
		pkHandler->OnMoved(kEvent);
}

bool GxMouseController::IsAcceptMove()const
{
	GxMouseHandlerInterface* pkHandler = _GetTop();
	if(pkHandler)
		return pkHandler->IsAcceptMove();
	else
		return false;
}
bool GxMouseController::IsExclusive()const
{
	GxMouseHandlerInterface* pkHandler = _GetTop();
	if(pkHandler)
		return pkHandler->IsExclusive();
	else
		return false;
}
void GxMouseController::Clear()
{
	m_kWorkList.Clear();
}
void GxMouseController::ClearPress()
{
	GxMouseHandlerInterface* pkHandler = _GetTop();
	if(pkHandler)
		return pkHandler->Reset();
}