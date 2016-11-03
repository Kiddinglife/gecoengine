#include <FvDebug.h>
#include <FvConcurrency.h>

FV_DECLARE_DEBUG_COMPONENT2( "Connect", 0 )

#include "FvServerConnection.h"

#ifndef FV_ROBOT_EXPORT
#include "SaintsApplication.h"
#endif

#include <FvVector3.h>
#include <FvNetPortMap.h>
#include <FvNetEncryptionFilter.h>
#include <FvNetPublicKeyCipher.h>
#include <FvNetMsgTypes.h>

#include <FvLoginInterface.h>
#include <FvBaseAppExtInterface.h>
#include <FvClientInterface.h>
#include "FvMemoryStream.h"


namespace
{
const int LOGIN_TIMEOUT = 8000000;

const float DEFAULT_INACTIVITY_TIMEOUT = 60.f;

const int MAX_LOGINAPP_LOGIN_ATTEMPTS = 10;

const int LOGINAPP_LOGIN_ATTEMPT_PERIOD = 1000000;

const int MAX_BASEAPP_LOGIN_ATTEMPTS = 10;

const int BASEAPP_LOGIN_ATTEMPT_PERIOD = 1000000;

}

#ifdef WIN32
#pragma warning (disable:4355)
#endif

FvRetryingRequest::FvRetryingRequest( FvLoginHandler & parent,
		const FvNetAddress & addr,
		const FvNetInterfaceElement & ie,
		int retryPeriod,
		int timeoutPeriod,
		int maxAttempts,
		bool useParentNub ) :
	m_spParent( &parent ),
	m_pkNub( NULL ),
	m_kAddr( addr ),
	m_kInterfaceElement( ie ),
	m_kTimerID( FV_NET_TIMER_ID_NONE ),
	m_bDone( false ),
	m_iRetryPeriod( retryPeriod ),
	m_iTimeoutPeriod( timeoutPeriod ),
	m_iNumAttempts( 0 ),
	m_iNumOutstandingAttempts( 0 ),
	m_iMaxAttempts( maxAttempts )
{
	if (useParentNub)
	{
		this->SetNub( &parent.ServConn().GetNub() );
	}

	parent.AddChildRequest( this );
}

FvRetryingRequest::~FvRetryingRequest()
{
	FV_ASSERT_DEV( m_kTimerID == FV_NET_TIMER_ID_NONE );
}

void FvRetryingRequest::HandleMessage( const FvNetAddress & srcAddr,
	FvNetUnpackedMessageHeader & header,
	FvBinaryIStream & data,
	void * arg )
{
	if (!m_bDone)
	{
		this->OnSuccess( data );
		this->Cancel();
	}

	--m_iNumOutstandingAttempts;
	this->DecRef();
}

void FvRetryingRequest::HandleException( const FvNetNubException & exc,
	void * arg )
{
	if (!m_bDone && (exc.Reason() != FV_NET_REASON_TIMER_EXPIRED))
	{
		FV_ERROR_MSG( "FvRetryingRequest::HandleException( %s ): "
			"Request to %s failed (%s)\n",
			m_kInterfaceElement.GetName(), m_kAddr.c_str(), NetReasonToString( exc.Reason() ) );

		this->OnFailure( exc.Reason() );
		this->Cancel();
	}

	--m_iNumOutstandingAttempts;

	if (!m_bDone && m_iNumOutstandingAttempts == 0)
	{
		if (m_iMaxAttempts > 1)
		{
			FV_ERROR_MSG( "FvRetryingRequest::HandleException( %s ): "
				"Final attempt of %d has failed (%s), aborting\n",
				m_kInterfaceElement.GetName(), m_iMaxAttempts,
				NetReasonToString( exc.Reason() ) );
		}

		this->OnFailure( exc.Reason() );
		this->Cancel();
	}

	this->DecRef();
}

int FvRetryingRequest::HandleTimeout( FvNetTimerID id, void * arg )
{
	this->Send();
	return 0;
}

void FvRetryingRequest::SetNub( FvNetNub * pNub )
{
	FV_ASSERT_DEV( m_pkNub == NULL );
	m_pkNub = pNub;
	m_kTimerID = m_pkNub->RegisterTimer( m_iRetryPeriod, this );
}

void FvRetryingRequest::Send()
{
	if (m_bDone) return;

	if (m_iNumAttempts < m_iMaxAttempts)
	{
		++m_iNumAttempts;

		FvNetBundle bundle;
		bundle.StartRequest( m_kInterfaceElement, this, NULL, m_iTimeoutPeriod,
			FV_NET_RELIABLE_NO );

		this->AddRequestArgs( bundle );

		FvRetryingRequestPtr pThis = this;

		this->IncRef();
		++m_iNumOutstandingAttempts;

		m_pkNub->Send( m_kAddr, bundle );
	}
}

void FvRetryingRequest::Cancel()
{
	if (m_kTimerID != FV_NET_TIMER_ID_NONE)
	{
		m_pkNub->CancelTimer( m_kTimerID );
		m_kTimerID = FV_NET_TIMER_ID_NONE;
	}
	m_spParent->RemoveChildRequest( this );
	m_bDone = true;
}

FvLoginRequest::FvLoginRequest( FvLoginHandler & parent ) :
	FvRetryingRequest( parent, parent.LoginAddr(), LoginInterface::Login )
{
	this->Send();
}

void FvLoginRequest::AddRequestArgs( FvNetBundle & bundle )
{
	LogOnParamsPtr pParams = m_spParent->pParams();

	bundle << FV_LOGIN_VERSION;

//#ifdef FV_USE_OPENSSL
//	if (!pParams->AddToStream( bundle, FvLogOnParams::HAS_ALL,
//			&m_spParent->ServConn().PublicKey() ))
//#else
	if (!pParams->AddToStream( bundle, FvLogOnParams::HAS_ALL,
			NULL))
//#endif
	{
		FV_ERROR_MSG( "FvLoginRequest::AddRequestArgs: "
			"Failed to assemble login bundle\n" );

		m_spParent->OnFailure( FV_NET_REASON_CORRUPTED_PACKET );
	}
}


void FvLoginRequest::OnSuccess( FvBinaryIStream & data )
{
	m_spParent->OnLoginReply( data );
}


void FvLoginRequest::OnFailure( FvNetReason reason )
{
	m_spParent->OnFailure( reason );
}


FvBaseAppLoginRequest::FvBaseAppLoginRequest( FvLoginHandler & parent ) :
	FvRetryingRequest( parent, parent.BaseAppAddr(),
		BaseAppExtInterface::BaseAppLogin,
		DEFAULT_RETRY_PERIOD,
		DEFAULT_TIMEOUT_PERIOD,
		/* maxAttempts: */ 1,
		/* useParentNub: */ false ),
	m_iAttempt( m_spParent->NumBaseAppLoginAttempts() )
{
	FvServerConnection & servConn = m_spParent->ServConn();

	FvNetNub* pNub = new FvNetNub();

	pNub->IsExternal( false );
	this->SetNub( pNub );

	// FV_ASSERT( pNub->socket() != -1 );

	m_pkChannel = new FvNetChannel(
		*m_pkNub, m_spParent->BaseAppAddr(),
		FvNetChannel::EXTERNAL,
		/* minInactivityResendDelay: */ 1.0,
		servConn.pFilter().GetObject() );

	m_pkChannel->BundlePrimer( servConn );

	m_pkChannel->IsIrregular( true );

	servConn.RegisterInterfaces( *m_pkNub );

	if (m_iAttempt == 0)
	{
		m_pkNub->SwitchSockets( &servConn.GetNub() );
	}

	servConn.GetNub().RegisterChildNub( m_pkNub );

	this->Send();
}


FvBaseAppLoginRequest::~FvBaseAppLoginRequest()
{
	if (m_pkChannel)
	{
		m_pkChannel->Destroy();
		m_pkChannel = NULL;
	}

	m_spParent->AddCondemnedNub( m_pkNub );
	m_pkNub = NULL;
}

int FvBaseAppLoginRequest::HandleTimeout( FvNetTimerID id, void * arg )
{
	m_pkNub->CancelTimer( m_kTimerID );
	m_kTimerID = FV_NET_TIMER_ID_NONE;

	if (!m_bDone)
	{
		m_spParent->SendBaseAppLogin();
	}
	return 0;
}

