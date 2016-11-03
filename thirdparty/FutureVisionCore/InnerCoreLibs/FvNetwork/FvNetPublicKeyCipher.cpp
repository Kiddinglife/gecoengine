#include "FvNetPublicKeyCipher.h"

#include "openssl/err.h"
#include "openssl/rand.h"

#define	RSA_PADDING	41	

#if defined( FV_USE_OPENSSL )

bool FvNetPublicKeyCipher::SetKey( const FvString & key )
{
	this->Cleanup();

	BIO *bio = BIO_new( BIO_s_mem() );
	BIO_puts( bio, key.c_str() );

	m_pkRSA = m_bHasPrivate ?
		PEM_read_bio_RSAPrivateKey( bio, NULL, NULL, NULL ) :
		PEM_read_bio_RSA_PUBKEY( bio, NULL, NULL, NULL );

	BIO_free( bio );

	if (m_pkRSA)
	{
		this->SetReadableKey();
		return true;
	}
	else
	{
		return false;
	}
}


#if !defined( PLAYSTATION3 ) && !defined( _XBOX360 )
bool FvNetPublicKeyCipher::SetKeyFromResource( const FvString & path )
{
	//DataSectionPtr pSection = BWResource::openSection( path );

	//if (pSection)
	//{
	//	BinaryPtr pBinData = pSection->asBinary();
	//	FvString keyStr( pBinData->cdata(), pBinData->len() );

	//	if (this->setKey( keyStr ))
	//	{
	//		FV_INFO_MSG( "PublicKeyCipher::setKeyFromResource: "
	//			"Loaded %d-bit %s key from %s\n",
	//			this->numBits(), this->type(), path.c_str() );
	//
	//		return true;
	//	}
	//	else
	//	{
	//		FV_ERROR_MSG( "PublicKeyCipher::setKeyFromResource: "
	//			"Failed to initialise RSA object: %s\n",
	//			this->err_str() );
	//
	//		return false;
	//	}
	//}
	//else
	//{
	//	FV_ERROR_MSG( "PublicKeyCipher::setKeyFromResource: "
	//		"Couldn't load private key from non-existant file %s\n",
	//		path.c_str() );

	//	return false;
	//}
	FV_ASSERT(false);
	return false;
}
#endif


int FvNetPublicKeyCipher::Encrypt( FvBinaryIStream & clearStream,
	FvBinaryOStream & cipherStream, EncryptionFunctionPtr pEncryptionFunc )
{

	int rsaModulusSize = RSA_size( m_pkRSA );	

	int partSize = rsaModulusSize - RSA_PADDING - 1;

	int encryptLen = 0;
	while (clearStream.RemainingLength())
	{
		int numBytesToRetrieve =
			std::min( partSize, clearStream.RemainingLength() );

		unsigned char * clearText = 
			(unsigned char*)clearStream.Retrieve( numBytesToRetrieve );

		unsigned char * cipherText =
			(unsigned char *)cipherStream.Reserve( rsaModulusSize );

		int curEncryptLen = 
			(*pEncryptionFunc)( 
				numBytesToRetrieve, clearText, cipherText, m_pkRSA, 
				RSA_PKCS1_OAEP_PADDING );
	
		FV_ASSERT( curEncryptLen == RSA_size( m_pkRSA ) );

		encryptLen += curEncryptLen; 
	}

	
	return encryptLen;
}


int FvNetPublicKeyCipher::Decrypt( FvBinaryIStream & cipherStream,
	FvBinaryOStream & clearStream, EncryptionFunctionPtr pEncryptionFunc )
{

	int rsaModulusSize = RSA_size( m_pkRSA );	

	unsigned char * clearText = 
		(unsigned char*) clearStream.Reserve( cipherStream.RemainingLength() );

	int clearTextSize = 0;
	while (cipherStream.RemainingLength())	
	{
		unsigned char * cipherText =
			(unsigned char *)cipherStream.Retrieve( rsaModulusSize  );

		if (cipherStream.Error())
		{
			FV_ERROR_MSG( "FvNetPublicKeyCipher::PrivateDecrypt: "
				"Not enough data on stream for encrypted data chunk\n" );

			return -1;
		}	

		int bytesDecrypted = 
			(*pEncryptionFunc)( RSA_size( m_pkRSA ), cipherText, clearText,
				m_pkRSA, RSA_PKCS1_OAEP_PADDING );	

		clearTextSize += bytesDecrypted;
		clearText += bytesDecrypted;
	}


	if (clearTextSize == 0)
	{
		return -1;
	}

	return  clearTextSize;
}


void FvNetPublicKeyCipher::Cleanup()
{
	if (m_pkRSA)
	{
		RSA_free( m_pkRSA );
		m_pkRSA = NULL;
	}
}


void FvNetPublicKeyCipher::SetReadableKey()
{
	BIO * bio = BIO_new( BIO_s_mem() );
	PEM_write_bio_RSA_PUBKEY( bio, m_pkRSA );

	char buf[ 1024 ];
	while (BIO_gets( bio, buf, sizeof( buf ) - 1 ) > 0)
	{
		m_kReadableKey.append( buf );
	}

	BIO_free( bio );
}


const char * FvNetPublicKeyCipher::err_str() const
{
	static bool errorStringsLoaded = false;

	if (!errorStringsLoaded)
	{
		ERR_load_crypto_strings();
	}

	return ERR_error_string( ERR_get_error(), 0 );
}

#endif	// FV_USE_OPENSSL

