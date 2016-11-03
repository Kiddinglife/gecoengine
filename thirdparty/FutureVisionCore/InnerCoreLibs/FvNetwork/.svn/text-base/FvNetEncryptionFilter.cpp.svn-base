#include "FvNetEncryptionFilter.h"
#include "FvNetPacket.h"
#include "FvNetTypes.h"
#include "FvProfile.h"
#include "FvNetChannel.h"
#include "FvNetNub.h"

#include "openssl/rand.h"

#ifdef FV_USE_OPENSSL

typedef FvUInt32 MagicType;

const MagicType ENCRYPTION_MAGIC = 0xdeadbeef;

FvProfileVal * FvNetEncryptionFilter::ms_pkProfileVal = NULL;

FvNetEncryptionFilter::FvNetEncryptionFilter( const Key & key ) :
	m_kKey( key ),
	m_iKeySize( key.size() )
{
	if (this->InitKey())
	{
		FV_DEBUG_MSG( "Using Blowfish key: %s\n", this->readableKey() );
	}
}


FvNetEncryptionFilter::FvNetEncryptionFilter( int keySize ) :
	m_kKey( keySize, 0 ),
	m_iKeySize( keySize )
{
	char * keyBytes = const_cast< char * >( m_kKey.c_str() );
	RAND_bytes( (unsigned char*)keyBytes, m_kKey.size() );

	if (this->InitKey())
	{
		FV_DEBUG_MSG( "Generated Blowfish key: %s\n", this->readableKey() );
	}
}


FvNetEncryptionFilter::~FvNetEncryptionFilter()
{
	delete this->pBFKey();
}


bool FvNetEncryptionFilter::InitKey()
{
#if FV_ENABLE_WATCHERS
	if (ms_pkProfileVal == NULL)
	{
		ms_pkProfileVal = new FvProfileVal( "Encrypt" );
	}
#endif // FV_ENABLE_WATCHERS
	FV_ASSERT( BLOCK_SIZE == sizeof( FvUInt64 ) );

	m_pkEncryptionObject = new BF_KEY;

	if ((m_iKeySize >= MIN_KEY_SIZE) && (m_iKeySize % BLOCK_SIZE == 0))
	{
		BF_set_key( this->pBFKey(), m_kKey.size(), (unsigned char*)m_kKey.c_str() );
		m_bIsGood = true;
	}
	else
	{
		FV_ERROR_MSG( "FvNetEncryptionFilter::InitKey: "
			"Tried to initialise filter with key of invalid length %d\n",
			m_iKeySize );

		m_bIsGood = false;
	}

	return m_bIsGood;
}


FvNetReason FvNetEncryptionFilter::Send( FvNetNub & nub, const FvNetAddress & addr,
		FvNetPacket * pPacket )
{
	int len = 0;
	FvNetPacketPtr toSend = NULL;
	FvUInt8 wastage = 0;

	{
		FV_SCOPED_PROFILER( *ms_pkProfileVal );

		if (!m_bIsGood)
		{
			FV_WARNING_MSG( "FvNetEncryptionFilter::Send: "
				"Dropping packet to %s due to invalid filter\n",
				addr.c_str() );

			return FV_NET_REASON_GENERAL_NETWORK;
		}

		toSend = new FvNetPacket();

		len = pPacket->TotalSize();

		len += sizeof( ENCRYPTION_MAGIC );
		wastage = ((BLOCK_SIZE - ((len + 1) % BLOCK_SIZE)) % BLOCK_SIZE) + 1;

		len += wastage;

		FV_ASSERT( len <= PACKET_MAX_SIZE );

		toSend->MsgEndOffset( len );

		FvUInt32 startWastage = len - 1;
		FvUInt32 startMagic = startWastage - sizeof( ENCRYPTION_MAGIC );

		pPacket->Data()[ startWastage ] = wastage;
		*(MagicType *)(pPacket->Data() + startMagic) = ENCRYPTION_MAGIC;

		this->Encrypt( (const unsigned char*)pPacket->Data(),
			(unsigned char*)toSend->Data(), len );
	}

	return this->FvNetPacketFilter::Send( nub, addr, toSend.GetObject() );
}

