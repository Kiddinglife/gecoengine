//{future header message}
#ifndef __FvAoIObj_H__
#define __FvAoIObj_H__

#include "FvAoIUtility.h"

struct FvAoIBase;

//! 2*4 Byte
struct FvAoIDLNode
{
	FvAoIDLNode*	m_pkPre;
	FvAoIDLNode*	m_pkNex;

	void DrawOut()
	{
		m_pkPre->m_pkNex = m_pkNex;
		m_pkNex->m_pkPre = m_pkPre;
	}
};

//! 12*4 Byte
struct FvAoIBase
{
	FvAoIDLNode		m_kRelateMin;
	FvAoIDLNode		m_kRelateMax;
	FvAoIDLNode		m_kEvtMin;
	FvAoIDLNode		m_kEvtMax;
	FvUInt64		m_uiDummy;
	void*			m_pkUserData;
	struct
	{
		FvUInt8		m_bPt : 1;		//! 0:Box, 1:Pt
		FvUInt8		m_bAoI : 1;		//! 0:Trap, 1:AoI
		FvUInt8		m_bRef : 1;		//! 1:RefPt
		FvUInt8		m_bAdd : 1;		//! 1:Add, 0:Del
		FvUInt8		m_bAddFlg : 1;	//! 1:do Add/undo Del, 0:undo Add/do Del
		FvUInt8		m_uiR1 : 3;
		FvUInt8		m_uiR2;
	};
	FvUInt16		m_uiMask;

	void Init(bool bPt, bool bRef, bool bAoI, FvUInt16 uiMask, void* pkData)
	{
		m_bPt = bPt;
		m_bAoI = bAoI;
		m_bRef = bRef;
		m_uiMask = uiMask;
		m_pkUserData = pkData;
		m_uiDummy = 0;

		m_kRelateMin.m_pkPre = NULL;
		m_kRelateMin.m_pkNex = &m_kRelateMax;
		m_kRelateMax.m_pkPre = &m_kRelateMin;
		m_kRelateMax.m_pkNex = NULL;

		m_kEvtMin.m_pkPre = NULL;
		m_kEvtMin.m_pkNex = &m_kEvtMax;
		m_kEvtMax.m_pkPre = &m_kEvtMin;
		m_kEvtMax.m_pkNex = NULL;
	}

	void AddEvt(FvAoIDLNode& kNode)
	{
		FvAoIDLNode& kNodePre = *(m_kEvtMax.m_pkPre);
		FvAoIDLNode& kNodeNex = m_kEvtMax;
		kNode.m_pkPre = &kNodePre;
		kNode.m_pkNex = &kNodeNex;
		kNodePre.m_pkNex = &kNode;
		kNodeNex.m_pkPre = &kNode;
	}

	void AddRelateBegin(FvAoIDLNode& kNode)
	{
		FvAoIDLNode& kNodePre = m_kRelateMin;
		FvAoIDLNode& kNodeNex = *(m_kRelateMin.m_pkNex);
		kNode.m_pkPre = &kNodePre;
		kNode.m_pkNex = &kNodeNex;
		kNodePre.m_pkNex = &kNode;
		kNodeNex.m_pkPre = &kNode;
	}

	void AddRelateEnd(FvAoIDLNode& kNode)
	{
		FvAoIDLNode& kNodePre = *(m_kRelateMax.m_pkPre);
		FvAoIDLNode& kNodeNex = m_kRelateMax;
		kNode.m_pkPre = &kNodePre;
		kNode.m_pkNex = &kNodeNex;
		kNodePre.m_pkNex = &kNode;
		kNodeNex.m_pkPre = &kNode;
	}

	bool IsObject()
	{
		return m_bPt;
	}

	bool IsObserver()
	{
		return !m_bPt;
	}

	bool IsAoI()
	{
		return m_bAoI;
	}

	bool IsTrap()
	{
		return !m_bAoI;
	}

	bool IsRefPt()
	{
		return m_bRef;
	}

	bool NeedAdd()
	{
		return m_bAdd && m_bAddFlg;
	}

	bool NeedDel()
	{
		return !(m_bAdd || m_bAddFlg);
	}
};

//! 8*4 Byte
struct FvAoIPos
{
	FvAoIPos*		m_pkPreX;
	FvAoIPos*		m_pkNexX;
	FvAoIPos*		m_pkPreY;
	FvAoIPos*		m_pkNexY;
	FvInt32			m_iX;
	FvInt32			m_iY;
	FvAoIBase*		m_pkOwner;
	struct
	{
		FvUInt8		m_uiFlag : 2;	//! *1:Pt, 00:Box(Min), 10:Box(Max)
		FvUInt8		m_uiR1 : 6;
		FvUInt8		m_uiR2;
		FvUInt8		m_uiObjCntHigh;
		FvUInt8		m_uiObjCntLow;
	};