void FvBaseAppLoginRequest::AddRequestArgs( FvNetBundle & bundle )
{
	bundle << m_spParent->ReplyRecord().m_uiSessionKey << m_iAttempt;
}

void FvBaseAppLoginRequest::OnSuccess( FvBinaryIStream & data )
{
	FvSessionKey sessionKey = 0;
	data >> sessionKey;
	m_spParent->OnBaseAppReply( this, sessionKey );

	m_pkChannel = NULL;
}


void FvBaseAppLoginRequest::Cancel()
{
	this->FvRetryingRequest::Cancel();

	m_pkNub->CancelReplyMessageHandler( this );
}


FvLoginHandler::FvLoginHandler(
		FvServerConnection* pServerConnection, FvLogOnStatus loginNotSent ) :
	m_kLoginAppAddr( FvNetAddress::NONE ),
	m_kBaseAppAddr( FvNetAddress::NONE ),
	m_spParams( NULL ),
	m_pkServerConnection( pServerConnection ),
	m_bDone( loginNotSent != FvLogOnStatus::NOT_SET ),
	m_uiStatus( loginNotSent ),
	m_iNumBaseAppLoginAttempts( 0 )
{
}


FvLoginHandler::~FvLoginHandler()
{
	for (CondemnedNubs::iterator iter = m_kCondemnedNubs.begin();
		 iter != m_kCondemnedNubs.end(); ++iter)
	{
		delete *iter;
	}
}


void FvLoginHandler::Start( const FvNetAddress & loginAppAddr,
	LogOnParamsPtr pParams )
{
	m_kLoginAppAddr = loginAppAddr;
	m_spParams = pParams;
	this->SendLoginAppLogin();
}


void FvLoginHandler::Finish()
{
	while (!m_kChildRequests.empty())
	{
		(*m_kChildRequests.begin())->Cancel();
	}
	m_pkServerConnection->GetNub().BreakProcessing();
	m_bDone = true;
}


void FvLoginHandler::SendLoginAppLogin()
{
	new FvLoginRequest( *this );
}


void FvLoginHandler::OnLoginReply( FvBinaryIStream & data )
{
	data >> m_uiStatus;

	if (m_uiStatus == FvLogOnStatus::LOGGED_ON)
	{
//#ifdef FV_USE_OPENSSL
//		if (m_pkServerConnection->pFilter())
//		{
//			FvMemoryOStream clearText;
//			m_pkServerConnection->pFilter()->DecryptStream( data, clearText );
//			clearText >> m_kReplyRecord;
//		}
//		else
//#endif
		{
			data >> m_kReplyRecord;
		}

		if (!data.Error())
		{
			m_kBaseAppAddr = m_kReplyRecord.m_kServerAddr;
			//! add by Uman, 20100211, 强制改为外网ip,因为login返回的是内网ip
			//m_kBaseAppAddr.m_uiIP = inet_addr("58.246.146.222");
			this->SendBaseAppLogin();
			m_kErrorMsg = "";
		}
		else
		{
			FV_ERROR_MSG( "FvLoginHandler::HandleMessage: "
				"Got reply of unexpected size (%d)\n",
				data.RemainingLength() );

			m_uiStatus = FvLogOnStatus::CONNECTION_FAILED;
			m_kErrorMsg = "FV_NET_REASON_CORRUPTED_PACKET";

			this->Finish();
		}
	}
	else
	{
		data >> m_kErrorMsg;

		if (m_kErrorMsg.empty())	
		{
			if (m_uiStatus == FvLogOnStatus::LOGIN_CUSTOM_DEFINED_ERROR)
			{
				m_kErrorMsg = "Unspecified error.";
			}
			else
			{
				m_kErrorMsg = "Unelaborated error.";
			}
		}

		this->Finish();
	}
}

void FvLoginHandler::SendBaseAppLogin()
{
	if (m_iNumBaseAppLoginAttempts < MAX_BASEAPP_LOGIN_ATTEMPTS)
	{
		new FvBaseAppLoginRequest( *this );
		++m_iNumBaseAppLoginAttempts;
	}
	else
	{
		m_uiStatus = FvLogOnStatus::CONNECTION_FAILED;
		m_kErrorMsg =
			"Unable to connect to BaseApp: "
			"A NAT or firewall error may have occurred?";

		this->Finish();
	}
}

void FvLoginHandler::OnBaseAppReply( FvBaseAppLoginRequestPtr pHandler,
	FvSessionKey sessionKey )
{
	FvNetNub * pMainNub = &m_pkServerConnection->GetNub();

	pHandler->GetNub().SwitchSockets( pMainNub );

	pHandler->GetChannel().SwitchNub( pMainNub );
	m_pkServerConnection->Channel( pHandler->GetChannel() );

	m_kReplyRecord.m_uiSessionKey = sessionKey;
	m_pkServerConnection->SessionKey( sessionKey );

	this->Finish();
}

void FvLoginHandler::OnFailure( FvNetReason reason )
{
	m_uiStatus = FvLogOnStatus::CONNECTION_FAILED;
	m_kErrorMsg = "Mercury::";
	m_kErrorMsg += NetReasonToString( reason );

	this->Finish();
}

void FvLoginHandler::AddChildRequest( FvRetryingRequestPtr pRequest )
{
	m_kChildRequests.insert( pRequest );
}

void FvLoginHandler::RemoveChildRequest( FvRetryingRequestPtr pRequest )
{
	m_kChildRequests.erase( pRequest );
}

void FvLoginHandler::AddCondemnedNub( FvNetNub * pNub )
{
	m_kCondemnedNubs.push_back( pNub );
}

class EntityMessageHandler : public FvNetInputMessageHandler
{
	public:
		EntityMessageHandler();

	protected:
		virtual void HandleMessage( const FvNetAddress & /* srcAddr */,
			FvNetUnpackedMessageHeader & msgHeader,
			FvBinaryIStream & data );
};

EntityMessageHandler g_entityMessageHandler;

static bool s_requestQueueEnabled;

float FvServerConnection::ms_fUpdateFrequency = 10.f;

FvServerConnection::FvServerConnection() :
	m_uiSessionKey( 0 ),
	m_pkHandler( NULL ),
	m_pkTime( NULL ),
	m_fLastTime( 0.0 ),
	m_fMinSendInterval( 1.01/20.0 ),
	m_kNub( ),
	m_pkChannel( NULL ),
	m_fInactivityTimeout( DEFAULT_INACTIVITY_TIMEOUT )
	//FIRST_AVATAR_UPDATE_MESSAGE(
	//	ClientInterface::AvatarUpdateNoAliasFullPosYawPitchRoll.GetID() ),
	//LAST_AVATAR_UPDATE_MESSAGE(
	//	ClientInterface::AvatarUpdateAliasNoPosNoDir.GetID() )
//#ifdef FV_USE_OPENSSL
//	m_spFilter( new FvNetEncryptionFilter() ),
//	m_kPublicKey( /* hasPrivate: */ false )
//#else
//	m_spFilter( NULL )
//#endif
{

	m_bTryToReconfigurePorts = false;
	this->InitialiseConnectionState();

	memset( &m_kDigest, 0, sizeof( m_kDigest ) );

	m_kNub.IsExternal( true );

	m_kNub.OnceOffResendPeriod( FV_CLIENT_ONCEOFF_RESEND_PERIOD );
	m_kNub.OnceOffMaxResends( FV_CLIENT_ONCEOFF_MAX_RESENDS );
}

FvServerConnection::~FvServerConnection()
{
	this->Disconnect();

	if (m_pkChannel)
	{
		m_pkChannel->Destroy();
		m_pkChannel = NULL;
	}
}

void FvServerConnection::InitialiseConnectionState()
{
	m_iConnectedID = FvEntityID( -1 );
	m_iSpaceID = FvSpaceID( -1 );
	m_iBandwidthFromServer = 0;

	m_fLastSendTime = 0.0;

	m_bEverReceivedPacket = false;
	m_bEntitiesEnabled = false;

	m_kServerTimeHandler = ServerTimeHandler();

	m_uiSendingSequenceNumber = 0;

	memset( m_akIDAlias, 0, sizeof( m_akIDAlias ) );

	m_kNub.IsExternal( false );

	m_kControlledEntities.clear();
}

void FvServerConnection::RegisterInterfaces( FvNetNub & nub )
{
	ClientInterface::RegisterWithNub( nub );

	for (FvNetMessageID id = 128; id != 255; id++)
	{
		nub.ServeInterfaceElement( ClientInterface::EntityMessage,
			id, &g_entityMessageHandler );
	}

	nub.pExtensionData( this );
}


