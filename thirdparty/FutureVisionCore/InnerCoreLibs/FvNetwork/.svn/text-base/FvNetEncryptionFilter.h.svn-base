//{future header message}
#ifndef __FvNetEncryptionFilter_H__
#define __FvNetEncryptionFilter_H__

#if !defined(FV_SERVER) //defined( WIN32 ) && !defined( _XBOX360 )
#define FV_USE_OPENSSL
#endif

#include "FvNetwork.h"
#include "FvNetPacketFilter.h"

#include <FvBaseTypes.h>
#include <FvSmartPointer.h>
#include <FvMemoryStream.h>

#include "openssl/blowfish.h"
#include <string>

class FvProfileVal;

#ifdef FV_USE_OPENSSL

class FV_NETWORK_API FvNetEncryptionFilter : public FvNetPacketFilter
{
public:
	static const int BLOCK_SIZE = 64 / 8;

	static const int MIN_KEY_SIZE = 128 / 8;

	typedef FvString Key;

	FvNetEncryptionFilter( const Key & key );
	FvNetEncryptionFilter( int keySize = MIN_KEY_SIZE );
	~FvNetEncryptionFilter();

	virtual FvNetReason Send( FvNetNub & nub, const FvNetAddress & addr, FvNetPacket * pPacket );
	virtual FvNetReason Recv( FvNetNub & nub, const FvNetAddress & addr, FvNetPacket * pPacket );

	virtual int MaxSpareSize();

	const Key & GetKey() const { return m_kKey; }
	const char * readableKey() const;
	bool IsGood() const { return m_bIsGood; }

	BF_KEY * pBFKey() { return (BF_KEY*)m_pkEncryptionObject; }

	void EncryptStream( FvMemoryOStream & clearStream,
		FvBinaryOStream & cipherStream );

	void DecryptStream( FvBinaryIStream & cipherStream,
		FvBinaryOStream & clearStream );

private:
	int Encrypt( const unsigned char * src, unsigned char * dest, int length );
	int Decrypt( const unsigned char * src, unsigned char * dest, int length );

	bool InitKey();

	Key m_kKey;
	int m_iKeySize;
	bool m_bIsGood;

	void *m_pkEncryptionObject;

	static FvProfileVal *ms_pkProfileVal;
};

#else // FV_USE_OPENSSL

class FV_NETWORK_API FvNetEncryptionFilter : public FvNetPacketFilter
{
public:
	typedef FvString Key;
	const Key GetKey() const { return FvString(""); }
};

#endif // !FV_USE_OPENSSL

typedef FvSmartPointer< FvNetEncryptionFilter > FvNetEncryptionFilterPtr;

#endif // __FvNetEncryptionFilter_H__
