//{future header message}
#ifndef __FvNetMask_H__
#define __FvNetMask_H__

#include "FvNetwork.h"

class FV_NETWORK_API FvNetMask
{
public:
	FvNetMask();

	bool Parse( const char* str );
	bool ContainsAddress( unsigned long addr ) const;

	void Clear();

private:
	unsigned long m_uiMask;
	int m_iBits;
};

#endif // __FvNetMask_H__
