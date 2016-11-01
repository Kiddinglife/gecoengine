
template<typename TAoIExt, typename TTrapExt>
FvAoIMgr<TAoIExt, TTrapExt>::FvAoIMgr()
:m_kEvtCache()
,m_uiDummy(0)
,m_fCoorScale(1.0f),m_fCoorScaleInv(1.0f)
,m_iMinStep(0)
,m_iGridMinX(0),m_iGridMinY(0),m_iGridMaxX(0),m_iGridMaxY(0)
,m_iGridInnerMinX(0),m_iGridInnerMinY(0),m_iGridInnerMaxX(0),m_iGridInnerMaxY(0)
,m_fXYRate(.0f)
,m_fInsOffsetX(.0f),m_fInsOffsetY(.0f)
,m_fInsInvX(.0f),m_fInsInvY(.0f)
,m_uiRefPtCntInAxis(0)
,m_pkRefPts(NULL)
{
	m_kMinPos.Init(FVAOI_MIN_POS, FVAOI_MIN_POS, true, false, NULL);
	m_kMaxPos.Init(FVAOI_MAX_POS, FVAOI_MAX_POS, true, false, NULL);

	m_kMinPos.m_pkPreX = NULL;
	m_kMinPos.m_pkNexX = &m_kMaxPos;
	m_kMaxPos.m_pkPreX = &m_kMinPos;
	m_kMaxPos.m_pkNexX = NULL;

	m_kMinPos.m_pkPreY = NULL;
	m_kMinPos.m_pkNexY = &m_kMaxPos;
	m_kMaxPos.m_pkPreY = &m_kMinPos;
	m_kMaxPos.m_pkNexY = NULL;
}

template<typename TAoIExt, typename TTrapExt>
FvAoIMgr<TAoIExt, TTrapExt>::~FvAoIMgr()
{
	FV_SAFE_DELETE_ARRAY(m_pkRefPts);
}

template<typename TAoIExt, typename TTrapExt>
bool FvAoIMgr<TAoIExt, TTrapExt>::Init(float fX, float fY, float fWidthX, float fWidthY,
										float fGridSize, FvUInt8 uiRefPtLevel, float fMinStep,
										FvUInt32 uiObjAndObsInitCnt, FvUInt32 uiObjAndObsIncrCnt,
										FvUInt32 uiPosInitCnt, FvUInt32 uiPosIncrCnt,
										FvUInt32 uiRelaAndEvtInitCnt, FvUInt32 uiRelaAndEvtIncrCnt,
										FvUInt32 uiAoiExtInitCnt, FvUInt32 uiAoiExtIncrCnt,
										FvUInt32 uiTrapExtInitCnt, FvUInt32 uiTrapExtIncrCnt)
{
	if(fWidthX <= .0f || fWidthY <= .0f ||
		fGridSize <= .0f ||
		uiRefPtLevel > 5 ||
		m_pkRefPts)
		return false;

	if(!m_kObjAndObsAlloc.Init(sizeof(FvAoIObject), uiObjAndObsInitCnt, uiObjAndObsIncrCnt)) return false;
	if(!m_kPosAlloc.Init(sizeof(FvAoIPos), uiPosInitCnt, uiPosIncrCnt)) return false;
	FV_ASSERT(sizeof(FvAoIRelate) == sizeof(FvAoIEvt));
	if(!m_kRelaAndEvtAlloc.Init(sizeof(FvAoIRelate), uiRelaAndEvtInitCnt, uiRelaAndEvtIncrCnt)) return false;
	if(!m_kAoiExtAlloc.Init(sizeof(TAoIExt), uiAoiExtInitCnt, uiAoiExtIncrCnt)) return false;
	if(!m_kTrapExtAlloc.Init(sizeof(TTrapExt), uiTrapExtInitCnt, uiTrapExtIncrCnt)) return false;

	FvUInt32 uiGridSize = (FvUInt32)RoundToInt(fGridSize * 100.0f);
	FV_ASSERT(uiGridSize);
	FvUInt32 uiGridSizeP2(uiGridSize);
	if(!IsPowerOfTwo(uiGridSize)) uiGridSizeP2 = NextPowerOfTwo(uiGridSize);
	FvUInt32 uiGridSizeBits = Log2(uiGridSizeP2);
	m_fCoorScale = uiGridSizeP2 / fGridSize;
	m_fCoorScaleInv = fGridSize / uiGridSizeP2;
	m_iMinStep = TransPosAndScale(fMinStep);
	m_fXYRate = fWidthX / fWidthY;

	m_iGridMinX = TransPosAndScale(fX);
	m_iGridMinY = TransPosAndScale(fY);
	m_iGridMaxX = TransPosAndScale(fX + fWidthX);
	m_iGridMaxY = TransPosAndScale(fY + fWidthY);
	FvInt32 iGridMidX = TransPosAndScale(fX + fWidthX * .5f);
	FvInt32 iGridMidY = TransPosAndScale(fY + fWidthY * .5f);

	if(!m_kGrid.Init(uiGridSizeBits, m_iGridMinX, m_iGridMinY, m_iGridMaxX, m_iGridMaxY))
		return false;

	if(uiRefPtLevel == 0)
	{
		m_fInsOffsetX = fX;
		m_fInsOffsetY = fY;
		m_fInsInvX = 1.0f / fWidthX;
		m_fInsInvY = 1.0f / fWidthY;
		m_iGridInnerMinX = iGridMidX +1;
		m_iGridInnerMinY = iGridMidY +1;
		m_iGridInnerMaxX = iGridMidX -1;
		m_iGridInnerMaxY = iGridMidY -1;
	}
	else
	{
		m_uiRefPtCntInAxis = 0x01 << uiRefPtLevel;
		m_fInsInvX = m_uiRefPtCntInAxis / fWidthX;
		m_fInsInvY = m_uiRefPtCntInAxis / fWidthY;
		float fRefWidthX = fWidthX / m_uiRefPtCntInAxis;
		float fRefWidthY = fWidthY / m_uiRefPtCntInAxis;
		float fHalfRefWidthX = fRefWidthX * .5f;
		float fHalfRefWidthY = fRefWidthY * .5f;
		m_fInsOffsetX = fX - fHalfRefWidthX;
		m_fInsOffsetY = fY - fHalfRefWidthY;
		FvInt32 iInnerX = TransPosAndScale(fRefWidthX * .25f);	//! TODO: 可以调整比率,让Inner边界再往里一些
		FvInt32 iInnerY = TransPosAndScale(fRefWidthY * .25f);
		m_iGridInnerMinX = m_iGridMinX + iInnerX;
		m_iGridInnerMinY = m_iGridMinY + iInnerY;
		m_iGridInnerMaxX = m_iGridMaxX - iInnerX;
		m_iGridInnerMaxY = m_iGridMaxY - iInnerY;

		m_pkRefPts = new FvAoIObject[0x01 << (uiRefPtLevel << 1)];

		float fRefX;
		float fRefY = fY + fHalfRefWidthY;
		int iRefIdx(0);
		for(FvUInt32 j=0; j<m_uiRefPtCntInAxis; ++j)
		{
			fRefX = fX + fHalfRefWidthX;
			FvInt32 iRefY = TransPosAndScale(fRefY);

			for(FvUInt32 i=0; i<m_uiRefPtCntInAxis; ++i)
			{
				m_pkRefPts[iRefIdx].Init(TransPosAndScale(fRefX), iRefY, 0, 0xFFFF, true,
					NULL,
					m_kPosAlloc.Pop());

				++iRefIdx;
				fRefX += fRefWidthX;
			}

			fRefY += fRefWidthY;
		}

		//! 将RefPt的Pos加入到队列排序
		for(FvUInt32 i=0; i<m_uiRefPtCntInAxis; ++i)
		{
			iRefIdx = i;
			for(FvUInt32 j=0; j<m_uiRefPtCntInAxis; ++j)
			{
				FvAoIPos* pkPos = m_pkRefPts[iRefIdx].m_pkPos;

				pkPos->m_pkPreX = m_kMaxPos.m_pkPreX;
				pkPos->m_pkNexX = &m_kMaxPos;
				m_kMaxPos.m_pkPreX->m_pkNexX = pkPos;
				m_kMaxPos.m_pkPreX = pkPos;

				iRefIdx += m_uiRefPtCntInAxis;
			}
		}

		iRefIdx = 0;
		for(FvUInt32 j=0; j<m_uiRefPtCntInAxis; ++j)
		{
			for(FvUInt32 i=0; i<m_uiRefPtCntInAxis; ++i)
			{
				FvAoIPos* pkPos = m_pkRefPts[iRefIdx].m_pkPos;

				pkPos->m_pkPreY = m_kMaxPos.m_pkPreY;
				pkPos->m_pkNexY = &m_kMaxPos;
				m_kMaxPos.m_pkPreY->m_pkNexY = pkPos;
				m_kMaxPos.m_pkPreY = pkPos;

				++iRefIdx;
			}
		}
	}

	return true;
}

class CheckerForAddObjWithRefPt
{
public:
	CheckerForAddObjWithRefPt(FvAoIEvtCache* pkEvtCache, FvUInt16 uiMask, FvInt32 iX, FvInt32 iY, FvInt32 iRefX, FvInt32 iRefY)
	:m_pkEvtCache(pkEvtCache),m_uiMask(uiMask),m_iX(iX),m_iY(iY),m_iRefX(iRefX),m_iRefY(iRefY)
	{
	}

#define _UP_MIN(_T)													\
	if(pkPos->IsBoxMin())											\
	{																\
		FvAoIObserver* pkObs = (FvAoIObserver*)(pkPos->m_pkOwner);	\
		if(m_uiMask & pkObs->m_uiMask &&							\
			pkPos->m_i##_T < m_i##_T &&								\
			m_iX < AOIOBS_MAX_X(pkObs) &&							\
			m_iY < AOIOBS_MAX_Y(pkObs))								\
			m_pkEvtCache->AddEvt(pkObs, true);						\
	}

#define _UP_MAX()													\
	else if(pkPos->IsBoxMax())										\
	{																\
		FvAoIObserver* pkObs = (FvAoIObserver*)(pkPos->m_pkOwner);	\
		if(m_uiMask & pkObs->m_uiMask &&							\
			m_iRefX <= pkPos->m_iX &&								\
			m_iRefY <= pkPos->m_iY &&								\
			AOIOBS_MIN_X(pkObs) <= m_iRefX &&						\
			AOIOBS_MIN_Y(pkObs) <= m_iRefY)							\
			m_pkEvtCache->AddEvt(pkObs, false);						\
	}

#define _DOWN_MIN()													\
	if(pkPos->IsBoxMin())											\
	{																\
		FvAoIObserver* pkObs = (FvAoIObserver*)(pkPos->m_pkOwner);	\
		if(m_uiMask & pkObs->m_uiMask &&							\
			pkPos->m_iX <= m_iRefX &&								\
			pkPos->m_iY <= m_iRefY &&								\
			m_iRefX <= AOIOBS_MAX_X(pkObs) &&						\
			m_iRefY <= AOIOBS_MAX_Y(pkObs))							\
			m_pkEvtCache->AddEvt(pkObs, false);						\
	}

#define _DOWN_MAX(_T)												\
	else if(pkPos->IsBoxMax())										\
	{																\
		FvAoIObserver* pkObs = (FvAoIObserver*)(pkPos->m_pkOwner);	\
		if(m_uiMask & pkObs->m_uiMask &&							\
			m_i##_T < pkPos->m_i##_T &&								\
			AOIOBS_MIN_X(pkObs) < m_iX &&							\
			AOIOBS_MIN_Y(pkObs) < m_iY)								\
			m_pkEvtCache->AddEvt(pkObs, true);						\
	}

	void UpXCheck(FvAoIPos* pkPos)
	{
		_UP_MIN(Y)
		_UP_MAX()
	}

	void DownXCheck(FvAoIPos* pkPos)
	{
		_DOWN_MIN()
		_DOWN_MAX(Y)
	}

	void UpYCheck(FvAoIPos* pkPos)
	{
		_UP_MIN(X)
		_UP_MAX()
	}

	void DownYCheck(FvAoIPos* pkPos)
	{
		_DOWN_MIN()
		_DOWN_MAX(X)
	}

#undef _UP_MIN
#undef _UP_MAX
#undef _DOWN_MIN
#undef _DOWN_MAX

protected:
	FvUInt16		m_uiMask;
	FvInt32			m_iX, m_iY;
	FvInt32			m_iRefX, m_iRefY;
	FvAoIEvtCache*	m_pkEvtCache;
};

class CheckerForAddObj
{
public:
	CheckerForAddObj(FvAoIMemMgr* pkRelaAndEvtAlloc, FvAoIObject* pkObj, FvUInt16 uiMask, FvInt32 iX, FvInt32 iY)
	:m_pkRelaAndEvtAlloc(pkRelaAndEvtAlloc),m_pkObj(pkObj),m_uiMask(uiMask),m_iX(iX),m_iY(iY)
	{
	}

#define _UP_CHECK(_T)												\
	if(pkPos->IsBoxMin())											\
	{																\
		FvAoIObserver* pkObs = (FvAoIObserver*)(pkPos->m_pkOwner);	\
		if(m_uiMask & pkObs->m_uiMask &&							\
			pkPos->m_i##_T < m_i##_T &&								\
			m_iX < AOIOBS_MAX_X(pkObs) &&							\
			m_iY < AOIOBS_MAX_Y(pkObs))								\
		{															\
			FvAoIEvt* pkEvt = (FvAoIEvt*)m_pkRelaAndEvtAlloc->Pop();\
			pkEvt->InitIn(pkObs, m_pkObj);							\
			pkObs->AddEvt(pkEvt->m_kObsNode);						\
			m_pkObj->AddEvt(pkEvt->m_kObjNode);						\
		}															\
	}

#define _DOWN_CHECK(_T)												\
	if(pkPos->IsBoxMax())											\
	{																\
		FvAoIObserver* pkObs = (FvAoIObserver*)(pkPos->m_pkOwner);	\
		if(m_uiMask & pkObs->m_uiMask &&							\
			m_i##_T < pkPos->m_i##_T &&								\
			AOIOBS_MIN_X(pkObs) < m_iX &&							\
			AOIOBS_MIN_Y(pkObs) < m_iY)								\
		{															\
			FvAoIEvt* pkEvt = (FvAoIEvt*)m_pkRelaAndEvtAlloc->Pop();\
			pkEvt->InitIn(pkObs, m_pkObj);							\
			pkObs->AddEvt(pkEvt->m_kObsNode);						\
			m_pkObj->AddEvt(pkEvt->m_kObjNode);						\
		}															\
	}

