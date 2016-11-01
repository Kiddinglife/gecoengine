#include "FvLoginApp.h"

#include <sys/types.h>

#ifndef _WIN32
#include <sys/signal.h>
#include <pwd.h>
#else //ifndef _WIN32
#include <signal.h>
#endif // ndef _WIN32

#include "FvLoginIntInterface.h"
//#include "FvStatusCheckWatcher.h"

#include <../FvDBManager/FvDBInterface.h>

#include <FvServerResource.h>

#include <FvServerConfig.h>
#include <FvServerUtil.h>
#include <FvWriteDB.h>

#include <FvNetTypes.h>
#include <FvNetWatcherGlue.h>
#include <FvNetEncryptionFilter.h>

FV_DECLARE_DEBUG_COMPONENT( 0 )

const FvUInt32 FvLoginApp::UPDATE_STATS_PERIOD = 1000000;

static unsigned int gNumLogins = 0;
static unsigned int gNumLoginFailures = 0;
static unsigned int gNumLoginAttempts = 0;

static char g_buildID[81] = "Build ID";

FvUInt32 g_latestVersion = FvUInt32(-1);
FvUInt32 g_impendingVersion = FvUInt32(-1);

FV_SINGLETON_STORAGE( FvLoginApp )

extern "C" void interruptHandler( int )
{
	if (FvLoginApp::pInstance())
	{
		FvLoginApp::pInstance()->GetIntNub().BreakProcessing();
	}
}

extern "C" void controlledShutDownHandler( int )
{
	if (FvLoginApp::pInstance())
	{
		FvLoginApp::pInstance()->ControlledShutDown();
	}
}

bool commandStopServer( FvString & output, FvString & value )
{
	if (FvLoginApp::pInstance())
	{
		FvLoginApp::pInstance()->ControlledShutDown();
	}

	return true;
}


class DatabaseReplyHandler : public FvNetReplyMessageHandler
{
public:
	DatabaseReplyHandler(
		const FvNetAddress & clientAddr,
		const FvNetReplyID replyID,
		LogOnParamsPtr pParams );

	virtual ~DatabaseReplyHandler() {}

	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg );

	virtual void HandleException( const FvNetNubException & ne,
		void * arg );

private:
	FvNetAddress	clientAddr_;
	FvNetReplyID	replyID_;
	LogOnParamsPtr		pParams_;
};


FvLoginApp::FvLoginApp( FvNetNub & intNub, FvUInt16 loginPort ) :
#ifdef FV_USE_OPENSSL
	m_kPrivateKey( /* hasPrivate: */ true ),
#endif
	m_kIntNub( intNub ),
	m_kExtNub( intNub.GetMachinePort(),
				intNub.GetUserID(),
				htons( loginPort ),
				FvServerConfig::Get( "LoginApp/externalInterface",
					FvServerConfig::Get( "externalInterface" ) ).c_str() ),
	m_kNetMask(),
	m_uiExternalIP( 0 ),
	m_bIsControlledShutDown( false ),
	m_bIsProduction( FvServerConfig::Get( "production", false ) ),
	m_uiSystemOverloaded( 0 ),
	m_bAllowLogin( true ),
	m_bAllowProbe( true ),
	m_bLogProbes( false ),
	m_uiLastRateLimitCheckTime( 0 ),
	m_uiNumAllowedLoginsLeft( 0 ),
	m_iLoginRateLimit( 0 ),
	m_uiRateLimitDuration( 0 ),
	m_kStatsTimerID( 0 ),
	m_kLoginStats()
{
	m_kExtNub.IsVerbose( FvServerConfig::Get( "LoginApp/verboseExternalNub", false ) );

	double maxLoginDelay = FvServerConfig::Get( "LoginApp/maxLoginDelay", 10.f );
	m_uiMaxLoginDelay = FvUInt64( maxLoginDelay * ::StampsPerSecondD() );

	m_kExtNub.OnceOffResendPeriod( FV_CLIENT_ONCEOFF_RESEND_PERIOD );
	m_kExtNub.OnceOffMaxResends( FV_CLIENT_ONCEOFF_MAX_RESENDS );

	m_kExtNub.IsExternal( true );
}


