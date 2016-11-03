#include "FvNetInterfaceMinder.h"
#include "FvNetChannel.h"
#include "FvNetNub.h"

FV_DECLARE_DEBUG_COMPONENT2( "FvNetwork", 0 );

FvNetInterfaceElement & FvNetInterfaceMinder::Add( const char * name,
	FvInt8 lengthStyle, int lengthParam, FvNetInputMessageHandler * pHandler )
{
	FvNetInterfaceElement element( name, m_kElements.size(), lengthStyle, lengthParam,
		pHandler );

	m_kElements.push_back( element );
	return m_kElements.back();
}

void FvNetInterfaceMinder::RegisterWithNub( FvNetNub & nub )
{
	for (unsigned int i=0; i < m_kElements.size(); ++i)
	{
		FvNetInterfaceElement & element = m_kElements[i];
		nub.ServeInterfaceElement( element, i, element.GetHandler() );
	}
}

FvNetReason FvNetInterfaceMinder::RegisterWithMachined( FvNetNub & nub, int id ) const
{
	return nub.RegisterWithMachined( m_pcName, id );
}

bool FV_NETWORK_API WatcherStringToValue( const char * valueStr, FvNetAddress & value )
{
	int a1, a2, a3, a4, a5;

	if (sscanf( valueStr, "%d.%d.%d.%d:%d",
				&a1, &a2, &a3, &a4, &a5 ) != 5)
	{
		FV_WARNING_MSG( "WatcherStringToValue: "
				"Cannot convert '%s' to an Address.\n", valueStr );
		return false;
	}

	value.m_uiIP = (a1 << 24)|(a2 << 16)|(a3 << 8)|a4;

	value.m_uiPort = FvUInt16(a5);
	value.m_uiPort = ntohs( value.m_uiPort );
	value.m_uiIP = ntohl( value.m_uiIP );

	return true;
}

FvString FV_NETWORK_API WatcherValueToString( const FvNetAddress & value )
{
	return (char *)value;
}

#ifdef FV_CHECK_LOSS
void CheckLoss()
{
	FILE * f = fopen( "/proc/net/snmp", "r" );
	if (!f)
	{
		FV_ERROR_MSG( "CheckLoss(): Could not open /proc/net/snmp: %s\n",
			strerror( errno ) );
		return;
	}
	char	aline[256];

	int ipCount = 0;
	int udpCount = 0;

	FvInt32 inDiscards = 0;
	FvInt32 outDiscards = 0;
	FvInt32 udpErrors = 0;

	while (fgets( aline, 256, f ) != NULL)
	{
		if ((strncmp( aline, "Ip:", 3 ) == 0) && (++ipCount == 2))
		{
			int rv = sscanf( aline, "Ip:%*d%*d%*d%*d%*d"
					"%*d%*d%d%*d"
					"%*d%d%*d%*d%*d"
					"%*d%*d%*d%*d%*d", &inDiscards, &outDiscards );
			if (rv != 2)
			{
				FV_ERROR_MSG( "inDiscards = %d. outDiscards = %d. rv = %d\n",
						inDiscards, outDiscards, rv );
			}
		}

		if ((strncmp( aline, "Udp:", 4 ) == 0) && (++udpCount == 2))
		{
			int rv = sscanf( aline, "Udp:%*d%*d%d", &udpErrors );

			if (rv != 1)
			{
				FV_ERROR_MSG( "udpErrors = %d rv=%d\n", udpErrors, rv );
			}
		}
	}

	fclose(f);

	static FvInt32 old_inDiscards = 0;
	static FvInt32 old_outDiscards = 0;
	static FvInt32 old_udpErrors = 0;

	if (old_inDiscards != inDiscards)
	{
		FV_DEBUG_MSG( "inDiscards: %d = (%d - %d)\n",
				inDiscards - old_inDiscards,
				inDiscards, old_inDiscards );
		old_inDiscards = inDiscards;
	}

	if (old_outDiscards != outDiscards)
	{
		FV_DEBUG_MSG( "outDiscards: %d = (%d - %d)\n",
				outDiscards - old_outDiscards,
				outDiscards, old_outDiscards );
		old_outDiscards = outDiscards;
	}

	if (old_udpErrors != udpErrors)
	{
		FV_DEBUG_MSG( "udpErrors: %d = (%d - %d)\n",
				udpErrors - old_udpErrors,
				udpErrors, old_udpErrors );
		old_udpErrors = udpErrors;
	}
}
#endif // FV_CHECK_LOSS

