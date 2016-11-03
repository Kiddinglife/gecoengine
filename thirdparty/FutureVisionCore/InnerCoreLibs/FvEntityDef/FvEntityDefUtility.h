//{future header message}
#ifndef __FvEntityDefUtility_H__
#define __FvEntityDefUtility_H__

#include "FvEntityDefDefines.h"
#include <FvBinaryStream.h>


#define FV_OFFSET_OF(_Class, _Member)		((size_t)&(((_Class*)0)->_Member))



//! id压码,保留baseId的最大值
//! BITS:1, CNT:512
//! BITS:2, CNT:1024
//! BITS:3, CNT:2048
//! BITS:4, CNT:4096
//! BITS:5, CNT:8192
//! BITS:6, CNT:16384
//! BITS:7, CNT:32768
//! BITS:8, CNT:65536
template<FvUInt8 BITS>
inline void FvDataCompressID(int cnt, int id, int& baseId, int& extId)
{
	FV_ASSERT(0 < BITS && BITS < 9);
	const int KEY1 = (0x01 << BITS) -1;
	const int KEY2 = KEY1 - 255;

	int numExposed = cnt - KEY2;
	int numSubSlots = (numExposed + (numExposed>>8)) >> 8;//! = numExposed/255, numExposed<=65536时正确
	int begSubSlot = KEY1 - numSubSlots;

	int overBy = id - begSubSlot;
	if (overBy < 0)
	{
		baseId = id;
		extId = -1;
	}
	else
	{
		baseId = begSubSlot + (overBy>>8);
		extId = overBy & 0xFF;
	}
}

//! id压码,使用全部可能值,能比FvDataCompressID多表示256
//! BITS:1, CNT:256
//! BITS:2, CNT:768
//! BITS:3, CNT:1792
//! BITS:4, CNT:3840
//! BITS:5, CNT:7936
//! BITS:6, CNT:16128
//! BITS:7, CNT:32512
//! BITS:8, CNT:65280
template<FvUInt8 BITS>
inline void FvDataCompressIDFull(int cnt, int id, int& baseId, int& extId)
{
	FV_ASSERT(0 < BITS && BITS < 9);
	const int KEY1 = (0x01 << BITS);
	const int KEY2 = KEY1 - 255;

	int numExposed = cnt - KEY2;
	int numSubSlots = (numExposed + (numExposed>>8)) >> 8;//! = numExposed/255, numExposed<=65536时正确
	int begSubSlot = KEY1 - numSubSlots;

	int overBy = id - begSubSlot;
	if (overBy < 0)
	{
		baseId = id;
		extId = -1;
	}
	else
	{
		baseId = begSubSlot + (overBy>>8);
		extId = overBy & 0xFF;
	}
}

//! id解码,保留baseId的最大值
template<FvUInt8 BITS>
inline bool FvDataDecompressID(int cnt, int& id, int baseId, FvBinaryIStream& kIS)
{
	FV_ASSERT(0 < BITS && BITS < 9);
	const int KEY1 = (0x01 << BITS) -1;
	const int KEY2 = KEY1 - 255;

	int numExposed = cnt - KEY2;
	int numSubSlots = (numExposed + (numExposed>>8)) >> 8;//! = numExposed/255, numExposed<=65536时正确
	int begSubSlot = KEY1 - numSubSlots;
	int curSubSlot = baseId - begSubSlot;
	int index = curSubSlot < 0 ? baseId :
		(begSubSlot + (curSubSlot<<8) + *(FvUInt8*)kIS.Retrieve(1));

	FV_ASSERT(!kIS.Error());

	if (index < cnt)
	{
		id = index;
		return true;
	}
	else
	{
		return false;
	}
}

//! id解码,使用全部可能值,能比FvDataDecompressID多表示256
template<FvUInt8 BITS>
inline bool FvDataDecompressIDFull(int cnt, int& id, int baseId, FvBinaryIStream& kIS)
{
	FV_ASSERT(0 < BITS && BITS < 9);
	const int KEY1 = (0x01 << BITS);
	const int KEY2 = KEY1 - 255;

	int numExposed = cnt - KEY2;
	int numSubSlots = (numExposed + (numExposed>>8)) >> 8;//! = numExposed/255, numExposed<=65536时正确
	int begSubSlot = KEY1 - numSubSlots;
	int curSubSlot = baseId - begSubSlot;
	int index = curSubSlot < 0 ? baseId :
		(begSubSlot + (curSubSlot<<8) + *(FvUInt8*)kIS.Retrieve(1));

	FV_ASSERT(!kIS.Error());

	if (index < cnt)
	{
		id = index;
		return true;
	}
	else
	{
		return false;
	}
}


//! Pt->Idx->DataID
struct PtIdxToID;
FV_ENTITYDEF_API void FvPointerToIndexAndDataID(const void* pkThis, const void* pkPt, FvUInt16& uiDataIdx, FvUInt16& uiDataID,
							   FvUInt32 uiBitsArraySize, const FvUInt32* pBits, const FvUInt16* pIdx, const PtIdxToID* pkIdxToIDs);


//! 生成Pt->Idx->DataID用数据
struct NameAddr;
FV_ENTITYDEF_API void FvMakePtToIdxData(FvUInt32* pBits, FvUInt32 uiBitsArraySize, FvUInt16* pIdx, FvUInt32 uiPtCnt, const NameAddr* pPtInfo);
FV_ENTITYDEF_API void FvMakePtToIdxData(FvUInt32* pBits, FvUInt32 uiBitsArraySize, FvUInt16* pIdx, FvUInt32 uiPtCnt, const FvUInt32* pPtAddrs);


//! 将类成员函数地址转换为FvUInt32
template<class FuncAddrType>
FvUInt32 MemberFuncAddrToNum(FuncAddrType f)
{
	union
	{
		FuncAddrType _f;
		FvUInt32	 _t;
	}ut;

	ut._f = f;
	return ut._t;
}


/**
//! id压码
template<FvUInt8 BITS>
inline bool FvDataCompressID(int cnt, int id, int& baseId, int& extId)
{
	FV_ASSERT(0 < BITS && BITS < 9);
	const int KEY1 = (0x01 << BITS) + 1;
	const int KEY2 = (0x01 << BITS);

	int numExposed = cnt;
	int numSubSlots = (numExposed-KEY1 + 255) / 255;
	int begSubSlot = KEY2 - numSubSlots;

	int overBy = id - begSubSlot;
	if (overBy < 0)
	{
		baseId = id;
		extId = -1;
	}
	else
	{
		baseId = begSubSlot + (overBy>>8);
		extId = overBy & 0xFF;
	}

	return true;
}

//! id解码
template<FvUInt8 BITS>
inline bool FvDataDecompressID(int cnt, int& id, int baseId, FvBinaryIStream& kIS)
{
	FV_ASSERT(0 < BITS && BITS < 9);
	const int KEY1 = (0x01 << BITS) + 1;
	const int KEY2 = (0x01 << BITS);

	int numExposed = cnt;
	int numSubSlots = (numExposed-KEY1 + 255) / 255;
	int begSubSlot = KEY2 - numSubSlots;
	int curSubSlot = baseId - begSubSlot;
	int index = curSubSlot < 0 ? baseId :
		(begSubSlot + (curSubSlot<<8) + *(FvUInt8*)kIS.Retrieve(1));

	if (index < numExposed)
	{
		id = index;
		return true;
	}
	else
	{
		return false;
	}
}
**/

#endif//__FvEntityDefUtility_H__
