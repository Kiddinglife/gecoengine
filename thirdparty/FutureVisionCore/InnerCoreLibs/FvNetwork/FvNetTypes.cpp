#include "FvNetTypes.h"
#include "FvBinaryStream.h"
#include "FvWatcher.h"

#ifdef _WIN32
#ifndef _XBOX360
#include <Winsock.h>
#endif
#elif defined( PLAYSTATION3 )
#include <netinet/in.h>
#else
#include <arpa/inet.h>
#endif

FV_DECLARE_DEBUG_COMPONENT2( "FvNetwork", 0 )


char FvNetAddress::ms_pcStringBuf[ 2 ][ FvNetAddress::MAX_STRLEN ];
int FvNetAddress::ms_iCurrStringBuf = 0;
const FvNetAddress FvNetAddress::NONE( 0, 0 );

FvNetAddress::FvNetAddress(char* str)
:m_uiIP(0)
,m_uiPort(0)
,m_uiSalt(0)
{
	SetFromString(str);
}

int FvNetAddress::WriteToString( char * str, int length ) const
{
	FvUInt32	hip = ntohl( m_uiIP );
	FvUInt16	hport = ntohs( m_uiPort );

	return FvSNPrintf( str, length,
		"%d.%d.%d.%d:%d",
		(int)(unsigned char)(hip>>24),
		(int)(unsigned char)(hip>>16),
		(int)(unsigned char)(hip>>8),
		(int)(unsigned char)(hip),
		(int)hport );
}

char * FvNetAddress::c_str() const
{
	char * buf = FvNetAddress::NextStringBuf();
	this->WriteToString( buf, MAX_STRLEN );
    return buf;
}

const char * FvNetAddress::IPAsString() const
{
	FvUInt32	hip = ntohl( m_uiIP );
	char * buf = FvNetAddress::NextStringBuf();

	FvSNPrintf( buf, MAX_STRLEN,
		"%d.%d.%d.%d",
		(int)(unsigned char)(hip>>24),
		(int)(unsigned char)(hip>>16),
		(int)(unsigned char)(hip>>8),
		(int)(unsigned char)(hip) );

    return buf;
}

bool FvNetAddress::SetFromString(char* str)
{
	if(!str)
		return false;

	int a1, a2, a3, a4, a5;

	if(sscanf(str, "%d.%d.%d.%d:%d",
		&a1, &a2, &a3, &a4, &a5) != 5)
		return false;

	m_uiIP = (a1 << 24)|(a2 << 16)|(a3 << 8)|a4;

	m_uiPort = FvUInt16(a5);
	m_uiPort = ntohs(m_uiPort);
	m_uiIP = ntohl(m_uiIP);

	return true;
}

#if FV_ENABLE_WATCHERS
FvWatcher & FvNetAddress::GetWatcher()
{
	static FvMemberWatcher<char *,FvNetAddress>	* watchMe = NULL;

	if (watchMe == NULL)
	{
		watchMe = new FvMemberWatcher<char *,FvNetAddress>(
			*((FvNetAddress*)NULL),
			&FvNetAddress::operator char*,
			static_cast< void (FvNetAddress::*)( char* ) >( NULL )
			);
	}

	return *watchMe;
}
#endif // FV_ENABLE_WATCHERS


char * FvNetAddress::NextStringBuf()
{
	ms_iCurrStringBuf = (ms_iCurrStringBuf + 1) % 2;
	return ms_pcStringBuf[ ms_iCurrStringBuf ];
}


