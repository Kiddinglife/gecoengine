#include "FvObj.h"
#include "FvDataDescription.h"
#include "FvPropertyOwnerBase.h"

FvPropertyOwnerBase::FvPropertyOwnerBase()
:m_pkOwner(NULL)
,m_uiOwnerRef(0)
#ifndef FV_SERVER
,m_uiFlag(0)
#endif
{

}

void FvPropertyOwnerBase::SetNewObj()
{
#ifndef FV_SERVER
	m_uiFlag |= 0x04;
#endif
}

bool FvPropertyOwnerBase::IsNewObj()
{
#ifndef FV_SERVER
	return m_uiFlag & 0x04;
#else
	return false;
#endif
}

void FvPropertyOwnerBase::SetDirty()
{
#ifndef FV_SERVER
	m_uiFlag |= 0x01;
#endif
}
bool FvPropertyOwnerBase::IsDirty()
{
#ifndef FV_SERVER
	return m_uiFlag & 0x01;
#else
	return false;
#endif
}
void FvPropertyOwnerBase::SetDiscard()
{
#ifndef FV_SERVER
	m_uiFlag |= 0x02;
#endif
}
bool FvPropertyOwnerBase::IsDiscard()
{
#ifndef FV_SERVER
	return m_uiFlag & 0x02;
#else
	return false;
#endif
}
void FvPropertyOwnerBase::ClearDirtyFlag()
{
#ifndef FV_SERVER
	m_uiFlag &= 0xFE;
#endif
}
void FvPropertyOwnerBase::ClearDiscardFlag()
{
#ifndef FV_SERVER
	m_uiFlag &= 0xFD;
#endif
}
void FvPropertyOwnerBase::ClearNewObjFlag()
{
#ifndef FV_SERVER
	m_uiFlag &= 0xFB;
#endif
}

void FvPropertyOwnerBase::PropertyChanged(FvPropertyOperator op, FvUInt32 opIdx,
								  const ChangeVal& oldVal, const ChangeVal& newVal, ChangePath& path)
{
	if(m_pkOwner)
	{
		path.push_back(m_uiOwnerRef);
		m_pkOwner->PropertyChanged(op, opIdx, oldVal, newVal, path);
	}
}

