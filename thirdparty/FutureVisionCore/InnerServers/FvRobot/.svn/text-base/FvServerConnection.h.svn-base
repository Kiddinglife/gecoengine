//{future header message}
#ifndef __FvServerConnection_H__
#define __FvServerConnection_H__

#include "FvRobotDefines.h"
#include <FvPowerDefines.h>
#include <FvMD5.h>
#include <FvSMA.h>
#include <FvVector3.h>

#include <FvMemoryStream.h>
#include <FvNetChannel.h>
#include <FvNetEndpoint.h>
#include <FvNetNub.h>
#include <FvNetEncryptionFilter.h>
#include <FvNetPublicKeyCipher.h>
#include <FvClientInterface.h>

#include <FvLoginInterface.h>

#include <set>
#include <list>


class FvServerConnection;

class FvServerMessageHandler
{
public:
	virtual void OnBasePlayerCreate( FvEntityID id, FvEntityTypeID type,
		FvBinaryIStream & data ) = 0;

	virtual void OnCellPlayerCreate( FvEntityID id,
		FvSpaceID spaceID, FvEntityID vehicleID, const FvPosition3D & pos,
		float yaw, float pitch, float roll, FvBinaryIStream & data ) = 0;

	virtual void OnEntityControl( FvEntityID id, bool control ) { }

	virtual void OnEntityEnter( FvEntityID id, FvSpaceID spaceID,
		FvEntityID vehicleID ) = 0;

	virtual void OnEntityLeave( FvEntityID id, const FvCacheStamps & stamps=FvCacheStamps() ) = 0;

	virtual void OnEntityCreate( FvEntityID id, FvEntityTypeID type,
		FvSpaceID spaceID, FvEntityID vehicleID, const FvPosition3D & pos,
		float yaw, float pitch, float roll, FvBinaryIStream & data ) = 0;

	virtual void OnEntityProperties( FvEntityID id, FvBinaryIStream & data ) = 0;

	virtual void OnEntityProperty( FvEntityID objectID, int messageID,
		FvBinaryIStream & data ) = 0;

	virtual void OnEntityMethod( FvEntityID objectID, int messageID,
		FvBinaryIStream & data ) = 0;

	virtual void OnEntityMove( FvEntityID id, FvSpaceID spaceID, FvEntityID vehicleID,
		const FvPosition3D & pos, float yaw, float pitch, float roll,
		bool isVolatile ) {}

	virtual void OnEntityMoveWithError( FvEntityID id,
		FvSpaceID spaceID, FvEntityID vehicleID,
		const FvPosition3D & pos, const FvVector3 & posError,
		float yaw, float pitch, float roll,
		bool isVolatile )
	{
		this->OnEntityMove( id, spaceID, vehicleID, pos, yaw, pitch, roll,
			isVolatile );
	}

	virtual void SpaceData( FvSpaceID spaceID, FvSpaceEntryID& entryID,
		FvUInt16 key, const FvString & data ) = 0;

	virtual void SpaceGone( FvSpaceID spaceID ) = 0;

	virtual void OnVoiceData( const FvNetAddress & srcAddr,
		FvBinaryIStream & data ) {}

	virtual void OnStreamComplete( FvUInt16 id, const FvString &desc,
		FvBinaryIStream & data ) {}

	virtual void OnEntitiesReset( bool keepPlayerOnBase ) {}

	virtual void OnRestoreClient( FvEntityID id,
		FvSpaceID spaceID, FvEntityID vehicleID,
		const FvPosition3D & pos, const FvDirection3 & dir,
		FvBinaryIStream & data ) {}

	virtual void OnEnableEntitiesRejected() {}

	virtual void OnRPCCallBack(FvUInt8 uiCBID, FvBinaryIStream& kIS) {}

	virtual void OnRPCCallBackException(FvUInt8 uiCBID) {}
};

class FvLoginHandler;
typedef FvSmartPointer< FvLoginHandler > FvLoginHandlerPtr;

