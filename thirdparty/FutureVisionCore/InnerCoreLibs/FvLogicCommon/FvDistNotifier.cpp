#include "FvDistNotifier.h"


#include "FvLogicDebug.h"




FvRootRanger::FvRootRanger()
:m_kRootPos(0,0,0)
,m_fRange2(0.0f)
{
	FV_MEM_TRACK;
}
void FvRootRanger::SetRoot(const FvVector3& kPos)
{
	m_kRootPos = kPos;
}
void FvRootRanger::SetRange(const float fRange)
{
	m_fRange2 = (fRange*fRange);
}

bool FvRootRanger::IsIn(const FvVector3& kPos)const
{
	return (m_fRange2 >= FvDistance2(m_kRootPos, kPos));
}
const FvVector3& FvRootRanger::GetRoot()const
{
	return m_kRootPos;
}

//+----------------------------------------------------------------------------------------------------

FvDistNotifierInterface::FvDistNotifierInterface(void)
:m_fRange2(0.0f)
{
	FV_MEM_TRACK;
}

FvDistNotifierInterface::~FvDistNotifierInterface(void)
{
}
void FvDistNotifierInterface::SetTarget(const FvSmartPointer<FvProvider1<FvVector3>>& kTarget0, const FvSmartPointer<FvProvider1<FvVector3>>& kTarget1)
{
	FV_ASSERT_WARNING(kTarget0);
	FV_ASSERT_WARNING(kTarget1);
	m_spTarget0 = kTarget0;
	m_spTarget1 = kTarget1;
}
void FvDistNotifierInterface::SetRange(const float fRange)
{
	m_fRange2 = fRange*fRange;
}
float FvDistNotifierInterface::GetDistance()const
{
	FV_ASSERT_WARNING(m_spTarget0 && m_spTarget1);
	if(m_spTarget0 && m_spTarget1)
	{
		return FvDistance(m_spTarget0->GetData0(), m_spTarget1->GetData0());
	}
	else
		return 0.0f;
}
float FvDistNotifierInterface::GetDistance2()const
{
	FV_ASSERT_WARNING(m_spTarget0 && m_spTarget1);
	if(m_spTarget0 && m_spTarget1)
	{
		return FvDistance2(m_spTarget0->GetData0(), m_spTarget1->GetData0());
	}
	else
		return 0.0f;
}
bool FvDistNotifierInterface::IsIn()const
{
	return (GetDistance2() <= m_fRange2);
}
bool FvDistNotifierInterface::IsOut()const
{
	return (GetDistance2() > m_fRange2);
}
//+----------------------------------------------------------------------------------------------------



FvRefListNameSpace<FvDistNotifier>::List1 FvDistNotifier::s_InList;
FvRefListNameSpace<FvDistNotifier>::List1 FvDistNotifier::s_OutList;


void FvDistNotifier::UpdateTrackDetectors()
{
	s_InList.BeginIterator();
	while (!s_InList.IsEnd())
	{
		FvRefListNameSpace<FvDistNotifier>::List1::iterator iter = s_InList.GetIterator();
		s_InList.Next();
		FvDistNotifier* _this = (*iter).Content();
		FV_ASSERT_ERROR(_this);
		_this->_UpdateInside();
	}
	s_OutList.BeginIterator();
	while (!s_OutList.IsEnd())
	{
		FvRefListNameSpace<FvDistNotifier>::List1::iterator iter = s_OutList.GetIterator();
		s_OutList.Next();
		FvDistNotifier* _this = (*iter).Content();
		FV_ASSERT_ERROR(_this);
		_this->_UpdateOutside();
	}
}

FvDistNotifier::FvDistNotifier(void)
{
}

FvDistNotifier::~FvDistNotifier(void)
{
}
void FvDistNotifier::_UpdateOutside()
{
	FV_ASSERT_ERROR(m_spTarget0 && m_spTarget1);
	const float fDistance2 = FvDistance2(m_spTarget0->GetData0(), m_spTarget1->GetData0());
	if(fDistance2 < m_fRange2 )
	{
		s_InList.AttachBack(*this);
		_OnIn();
	}
}
void FvDistNotifier::_UpdateInside()
{
	FV_ASSERT_ERROR(m_spTarget0 && m_spTarget1);
	const float fDistance2 = FvDistance2(m_spTarget0->GetData0(), m_spTarget1->GetData0());
	if(fDistance2 > m_fRange2 )
	{
		s_OutList.AttachBack(*this);
		_OnOut();
	}
}

