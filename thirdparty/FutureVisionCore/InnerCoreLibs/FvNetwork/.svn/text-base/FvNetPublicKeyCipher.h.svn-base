//{future header message}
#ifndef __FvNetPublicKeyCipher_H__
#define __FvNetPublicKeyCipher_H__

#if !defined(FV_SERVER) // defined( WIN32 ) && !defined( _XBOX360 )
#define FV_USE_OPENSSL
#endif

#ifdef FV_USE_OPENSSL

#include "openssl/rsa.h"
#include "openssl/bio.h"
#include "openssl/pem.h"
#include <string>
#include <map>

#include "FvNetwork.h"

#include <FvBaseTypes.h>
#include <FvGlobalMacros.h>
#include <FvBinaryStream.h>

class FV_NETWORK_API FvNetPublicKeyCipher
{
public:
	FvNetPublicKeyCipher( bool isPrivate ) :
		m_pkRSA( NULL ),
		m_bHasPrivate( isPrivate )
	{}

	~FvNetPublicKeyCipher() { this->Cleanup(); }

	bool SetKey( const FvString & key );
	bool SetKeyFromResource( const FvString & path );

	typedef int (*EncryptionFunctionPtr)( int flen, const unsigned char * from,
		unsigned char * to, RSA * pRSA, int padding );

	int PublicEncrypt( FvBinaryIStream & clearStream,
		FvBinaryOStream & cipherStream )
	{
		return this->Encrypt( clearStream, cipherStream, &RSA_public_encrypt );
	}

	int PublicDecrypt( FvBinaryIStream & cipherStream,
		FvBinaryOStream & clearStream )
	{
		return this->Decrypt( cipherStream, clearStream, &RSA_public_decrypt );
	}

	int PrivateEncrypt( FvBinaryIStream & cipherStream,
		FvBinaryOStream & clearStream )
	{
		FV_ASSERT( m_bHasPrivate );
		return this->Encrypt( cipherStream, clearStream, &RSA_private_encrypt );
	}

	int PrivateDecrypt( FvBinaryIStream & cipherStream,
		FvBinaryOStream & clearStream )
	{
		FV_ASSERT( m_bHasPrivate );
		return this->Decrypt( cipherStream, clearStream, &RSA_private_decrypt );
	}

	bool IsGood() const { return m_pkRSA != NULL; }

	const char * c_str() const { return m_kReadableKey.c_str(); }

	const FvString & str() const { return m_kReadableKey; }

	int Size() const { return m_pkRSA ? RSA_size( m_pkRSA ) : -1; }

	int NumBits() const { return this->Size() * 8; }

	const char *type() const { return m_bHasPrivate ? "private" : "public"; }

	const char *err_str() const;

protected:
	void Cleanup();
	void SetReadableKey();

	int Encrypt( FvBinaryIStream & clearStream, FvBinaryOStream & cipherStream,
		EncryptionFunctionPtr pFunc );

	int Decrypt( FvBinaryIStream & cipherStream, FvBinaryOStream & clearStream,
		EncryptionFunctionPtr pFunc );

	RSA *m_pkRSA;

	FvString m_kReadableKey;

	bool m_bHasPrivate;
};

#endif // FV_USE_OPENSSL

#endif // __FvNetPublicKeyCipher_H__
