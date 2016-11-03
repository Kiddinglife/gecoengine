#include "FvLoginInterface.h"
#include <FvDebug.h>
#include <FvMemoryStream.h>
#include <FvNetPublicKeyCipher.h>

bool FvLogOnParams::AddToStream( FvBinaryOStream & data, Flags flags,
	FvNetPublicKeyCipher * pKey )
{
	if (flags == PASS_THRU)
	{
		flags = m_uiFlags;
	}

	FvMemoryOStream clearText;

	FvBinaryOStream * pRawStream = pKey ? &clearText : &data;

	*pRawStream << flags << m_kUsername << m_kPassword << m_kEncryptionKey;

	if (flags & HAS_DIGEST)
	{
		*pRawStream << m_kDigest;
	}

	*pRawStream << m_uiNonce;

	*pRawStream << m_uiNonce;

//#ifdef FV_USE_OPENSSL
//	if (pKey && (pKey->PublicEncrypt( clearText, data ) == -1))
//	{
//		return false;
//	}
//#else
//	FV_ASSERT_DEV( !pKey );
//#endif

	return true;
}

bool FvLogOnParams::ReadFromStream( FvBinaryIStream & data,
	FvNetPublicKeyCipher * pKey )
{
	FvMemoryOStream clearText;

//#ifdef FV_USE_OPENSSL
//	if (pKey && (pKey->PrivateDecrypt( data, clearText ) == -1))
//	{
//		return false;
//	}
//#else
//	FV_ASSERT_DEV( !pKey );
//#endif

	FvBinaryIStream * pRawStream = pKey ? &clearText : &data;

	*pRawStream >> m_uiFlags >> m_kUsername >> m_kPassword >> m_kEncryptionKey;

	if (m_uiFlags & HAS_DIGEST)
	{
		*pRawStream >> m_kDigest;
	}

	if (pRawStream->RemainingLength())
	{
		*pRawStream >> m_uiNonce;
	} 
	else
	{
		m_uiNonce = 0;
	}

	if (pRawStream->RemainingLength())
	{
		*pRawStream >> m_uiNonce;
	} 
	else
	{
		m_uiNonce = 0;
	}

	clearText.Finish();

	return !data.Error();
}