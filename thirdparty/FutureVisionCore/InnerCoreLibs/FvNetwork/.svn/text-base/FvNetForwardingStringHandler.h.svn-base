//{future header message}
#ifndef __FvNetForwardingStringHandler_H__
#define __FvNetForwardingStringHandler_H__

#include "FvNetwork.h"
#include "FvNetFormatStringHandler.h"
#include "FvNetBSDSNPrintf.h"

#include <stdarg.h>
#include <vector>
#include <FvBaseTypes.h>
#include <FvMemoryStream.h>

void TestStrings();

class FV_NETWORK_API FvNetForwardingStringHandler : public FvNetFormatStringHandler
{
public:
	FvNetForwardingStringHandler( const char *fmt, bool isSuppressible = false );

	virtual void OnToken( char type, int cflags, int min, int max,
		int flags, FvUInt8 base, int vflags );

	void ParseArgs( va_list argPtr, FvMemoryOStream &os );
	const FvString & fmt() const { return m_kFMT; }

	unsigned NumRecentCalls() const { return m_uiNumRecentCalls; }
	void AddRecentCall() { ++m_uiNumRecentCalls; }
	void ClearRecentCalls() { m_uiNumRecentCalls = 0; }

	bool IsSuppressible() const { return m_bIsSuppressible; }
	void IsSuppressible( bool b ) { m_bIsSuppressible = b; }

protected:

	class FormatData
	{
	public:
		FormatData( char type, FvUInt8 cflags, FvUInt8 vflags ) :
			type_( type ), cflags_( cflags ), vflags_( vflags ) {}

		char type_;
		unsigned cflags_:4;
		unsigned vflags_:4;
	};

	void ProcessToken( char type, int cflags );

	FvString m_kFMT;

	std::vector< FormatData > m_kFMTData;

	unsigned m_uiNumRecentCalls;

	bool m_bIsSuppressible;
};

#endif // __FvNetForwardingStringHandler_H__