FvNetReason FvNetEncryptionFilter::Recv( FvNetNub & nub, const FvNetAddress & addr, FvNetPacket * pPacket )
{
	{
		FV_SCOPED_PROFILER( *ms_pkProfileVal );

		if (!m_bIsGood)
		{
			FV_WARNING_MSG( "FvNetEncryptionFilter::Recv: "
				"Dropping packet from %s due to invalid filter\n",
				addr.c_str() );

			return FV_NET_REASON_GENERAL_NETWORK;
		}

		int decryptLen = this->Decrypt( (const unsigned char*)pPacket->Data(),
			(unsigned char*)pPacket->Data(), pPacket->TotalSize() );

		if (decryptLen == -1)
		{
			return nub.ReceivedCorruptedPacket();
		}

		FvUInt32 startWastage = pPacket->TotalSize() - 1;

		if (startWastage < sizeof( ENCRYPTION_MAGIC ))
		{
			FV_ASSERT_DEV( !"Not enough wastage" );
			return nub.ReceivedCorruptedPacket();
		}

		FvUInt32 startMagic   = startWastage - sizeof( ENCRYPTION_MAGIC );
		FvUInt8 wastage = pPacket->Data()[ startWastage ];

		MagicType &packetMagic = *(MagicType *)(pPacket->Data() + startMagic);

		if (packetMagic != ENCRYPTION_MAGIC)
		{
			FV_WARNING_MSG( "FvNetEncryptionFilter::Recv: "
				"Dropping packet with invalid magic 0x%x (expected 0x%x)\n",
				packetMagic, ENCRYPTION_MAGIC );
			return nub.ReceivedCorruptedPacket();
		}

		int footerSize = wastage + sizeof( ENCRYPTION_MAGIC );
		if (wastage > BLOCK_SIZE || footerSize > pPacket->TotalSize())
		{
			FV_WARNING_MSG( "FvNetEncryptionFilter::Recv: "
				"Dropping packet from %s due to illegal wastage count (%d)\n",
				addr.c_str(), wastage );

			return nub.ReceivedCorruptedPacket();
		}

		pPacket->Shrink( footerSize );
	}

	return this->FvNetPacketFilter::Recv( nub, addr, pPacket );
}


void FvNetEncryptionFilter::EncryptStream( FvMemoryOStream & clearStream,
	FvBinaryOStream & cipherStream )
{
	if (clearStream.RemainingLength() % BLOCK_SIZE != 0)
	{
		int padSize = BLOCK_SIZE - (clearStream.RemainingLength() % BLOCK_SIZE);
		void * padding = clearStream.Reserve( padSize );
		memset( padding, 0, padSize );
	}

	int size = clearStream.RemainingLength();
	unsigned char * cipherText = (unsigned char*)cipherStream.Reserve( size );

	this->Encrypt( (unsigned char*)clearStream.Data(), cipherText, size );
}


void FvNetEncryptionFilter::DecryptStream( FvBinaryIStream & cipherStream,
	FvBinaryOStream & clearStream )
{
	int size = cipherStream.RemainingLength();
	unsigned char * cipherText = (unsigned char*)cipherStream.Retrieve( size );
	unsigned char * clearText =	(unsigned char*)clearStream.Reserve( size );

	this->Decrypt( cipherText, clearText, size );
}


int FvNetEncryptionFilter::MaxSpareSize()
{
	const int MTU_ALLOWANCE = 200;

	return BLOCK_SIZE + sizeof( ENCRYPTION_MAGIC ) + MTU_ALLOWANCE;
}


const char * FvNetEncryptionFilter::readableKey() const
{
	static char buf[ 1024 ];

	char *c = buf;

	for (int i=0; i < m_iKeySize; i++)
	{
		c += sprintf( c, "%02hhX ", (unsigned char)m_kKey[i] );
	}

	c[-1] = '\0';

	return buf;
}


int FvNetEncryptionFilter::Encrypt( const unsigned char * src, unsigned char * dest,
	int length )
{
	if (length % BLOCK_SIZE != 0)
	{
		FV_CRITICAL_MSG( "FvNetEncryptionFilter::Encrypt: "
			"Input length (%d) is not a multiple of block size (%d)\n",
			length, BLOCK_SIZE );
	}

	FvUInt64 * pPrevBlock = NULL;

	for (int i=0; i < length; i += BLOCK_SIZE)
	{
		if (pPrevBlock)
		{
			*(FvUInt64*)(dest + i) = *(FvUInt64*)(src + i) ^ (*pPrevBlock);
		}
		else
		{
			*(FvUInt64*)(dest + i) = *(FvUInt64*)(src + i);
		}

		BF_ecb_encrypt( dest + i, dest + i, this->pBFKey(), BF_ENCRYPT );
		pPrevBlock = (FvUInt64*)(src + i);
	}

	return length;
}


int FvNetEncryptionFilter::Decrypt( const unsigned char * src, unsigned char * dest,
	int length )
{
	if (length % BLOCK_SIZE != 0)
	{
		FV_WARNING_MSG( "FvNetEncryptionFilter::Decrypt: "
			"Input stream size (%d) is not a multiple of the block size (%d)\n",
			length, BLOCK_SIZE );

		return -1;
	}

	FvUInt64 * pPrevBlock = NULL;

	for (int i=0; i < length; i += BLOCK_SIZE)
	{
		BF_ecb_encrypt( src + i, dest + i, this->pBFKey(), BF_DECRYPT );

		if (pPrevBlock)
		{
			*(FvUInt64*)(dest + i) ^= *pPrevBlock;
		}

		pPrevBlock = (FvUInt64*)(dest + i);
	}

	return length;
}

#endif	// FV_USE_OPENSSL
