//{future header message}
#ifndef __FvNetFormatStringHandler_H__
#define __FvNetFormatStringHandler_H__

#include <FvPowerDefines.h>

class FvNetFormatStringHandler
{
public:
	virtual ~FvNetFormatStringHandler( void ) {};

	virtual void OnString( int start, int end ) {};

	virtual void OnToken( char type, int cflags, int min, int max,
		int flags, FvUInt8 base, int vflags ) = 0;
};

#endif // __FvNetFormatStringHandler_H__
