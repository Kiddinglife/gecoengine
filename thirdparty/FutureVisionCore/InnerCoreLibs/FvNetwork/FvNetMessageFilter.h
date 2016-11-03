//{future header message}
#ifndef __FvNetMessageFilter_H__
#define __FvNetMessageFilter_H__

#include "FvNetTypes.h"
#include "FvNetInterfaces.h"

#include <FvBinaryStream.h>
#include <FvSmartPointer.h>

class FvNetMessageFilter: public FvReferenceCount
{
public:

	FvNetMessageFilter(): FvReferenceCount()
	{}

	virtual ~FvNetMessageFilter() {}

	virtual void FilterMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		FvNetInputMessageHandler * pHandler ) = 0;
};

typedef FvSmartPointer< FvNetMessageFilter > FvNetMessageFilterPtr;

#endif // __FvNetMessageFilter_H__