FV_THREADLOCAL( bool ) g_isMainThread( false );


FvLogOnStatus FvServerConnection::LogOn( FvServerMessageHandler * pHandler,
	const char * serverName,
	const char * username,
	const char * password,
	const char * publicKeyPath,
	FvUInt16 port )
{
	FvLoginHandlerPtr pLoginHandler = this->LogOnBegin(
		serverName, username, password, publicKeyPath, port );

	while (!pLoginHandler->Done())
	{
		try
		{
			m_kNub.ProcessContinuously();
		}
		catch (FvNetNubException& ex)
		{
			FV_WARNING_MSG(
				"servconn::LogOn: Got Mercury Exception %d\n", ex.Reason() );
		}
	}

	FvLogOnStatus status = this->LogOnComplete( pLoginHandler, pHandler );

	if (status == FvLogOnStatus::LOGGED_ON)
	{
		this->EnableEntities();
	}

	return status;
}


#if defined( PLAYSTATION3 ) || defined( _XBOX360 )
static char s_pubKey[] = "-----BEGIN PUBLIC KEY-----\n"\
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA7/MNyWDdFpXhpFTO9LHz\n"\
"CUQPYv2YP5rqJjUoxAFa3uKiPKbRvVFjUQ9lGHyjCmtixBbBqCTvDWu6Zh9Imu3x\n"\
"KgCJh6NPSkddH3l+C+51FNtu3dGntbSLWuwi6Au1ErNpySpdx+Le7YEcFviY/ClZ\n"\
"ayvVdA0tcb5NVJ4Axu13NvsuOUMqHxzCZRXCe6nyp6phFP2dQQZj8QZp0VsMFvhh\n"\
"MsZ4srdFLG0sd8qliYzSqIyEQkwO8TQleHzfYYZ90wPTCOvMnMe5+zCH0iPJMisP\n"\
"YB60u6lK9cvDEeuhPH95TPpzLNUFgmQIu9FU8PkcKA53bj0LWZR7v86Oco6vFg6V\n"\
"sQIDAQAB\n"\
"-----END PUBLIC KEY-----\n";
#endif

FvLoginHandlerPtr FvServerConnection::LogOnBegin(
	const char * serverName,
	const char * username,
	const char * password,
	const char * publicKeyPath,
	FvUInt16 port )
{
	FvString key = m_spFilter ? m_spFilter->GetKey() : "";
	LogOnParamsPtr pParams = new FvLogOnParams( username, password, key );

	pParams->Digest( this->Digest() );

	g_isMainThread = true;

	if (this->Online())
	{
		return new FvLoginHandler( this, FvLogOnStatus::ALREADY_ONLINE_LOCALLY );
	}

	this->InitialiseConnectionState();

	FV_TRACE_MSG( "FvServerConnection::LogOnBegin: "
		"server:%s username:%s\n", serverName, pParams->Username().c_str() );

	m_kUsername = pParams->Username();

	this->RegisterInterfaces( m_kNub );

	FvUInt16 loginPort = port ? port : FV_NET_PORT_LOGIN;

	const char * serverPort = strchr( serverName, ':' );
	FvString serverNameStr;
	if (serverPort != NULL)
	{
		loginPort = atoi( serverPort+1 );
		serverNameStr.assign( serverName, serverPort - serverName );
		serverName = serverNameStr.c_str();
	}

	FvNetAddress loginAddr( 0, htons( loginPort ) );
	if (FvNetEndpoint::ConvertAddress( serverName, (u_int32_t&)loginAddr.m_uiIP ) != 0 ||
		loginAddr.m_uiIP == 0)
	{
		return new FvLoginHandler( this, FvLogOnStatus::DNS_LOOKUP_FAILED );
	}

	if (!publicKeyPath || *publicKeyPath == '\0')
	{
		publicKeyPath = "loginapp.pubkey";
	}

//#ifdef FV_USE_OPENSSL
//#if defined( PLAYSTATION3 ) || defined( _XBOX360 )
//	if (!m_kPublicKey.SetKey( s_pubKey ))
//	{
//		return new FvLoginHandler( this, FvLogOnStatus::PUBLIC_KEY_LOOKUP_FAILED );
//	}
//#else
//	if (!m_kPublicKey.SetKeyFromResource( publicKeyPath ))
//	{
//		return new FvLoginHandler( this, FvLogOnStatus::PUBLIC_KEY_LOOKUP_FAILED );
//	}
//#endif
//#endif

	FvLoginHandlerPtr pLoginHandler = new FvLoginHandler( this );
	pLoginHandler->Start( loginAddr, pParams );
	return pLoginHandler;
}

FvLogOnStatus FvServerConnection::LogOnComplete(
	FvLoginHandlerPtr pLoginHandler,
	FvServerMessageHandler * pHandler )
{
	FvLogOnStatus status = FvLogOnStatus::UNKNOWN_ERROR;

	FV_ASSERT_DEV( pLoginHandler != NULL );

	if (this->Online())
	{
		status = FvLogOnStatus::ALREADY_ONLINE_LOCALLY;
	}

	status = pLoginHandler->Status();

	if ((status == FvLogOnStatus::LOGGED_ON) &&
			!this->Online())
	{
		FV_WARNING_MSG( "FvServerConnection::LogOnComplete: "
				"Already logged off\n" );

		status = FvLogOnStatus::CANCELLED;
		m_kErrorMsg = "Already logged off";
	}

	if (status == FvLogOnStatus::LOGGED_ON)
	{
		FV_DEBUG_MSG( "FvServerConnection::LogOn: status==LOGGED_ON\n" );

		const FvLoginReplyRecord & result = pLoginHandler->ReplyRecord();

		FV_DEBUG_MSG( "FvServerConnection::LogOn: from: %s\n",
				m_kNub.Address().c_str() );
		FV_DEBUG_MSG( "FvServerConnection::LogOn: to:   %s\n",
				result.m_kServerAddr.c_str() );

		if (result.m_kServerAddr != this->GetAddr())
		{
			char winningAddr[ 256 ];
			strncpy( winningAddr, this->GetAddr().c_str(), sizeof( winningAddr ) );

			FV_WARNING_MSG( "FvServerConnection::LogOnComplete: "
				"BaseApp address on login reply (%s) differs from winning "
				"BaseApp reply (%s)\n",
				result.m_kServerAddr.c_str(), winningAddr );
		}
	}
	else if (status == FvLogOnStatus::CONNECTION_FAILED)
	{
		FV_ERROR_MSG( "FvServerConnection::LogOnComplete: Logon failed (%s)\n",
				pLoginHandler->ErrorMsg().c_str() );
		status = FvLogOnStatus::CONNECTION_FAILED;
		m_kErrorMsg = pLoginHandler->ErrorMsg();
	}
	else if (status == FvLogOnStatus::DNS_LOOKUP_FAILED)
	{
 		m_kErrorMsg = "DNS lookup failed";
		FV_ERROR_MSG( "FvServerConnection::LogOnComplete: Logon failed: %s\n",
				m_kErrorMsg.c_str() );
	}
	else
	{
		m_kErrorMsg = pLoginHandler->ErrorMsg();
		FV_INFO_MSG( "FvServerConnection::LogOnComplete: Logon failed: %s\n",
				m_kErrorMsg.c_str() );
	}

	pLoginHandler = NULL;

	if (status != FvLogOnStatus::LOGGED_ON)
	{
		return status;
	}

	m_iConnectedID = 0;

	s_requestQueueEnabled = true;

	m_kNub.IsExternal( true );

	//! add by Uman
	FV_DEBUG_MSG( "FvServerConnection::Channel: %s\n",
		Channel().addr().c_str());
	//! add end

	this->PrimeBundle( this->GetBundle() );
	this->Send();

	// FV_DEBUG_MSG( "FvServerConnection::LogOn: sent initial message to server\n" );

	m_pkHandler = pHandler;

	this->Channel().StartInactivityDetection( m_fInactivityTimeout );

	return status;
}

void FvServerConnection::EnableEntities()
{
	BaseAppExtInterface::EnableEntitiesArgs & args =
		BaseAppExtInterface::EnableEntitiesArgs::start(
			this->GetBundle(), FV_NET_RELIABLE_DRIVER );

	args.dummy = 0;

	FV_DEBUG_MSG( "FvServerConnection::EnableEntities: Enabling entities\n" );

	this->Send();

	m_bEntitiesEnabled = true;
}