	void UpXCheck(FvAoIPos* pkPos)
	{
		_UP_CHECK(Y)
	}

	void DownXCheck(FvAoIPos* pkPos)
	{
		_DOWN_CHECK(Y)
	}

	void UpYCheck(FvAoIPos* pkPos)
	{
		_UP_CHECK(X)
	}

	void DownYCheck(FvAoIPos* pkPos)
	{
		_DOWN_CHECK(X)
	}

#undef _UP_CHECK
#undef _DOWN_CHECK

protected:
	FvUInt16		m_uiMask;
	FvInt32			m_iX, m_iY;
	FvAoIMemMgr*	m_pkRelaAndEvtAlloc;
	FvAoIObject*	m_pkObj;
};

template<typename TAoIExt, typename TTrapExt>
void FvAoIMgr<TAoIExt, TTrapExt>::AddObjectToOrderedGrid(FvAoIObject* pkObj, float fX, float fY, FvInt32 iX, FvInt32 iY)
{
	FvAoIPos* pkInsX(NULL), * pkInsY(NULL);
	bool bUpX(false), bUpY(false), bInsertXFirst(false);
	FvInt32 iDisX(0), iDisY(0);
	FvInt32 iIdX(0), iIdY(0);
	bool bInGridX(false), bInGridY(false);

	if(iX < m_iGridInnerMinX)
	{
		pkInsX = &m_kMinPos;
		bUpX = true;
		iDisX = iX - m_iGridMinX;
	}
	else if(m_iGridInnerMaxX < iX)
	{
		pkInsX = &m_kMaxPos;
		iDisX = m_iGridMaxX - iX;
	}
	else
	{
		bInGridX = true;
		FV_ASSERT(m_pkRefPts);

		iIdX = RoundToInt((fX - m_fInsOffsetX) * m_fInsInvX) -1;
		FV_ASSERT(-1 < iIdX && iIdX < (FvInt32)m_uiRefPtCntInAxis);
		FvAoIPos* pkRefPos = m_pkRefPts[iIdX].m_pkPos;
		if(pkRefPos->m_iX < iX)
		{
			bUpX = true;
			pkInsX = m_pkRefPts[(m_uiRefPtCntInAxis -1) * m_uiRefPtCntInAxis + iIdX].m_pkPos;
			iDisX = iX - pkRefPos->m_iX;
		}
		else
		{
			pkInsX = pkRefPos;
			iDisX = pkRefPos->m_iX - iX;
		}
	}

	if(iY < m_iGridInnerMinY)
	{
		pkInsY = &m_kMinPos;
		bUpY = true;
		iDisY = iY - m_iGridMinY;
	}
	else if(m_iGridInnerMaxY < iY)
	{
		pkInsY = &m_kMaxPos;
		iDisY = m_iGridMaxY - iY;
	}
	else
	{
		bInGridY = true;
		FV_ASSERT(m_pkRefPts);

		iIdY = RoundToInt((fY - m_fInsOffsetY) * m_fInsInvY) -1;
		FV_ASSERT(-1 < iIdY && iIdY < (FvInt32)m_uiRefPtCntInAxis);
		FvAoIPos* pkRefPos = m_pkRefPts[iIdY * m_uiRefPtCntInAxis].m_pkPos;
		if(pkRefPos->m_iY < iY)
		{
			bUpY = true;
			pkInsY = m_pkRefPts[(iIdY +1) * m_uiRefPtCntInAxis -1].m_pkPos;
			iDisY = iY - pkRefPos->m_iY;
		}
		else
		{
			pkInsY = pkRefPos;
			iDisY = pkRefPos->m_iY - iY;
		}
	}

	if(bInGridX == bInGridY)
	{
		if(float(iDisX) < float(iDisY) * m_fXYRate)
			bInsertXFirst = true;
	}
	else if(bInGridY)
	{
		bInsertXFirst = true;
	}

	if(bInGridX && bInGridY)
	{
		FV_ASSERT(m_pkRefPts);
		FvAoIObject& kRefPt = m_pkRefPts[iIdY * m_uiRefPtCntInAxis + iIdX];

		++m_uiDummy;
		FV_ASSERT(m_uiDummy < 0xFFFFFFFFFFFFFFFF);
		m_kEvtCache.Reset(m_uiDummy);

		FvAoIDLNode* pkDLNode = kRefPt.m_kRelateMin.m_pkNex;
		while(pkDLNode != &kRefPt.m_kRelateMax)
		{
			FvAoIObserver* pkObs = AOIRELATEOBJ(pkDLNode)->m_pkObs;
			if(pkObs->m_uiMask & pkObj->m_uiMask)
				m_kEvtCache.AddEvt(pkObs, true);
			pkDLNode = pkDLNode->m_pkNex;
		}

		pkDLNode = kRefPt.m_kEvtMin.m_pkNex;
		while(pkDLNode != &kRefPt.m_kEvtMax)
		{
			FvAoIEvt* pkEvt = AOIEVTOBJ(pkDLNode);
			if(pkEvt->m_pkObs->m_uiMask & pkObj->m_uiMask)
				m_kEvtCache.AddEvt(pkEvt, false);
			pkDLNode = pkDLNode->m_pkNex;
		}

		FvAoIPos* pkObjPos = pkObj->m_pkPos;
		CheckerForAddObjWithRefPt kChecker(&m_kEvtCache, pkObj->m_uiMask, pkObjPos->m_iX, pkObjPos->m_iY, kRefPt.m_pkPos->m_iX, kRefPt.m_pkPos->m_iY);
		if(bInsertXFirst)
		{
			if(bUpX)
				InsPosRightOnXAndCheck<CheckerForAddObjWithRefPt, &CheckerForAddObjWithRefPt::UpXCheck>(kRefPt.m_pkPos, pkObjPos, &kChecker);
			else
				InsPosLeftOnXAndCheck<CheckerForAddObjWithRefPt, &CheckerForAddObjWithRefPt::DownXCheck>(kRefPt.m_pkPos, pkObjPos, &kChecker);

			if(bUpY)
				InsPosRightOnYAndCheck<CheckerForAddObjWithRefPt, &CheckerForAddObjWithRefPt::UpYCheck>(kRefPt.m_pkPos, pkObjPos, &kChecker);
			else
				InsPosLeftOnYAndCheck<CheckerForAddObjWithRefPt, &CheckerForAddObjWithRefPt::DownYCheck>(kRefPt.m_pkPos, pkObjPos, &kChecker);
		}
		else
		{
			if(bUpX)
				InsPosRightOnXAndCheck<CheckerForAddObjWithRefPt, &CheckerForAddObjWithRefPt::UpXCheck>(kRefPt.m_pkPos, pkObjPos, &kChecker);
			else
				InsPosLeftOnXAndCheck<CheckerForAddObjWithRefPt, &CheckerForAddObjWithRefPt::DownXCheck>(kRefPt.m_pkPos, pkObjPos, &kChecker);

			if(bUpY)
				InsPosRightOnYAndCheck<CheckerForAddObjWithRefPt, &CheckerForAddObjWithRefPt::UpYCheck>(kRefPt.m_pkPos, pkObjPos, &kChecker);
			else
				InsPosLeftOnYAndCheck<CheckerForAddObjWithRefPt, &CheckerForAddObjWithRefPt::DownYCheck>(kRefPt.m_pkPos, pkObjPos, &kChecker);
		}

		FvUInt32 uiCacheEvts(0);
		FvAoIBase** ppkCache = m_kEvtCache.GetCache(uiCacheEvts);
		FvAoIBase* pkObs(NULL);
		FvAoIEvt* pkEvt(NULL);
		for(FvUInt32 i=0; i<uiCacheEvts; ++i)
		{
			FvAoIObserver* pkObs = (FvAoIObserver*)ppkCache[i];
			if(pkObs->NeedAdd())
			{
				pkEvt = (FvAoIEvt*)m_kRelaAndEvtAlloc.Pop();
				pkEvt->InitIn(pkObs, pkObj);
				pkObs->AddEvt(pkEvt->m_kObsNode);
				pkObj->AddEvt(pkEvt->m_kObjNode);

#ifdef FV_DEBUG
				if(pkObj->m_pkPos->m_iX < pkObs->m_pkPosMin->m_iX ||
					pkObs->m_pkPosMax->m_iX < pkObj->m_pkPos->m_iX ||
					pkObj->m_pkPos->m_iY < pkObs->m_pkPosMin->m_iY ||
					pkObs->m_pkPosMax->m_iY < pkObj->m_pkPos->m_iY)
				{
					//! 检测点是否在Box里面
					FV_ASSERT(0);
				}
#endif
			}
		}
	}
	else
	{
		FvAoIPos* pkObjPos = pkObj->m_pkPos;
		CheckerForAddObj kChecker(&m_kRelaAndEvtAlloc, pkObj, pkObj->m_uiMask, pkObjPos->m_iX, pkObjPos->m_iY);
		if(bInsertXFirst)
		{
			if(bUpX)
				InsPosRightOnXAndCheck<CheckerForAddObj, &CheckerForAddObj::UpXCheck>(pkInsX, pkObjPos, &kChecker);
			else
				InsPosLeftOnXAndCheck<CheckerForAddObj, &CheckerForAddObj::DownXCheck>(pkInsX, pkObjPos, &kChecker);

			if(bUpY)
				InsPosRightOnY(pkInsY, pkObjPos);
			else
				InsPosLeftOnY(pkInsY, pkObjPos);
		}
		else
		{
			if(bUpX)
				InsPosRightOnX(pkInsX, pkObjPos);
			else
				InsPosLeftOnX(pkInsX, pkObjPos);

			if(bUpY)
				InsPosRightOnYAndCheck<CheckerForAddObj, &CheckerForAddObj::UpYCheck>(pkInsY, pkObjPos, &kChecker);
			else
				InsPosLeftOnYAndCheck<CheckerForAddObj, &CheckerForAddObj::DownYCheck>(pkInsY, pkObjPos, &kChecker);
		}

#ifdef FV_DEBUG
		if(1)
		{
			FvAoIDLNode* pkBeginNode(NULL);
			FvAoIDLNode* pkEndNode(NULL);
			pkBeginNode = pkObj->m_kEvtMin.m_pkNex;
			pkEndNode = &pkObj->m_kEvtMax;
			while(pkBeginNode != pkEndNode)
			{
				FvAoIDLNode* pkNode = pkBeginNode;
				pkBeginNode = pkBeginNode->m_pkNex;

				if(AOIEVTOBJ(pkNode)->IsIn())
				{
					FvAoIObserver* pkObs = AOIEVTOBJ(pkNode)->m_pkObs;
					if(pkObj->m_pkPos->m_iX < pkObs->m_pkPosMin->m_iX ||
						pkObs->m_pkPosMax->m_iX < pkObj->m_pkPos->m_iX ||
						pkObj->m_pkPos->m_iY < pkObs->m_pkPosMin->m_iY ||
						pkObs->m_pkPosMax->m_iY < pkObj->m_pkPos->m_iY)
					{
						//! 检测点是否在Box里面
						FV_ASSERT(0);
					}
				}
			}
		}
#endif
	}

#ifdef FV_DEBUG
	if(1)
	{
		FvAoIPos* pkPos(NULL);
		pkPos = &m_kMinPos;
		while(pkPos != &m_kMaxPos)
		{
			if(pkPos->m_iX > pkPos->m_pkNexX->m_iX)
			{
				//! 检测坐标有序
				FV_ASSERT(0);
			}
			pkPos = pkPos->m_pkNexX;
		}
		pkPos = &m_kMinPos;
		while(pkPos != &m_kMaxPos)
		{
			if(pkPos->m_iY > pkPos->m_pkNexY->m_iY)
			{
				//! 检测坐标有序
				FV_ASSERT(0);
			}
			pkPos = pkPos->m_pkNexY;
		}
	}
#endif
}

class CheckerForAddObs
{
public:
	CheckerForAddObs(FvAoIMemMgr* pkRelaAndEvtAlloc, FvAoIObserver* pkObs, FvUInt16 uiMask, FvInt32 iMinX, FvInt32 iMinY, FvInt32 iMaxX, FvInt32 iMaxY)
	:m_pkRelaAndEvtAlloc(pkRelaAndEvtAlloc),m_pkObs(pkObs),m_uiMask(uiMask),m_iMinX(iMinX),m_iMinY(iMinY),m_iMaxX(iMaxX),m_iMaxY(iMaxY)
	{
	}

#define _ITERATE												\
	if(pkPos->IsPt() &&											\
		m_uiMask & pkPos->m_pkOwner->m_uiMask &&				\
		m_iMinX < pkPos->m_iX && pkPos->m_iX < m_iMaxX &&		\
		m_iMinY < pkPos->m_iY && pkPos->m_iY < m_iMaxY &&		\
		pkPos->m_pkOwner != m_pkObs->m_pkOwner)					\
	{															\
		FvAoIEvt* pkEvt = (FvAoIEvt*)m_pkRelaAndEvtAlloc->Pop();\
		pkEvt->InitIn(m_pkObs, (FvAoIObject*)pkPos->m_pkOwner);	\
		m_pkObs->AddEvt(pkEvt->m_kObsNode);						\
		pkPos->m_pkOwner->AddEvt(pkEvt->m_kObjNode);			\
	}

#define _DOWN_CHECK(_T, _S)										\
	if(pkPos->IsPt() &&											\
		m_uiMask & pkPos->m_pkOwner->m_uiMask &&				\
		pkPos->m_i##_T < m_iMax##_T &&							\
		m_iMin##_S < pkPos->m_i##_S &&							\
		pkPos->m_i##_S < m_iMax##_S &&							\
		pkPos->m_pkOwner != m_pkObs->m_pkOwner)					\
	{															\
		FvAoIEvt* pkEvt = (FvAoIEvt*)m_pkRelaAndEvtAlloc->Pop();\
		pkEvt->InitIn(m_pkObs, (FvAoIObject*)pkPos->m_pkOwner);	\
		m_pkObs->AddEvt(pkEvt->m_kObsNode);						\
		pkPos->m_pkOwner->AddEvt(pkEvt->m_kObjNode);			\
	}