bool FvDistNotifier::IsAttach()const
{
	return FvRefNode1::IsAttach();
}
void FvDistNotifier::AttachUpdate()
{
	FV_ASSERT_WARNING(m_spTarget0 && m_spTarget1);
	FV_CHECK_RETURN(m_spTarget0 && m_spTarget1);
	const float distance2 = FvDistance2(m_spTarget0->GetData0(), m_spTarget1->GetData0());
	if(distance2 < m_fRange2 )
	{
		s_InList.AttachBack(*this);
	}
	else
	{
		s_OutList.AttachBack(*this);
	}
}
FvDistNotifier::_State FvDistNotifier::GetState(const FvDistNotifier& kThis)
{
	FV_ASSERT_WARNING(kThis.m_spTarget0 && kThis.m_spTarget1);
	if(kThis.m_spTarget0 && kThis.m_spTarget1)
	{
		return NONE;
	}
	const float fDistance2 = FvDistance2(kThis.m_spTarget0->GetData0(), kThis.m_spTarget1->GetData0());
	if(fDistance2 < kThis.m_fRange2)
	{
		return INSIDE;
	}
	else
	{
		return OUTSIDE;
	}
}
void FvDistNotifier::DetachUpdate()
{
	FvRefNode1::Detach();
}
void FvDistNotifier::Notify()
{
	if(IsAttach())
	{
		_OnIn();
	}
	else
	{
		_OnOut();
	}
}
//+----------------------------------------------------------------------------------------------------

FvRefListNameSpace<FvDistOutNotifier>::List1 FvDistOutNotifier::s_List;

void FvDistOutNotifier::UpdateTrackDetectors()
{
	s_List.BeginIterator();
	while (!s_List.IsEnd())
	{
		FvRefListNameSpace<FvDistOutNotifier>::List1::iterator iter = s_List.GetIterator();
		s_List.Next();
		FvDistOutNotifier* _this = static_cast<FvDistOutNotifier*>(iter);
		FV_ASSERT(_this);
		_this->_Update();
	}
}

FvDistOutNotifier::FvDistOutNotifier(void)
{}
FvDistOutNotifier::~FvDistOutNotifier(void)
{}

void FvDistOutNotifier::_Update()
{
	FV_ASSERT_ERROR(m_spTarget0 && m_spTarget1);
	const float fDistance2 = FvDistance2(m_spTarget0->GetData0(), m_spTarget1->GetData0());
	if(fDistance2 > m_fRange2 )
	{
		DetachUpdate();
		_OnOut();
	}
}
bool FvDistOutNotifier::IsAttach()const
{
	return FvRefNode1::IsAttach();
}
void FvDistOutNotifier::AttachUpdate()
{
	FV_ASSERT_WARNING(m_spTarget0 && m_spTarget1);
	FV_CHECK_RETURN(m_spTarget0 && m_spTarget1);
	s_List.AttachBack(*this);
}
void FvDistOutNotifier::DetachUpdate()
{
	FvRefNode1::Detach();
}

//+----------------------------------------------------------------------------------------------------

FvRefListNameSpace<FvDistInNotifier>::List1 FvDistInNotifier::s_List;

void FvDistInNotifier::UpdateTrackDetectors()
{
	s_List.BeginIterator();
	while (!s_List.IsEnd())
	{
		FvRefListNameSpace<FvDistInNotifier>::List1::iterator iter = s_List.GetIterator();
		s_List.Next();
		FvDistInNotifier* _this = (*iter).Content();
		FV_ASSERT_ERROR(_this);
		_this->_Update();
	}
}

FvDistInNotifier::FvDistInNotifier(void)
{}

FvDistInNotifier::~FvDistInNotifier(void)
{}

void FvDistInNotifier::_Update()
{
	FV_ASSERT_ERROR(m_spTarget0 && m_spTarget1);
	const float fDistance2 = FvDistance2(m_spTarget0->GetData0(), m_spTarget1->GetData0());
	if(fDistance2 < m_fRange2)
	{
		DetachUpdate();
		_OnIn();
	}
}
bool FvDistInNotifier::IsAttach()const
{
	return FvRefNode1::IsAttach();
}
void FvDistInNotifier::AttachUpdate()
{
	FV_ASSERT_WARNING(m_spTarget0 && m_spTarget1);
	FV_CHECK_RETURN(m_spTarget0 && m_spTarget1);
	s_List.AttachBack(*this);
}
void FvDistInNotifier::DetachUpdate()
{
	FvRefNode1::Detach();
}