	void Init(FvInt32 iX, FvInt32 iY, bool bPt, bool bMax, FvAoIBase* pkOwner)
	{
		m_iX = iX;
		m_iY = iY;
		m_uiFlag = FvUInt8(bPt) | (FvUInt8(bMax) << 1);
		m_pkOwner = pkOwner;
	}

	void SetPos(FvInt32 iX, FvInt32 iY)
	{
		m_iX = iX;
		m_iY = iY;
	}

	bool IsPt()
	{
		return m_uiFlag & 0x01;
	}

	bool IsBoxMin()
	{
		return m_uiFlag == 0x00;
	}

	bool IsBoxMax()
	{
		return m_uiFlag & 0x02;
	}

	void DrawOutX()
	{
		m_pkPreX->m_pkNexX = m_pkNexX;
		m_pkNexX->m_pkPreX = m_pkPreX;
	}

	void DrawOutY()
	{
		m_pkPreY->m_pkNexY = m_pkNexY;
		m_pkNexY->m_pkPreY = m_pkPreY;
	}

	void DrawOut()
	{
		DrawOutX();
		DrawOutY();
	}
};

#define AOIPOS_MIN_X(pkPos)	(((FvAoIObserver*)(pkPos)->m_pkOwner)->m_pkPosMin->m_iX)
#define AOIPOS_MIN_Y(pkPos)	(((FvAoIObserver*)(pkPos)->m_pkOwner)->m_pkPosMin->m_iY)
#define AOIPOS_MAX_X(pkPos)	(((FvAoIObserver*)(pkPos)->m_pkOwner)->m_pkPosMax->m_iX)
#define AOIPOS_MAX_Y(pkPos)	(((FvAoIObserver*)(pkPos)->m_pkOwner)->m_pkPosMax->m_iY)

//! 17*4 Byte
struct FvAoIObject : public FvAoIBase
{
	FvAoIDLNode		m_kCellNode;
	FvAoIPos*		m_pkPos;
	FvInt32			m_iVisibility;
	FvUInt16		m_uiCellIDX;
	FvUInt16		m_uiCellIDY;

	void Init(FvInt32 iX, FvInt32 iY, FvInt32 iVisibility, FvUInt16 uiMask, bool bRef, void* pkData, void* pkPos)
	{
		FvAoIBase::Init(true, bRef, false, uiMask, pkData);

		m_pkPos = (FvAoIPos*)pkPos;
		m_iVisibility = iVisibility;

		m_pkPos->Init(iX, iY, true, false, this);
		*(FvUInt16*)&(m_pkPos->m_uiObjCntHigh) = 0;
	}

	void SetPos(FvInt32 iX, FvInt32 iY)
	{
		m_pkPos->SetPos(iX, iY);
	}

	void AddObs()
	{
		++(*(FvUInt16*)&(m_pkPos->m_uiObjCntHigh));
	}

	void DelObs()
	{
		FV_ASSERT((*(FvUInt16*)&(m_pkPos->m_uiObjCntHigh)) > 0);
		--(*(FvUInt16*)&(m_pkPos->m_uiObjCntHigh));
	}

	FvUInt16 ObsCnt()
	{
		return *(FvUInt16*)&(m_pkPos->m_uiObjCntHigh);
	}
};

//! 17*4 Byte
struct FvAoIObserver : public FvAoIBase
{
	FvAoIPos*		m_pkPosMin;
	FvAoIPos*		m_pkPosMax;
	FvAoIObject*	m_pkOwner;
	FvInt32			m_iVision;
	FvInt32			m_iDisVisibility;

	void Init(FvAoIObject* pkOwner, bool bAoI, FvInt32 iVision, FvInt32 iDisVisibility, FvUInt16 uiMask, void* pkData)
	{
		FvAoIBase::Init(false, false, bAoI, uiMask, pkData);

		m_pkOwner = pkOwner;
		m_iVision = iVision;
		m_iDisVisibility = iDisVisibility;
		FV_ASSERT(m_pkPosMin);
		*(FvUInt16*)&(m_pkPosMin->m_uiObjCntHigh) = 0;
	}

	void AddObj()
	{
		++(*(FvUInt16*)&(m_pkPosMin->m_uiObjCntHigh));
	}

	void DelObj()
	{
		FV_ASSERT((*(FvUInt16*)&(m_pkPosMin->m_uiObjCntHigh)) > 0);
		--(*(FvUInt16*)&(m_pkPosMin->m_uiObjCntHigh));
	}

