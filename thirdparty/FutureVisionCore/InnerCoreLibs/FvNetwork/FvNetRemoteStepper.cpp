#include "FvNetRemoteStepper.h"
#include "FvNetEndpoint.h"

#include <FvPrintf.h>

#ifdef _WIN32
#ifndef FV_SERVER
extern const char * g_pcCompileTimeString  = __TIMESTAMP__;
#else // FV_SERVER
const char * g_pcCompileTimeString = __TIMESTAMP__;
#endif // FV_SERVER
#endif

FV_DECLARE_DEBUG_COMPONENT2( "FvNetwork", -1 )

FvNetRemoteStepper::FvNetRemoteStepper() :
	m_pkLEP( NULL ),
	m_pkCEP( NULL )
{
	m_pkLEP = new FvNetEndpoint();
	m_pkLEP->Socket( SOCK_STREAM );
	m_pkLEP->Bind( htons( 11111 ) );
	m_pkLEP->Listen();
	m_pkLEP->SetNonblocking( true );

	u_int32_t	localip = 0;
	m_pkLEP->GetInterfaceAddress( "eth0", localip );
	FvDPrintfNormal( "Waiting for remote stepper connection on %d.%d.%d.%d:11111\n",
		((localip)&255), ((localip>>8)&255), ((localip>>16)&255), ((localip>>24)&255) );

#if 0
	// wait up to .1s for a connection here
	int nloops = 0;
	while (!this->tryaccept())
	{
		Sleep( 50 );

		if (++nloops > 100/50)
		{
			dprintf( "Given up waiting for a connection.\n" );
			break;
		}
	}
#endif
}

FvNetRemoteStepper::~FvNetRemoteStepper()
{
	if (m_pkCEP != NULL)
	{
		m_pkCEP->Close();
		delete m_pkCEP;
	}

	m_pkLEP->Close();
	delete m_pkLEP;
}


bool FvNetRemoteStepper::Tryaccept()
{
	m_pkCEP = m_pkLEP->Accept();
	if (m_pkCEP == NULL) return false;

	m_pkCEP->SetNonblocking( false );

	FvDPrintfNormal( "Got remote stepper connection\n" );

	FvString sendout;
#ifdef _WIN32
	sendout = "Welcome to the FutureVision Client.\r\nCompile time was";
	sendout += g_pcCompileTimeString;
#endif // _WIN32
	sendout += "\r\nStarting game...\r\n";
	m_pkCEP->Send( sendout.c_str(), sendout.length() );
	return true;
}


enum TelnetEscape
{
	TELNET_WILL			= 251,
	TELNET_DO			= 252,
	TELNET_WONT			= 253,
	TELNET_DONT			= 254,
	TELNET_IAC			= 255
};



void FvNetRemoteStepper::StepInt( const FvString & desc, bool wait )
{
	FV_TRACE_MSG( "Remote stepper at step %s\n", desc.c_str() );

	if (m_pkCEP == NULL)
	{
		if (!Tryaccept()) return;
	}

	FvString sendout = "Now at step: " + desc + "\r\n";
	m_pkCEP->Send( sendout.c_str(), sendout.length() );

	if (!wait) return;

	sendout = "Press enter to continue\r\n";
	m_pkCEP->Send( sendout.c_str(), sendout.length() );

	char	recvBuf[128];
	int		recvAmt;
	bool	gotEnter = false;
	while (!gotEnter &&
		(recvAmt = m_pkCEP->Recv( recvBuf, sizeof(recvBuf) )) > 0)
	{
		for (int i = 0; i < recvAmt; i++)
		{
			if (recvBuf[i] == '\n')
			{
				gotEnter = true;
				sendout = "Continuing...\r\n";
				m_pkCEP->Send( sendout.c_str(), sendout.length() );
//				Sleep( 50 );
			}
		}
	}
	if (recvAmt < 0)
	{
		FvDPrintfNormal( "Remote stepper got error on connection.\n" );
		m_pkCEP->Close();
		delete m_pkCEP;
		m_pkCEP = NULL;
	}
}


static FvNetRemoteStepper * pUfa = NULL;

void FvNetRemoteStepper::Start()
{
	if (pUfa == NULL) pUfa = new FvNetRemoteStepper();
}

void FvNetRemoteStepper::Step( const FvString & desc, bool wait )
{
	if (pUfa != NULL) pUfa->StepInt( desc, wait );
}
