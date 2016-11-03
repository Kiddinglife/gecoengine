//{future header message}
#ifndef __FvAoIEvtCache_H__
#define __FvAoIEvtCache_H__

#include "FvAoIUtility.h"

struct FvAoIBase;
struct FvAoIEvt;


class FvAoIEvtCache
{
public:
	FvAoIEvtCache(FvUInt32 uiInitSize = 0);
	~FvAoIEvtCache();

	void		Reset(FvUInt64 uiDummy);
	void		AddEvt(FvAoIBase* pkObj, bool bIn);
	bool		AddEvt(FvAoIEvt* pkEvt, bool bFocusObj);	//! ·µ»ØFalse:EvtµÄflgÎªDel
	FvAoIBase**	GetCache(FvUInt32& uiCnt);

protected:
	void		CheckCacheOverload();

protected:
	FvUInt64	m_uiDummy;
	FvUInt32	m_uiIndex;
	FvUInt32	m_uiMaxSize;
	FvAoIBase**	m_ppkCache;
};


#endif//__FvAoIEvtCache_H__

