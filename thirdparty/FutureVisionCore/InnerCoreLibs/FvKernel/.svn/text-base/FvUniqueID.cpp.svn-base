#include "FvUniqueID.h"
#include "FvDebug.h"

#if defined( _WIN32 ) && !defined( _XBOX360 )
#include <objbase.h>
#endif

FV_DECLARE_DEBUG_COMPONENT2( "FvKernel", 0 )

FvUniqueID FvUniqueID::ms_kZero( 0, 0, 0, 0 );

FvUniqueID::FvUniqueID() :
m_uiA( 0 ), m_uiB( 0 ), m_uiC( 0 ), m_uiD( 0 )
{
}

FvUniqueID::FvUniqueID( const FvString& s ) :
m_uiA( 0 ), m_uiB( 0 ), m_uiC( 0 ), m_uiD( 0 )
{
	if (!s.empty())
	{
		unsigned int data[4];
		if (FromString(s, &data[0]))
		{
			m_uiA = data[0]; m_uiB = data[1]; m_uiC = data[2]; m_uiD = data[3];
		}
		else
		{
			FV_ERROR_MSG( "Error parsing UniqueID : %s\n", s.c_str() );
		}
	}
}

FvUniqueID::operator FvString() const
{
	char buf[80];
	FvSNPrintf( buf, sizeof(buf), "%08X.%08X.%08X.%08X", m_uiA, m_uiB, m_uiC, m_uiD );
	return FvString( buf );
}

bool FvUniqueID::operator==( const FvUniqueID &kOther ) const
{
	return (m_uiA == kOther.m_uiA) && (m_uiB == kOther.m_uiB) && (m_uiC == kOther.m_uiC) && (m_uiD == kOther.m_uiD);
}

bool FvUniqueID::operator!=( const FvUniqueID &kOther ) const
{
	return !(*this == kOther);
}

bool FvUniqueID::operator<( const FvUniqueID &kOther ) const
{
	if (m_uiA < kOther.m_uiA)
		return true;
	else if (m_uiA > kOther.m_uiA)
		return false;

	if (m_uiB < kOther.m_uiB)
		return true;
	else if (m_uiB > kOther.m_uiB)
		return false;

	if (m_uiC < kOther.m_uiC)
		return true;
	else if (m_uiC > kOther.m_uiC)
		return false;

	if (m_uiD < kOther.m_uiD)
		return true;
	else if (m_uiD > kOther.m_uiD)
		return false;

	return false;
}

#if defined( _WIN32 ) && !defined( _XBOX360 )
FvUniqueID FvUniqueID::Generate()
{
	FvUniqueID n;
#ifndef STATION_POSTPROCESSOR
	if (FAILED(CoCreateGuid( reinterpret_cast<GUID*>(&n) )))
		FV_CRITICAL_MSG( "Couldn't create GUID" );
#endif
	return n;
}
#endif

bool FvUniqueID::IsUniqueID( const FvString &kStr )
{
	unsigned int auiData[4];
	return FromString( kStr, &auiData[0] );
}

bool FvUniqueID::FromString( const FvString &kStr, unsigned int *pcData )
{
	if (kStr.empty())
		return false;

	FvString kCopyS(kStr);
	char *pcStr = const_cast<char*>(kCopyS.c_str());
	for (int iOffset = 0; iOffset < 4; iOffset++)
	{
		char* initstr = pcStr;
		pcData[iOffset] = strtoul(initstr, &pcStr, 16);

		if (initstr == pcStr)
		{
			return false;
		}

		pcStr++;
	}

	return true;
}
