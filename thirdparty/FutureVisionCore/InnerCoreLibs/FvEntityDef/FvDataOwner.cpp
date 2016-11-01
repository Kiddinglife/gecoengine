#include "FvDataOwner.h"
#include "FvEntityDefUtility.h"
#include "FvEntityExport.h"


#ifdef FV_SERVER
FvMemoryOStream	FvDataOwner::ms_kDataStream;
FvDataOwner::ChangePath FvDataOwner::ms_kDataPath;
#endif

bool FvDataCallBack::ms_bOwnCB = false;
FvEntity* FvDataCallBack::ms_pkEntity = NULL;
DataCallBackPath FvDataCallBack::ms_kCBDataPath;
FvUInt32 FvDataCallBack::ms_uiCBIndex = INVALID_DATACALLBACK_INDEX;
FvDataObj* FvDataCallBack::ms_pkCBObj = NULL;


class BitWriter
{
public:
	BitWriter()// : m_iByteCount( 0 ), m_iBitsLeft( 8 )
	{
		//memset( m_aBytes, 0, sizeof(m_aBytes) );
	}

	void Clear()
	{
		m_iByteCount = 0;
		m_iBitsLeft = 8;
		memset( m_aBytes, 0, sizeof(m_aBytes) );
	}

	void Add( FvInt32 dataBits, FvInt32 dataWord )
	{
		FvUInt32 dataHigh = dataWord << (32-dataBits);

		FvInt32 bitAt = 0;
		while (bitAt < dataBits)
		{
			m_aBytes[m_iByteCount] |= (dataHigh>>(32-m_iBitsLeft));
			dataHigh <<= m_iBitsLeft;

			bitAt += m_iBitsLeft;
			if (bitAt <= dataBits)
				m_iBitsLeft = 8, m_iByteCount++;
			else
				m_iBitsLeft = bitAt-dataBits;
		}
	}

	FvInt32		UsedBytes() { return m_iByteCount + (m_iBitsLeft != 8); }

	FvInt32		m_iByteCount;
	FvInt32		m_iBitsLeft;
	FvUInt8		m_aBytes[224];
};


class BitReader
{
public:
	BitReader( FvBinaryIStream & data ) : m_kData( data ), m_iBitsLeft( 0 ) { }

	FvInt32 Get( FvInt32 nbits )
	{
		FvInt32	ret = 0;

		FvInt32 gbits = 0;
		while (gbits < nbits)	// not as efficient as the writer...
		{
			if (m_iBitsLeft == 0)
			{
				m_uiByte = *(FvUInt8*)m_kData.Retrieve( 1 );
				m_iBitsLeft = 8;
			}

			FvInt32 bitsTake = min( nbits-gbits, m_iBitsLeft );
			ret = (ret << bitsTake) | (m_uiByte >> (8-bitsTake));
			m_uiByte <<= bitsTake;
			m_iBitsLeft -= bitsTake;
			gbits += bitsTake;
		}

		return ret;
	}

	FvBinaryIStream&	m_kData;
	FvInt32				m_iBitsLeft;
	FvUInt8				m_uiByte;
};

#ifdef FV_SERVER
FvUInt8 FvDataOwner::AddHeadToStream(OpCode uiOpCode, FvUInt16 uiDataIDCnt, FvUInt16 uiDataID)
{
	FV_ASSERT(0<=uiOpCode && uiOpCode<4);
	FV_ASSERT(uiDataID > 0);
	--uiDataID;
	FV_ASSERT(uiDataIDCnt <= 4096 && uiDataID < uiDataIDCnt);
	ms_kDataStream.Reset();

	int iBaseID, iExtID;
	FvDataCompressIDFull<4>(uiDataIDCnt, uiDataID, iBaseID, iExtID);

	FvUInt8 uiMessageID((iBaseID << 2) | uiOpCode);
	if(iExtID != -1)
		ms_kDataStream << FvUInt8(iExtID);

	static BitWriter kBits;
	kBits.Clear();

	for(int i=ms_kDataPath.size()-1; 0<=i; --i)
	{
		FvUInt32 uiVal = ms_kDataPath[i];

		if(uiVal < 128)
		{
			kBits.Add(1, 0);
			kBits.Add(7, uiVal);
		}
		else if(uiVal < 16384)
		{
			kBits.Add(2, 2);
			kBits.Add(14, uiVal);
		}
		else
		{
			FV_ASSERT(uiVal < 0x40000000);
			kBits.Add(2, 3);
			kBits.Add(30, uiVal);
		}
	}
	ms_kDataPath.clear();

	FvInt32 iUsed = kBits.UsedBytes();
	if(iUsed > 0)
	{
		memcpy(ms_kDataStream.Reserve(iUsed), kBits.m_aBytes, iUsed);//! TODO: 把流嵌入到BitWriter,避免cpy
	}

	return uiMessageID;
}
#endif