bool FvServerConnection::Online() const
{
	return m_pkChannel != NULL;
}

void FvServerConnection::Disconnect( bool informServer )
{
	if (!this->Online()) return;

	if (informServer)
	{
		BaseAppExtInterface::DisconnectClientArgs::start( this->GetBundle(),
			FV_NET_RELIABLE_NO ).reason = 0;

		this->Channel().Send();
	}

	if (m_pkChannel != NULL)
	{
		m_kNub.IsExternal(false);
		m_pkChannel->Destroy();
		m_pkChannel = NULL;
	}

	m_pkHandler = NULL;
	m_uiSessionKey = 0;
}

FvNetChannel & FvServerConnection::Channel()
{
	FV_ASSERT_DEV( m_pkChannel );
	return *m_pkChannel;
}


const FvNetAddress & FvServerConnection::GetAddr() const
{
	return m_kNub.Address();
}


void FvServerConnection::AddMove( FvEntityID id, FvSpaceID spaceID,
	FvEntityID vehicleID, const FvVector3 & pos, float yaw, float pitch, float roll,
	bool onGround, const FvVector3 & globalPos )
{
/**
	if (this->Offline())
		return;

	if (spaceID != m_iSpaceID)
	{
		FV_ERROR_MSG( "FvServerConnection::AddMove: "
					"Attempted to move %u from space %u to space %u\n",
				id, m_iSpaceID, spaceID );
		return;
	}

	if (!this->IsControlledLocally( id ))
	{
		FV_ERROR_MSG( "FvServerConnection::AddMove: "
				"Tried to add a move for entity id %u that we do not control\n",
			id );
		return;
	}

	bool changedVehicle = false;
	FvEntityID currVehicleID = this->GetVehicleID( id );

	if (vehicleID != currVehicleID)
	{
		this->SetVehicle( id, vehicleID );
		changedVehicle = true;
	}

	FvCoord coordPos( FV_HTONF( pos.x ), FV_HTONF( pos.y ), FV_HTONF( pos.z ) );
	FvYawPitchRoll dir( yaw , pitch, roll );

	FvNetBundle & bundle = this->GetBundle();

	if (id == m_iConnectedID)
	{
		FvUInt8 refNum = m_uiSendingSequenceNumber;
		m_akSentPositions[ m_uiSendingSequenceNumber ] = globalPos;
		++m_uiSendingSequenceNumber;

		if (!changedVehicle)
		{
			BaseAppExtInterface::AvatarUpdateImplicitArgs & upArgs =
				BaseAppExtInterface::AvatarUpdateImplicitArgs::start(
						bundle, FV_NET_RELIABLE_NO );

			upArgs.pos = coordPos;
			upArgs.dir = dir;
			upArgs.refNum = refNum;
		}
		else
		{
			BaseAppExtInterface::AvatarUpdateExplicitArgs & upArgs =
				BaseAppExtInterface::AvatarUpdateExplicitArgs::start( bundle,
						FV_NET_RELIABLE_NO );

			upArgs.spaceID = FV_HTONL( spaceID );
			upArgs.vehicleID = FV_HTONL( vehicleID );
			upArgs.onGround = onGround;

			upArgs.pos = coordPos;
			upArgs.dir = dir;
			upArgs.refNum = refNum;
		}
	}
	else
	{
		if (!changedVehicle)
		{
			BaseAppExtInterface::AvatarUpdateWardImplicitArgs & upArgs =
				BaseAppExtInterface::AvatarUpdateWardImplicitArgs::start(
						bundle, FV_NET_RELIABLE_NO );

			upArgs.ward = FV_HTONL( id );

			upArgs.pos = coordPos;
			upArgs.dir = dir;
		}
		else
		{
			BaseAppExtInterface::AvatarUpdateWardExplicitArgs & upArgs =
				BaseAppExtInterface::AvatarUpdateWardExplicitArgs::start(
						bundle, FV_NET_RELIABLE_NO );

			upArgs.ward = FV_HTONL( id );
			upArgs.spaceID = FV_HTONL( spaceID );
			upArgs.vehicleID = FV_HTONL( vehicleID );
			upArgs.onGround = onGround;

			upArgs.pos = coordPos;
			upArgs.dir = dir;
		}
	}
**/
}

FvBinaryOStream* FvServerConnection::StartProxyMessage( int messageId )
{
	if (this->Offline())
	{
		FV_WARNING_MSG( "FvServerConnection::StartProxyMessage: "
				"Called when not connected to server!\n" );
		return NULL;
	}

	static FvNetInterfaceElement anie = BaseAppExtInterface::EntityMessage;
	anie.SetID( ((unsigned char)messageId) | 0xC0 );
	this->GetBundle().StartMessage( anie, /*isReliable:*/true );

	return &(this->GetBundle());
}

FvBinaryOStream* FvServerConnection::StartAvatarMessage( int messageId )
{
	if (this->Offline())
	{
		FV_WARNING_MSG( "FvServerConnection::StartEntityMessage: "
			"Called when not connected to server!\n" );
		return NULL;
	}

	static FvNetInterfaceElement anie = BaseAppExtInterface::EntityMessage;
	anie.SetID( ((unsigned char)messageId) | 0x40 );
	this->GetBundle().StartMessage( anie, /*isReliable:*/true );

	return &(this->GetBundle());
}

FvBinaryOStream* FvServerConnection::StartEntityMessage( int messageId,
		FvEntityID entityId )
{
	if (this->Offline())
	{
		FV_WARNING_MSG( "FvServerConnection::StartEntityMessage: "
				"Called when not connected to server!\n" );
		return NULL;
	}

	static FvNetInterfaceElement anie = BaseAppExtInterface::EntityMessage;
	anie.SetID( ((unsigned char)messageId) | 0x80 );
	this->GetBundle().StartMessage( anie, /*isReliable:*/true );
	this->GetBundle() << entityId;

	return &(this->GetBundle());
}

void FvServerConnection::StartGMMessage(bool bToBase, FvEntityID iEntityID, FvUInt16 uiMethodID, FvBinaryIStream& kIS)
{
	if(this->Offline())
	{
		FV_ERROR_MSG( "FvServerConnection::StartGMMessage: "
			"Called when not connected to server!\n" );
		return;
	}

	this->GetBundle().StartMessage( BaseAppExtInterface::GMCmd, /*isReliable:*/true );
	this->GetBundle() << bToBase << iEntityID << uiMethodID;
	this->GetBundle().Transfer(kIS, kIS.RemainingLength());
}

bool FvServerConnection::ProcessInput()
{
	bool gotAnyPackets = false;
	try
	{
		bool gotAPacket = false;
		do
		{
			gotAPacket = m_kNub.ProcessPendingEvents();

			gotAnyPackets |= gotAPacket;
			m_bEverReceivedPacket |= gotAPacket;
		}
		while (gotAPacket);
	}
	catch (FvNetNubException & ne)
	{
		switch (ne.Reason())
		{
			case FV_NET_REASON_CORRUPTED_PACKET:
			{
				FV_ERROR_MSG( "FvServerConnection::ProcessInput: "
					"Dropped corrupted incoming packet\n" );
				break;
			}
			case FV_NET_REASON_INACTIVITY:
			{
				if (this->Online())
				{
					FV_ERROR_MSG( "FvServerConnection::ProcessInput: "
						"Disconnecting due to nub exception (%s)\n",
						NetReasonToString( ne.Reason() ) );

					this->Disconnect();
				}

				break;
			}
			default:

				FV_WARNING_MSG( "FvServerConnection::ProcessInput: "
					"Got a nub exception (%s)\n",
					NetReasonToString( ne.Reason() ) );
		}
	}

	if (!this->Online())
	{
		return gotAnyPackets;
	}

	if (gotAnyPackets)
	{
		static FvUInt64 lastTimeStamp = Timestamp();
		FvUInt64 currTimeStamp = Timestamp();
		FvUInt64 delta = (currTimeStamp - lastTimeStamp)
						* FvUInt64( 1000 ) / StampsPerSecond();
		int deltaInMS = int( delta );

		if (deltaInMS > 400)
		{
			//! TODO: 暂时关闭
			//FV_WARNING_MSG( "FvServerConnection::ProcessInput: "
			//	"There were %d ms between packets\n", deltaInMS );
		}

		lastTimeStamp = currTimeStamp;
	}

	return gotAnyPackets;
}

