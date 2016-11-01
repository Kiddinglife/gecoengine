//{future header message}
#ifndef __FvDBInterfaceUtils_H__
#define __FvDBInterfaceUtils_H__

#include <FvNetChannel.h>

class FvNetNub;
class FvBinaryIStream;
class FvBinaryOStream;

namespace FvDBInterfaceUtils
{
	struct Blob
	{
		const char *m_pkBlob;
		FvUInt32 m_iLength;

		Blob() : m_pkBlob( NULL ), m_iLength( 0 ) {}	// NULL blob
		Blob( const char *pData, FvUInt32 len ) : m_pkBlob( pData ), m_iLength( len )
		{}

		bool IsNull() const 	{ return (m_pkBlob == NULL); }
	};

	void AddPotentialNullBlobToStream( FvBinaryOStream &stream, const Blob &blob );
	Blob GetPotentialNullBlobFromStream( FvBinaryIStream &stream );
}

namespace BaseAppIntInterface
{
	const FvUInt8 LOG_ON_ATTEMPT_REJECTED = 0;
	const FvUInt8 LOG_ON_ATTEMPT_TOOK_CONTROL = 1;
	const FvUInt8 LOG_ON_ATTEMPT_NOT_EXIST = 2;
}

#endif // __FvDBInterfaceUtils_H__
