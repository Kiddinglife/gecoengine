//{future header message}
#ifndef __FvAoIMgr_H__
#define __FvAoIMgr_H__

#include "FvAoIUtility.h"
#include "FvAoIObj.h"
#include "FvAoIMemMgr.h"
#include "FvAoIEvtCache.h"
#include "FvAoIGrid.h"

typedef FvAoIBase* FvAoIHandle;
const FvAoIHandle FVAOI_NULL_HANDLE = ((FvAoIHandle)0);


template<typename TAoIExt, typename TTrapExt>
class FvAoIMgr
{
public:
	FvAoIMgr();
	~FvAoIMgr();

	bool		Init(float fX, float fY, float fWidthX, float fWidthY,
					float fGridSize, FvUInt8 uiRefPtLevel, float fMinStep = .1f,
					FvUInt32 uiObjAndObsInitCnt = 1024, FvUInt32 uiObjAndObsIncrCnt = 512,
					FvUInt32 uiPosInitCnt = 1024, FvUInt32 uiPosIncrCnt = 512,
					FvUInt32 uiRelaAndEvtInitCnt = 1024, FvUInt32 uiRelaAndEvtIncrCnt = 512,
					FvUInt32 uiAoiExtInitCnt = 1024, FvUInt32 uiAoiExtIncrCnt = 512,
					FvUInt32 uiTrapExtInitCnt = 1024, FvUInt32 uiTrapExtIncrCnt = 512);

	FvAoIHandle	AddObject(FvUInt16 uiMask, float fX, float fY, float fVisibility, void* pkData);
	FvAoIHandle AddObserver(FvAoIHandle hOwner, FvUInt16 uiMask, float fX, float fY, float fVision, float fDisVisibility, bool bAoI, void* pkData);

	void		Remove(FvAoIHandle hHandle);

	bool		SetMask(FvAoIHandle hHandle, FvUInt16 uiMask);
	bool		SetVisibility(FvAoIHandle hHandle, float fVisibility);
	bool		SetVision(FvAoIHandle hHandle, float fVision);
	bool		SetDisVisibility(FvAoIHandle hHandle, float fDisVisibility);

	FvUInt16	GetMask(FvAoIHandle hHandle);
	float		GetVisibility(FvAoIHandle hHandle);
	float		GetVision(FvAoIHandle hHandle);
	float		GetDisVisibility(FvAoIHandle hHandle);

	FvUInt16	GetObjCntInVision(FvAoIHandle hHandle);
	FvUInt16	GetObsCntInDVision(FvAoIHandle hHandle);

#ifndef FV_DEBUG
	void		Move(FvAoIHandle hHandle, float fX, float fY);

	template<typename TListener,
		void (TListener::*OnEnter)(void* pkObsData, FvAoIExt* pkExt),
		void (TListener::*OnStand)(void* pkObsData, FvAoIExt* pkExt),
		void (TListener::*OnLeave)(void* pkObsData, FvAoIExt* pkExt)>
	void		UpdateAoI(FvAoIHandle hHandle, TListener* pkListener);

	template<typename TListener,
		void (TListener::*OnEnter)(void* pkObsData, FvAoIExt* pkExt),
		void (TListener::*OnLeave)(void* pkObsData, FvAoIExt* pkExt)>
	void		UpdateTrap(FvAoIHandle hHandle, TListener* pkListener);

#else
	FvUInt64	Move(FvAoIHandle hHandle, float fX, float fY);

	template<typename TListener,
		void (TListener::*OnEnter)(void* pkObsData, FvAoIExt* pkExt),
		void (TListener::*OnStand)(void* pkObsData, FvAoIExt* pkExt),
		void (TListener::*OnLeave)(void* pkObsData, FvAoIExt* pkExt)>
	void		UpdateAoI(FvAoIHandle hHandle, TListener* pkListener,
						FvUInt64& uiEvtCnt, FvUInt64& uiRelateCnt, FvUInt64& uiIncreCnt);

	template<typename TListener,
		void (TListener::*OnEnter)(void* pkObsData, FvAoIExt* pkExt),
		void (TListener::*OnLeave)(void* pkObsData, FvAoIExt* pkExt)>
	void		UpdateTrap(FvAoIHandle hHandle, TListener* pkListener,
						FvUInt64& uiEvtCnt, FvUInt64& uiRelateCnt, FvUInt64& uiIncreCnt);
#endif

	template<typename TVisiter,
		void (TVisiter::*OnVisit)(void* pkObjData)>
	FvUInt32	QueryVision(FvAoIHandle hHandle, TVisiter* pkVisiter);

	template<typename TVisiter,
		void (TVisiter::*OnVisit)(void* pkObsData)>
	FvUInt32	QueryDVision(FvAoIHandle hHandle, TVisiter* pkVisiter);

	template<typename TVisiter,
		void (TVisiter::*OnVisit)(void* pkObjData)>
	FvUInt32	QueryArea(float fX, float fY, float fRadius, TVisiter* pkVisiter);

protected:
	FvInt32		TransPosAndScale(float fPos);
	void		AddObjectToOrderedGrid(FvAoIObject* pkObj, float fX, float fY, FvInt32 iX, FvInt32 iY);
	void		SetObjectMask(FvAoIObject* pkObj);
	void		SetObserverMask(FvAoIObserver* pkObs);
#ifndef FV_DEBUG
	void		MoveObject(FvAoIObject* pkObj, float fX, float fY);
	void		MoveObserver(FvAoIObserver* pkObs, float fX, float fY);
#else
	FvUInt64	MoveObject(FvAoIObject* pkObj, float fX, float fY);
	FvUInt64	MoveObserver(FvAoIObserver* pkObs, float fX, float fY);
#endif
	FvAoIExt*	CreateAoIExt(void* pkObsUserData, void* pkObjUserData);
	FvAoIExt*	CreateTrapExt(void* pkObsUserData, void* pkObjUserData);
	void		DestroyAoIExt(FvAoIExt* pkExt);
	void		DestroyTrapExt(FvAoIExt* pkExt);

#ifndef FV_DEBUG
protected:
#else
public:
#endif
public:
	FvAoIGrid	m_kGrid;
	FvAoIPos	m_kMinPos;
	FvAoIPos	m_kMaxPos;

	FvAoIMemMgr	m_kObjAndObsAlloc;
	FvAoIMemMgr m_kPosAlloc;
	FvAoIMemMgr m_kRelaAndEvtAlloc;
	FvAoIMemMgr	m_kAoiExtAlloc;
	FvAoIMemMgr	m_kTrapExtAlloc;

	FvAoIEvtCache	m_kEvtCache;
	FvUInt64	m_uiDummy;

	float		m_fCoorScale;
	float		m_fCoorScaleInv;
	FvInt32		m_iMinStep;
	FvInt32		m_iGridMinX;
	FvInt32		m_iGridMinY;
	FvInt32		m_iGridMaxX;
	FvInt32		m_iGridMaxY;
	FvInt32		m_iGridInnerMinX;
	FvInt32		m_iGridInnerMinY;
	FvInt32		m_iGridInnerMaxX;
	FvInt32		m_iGridInnerMaxY;
	float		m_fXYRate;
	float		m_fInsOffsetX;
	float		m_fInsOffsetY;
	float		m_fInsInvX;
	float		m_fInsInvY;
	FvUInt32	m_uiRefPtCntInAxis;
	FvAoIObject*m_pkRefPts;
};

#include "FvAoIMgr.inl"

#endif//__FvAoIMgr_H__