class FvRetryingRequest :
	public FvNetReplyMessageHandler,
	public FvNetTimerExpiryHandler,
	public FvSafeReferenceCount
{
public:
	static const int DEFAULT_RETRY_PERIOD = 1000000;

	static const int DEFAULT_TIMEOUT_PERIOD = 8000000;

	static const int DEFAULT_MAX_ATTEMPTS = 10;

	FvRetryingRequest( FvLoginHandler & parent,
		const FvNetAddress & addr,
		const FvNetInterfaceElement & ie,
		int retryPeriod = DEFAULT_RETRY_PERIOD,
		int timeoutPeriod = DEFAULT_TIMEOUT_PERIOD,
		int maxAttempts = DEFAULT_MAX_ATTEMPTS,
		bool useParentNub = true );

	virtual ~FvRetryingRequest();


	virtual void HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg );

	virtual void HandleException( const FvNetNubException & exc,
		void * arg );

	virtual int HandleTimeout( FvNetTimerID id,
		void * arg );

	virtual void AddRequestArgs( FvNetBundle & bundle ) = 0;

	virtual void OnSuccess( FvBinaryIStream & data ) = 0;

	virtual void OnFailure( FvNetReason reason ) {}

	virtual void Cancel();

protected:
	void SetNub( FvNetNub * pNub );
	void Send();

protected:
	FvSmartPointer< FvLoginHandler > m_spParent;
	FvNetNub *m_pkNub;
	const FvNetAddress m_kAddr;
	const FvNetInterfaceElement &m_kInterfaceElement;
	FvNetTimerID m_kTimerID;
	bool m_bDone;

private:
	int m_iRetryPeriod;
	int m_iTimeoutPeriod;

	int m_iNumAttempts;

	int m_iNumOutstandingAttempts;

	int m_iMaxAttempts;
};

typedef FvSmartPointer< FvRetryingRequest > FvRetryingRequestPtr;

class FvLoginRequest : public FvRetryingRequest
{
public:
	FvLoginRequest( FvLoginHandler & parent );

	virtual void AddRequestArgs( FvNetBundle & bundle );
	virtual void OnSuccess( FvBinaryIStream & data );
	virtual void OnFailure( FvNetReason reason );
};

class FvBaseAppLoginRequest : public FvRetryingRequest
{
public:
	FvBaseAppLoginRequest( FvLoginHandler & parent );
	virtual ~FvBaseAppLoginRequest();

	virtual int HandleTimeout( FvNetTimerID id, void * arg );
	virtual void AddRequestArgs( FvNetBundle & bundle );
	virtual void OnSuccess( FvBinaryIStream & data );

	FvNetNub &GetNub() { return *m_pkNub; }
	FvNetChannel &GetChannel() { return *m_pkChannel; }
	virtual void Cancel();

private:
	FvNetChannel *m_pkChannel;

	int m_iAttempt;
};

typedef FvSmartPointer< FvBaseAppLoginRequest > FvBaseAppLoginRequestPtr;

class FvLoginHandler : public FvSafeReferenceCount
{
public:
	FvLoginHandler( FvServerConnection * pServerConnection, 
		FvLogOnStatus loginNotSent = FvLogOnStatus::NOT_SET );
	~FvLoginHandler();

	void Start( const FvNetAddress & loginAppAddr, LogOnParamsPtr pParams );
	void Finish();

	void SendLoginAppLogin();
	void OnLoginReply( FvBinaryIStream & data );

	void SendBaseAppLogin();
	void OnBaseAppReply( FvBaseAppLoginRequestPtr pHandler,
		FvSessionKey sessionKey );

	void OnFailure( FvNetReason reason );

	const FvLoginReplyRecord &ReplyRecord() const { return m_kReplyRecord; }

	bool Done() const						{ return m_bDone; }
	int Status() const						{ return m_uiStatus; }
	LogOnParamsPtr pParams()				{ return m_spParams; }

	const FvString &ErrorMsg() const	{ return m_kErrorMsg; }