void FvServerConnection::HandleEntityMessage( int messageID, FvBinaryIStream & data,
	int length )
{
	FvEntityID objectID;
	data >> objectID;
	length -= sizeof( objectID );

	if (m_pkHandler)
	{
		const int PROPERTY_FLAG = 0x40;

		if (messageID & PROPERTY_FLAG)
		{
			m_pkHandler->OnEntityMethod( objectID,
				messageID & ~PROPERTY_FLAG, data );
		}
		else
		{
			m_pkHandler->OnEntityProperty( objectID,
				messageID, data );
		}
	}
}


void FvServerConnection::SetVehicle( FvEntityID passengerID, FvEntityID vehicleID )
{
	if (vehicleID)
	{
		m_kPassengerToVehicle[ passengerID ] = vehicleID;
	}
	else
	{
		m_kPassengerToVehicle.erase( passengerID );
	}
}



void FvServerConnection::DetailedPosition(
	const ClientInterface::DetailedPositionArgs & args )
{
	FvEntityID entityID = args.id;
	FvEntityID vehicleID = GetVehicleID( entityID );

	if ((m_pkHandler != NULL) &&
		entityID != m_iConnectedID)
	{
		m_pkHandler->OnEntityMoveWithError(
			entityID,
			m_iSpaceID,
			vehicleID,
			args.position,
			FvVector3::ZERO,
			args.direction.Yaw(),
			args.direction.Pitch(),
			args.direction.Roll(),
			false );
	}

/**
	FvEntityID entityID = args.id;
	FvEntityID vehicleID = this->GetVehicleID( entityID );

	this->DetailedPositionReceived(
		entityID, m_iSpaceID, 0, args.position );

	if ((m_pkHandler != NULL) &&
			!this->IsControlledLocally( entityID ))
	{
		m_pkHandler->OnEntityMoveWithError(
			entityID,
			m_iSpaceID,
			vehicleID,
			args.position,
			FvVector3::ZERO,
			args.direction.Yaw(),
			args.direction.Pitch(),
			args.direction.Roll(),
			false );
	}
**/
}

void FvServerConnection::ForcedPosition(
	const ClientInterface::ForcedPositionArgs & args )
{
	if(args.id != m_iConnectedID)
		return;
/**
	if (!this->IsControlledLocally( args.id ))
	{
		FV_WARNING_MSG( "FvServerConnection::ForcedPosition: "
			"Received forced position for entity %u that we do not control\n",
			args.id );
		return;
	}
**/
	if (args.id == m_iConnectedID)
	{
		if ((m_iSpaceID != 0) &&
				(m_iSpaceID != args.spaceID) &&
				(m_pkHandler != NULL))
		{
			m_pkHandler->SpaceGone( m_iSpaceID );
		}

		m_iSpaceID = args.spaceID;
/**
		BaseAppExtInterface::AckPhysicsCorrectionArgs & ackArgs =
					BaseAppExtInterface::AckPhysicsCorrectionArgs::start(
							this->GetBundle() );

		ackArgs.dummy = 0;
**/
	}
	else
	{
/**
		BaseAppExtInterface::AckWardPhysicsCorrectionArgs & ackArgs =
					BaseAppExtInterface::AckWardPhysicsCorrectionArgs::start(
							this->GetBundle() );

		ackArgs.ward = FV_HTONL( args.id );
		ackArgs.dummy = 0;
**/
	}


	if (m_pkHandler != NULL)
	{
		m_pkHandler->OnEntityMoveWithError(
			args.id,
			args.spaceID,
			args.vehicleID,
			args.position,
			FvVector3::ZERO,
			args.direction.Yaw(),
			args.direction.Pitch(),
			args.direction.Roll(),
			false );
	}
}

void FvServerConnection::ControlEntity(
	const ClientInterface::ControlEntityArgs & args )
{
	if (args.on)
	{
		m_kControlledEntities.insert( args.id );
	}
	else
	{
		m_kControlledEntities.erase( args.id );
	}

	if (m_pkHandler != NULL)
	{
		m_pkHandler->OnEntityControl( args.id, args.on );
	}
}

void FvServerConnection::DetailedPositionReceived( FvEntityID id,
	FvSpaceID spaceID, FvEntityID vehicleID, const FvVector3 & position )
{
	if ((id == m_iConnectedID) && (vehicleID == 0))
	{
		m_kReferencePosition = ::CalculateReferencePosition( position );
	}
}


static void (*s_bandwidthFromServerMutator)( int bandwidth ) = NULL;
void FV_ROBOT_API SetBandwidthFromServerMutator( void (*mutatorFn)( int bandwidth ) )
{
	s_bandwidthFromServerMutator = mutatorFn;
}

int FvServerConnection::BandwidthFromServer() const
{
	return m_iBandwidthFromServer;
}

void FvServerConnection::BandwidthFromServer( int bandwidth )
{
	if (s_bandwidthFromServerMutator == NULL)
	{
		FV_ERROR_MSG( "FvServerConnection::BandwidthFromServer: Cannot comply "
			"since no mutator set with 'SetBandwidthFromServerMutator'\n" );
		return;
	}

	const int MIN_BANDWIDTH = 0;
	const int MAX_BANDWIDTH = PACKET_MAX_SIZE * FV_NET_BITS_PER_BYTE * 10 / 2;

	bandwidth = FvClampEx( MIN_BANDWIDTH, bandwidth, MAX_BANDWIDTH );

	(*s_bandwidthFromServerMutator)( bandwidth );
}

double FvServerConnection::BPSIn() const
{
	const_cast<FvServerConnection *>(this)->UpdateStats();

	return m_kBitsIn.ChangePerSecond();
}

double FvServerConnection::BPSOut() const
{
	const_cast<FvServerConnection *>(this)->UpdateStats();

	return m_kBitsOut.ChangePerSecond();
}

double FvServerConnection::PacketsPerSecondIn() const
{
	const_cast<FvServerConnection *>(this)->UpdateStats();

	return m_kPacketsIn.ChangePerSecond();
}

double FvServerConnection::PacketsPerSecondOut() const
{
	const_cast<FvServerConnection *>(this)->UpdateStats();

	return m_kPacketsOut.ChangePerSecond();
}

double FvServerConnection::MessagesPerSecondIn() const
{
	const_cast<FvServerConnection *>(this)->UpdateStats();

	return m_kMessagesIn.ChangePerSecond();
}

double FvServerConnection::MessagesPerSecondOut() const
{
	const_cast<FvServerConnection *>(this)->UpdateStats();

	return m_kMessagesOut.ChangePerSecond();
}

double FvServerConnection::MovementBytesPercent() const
{
	const_cast<FvServerConnection *>(this)->UpdateStats();

	return m_kMovementBytes.ChangePerSecond() /
		m_kTotalBytes.ChangePerSecond() * 100.0;
}

double FvServerConnection::NonMovementBytesPercent() const
{
	const_cast<FvServerConnection *>(this)->UpdateStats();

	return m_kNonMovementBytes.ChangePerSecond() /
		m_kTotalBytes.ChangePerSecond() * 100.0;
}

double FvServerConnection::OverheadBytesPercent() const
{
	const_cast<FvServerConnection *>(this)->UpdateStats();

	return m_kOverheadBytes.ChangePerSecond() /
		m_kTotalBytes.ChangePerSecond() * 100.0;
}

int FvServerConnection::MovementBytesTotal() const
{
	return m_kMovementBytes;
}

int FvServerConnection::NonMovementBytesTotal() const
{
	return m_kNonMovementBytes;
}

int FvServerConnection::OverheadBytesTotal() const
{
	return m_kOverheadBytes;
}

int FvServerConnection::MovementMessageCount() const
{
	int count = 0;
	//for (int i = FIRST_AVATAR_UPDATE_MESSAGE;
	//			i <= LAST_AVATAR_UPDATE_MESSAGE; i++)
	//{
	//	m_kNub.NumMessagesReceivedForMessage( i );
	//}

	return count;
}