	void Iterate(FvAoIPos* pkPos)
	{
		_ITERATE
	}

	void DownXCheck(FvAoIPos* pkPos)
	{
		_DOWN_CHECK(X,Y)
	}

	void DownYCheck(FvAoIPos* pkPos)
	{
		_DOWN_CHECK(Y,X)
	}

#undef _ITERATE
#undef _DOWN_CHECK

protected:
	FvUInt16		m_uiMask;
	FvInt32			m_iMinX;
	FvInt32			m_iMinY;
	FvInt32			m_iMaxX;
	FvInt32			m_iMaxY;
	FvAoIMemMgr*	m_pkRelaAndEvtAlloc;
	FvAoIObserver*	m_pkObs;
};

template<typename TAoIExt, typename TTrapExt>
FvAoIHandle FvAoIMgr<TAoIExt, TTrapExt>::AddObserver(FvAoIHandle hOwner, FvUInt16 uiMask, float fX, float fY, float fVision, float fDisVisibility, bool bAoI, void* pkData)
{
	FV_ASSERT(hOwner && hOwner->IsObject())
	if(fVision < .0f)
		fVision = .0f;

	FvInt32 iX = TransPosAndScale(fX);
	FvInt32 iY = TransPosAndScale(fY);
	FvInt32 iVision = TransPosAndScale(fVision);
	FvInt32 iDisVisibility = TransPosAndScale(fDisVisibility);
	FV_ASSERT(FVAOI_MIN_POS < iX && iX < FVAOI_MAX_POS &&
		FVAOI_MIN_POS < iY && iY < FVAOI_MAX_POS);

	FvAoIObserver* pkObs = (FvAoIObserver*)m_kObjAndObsAlloc.Pop();

	FvInt32 iVisionMinX = iX - iVision;
	FvInt32 iVisionMaxX = iX + iVision;
	FvInt32 iVisionMinY = iY - iVision;
	FvInt32 iVisionMaxY = iY + iVision;

	pkObs->m_pkPosMin = (FvAoIPos*)m_kPosAlloc.Pop();
	pkObs->m_pkPosMin->Init(iVisionMinX, iVisionMinY, false, false, pkObs);
	pkObs->m_pkPosMax = (FvAoIPos*)m_kPosAlloc.Pop();
	pkObs->m_pkPosMax->Init(iVisionMaxX, iVisionMaxY, false, true, pkObs);

	pkObs->Init((FvAoIObject*)hOwner, bAoI, iVision, iDisVisibility, uiMask, pkData);

	FvAoIPos* pkInsMinX(NULL), * pkInsMaxX(NULL), * pkInsMinY(NULL), * pkInsMaxY(NULL);
	bool bUpMinX(false), bUpMinY(false);
	bool bInsMinFirstX(true), bInsMinFirstY(true);

	if(iVisionMinX < m_iGridInnerMinX)
	{
		pkInsMinX = &m_kMinPos;
		bUpMinX = true;
	}
	else if(m_iGridInnerMaxX < iVisionMaxX)
	{
		bInsMinFirstX = false;
		pkInsMaxX = &m_kMaxPos;
	}
	else
	{
		FV_ASSERT(m_pkRefPts);
		FvInt32 iIdX = RoundToInt((fX - fVision - m_fInsOffsetX) * m_fInsInvX) -1;
		FV_ASSERT(-1 < iIdX && iIdX < (FvInt32)m_uiRefPtCntInAxis);
		pkInsMinX = m_pkRefPts[(m_uiRefPtCntInAxis -1) * m_uiRefPtCntInAxis + iIdX].m_pkPos;
		if(pkInsMinX->m_iX < iVisionMinX)
			bUpMinX = true;
	}

	if(iVisionMinY < m_iGridInnerMinY)
	{
		pkInsMinY = &m_kMinPos;
		bUpMinY = true;
	}
	else if(m_iGridInnerMaxY < iVisionMaxY)
	{
		bInsMinFirstY = false;
		pkInsMaxY = &m_kMaxPos;
	}
	else
	{
		FV_ASSERT(m_pkRefPts);
		FvInt32 iIdY = RoundToInt((fY - fVision - m_fInsOffsetY) * m_fInsInvY) -1;
		FV_ASSERT(-1 < iIdY && iIdY < (FvInt32)m_uiRefPtCntInAxis);
		pkInsMinY = m_pkRefPts[(iIdY +1) * m_uiRefPtCntInAxis -1].m_pkPos;
		if(pkInsMinY->m_iY < iVisionMinY)
			bUpMinY = true;
	}

	FvInt32 iCrossMinX = iVisionMinX < m_iGridMinX ? m_iGridMinX : iVisionMinX;
	FvInt32 iCrossMinY = iVisionMinY < m_iGridMinY ? m_iGridMinY : iVisionMinY;
	FvInt32 iCrossMaxX = m_iGridMaxX < iVisionMaxX ? m_iGridMaxX : iVisionMaxX;
	FvInt32 iCrossMaxY = m_iGridMaxY < iVisionMaxY ? m_iGridMaxY : iVisionMaxY;

	FvInt32 iDisX = iCrossMaxX - iCrossMinX;
	FvInt32 iDisY = iCrossMaxY - iCrossMinY;
	bool bCheckOnX = (float(iDisY) * m_fXYRate) < float(iDisX) ? false : true;
	CheckerForAddObs kChecker(&m_kRelaAndEvtAlloc, pkObs, pkObs->m_uiMask, iVisionMinX, iVisionMinY, iVisionMaxX, iVisionMaxY);

	if(bCheckOnX)
	{
		if(bInsMinFirstX)
		{
			if(bUpMinX)
			{
				InsPosRightOnX(pkInsMinX, pkObs->m_pkPosMin);
				InsPosRightOnXAndCheck<CheckerForAddObs, &CheckerForAddObs::Iterate>(pkObs->m_pkPosMin, pkObs->m_pkPosMax, &kChecker);
			}
			else
			{
				if(iVisionMaxX < pkInsMinX->m_iX)
				{
					InsPosLeftOnX(pkInsMinX, pkObs->m_pkPosMax);
					InsPosLeftOnXAndCheck<CheckerForAddObs, &CheckerForAddObs::Iterate>(pkObs->m_pkPosMax, pkObs->m_pkPosMin, &kChecker);
				}
				else
				{
					InsPosLeftOnXAndCheck<CheckerForAddObs, &CheckerForAddObs::DownXCheck>(pkInsMinX, pkObs->m_pkPosMin, &kChecker);
					InsPosRightOnXAndCheck<CheckerForAddObs, &CheckerForAddObs::Iterate>(pkInsMinX->m_pkPreX, pkObs->m_pkPosMax, &kChecker);
				}
			}
		}
		else
		{
			InsPosLeftOnX(pkInsMaxX, pkObs->m_pkPosMax);
			InsPosLeftOnXAndCheck<CheckerForAddObs, &CheckerForAddObs::Iterate>(pkObs->m_pkPosMax, pkObs->m_pkPosMin, &kChecker);
		}

		if(bInsMinFirstY)
		{
			if(bUpMinY)
			{
				InsPosRightOnY(pkInsMinY, pkObs->m_pkPosMin);
				InsPosRightOnY(pkObs->m_pkPosMin, pkObs->m_pkPosMax);
			}
			else
			{
				if(iVisionMaxY < pkInsMinY->m_iY)
				{
					InsPosLeftOnY(pkInsMinY, pkObs->m_pkPosMax);
					InsPosLeftOnY(pkObs->m_pkPosMax, pkObs->m_pkPosMin);
				}
				else
				{
					InsPosLeftOnY(pkInsMinY, pkObs->m_pkPosMin);
					InsPosRightOnY(pkInsMinY, pkObs->m_pkPosMax);
				}
			}
		}
		else
		{
			InsPosLeftOnY(pkInsMaxY, pkObs->m_pkPosMax);
			InsPosLeftOnY(pkObs->m_pkPosMax, pkObs->m_pkPosMin);
		}
	}
	else
	{
		if(bInsMinFirstX)
		{
			if(bUpMinX)
			{
				InsPosRightOnX(pkInsMinX, pkObs->m_pkPosMin);
				InsPosRightOnX(pkObs->m_pkPosMin, pkObs->m_pkPosMax);
			}
			else
			{
				if(iVisionMaxX < pkInsMinX->m_iX)
				{
					InsPosLeftOnX(pkInsMinX, pkObs->m_pkPosMax);
					InsPosLeftOnX(pkObs->m_pkPosMax, pkObs->m_pkPosMin);
				}
				else
				{
					InsPosLeftOnX(pkInsMinX, pkObs->m_pkPosMin);
					InsPosRightOnX(pkInsMinX, pkObs->m_pkPosMax);
				}
			}
		}
		else
		{
			InsPosLeftOnX(pkInsMaxX, pkObs->m_pkPosMax);
			InsPosLeftOnX(pkObs->m_pkPosMax, pkObs->m_pkPosMin);
		}

		if(bInsMinFirstY)
		{
			if(bUpMinY)
			{
				InsPosRightOnY(pkInsMinY, pkObs->m_pkPosMin);
				InsPosRightOnYAndCheck<CheckerForAddObs, &CheckerForAddObs::Iterate>(pkObs->m_pkPosMin, pkObs->m_pkPosMax, &kChecker);
			}
			else
			{
				if(iVisionMaxY < pkInsMinY->m_iY)
				{
					InsPosLeftOnY(pkInsMinY, pkObs->m_pkPosMax);
					InsPosLeftOnYAndCheck<CheckerForAddObs, &CheckerForAddObs::Iterate>(pkObs->m_pkPosMax, pkObs->m_pkPosMin, &kChecker);
				}
				else
				{
					InsPosLeftOnYAndCheck<CheckerForAddObs, &CheckerForAddObs::DownYCheck>(pkInsMinY, pkObs->m_pkPosMin, &kChecker);
					InsPosRightOnYAndCheck<CheckerForAddObs, &CheckerForAddObs::Iterate>(pkInsMinY->m_pkPreY, pkObs->m_pkPosMax, &kChecker);
				}
			}
		}
		else
		{
			InsPosLeftOnY(pkInsMaxY, pkObs->m_pkPosMax);
			InsPosLeftOnYAndCheck<CheckerForAddObs, &CheckerForAddObs::Iterate>(pkObs->m_pkPosMax, pkObs->m_pkPosMin, &kChecker);
		}
	}

#ifdef FV_DEBUG
	if(1)
	{
		FvAoIPos* pkPos(NULL);
		pkPos = &m_kMinPos;
		while(pkPos != &m_kMaxPos)
		{
			if(pkPos->m_iX > pkPos->m_pkNexX->m_iX)
			{
				//! 检测坐标有序
				FV_ASSERT(0);
			}
			pkPos = pkPos->m_pkNexX;
		}
		pkPos = &m_kMinPos;
		while(pkPos != &m_kMaxPos)
		{
			if(pkPos->m_iY > pkPos->m_pkNexY->m_iY)
			{
				//! 检测坐标有序
				FV_ASSERT(0);
			}
			pkPos = pkPos->m_pkNexY;
		}

		FvAoIDLNode* pkBeginNode(NULL);
		FvAoIDLNode* pkEndNode(NULL);
		pkBeginNode = pkObs->m_kEvtMin.m_pkNex;
		pkEndNode = &pkObs->m_kEvtMax;
		while(pkBeginNode != pkEndNode)
		{
			FvAoIDLNode* pkNode = pkBeginNode;
			pkBeginNode = pkBeginNode->m_pkNex;

			FvAoIObject* pkObj = AOIEVTOBS(pkNode)->m_pkObj;
			if(pkObj->m_pkPos->m_iX < pkObs->m_pkPosMin->m_iX ||
				pkObs->m_pkPosMax->m_iX < pkObj->m_pkPos->m_iX ||
				pkObj->m_pkPos->m_iY < pkObs->m_pkPosMin->m_iY ||
				pkObs->m_pkPosMax->m_iY < pkObj->m_pkPos->m_iY)
			{
				//! 检测点是否在Box里面
				FV_ASSERT(0);
			}
		}
	}
#endif

	return pkObs;
}