bool FvLoginApp::Init( int argc, char * argv[], FvUInt16 loginPort )
{

	if (m_bIsProduction)
	{
		FV_INFO_MSG( "FvLoginApp::Init: Production mode enabled.\n" );
	}

	if ((m_kExtNub.Socket() == -1) &&
		!FvServerConfig::Get( "LoginApp/shouldShutDownIfPortUsed", false ) &&
		(loginPort != 0))
	{
		FV_INFO_MSG( "FvLoginApp::Init: "
					"Couldn't bind ext nub to %d, trying any available port\n",
				  loginPort );
		m_kExtNub.RecreateListeningSocket( 0,
			FvServerConfig::Get( "LoginApp/externalInterface", FvServerConfig::Get( "externalInterface" ) ).c_str() );
	}

	if (m_kExtNub.Socket() == -1)
	{
		FV_ERROR_MSG( "Loginapp::Init: "
			"Unable to bind to external interface on specified port %d.\n",
			loginPort );
		return false;
	}


#ifdef FV_USE_OPENSSL
	FvString privateKeyPath = FvServerConfig::Get( "LoginApp/privateKey",
		"loginapp.privkey" );

	if (!privateKeyPath.empty())
	{
		if (!m_kPrivateKey.SetKeyFromResource( privateKeyPath ))
		{
			return false;
		}
	}
	else
	{
		FV_ERROR_MSG( "FvLoginApp::Init: "
			"You must specify a private key to use with the "
			"<LoginApp/privateKey> option in FutureVision.xml\n" );

		return false;
	}
#endif

	if (m_kIntNub.Socket() == -1)
	{
		FV_ERROR_MSG( "Failed to create Nub on internal interface.\n" );
		return false;
	}

	if ((m_kExtNub.Address().m_uiIP == 0) ||
			(m_kIntNub.Address().m_uiIP == 0))
	{
		FV_ERROR_MSG( "FvLoginApp::Init: Failed to open UDP ports. "
				"Maybe another process already owns it\n" );
		return false;
	}

	FV_INIT_WATCHER_DOC( "LoginApp" );

	FvServerConfig::Update( "LoginApp/shutDownSystemOnExit", m_bIsControlledShutDown );
	FV_WATCH( "shutDownSystemOnExit", m_bIsControlledShutDown );

	FvString netMask = FvServerConfig::Get( "LoginApp/localNetMask" );
	m_kNetMask.Parse( netMask.c_str() );

	FvString extAddr = FvServerConfig::Get( "LoginApp/externalAddress" );
	m_uiExternalIP = inet_addr( extAddr.c_str() );

	if (m_kNetMask.ContainsAddress( m_kIntNub.Address().m_uiIP ))
	{
		FV_INFO_MSG( "Local subnet: %s\n", netMask.c_str() );
		FV_INFO_MSG( "External addr: %s\n", extAddr.c_str() );
	}
	else
	{
		FV_WARNING_MSG( "FvLoginApp::FvLoginApp: "
					"localNetMask %s does not match local ip %s\n",
				netMask.c_str(), m_kIntNub.Address().c_str() );
		FV_INFO_MSG( "Not using localNetMask\n" );

		m_kNetMask.Clear();
	}

	FV_WATCH( "numLogins", gNumLogins );
	FV_WATCH( "numLoginFailures", gNumLoginFailures );
	FV_WATCH( "numLoginAttempts", gNumLoginAttempts );

	FvReviverSubject::Instance().Init( &m_kIntNub, "LoginApp" );

	if (m_kExtNub.Address().m_uiIP == 0)
	{
		FV_CRITICAL_MSG( "Login::Init: Failed to start Nub.\n"
			"\tLog in port is probably in use.\n"
			"\tIs there another login server running on this machine?\n" );
		return false;
	}

	FV_INFO_MSG( "External address = %s\n", m_kExtNub.Address().c_str() );
	FV_INFO_MSG( "Internal address = %s\n", m_kIntNub.Address().c_str() );

	if (FV_INIT_ANONYMOUS_CHANNEL_CLIENT( m_kDBManager, m_kIntNub,
			LoginIntInterface, DBInterface, 0 ))
	{
		FV_INFO_MSG( "FvLoginApp::Init: DB addr: %s\n",
			this->GetDBManager().Channel().c_str() );
	}
	else
	{
		FV_INFO_MSG( "FvLoginApp::Init: Database not ready yet.\n" );
	}

	LoginInterface::RegisterWithNub( m_kExtNub );
	LoginIntInterface::RegisterWithNub( m_kIntNub );

	m_bAllowLogin = FvServerConfig::Get( "LoginApp/allowLogin", m_bAllowLogin );
	m_bAllowProbe = FvServerConfig::Get( "LoginApp/allowProbe", m_bAllowProbe );
	m_bLogProbes = FvServerConfig::Get( "LoginApp/logProbes", m_bLogProbes );
	FV_WATCH( "allowLogin", m_bAllowLogin );
	FV_WATCH( "allowProbe", m_bAllowProbe );
	FV_WATCH( "logProbes", m_bLogProbes );
	FV_WATCH( "systemOverloaded", m_uiSystemOverloaded );

	if ( (m_bAllowProbe) && (m_bIsProduction) )
	{
		FV_ERROR_MSG( "Production Mode: FutureVision.xml/LoginApp/allowProbe is enabled. "
			"This is a development-time feature only and should be disabled "
			"during production.\n" );
	}

	m_kExtNub.SetLatency( FvServerConfig::Get( "LoginApp/externalLatencyMin",
								FvServerConfig::Get( "externalLatencyMin", 0.f ) ),
						FvServerConfig::Get( "LoginApp/externalLatencyMax",
								FvServerConfig::Get( "externalLatencyMax", 0.f ) ) );
	m_kExtNub.SetLossRatio( FvServerConfig::Get( "LoginApp/externalLossRatio",
								FvServerConfig::Get( "externalLossRatio", 0.f ) ) );
	if (m_kExtNub.HasArtificialLossOrLatency())
	{
		FV_WARNING_MSG( "FvLoginApp::Init: External Nub loss/latency enabled\n" );
	}

	FvUInt32 rateLimitSeconds = FvServerConfig::Get( "LoginApp/rateLimitDuration",
		FvUInt32( 0 ) );
	m_uiRateLimitDuration = rateLimitSeconds * StampsPerSecond();
	FvServerConfig::Update( "LoginApp/loginRateLimit", m_iLoginRateLimit );

	if (rateLimitSeconds)
	{
		FV_INFO_MSG( "FvLoginApp::Init: "
				"Login rate limiting enabled: period = %u, limit = %d\n",
			rateLimitSeconds, m_iLoginRateLimit );
	}

	m_uiNumAllowedLoginsLeft = m_iLoginRateLimit;
	m_uiLastRateLimitCheckTime = Timestamp();
	FV_WATCH( "rateLimit/duration", *this,
			&FvLoginApp::RateLimitSeconds, &FvLoginApp::RateLimitSeconds );
	FV_WATCH( "rateLimit/loginLimit", m_iLoginRateLimit );

	FvNetReason reason =
		LoginIntInterface::RegisterWithMachined( m_kIntNub, 0 );

	if (reason != FV_NET_REASON_SUCCESS)
	{
		FV_ERROR_MSG( "FvLoginApp::Init: Unable to register with nub. "
			"Is machined running?\n");
		return false;
	}

	if (FvServerConfig::Get( "LoginApp/registerExternalInterface", true ))
	{
		LoginInterface::RegisterWithMachined( m_kExtNub, 0 );
	}

	m_bAllowUnencryptedLogins =
		FvServerConfig::Get( "LoginApp/allowUnencryptedLogins", false );

	signal( SIGINT, interruptHandler );
#ifndef _WIN32
	signal( SIGUSR1, controlledShutDownHandler );
#endif

	m_kIntNub.RegisterChildNub( &m_kExtNub );

	//FV_REGISTER_WATCHER( 0, "LoginApp", "LoginApp", "LoginApp", m_kIntNub );

	//FvWatcher & root = FvWatcher::RootWatcher();
	//root.AddChild( "nub", FvNetNub::GetWatcher(), &m_kIntNub );
	//root.AddChild( "nubExternal", FvNetNub::GetWatcher(), &m_kExtNub );

	//root.AddChild( "command/statusCheck", new FvStatusCheckWatcher() );
	//root.AddChild( "command/shutDownServer",
	//		new FvNoArgCallableWatcher( commandStopServer,
	//			FvCallableWatcher::LOCAL_ONLY,
	//			"Shuts down the entire server" ) );

	//FvWatcherPtr pStatsWatcher = new FvDirectoryWatcher();
	//pStatsWatcher->AddChild( "rateLimited",
	//		MakeWatcher( m_kLoginStats, &LoginStats::RateLimited ) );
	//pStatsWatcher->AddChild( "repeatedForAlreadyPending",
	//		MakeWatcher( m_kLoginStats, &LoginStats::Pending ) );
	//pStatsWatcher->AddChild( "failures",
	//		MakeWatcher( m_kLoginStats, &LoginStats::Fails ) );
	//pStatsWatcher->AddChild( "successes",
	//		MakeWatcher( m_kLoginStats, &LoginStats::Successes ) );
	//pStatsWatcher->AddChild( "all",
	//		MakeWatcher( m_kLoginStats, &LoginStats::All ) );

	//{
	//	static FvUInt32 s_updateStatsPeriod = FvLoginApp::UPDATE_STATS_PERIOD;
	//	pStatsWatcher->AddChild( "updatePeriod",
	//			MakeWatcher( s_updateStatsPeriod ) );
	//}

	//root.AddChild( "averages", pStatsWatcher );

	m_kIntNub.RegisterTimer( UPDATE_STATS_PERIOD, &m_kLoginStats, NULL );
	return true;
}