void FvServerConnection::UpdateStats()
{
	const double UPDATE_PERIOD = 2.0;

	double timeDelta = this->AppTime() - m_fLastTime;

	if ( timeDelta > UPDATE_PERIOD )
	{
		m_fLastTime = this->AppTime();

		m_kPacketsIn = m_kNub.NumPacketsReceived();
		m_kMessagesIn = m_kNub.NumMessagesReceived();
		m_kBitsIn = m_kNub.NumBytesReceived() * FV_NET_BITS_PER_BYTE;

		//m_kMovementBytes = m_kNub.NumBytesReceivedForMessage(
		//				ClientInterface::RelativePositionReference.GetID() );

		//for (int i = FIRST_AVATAR_UPDATE_MESSAGE;
		//		i <= LAST_AVATAR_UPDATE_MESSAGE; i++)
		//{
		//	m_kMovementBytes += m_kNub.NumBytesReceivedForMessage( i );
		//}

		m_kTotalBytes = m_kNub.NumBytesReceived();
		m_kOverheadBytes = m_kNub.NumOverheadBytesReceived();
		m_kNonMovementBytes = m_kTotalBytes - m_kMovementBytes - m_kOverheadBytes;

		m_kPacketsIn.Update( timeDelta );
		m_kPacketsOut.Update( timeDelta );

		m_kBitsIn.Update( timeDelta );
		m_kBitsOut.Update( timeDelta );

		m_kMessagesIn.Update( timeDelta );
		m_kMessagesOut.Update( timeDelta );

		m_kTotalBytes.Update( timeDelta );
		m_kMovementBytes.Update( timeDelta );
		m_kNonMovementBytes.Update( timeDelta );
		m_kOverheadBytes.Update( timeDelta );
	}
}

void FvServerConnection::Send()
{
	if (this->Offline())
		return;

	if (m_bTryToReconfigurePorts && !m_bEverReceivedPacket)
	{
		FvNetBundle bundle;
		bundle.StartMessage( BaseAppExtInterface::Authenticate, false );
		bundle << m_uiSessionKey;
		this->GetNub().Send( this->GetAddr(), bundle );
	}

	if (m_pkTime)
		m_fLastSendTime = *m_pkTime;

	const FvNetBundle & bundle = this->GetBundle();
	m_kPacketsOut += bundle.SizeInPackets();
	m_kMessagesOut += bundle.NumMessages();
	m_kBitsOut += (bundle.Size() + FV_NET_UDP_OVERHEAD) * FV_NET_BITS_PER_BYTE;

	this->Channel().Send();

	const int OVERFLOW_LIMIT = 1024;

	if (this->Channel().SendWindowUsage() > OVERFLOW_LIMIT)
	{
		FV_WARNING_MSG( "FvServerConnection::Send: "
			"Disconnecting since channel has overflowed.\n" );

		this->Disconnect();
	}
}

void FvServerConnection::PrimeBundle( FvNetBundle & bundle )
{
	if (m_uiSessionKey)
	{
		bundle.StartMessage( BaseAppExtInterface::Authenticate, false );
		bundle << m_uiSessionKey;
	}
}

int FvServerConnection::NumUnreliableMessages() const
{
	return m_uiSessionKey ? 1 : 0;
}

void FvServerConnection::RequestEntityUpdate( FvEntityID id,
	const FvCacheStamps & stamps )
{
	if (this->Offline())
		return;

	this->GetBundle().StartMessage( BaseAppExtInterface::RequestEntityUpdate,
		  /*isReliable:*/true );
	this->GetBundle() << id;

	FvCacheStamps::const_iterator iter = stamps.begin();

	while (iter != stamps.end())
	{
		this->GetBundle() << (*iter);

		iter++;
	}
}

float FvServerConnection::Latency() const
{
	return m_pkChannel ? float( m_pkChannel->RoundTripTimeInSeconds() ) : 0.f;
}

EntityMessageHandler::EntityMessageHandler()
{
}

void EntityMessageHandler::HandleMessage(
	const FvNetAddress & /* srcAddr */,
	FvNetUnpackedMessageHeader & header,
	FvBinaryIStream & data )
{
	FvServerConnection * pServConn =
		(FvServerConnection *)header.m_pkNub->pExtensionData();
	pServConn->HandleEntityMessage( header.m_uiIdentifier & 0x7F, data,
		header.m_iLength );
}

template <class ARGS>
class ClientMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvServerConnection::*Handler)( const ARGS & args );

	ClientMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & /*srcAddr*/,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data )
	{
#ifndef _BIG_ENDIAN
		ARGS & args = *(ARGS*)data.Retrieve( sizeof(ARGS) );
#else
		ARGS args;
		data >> args;
#endif

		FvServerConnection * pServConn =
			(FvServerConnection *)header.m_pkNub->pExtensionData();
		(pServConn->*handler_)( args );
	}

	Handler handler_;
};

class ClientVarLenMessageHandler : public FvNetInputMessageHandler
{
public:
	typedef void (FvServerConnection::*Handler)( FvBinaryIStream & stream,
			int length );

	ClientVarLenMessageHandler( Handler handler ) : handler_( handler ) {}

private:
	virtual void HandleMessage( const FvNetAddress & /*srcAddr*/,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data )
	{
		FvServerConnection * pServConn =
			(FvServerConnection *)header.m_pkNub->pExtensionData();
		(pServConn->*handler_)( data, header.m_iLength );
	}

	Handler handler_;
};

class ClientVarLenWithAddrMessageHandler : public FvNetInputMessageHandler
{
	public:
		typedef void (FvServerConnection::*Handler)(
				const FvNetAddress & srcAddr,
				FvBinaryIStream & stream,
				int length );

		ClientVarLenWithAddrMessageHandler( Handler handler ) :
			handler_( handler ) {}

	private:
		virtual void HandleMessage( const FvNetAddress & srcAddr,
				FvNetUnpackedMessageHeader & header,
				FvBinaryIStream & data )
		{
			FvServerConnection * pServConn =
				(FvServerConnection *)header.m_pkNub->pExtensionData();
			(pServConn->*handler_)( srcAddr, data, header.m_iLength );
		}

		Handler handler_;
};

double FvServerConnection::ServerTime( double clientTime ) const
{
	return m_kServerTimeHandler.ServerTime( clientTime );
}

double FvServerConnection::ServerTime() const
{
	return m_kServerTimeHandler.ServerTime( Timestamp()/StampsPerSecondD() );
}

double FvServerConnection::LastMessageTime() const
{
	return m_kServerTimeHandler.LastMessageTime();
}

FvTimeStamp FvServerConnection::LastGameTime() const
{
	return m_kServerTimeHandler.LastGameTime();
}

double FvServerConnection::SmoothServerTimeInDay() const
{
	return m_kServerTimeHandler.SmoothServerTimeInDay();
}


const double FvServerConnection::ServerTimeHandler::UNINITIALISED_TIME = -1000.0;
const double SECONDS_IN_DAY = 86400.0;	//! 一天多少秒

FvServerConnection::ServerTimeHandler::ServerTimeHandler() :
	m_uiTickByte( 0 ),
	m_fTimeAtSequenceStart( UNINITIALISED_TIME ),
	m_uiGameTimeAtSequenceStart( 0 )
{
}

void FvServerConnection::ServerTimeHandler::GameTime( FvTimeStamp newGameTime,
		double currentTime )
{
	m_uiTickByte = FvUInt8( newGameTime );
	m_uiGameTimeAtSequenceStart = newGameTime - m_uiTickByte;
	m_fTimeAtSequenceStart = currentTime -
		double(m_uiTickByte) / FvServerConnection::UpdateFrequency();

	m_uiCliTimeStartForSmoothSvrTime = Timestamp();
	m_fSvrTimeStartForSmoothSvrTime = newGameTime / FvServerConnection::UpdateFrequency();
	m_fSvrTimeStartForSmoothSvrTime = fmod(m_fSvrTimeStartForSmoothSvrTime, SECONDS_IN_DAY);
	m_fTimeScalingForSmoothSvrTime = 1.0;
}

