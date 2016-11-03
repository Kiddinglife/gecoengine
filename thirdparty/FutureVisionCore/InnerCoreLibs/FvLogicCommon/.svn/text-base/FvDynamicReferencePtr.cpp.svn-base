#include "FvDynamicReferencePtr.h"


//+-----------------------------------------------------------------------------------------------------------------


FvDynamicReferencePtr::FvDynamicReferencePtr(const FvDynamicReferencePtr& ptr)
{
	m_uiType = ptr.m_uiType;
	if(ptr.m_kNode.m_Content)
	{
		m_kNode.m_Content = ptr.m_kNode.m_Content;
		FvDoubleLink<void*>::PushAfter(const_cast<FvDynamicReferencePtr&>(ptr).m_kNode, m_kNode);
	}
}
FvDynamicReferencePtr& FvDynamicReferencePtr::operator=(const FvDynamicReferencePtr& ptr)
{
	m_uiType = ptr.m_uiType;
	if(ptr.m_kNode.m_Content)
	{
		m_kNode.m_Content = ptr.m_kNode.m_Content;
		FvDoubleLink<void*>::PushAfter(const_cast<FvDynamicReferencePtr&>(ptr).m_kNode, m_kNode);
	}
	else
	{
		m_kNode.m_Content = NULL;
		m_kNode.Detach();
	}
	return *this;
}
bool FvDynamicReferencePtr::operator==(const FvDynamicReferencePtr& ptr)const
{
	if(m_uiType != ptr.m_uiType)
	{
		return false;
	}
	return m_kNode.m_Content == ptr.m_kNode.m_Content;
}
bool FvDynamicReferencePtr::operator!=(const FvDynamicReferencePtr& ptr)const
{
	if(m_uiType != ptr.m_uiType)
	{
		return true;
	}
	return m_kNode.m_Content != ptr.m_kNode.m_Content;
}
void FvDynamicReferencePtr::Clear()
{
	m_kNode.Detach();
	m_kNode.m_Content = NULL;
	m_uiType = NULL_TYPE;
}
bool FvDynamicReferencePtr::IsClear()const
{
	return (m_kNode.m_Content == NULL);
}