	FvUInt16 ObjCnt()
	{
		return *(FvUInt16*)&(m_pkPosMin->m_uiObjCntHigh);
	}
};

#define AOIOBS_MIN_X(pkObs)	((pkObs)->m_pkPosMin->m_iX)
#define AOIOBS_MIN_Y(pkObs)	((pkObs)->m_pkPosMin->m_iY)
#define AOIOBS_MAX_X(pkObs)	((pkObs)->m_pkPosMax->m_iX)
#define AOIOBS_MAX_Y(pkObs)	((pkObs)->m_pkPosMax->m_iY)

//! 1*4 Byte
struct FvAoIExt
{
	float	m_fDisSqr;
};

//! 7*4 Byte
struct FvAoIRelate
{
	FvAoIDLNode		m_kObsNode;
	FvAoIDLNode		m_kObjNode;
	FvAoIObserver*	m_pkObs;
	FvAoIObject*	m_pkObj;
	FvAoIExt*		m_pkExt;

	void Init(FvAoIObserver* pkObs, FvAoIObject* pkObj)
	{
		m_pkObs = pkObs;
		m_pkObj = pkObj;
		m_pkExt = NULL;
	}
};

#define AOIRELATEOBS(pkNode)	((FvAoIRelate*)(pkNode))
#define AOIRELATEOBJ(pkNode)	((FvAoIRelate*)((pkNode)-1))

//! 7*4 Byte
struct FvAoIEvt
{
	FvAoIDLNode		m_kObsNode;
	FvAoIDLNode		m_kObjNode;
	struct
	{
		FvUInt8		m_uiFlag : 2;	//! 00:Out, 01:In, 1*:Del
		FvUInt8		m_uiR1 : 6;
		FvUInt8		m_uiR2;
		FvUInt8		m_uiR3;
		FvUInt8		m_uiR4;
	};
	FvAoIObserver*	m_pkObs;
	union
	{
		FvAoIObject*m_pkObj;
		FvAoIExt*	m_pkExt;
	};

	void InitIn(FvAoIObserver* pkObs, FvAoIObject* pkObj)
	{
		m_uiFlag = 0x01;
		m_pkObs = (FvAoIObserver*)pkObs;
		m_pkObj = (FvAoIObject*)pkObj;
	}
	void InitOut(FvAoIObserver* pkObs, FvAoIObject* pkObj)
	{
		m_uiFlag = 0x00;
		m_pkObs = (FvAoIObserver*)pkObs;
		m_pkObj = (FvAoIObject*)pkObj;
	}
	void InitDel(FvAoIObserver* pkObs, FvAoIExt* pkExt)
	{
		m_uiFlag = 0x02;
		m_pkObs = (FvAoIObserver*)pkObs;
		m_pkExt = pkExt;
	}

	bool IsOut() { return m_uiFlag == 0; }
	bool IsIn() { return m_uiFlag & 0x01; }
	bool IsDel() { return m_uiFlag & 0x02; }
};


#define AOIEVTOBS(pkNode)	((FvAoIEvt*)(pkNode))
#define AOIEVTOBJ(pkNode)	((FvAoIEvt*)((pkNode)-1))

