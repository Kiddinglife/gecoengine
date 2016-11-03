//{future header message}
#ifndef __FvAoIMemMgr_H__
#define __FvAoIMemMgr_H__

#include "FvAoIUtility.h"


class FvAoIMemMgr
{
public:
	FvAoIMemMgr();
	~FvAoIMemMgr();

	bool		Init(FvUInt16 uiNodeSize, FvUInt32 uiInitCnt, FvUInt32 uiIncrCnt);

	FvUInt32	UseCnt() const { return m_uiUseCnt; }

	void*		Pop();
	void		Push(void* pkNode);

protected:
	FvUInt32	m_uiNodeIncrCnt;
	char*		m_pcIdx;
	char*		m_pcEnd;
	FvUInt32	m_uiUseCnt;
	void*		m_pkFreeNode;
	void*		m_pkMemPage;
	FvUInt16	m_uiNodeSize;

};


#endif//__FvAoIMemMgr_H__