	void SetError( int status, const FvString & errorMsg )
	{
		m_uiStatus = status;
		m_kErrorMsg = errorMsg;
	}

	FvServerConnection *pServerConnection() const { return m_pkServerConnection; }
	FvServerConnection &ServConn() const { return *m_pkServerConnection; }

	const FvNetAddress &LoginAddr() const { return m_kLoginAppAddr; }
	const FvNetAddress &BaseAppAddr() const { return m_kBaseAppAddr; }

	void AddChildRequest( FvRetryingRequestPtr pRequest );
	void RemoveChildRequest( FvRetryingRequestPtr pRequest );
	void AddCondemnedNub( FvNetNub * pNub );

	int NumBaseAppLoginAttempts() const { return m_iNumBaseAppLoginAttempts; }

private:
	FvNetAddress		m_kLoginAppAddr;
	FvNetAddress		m_kBaseAppAddr;

	LogOnParamsPtr		m_spParams;

	FvServerConnection*	m_pkServerConnection;
	FvLoginReplyRecord	m_kReplyRecord;
	bool				m_bDone;
	FvUInt8				m_uiStatus;

	FvString			m_kErrorMsg;
	int					m_iNumBaseAppLoginAttempts;

	typedef std::set< FvRetryingRequestPtr > ChildRequests;
	ChildRequests		m_kChildRequests;
	typedef std::vector< FvNetNub* > CondemnedNubs;
	CondemnedNubs		m_kCondemnedNubs;
};

class FV_ROBOT_API FvServerConnection : public FvNetBundlePrimer
{
public:
	FvServerConnection();
	~FvServerConnection();

	bool ProcessInput();
	void RegisterInterfaces( FvNetNub & nub );

	void SetInactivityTimeout( float seconds );

	FvLogOnStatus LogOn( FvServerMessageHandler * pHandler,
		const char * serverName,
		const char * username,
		const char * password,
		const char * publicKeyPath = NULL,
		FvUInt16 port = 0 );

	FvLoginHandlerPtr LogOnBegin(
		const char * serverName,
		const char * username,
		const char * password,
		const char * publicKeyPath = NULL,
		FvUInt16 port = 0 );

	FvLogOnStatus LogOnComplete(
		FvLoginHandlerPtr pLRH,
		FvServerMessageHandler * pHandler );

	void EnableReconfigurePorts() { m_bTryToReconfigurePorts = true; }
	void EnableEntities();

	bool Online() const;
	bool Offline() const				{ return !this->Online(); }
	void Disconnect( bool informServer = true );

	void Channel( FvNetChannel & channel ) { m_pkChannel = &channel; }

	FvNetChannel &Channel();
	FvNetBundle &GetBundle() { return this->Channel().Bundle(); }
	const FvNetAddress &GetAddr() const;

	FvNetEncryptionFilterPtr pFilter() { return m_spFilter; }

	void AddMove( FvEntityID id, FvSpaceID spaceID, FvEntityID vehicleID,
		const FvVector3 & pos, float yaw, float pitch, float roll,
		bool onGround, const FvVector3 & globalPos );

	FvBinaryOStream* StartProxyMessage( int messageId );
	FvBinaryOStream* StartAvatarMessage( int messageId );
	FvBinaryOStream* StartEntityMessage( int messageId, FvEntityID entityId );
	void StartGMMessage(bool bToBase, FvEntityID iEntityID, FvUInt16 uiMethodID, FvBinaryIStream& kIS);

	void RequestEntityUpdate( FvEntityID id,
		const FvCacheStamps & stamps = FvCacheStamps() );

	const FvString &ErrorMsg() const	{ return m_kErrorMsg; }

	FvEntityID ConnectedID() const			{ return m_iConnectedID; }
	void SessionKey( FvSessionKey key ) { m_uiSessionKey = key; }

	float Latency() const;

	FvUInt32 PacketsIn() const;
	FvUInt32 PacketsOut() const;

	FvUInt32 BitsIn() const;
	FvUInt32 BitsOut() const;