template<typename TAoIExt, typename TTrapExt>
void FvAoIMgr<TAoIExt, TTrapExt>::Remove(FvAoIHandle hHandle)
{
	FV_ASSERT(hHandle);

	FvAoIDLNode* pkBeginNode(NULL);
	FvAoIDLNode* pkEndNode(NULL);

	if(hHandle->IsObject())
	{
		FvAoIObject* pkObj = (FvAoIObject*)hHandle;

		pkBeginNode = pkObj->m_kEvtMin.m_pkNex;
		pkEndNode = &pkObj->m_kEvtMax;
		while(pkBeginNode != pkEndNode)
		{
			FvAoIDLNode* pkNode = pkBeginNode;
			pkBeginNode = pkBeginNode->m_pkNex;

			FvAoIEvt* pkEvt = AOIEVTOBJ(pkNode);
			pkEvt->m_kObsNode.DrawOut();
			m_kRelaAndEvtAlloc.Push(pkEvt);
		}

		pkBeginNode = pkObj->m_kRelateMin.m_pkNex;
		pkEndNode = &pkObj->m_kRelateMax;
		while(pkBeginNode != pkEndNode)
		{
			FvAoIDLNode* pkNode = pkBeginNode;
			pkBeginNode = pkBeginNode->m_pkNex;

			FvAoIRelate* pkRelate = AOIRELATEOBJ(pkNode);
			pkRelate->m_kObsNode.DrawOut();
			if(pkRelate->m_pkExt)
			{
				FvAoIEvt* pkEvt = (FvAoIEvt*)m_kRelaAndEvtAlloc.Pop();
				pkEvt->InitDel(pkRelate->m_pkObs, pkRelate->m_pkExt);
				pkRelate->m_pkObs->AddEvt(pkEvt->m_kObsNode);
			}
			m_kRelaAndEvtAlloc.Push(pkRelate);
		}

		m_kGrid.RemoveObject(pkObj);

		pkObj->m_pkPos->DrawOut();
		m_kPosAlloc.Push(pkObj->m_pkPos);
	}
	else
	{
		FvAoIObserver* pkObs = (FvAoIObserver*)hHandle;

		pkBeginNode = pkObs->m_kEvtMin.m_pkNex;
		pkEndNode = &pkObs->m_kEvtMax;
		while(pkBeginNode != pkEndNode)
		{
			FvAoIDLNode* pkNode = pkBeginNode;
			pkBeginNode = pkBeginNode->m_pkNex;

			FvAoIEvt* pkEvt = AOIEVTOBS(pkNode);
			if(pkEvt->IsDel())
			{
				if(pkObs->IsAoI())
					DestroyAoIExt(pkEvt->m_pkExt);
				else
					DestroyTrapExt(pkEvt->m_pkExt);
			}
			else
			{
				pkEvt->m_kObjNode.DrawOut();
			}
			m_kRelaAndEvtAlloc.Push(pkEvt);
		}

		pkBeginNode = pkObs->m_kRelateMin.m_pkNex;
		pkEndNode = &pkObs->m_kRelateMax;
		while(pkBeginNode != pkEndNode)
		{
			FvAoIDLNode* pkNode = pkBeginNode;
			pkBeginNode = pkBeginNode->m_pkNex;

			FvAoIRelate* pkRelate = AOIRELATEOBS(pkNode);
			pkRelate->m_kObjNode.DrawOut();
			if(pkRelate->m_pkExt)
			{
				pkRelate->m_pkObj->DelObs();

				if(pkObs->IsAoI())
					DestroyAoIExt(pkRelate->m_pkExt);
				else
					DestroyTrapExt(pkRelate->m_pkExt);
			}
			m_kRelaAndEvtAlloc.Push(pkRelate);
		}

		pkObs->m_pkPosMin->DrawOut();
		pkObs->m_pkPosMax->DrawOut();
		m_kPosAlloc.Push(pkObs->m_pkPosMin);
		m_kPosAlloc.Push(pkObs->m_pkPosMax);
	}
	m_kObjAndObsAlloc.Push(hHandle);
}

template<typename TAoIExt, typename TTrapExt>
bool FvAoIMgr<TAoIExt, TTrapExt>::SetMask(FvAoIHandle hHandle, FvUInt16 uiMask)
{
	FV_ASSERT(hHandle);
	if(hHandle->m_uiMask == uiMask)
		return true;

	hHandle->m_uiMask = uiMask;

	if(hHandle->IsObject())
		SetObjectMask((FvAoIObject*)hHandle);
	else
		SetObserverMask((FvAoIObserver*)hHandle);
	return true;
}

template<typename TAoIExt, typename TTrapExt>
bool FvAoIMgr<TAoIExt, TTrapExt>::SetVisibility(FvAoIHandle hHandle, float fVisibility)
{
	FV_ASSERT(hHandle && hHandle->IsObject());
	((FvAoIObject*)hHandle)->m_iVisibility = TransPosAndScale(fVisibility);
	return true;
}

class CheckerForSetVision
{
public:
	CheckerForSetVision(FvAoIMemMgr* pkRelaAndEvtAlloc, FvAoIObserver* pkObs,
		FvUInt16 uiMask,
		FvInt32 iOldMinX, FvInt32 iOldMaxX, FvInt32 iNewMinX, FvInt32 iNewMaxX,
		FvInt32 iOldMinY, FvInt32 iOldMaxY, FvInt32 iNewMinY, FvInt32 iNewMaxY)
		:m_pkRelaAndEvtAlloc(pkRelaAndEvtAlloc),m_pkObs(pkObs)
		,m_uiMask(uiMask)
		,m_iOldMinX(iOldMinX),m_iOldMaxX(iOldMaxX)
		,m_iNewMinX(iNewMinX),m_iNewMaxX(iNewMaxX)
		,m_iOldMinY(iOldMinY),m_iOldMaxY(iOldMaxY)
		,m_iNewMinY(iNewMinY),m_iNewMaxY(iNewMaxY)
	{
	}

#define _CHECK_IN(_T)											\
	if(pkPos->IsPt() &&											\
		m_uiMask & pkPos->m_pkOwner->m_uiMask &&				\
		m_iNewMin##_T < pkPos->m_i##_T &&						\
		pkPos->m_i##_T < m_iNewMax##_T &&						\
		pkPos->m_pkOwner != m_pkObs->m_pkOwner)					\
	{															\
		FvAoIEvt* pkEvt = (FvAoIEvt*)m_pkRelaAndEvtAlloc->Pop();\
		pkEvt->InitIn(m_pkObs, (FvAoIObject*)pkPos->m_pkOwner);	\
		m_pkObs->AddEvt(pkEvt->m_kObsNode);						\
		pkPos->m_pkOwner->AddEvt(pkEvt->m_kObjNode);			\
	}

#define _CHECK_OUT(_T)											\
	if(pkPos->IsPt() &&											\
		m_uiMask & pkPos->m_pkOwner->m_uiMask &&				\
		m_iOldMin##_T <= pkPos->m_i##_T &&						\
		pkPos->m_i##_T <= m_iOldMax##_T &&						\
		pkPos->m_pkOwner != m_pkObs->m_pkOwner)					\
	{															\
		FvAoIEvt* pkEvt = (FvAoIEvt*)m_pkRelaAndEvtAlloc->Pop();\
		pkEvt->InitOut(m_pkObs, (FvAoIObject*)pkPos->m_pkOwner);\
		m_pkObs->AddEvt(pkEvt->m_kObsNode);						\
		pkPos->m_pkOwner->AddEvt(pkEvt->m_kObjNode);			\
	}

	void UpMinXCheck(FvAoIPos* pkPos)
	{
		_CHECK_OUT(Y)
	}

	void UpMaxXCheck(FvAoIPos* pkPos)
	{
		_CHECK_IN(Y)
	}

	void DownMinXCheck(FvAoIPos* pkPos)
	{
		_CHECK_IN(Y)
	}

	void DownMaxXCheck(FvAoIPos* pkPos)
	{
		_CHECK_OUT(Y)
	}

	void UpMinYCheck(FvAoIPos* pkPos)
	{
		_CHECK_OUT(X)
	}

	void UpMaxYCheck(FvAoIPos* pkPos)
	{
		_CHECK_IN(X)
	}

	void DownMinYCheck(FvAoIPos* pkPos)
	{
		_CHECK_IN(X)
	}

	void DownMaxYCheck(FvAoIPos* pkPos)
	{
		_CHECK_OUT(X)
	}

#undef _CHECK_IN
#undef _CHECK_OUT

protected:
	FvUInt16		m_uiMask;
	FvInt32			m_iOldMinX, m_iOldMaxX;
	FvInt32			m_iNewMinX, m_iNewMaxX;
	FvInt32			m_iOldMinY, m_iOldMaxY;
	FvInt32			m_iNewMinY, m_iNewMaxY;
	FvAoIObserver*	m_pkObs;
	FvAoIMemMgr*	m_pkRelaAndEvtAlloc;
};

template<typename TAoIExt, typename TTrapExt>
bool FvAoIMgr<TAoIExt, TTrapExt>::SetVision(FvAoIHandle hHandle, float fVision)
{
	FV_ASSERT(hHandle && hHandle->IsObserver());

	if(fVision < .0f)
		fVision = .0f;

	FvAoIObserver* pkObs = (FvAoIObserver*)hHandle;
	FvInt32 iNewVision = TransPosAndScale(fVision);
	if(iNewVision == pkObs->m_iVision)
		return true;

	FvAoIPos* pkMinPos = pkObs->m_pkPosMin;
	FvAoIPos* pkMaxPos = pkObs->m_pkPosMax;
	FvInt32 iOldMinX = pkMinPos->m_iX;
	FvInt32 iOldMaxX = pkMaxPos->m_iX;
	FvInt32 iOldMinY = pkMinPos->m_iY;
	FvInt32 iOldMaxY = pkMaxPos->m_iY;
	FvInt32 iX = iOldMinX + pkObs->m_iVision;
	FvInt32 iY = iOldMinY + pkObs->m_iVision;
	FvInt32 iNewMinX = iX - iNewVision;
	FvInt32 iNewMaxX = iX + iNewVision;
	FvInt32 iNewMinY = iY - iNewVision;
	FvInt32 iNewMaxY = iY + iNewVision;
	pkMinPos->SetPos(iX - iNewVision, iNewMinY);
	pkMaxPos->SetPos(iX + iNewVision, iNewMaxY);

	CheckerForSetVision kChecker(&m_kRelaAndEvtAlloc, pkObs,
		pkObs->m_uiMask,
		iOldMinX, iOldMaxX,
		iNewMinX, iNewMaxX,
		iOldMinY, iOldMaxY,
		iNewMinY, iNewMaxY);

	bool bBigger = pkObs->m_iVision < iNewVision ? true : false;
	pkObs->m_iVision = iNewVision;

	if(bBigger)
	{
		pkMaxPos->DrawOutX();
		InsPosRightOnXAndCheck<CheckerForSetVision, &CheckerForSetVision::UpMaxXCheck>(pkMaxPos->m_pkPreX, pkMaxPos, &kChecker);
		pkMinPos->DrawOutX();
		InsPosLeftOnXAndCheck<CheckerForSetVision, &CheckerForSetVision::DownMinXCheck>(pkMinPos->m_pkNexX, pkMinPos, &kChecker);

		pkMaxPos->DrawOutY();
		InsPosRightOnYAndCheck<CheckerForSetVision, &CheckerForSetVision::UpMaxYCheck>(pkMaxPos->m_pkPreY, pkMaxPos, &kChecker);
		pkMinPos->DrawOutY();
		InsPosLeftOnYAndCheck<CheckerForSetVision, &CheckerForSetVision::DownMinYCheck>(pkMinPos->m_pkNexY, pkMinPos, &kChecker);
	}
	else
	{
		pkMaxPos->DrawOutX();
		InsPosLeftOnXAndCheck<CheckerForSetVision, &CheckerForSetVision::DownMaxXCheck>(pkMaxPos->m_pkNexX, pkMaxPos, &kChecker);
		pkMinPos->DrawOutX();
		InsPosRightOnXAndCheck<CheckerForSetVision, &CheckerForSetVision::UpMinXCheck>(pkMinPos->m_pkPreX, pkMinPos, &kChecker);

		pkMaxPos->DrawOutY();
		InsPosLeftOnYAndCheck<CheckerForSetVision, &CheckerForSetVision::DownMaxYCheck>(pkMaxPos->m_pkNexY, pkMaxPos, &kChecker);
		pkMinPos->DrawOutY();
		InsPosRightOnYAndCheck<CheckerForSetVision, &CheckerForSetVision::UpMinYCheck>(pkMinPos->m_pkPreY, pkMinPos, &kChecker);
	}

#ifdef FV_DEBUG
	if(1)
	{
		FvAoIPos* pkPos(NULL);
		pkPos = &m_kMinPos;
		while(pkPos != &m_kMaxPos)
		{
			if(pkPos->m_iX > pkPos->m_pkNexX->m_iX)
			{
				//! 检测坐标有序
				FV_ASSERT(0);
			}
			pkPos = pkPos->m_pkNexX;
		}
		pkPos = &m_kMinPos;
		while(pkPos != &m_kMaxPos)
		{
			if(pkPos->m_iY > pkPos->m_pkNexY->m_iY)
			{
				//! 检测坐标有序
				FV_ASSERT(0);
			}
			pkPos = pkPos->m_pkNexY;
		}
	}
#endif

	return true;
}

template<typename TAoIExt, typename TTrapExt>
bool FvAoIMgr<TAoIExt, TTrapExt>::SetDisVisibility(FvAoIHandle hHandle, float fDisVisibility)
{
	FV_ASSERT(hHandle && hHandle->IsObserver());
	((FvAoIObserver*)hHandle)->m_iDisVisibility = TransPosAndScale(fDisVisibility);
	return true;
}

template<typename TAoIExt, typename TTrapExt>
FvUInt16 FvAoIMgr<TAoIExt, TTrapExt>::GetMask(FvAoIHandle hHandle)
{
	FV_ASSERT(hHandle);
	return hHandle->m_uiMask;
}

template<typename TAoIExt, typename TTrapExt>
float FvAoIMgr<TAoIExt, TTrapExt>::GetVisibility(FvAoIHandle hHandle)
{
	FV_ASSERT(hHandle && hHandle->IsObject());
	return ((FvAoIObject*)hHandle)->m_iVisibility * m_fCoorScaleInv;
}

template<typename TAoIExt, typename TTrapExt>
float FvAoIMgr<TAoIExt, TTrapExt>::GetVision(FvAoIHandle hHandle)
{
	FV_ASSERT(hHandle && hHandle->IsObserver());
	return ((FvAoIObserver*)hHandle)->m_iVision * m_fCoorScaleInv;
}