//! 往左插入Pos
#define AOIPOS_INS_LEFT_BEGIN(_T)			\
	FvInt32 iValue = pkPos->m_i##_T;		\
	pkIns = pkIns->m_pkPre##_T;				\
	while(iValue < pkIns->m_i##_T)			\
	{

#define AOIPOS_INS_LEFT_END(_T)				\
		pkIns = pkIns->m_pkPre##_T;			\
	}										\
	FvAoIPos* pkNex = pkIns->m_pkNex##_T;	\
	pkPos->m_pkPre##_T = pkIns;				\
	pkPos->m_pkNex##_T = pkNex;				\
	pkNex->m_pkPre##_T = pkPos;				\
	pkIns->m_pkNex##_T = pkPos;

//! 往右插入Pos
#define AOIPOS_INS_RIGHT_BEGIN(_T)			\
	FvInt32 iValue = pkPos->m_i##_T;		\
	pkIns = pkIns->m_pkNex##_T;				\
	while(pkIns->m_i##_T < iValue)			\
	{

#define AOIPOS_INS_RIGHT_END(_T)			\
		pkIns = pkIns->m_pkNex##_T;			\
	}										\
	FvAoIPos* pkPre = pkIns->m_pkPre##_T;	\
	pkPos->m_pkPre##_T = pkPre;				\
	pkPos->m_pkNex##_T = pkIns;				\
	pkPre->m_pkNex##_T = pkPos;				\
	pkIns->m_pkPre##_T = pkPos;

//! 遍历
#define AOIPOS_ITERATE_BEGIN				\
	while(pkBegin != pkEnd)					\
	{

#define AOIPOS_ITERATE_END(_T)				\
		pkBegin = pkBegin->m_pkNex##_T;		\
	}


#ifndef FV_DEBUG

//! 沿x轴,从pkIns的左边插入pkPos
inline void InsPosLeftOnX(FvAoIPos* pkIns, FvAoIPos* pkPos)
{
	AOIPOS_INS_LEFT_BEGIN(X)
	AOIPOS_INS_LEFT_END(X)
}

//! 沿x轴,从pkIns的右边插入pkPos
inline void InsPosRightOnX(FvAoIPos* pkIns, FvAoIPos* pkPos)
{
	AOIPOS_INS_RIGHT_BEGIN(X)
	AOIPOS_INS_RIGHT_END(X)
}

//! 沿y轴,从pkIns的左边插入pkPos
inline void InsPosLeftOnY(FvAoIPos* pkIns, FvAoIPos* pkPos)
{
	AOIPOS_INS_LEFT_BEGIN(Y)
	AOIPOS_INS_LEFT_END(Y)
}

//! 沿y轴,从pkIns的右边插入pkPos
inline void InsPosRightOnY(FvAoIPos* pkIns, FvAoIPos* pkPos)
{
	AOIPOS_INS_RIGHT_BEGIN(Y)
	AOIPOS_INS_RIGHT_END(Y)
}

//! 沿x轴,从pkIns的左边插入pkPos,并检查
template<typename TChecker, void (TChecker::*func)(FvAoIPos*)>
inline void InsPosLeftOnXAndCheck(FvAoIPos* pkIns, FvAoIPos* pkPos, TChecker* pkChecker)
{
	AOIPOS_INS_LEFT_BEGIN(X)
		((*pkChecker).*func)(pkIns);
	AOIPOS_INS_LEFT_END(X)
}

//! 沿x轴,从pkIns的右边插入pkPos,并检查
template<typename TChecker, void (TChecker::*func)(FvAoIPos*)>
inline void InsPosRightOnXAndCheck(FvAoIPos* pkIns, FvAoIPos* pkPos, TChecker* pkChecker)
{
	AOIPOS_INS_RIGHT_BEGIN(X)
		((*pkChecker).*func)(pkIns);
	AOIPOS_INS_RIGHT_END(X)
}

//! 沿y轴,从pkIns的左边插入pkPos,并检查
template<typename TChecker, void (TChecker::*func)(FvAoIPos*)>
inline void InsPosLeftOnYAndCheck(FvAoIPos* pkIns, FvAoIPos* pkPos, TChecker* pkChecker)
{
	AOIPOS_INS_LEFT_BEGIN(Y)
		((*pkChecker).*func)(pkIns);
	AOIPOS_INS_LEFT_END(Y)
}

//! 沿y轴,从pkIns的右边插入pkPos,并检查
template<typename TChecker, void (TChecker::*func)(FvAoIPos*)>
inline void InsPosRightOnYAndCheck(FvAoIPos* pkIns, FvAoIPos* pkPos, TChecker* pkChecker)
{
	AOIPOS_INS_RIGHT_BEGIN(Y)
		((*pkChecker).*func)(pkIns);
	AOIPOS_INS_RIGHT_END(Y)
}

//! 沿x轴,从pkBegin遍历到pkEnd(包括Begin不包括End),并检查
template<typename TChecker, void (TChecker::*func)(FvAoIPos*)>
inline void IteratePosOnXAndCheck(FvAoIPos* pkBegin, FvAoIPos* pkEnd, TChecker* pkChecker)
{
	AOIPOS_ITERATE_BEGIN
		((*pkChecker).*func)(pkBegin);
	AOIPOS_ITERATE_END(X)
}

//! 沿y轴,从pkBegin遍历到pkEnd(包括Begin不包括End),并检查
template<typename TChecker, void (TChecker::*func)(FvAoIPos*)>
inline void IteratePosOnYAndCheck(FvAoIPos* pkBegin, FvAoIPos* pkEnd, TChecker* pkChecker)
{
	AOIPOS_ITERATE_BEGIN
		((*pkChecker).*func)(pkBegin);
	AOIPOS_ITERATE_END(Y)
}

#else

inline FvUInt64 InsPosLeftOnX(FvAoIPos* pkIns, FvAoIPos* pkPos)
{
	FvUInt64 cnt(0);
	AOIPOS_INS_LEFT_BEGIN(X)
		++cnt;
	AOIPOS_INS_LEFT_END(X)
	return cnt;
}

//! 沿x轴,从pkIns的右边插入pkPos
inline FvUInt64 InsPosRightOnX(FvAoIPos* pkIns, FvAoIPos* pkPos)
{
	FvUInt64 cnt(0);
	AOIPOS_INS_RIGHT_BEGIN(X)
		++cnt;
	AOIPOS_INS_RIGHT_END(X)
	return cnt;
}

//! 沿y轴,从pkIns的左边插入pkPos
inline FvUInt64 InsPosLeftOnY(FvAoIPos* pkIns, FvAoIPos* pkPos)
{
	FvUInt64 cnt(0);
	AOIPOS_INS_LEFT_BEGIN(Y)
		++cnt;
	AOIPOS_INS_LEFT_END(Y)
	return cnt;
}

//! 沿y轴,从pkIns的右边插入pkPos
inline FvUInt64 InsPosRightOnY(FvAoIPos* pkIns, FvAoIPos* pkPos)
{
	FvUInt64 cnt(0);
	AOIPOS_INS_RIGHT_BEGIN(Y)
		++cnt;
	AOIPOS_INS_RIGHT_END(Y)
	return cnt;
}

//! 沿x轴,从pkIns的左边插入pkPos,并检查
template<typename TChecker, void (TChecker::*func)(FvAoIPos*)>
inline FvUInt64 InsPosLeftOnXAndCheck(FvAoIPos* pkIns, FvAoIPos* pkPos, TChecker* pkChecker)
{
	FvUInt64 cnt(0);
	AOIPOS_INS_LEFT_BEGIN(X)
		((*pkChecker).*func)(pkIns);
		++cnt;
	AOIPOS_INS_LEFT_END(X)
	return cnt;
}

//! 沿x轴,从pkIns的右边插入pkPos,并检查
template<typename TChecker, void (TChecker::*func)(FvAoIPos*)>
inline FvUInt64 InsPosRightOnXAndCheck(FvAoIPos* pkIns, FvAoIPos* pkPos, TChecker* pkChecker)
{
	FvUInt64 cnt(0);
	AOIPOS_INS_RIGHT_BEGIN(X)
		((*pkChecker).*func)(pkIns);
		++cnt;
	AOIPOS_INS_RIGHT_END(X)
	return cnt;
}

//! 沿y轴,从pkIns的左边插入pkPos,并检查
template<typename TChecker, void (TChecker::*func)(FvAoIPos*)>
inline FvUInt64 InsPosLeftOnYAndCheck(FvAoIPos* pkIns, FvAoIPos* pkPos, TChecker* pkChecker)
{
	FvUInt64 cnt(0);
	AOIPOS_INS_LEFT_BEGIN(Y)
		((*pkChecker).*func)(pkIns);
		++cnt;
	AOIPOS_INS_LEFT_END(Y)
	return cnt;
}

//! 沿y轴,从pkIns的右边插入pkPos,并检查
template<typename TChecker, void (TChecker::*func)(FvAoIPos*)>
inline FvUInt64 InsPosRightOnYAndCheck(FvAoIPos* pkIns, FvAoIPos* pkPos, TChecker* pkChecker)
{
	FvUInt64 cnt(0);
	AOIPOS_INS_RIGHT_BEGIN(Y)
		((*pkChecker).*func)(pkIns);
		++cnt;
	AOIPOS_INS_RIGHT_END(Y)
	return cnt;
}

//! 沿x轴,从pkBegin遍历到pkEnd(包括Begin不包括End),并检查
template<typename TChecker, void (TChecker::*func)(FvAoIPos*)>
inline FvUInt64 IteratePosOnXAndCheck(FvAoIPos* pkBegin, FvAoIPos* pkEnd, TChecker* pkChecker)
{
	FvUInt64 cnt(0);
	AOIPOS_ITERATE_BEGIN
		((*pkChecker).*func)(pkBegin);
		++cnt;
	AOIPOS_ITERATE_END(X)
	return cnt;
}

//! 沿y轴,从pkBegin遍历到pkEnd(包括Begin不包括End),并检查
template<typename TChecker, void (TChecker::*func)(FvAoIPos*)>
inline FvUInt64 IteratePosOnYAndCheck(FvAoIPos* pkBegin, FvAoIPos* pkEnd, TChecker* pkChecker)
{
	FvUInt64 cnt(0);
	AOIPOS_ITERATE_BEGIN
		((*pkChecker).*func)(pkBegin);
		++cnt;
	AOIPOS_ITERATE_END(Y)
	return cnt;
}

#endif


#endif//__FvAoIObj_H__