FvUInt16 FvDataOwner::DoDiffRenovate(FvInt32 iMessageID, FvBinaryIStream& kIS, FvEntity* pkEntity, void* pkRoot, FvUInt16 uiDataIDCnt, DataPath* pkPath, DataCallBackInfo* pkCBs)
{
	FV_ASSERT(0<=iMessageID && iMessageID<64);

	OpCode uiOpCode = OpCode(iMessageID & 0x03);

	int iID;
	bool bRet = FvDataDecompressIDFull<4>(uiDataIDCnt, iID, (iMessageID>>2), kIS);
	FV_ASSERT(bRet);

	FV_ASSERT(pkEntity && pkRoot && 0<=iID && iID<uiDataIDCnt && pkPath && pkCBs);
	DataPath& kPath = pkPath[iID];
	FvDataOwner* pkOwner = (FvDataOwner*)((char*)pkRoot + kPath.uiAddr);

	DataCallBackInfo& kDataCBInfo = pkCBs[iID];
	FvUInt8 uiCBType = kDataCBInfo.uiCBType;//! 0:无回调,1:本节点回调,2:父节点回调,3:父节点Array回调,4:Pack回调
	FvDataCallBack* pkCB = kDataCBInfo.pkCallBack;

	FvDataCallBack::ms_bOwnCB = false;
	FvDataCallBack::ms_pkEntity = pkEntity;
	FvDataCallBack::ms_kCBDataPath.clear();
	FvDataCallBack::ms_uiCBIndex = INVALID_DATACALLBACK_INDEX;
	FvDataCallBack::ms_pkCBObj = NULL;

#ifndef FV_SHIPPING
	FV_INFO_MSG("\tDataID:%d, CBType:%d, RemainLen:%d\n", iID+1, uiCBType, kIS.RemainingLength());
#endif

	if(uiCBType == 4)//! Pack回调
	{
		FvDataCallBack::ms_uiCBIndex = kPath.uiPackIdx;
		FvDataCallBack::ms_pkCBObj = (FvDataObj*)pkOwner;
		FV_ASSERT(pkCB && FvDataCallBack::ms_uiCBIndex!=0xFFFF && kDataCBInfo.uiCBDataID==0xFFFF);
	}
	else if(uiCBType == 2)//! 父节点回调
	{
		DataPath& kCBOwnPath = pkPath[kDataCBInfo.uiCBDataID -1];
		FV_ASSERT(kDataCBInfo.uiCBDataID < uiDataIDCnt+1 && kDataCBInfo.uiCBDataID < iID+1 && kCBOwnPath.uiPathLen <= kPath.uiPathLen);
		FvDataCallBack::ms_pkCBObj = (FvDataObj*)((char*)pkRoot + kCBOwnPath.uiAddr);

		if(kCBOwnPath.uiPathLen > 0)
		{
			FV_ASSERT((FvDataOwner*)(FvDataCallBack::ms_pkCBObj) == pkOwner);
			FV_ASSERT(kCBOwnPath.pkPath && kCBOwnPath.pkPath[0]==0xFFFF);

			FvUInt8 uiPathLen = kCBOwnPath.uiPathLen -1;
			if(kPath.pkPath[uiPathLen] == kCBOwnPath.pkPath[uiPathLen])
				++uiPathLen;

			BitReader kBits(kIS);
			FvUInt8 idx(0);
			for(; idx<uiPathLen; ++idx)
			{
				FV_ASSERT(kPath.pkPath[idx] == kCBOwnPath.pkPath[idx]);
				if(kPath.pkPath[idx] == 0xFFFF)
				{
					FvUInt32 uiPathVal;
					if(kBits.Get(1) == 0)
						uiPathVal = FvUInt32(kBits.Get(7));
					else if(kBits.Get(1) == 0)
						uiPathVal = FvUInt32(kBits.Get(14));
					else
						uiPathVal = FvUInt32(kBits.Get(30));

					pkOwner = pkOwner->DataVassal(uiPathVal);
					FvDataCallBack::ms_kCBDataPath.push_back(uiPathVal);
				}
				else
				{
					pkOwner = pkOwner->DataVassal(kPath.pkPath[idx]);
				}
			}
			FV_ASSERT(!kIS.Error());

			if(uiPathLen != kCBOwnPath.uiPathLen)
			{
				FV_ASSERT(kCBOwnPath.pkPath[idx] != 0xFFFF);
				FvDataCallBack::ms_pkCBObj = (FvDataObj*)pkOwner->DataVassal(kCBOwnPath.pkPath[idx]);
			}
			else
			{
				FvDataCallBack::ms_pkCBObj = (FvDataObj*)pkOwner;
			}

			uiPathLen = kPath.uiPathLen;
			for(; idx<uiPathLen; ++idx)
			{
				if(kPath.pkPath[idx] == 0xFFFF)
				{
					FvUInt32 uiPathVal;
					if(kBits.Get(1) == 0)
						uiPathVal = FvUInt32(kBits.Get(7));
					else if(kBits.Get(1) == 0)
						uiPathVal = FvUInt32(kBits.Get(14));
					else
						uiPathVal = FvUInt32(kBits.Get(30));

					pkOwner = pkOwner->DataVassal(uiPathVal);
				}
				else
				{
					pkOwner = pkOwner->DataVassal(kPath.pkPath[idx]);
				}
			}
			FV_ASSERT(!kIS.Error());
		}
		else if(kPath.uiPathLen > 0)
		{
			FV_ASSERT(kPath.pkPath && kPath.pkPath[0]==0xFFFF);

			BitReader kBits(kIS);
			for(FvUInt8 idx(0); idx<kPath.uiPathLen; ++idx)
			{
				if(kPath.pkPath[idx] == 0xFFFF)
				{
					FvUInt32 uiPathVal;
					if(kBits.Get(1) == 0)
						uiPathVal = FvUInt32(kBits.Get(7));
					else if(kBits.Get(1) == 0)
						uiPathVal = FvUInt32(kBits.Get(14));
					else
						uiPathVal = FvUInt32(kBits.Get(30));

					pkOwner = pkOwner->DataVassal(uiPathVal);
				}
				else
				{
					pkOwner = pkOwner->DataVassal(kPath.pkPath[idx]);
				}
			}
			FV_ASSERT(!kIS.Error());
		}

		FV_ASSERT(pkCB);
	}
	else if(uiCBType == 3)//! 父节点Array回调
	{
		DataPath& kCBOwnPath = pkPath[kDataCBInfo.uiCBDataID -1];
		FV_ASSERT(kDataCBInfo.uiCBDataID < uiDataIDCnt+1 && kDataCBInfo.uiCBDataID < iID+1 && kCBOwnPath.uiPathLen < kPath.uiPathLen);
		FvDataCallBack::ms_pkCBObj = (FvDataObj*)((char*)pkRoot + kCBOwnPath.uiAddr);

		if(kCBOwnPath.uiPathLen > 0)
		{
			FV_ASSERT((FvDataOwner*)(FvDataCallBack::ms_pkCBObj) == pkOwner);
			FV_ASSERT(kCBOwnPath.pkPath && kCBOwnPath.pkPath[0]==0xFFFF);

			FvUInt8 uiPathLen = kCBOwnPath.uiPathLen;

			BitReader kBits(kIS);
			FvUInt8 idx(0);
			for(; idx<uiPathLen; ++idx)
			{
				FV_ASSERT(kPath.pkPath[idx] == kCBOwnPath.pkPath[idx]);
				if(kPath.pkPath[idx] == 0xFFFF)
				{
					FvUInt32 uiPathVal;
					if(kBits.Get(1) == 0)
						uiPathVal = FvUInt32(kBits.Get(7));
					else if(kBits.Get(1) == 0)
						uiPathVal = FvUInt32(kBits.Get(14));
					else
						uiPathVal = FvUInt32(kBits.Get(30));

					pkOwner = pkOwner->DataVassal(uiPathVal);
					FvDataCallBack::ms_kCBDataPath.push_back(uiPathVal);
				}
				else
				{
					pkOwner = pkOwner->DataVassal(kPath.pkPath[idx]);
				}
			}
			FV_ASSERT(!kIS.Error());

			FvDataCallBack::ms_pkCBObj = (FvDataObj*)pkOwner;
			FV_ASSERT(kPath.pkPath[idx] == 0xFFFF);
			{
				FvUInt32 uiPathVal;
				if(kBits.Get(1) == 0)
					uiPathVal = FvUInt32(kBits.Get(7));
				else if(kBits.Get(1) == 0)
					uiPathVal = FvUInt32(kBits.Get(14));
				else
					uiPathVal = FvUInt32(kBits.Get(30));

				pkOwner = pkOwner->DataVassal(uiPathVal);
				FvDataCallBack::ms_uiCBIndex = uiPathVal;
				++idx;
			}

			uiPathLen = kPath.uiPathLen;
			for(; idx<uiPathLen; ++idx)
			{
				if(kPath.pkPath[idx] == 0xFFFF)
				{
					FvUInt32 uiPathVal;
					if(kBits.Get(1) == 0)
						uiPathVal = FvUInt32(kBits.Get(7));
					else if(kBits.Get(1) == 0)
						uiPathVal = FvUInt32(kBits.Get(14));
					else
						uiPathVal = FvUInt32(kBits.Get(30));

					pkOwner = pkOwner->DataVassal(uiPathVal);
				}
				else
				{
					pkOwner = pkOwner->DataVassal(kPath.pkPath[idx]);
				}
			}
			FV_ASSERT(!kIS.Error());
		}
		else if(kPath.uiPathLen > 0)
		{
			FV_ASSERT(kPath.pkPath && kPath.pkPath[0]==0xFFFF);

			BitReader kBits(kIS);
			FvUInt8 idx(0);
			{
				FvUInt32 uiPathVal;
				if(kBits.Get(1) == 0)
					uiPathVal = FvUInt32(kBits.Get(7));
				else if(kBits.Get(1) == 0)
					uiPathVal = FvUInt32(kBits.Get(14));
				else
					uiPathVal = FvUInt32(kBits.Get(30));

				pkOwner = pkOwner->DataVassal(uiPathVal);
				FvDataCallBack::ms_uiCBIndex = uiPathVal;
				++idx;
			}

			for(; idx<kPath.uiPathLen; ++idx)
			{
				if(kPath.pkPath[idx] == 0xFFFF)
				{
					FvUInt32 uiPathVal;
					if(kBits.Get(1) == 0)
						uiPathVal = FvUInt32(kBits.Get(7));
					else if(kBits.Get(1) == 0)
						uiPathVal = FvUInt32(kBits.Get(14));
					else
						uiPathVal = FvUInt32(kBits.Get(30));

					pkOwner = pkOwner->DataVassal(uiPathVal);
				}
				else
				{
					pkOwner = pkOwner->DataVassal(kPath.pkPath[idx]);
				}
			}
			FV_ASSERT(!kIS.Error());
		}

		FV_ASSERT(pkCB);
	}
	else//! 本节点回调 or 无回调
	{
		if(kPath.uiPathLen)
		{
			FV_ASSERT(kPath.pkPath && kPath.pkPath[0]==0xFFFF);

			BitReader kBits(kIS);
			for(FvUInt8 idx(0); idx<kPath.uiPathLen; ++idx)
			{
				if(kPath.pkPath[idx] == 0xFFFF)
				{
					FvUInt32 uiPathVal;
					if(kBits.Get(1) == 0)
						uiPathVal = FvUInt32(kBits.Get(7));
					else if(kBits.Get(1) == 0)
						uiPathVal = FvUInt32(kBits.Get(14));
					else
						uiPathVal = FvUInt32(kBits.Get(30));

					pkOwner = pkOwner->DataVassal(uiPathVal);
					FvDataCallBack::ms_kCBDataPath.push_back(uiPathVal);
				}
				else
				{
					pkOwner = pkOwner->DataVassal(kPath.pkPath[idx]);
				}
			}
			FV_ASSERT(!kIS.Error());
		}

		if(uiCBType == 1)//! 本节点回调
		{
			FvDataCallBack::ms_bOwnCB = true;
			FvDataCallBack::ms_pkCBObj = (FvDataObj*)pkOwner;
			FV_ASSERT(pkCB && kDataCBInfo.uiCBDataID-1==iID);
		}
		else//! 无回调
		{
			FV_ASSERT(!pkCB && kDataCBInfo.uiCBDataID==0xFFFF);
		}
	}

#ifndef FV_SHIPPING
	FV_INFO_MSG("\tFindOwner, RemainLen:%d\n", kIS.RemainingLength());
#endif

	pkOwner->DataRenovate(pkCB, uiOpCode, kIS);
	FV_ASSERT(!kIS.Error());

	return FvUInt16(iID + 1);
}