template<typename TAoIExt, typename TTrapExt>
float FvAoIMgr<TAoIExt, TTrapExt>::GetDisVisibility(FvAoIHandle hHandle)
{
	FV_ASSERT(hHandle && hHandle->IsObserver());
	return ((FvAoIObserver*)hHandle)->m_iDisVisibility * m_fCoorScaleInv;
}

template<typename TAoIExt, typename TTrapExt>
FvUInt16 FvAoIMgr<TAoIExt, TTrapExt>::GetObjCntInVision(FvAoIHandle hHandle)
{
	FV_ASSERT(hHandle && hHandle->IsObserver());
	return ((FvAoIObserver*)hHandle)->ObjCnt();
}

template<typename TAoIExt, typename TTrapExt>
FvUInt16 FvAoIMgr<TAoIExt, TTrapExt>::GetObsCntInDVision(FvAoIHandle hHandle)
{
	FV_ASSERT(hHandle && hHandle->IsObject());
	return ((FvAoIObject*)hHandle)->ObsCnt();
}

#ifndef FV_DEBUG
template<typename TAoIExt, typename TTrapExt>
void FvAoIMgr<TAoIExt, TTrapExt>::Move(FvAoIHandle hHandle, float fX, float fY)
{
	FV_ASSERT(hHandle);

	if(hHandle->IsObject())
	{
		MoveObject((FvAoIObject*)hHandle, fX, fY);
	}
	else
	{
		MoveObserver((FvAoIObserver*)hHandle, fX, fY);
	}
}
#else
template<typename TAoIExt, typename TTrapExt>
FvUInt64 FvAoIMgr<TAoIExt, TTrapExt>::Move(FvAoIHandle hHandle, float fX, float fY)
{
	FV_ASSERT(hHandle);

	if(hHandle->IsObject())
	{
		return MoveObject((FvAoIObject*)hHandle, fX, fY);
	}
	else
	{
		return MoveObserver((FvAoIObserver*)hHandle, fX, fY);
	}
}
#endif


template<typename TAoIExt, typename TTrapExt>
template<typename TListener,
void (TListener::*OnEnter)(void* pkObsData, FvAoIExt* pkExt),
void (TListener::*OnStand)(void* pkObsData, FvAoIExt* pkExt),
void (TListener::*OnLeave)(void* pkObsData, FvAoIExt* pkExt)>
void FvAoIMgr<TAoIExt, TTrapExt>::UpdateAoI(FvAoIHandle hHandle, TListener* pkListener
#ifndef FV_DEBUG
											)
#else
										  , FvUInt64& uiEvtCnt, FvUInt64& uiRelateCnt, FvUInt64& uiIncreCnt)
#endif
{
	FV_ASSERT(hHandle && hHandle->IsObserver() && hHandle->IsAoI());

	FvAoIObserver* pkObs = (FvAoIObserver*)hHandle;
	void* pkObsData = pkObs->m_pkUserData;

	++m_uiDummy;
	FV_ASSERT(m_uiDummy < 0xFFFFFFFFFFFFFFFF);
	m_kEvtCache.Reset(m_uiDummy);

	FvAoIDLNode* pkBeginNode(NULL);
	FvAoIDLNode* pkEndNode(NULL);

	pkBeginNode = pkObs->m_kEvtMin.m_pkNex;
	pkEndNode = &pkObs->m_kEvtMax;
	while(pkBeginNode != pkEndNode)
	{
#ifdef FV_DEBUG
		++uiEvtCnt;
#endif
		FvAoIDLNode* pkNode = pkBeginNode;
		pkBeginNode = pkBeginNode->m_pkNex;

		FvAoIEvt* pkEvt = AOIEVTOBS(pkNode);
		if(m_kEvtCache.AddEvt(pkEvt, true))
		{
			pkEvt->m_kObjNode.DrawOut();
		}
		else
		{
			//! leave
			FV_ASSERT(pkEvt->IsDel() && pkEvt->m_pkExt);
			pkObs->DelObj();
			((*pkListener).*OnLeave)(pkObsData, pkEvt->m_pkExt);
			DestroyAoIExt(pkEvt->m_pkExt);
		}
		m_kRelaAndEvtAlloc.Push(pkEvt);
	}
	pkObs->m_kEvtMin.m_pkNex = &pkObs->m_kEvtMax;
	pkObs->m_kEvtMax.m_pkPre = &pkObs->m_kEvtMin;

	FvAoIRelate kMinRelate, kMaxRelate;
	FvAoIExt kMinExt, kMaxExt;
	kMinExt.m_fDisSqr = -1.0f;
	kMaxExt.m_fDisSqr = FV_MAXFLOAT_F;
	kMinRelate.m_pkExt = &kMinExt;
	kMaxRelate.m_pkExt = &kMaxExt;
	kMinRelate.m_kObsNode.m_pkNex = &kMaxRelate.m_kObsNode;
	kMaxRelate.m_kObsNode.m_pkPre = &kMinRelate.m_kObsNode;
	bool bHasObj(false);

	FvInt32 iX = pkObs->m_pkPosMin->m_iX + pkObs->m_iVision;
	FvInt32 iY = pkObs->m_pkPosMin->m_iY + pkObs->m_iVision;

	pkBeginNode = pkObs->m_kRelateMin.m_pkNex;
	pkEndNode = &pkObs->m_kRelateMax;
	while(pkBeginNode != pkEndNode)
	{
#ifdef FV_DEBUG
		++uiRelateCnt;
#endif
		FvAoIDLNode* pkNode = pkBeginNode;
		pkBeginNode = pkBeginNode->m_pkNex;

		FvAoIRelate* pkRelate = AOIRELATEOBS(pkNode);
		FV_ASSERT(pkRelate->m_pkObj->m_uiDummy <= m_uiDummy);
		FvAoIObject* pkObj = pkRelate->m_pkObj;

		if(pkObj->m_uiDummy == m_uiDummy && pkObj->NeedDel())
		{
			//! leave
			if(pkRelate->m_pkExt)
			{
				pkObs->DelObj();
				pkObj->DelObs();
				((*pkListener).*OnLeave)(pkObsData, pkRelate->m_pkExt);
				DestroyAoIExt(pkRelate->m_pkExt);
			}

			pkRelate->m_kObsNode.DrawOut();
			pkRelate->m_kObjNode.DrawOut();
			m_kRelaAndEvtAlloc.Push(pkRelate);
		}
		else if(!pkObj->IsRefPt())
		{
#ifdef FV_DEBUG
			if(pkObj->m_pkPos->m_iX < pkObs->m_pkPosMin->m_iX ||
				pkObs->m_pkPosMax->m_iX < pkObj->m_pkPos->m_iX ||
				pkObj->m_pkPos->m_iY < pkObs->m_pkPosMin->m_iY ||
				pkObs->m_pkPosMax->m_iY < pkObj->m_pkPos->m_iY)
			{
				//! 检测点是否在Box里面
				FV_ASSERT(0);
			}
#endif

			FvInt32 i1 = pkObj->m_iVisibility + pkObs->m_iDisVisibility;
			float f2 = float(pkObs->m_iVision > i1 ? i1 : pkObs->m_iVision);
			float fV = f2 * f2;
			float f3 = float(iX - pkObj->m_pkPos->m_iX);
			float f4 = float(iY - pkObj->m_pkPos->m_iY);
			float fD = f3 * f3 + f4 * f4;

			FvUInt8 uiTmp = pkRelate->m_pkExt ? 0x02 : 0x00;
			if(fV > fD) uiTmp = uiTmp | 0x01;

			switch(uiTmp)
			{
			case 1://! enter
				{
					pkObs->AddObj();
					pkObj->AddObs();
					pkRelate->m_pkExt = CreateAoIExt(pkObsData, pkObj->m_pkUserData);
					((*pkListener).*OnEnter)(pkObsData, pkRelate->m_pkExt);
					pkRelate->m_pkExt->m_fDisSqr = fD;

					bHasObj = true;
					pkRelate->m_kObsNode.DrawOut();
					FvAoIDLNode* pkIns = kMaxRelate.m_kObsNode.m_pkPre;
					while(AOIRELATEOBS(pkRelate)->m_pkExt->m_fDisSqr < AOIRELATEOBS(pkIns)->m_pkExt->m_fDisSqr)
						pkIns = pkIns->m_pkPre;
					pkRelate->m_kObsNode.m_pkPre = pkIns;
					pkRelate->m_kObsNode.m_pkNex = pkIns->m_pkNex;
					pkIns->m_pkNex->m_pkPre = &pkRelate->m_kObsNode;
					pkIns->m_pkNex = &pkRelate->m_kObsNode;

					//! TODO: 当需要频繁查询被视野时,需要打开以下代码,把被视野节点提前到Head
					//pkRelate->m_kObjNode.DrawOut();
					//pkObj->AddRelateBegin(pkRelate->m_kObjNode);
				}
				break;
			case 2://! leave
				{
					pkObs->DelObj();
					pkObj->DelObs();
					((*pkListener).*OnLeave)(pkObsData, pkRelate->m_pkExt);
					DestroyAoIExt(pkRelate->m_pkExt);
					pkRelate->m_pkExt = NULL;

					//! TODO: 当需要频繁查询被视野时,需要打开以下代码,把被视野节点提前到Head
					//pkRelate->m_kObjNode.DrawOut();
					//pkObj->AddRelateEnd(pkRelate->m_kObjNode);
				}
				break;
			case 3://! stand
				{
					((*pkListener).*OnStand)(pkObsData, pkRelate->m_pkExt);
					pkRelate->m_pkExt->m_fDisSqr = fD;

					bHasObj = true;
					pkRelate->m_kObsNode.DrawOut();
					FvAoIDLNode* pkIns = kMaxRelate.m_kObsNode.m_pkPre;
					while(AOIRELATEOBS(pkRelate)->m_pkExt->m_fDisSqr < AOIRELATEOBS(pkIns)->m_pkExt->m_fDisSqr)
						pkIns = pkIns->m_pkPre;
					pkRelate->m_kObsNode.m_pkPre = pkIns;
					pkRelate->m_kObsNode.m_pkNex = pkIns->m_pkNex;
					pkIns->m_pkNex->m_pkPre = &pkRelate->m_kObsNode;
					pkIns->m_pkNex = &pkRelate->m_kObsNode;

					//! TODO: 当需要频繁查询被视野时,需要打开以下代码,把被视野节点提前到Head
					//pkRelate->m_kObjNode.DrawOut();
					//pkObj->AddRelateBegin(pkRelate->m_kObjNode);
				}
				break;
			}
		}
	}

	FvUInt32 uiCacheEvts(0);
	FvAoIBase** ppkCache = m_kEvtCache.GetCache(uiCacheEvts);
	for(FvUInt32 i=0; i<uiCacheEvts; ++i)
	{
#ifdef FV_DEBUG
		++uiIncreCnt;
#endif
		FvAoIObject* pkObj = (FvAoIObject*)ppkCache[i];
		if(pkObj->NeedAdd())
		{
#ifdef FV_DEBUG
			if(pkObj->m_pkPos->m_iX < pkObs->m_pkPosMin->m_iX ||
				pkObs->m_pkPosMax->m_iX < pkObj->m_pkPos->m_iX ||
				pkObj->m_pkPos->m_iY < pkObs->m_pkPosMin->m_iY ||
				pkObs->m_pkPosMax->m_iY < pkObj->m_pkPos->m_iY)
			{
				//! 检测点是否在Box里面
				FV_ASSERT(0);
			}
#endif

			FvAoIRelate* pkRelate = (FvAoIRelate*)m_kRelaAndEvtAlloc.Pop();
			pkRelate->Init(pkObs, pkObj);

			if(!pkObj->IsRefPt())
			{
				FvInt32 i1 = pkObj->m_iVisibility + pkObs->m_iDisVisibility;
				float f2 = float(pkObs->m_iVision > i1 ? i1 : pkObs->m_iVision);
				float fV = f2 * f2;
				float f3 = float(iX - pkObj->m_pkPos->m_iX);
				float f4 = float(iY - pkObj->m_pkPos->m_iY);
				float fD = f3 * f3 + f4 * f4;

				//! enter
				if(fV > fD)
				{
					pkObs->AddObj();
					pkObj->AddObs();
					pkRelate->m_pkExt = CreateAoIExt(pkObsData, pkObj->m_pkUserData);
					((*pkListener).*OnEnter)(pkObsData, pkRelate->m_pkExt);
					pkRelate->m_pkExt->m_fDisSqr = fD;

					bHasObj = true;
					FvAoIDLNode* pkIns = kMaxRelate.m_kObsNode.m_pkPre;
					while(AOIRELATEOBS(pkRelate)->m_pkExt->m_fDisSqr < AOIRELATEOBS(pkIns)->m_pkExt->m_fDisSqr)
						pkIns = pkIns->m_pkPre;
					pkRelate->m_kObsNode.m_pkPre = pkIns;
					pkRelate->m_kObsNode.m_pkNex = pkIns->m_pkNex;
					pkIns->m_pkNex->m_pkPre = &pkRelate->m_kObsNode;
					pkIns->m_pkNex = &pkRelate->m_kObsNode;

					pkObj->AddRelateBegin(pkRelate->m_kObjNode);
				}
				else
				{
					pkObs->AddRelateEnd(pkRelate->m_kObsNode);
					pkObj->AddRelateEnd(pkRelate->m_kObjNode);
				}
			}
			else
			{
				pkObs->AddRelateEnd(pkRelate->m_kObsNode);
				pkObj->AddRelateEnd(pkRelate->m_kObjNode);
			}
		}
	}

	if(bHasObj)
	{
		kMinRelate.m_kObsNode.m_pkNex->m_pkPre = &pkObs->m_kRelateMin;
		kMaxRelate.m_kObsNode.m_pkPre->m_pkNex = pkObs->m_kRelateMin.m_pkNex;
		pkObs->m_kRelateMin.m_pkNex->m_pkPre = kMaxRelate.m_kObsNode.m_pkPre;
		pkObs->m_kRelateMin.m_pkNex = kMinRelate.m_kObsNode.m_pkNex;
	}
}