class BitWriter
{
public:
	BitWriter() : m_iByteCount( 0 ), m_iBitsLeft( 8 )
	{
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

FvUInt8 FvPropertyOwnerBase::AddToStream(FvPropertyOperator op, FvUInt32 opIdx,
										 const ChangeVal& val, const ChangePath& path,
										 FvBinaryOStream& stream, bool bCompressPath)
{
	FvUInt8 messageID = (FvUInt8)~0U;

	if (!bCompressPath)
	{
		stream << (FvUInt8)op;
		stream << opIdx;

		FvUInt8 pathSize = (FvUInt8)path.size();
		stream << pathSize;
		for (FvUInt8 i = 0; i < pathSize; ++i)
			stream << path[i];
	}
	else
	{
		FvInt32 pathSize = path.size();

		BitWriter bits;
		bits.Add( 2, op );

		if (opIdx < 61 && pathSize == 0)
		{
			messageID = (FvUInt8)opIdx;
		}
		else
		{
			messageID = 61;

			if (opIdx < 64)
			{
				bits.Add( 1, 0 );
				bits.Add( 6, opIdx );
			}
			else if (opIdx < 8192)
			{
				bits.Add( 2, 2 );
				bits.Add( 13, opIdx );
			}
			else
			{
				bits.Add( 2, 3 );
				bits.Add( 32, opIdx );
			}

			if(pathSize == 0)
			{
				bits.Add( 1, 0 );
			}
			else
			{
				bits.Add( 1, 1 );

				FvPropertyOwnerBase * pOwner = this;
				for (FvInt32 i = pathSize-1; i >= 0; --i)
				{
					FvUInt32 curIdx = path[i];

					if (i != (FvInt32)path.size()-1)
						bits.Add( 1, 1 );

					FvInt32 maxIdx = pOwner->PropertyDivisions();
					if (maxIdx > 1)
					{
						maxIdx--;
						register FvInt32 nbits;
#ifdef _WIN32
						_asm bsr eax, maxIdx
							_asm mov nbits, eax
#else
						__asm__ (
							"bsr 	%1, %%eax"
							:"=a"	(nbits)
							:"r"	(maxIdx)
							);
#endif
						bits.Add( nbits+1, curIdx );
					}
					else if (maxIdx >= 0)
					{
					}
					else
					{
						if (curIdx < 64)
						{
							bits.Add( 1, 0 );
							bits.Add( 6, curIdx );
						}
						else if (curIdx < 8192)
						{
							bits.Add( 2, 2 );
							bits.Add( 13, curIdx );
						}
						else
						{
							bits.Add( 2, 3 );
							bits.Add( 32, curIdx );
						}
					}

					pOwner = pOwner->PropertyVassal( path[i] );
				}

				FV_ASSERT(pOwner);
				bits.Add( 1, 0 );
			}
		}

		FvInt32 used = bits.UsedBytes();
		FV_ASSERT(used);
		memcpy( stream.Reserve( used ), bits.m_aBytes, used );
	}

	if(val.first)
		val.first->DataType()->AddToStream(val.first, stream, false);
	if(val.second)
		val.second->DataType()->AddToStream(val.second, stream, false);

	return messageID;
}

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

			FvInt32 bitsTake = std::min( nbits-gbits, m_iBitsLeft );
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

FvPropertyOwnerBase * FvPropertyOwnerBase::GetPathFromStream(FvInt32 messageID, FvBinaryIStream& data,
															 FvPropertyOperator& op, FvUInt32& opIdx, ChangePath& path)
{
	FvPropertyOwnerBase * pOwner = this;
	pOwner->SetDirty();

	if (messageID == 255)
	{
		FvUInt8 tmp(0);
		data >> tmp;
		op = (FvPropertyOperator)tmp;
		data >> opIdx;

		FvUInt8 cpLen;
		data >> cpLen;
		path.assign( cpLen, 0 );
		for (FvUInt8 i = 0; i < cpLen; ++i)
			data >> path[i];

		for (int i = cpLen-1; i >= 0; --i)
		{
			pOwner = pOwner->PropertyVassal( path[i] );
			FV_ASSERT(pOwner);
			pOwner->SetDirty();
		}
	}
	else
	{
		BitReader bits( data );
		op = (FvPropertyOperator)bits.Get( 2 );

		if (messageID < 61)
		{
			opIdx = (FvUInt32)messageID;
		}
		else
		{
			if (bits.Get( 1 ) == 0)
			{
				opIdx = bits.Get( 6 );
			}
			else if (bits.Get( 1 ) == 0)
			{
				opIdx = bits.Get( 13 );
			}
			else
			{
				opIdx = bits.Get( 32 );
			}

			if (bits.Get( 1 ) != 0)
			{
				FvInt32 lpath[256];
				FvInt32 * ppath = lpath;

				while (1)
				{
					FvUInt32 curIdx;
					FvInt32 maxIdx = pOwner->PropertyDivisions();
					if (maxIdx > 1)
					{
						maxIdx--;
						register FvInt32 nbits;
#ifdef _WIN32
						_asm bsr eax, maxIdx
							_asm mov nbits, eax
#else
						__asm__ (
							"bsr 	%1, %%eax"
							:"=a"	(nbits)
							:"r"	(maxIdx)
							);
#endif
						curIdx = bits.Get( nbits+1 );
					}
					else if (maxIdx >= 0)
					{
						curIdx = 0;
					}
					else
					{
						if (bits.Get( 1 ) == 0)
						{
							curIdx = bits.Get( 6 );
						}
						else if (bits.Get( 1 ) == 0)
						{
							curIdx = bits.Get( 13 );
						}
						else
						{
							curIdx = bits.Get( 32 );
						}
					}

					*ppath++ = curIdx;

					pOwner = pOwner->PropertyVassal( curIdx );
					FV_ASSERT(pOwner);
					pOwner->SetDirty();

					if (bits.Get( 1 ) == 0) break;
				}

				path.assign( ppath-lpath, 0 );
				for (FvUInt32 i = 0; i < path.size(); ++i)
					path[i] = *--ppath;
			}
		}
	}

	return pOwner;
}



