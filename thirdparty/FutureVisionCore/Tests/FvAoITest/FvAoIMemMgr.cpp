#include "FvAoIMemMgr.h"



struct MemNode
{
	MemNode*	pkNode;
};


struct MemPage
{
	MemPage*	pkPage;
};


FvAoIMemMgr::FvAoIMemMgr()
:m_uiNodeSize(0),m_uiNodeIncrCnt(0)
,m_pcIdx(NULL),m_pcEnd(NULL)
,m_uiUseCnt(0)
,m_pkFreeNode(NULL),m_pkMemPage(NULL)
{

}

FvAoIMemMgr::~FvAoIMemMgr()
{
	MemPage* pkPage = (MemPage*)m_pkMemPage;
	while(pkPage)
	{
		char* pkMem = (char*)pkPage;
		pkPage = pkPage->pkPage;
		delete [] pkMem;
	}
	m_pkMemPage = NULL;
}

bool FvAoIMemMgr::Init(FvUInt16 uiNodeSize, FvUInt32 uiInitCnt, FvUInt32 uiIncrCnt)
{
	if(m_pkMemPage || !uiNodeSize || !uiInitCnt)
		return false;

	if(uiIncrCnt)
		m_uiNodeIncrCnt = uiIncrCnt;
	else
		m_uiNodeIncrCnt = uiInitCnt;

	m_uiNodeSize = ((uiNodeSize+3)>>2)<<2;

	FvUInt32 uiInitSize = m_uiNodeSize * uiInitCnt;
	MemPage* pkNewPage = (MemPage*)new char[uiInitSize +4];
	pkNewPage->pkPage = NULL;
	m_pkMemPage = pkNewPage;

	m_pcIdx = (char*)pkNewPage +4;
	m_pcEnd = m_pcIdx + uiInitSize -1;

	return true;
}

void* FvAoIMemMgr::Pop()
{
	FV_ASSERT(m_pkMemPage);

	void* pkRet(NULL);

	MemNode* pkNode = (MemNode*)m_pkFreeNode;
	if(pkNode)
	{
		pkRet = pkNode;
		m_pkFreeNode = pkNode->pkNode;
	}
	else
	{
		if(m_pcIdx > m_pcEnd)
		{
			//! TODO: 内存只增不减,需要可以回收
			FvUInt32 uiIncrSize = m_uiNodeSize * m_uiNodeIncrCnt;
			MemPage* pkNewPage = (MemPage*)new char[uiIncrSize +4];
			pkNewPage->pkPage = (MemPage*)m_pkMemPage;
			m_pkMemPage = pkNewPage;

			m_pcIdx = (char*)pkNewPage +4;
			m_pcEnd = m_pcIdx + uiIncrSize -1;
		}

		pkRet = m_pcIdx;
		m_pcIdx += m_uiNodeSize;
	}

	++m_uiUseCnt;
	return pkRet;
}

void FvAoIMemMgr::Push(void* pkNode)
{
	FV_ASSERT(pkNode);

	((MemNode*)pkNode)->pkNode = (MemNode*)m_pkFreeNode;
	m_pkFreeNode = pkNode;
}