template<typename TAoIExt, typename TTrapExt>
template<typename TListener,
void (TListener::*OnEnter)(void* pkObsData, FvAoIExt* pkExt),
void (TListener::*OnLeave)(void* pkObsData, FvAoIExt* pkExt)>
void FvAoIMgr<TAoIExt, TTrapExt>::UpdateTrap(FvAoIHandle hHandle, TListener* pkListener
#ifndef FV_DEBUG
											 )
#else
											 , FvUInt64& uiEvtCnt, FvUInt64& uiRelateCnt, FvUInt64& uiIncreCnt)
#endif
{
	FV_ASSERT(hHandle && hHandle->IsObserver() && hHandle->IsTrap());

	FvAoIObserver* pkObs = (FvAoIObserver*)hHandle;
	void* pkObsData = pkObs->m_pkUserData;

	++m_uiDummy;
	FV_ASSERT(m_uiDummy < 0xFFFFFFFFFFFFFFFF);
	m_kEvtCache.Reset(m_uiDummy);

	FvAoIDLNode* pkBeginNode(NULL);
	FvAoIDLNode* pkEndNode(NULL);

	pkBeginNode = pkObs->m_kEvtMin.m_pkNex;
	pkEndNode = &pkObs->m_kEvtMax;
	while(pkBeginNode != pkEndNode)
	{
#ifdef FV_DEBUG
		++uiEvtCnt;
#endif
		FvAoIDLNode* pkNode = pkBeginNode;
		pkBeginNode = pkBeginNode->m_pkNex;

		FvAoIEvt* pkEvt = AOIEVTOBS(pkNode);
		if(m_kEvtCache.AddEvt(pkEvt, true))
		{
			pkEvt->m_kObjNode.DrawOut();
		}
		else
		{
			//! leave
			FV_ASSERT(pkEvt->IsDel() && pkEvt->m_pkExt);
			pkObs->DelObj();
			((*pkListener).*OnLeave)(pkObsData, pkEvt->m_pkExt);
			DestroyTrapExt(pkEvt->m_pkExt);
		}
		m_kRelaAndEvtAlloc.Push(pkEvt);
	}
	pkObs->m_kEvtMin.m_pkNex = &pkObs->m_kEvtMax;
	pkObs->m_kEvtMax.m_pkPre = &pkObs->m_kEvtMin;

	FvInt32 iX = pkObs->m_pkPosMin->m_iX + pkObs->m_iVision;
	FvInt32 iY = pkObs->m_pkPosMin->m_iY + pkObs->m_iVision;

	pkBeginNode = pkObs->m_kRelateMin.m_pkNex;
	pkEndNode = &pkObs->m_kRelateMax;
	while(pkBeginNode != pkEndNode)
	{
#ifdef FV_DEBUG
		++uiRelateCnt;
#endif
		FvAoIDLNode* pkNode = pkBeginNode;
		pkBeginNode = pkBeginNode->m_pkNex;

		FvAoIRelate* pkRelate = AOIRELATEOBS(pkNode);
		FV_ASSERT(pkRelate->m_pkObj->m_uiDummy <= m_uiDummy);
		FvAoIObject* pkObj = pkRelate->m_pkObj;

		if(pkObj->m_uiDummy == m_uiDummy && pkObj->NeedDel())
		{
			//! leave
			if(pkRelate->m_pkExt)
			{
				pkObs->DelObj();
				pkObj->DelObs();
				((*pkListener).*OnLeave)(pkObsData, pkRelate->m_pkExt);
				DestroyTrapExt(pkRelate->m_pkExt);
			}

			pkRelate->m_kObsNode.DrawOut();
			pkRelate->m_kObjNode.DrawOut();
			m_kRelaAndEvtAlloc.Push(pkRelate);
		}
		else if(!pkObj->IsRefPt())
		{
#ifdef FV_DEBUG
			if(pkObj->m_pkPos->m_iX < pkObs->m_pkPosMin->m_iX ||
				pkObs->m_pkPosMax->m_iX < pkObj->m_pkPos->m_iX ||
				pkObj->m_pkPos->m_iY < pkObs->m_pkPosMin->m_iY ||
				pkObs->m_pkPosMax->m_iY < pkObj->m_pkPos->m_iY)
			{
				//! 检测点是否在Box里面
				FV_ASSERT(0);
			}
#endif

			FvInt32 i1 = pkObj->m_iVisibility + pkObs->m_iDisVisibility;
			float f2 = float(pkObs->m_iVision > i1 ? i1 : pkObs->m_iVision);
			float fV = f2 * f2;
			float f3 = float(iX - pkObj->m_pkPos->m_iX);
			float f4 = float(iY - pkObj->m_pkPos->m_iY);
			float fD = f3 * f3 + f4 * f4;

			FvUInt8 uiTmp = pkRelate->m_pkExt ? 0x02 : 0x00;
			if(fV > fD) uiTmp = uiTmp | 0x01;

			switch(uiTmp)
			{
			case 1://! enter
				{
					pkObs->AddObj();
					pkObj->AddObs();
					pkRelate->m_pkExt = CreateTrapExt(pkObsData, pkObj->m_pkUserData);
					((*pkListener).*OnEnter)(pkObsData, pkRelate->m_pkExt);
					pkRelate->m_pkExt->m_fDisSqr = fD;

					pkRelate->m_kObsNode.DrawOut();
					pkObs->AddRelateBegin(pkRelate->m_kObsNode);

					//! TODO: 当需要频繁查询被视野时,需要打开以下代码,把被视野节点提前到Head
					//pkRelate->m_kObjNode.DrawOut();
					//pkObj->AddRelateBegin(pkRelate->m_kObjNode);
				}
				break;
			case 2://! leave
				{
					pkObs->DelObj();
					pkObj->DelObs();
					((*pkListener).*OnLeave)(pkObsData, pkRelate->m_pkExt);
					DestroyTrapExt(pkRelate->m_pkExt);
					pkRelate->m_pkExt = NULL;

					pkRelate->m_kObsNode.DrawOut();
					pkObs->AddRelateEnd(pkRelate->m_kObsNode);

					//! TODO: 当需要频繁查询被视野时,需要打开以下代码,把被视野节点提前到Head
					//pkRelate->m_kObjNode.DrawOut();
					//pkObj->AddRelateEnd(pkRelate->m_kObjNode);
				}
				break;
			case 3://! stand
				{
					pkRelate->m_pkExt->m_fDisSqr = fD;

					pkRelate->m_kObsNode.DrawOut();
					pkObs->AddRelateBegin(pkRelate->m_kObsNode);

					//! TODO: 当需要频繁查询被视野时,需要打开以下代码,把被视野节点提前到Head
					//pkRelate->m_kObjNode.DrawOut();
					//pkObj->AddRelateBegin(pkRelate->m_kObjNode);
				}
				break;
			}
		}
	}

	FvUInt32 uiCacheEvts(0);
	FvAoIBase** ppkCache = m_kEvtCache.GetCache(uiCacheEvts);
	for(FvUInt32 i=0; i<uiCacheEvts; ++i)
	{
#ifdef FV_DEBUG
		++uiIncreCnt;
#endif
		FvAoIObject* pkObj = (FvAoIObject*)ppkCache[i];
		if(pkObj->NeedAdd())
		{
#ifdef FV_DEBUG
			if(pkObj->m_pkPos->m_iX < pkObs->m_pkPosMin->m_iX ||
				pkObs->m_pkPosMax->m_iX < pkObj->m_pkPos->m_iX ||
				pkObj->m_pkPos->m_iY < pkObs->m_pkPosMin->m_iY ||
				pkObs->m_pkPosMax->m_iY < pkObj->m_pkPos->m_iY)
			{
				//! 检测点是否在Box里面
				FV_ASSERT(0);
			}
#endif

			FvAoIRelate* pkRelate = (FvAoIRelate*)m_kRelaAndEvtAlloc.Pop();
			pkRelate->Init(pkObs, pkObj);

			if(!pkObj->IsRefPt())
			{
				FvInt32 i1 = pkObj->m_iVisibility + pkObs->m_iDisVisibility;
				float f2 = float(pkObs->m_iVision > i1 ? i1 : pkObs->m_iVision);
				float fV = f2 * f2;
				float f3 = float(iX - pkObj->m_pkPos->m_iX);
				float f4 = float(iY - pkObj->m_pkPos->m_iY);
				float fD = f3 * f3 + f4 * f4;

				//! enter
				if(fV > fD)
				{
					pkObs->AddObj();
					pkObj->AddObs();
					pkRelate->m_pkExt = CreateTrapExt(pkObsData, pkObj->m_pkUserData);
					((*pkListener).*OnEnter)(pkObsData, pkRelate->m_pkExt);
					pkRelate->m_pkExt->m_fDisSqr = fD;

					pkObs->AddRelateBegin(pkRelate->m_kObsNode);
					pkObj->AddRelateBegin(pkRelate->m_kObjNode);
				}
				else
				{
					pkObs->AddRelateEnd(pkRelate->m_kObsNode);
					pkObj->AddRelateEnd(pkRelate->m_kObjNode);
				}
			}
			else
			{
				pkObs->AddRelateEnd(pkRelate->m_kObsNode);
				pkObj->AddRelateEnd(pkRelate->m_kObjNode);
			}
		}
	}
}

class CheckerForMoveObj
{
public:
	CheckerForMoveObj(FvAoIMemMgr* pkRelaAndEvtAlloc, FvAoIObject* pkObj, FvUInt16 uiMask, FvInt32 iX, FvInt32 iY, FvInt32 iOldX, FvInt32 iOldY)
	:m_pkRelaAndEvtAlloc(pkRelaAndEvtAlloc),m_pkObj(pkObj),m_uiMask(uiMask),m_iX(iX),m_iY(iY),m_iOldX(iOldX),m_iOldY(iOldY)
	{
	}

#define _UP_MIN(_T)													\
	if(pkPos->IsBoxMin())											\
	{																\
		FvAoIObserver* pkObs = (FvAoIObserver*)(pkPos->m_pkOwner);	\
		if(m_uiMask & pkObs->m_uiMask &&							\
			pkPos->m_i##_T < m_i##_T &&								\
			m_iX < AOIOBS_MAX_X(pkObs) &&							\
			m_iY < AOIOBS_MAX_Y(pkObs) &&							\
			pkObs->m_pkOwner != m_pkObj)							\
		{															\
			FvAoIEvt* pkEvt = (FvAoIEvt*)m_pkRelaAndEvtAlloc->Pop();\
			pkEvt->InitIn(pkObs, m_pkObj);							\
			pkObs->AddEvt(pkEvt->m_kObsNode);						\
			m_pkObj->AddEvt(pkEvt->m_kObjNode);						\
		}															\
	}

#define _UP_MAX()													\
	else if(pkPos->IsBoxMax())										\
	{																\
		FvAoIObserver* pkObs = (FvAoIObserver*)(pkPos->m_pkOwner);	\
		if(m_uiMask & pkObs->m_uiMask &&							\
			m_iOldX <= pkPos->m_iX &&								\
			m_iOldY <= pkPos->m_iY &&								\
			AOIOBS_MIN_X(pkObs) <= m_iOldX &&						\
			AOIOBS_MIN_Y(pkObs) <= m_iOldY &&						\
			pkObs->m_pkOwner != m_pkObj)							\
		{															\
			FvAoIEvt* pkEvt = (FvAoIEvt*)m_pkRelaAndEvtAlloc->Pop();\
			pkEvt->InitOut(pkObs, m_pkObj);							\
			pkObs->AddEvt(pkEvt->m_kObsNode);						\
			m_pkObj->AddEvt(pkEvt->m_kObjNode);						\
		}															\
	}

#define _DOWN_MIN()													\
	if(pkPos->IsBoxMin())											\
	{																\
		FvAoIObserver* pkObs = (FvAoIObserver*)(pkPos->m_pkOwner);	\
		if(m_uiMask & pkObs->m_uiMask &&							\
			pkPos->m_iX <= m_iOldX &&								\
			pkPos->m_iY <= m_iOldY &&								\
			m_iOldX <= AOIOBS_MAX_X(pkObs) &&						\
			m_iOldY <= AOIOBS_MAX_Y(pkObs) &&						\
			pkObs->m_pkOwner != m_pkObj)							\
		{															\
			FvAoIEvt* pkEvt = (FvAoIEvt*)m_pkRelaAndEvtAlloc->Pop();\
			pkEvt->InitOut(pkObs, m_pkObj);							\
			pkObs->AddEvt(pkEvt->m_kObsNode);						\
			m_pkObj->AddEvt(pkEvt->m_kObjNode);						\
		}															\
	}

#define _DOWN_MAX(_T)												\
	else if(pkPos->IsBoxMax())										\
	{																\
		FvAoIObserver* pkObs = (FvAoIObserver*)(pkPos->m_pkOwner);	\
		if(m_uiMask & pkObs->m_uiMask &&							\
			m_i##_T < pkPos->m_i##_T &&								\
			AOIOBS_MIN_X(pkObs) < m_iX &&							\
			AOIOBS_MIN_Y(pkObs) < m_iY &&							\
			pkObs->m_pkOwner != m_pkObj)							\
		{															\
			FvAoIEvt* pkEvt = (FvAoIEvt*)m_pkRelaAndEvtAlloc->Pop();\
			pkEvt->InitIn(pkObs, m_pkObj);							\
			pkObs->AddEvt(pkEvt->m_kObsNode);						\
			m_pkObj->AddEvt(pkEvt->m_kObjNode);						\
		}															\
	}

	void UpXCheck(FvAoIPos* pkPos)
	{
		_UP_MIN(Y)
		_UP_MAX()
	}