	FvUInt32 MessagesIn() const;
	FvUInt32 MessagesOut() const;

	double BPSIn() const;
	double BPSOut() const;

	double PacketsPerSecondIn() const;
	double PacketsPerSecondOut() const;

	double MessagesPerSecondIn() const;
	double MessagesPerSecondOut() const;

	int	BandwidthFromServer() const;
	void BandwidthFromServer( int bandwidth );

	double MovementBytesPercent() const;
	double NonMovementBytesPercent() const;
	double OverheadBytesPercent() const;

	int MovementBytesTotal() const;
	int NonMovementBytesTotal() const;
	int OverheadBytesTotal() const;

	int MovementMessageCount() const;

	void pTime( const double * pTime );

	const double *pTime()					{ return m_pkTime; }

	double ServerTime( double gameTime ) const;
	double ServerTime() const;
	double LastMessageTime() const;
	FvTimeStamp	LastGameTime() const;
	double SmoothServerTimeInDay() const;

	double LastSendTime() const	{ return m_fLastSendTime; }
	double MinSendInterval() const	{ return m_fMinSendInterval; }

	//! external interface
	void Authenticate( const ClientInterface::AuthenticateArgs & args );
	void BandwidthNotification( const ClientInterface::BandwidthNotificationArgs & args );
	void UpdateFrequencyNotification( const ClientInterface::UpdateFrequencyNotificationArgs & args );
	void TickSync( const ClientInterface::TickSyncArgs & args );
	void SetGameTime( const ClientInterface::SetGameTimeArgs & args );
	void ResetEntities( const ClientInterface::ResetEntitiesArgs & args );
	void CreateBasePlayer( FvBinaryIStream & stream, int length );
	void CreateCellPlayer( FvBinaryIStream & stream, int length );
	void SpaceData( FvBinaryIStream & stream, int length );
	void EnterAoI( const ClientInterface::EnterAoIArgs & args );
	void EnterAoIOnVehicle(	const ClientInterface::EnterAoIOnVehicleArgs & args );
	void LeaveAoI( FvBinaryIStream & stream, int length );
	void SetVehicle( const ClientInterface::SetVehicleArgs & args );
	void CreateEntity( FvBinaryIStream & stream, int length );
	void UpdateEntity( FvBinaryIStream & stream, int length );
	void DetailedPosition( const ClientInterface::DetailedPositionArgs & args );
	void ForcedPosition( const ClientInterface::ForcedPositionArgs & args );
	void ControlEntity( const ClientInterface::ControlEntityArgs & args );
	void LoggedOff( const ClientInterface::LoggedOffArgs & args );
	void RPCCallBack( FvBinaryIStream & stream, int length );
	void RPCCallBackException( const ClientInterface::RPCCallBackExceptionArgs & args );
	//! 

	void HandleEntityMessage( int messageID, FvBinaryIStream & data, int length );

	void SetMessageHandler( FvServerMessageHandler * pHandler )
								{ if (m_pkHandler != NULL) m_pkHandler = pHandler; }

	FvNetNub &GetNub() 						{ return m_kNub; }

	static const float &UpdateFrequency()		{ return ms_fUpdateFrequency; }

	void InitDebugInfo();

	void Digest( const FvMD5::Digest & digest )	{ m_kDigest = digest; }
	const FvMD5::Digest Digest() const			{ return m_kDigest; }

	void Send();

	void RPCCallBackException(FvUInt8 uiCBID);

private:
	FvServerConnection(const FvServerConnection&);
	FvServerConnection& operator=(const FvServerConnection&);
	double AppTime() const;
	void UpdateStats();

	void SetVehicle( FvEntityID passengerID, FvEntityID vehicleID );
	FvEntityID GetVehicleID( FvEntityID passengerID ) const
	{
		PassengerToVehicleMap::const_iterator iter =
			m_kPassengerToVehicle.find( passengerID );

		return iter == m_kPassengerToVehicle.end() ? 0 : iter->second;
	}

	void InitialiseConnectionState();

