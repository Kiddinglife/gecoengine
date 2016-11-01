#include "FvReferenceCountor.h"

#include <FvDebug.h>

FvReferenceCountor::FvReferenceCountor(void)
:m_uiCount(0)
{
}

FvReferenceCountor::~FvReferenceCountor(void)
{
}
bool FvReferenceCountor::Add()
{
	if(m_uiCount == 0)
	{
		++m_uiCount;
		return true;
	}
	else
	{
		++m_uiCount;
		return false;
	}
}
bool FvReferenceCountor::Del()
{
	FV_ASSERT(m_uiCount > 0 && "没有计数");
	if(m_uiCount > 0)
	{
		--m_uiCount;
		if(m_uiCount == 0)
		{
			return true;
		}
	}
	return false;
}
bool FvReferenceCountor::HasCount()const
{
	return (m_uiCount > 0);
}