	void DownXCheck(FvAoIPos* pkPos)
	{
		_DOWN_MIN()
		_DOWN_MAX(Y)
	}

	void UpYCheck(FvAoIPos* pkPos)
	{
		_UP_MIN(X)
		_UP_MAX()
	}

	void DownYCheck(FvAoIPos* pkPos)
	{
		_DOWN_MIN()
		_DOWN_MAX(X)
	}

#undef _UP_MIN
#undef _UP_MAX
#undef _DOWN_MIN
#undef _DOWN_MAX

protected:
	FvUInt16		m_uiMask;
	FvInt32			m_iX, m_iY;
	FvInt32			m_iOldX, m_iOldY;
	FvAoIMemMgr*	m_pkRelaAndEvtAlloc;
	FvAoIObject*	m_pkObj;
};

template<typename TAoIExt, typename TTrapExt>
#ifndef FV_DEBUG
void FvAoIMgr<TAoIExt, TTrapExt>::MoveObject(FvAoIObject* pkObj, float fX, float fY)
#else
FvUInt64 FvAoIMgr<TAoIExt, TTrapExt>::MoveObject(FvAoIObject* pkObj, float fX, float fY)
#endif
{
	FvAoIPos* pkPos = pkObj->m_pkPos;
	FvInt32 iOldX = pkPos->m_iX;
	FvInt32 iOldY = pkPos->m_iY;
	FvInt32 iNewX = TransPosAndScale(fX);
	FvInt32 iNewY = TransPosAndScale(fY);
	FvInt32 iDisX = iNewX - iOldX;
	FvInt32 iDisY = iNewY - iOldY;
	bool bUpX(true), bUpY(true);
	if(iDisX < 0)
	{
		bUpX = false;
		iDisX = -iDisX;
	}
	if(iDisY < 0)
	{
		bUpY = false;
		iDisY = -iDisY;
	}
	if(iDisX < m_iMinStep && iDisY < m_iMinStep)
#ifndef FV_DEBUG
		return;
#else
		return 0;
#endif

	pkPos->SetPos(iNewX, iNewY);

	m_kGrid.MoveObject(pkObj, iNewX, iNewY);

	pkPos->DrawOut();
	CheckerForMoveObj kChecker(&m_kRelaAndEvtAlloc, pkObj, pkObj->m_uiMask, iNewX, iNewY, iOldX, iOldY);

#ifndef FV_DEBUG
	if(bUpX)
		InsPosRightOnXAndCheck<CheckerForMoveObj, &CheckerForMoveObj::UpXCheck>(pkPos->m_pkPreX, pkPos, &kChecker);
	else
		InsPosLeftOnXAndCheck<CheckerForMoveObj, &CheckerForMoveObj::DownXCheck>(pkPos->m_pkNexX, pkPos, &kChecker);

	if(bUpY)
		InsPosRightOnYAndCheck<CheckerForMoveObj, &CheckerForMoveObj::UpYCheck>(pkPos->m_pkPreY, pkPos, &kChecker);
	else
		InsPosLeftOnYAndCheck<CheckerForMoveObj, &CheckerForMoveObj::DownYCheck>(pkPos->m_pkNexY, pkPos, &kChecker);
#else
	FvUInt64 cnt(0);
	if(bUpX)
		cnt += InsPosRightOnXAndCheck<CheckerForMoveObj, &CheckerForMoveObj::UpXCheck>(pkPos->m_pkPreX, pkPos, &kChecker);
	else
		cnt += InsPosLeftOnXAndCheck<CheckerForMoveObj, &CheckerForMoveObj::DownXCheck>(pkPos->m_pkNexX, pkPos, &kChecker);

	if(bUpY)
		cnt += InsPosRightOnYAndCheck<CheckerForMoveObj, &CheckerForMoveObj::UpYCheck>(pkPos->m_pkPreY, pkPos, &kChecker);
	else
		cnt += InsPosLeftOnYAndCheck<CheckerForMoveObj, &CheckerForMoveObj::DownYCheck>(pkPos->m_pkNexY, pkPos, &kChecker);
	return cnt;
#endif
}

class CheckerForMoveObs
{
public:
	CheckerForMoveObs(FvAoIMemMgr* pkRelaAndEvtAlloc, FvAoIObserver* pkObs,
		FvUInt16 uiMask,
		FvInt32 iOldMinX, FvInt32 iOldMinY, FvInt32 iOldMaxX, FvInt32 iOldMaxY,
		FvInt32 iNewMinX, FvInt32 iNewMinY, FvInt32 iNewMaxX, FvInt32 iNewMaxY)
		:m_pkRelaAndEvtAlloc(pkRelaAndEvtAlloc),m_pkObs(pkObs)
		,m_uiMask(uiMask)
		,m_iOldMinX(iOldMinX),m_iOldMinY(iOldMinY),m_iOldMaxX(iOldMaxX),m_iOldMaxY(iOldMaxY)
		,m_iNewMinX(iNewMinX),m_iNewMinY(iNewMinY),m_iNewMaxX(iNewMaxX),m_iNewMaxY(iNewMaxY)
	{
	}

#define _UP_CHECK_IN(_T, _S)									\
	if(pkPos->IsPt() &&											\
		m_uiMask & pkPos->m_pkOwner->m_uiMask &&				\
		m_iNewMin##_T < pkPos->m_i##_T &&						\
		m_iNewMin##_S < pkPos->m_i##_S &&						\
		pkPos->m_i##_S < m_iNewMax##_S &&						\
		pkPos->m_pkOwner != m_pkObs->m_pkOwner)					\
	{															\
		FvAoIEvt* pkEvt = (FvAoIEvt*)m_pkRelaAndEvtAlloc->Pop();\
		pkEvt->InitIn(m_pkObs, (FvAoIObject*)pkPos->m_pkOwner);	\
		m_pkObs->AddEvt(pkEvt->m_kObsNode);						\
		pkPos->m_pkOwner->AddEvt(pkEvt->m_kObjNode);			\
	}

#define _DOWN_CHECK_IN(_T, _S)									\
	if(pkPos->IsPt() &&											\
		m_uiMask & pkPos->m_pkOwner->m_uiMask &&				\
		pkPos->m_i##_T < m_iNewMax##_T &&						\
		m_iNewMin##_S < pkPos->m_i##_S &&						\
		pkPos->m_i##_S < m_iNewMax##_S &&						\
		pkPos->m_pkOwner != m_pkObs->m_pkOwner)					\
	{															\
		FvAoIEvt* pkEvt = (FvAoIEvt*)m_pkRelaAndEvtAlloc->Pop();\
		pkEvt->InitIn(m_pkObs, (FvAoIObject*)pkPos->m_pkOwner);	\
		m_pkObs->AddEvt(pkEvt->m_kObsNode);						\
		pkPos->m_pkOwner->AddEvt(pkEvt->m_kObjNode);			\
	}

#define _CHECK_OUT()											\
	if(pkPos->IsPt() &&											\
		m_uiMask & pkPos->m_pkOwner->m_uiMask &&				\
		m_iOldMinX <= pkPos->m_iX &&							\
		pkPos->m_iX <= m_iOldMaxX &&							\
		m_iOldMinY <= pkPos->m_iY &&							\
		pkPos->m_iY <= m_iOldMaxY &&							\
		pkPos->m_pkOwner != m_pkObs->m_pkOwner)					\
	{															\
		FvAoIEvt* pkEvt = (FvAoIEvt*)m_pkRelaAndEvtAlloc->Pop();\
		pkEvt->InitOut(m_pkObs, (FvAoIObject*)pkPos->m_pkOwner);\
		m_pkObs->AddEvt(pkEvt->m_kObsNode);						\
		pkPos->m_pkOwner->AddEvt(pkEvt->m_kObjNode);			\
	}

	void UpMinXCheck(FvAoIPos* pkPos)
	{
		_CHECK_OUT()
	}

	void UpMaxXCheck(FvAoIPos* pkPos)
	{
		_UP_CHECK_IN(X,Y)
	}

	void DownMinXCheck(FvAoIPos* pkPos)
	{
		_DOWN_CHECK_IN(X,Y)
	}

	void DownMaxXCheck(FvAoIPos* pkPos)
	{
		_CHECK_OUT()
	}

	void UpMinYCheck(FvAoIPos* pkPos)
	{
		_CHECK_OUT()
	}

	void UpMaxYCheck(FvAoIPos* pkPos)
	{
		_UP_CHECK_IN(Y,X)
	}

	void DownMinYCheck(FvAoIPos* pkPos)
	{
		_DOWN_CHECK_IN(Y,X)
	}

	void DownMaxYCheck(FvAoIPos* pkPos)
	{
		_CHECK_OUT()
	}

#undef _UP_CHECK_IN
#undef _DOWN_CHECK_IN
#undef _CHECK_OUT

protected:
	FvUInt16		m_uiMask;
	FvInt32			m_iOldMinX, m_iOldMinY, m_iOldMaxX, m_iOldMaxY;
	FvInt32			m_iNewMinX, m_iNewMinY, m_iNewMaxX, m_iNewMaxY;
	FvAoIObserver*	m_pkObs;
	FvAoIMemMgr*	m_pkRelaAndEvtAlloc;
};

template<typename TAoIExt, typename TTrapExt>
#ifndef FV_DEBUG
void FvAoIMgr<TAoIExt, TTrapExt>::MoveObserver(FvAoIObserver* pkObs, float fX, float fY)
#else
FvUInt64 FvAoIMgr<TAoIExt, TTrapExt>::MoveObserver(FvAoIObserver* pkObs, float fX, float fY)
#endif
{
	FvAoIPos* pkMinPos = pkObs->m_pkPosMin;
	FvAoIPos* pkMaxPos = pkObs->m_pkPosMax;
	FvInt32 iOldX = pkMinPos->m_iX + pkObs->m_iVision;
	FvInt32 iOldY = pkMinPos->m_iY + pkObs->m_iVision;
	FvInt32 iNewX = TransPosAndScale(fX);
	FvInt32 iNewY = TransPosAndScale(fY);
	FvInt32 iDisX = iNewX - iOldX;
	FvInt32 iDisY = iNewY - iOldY;
	bool bUpX(true), bUpY(true);
	if(iDisX < 0)
	{
		bUpX = false;
		iDisX = -iDisX;
	}
	if(iDisY < 0)
	{
		bUpY = false;
		iDisY = -iDisY;
	}
	if(iDisX < m_iMinStep && iDisY < m_iMinStep)
#ifndef FV_DEBUG
		return;
#else
		return 0;
#endif

	FvInt32 iOldMinX = pkMinPos->m_iX;
	FvInt32 iOldMinY = pkMinPos->m_iY;
	FvInt32 iOldMaxX = pkMaxPos->m_iX;
	FvInt32 iOldMaxY = pkMaxPos->m_iY;
	FvInt32 iNewMinX = iNewX - pkObs->m_iVision;
	FvInt32 iNewMinY = iNewY - pkObs->m_iVision;
	FvInt32 iNewMaxX = iNewX + pkObs->m_iVision;
	FvInt32 iNewMaxY = iNewY + pkObs->m_iVision;
	pkMinPos->SetPos(iNewMinX, iNewMinY);
	pkMaxPos->SetPos(iNewMaxX, iNewMaxY);

	CheckerForMoveObs kChecker(&m_kRelaAndEvtAlloc, pkObs,
		pkObs->m_uiMask,
		iOldMinX, iOldMinY, iOldMaxX, iOldMaxY,
		iNewMinX, iNewMinY, iNewMaxX, iNewMaxY);

#ifndef FV_DEBUG
	if(bUpX)
	{
		pkMaxPos->DrawOutX();
		InsPosRightOnXAndCheck<CheckerForMoveObs, &CheckerForMoveObs::UpMaxXCheck>(pkMaxPos->m_pkPreX, pkMaxPos, &kChecker);
		pkMinPos->DrawOutX();
		InsPosRightOnXAndCheck<CheckerForMoveObs, &CheckerForMoveObs::UpMinXCheck>(pkMinPos->m_pkPreX, pkMinPos, &kChecker);
	}
	else
	{
		pkMinPos->DrawOutX();
		InsPosLeftOnXAndCheck<CheckerForMoveObs, &CheckerForMoveObs::DownMinXCheck>(pkMinPos->m_pkNexX, pkMinPos, &kChecker);
		pkMaxPos->DrawOutX();
		InsPosLeftOnXAndCheck<CheckerForMoveObs, &CheckerForMoveObs::DownMaxXCheck>(pkMaxPos->m_pkNexX, pkMaxPos, &kChecker);
	}

	if(bUpY)
	{
		pkMaxPos->DrawOutY();
		InsPosRightOnYAndCheck<CheckerForMoveObs, &CheckerForMoveObs::UpMaxYCheck>(pkMaxPos->m_pkPreY, pkMaxPos, &kChecker);
		pkMinPos->DrawOutY();
		InsPosRightOnYAndCheck<CheckerForMoveObs, &CheckerForMoveObs::UpMinYCheck>(pkMinPos->m_pkPreY, pkMinPos, &kChecker);
	}
	else
	{
		pkMinPos->DrawOutY();
		InsPosLeftOnYAndCheck<CheckerForMoveObs, &CheckerForMoveObs::DownMinYCheck>(pkMinPos->m_pkNexY, pkMinPos, &kChecker);
		pkMaxPos->DrawOutY();
		InsPosLeftOnYAndCheck<CheckerForMoveObs, &CheckerForMoveObs::DownMaxYCheck>(pkMaxPos->m_pkNexY, pkMaxPos, &kChecker);
	}
#else
	FvUInt64 cnt(0);
	if(bUpX)
	{
		pkMaxPos->DrawOutX();
		cnt += InsPosRightOnXAndCheck<CheckerForMoveObs, &CheckerForMoveObs::UpMaxXCheck>(pkMaxPos->m_pkPreX, pkMaxPos, &kChecker);
		pkMinPos->DrawOutX();
		cnt += InsPosRightOnXAndCheck<CheckerForMoveObs, &CheckerForMoveObs::UpMinXCheck>(pkMinPos->m_pkPreX, pkMinPos, &kChecker);
	}
	else
	{
		pkMinPos->DrawOutX();
		cnt += InsPosLeftOnXAndCheck<CheckerForMoveObs, &CheckerForMoveObs::DownMinXCheck>(pkMinPos->m_pkNexX, pkMinPos, &kChecker);
		pkMaxPos->DrawOutX();
		cnt += InsPosLeftOnXAndCheck<CheckerForMoveObs, &CheckerForMoveObs::DownMaxXCheck>(pkMaxPos->m_pkNexX, pkMaxPos, &kChecker);
	}

	if(bUpY)
	{
		pkMaxPos->DrawOutY();
		cnt += InsPosRightOnYAndCheck<CheckerForMoveObs, &CheckerForMoveObs::UpMaxYCheck>(pkMaxPos->m_pkPreY, pkMaxPos, &kChecker);
		pkMinPos->DrawOutY();
		cnt += InsPosRightOnYAndCheck<CheckerForMoveObs, &CheckerForMoveObs::UpMinYCheck>(pkMinPos->m_pkPreY, pkMinPos, &kChecker);
	}
	else
	{
		pkMinPos->DrawOutY();
		cnt += InsPosLeftOnYAndCheck<CheckerForMoveObs, &CheckerForMoveObs::DownMinYCheck>(pkMinPos->m_pkNexY, pkMinPos, &kChecker);
		pkMaxPos->DrawOutY();
		cnt += InsPosLeftOnYAndCheck<CheckerForMoveObs, &CheckerForMoveObs::DownMaxYCheck>(pkMaxPos->m_pkNexY, pkMaxPos, &kChecker);
	}
	return cnt;
#endif
}