void FvServerConnection::ServerTimeHandler::TickSync( FvUInt8 newSeqNum,
		double currentTime )
{
	const float updateFrequency = FvServerConnection::UpdateFrequency();
	const double SEQUENCE_PERIOD = 256.0/updateFrequency;
	const FvInt32 SEQUENCE_PERIOD_INT = 256;

	const FvUInt8 LAST_HEAD_SEQ_NUM = 256/3 - 1;
	const FvUInt8 FIRST_TAIL_SEQ_NUM = 255 - LAST_HEAD_SEQ_NUM;

	if (m_fTimeAtSequenceStart == UNINITIALISED_TIME)
	{
		return;
	}

	if (m_uiTickByte >= FIRST_TAIL_SEQ_NUM &&
		newSeqNum <= LAST_HEAD_SEQ_NUM)
	{
		m_fTimeAtSequenceStart += SEQUENCE_PERIOD;
		m_uiGameTimeAtSequenceStart += SEQUENCE_PERIOD_INT;
	}
	else if (newSeqNum >= FIRST_TAIL_SEQ_NUM &&
		m_uiTickByte <= LAST_HEAD_SEQ_NUM)
	{
		FV_WARNING_MSG( "ServerTimeHandler::sequenceNumber: "
				"Got a reverse change over (%d, %d)\n",
			m_uiTickByte, newSeqNum );
		m_fTimeAtSequenceStart -= SEQUENCE_PERIOD;
		m_uiGameTimeAtSequenceStart -= SEQUENCE_PERIOD_INT;
	}

	if (FvUInt8( newSeqNum - m_uiTickByte ) > 0x80)
	{
		FV_DEBUG_MSG( "Non-sequential sequence numbers. Wanted %d, got %d\n",
			FvUInt8( m_uiTickByte + 1 ), newSeqNum );
	}

	m_uiTickByte = newSeqNum;

	double timeError = currentTime - this->LastMessageTime();
	const double MAX_TIME_ERROR = 0.05;
	const double MAX_TIME_ADJUST = 0.005;

	if (timeError > MAX_TIME_ERROR)
	{
		m_fTimeAtSequenceStart += FV_MIN( timeError, MAX_TIME_ADJUST );

		while (timeError > 2 * SEQUENCE_PERIOD/3.f)
		{
			m_fTimeAtSequenceStart += SEQUENCE_PERIOD;
			timeError -= SEQUENCE_PERIOD;
		}
	}
	else if (-timeError > MAX_TIME_ERROR)
	{
		m_fTimeAtSequenceStart += FV_MAX( timeError, -MAX_TIME_ADJUST );

		while (timeError < -2 * SEQUENCE_PERIOD/3.f)
		{
			m_fTimeAtSequenceStart -= SEQUENCE_PERIOD;
			timeError += SEQUENCE_PERIOD;
		}
	}

	if (timeError < -30.f || 30.f < timeError)
	{
		FV_WARNING_MSG( "Time error is %f. Client = %.3f. Server = %.3f.\n",
			timeError,
			currentTime,
			this->LastMessageTime() );
	}


	double fNewSvrTime = ServerTime(Timestamp() / StampsPerSecondD());
	double fSmoothSvrTime = SmoothServerTimeInDay();
	double fSvrTimeDiff = fNewSvrTime - fSmoothSvrTime;
	fSvrTimeDiff = fmod(fSvrTimeDiff, SECONDS_IN_DAY);
	double fDiffTime(3.0);
	if(fSvrTimeDiff > 0.0)
	{
		if(fSvrTimeDiff > fDiffTime * fDiffTime * fDiffTime)
			m_fTimeScalingForSmoothSvrTime = 1.3;
		else if(fSvrTimeDiff > fDiffTime * fDiffTime)
			m_fTimeScalingForSmoothSvrTime = 1.2;
		else if(fSvrTimeDiff > fDiffTime)
			m_fTimeScalingForSmoothSvrTime = 1.1;
		else
			m_fTimeScalingForSmoothSvrTime = 1.0;
	}
	else
	{
		if(-fSvrTimeDiff > fDiffTime * fDiffTime * fDiffTime)
			m_fTimeScalingForSmoothSvrTime = 0.7;
		else if(-fSvrTimeDiff > fDiffTime * fDiffTime)
			m_fTimeScalingForSmoothSvrTime = 0.8;
		else if(-fSvrTimeDiff > fDiffTime)
			m_fTimeScalingForSmoothSvrTime = 0.9;
		else
			m_fTimeScalingForSmoothSvrTime = 1.0;
	}

	m_uiCliTimeStartForSmoothSvrTime = Timestamp();
	m_fSvrTimeStartForSmoothSvrTime = fSmoothSvrTime;
}

double FvServerConnection::ServerTimeHandler::ServerTime( double clientTime ) const
{
	return (m_uiGameTimeAtSequenceStart / FvServerConnection::UpdateFrequency()) +
		(clientTime - m_fTimeAtSequenceStart);
}

double FvServerConnection::ServerTimeHandler::LastMessageTime() const
{
	return m_fTimeAtSequenceStart +
		double(m_uiTickByte) / FvServerConnection::UpdateFrequency();
}

FvTimeStamp FvServerConnection::ServerTimeHandler::LastGameTime() const
{
	return m_uiGameTimeAtSequenceStart + m_uiTickByte;
}

double FvServerConnection::ServerTimeHandler::SmoothServerTimeInDay() const
{
	int iDTime = Timestamp() - m_uiCliTimeStartForSmoothSvrTime;
	return iDTime * m_fTimeScalingForSmoothSvrTime / StampsPerSecondD() + m_fSvrTimeStartForSmoothSvrTime;
}

void FvServerConnection::InitDebugInfo()
{
	FV_WATCH(  "Comms/Desired bps in",
		*this,
		FV_ACCESSORS( int, FvServerConnection, BandwidthFromServer ) );

	FV_WATCH( "Comms/bps in",	*this, &FvServerConnection::BPSIn );
	FV_WATCH( "Comms/bps out",	*this, &FvServerConnection::BPSOut );

	FV_WATCH( "Comms/PacketsSec in ", *this,
		&FvServerConnection::PacketsPerSecondIn );
	FV_WATCH( "Comms/PacketsSec out", *this,
		&FvServerConnection::PacketsPerSecondOut );

	FV_WATCH( "Comms/Messages in",	*this,
		&FvServerConnection::MessagesPerSecondIn );
	FV_WATCH( "Comms/Messages out",	*this,
		&FvServerConnection::MessagesPerSecondOut );

	FV_WATCH( "Comms/Expected Freq", FvServerConnection::ms_fUpdateFrequency,
		FvWatcher::WT_READ_ONLY );

	FV_WATCH( "Comms/Game Time", *this, &FvServerConnection::LastGameTime );

	FV_WATCH( "Comms/Movement pct", *this, &FvServerConnection::MovementBytesPercent);
	FV_WATCH( "Comms/Non-move pct", *this, &FvServerConnection::NonMovementBytesPercent);
	FV_WATCH( "Comms/Overhead pct", *this, &FvServerConnection::OverheadBytesPercent);

	FV_WATCH( "Comms/Movement total", *this, &FvServerConnection::MovementBytesTotal);
	FV_WATCH( "Comms/Non-move total", *this, &FvServerConnection::NonMovementBytesTotal);
	FV_WATCH( "Comms/Overhead total", *this, &FvServerConnection::OverheadBytesTotal);

	FV_WATCH( "Comms/Movement count", *this, &FvServerConnection::MovementMessageCount);
	FV_WATCH( "Comms/Packet count", m_kNub, &FvNetNub::NumPacketsReceived );

	FV_WATCH( "Comms/Latency", *this, &FvServerConnection::Latency );

}

void FvServerConnection::Authenticate(
	const ClientInterface::AuthenticateArgs & args )
{
	if (args.key != m_uiSessionKey)
	{
		FV_ERROR_MSG( "FvServerConnection::Authenticate: "
				   "Unexpected key! (%x, wanted %x)\n",
				   args.key, m_uiSessionKey );
		return;
	}
}

void FvServerConnection::BandwidthNotification(
	const ClientInterface::BandwidthNotificationArgs & args )
{
	// FV_TRACE_MSG( "FvServerConnection::BandwidthNotification: %d\n", args.bps);
	m_iBandwidthFromServer = args.bps;
}

void FvServerConnection::UpdateFrequencyNotification(
		const ClientInterface::UpdateFrequencyNotificationArgs & args )
{
	ms_fUpdateFrequency = (float)args.hertz;
}

void FvServerConnection::TickSync(
	const ClientInterface::TickSyncArgs & args )
{
	m_kServerTimeHandler.TickSync( args.tickByte, Timestamp()/StampsPerSecondD() );
}

void FvServerConnection::SetGameTime(
	const ClientInterface::SetGameTimeArgs & args )
{
	m_kServerTimeHandler.GameTime( args.gameTime, Timestamp()/StampsPerSecondD() );

#ifndef FV_ROBOT_EXPORT
	//! 客户端的处理
	PFN_ONSETGAMETIME pFnOnSetGameTime = (PFN_ONSETGAMETIME)GetProcAddress(SaintsApplication::GetKernelDLL(), "OnSetGameTime");
	if(pFnOnSetGameTime)
		pFnOnSetGameTime(args.gameTime, ms_fUpdateFrequency);
#else
	//! 机器人的处理

#endif
}

