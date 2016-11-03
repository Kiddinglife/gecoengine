//{future header message}
#ifndef __FvUniqueID_H__
#define __FvUniqueID_H__

#include "FvKernel.h"
#include "FvBaseTypes.h"
#include <FvPrintf.h>
#include "FvBinaryStream.h"

class FV_KERNEL_API FvUniqueID
{
private:
	unsigned int m_uiA;
	unsigned int m_uiB;
	unsigned int m_uiC;
	unsigned int m_uiD;
	
	static FvUniqueID ms_kZero;

public:
	FvUniqueID();
	FvUniqueID( const FvString& s );
	FvUniqueID( unsigned int a, unsigned int b, unsigned int c, unsigned int d ) : 
		m_uiA( a ), m_uiB( b ), m_uiC( c ), m_uiD( d )
	{}
	
	unsigned int GetA() const	{ return m_uiA; }
	unsigned int GetB() const	{ return m_uiB; }
	unsigned int GetC() const	{ return m_uiC; }
	unsigned int GetD() const	{ return m_uiD; }

	FvString ToString() const {return *this;}
	operator FvString() const;

	bool operator==( const FvUniqueID &kOther ) const;
	bool operator!=( const FvUniqueID &kOther ) const;
	bool operator<( const FvUniqueID &kOther ) const;

#ifdef _WIN32
	static FvUniqueID Generate();
#endif

	static const FvUniqueID& Zero() { return ms_kZero; }

    static bool IsUniqueID( const FvString &kStr );

private:
    static bool FromString( const FvString &kStr, unsigned int *pcData );
};

FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvUniqueID)

#endif // __FvUniqueID_H__