template<typename TAoIExt, typename TTrapExt>
FvAoIExt* FvAoIMgr<TAoIExt, TTrapExt>::CreateAoIExt(void* pkObsUserData, void* pkObjUserData)
{
	FvAoIExt* pkExt = (FvAoIExt*)m_kAoiExtAlloc.Pop();
	((TAoIExt*)pkExt)->Create(pkObsUserData, pkObjUserData);
	return pkExt;
}

template<typename TAoIExt, typename TTrapExt>
void FvAoIMgr<TAoIExt, TTrapExt>::DestroyAoIExt(FvAoIExt* pkExt)
{
	((TAoIExt*)pkExt)->Destroy();
	m_kAoiExtAlloc.Push(pkExt);
}

template<typename TAoIExt, typename TTrapExt>
FvAoIExt* FvAoIMgr<TAoIExt, TTrapExt>::CreateTrapExt(void* pkObsUserData, void* pkObjUserData)
{
	FvAoIExt* pkExt = (FvAoIExt*)m_kTrapExtAlloc.Pop();
	((TTrapExt*)pkExt)->Create(pkObsUserData, pkObjUserData);
	return pkExt;
}

template<typename TAoIExt, typename TTrapExt>
void FvAoIMgr<TAoIExt, TTrapExt>::DestroyTrapExt(FvAoIExt* pkExt)
{
	((TTrapExt*)pkExt)->Destroy();
	m_kTrapExtAlloc.Push(pkExt);
}

template<typename TAoIExt, typename TTrapExt>
FvInt32 FvAoIMgr<TAoIExt, TTrapExt>::TransPosAndScale(float fPos) const
{
	return RoundToInt(fPos * m_fCoorScale);
}

template<typename TAoIExt, typename TTrapExt>
void FvAoIMgr<TAoIExt, TTrapExt>::SetObjectMask(FvAoIObject* pkObj)
{
	FvAoIDLNode* pkBeginNode(NULL);
	FvAoIDLNode* pkEndNode(NULL);

	pkBeginNode = pkObj->m_kEvtMin.m_pkNex;
	pkEndNode = &pkObj->m_kEvtMax;
	while(pkBeginNode != pkEndNode)
	{
		FvAoIDLNode* pkNode = pkBeginNode;
		pkBeginNode = pkBeginNode->m_pkNex;

		FvAoIEvt* pkEvt = AOIEVTOBJ(pkNode);
		pkEvt->m_kObsNode.DrawOut();
		m_kRelaAndEvtAlloc.Push(pkEvt);
	}
	pkObj->m_kEvtMin.m_pkNex = &pkObj->m_kEvtMax;
	pkObj->m_kEvtMax.m_pkPre = &pkObj->m_kEvtMin;

	pkBeginNode = pkObj->m_kRelateMin.m_pkNex;
	pkEndNode = &pkObj->m_kRelateMax;
	while(pkBeginNode != pkEndNode)
	{
		FvAoIObserver* pkObs = AOIRELATEOBJ(pkBeginNode)->m_pkObs;
		FvAoIEvt* pkEvt = (FvAoIEvt*)m_kRelaAndEvtAlloc.Pop();
		pkEvt->InitOut(pkObs, pkObj);
		pkObs->AddEvt(pkEvt->m_kObsNode);
		pkObj->AddEvt(pkEvt->m_kObjNode);

		pkBeginNode = pkBeginNode->m_pkNex;
	}

	FvInt32 iX = pkObj->m_pkPos->m_iX;
	FvInt32 iY = pkObj->m_pkPos->m_iY;
	pkObj->m_pkPos->DrawOut();
	AddObjectToOrderedGrid(pkObj,
		float(iX) * m_fCoorScaleInv,
		float(iY) * m_fCoorScaleInv,
		iX, iY);
}

template<typename TAoIExt, typename TTrapExt>
void FvAoIMgr<TAoIExt, TTrapExt>::SetObserverMask(FvAoIObserver* pkObs)
{
	FvAoIDLNode* pkBeginNode(NULL);
	FvAoIDLNode* pkEndNode(NULL);

	pkBeginNode = pkObs->m_kEvtMin.m_pkNex;
	pkEndNode = &pkObs->m_kEvtMax;
	while(pkBeginNode != pkEndNode)
	{
		FvAoIDLNode* pkNode = pkBeginNode;
		pkBeginNode = pkBeginNode->m_pkNex;

		FvAoIEvt* pkEvt = AOIEVTOBS(pkNode);
		if(!pkEvt->IsDel())
		{
			pkEvt->m_kObsNode.DrawOut();
			pkEvt->m_kObjNode.DrawOut();
			m_kRelaAndEvtAlloc.Push(pkEvt);
		}
	}

	pkBeginNode = pkObs->m_kRelateMin.m_pkNex;
	pkEndNode = &pkObs->m_kRelateMax;
	while(pkBeginNode != pkEndNode)
	{
		FvAoIObject* pkObj = AOIRELATEOBS(pkBeginNode)->m_pkObj;
		FvAoIEvt* pkEvt = (FvAoIEvt*)m_kRelaAndEvtAlloc.Pop();
		pkEvt->InitOut(pkObs, pkObj);
		pkObs->AddEvt(pkEvt->m_kObsNode);
		pkObj->AddEvt(pkEvt->m_kObjNode);

		pkBeginNode = pkBeginNode->m_pkNex;
	}

	FvAoIPos* pkBeginPos = pkObs->m_pkPosMin->m_pkNexX;
	FvAoIPos* pkEndPos = pkObs->m_pkPosMax;
	FvUInt16 uiMask = pkObs->m_uiMask;
	FvInt32 iMinX = pkObs->m_pkPosMin->m_iX;
	FvInt32 iMinY = pkObs->m_pkPosMin->m_iY;
	FvInt32 iMaxX = pkObs->m_pkPosMax->m_iX;
	FvInt32 iMaxY = pkObs->m_pkPosMax->m_iY;
	while(pkBeginPos != pkEndPos)
	{
		if(pkBeginPos->IsPt() &&
			uiMask & pkBeginPos->m_pkOwner->m_uiMask &&
			iMinX < pkBeginPos->m_iX && pkBeginPos->m_iX < iMaxX &&
			iMinY < pkBeginPos->m_iY && pkBeginPos->m_iY < iMaxY &&
			pkBeginPos->m_pkOwner != pkObs->m_pkOwner)
		{
			FvAoIEvt* pkEvt = (FvAoIEvt*)m_kRelaAndEvtAlloc.Pop();
			pkEvt->InitIn(pkObs, (FvAoIObject*)pkBeginPos->m_pkOwner);
			pkObs->AddEvt(pkEvt->m_kObsNode);
			pkBeginPos->m_pkOwner->AddEvt(pkEvt->m_kObjNode);
		}

		pkBeginPos = pkBeginPos->m_pkNexX;
	}
}

template<typename TAoIExt, typename TTrapExt>
FvAoIHandle	FvAoIMgr<TAoIExt, TTrapExt>::AddObject(FvUInt16 uiMask, float fX, float fY, float fVisibility, void* pkData)
{
	FvInt32 iX = TransPosAndScale(fX);
	FvInt32 iY = TransPosAndScale(fY);
	FvInt32 iVisibility = TransPosAndScale(fVisibility);
	FV_ASSERT(FVAOI_MIN_POS < iX && iX < FVAOI_MAX_POS &&
		FVAOI_MIN_POS < iY && iY < FVAOI_MAX_POS);

	FvAoIObject* pkObj = (FvAoIObject*)m_kObjAndObsAlloc.Pop();
	pkObj->Init(iX, iY, iVisibility, uiMask, false, pkData, m_kPosAlloc.Pop());

	m_kGrid.AddObject(pkObj, iX, iY);
	AddObjectToOrderedGrid(pkObj, fX, fY, iX, iY);
	return pkObj;
}

template<typename TAoIExt, typename TTrapExt>
template<typename TVisiter,
void (TVisiter::*OnVisit)(void* pkObjData)>
FvUInt32 FvAoIMgr<TAoIExt, TTrapExt>::QueryVision(FvAoIHandle hHandle, TVisiter* pkVisiter) const
{
	FV_ASSERT(hHandle && hHandle->IsObserver());
	FvAoIObserver* pkObs = (FvAoIObserver*)hHandle;

	FvUInt32 uiCnt(0);
	FvAoIDLNode* pkBeginNode(NULL);
	FvAoIDLNode* pkEndNode(NULL);
	pkBeginNode = pkObs->m_kRelateMin.m_pkNex;
	pkEndNode = &pkObs->m_kRelateMax;
	while(pkBeginNode != pkEndNode)
	{
		FvAoIRelate* pkRelate = AOIRELATEOBS(pkBeginNode);
		if(pkRelate->m_pkExt)
		{
			++uiCnt;
			((*pkVisiter).*OnVisit)(pkRelate->m_pkObj->m_pkUserData);
		}
		else
		{
			break;
		}

		pkBeginNode = pkBeginNode->m_pkNex;
	}

	return uiCnt;
}

template<typename TAoIExt, typename TTrapExt>
template<typename TVisiter,
void (TVisiter::*OnVisit)(FvAoIExt* pkExt)>
FvUInt32 FvAoIMgr<TAoIExt, TTrapExt>::QueryVisionExt(FvAoIHandle hHandle, TVisiter* pkVisiter) const
{
	FV_ASSERT(hHandle && hHandle->IsObserver());
	FvAoIObserver* pkObs = (FvAoIObserver*)hHandle;

	FvUInt32 uiCnt(0);
	FvAoIDLNode* pkBeginNode(NULL);
	FvAoIDLNode* pkEndNode(NULL);
	pkBeginNode = pkObs->m_kRelateMin.m_pkNex;
	pkEndNode = &pkObs->m_kRelateMax;
	while(pkBeginNode != pkEndNode)
	{
		FvAoIRelate* pkRelate = AOIRELATEOBS(pkBeginNode);
		if(pkRelate->m_pkExt)
		{
			++uiCnt;
			((*pkVisiter).*OnVisit)(pkRelate->m_pkExt);
		}
		else
		{
			break;
		}

		pkBeginNode = pkBeginNode->m_pkNex;
	}

	return uiCnt;
}

template<typename TAoIExt, typename TTrapExt>
template<typename TVisiter,
void (TVisiter::*OnVisit)(void* pkObsData)>
FvUInt32 FvAoIMgr<TAoIExt, TTrapExt>::QueryDVision(FvAoIHandle hHandle, TVisiter* pkVisiter) const
{
	FV_ASSERT(hHandle && hHandle->IsObject());
	FvAoIObject* pkObj = (FvAoIObject*)hHandle;

	FvUInt32 uiCnt(0);
	FvAoIDLNode* pkBeginNode(NULL);
	FvAoIDLNode* pkEndNode(NULL);
	pkBeginNode = pkObj->m_kRelateMin.m_pkNex;
	pkEndNode = &pkObj->m_kRelateMax;
	while(pkBeginNode != pkEndNode)
	{
		FvAoIRelate* pkRelate = AOIRELATEOBJ(pkBeginNode);
		if(pkRelate->m_pkExt && pkRelate->m_pkObs->IsAoI())//! 只能是AoI,不要Trap
		{
			++uiCnt;
			((*pkVisiter).*OnVisit)(pkRelate->m_pkObs->m_pkUserData);
		}
		//! TODO: 当需要频繁查询被视野时,只需要遍历到m_pkExt为空就可以了

		pkBeginNode = pkBeginNode->m_pkNex;
	}

	return uiCnt;
}

template<typename TAoIExt, typename TTrapExt>
template<typename TVisiter,
void (TVisiter::*OnVisit)(void* pkObjData)>
FvUInt32 FvAoIMgr<TAoIExt, TTrapExt>::QueryArea(FvAoIHandle hHandle, float fX, float fY, float fRadius, TVisiter* pkVisiter) const
{
	FvInt32 iX = TransPosAndScale(fX);
	FvInt32 iY = TransPosAndScale(fY);
	FvInt32 iRadius = TransPosAndScale(fRadius);
	return m_kGrid.QueryArea<TVisiter, OnVisit>(hHandle, iX, iY, iRadius, pkVisiter);
}






