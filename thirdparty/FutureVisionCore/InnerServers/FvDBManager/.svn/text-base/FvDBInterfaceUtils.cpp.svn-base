#include "FvDBInterfaceUtils.h"
#include "FvDBInterface.h"

#include <FvDebug.h>
#include <FvBinaryStream.h>

FV_DECLARE_DEBUG_COMPONENT(0)

namespace FvDBInterfaceUtils
{
	void AddPotentialNullBlobToStream( FvBinaryOStream &stream, const Blob &blob )
	{
		if (blob.m_pkBlob && blob.m_iLength)
		{
			stream.AppendString( blob.m_pkBlob, blob.m_iLength );
		}
		else
		{
			stream.AppendString( "", 0 );
			stream << FvUInt8((blob.m_pkBlob) ? 1 : 0);
		}
	}

	Blob GetPotentialNullBlobFromStream( FvBinaryIStream &stream )
	{
		int length = stream.ReadStringLength();
		if (length > 0)
		{
			return Blob( (char*) stream.Retrieve( length ), length );
		}
		else
		{
			FvUInt8 isNotNull;
			stream >> isNotNull;

			return (isNotNull) ? Blob( "", 0 ) : Blob();
		}
	}
}
