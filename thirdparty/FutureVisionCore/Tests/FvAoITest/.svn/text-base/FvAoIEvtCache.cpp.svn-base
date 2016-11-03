#include "FvAoIEvtCache.h"
#include "FvAoIObj.h"



FvAoIEvtCache::FvAoIEvtCache(FvUInt32 uiInitSize)
:m_uiDummy(0)
,m_uiIndex(0)
,m_uiMaxSize(0)
,m_ppkCache(NULL)
{
	if(!uiInitSize)
		m_uiMaxSize = 128;
	else
		m_uiMaxSize = uiInitSize;

	m_ppkCache = new FvAoIBase*[m_uiMaxSize];
}

FvAoIEvtCache::~FvAoIEvtCache()
{
	FV_SAFE_DELETE_ARRAY(m_ppkCache);
}

void FvAoIEvtCache::Reset(FvUInt64 uiDummy)
{
	m_uiIndex = 0;
	m_uiDummy = uiDummy;
	//printf("-----------------------------\n");
	//printf("Dummy = %d\n", uiDummy);
}

void FvAoIEvtCache::AddEvt(FvAoIBase* pkObj, bool bIn)
{
	FV_ASSERT(pkObj && pkObj->m_uiDummy <= m_uiDummy);

	if(pkObj->m_uiDummy < m_uiDummy)
	{
		pkObj->m_uiDummy = m_uiDummy;
		if(bIn)
		{
			pkObj->m_bAdd = true;
			pkObj->m_bAddFlg = true;
			//if(pkObj->m_pkUserData)
			//{
			//	printf("Set %d = 2\n", *(int*)(pkObj->m_pkUserData));
			//}
		}
		else
		{
			pkObj->m_bAdd = false;
			pkObj->m_bAddFlg = false;
			//if(pkObj->m_pkUserData)
			//{
			//	printf("Set %d = 0\n", *(int*)(pkObj->m_pkUserData));
			//}
		}

		if(m_uiIndex >= m_uiMaxSize)
			CheckCacheOverload();

		m_ppkCache[m_uiIndex] = pkObj;
		++m_uiIndex;
	}
	else
	{
		if(bIn)
		{
			//if(pkObj->m_pkUserData)
			//{
			//	printf("++ %d : %d\n", *(int*)(pkObj->m_pkUserData), pkObj->m_uiCnter);
			//}
			//FV_ASSERT(pkObj->m_uiCnter != 2);
			//++pkObj->m_uiCnter;
			pkObj->m_bAddFlg = true;
		}
		else
		{
			//if(pkObj->m_pkUserData)
			//{
			//	printf("-- %d : %d\n", *(int*)(pkObj->m_pkUserData), pkObj->m_uiCnter);
			//}
			//FV_ASSERT(pkObj->m_uiCnter != 0);
			//--pkObj->m_uiCnter;
			pkObj->m_bAddFlg = false;
		}
	}
}

bool FvAoIEvtCache::AddEvt(FvAoIEvt* pkEvt, bool bFocusObj)
{
	FV_ASSERT(pkEvt);

	if(bFocusObj)
	{
		if(pkEvt->IsDel())
			return false;

		AddEvt(pkEvt->m_pkObj, pkEvt->IsIn());
	}
	else
	{
		FV_ASSERT(!pkEvt->IsDel());

		AddEvt(pkEvt->m_pkObs, pkEvt->IsIn());
	}

	return true;
}

FvAoIBase** FvAoIEvtCache::GetCache(FvUInt32& uiCnt)
{
	uiCnt = m_uiIndex;
	return m_ppkCache;
}

void FvAoIEvtCache::CheckCacheOverload()
{
	FV_ASSERT(m_uiIndex >= m_uiMaxSize);

	FvUInt32 newMaxSize = m_uiMaxSize << 1;
	FvAoIBase**	ppkNewCache = new FvAoIBase*[newMaxSize];
	memcpy_s(ppkNewCache, newMaxSize*sizeof(FvAoIBase*), m_ppkCache, m_uiIndex*sizeof(FvAoIBase*));
	delete [] m_ppkCache;
	m_ppkCache = ppkNewCache;
	m_uiMaxSize = newMaxSize;
}