void FvLoginApp::Run()
{
	m_kIntNub.ProcessUntilBreak();

	FV_INFO_MSG( "FvLoginApp::Run: Terminating normally.\n" );

	if (this->IsDBReady() && m_bIsControlledShutDown)
	{
		FvNetBundle	& dbBundle = this->GetDBManager().Bundle();
		DBInterface::ControlledShutDownArgs args;
		args.stage = SHUTDOWN_REQUEST;
		dbBundle << args;
		this->GetDBManager().Send();

		m_kIntNub.ProcessUntilChannelsEmpty();
	}
}


void FvLoginApp::SendFailure( const FvNetAddress & addr,
	FvNetReplyID replyID, int status, const char * pDescription,
	LogOnParamsPtr pParams )
{
	if (status == FvLogOnStatus::LOGIN_REJECTED_RATE_LIMITED)
	{
		m_kLoginStats.IncRateLimited();
	}
	else
	{
		m_kLoginStats.IncFails();
	}

	if (pDescription == NULL)
		pDescription = "";

	FV_INFO_MSG( "FvLoginApp::SendFailure: "
		"LogOn for %s failed, FvLogOnStatus %d, description '%s'.\n",
			addr.c_str(), status, pDescription );

	++gNumLoginFailures;

	FvNetBundle bundle;

	bundle.StartReply( replyID, FV_NET_RELIABLE_NO );
	bundle << (FvInt8)status;
	bundle << pDescription;

	FvLoginApp & app = FvLoginApp::Instance();
	app.m_kExtNub.Send( addr, bundle );

	if (*pDescription == 0)
	{
		FV_WARNING_MSG( "FvLoginApp::SendFailure: "
			"Sent FvLogOnStatus %d without a description (bad form)", status );
	}

	if (pParams)
	{
		app.m_kCachedLoginMap.erase( addr );
	}
}


