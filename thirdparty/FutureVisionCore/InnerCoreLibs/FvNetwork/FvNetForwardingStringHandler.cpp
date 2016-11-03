#include "FvNetForwardingStringHandler.h"

#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvNetwork", 0 );

FvNetForwardingStringHandler::FvNetForwardingStringHandler(
		const char *fmt, bool isSuppressible ) :
	m_kFMT( fmt ),
	m_uiNumRecentCalls( 0 ),
	m_bIsSuppressible( isSuppressible )
{
	HandleFormatString( fmt, *this );
}

void FvNetForwardingStringHandler::OnToken( char type, int cflags, int min, int max,
	int flags, FvUInt8 base, int vflags )
{
	m_kFMTData.push_back( FormatData( type, cflags, vflags ) );
}

void FvNetForwardingStringHandler::ParseArgs( va_list argPtr, FvMemoryOStream &os )
{
	va_list args;
#ifdef _WIN32
	memcpy( &args, &argPtr, sizeof(va_list) );
#else
	va_copy( args, argPtr );
#endif

	for (unsigned i=0; i < m_kFMTData.size(); i++)
	{
		FormatData &fd = m_kFMTData[i];

		if (fd.vflags_ & FV_NET_VARIABLE_MIN_WIDTH)
			os << (FvNetWidthType)va_arg( args, int );
		if (fd.vflags_ & FV_NET_VARIABLE_MAX_WIDTH)
			os << (FvNetWidthType)va_arg( args, int );

		switch (fd.type_)
		{
		case 'd':
		{
			switch (fd.cflags_)
			{
			case FV_NET_DP_C_SHORT:
				os << (short)va_arg( args, int ); break;
			case FV_NET_DP_C_LONG:
				os << va_arg( args, long ); break;
			case FV_NET_DP_C_LLONG:
				os << va_arg( args, long long ); break;
			default:
				os << va_arg( args, int ); break;
			}
			break;
		}

		case 'o':
		case 'u':
		case 'x':
		{
			switch (fd.cflags_)
			{
			case FV_NET_DP_C_SHORT:
				os << (unsigned short)va_arg( args, unsigned int ); break;
			case FV_NET_DP_C_LONG:
				os << (long)va_arg( args, unsigned long ); break;
			case FV_NET_DP_C_LLONG:
				os << va_arg( args, unsigned long long ); break;
			default:
				os << va_arg( args, unsigned int ); break;
			}
			break;
		}

		case 'f':
		case 'e':
		case 'g':
		{
			switch (fd.cflags_)
			{
			case FV_NET_DP_C_LDOUBLE: os << va_arg( args, FV_NET_LDOUBLE ); break;
			default: os << va_arg( args, double ); break;
			}
			break;
		}

		case 's':
		{
			char *tmpArg = va_arg( args, char* );
			if (tmpArg == NULL)
			{
				os << "(null)";
			}
			else
			{
				os << tmpArg;
			}

			break;
		}

		case 'p':
		{
			//os << va_arg( args, void* );
			//! modify by Uman, 20101108, void*不能流化, 只流化值
			FvUInt32 v = (FvUInt32)va_arg( args, void* );
			os << v;
			break;
		}

		case 'c':
		{
			os << (char)va_arg( args, int );
			break;
		}

		case '*':
		{
			os << va_arg( args, int );
			break;
		}

		default:
			printf( "FvNetForwardingStringHandler::OnToken() Unknown type '%c'\n", fd.type_ );
			break;
		}
	}

	va_end( args );
}