	virtual void PrimeBundle( FvNetBundle & bundle );
	virtual int NumUnreliableMessages() const;

	FvUInt32		m_uiSessionKey;
	FvString		m_kUsername;

	FvServerMessageHandler*	m_pkHandler;

	FvEntityID		m_iConnectedID;
	FvSpaceID		m_iSpaceID;
	int				m_iBandwidthFromServer;

	const double*	m_pkTime;
	double			m_fLastTime;
	double			m_fLastSendTime;
	double			m_fMinSendInterval;

	FvNetNub		m_kNub;
	FvNetChannel*	m_pkChannel;

	bool			m_bEverReceivedPacket;
	bool			m_bTryToReconfigurePorts;
	bool			m_bEntitiesEnabled;

	float			m_fInactivityTimeout;
	FvMD5::Digest	m_kDigest;

	class ServerTimeHandler
	{
	public:
		ServerTimeHandler();

		void		TickSync( FvUInt8 newSeqNum, double currentTime );
		void		GameTime( FvTimeStamp gameTime, double currentTime );

		double		ServerTime( double gameTime ) const;
		double		LastMessageTime() const;
		FvTimeStamp	LastGameTime() const;

		double		SmoothServerTimeInDay() const;

	private:
		static const double UNINITIALISED_TIME;

		FvUInt8		m_uiTickByte;
		double		m_fTimeAtSequenceStart;
		FvTimeStamp	m_uiGameTimeAtSequenceStart;

		FvTimeStamp	m_uiCliTimeStartForSmoothSvrTime;
		double		m_fSvrTimeStartForSmoothSvrTime;
		double		m_fTimeScalingForSmoothSvrTime;
	};
	ServerTimeHandler	m_kServerTimeHandler;

	static float	ms_fUpdateFrequency;

	FvString		m_kErrorMsg;

	FvUInt8			m_uiSendingSequenceNumber;

	FvEntityID		m_akIDAlias[ 256 ];

	typedef std::map< FvEntityID, FvEntityID > PassengerToVehicleMap;
	PassengerToVehicleMap m_kPassengerToVehicle;

	FvVector3		m_akSentPositions[ 256 ];
	FvVector3		m_kReferencePosition;

	typedef std::set< FvEntityID > ControlledEntities;
	ControlledEntities	m_kControlledEntities;

	bool IsControlledLocally( FvEntityID id ) const
	{
		return m_kControlledEntities.find( id ) != m_kControlledEntities.end();
	}

	void DetailedPositionReceived( FvEntityID id, FvSpaceID spaceID, FvEntityID vehicleID, const FvVector3 & position );

	FvMemoryOStream	m_kCreateCellPlayerMsg;

	FvNetEncryptionFilterPtr m_spFilter;

	class Statistic
	{
	public:
		Statistic()
			:m_uiValue( 0 )
			,m_uiOldValue( 0 )
			,m_fChangePerSecond( 0 ){}

		void Update( double deltaTime )
		{
			m_fChangePerSecond = (m_uiValue - m_uiOldValue)/deltaTime;
			m_uiOldValue = m_uiValue;
		}

		operator FvUInt32() const 			{ return m_uiValue; }
		void operator =( FvUInt32 value )	{ m_uiValue = value; }

		void operator +=( FvUInt32 value )	{ m_uiValue += value; }

		double ChangePerSecond() const		{ return m_fChangePerSecond; }

	private:
		FvUInt32	m_uiValue;
		FvUInt32	m_uiOldValue;
		double		m_fChangePerSecond;
	};

	Statistic m_kPacketsIn;
	Statistic m_kPacketsOut;

	Statistic m_kBitsIn;
	Statistic m_kBitsOut;

	Statistic m_kMessagesIn;
	Statistic m_kMessagesOut;

	Statistic m_kTotalBytes;
	Statistic m_kMovementBytes;
	Statistic m_kNonMovementBytes;
	Statistic m_kOverheadBytes;
};

#include "FvServerConnection.inl"

#endif // __FvServerConnection_H__
