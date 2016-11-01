#include "FvEntityDefUtility.h"
#include "FvDataObj.h"
#include "FvEntityExport.h"


void FvPointerToIndexAndDataID(const void* pkThis, const void* pkPt, FvUInt16& uiDataIdx, FvUInt16& uiDataID,
							   FvUInt32 uiBitsArraySize, const FvUInt32* pBits, const FvUInt16* pIdx, const PtIdxToID* pkIdxToIDs)
{
	FV_ASSERT(pkThis && pkPt && uiBitsArraySize && pBits && pIdx && pkIdxToIDs);

	int iDis = (char*)pkPt - (char*)pkThis;
	FV_ASSERT(0 <= iDis);
	FvUInt32 uiBit = iDis >> 2;
	FvUInt32 uiDWD = uiBit >> 5;
	FvUInt32 uiIdx = uiBit & 0x1F;
	FV_ASSERT(uiDWD < uiBitsArraySize);

	union
	{
		FvUInt32 ui32;
		struct
		{
			FvUInt8	uiA;
			FvUInt8	uiB;
			FvUInt8	uiC;
			FvUInt8	uiD;
		} st;
	} u;

	FV_ASSERT(pBits[uiDWD] & (0x01 << (31 - uiIdx)));
	u.ui32 = pBits[uiDWD] & (0xFFFFFFFF << (31 - uiIdx));

	const FvUInt8 BitsBuf[256] = {	0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
									1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
									1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
									2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
									1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
									2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
									2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
									3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8};

	FvUInt16 uiPtIdx = pIdx[uiDWD] + BitsBuf[u.st.uiA] + BitsBuf[u.st.uiB] + BitsBuf[u.st.uiC] + BitsBuf[u.st.uiD] -1;
	uiDataIdx = pkIdxToIDs[uiPtIdx].uiDataIdx;
	uiDataID = pkIdxToIDs[uiPtIdx].uiDataID;
}

//! 生成Pt->Idx->DataID用数据
void FvMakePtToIdxData(FvUInt32* pBits, FvUInt32 uiBitsArraySize, FvUInt16* pIdx, FvUInt32 uiPtCnt, const NameAddr* pPtInfo)
{
	if(uiPtCnt == 0)
		return;

	FV_ASSERT(pBits && uiBitsArraySize && pIdx && pPtInfo && uiPtCnt);
	FvUInt32 uiSizeWanna = (pPtInfo[uiPtCnt-1].uiAddr>>7)+1;
	FV_ASSERT(uiSizeWanna == uiBitsArraySize);

	memset(pBits, 0, sizeof(FvUInt32) * uiBitsArraySize);
	memset(pIdx, 0, sizeof(FvUInt16) * uiBitsArraySize);

	FvUInt32 uiPreVal(0);
	FvUInt32 uiBitIdx(0);
	FvUInt16 uiAccu(0);
	for(FvUInt32 i=0; i<uiPtCnt; ++i)
	{
		FvUInt32 uiAddr = pPtInfo[i].uiAddr;
		FV_ASSERT(!uiPreVal || uiPreVal < uiAddr);
		uiPreVal = uiAddr;

		FvUInt32 uiBit = uiAddr >> 2;
		FvUInt32 uiDWD = uiBit >> 5;
		FvUInt32 uiIdx = uiBit & 0x1F;

		if(uiDWD != uiBitIdx)
		{
			uiBitIdx = uiDWD;
			pIdx[uiDWD] = uiAccu;
		}

		pBits[uiDWD] |= (0x01 << (31 - uiIdx));
		++uiAccu;
	}
}

void FvMakePtToIdxData(FvUInt32* pBits, FvUInt32 uiBitsArraySize, FvUInt16* pIdx, FvUInt32 uiPtCnt, const FvUInt32* pPtAddrs)
{
	if(uiPtCnt == 0)
		return;

	FV_ASSERT(pBits && uiBitsArraySize && pIdx && pPtAddrs && uiPtCnt);
	FvUInt32 uiSizeWanna = (pPtAddrs[uiPtCnt-1]>>7)+1;
	FV_ASSERT(uiSizeWanna == uiBitsArraySize);

	memset(pBits, 0, sizeof(FvUInt32) * uiBitsArraySize);
	memset(pIdx, 0, sizeof(FvUInt16) * uiBitsArraySize);

	FvUInt32 uiPreVal(0);
	FvUInt32 uiBitIdx(0);
	FvUInt16 uiAccu(0);
	for(FvUInt32 i=0; i<uiPtCnt; ++i)
	{
		FvUInt32 uiAddr = pPtAddrs[i];
		FV_ASSERT(!uiPreVal || uiPreVal < uiAddr);
		uiPreVal = uiAddr;

		FvUInt32 uiBit = uiAddr >> 2;
		FvUInt32 uiDWD = uiBit >> 5;
		FvUInt32 uiIdx = uiBit & 0x1F;

		if(uiDWD != uiBitIdx)
		{
			uiBitIdx = uiDWD;
			pIdx[uiDWD] = uiAccu;
		}

		pBits[uiDWD] |= (0x01 << (31 - uiIdx));
		++uiAccu;
	}
}