void FvServerConnection::ResetEntities(
	const ClientInterface::ResetEntitiesArgs & args )
{
	FV_ASSERT(m_bEntitiesEnabled);

	this->Channel().IsIrregular(true);

	//this->Send();

	//m_kControlledEntities.clear();
	m_kPassengerToVehicle.clear();
	//m_kCreateCellPlayerMsg.Reset();

	//if (!args.keepPlayerOnBase)
	//{
	//	m_iConnectedID = 0;

	//	for (unsigned int i = 0; i < m_kDataDownloads.size(); ++i)
	//		delete m_kDataDownloads[i];
	//	m_kDataDownloads.clear();
	//}

	//this->Send();

	m_bEntitiesEnabled = false;
	this->EnableEntities();

	if (m_pkHandler)
	{
		m_pkHandler->OnEntitiesReset( args.keepPlayerOnBase );
	}
}

void FvServerConnection::CreateBasePlayer( FvBinaryIStream & stream,
										int /*length*/ )
{
	FvEntityID playerID = 0;
	stream >> playerID;

	FV_INFO_MSG( "FvServerConnection::CreateBasePlayer: id %u\n", playerID );

	m_iConnectedID = playerID;

	FvEntityTypeID playerType = FvEntityTypeID(-1);
	stream >> playerType;

	if (m_pkHandler)
	{
		m_pkHandler->OnBasePlayerCreate( m_iConnectedID, playerType,
			stream );
	}

	//if (m_kCreateCellPlayerMsg.RemainingLength() > 0)
	//{
	//	FV_INFO_MSG( "FvServerConnection::CreateBasePlayer: "
	//		"Playing buffered CreateCellPlayer message\n" );
	//	this->CreateCellPlayer( m_kCreateCellPlayerMsg,
	//		m_kCreateCellPlayerMsg.RemainingLength() );
	//	m_kCreateCellPlayerMsg.Reset();
	//}
}

void FvServerConnection::CreateCellPlayer( FvBinaryIStream & stream,
										int /*length*/ )
{
	FV_ASSERT(m_iConnectedID);
	FV_INFO_MSG( "FvServerConnection::CreateCellPlayer: id %u\n", m_iConnectedID );

	FvEntityID vehicleID;
	FvPosition3D pos;
	FvDirection3 dir;
	stream >> m_iSpaceID >> vehicleID >> pos >> dir;

	//m_kControlledEntities.insert( m_iConnectedID );

	this->SetVehicle( m_iConnectedID, vehicleID );

	this->Channel().IsIrregular( false );

	if (m_pkHandler)
	{
		m_pkHandler->OnCellPlayerCreate( m_iConnectedID,
			m_iSpaceID, vehicleID, pos, dir.Yaw(), dir.Pitch(), dir.Roll(),
			stream );
	}

	//this->DetailedPositionReceived( m_iConnectedID, m_iSpaceID, vehicleID, pos );
}

void FvServerConnection::SpaceData( FvBinaryIStream & stream, int length )
{
	FvSpaceID spaceID;
	FvSpaceEntryID spaceEntryID;
	FvUInt16 key;
	FvString data;

	stream >> spaceID >> spaceEntryID >> key;
	length -= sizeof(spaceID) + sizeof(spaceEntryID) + sizeof(key);
	data.assign( (char*)stream.Retrieve( length ), length );

	FV_TRACE_MSG( "FvServerConnection::SpaceData: space %u key %hu\n",
		spaceID, key );

	if (m_pkHandler)
	{
		m_pkHandler->SpaceData( spaceID, spaceEntryID, key, data );
	}
}

void FvServerConnection::EnterAoI( const ClientInterface::EnterAoIArgs & args )
{
	FV_ASSERT(0);
	//m_akIDAlias[ args.idAlias ] = args.id;

	//if (m_pkHandler)
	//{
	//	//FV_TRACE_MSG( "FvServerConnection::EnterAoI: Entity = %d\n", args.id );
	//	m_pkHandler->OnEntityEnter( args.id, m_iSpaceID, 0 );
	//}
}

void FvServerConnection::EnterAoIOnVehicle(
	const ClientInterface::EnterAoIOnVehicleArgs & args )
{
	FV_ASSERT(0);
	//m_akIDAlias[ args.idAlias ] = args.id;
	//this->SetVehicle( args.id, args.vehicleID );

	//if (m_pkHandler)
	//{
	//	//FV_TRACE_MSG( "FvServerConnection::EnterAoI: Entity = %d\n", args.id );
	//	m_pkHandler->OnEntityEnter( args.id, m_iSpaceID, args.vehicleID );
	//}
}

void FvServerConnection::LeaveAoI( FvBinaryIStream & stream, int /*length*/ )
{
	FvEntityID id;
	stream >> id;

	if (m_pkHandler)
	{
		//FvCacheStamps stamps( stream.RemainingLength() / sizeof(FvEventNumber) );
		//FvCacheStamps::iterator iter = stamps.begin();
		//while (iter != stamps.end())
		//{
		//	stream >> (*iter);
		//	iter++;
		//}
		//m_pkHandler->OnEntityLeave( id, stamps );
		m_pkHandler->OnEntityLeave( id );
	}
	m_kPassengerToVehicle.erase( id );
	//m_kControlledEntities.erase( id );
}

void FvServerConnection::SetVehicle( const ClientInterface::SetVehicleArgs & args )
{
	SetVehicle(args.passengerID, args.vehicleID);
}

void FvServerConnection::CreateEntity( FvBinaryIStream & stream, int /*length*/ )
{
	FvEntityID id;
	stream >> id;

	FV_ASSERT_DEV( id != FvEntityID( -1 ) )	

	FvEntityTypeID type;
	stream >> type;

	FvVector3 pos( 0.f, 0.f, 0.f );
	FvDirection3 dir;
	stream >> pos >> dir;
	//FvInt8 compressedYaw = 0;
	//FvInt8 compressedPitch = 0;
	//FvInt8 compressedRoll = 0;
	//stream >> pos >> compressedYaw >> compressedPitch >> compressedRoll;

	//float yaw = Int8ToAngle( compressedYaw );
	//float pitch = Int8ToAngle( compressedPitch );
	//float roll = Int8ToAngle( compressedRoll );

	//FvEntityID vehicleID = this->GetVehicleID( id );
	FvEntityID vehicleID(0);
	stream >> vehicleID;

	SetVehicle(id, vehicleID);

	if (m_pkHandler)
	{
		m_pkHandler->OnEntityCreate( id, type,
			m_iSpaceID, vehicleID, pos, dir.Yaw(), dir.Pitch(), dir.Roll(),
			stream );
	}

	//this->DetailedPositionReceived( id, m_iSpaceID, vehicleID, pos );
}

void FvServerConnection::UpdateEntity(
	FvBinaryIStream & stream, int /*length*/ )
{
	if (m_pkHandler)
	{
		FvEntityID id;
		stream >> id;
		m_pkHandler->OnEntityProperties( id, stream );
	}
}

void FvServerConnection::LoggedOff( const ClientInterface::LoggedOffArgs & args )
{
	FV_INFO_MSG( "FvServerConnection::LoggedOff: "
		"The server has disconnected us. reason = %d\n", args.reason );
	this->Disconnect( /*informServer:*/ false );
}

void FvServerConnection::RPCCallBack( FvBinaryIStream & stream, int length )
{
	if (m_pkHandler)
	{
		FvUInt8 id;
		stream >> id;
		m_pkHandler->OnRPCCallBack(id, stream);
	}
}

void FvServerConnection::RPCCallBackException( const ClientInterface::RPCCallBackExceptionArgs & args )
{
	if (m_pkHandler)
	{
		m_pkHandler->OnRPCCallBackException(args.id);
	}
}

void FvServerConnection::RPCCallBackException(FvUInt8 uiCBID)
{
	if (m_pkHandler)
	{
		m_pkHandler->OnRPCCallBackException(uiCBID);
	}
}

#define DEFINE_INTERFACE_HERE
#include <FvLoginInterface.h>

#define DEFINE_INTERFACE_HERE
#include <FvBaseAppExtInterface.h>

#define DEFINE_SERVER_HERE
#include <FvClientInterface.h>