void FvLoginApp::Login( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
{

	if (m_uiRateLimitDuration &&
			(Timestamp() > m_uiLastRateLimitCheckTime + m_uiRateLimitDuration) )
	{
		m_uiNumAllowedLoginsLeft = m_iLoginRateLimit;
		m_uiLastRateLimitCheckTime = Timestamp();
	}

	if (!m_bAllowLogin)
	{
		FV_WARNING_MSG( "FvLoginApp::Login: "
			"Dropping login attempt from %s as logins aren't allowed yet\n",
			source.c_str() );

		this->SendFailure( source, header.m_iReplyID,
			FvLogOnStatus::LOGIN_REJECTED_LOGINS_NOT_ALLOWED,
			"Logins currently not permitted" );
		data.Finish();
		return;
	}
	if (!source.m_uiIP)
	{
		FV_ERROR_MSG( "FvLoginApp::Login: Spoofed empty address\n" );
		data.Retrieve( data.RemainingLength() );
		m_kLoginStats.IncFails();
		return;
	}

	bool isReattempt = (m_kCachedLoginMap.count( source ) != 0);
 	FV_INFO_MSG( "FvLoginApp::Login: %s from %s\n",
		isReattempt ? "Re-attempt" : "Attempt", source.c_str() );

	++gNumLoginAttempts;

	FvUInt32 version = 0;
	data >> version;

	if (data.Error())
	{
		FV_ERROR_MSG( "FvLoginApp::Login: "
			"Not enough data on stream (%d bytes total)\n",
			header.m_iLength );

		this->SendFailure( source, header.m_iReplyID,
			FvLogOnStatus::LOGIN_MALFORMED_REQUEST,
			"Undersized login message");

		return;
	}

	if (version != FV_LOGIN_VERSION)
	{
		FV_ERROR_MSG( "FvLoginApp::Login: "
			"User at %s tried to log on with version %u. Expected %u\n",
			source.c_str(), version, FV_LOGIN_VERSION );

		char msg[BUFSIZ];

		FvSNPrintf( msg, sizeof(msg), "Incorrect protocol version. "
				"Client version is %u, server version is %u. "
				"Your %s is out of date.", version, FV_LOGIN_VERSION,
			  	version < FV_LOGIN_VERSION ? "client" : "server" );

		this->SendFailure( source, header.m_iReplyID,
			FvLogOnStatus::LOGIN_BAD_PROTOCOL_VERSION, msg );

		data.Finish();

		return;
	}

	bool isRateLimited = (m_uiRateLimitDuration && m_uiNumAllowedLoginsLeft == 0);
	if (isRateLimited)
	{
		FV_NOTICE_MSG( "FvLoginApp::Login: "
				"Login from %s not allowed due to rate limiting\n",
				source.c_str() );

		this->SendFailure( source, header.m_iReplyID,
				FvLogOnStatus::LOGIN_REJECTED_RATE_LIMITED,
				"Logins temporarily disallowed due to rate limiting" );
		data.Finish();
		return;
	}

	if (!this->IsDBReady())
	{
		FV_INFO_MSG( "FvLoginApp::Login: "
			"Attempted login when database not yet ready.\n" );

		this->SendFailure( source, header.m_iReplyID,
			FvLogOnStatus::LOGIN_REJECTED_DB_NOT_READY, "DB not ready" );

		return;
	}

	if (m_uiSystemOverloaded != 0)
	{
		if (m_uiSystemOverloadedTime + StampsPerSecond() < Timestamp())
		{
			m_uiSystemOverloaded = 0;
		}
		else
		{
			FV_INFO_MSG( "FvLoginApp::Login: "
				"Attempted login when system overloaded or not yet ready.\n" );
			this->SendFailure( source, header.m_iReplyID,
				m_uiSystemOverloaded, "System overloaded wait state." );
			return;
		}
	}

	LogOnParamsPtr pParams = new FvLogOnParams();

	int dataLength = data.RemainingLength();
	const void * pDataData = data.Retrieve( dataLength );


	if (this->HandleResentPendingAttempt( source, header.m_iReplyID ))
	{
		m_kLoginStats.IncPending();
		return;
	}


#ifdef FV_USE_OPENSSL
	FvNetPublicKeyCipher * pPrivateKey = &m_kPrivateKey;
#else
	FvNetPublicKeyCipher * pPrivateKey = NULL;
#endif

	do
	{
		FvMemoryIStream attempt = FvMemoryIStream( pDataData, dataLength );

		if (pParams->ReadFromStream( attempt, pPrivateKey ))
		{
			break;
		}

		if (pPrivateKey && m_bAllowUnencryptedLogins)
		{
			pPrivateKey = NULL;
			continue;
		}

		this->SendFailure( source, header.m_iReplyID,
			FvLogOnStatus::LOGIN_MALFORMED_REQUEST,
			"Could not destream login parameters. Possibly caused by "
			"mis-matching FvLoginApp keypair." );
		return;
	}
	while (false);

	if (this->HandleResentCachedAttempt( source, pParams, header.m_iReplyID ))
	{
		return;
	}

	if (m_uiRateLimitDuration)
	{
		--m_uiNumAllowedLoginsLeft;
	}

	if (pParams->EncryptionKey().empty() && !m_bAllowUnencryptedLogins)
	{
		this->SendFailure( source, header.m_iReplyID,
			FvLogOnStatus::LOGIN_MALFORMED_REQUEST,
			"No encryption key supplied, and server is not allowing "
				"unencrypted logins." );
		return;
	}


	FV_INFO_MSG( "Logging in %s{%s} (%s)\n",
		pParams->Username().c_str(),
		pParams->Password().c_str(),
		source.c_str() );

	m_kCachedLoginMap[ source ].Reset();
	m_kCachedLoginMap[ source ].pParams( pParams );

	DatabaseReplyHandler * pDBHandler =
		new DatabaseReplyHandler( source, header.m_iReplyID, pParams );

	FvNetBundle	& dbBundle = this->GetDBManager().Bundle();
	dbBundle.StartRequest( DBInterface::LogOn, pDBHandler );

	dbBundle << source << false /*off channel*/ << *pParams;

	this->GetDBManager().Send();
}


DatabaseReplyHandler::DatabaseReplyHandler(
		const FvNetAddress & clientAddr,
		FvNetReplyID replyID,
		LogOnParamsPtr pParams ) :
	clientAddr_( clientAddr ),
	replyID_( replyID ),
	pParams_( pParams )
{
	FV_DEBUG_MSG( "DBReplyHandler created at %f\n",
		float(double(Timestamp())/StampsPerSecondD()) );
}


void DatabaseReplyHandler::HandleMessage(
	const FvNetAddress & /*source*/,
	FvNetUnpackedMessageHeader & header,
	FvBinaryIStream & data,
	void * /*arg*/ )
{
	FvUInt8 status;
	data >> status;

	if (status != FvLogOnStatus::LOGGED_ON)
	{
		if (data.RemainingLength() > 0)
		{
			FvString msg;
			data >> msg;
			FvLoginApp::Instance().SendFailure( clientAddr_, replyID_, status,
				msg.c_str(), pParams_ );
		}
		else
		{
			FvLoginApp::Instance().SendFailure( clientAddr_, replyID_, status,
				"Database returned an unelaborated error. Check DBManager log.",
				pParams_ );
		}

		FvLoginApp & app = FvLoginApp::Instance();
		if ((app.SystemOverloaded() == 0 &&
			 status == FvLogOnStatus::LOGIN_REJECTED_BASEAPP_OVERLOAD) ||
				status == FvLogOnStatus::LOGIN_REJECTED_CELLAPP_OVERLOAD ||
				status == FvLogOnStatus::LOGIN_REJECTED_DBMGR_OVERLOAD)
		{
			FV_DEBUG_MSG( "DatabaseReplyHandler::HandleMessage(%s): "
					"failure due to overload (status=%x)\n",
				clientAddr_.c_str(), status );
			app.SystemOverloaded( status );
		}
		delete this;
		return;
	}

	if (data.RemainingLength() < int(sizeof( FvLoginReplyRecord )))
	{
		FV_ERROR_MSG( "DatabaseReplyHandler::HandleMessage: "
						"Login failed. Expected %lu bytes got %d\n",
				sizeof( FvLoginReplyRecord ), data.RemainingLength() );

		if (data.RemainingLength() == sizeof(FvLoginReplyRecord) - sizeof(int))
		{
			FV_ERROR_MSG( "DatabaseReplyHandler::HandleMessage: "
					"This can occur if a login is attempted to an entity type "
					"that is not a Proxy.\n" );

			FvLoginApp::Instance().SendFailure( clientAddr_, replyID_,
				FvLogOnStatus::LOGIN_CUSTOM_DEFINED_ERROR,
				"Database returned a non-proxy entity type.",
				pParams_ );
		}
		else
		{
			FvLoginApp::Instance().SendFailure( clientAddr_, replyID_,
				FvLogOnStatus::LOGIN_REJECTED_DB_GENERAL_FAILURE,
				"Database returned an unknown error.",
				pParams_ );
		}

		delete this;
		return;
	}

	FvLoginReplyRecord lrr;
	data >> lrr;

	if (!FvLoginApp::Instance().NetMask().ContainsAddress( clientAddr_.m_uiIP ))
	{
		FV_INFO_MSG( "DatabaseReplyHandler::HandleMessage: "
				"Redirecting external client %s to firewall.\n",
			clientAddr_.c_str() );
		lrr.m_kServerAddr.m_uiIP = FvLoginApp::Instance().ExternalIP();
	}

	FvLoginApp::Instance().SendAndCacheSuccess( clientAddr_,
			replyID_, lrr, pParams_ );

	gNumLogins++;

	delete this;
}


void DatabaseReplyHandler::HandleException(
	const FvNetNubException & ne,
	void * /*arg*/ )
{
	FvLoginApp::Instance().SendFailure( clientAddr_, replyID_,
		FvLogOnStatus::LOGIN_REJECTED_DBMGR_OVERLOAD, "No reply from DBManager.",
		pParams_ );

	FV_WARNING_MSG( "DatabaseReplyHandler: got an exception (%s)\n",
			NetReasonToString( ne.Reason() ) );
	FV_DEBUG_MSG( "DBReplyHandler timed out at %f\n",
			float(double(Timestamp())/StampsPerSecondD()) );

	delete this;
}


void FvLoginApp::Probe( const FvNetAddress & source,
	FvNetUnpackedMessageHeader & header,
	FvBinaryIStream & /*data*/ )
{
	if (m_bLogProbes)
	{
		FV_INFO_MSG( "FvLoginApp::Probe: Got Probe from %s\n", source.c_str() );
	}

	if (!m_bAllowProbe || header.m_iLength != 0) return;

	FvNetBundle bundle;
	bundle.StartReply( header.m_iReplyID );

	char buf[256];
	gethostname( buf, sizeof(buf) ); buf[sizeof(buf)-1]=0;
	bundle << PROBE_KEY_HOST_NAME << buf;

#ifndef _WIN32
	struct passwd *pwent = getpwuid( getUserId() );
	const char * username = pwent ? (pwent->pw_name ? pwent->pw_name : "") : "";
	bundle << PROBE_KEY_OWNER_NAME << username;

	if (!pwent)
	{
		FV_ERROR_MSG( "FvLoginApp::Probe: "
			"Process uid %d doesn't exist on this system!\n", getUserId() );
	}

#else
	DWORD bsz = sizeof(buf);
	if (!GetUserName( buf, &bsz )) buf[0]=0;
	bundle << PROBE_KEY_OWNER_NAME << buf;
#endif

	FvSNPrintf( buf, sizeof(buf), buf, "%d", gNumLogins );
	bundle << PROBE_KEY_USERS_COUNT << buf;

	bundle << PROBE_KEY_UNIVERSE_NAME << FvServerConfig::Get( "universe" );
	bundle << PROBE_KEY_SPACE_NAME << FvServerConfig::Get( "space" );

	bundle << PROBE_KEY_BINARY_ID << g_buildID;

	m_kExtNub.Send( source, bundle );
}


class LoginAppRawMessageHandler : public FvNetInputMessageHandler
{
	public:
		typedef void (FvLoginApp::*Handler)(
			const FvNetAddress & srcAddr,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & stream );

		LoginAppRawMessageHandler( Handler handler ) :
			handler_( handler )
		{}

	private:
		virtual void HandleMessage( const FvNetAddress & srcAddr,
				FvNetUnpackedMessageHeader & header,
				FvBinaryIStream & data )
		{
			(FvLoginApp::Instance().*handler_)( srcAddr, header, data );
		}

		Handler handler_;
};

LoginAppRawMessageHandler g_kLoginHandler( &FvLoginApp::Login );
LoginAppRawMessageHandler g_kProbeHandler( &FvLoginApp::Probe );
LoginAppRawMessageHandler g_kShutDownHandler( &FvLoginApp::ControlledShutDown );


void FvLoginApp::SendAndCacheSuccess( const FvNetAddress & addr,
		FvNetReplyID replyID, const FvLoginReplyRecord & replyRecord,
		LogOnParamsPtr pParams )
{
	this->SendSuccess( addr, replyID, replyRecord,
		pParams->EncryptionKey() );

	m_kCachedLoginMap[ addr ].ReplyRecord( replyRecord );

	if (m_kCachedLoginMap.size() > 100)
	{
		CachedLoginMap::iterator iter = m_kCachedLoginMap.begin();

		while (iter != m_kCachedLoginMap.end())
		{
			CachedLoginMap::iterator prevIter = iter;
			++iter;

			if (prevIter->second.IsTooOld())
			{
				m_kCachedLoginMap.erase( prevIter );
			}
		}
	}
}


void FvLoginApp::SendSuccess( const FvNetAddress & addr,
	FvNetReplyID replyID, const FvLoginReplyRecord & replyRecord,
	const FvString & encryptionKey )
{
	FvNetBundle b;
	b.StartReply( replyID );
	b << (FvInt8)FvLogOnStatus::LOGGED_ON;

#ifdef FV_USE_OPENSSL
	if (!encryptionKey.empty())
	{
		FvNetEncryptionFilter filter( encryptionKey );
		FvMemoryOStream clearText;
		clearText << replyRecord;
		filter.EncryptStream( clearText, b );
	}
	else
#endif
	{
		b << replyRecord;
	}

	m_kLoginStats.IncSuccesses();

	this->GetExtNub().Send( addr, b );
}

bool FvLoginApp::HandleResentPendingAttempt( const FvNetAddress & addr,
		FvNetReplyID replyID )
{
	CachedLoginMap::iterator iter = m_kCachedLoginMap.find( addr );

	if (iter != m_kCachedLoginMap.end())
	{
		CachedLogin & cache = iter->second;

		if (cache.IsPending())
		{
			FV_DEBUG_MSG( "Ignoring repeat attempt from %s "
					"while another attempt is in progress (for %s)\n",
				addr.c_str(),
				cache.pParams()->Username().c_str() );

			return true;
		}
	}
	return false;
}

bool FvLoginApp::HandleResentCachedAttempt( const FvNetAddress & addr,
		LogOnParamsPtr pParams, FvNetReplyID replyID )
{
	CachedLoginMap::iterator iter = m_kCachedLoginMap.find( addr );

	if (iter != m_kCachedLoginMap.end())
	{
		CachedLogin & cache = iter->second;
		if (!cache.IsTooOld() && *cache.pParams() == *pParams)
		{
			FV_DEBUG_MSG( "%s retransmitting successful login to %s\n",
					   addr.c_str(),
					   cache.pParams()->Username().c_str() );
			this->SendSuccess( addr, replyID, cache.ReplyRecord(),
				cache.pParams()->EncryptionKey() );

			return true;
		}
	}

	return false;
}

void FvLoginApp::ControlledShutDown( const FvNetAddress &source,
	FvNetUnpackedMessageHeader &header,
	FvBinaryIStream &data )
{
	FV_INFO_MSG( "FvLoginApp::ControlledShutDown: "
		"Got shutdown command from %s\n", source.c_str() );

	this->ControlledShutDown();
}

bool FvLoginApp::CachedLogin::IsPending() const
{
	return m_uiCreationTime == 0;
}

bool FvLoginApp::CachedLogin::IsTooOld() const
{
	const FvUInt64 MAX_LOGIN_DELAY = FvLoginApp::Instance().MaxLoginDelay();

	return !this->IsPending() &&
		(::Timestamp() - m_uiCreationTime > MAX_LOGIN_DELAY);
}

void FvLoginApp::CachedLogin::ReplyRecord( const FvLoginReplyRecord & record )
{
	m_kReplyRecord = record;
	m_uiCreationTime = ::Timestamp();
}


template <class Args> class LoginAppStructMessageHandler :
	public FvNetInputMessageHandler
{
public:
	typedef void (FvLoginApp::*Handler)( const Args & args );

	LoginAppStructMessageHandler( Handler handler ) :
		handler_( handler )
	{}

private:
	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
	{
		Args * pArgs = (Args*)data.Retrieve( sizeof(Args) );
		(FvLoginApp::Instance().*handler_)( *pArgs );
	}

	Handler handler_;
};

static const unsigned int WEIGHTING_NUM_SAMPLES = 5;

const float FvLoginApp::LoginStats::BIAS = 2.f / (WEIGHTING_NUM_SAMPLES + 1);

FvLoginApp::LoginStats::LoginStats():
			m_kFails( BIAS ),
			m_kRateLimited( BIAS ),
			m_kPending( BIAS ),
			m_kSuccesses( BIAS ),
			m_kAll( BIAS )
{}

#define DEFINE_INTERFACE_HERE
#include <../FvDBManager/FvDBInterface.h>

#define DEFINE_SERVER_HERE
#include "FvLoginIntInterface.h"
