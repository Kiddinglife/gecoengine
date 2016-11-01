//{future header message}
#ifndef __FvNetMachineGuard_H__
#define __FvNetMachineGuard_H__

#include "FvNetwork.h"
#include "FvNetMisc.h"
#include "FvNetEndpoint.h"

#include <FvSmartPointer.h>
#include <FvBinaryStream.h>

#if !defined( PLAYSTATION3 )
#include <signal.h>
#endif

#if !defined( _WIN32 ) && !defined( PLAYSTATION3 )
# include <sys/types.h>
# include <pwd.h>
#else
#define SIGINT 2
#define SIGQUIT 3
#define SIGUSR1 10
#endif


class FvNetMachineGuardMessage;
class FvMemoryIStream;
class FvMemoryOStream;

class FV_NETWORK_API FvNetMGMPacket
{
public:
	static const int MAX_SIZE = 32768;

	enum Flags {
		PACKET_STAGGER_REPLIES = 0x1
	};

	typedef std::vector< FvNetMachineGuardMessage* > MGMs;

	FvUInt8	m_uiFlags;
	FvUInt32 m_uiBuddy;
	MGMs m_kMessages;

protected:

	std::vector< bool > m_kDelInfo;

	bool m_bDontDeleteMessages;

public:
	FvNetMGMPacket() : m_uiFlags( 0 ), m_uiBuddy( 0 ), m_bDontDeleteMessages( false ) {}
	FvNetMGMPacket( FvMemoryIStream &is ) :
		m_uiFlags( 0 ), m_uiBuddy( 0 ), m_bDontDeleteMessages( false )
	{
		this->Read( is );
	}

	~FvNetMGMPacket();

	void Read( FvMemoryIStream &is );
	bool Write( FvMemoryOStream &os ) const;

	void Append( FvNetMachineGuardMessage &mgm, bool shouldDelete=false );
	inline void StealMessages() { m_bDontDeleteMessages = true; }

	static void SetBuddy( FvUInt32 addr );

	static FvUInt32 ms_uiBuddy;
};

class FvNetHighPrecisionMachineMessage;
class FvNetWholeMachineMessage;
class FvNetProcessMessage;
class FvNetProcessStatsMessage;
class FvNetListenerMessage;
class FvNetCreateMessage;
class FvNetSignalMessage;
class FvNetTagsMessage;
class FvNetUserMessage;
class FvNetPidMessage;
class FvNetResetMessage;
class FvNetErrorMessage;
class FvNetMachinedAnnounceMessage;
class FvNetQueryInterfaceMessage;
class FvNetCreateWithArgsMessage;

class FV_NETWORK_API FvNetMachineGuardMessage
{
public:
	enum Message
	{
		WHOLE_MACHINE_MESSAGE = 1,
		PROCESS_MESSAGE = 2,
		PROCESS_STATS_MESSAGE = 3,
		LISTENER_MESSAGE = 4,
		CREATE_MESSAGE = 5,
		SIGNAL_MESSAGE = 6,
		TAGS_MESSAGE = 7,
		USER_MESSAGE = 8,
		PID_MESSAGE = 9,
		RESET_MESSAGE = 10,
		ERROR_MESSAGE = 11,
		QUERY_INTERFACE_MESSAGE = 12,
		CREATE_WITH_ARGS_MESSAGE = 13,
		HIGH_PRECISION_MACHINE_MESSAGE = 14,

		MACHINED_ANNOUNCE_MESSAGE = 64,
	};

	enum Flags
	{
		MESSAGE_DIRECTION_OUTGOING = 0x1,
		MESSAGE_NOT_UNDERSTOOD = 0x2
	};

	FvUInt8	m_uiMessage;
	FvUInt8 m_uiFlags;

private:
	FvUInt16 m_uiSeq;

public:
	FvNetMachineGuardMessage() {};
	FvNetMachineGuardMessage( FvUInt8 message, FvUInt8 flags = 0, FvUInt16 seq = 0 );

	virtual ~FvNetMachineGuardMessage() {}

	virtual void Read( FvBinaryIStream &is );
	virtual void Write( FvBinaryOStream &os );
	virtual const char *c_str() const;

	static FvNetMachineGuardMessage *Create( FvBinaryIStream &is );

	static FvNetMachineGuardMessage *Create( void *buf, int length );

	const char *TypeStr() const;

	bool SendTo( FvNetEndpoint &ep, FvUInt16 port, FvUInt32 addr = FV_NET_BROADCAST,
		FvUInt8 packFlags = 0 );

	class ReplyHandler
	{
	public:
		virtual ~ReplyHandler() {};

		bool handle( FvNetMachineGuardMessage &mgm, FvUInt32 addr );

		virtual bool OnUnhandledMsg(
			FvNetMachineGuardMessage &mgm, FvUInt32 addr );

		virtual bool OnHighPrecisionMachineMessage(
			FvNetHighPrecisionMachineMessage &hpmm, FvUInt32 addr );
		virtual bool OnWholeMachineMessage(
			FvNetWholeMachineMessage &wmm, FvUInt32 addr );
		virtual bool OnProcessMessage(
			FvNetProcessMessage &pm, FvUInt32 addr );
		virtual bool OnProcessStatsMessage(
			FvNetProcessStatsMessage &psm, FvUInt32 addr );
		virtual bool OnListenerMessage(
			FvNetListenerMessage &lm, FvUInt32 addr );
		virtual bool OnCreateMessage(
			FvNetCreateMessage &cm, FvUInt32 addr );
		virtual bool OnSignalMessage(
			FvNetSignalMessage &sm, FvUInt32 addr );
		virtual bool OnTagsMessage(
			FvNetTagsMessage &tm, FvUInt32 addr );
		virtual bool OnUserMessage(
			FvNetUserMessage &um, FvUInt32 addr );
		virtual bool OnPidMessage(
			FvNetPidMessage &pm, FvUInt32 addr );
		virtual bool OnResetMessage(
			FvNetResetMessage &rm, FvUInt32 addr );
		virtual bool OnErrorMessage(
			FvNetErrorMessage &em, FvUInt32 addr );
		virtual bool OnMachinedAnnounceMessage(
			FvNetMachinedAnnounceMessage &mam, FvUInt32 addr );
		virtual bool OnQueryInterfaceMessage(
			FvNetQueryInterfaceMessage &wmm, FvUInt32 addr );
		virtual bool OnCreateWithArgsMessage(
			FvNetCreateWithArgsMessage &cwam, FvUInt32 addr );
	};

	FvNetReason SendAndRecv( FvNetEndpoint &ep, FvUInt32 destaddr, FvUInt16 uiMachinePort,
		ReplyHandler *pHandler = NULL );

	FvNetReason SendAndRecv( FvUInt32 srcip, FvUInt32 destaddr, FvUInt16 uiMachinePort,
		ReplyHandler *pHandler = NULL );

	FvNetReason SendAndRecvFromEndpointAddr( FvNetEndpoint & ep, FvUInt32 destaddr, FvUInt16 uiMachinePort,
		ReplyHandler * pHandler = NULL );

	inline void CopySeq( const FvNetMachineGuardMessage &mgm )
	{
		m_uiSeq = mgm.m_uiSeq;
		m_bSeqSent = false;
	}

	FV_INLINE FvUInt16 seq() const { return m_uiSeq; }

	void RefreshSeq();

	inline void SetFlag( FvUInt8 flag, bool status )
	{
		if (status)
			m_uiFlags |= flag;
		else
			m_uiFlags &= ~flag;
	}

	inline bool GetFlag( FvUInt8 flag ) const
	{
		return (m_uiFlags & flag) ? true : false;
	}

	inline void OutGoing( bool b ) {
		this->SetFlag( MESSAGE_DIRECTION_OUTGOING, b ); }
	inline bool OutGoing() const {
		return this->GetFlag( MESSAGE_DIRECTION_OUTGOING ); }

protected:
	static char ms_acBuf[ 1024 ];

	static FvUInt16 ms_uiSeqTicker;

	bool m_bSeqSent;
};

struct FvNetLowPrecisionInterfaceStats
{
public:
	FvNetLowPrecisionInterfaceStats() :
		m_uiBitsIn( 0 ),
		m_uiBitsOut( 0 ),
		m_uiPackIn( 0 ),
		m_uiPackOut( 0 ) {}

	FvString m_kName;
	FvUInt8 m_uiBitsIn;
	FvUInt8 m_uiBitsOut;
	FvUInt8 m_uiPackIn;
	FvUInt8 m_uiPackOut;
};

struct FvNetHighPrecisionInterfaceStats
{
public:
	FvNetHighPrecisionInterfaceStats() :
		m_uiBitsIn( 0 ),
		m_uiBitsOut( 0 ),
		m_uiPackIn( 0 ),
		m_uiPackOut( 0 ) {}

	FvString m_kName;
	FvUInt32 m_uiBitsIn;
	FvUInt32 m_uiBitsOut;
	FvUInt32 m_uiPackIn;
	FvUInt32 m_uiPackOut;
};

class FV_NETWORK_API FvNetQueryInterfaceMessage: public FvNetMachineGuardMessage
{
public:
	FvNetQueryInterfaceMessage();
	virtual ~FvNetQueryInterfaceMessage();

	virtual void Write( FvBinaryOStream &os );
	virtual void Read( FvBinaryIStream &is );
	virtual const char *c_str() const;

	enum {
		INTERNAL = 0x0
	};

	FvUInt32 m_uiAddress;

};

class FV_NETWORK_API FvNetHighPrecisionMachineMessage : public FvNetMachineGuardMessage
{
public:
	FvNetHighPrecisionMachineMessage();
	virtual ~FvNetHighPrecisionMachineMessage();

	virtual void Write( FvBinaryOStream &os );
	virtual void Read( FvBinaryIStream &is );

	virtual const char *c_str() const;

	inline void SetNCpus( int n )
	{
		m_uiNCPUs = n;
		m_kCPULoads.clear();
		m_kCPULoads.resize( n );
	}

	inline void SetNInterfaces( int n )
	{
		m_uiNInterfaces = n;
		m_kIFStats.clear();
		m_kIFStats.resize( n );
	}


	FvString m_kHostname;
	FvUInt16 m_uiCPUSpeed;
	FvUInt8 m_uiNCPUs;
	std::vector< FvUInt8 > m_kCPULoads;

	FvUInt8 m_uiIOWaitTime;

	FvUInt8 m_uiMem;
	FvUInt8 m_uiNInterfaces;

	std::vector< struct FvNetHighPrecisionInterfaceStats >
				m_kIFStats;

	FvUInt8 m_uiInDiscards, m_uiOutDiscards;
	FvUInt8 m_uiVersion;
};

class FV_NETWORK_API FvNetWholeMachineMessage : public FvNetMachineGuardMessage
{
public:
	FvNetWholeMachineMessage();
	virtual ~FvNetWholeMachineMessage();

	virtual void Write( FvBinaryOStream &os );
	virtual void Read( FvBinaryIStream &is );
	virtual const char *c_str() const;

	inline void SetNCpus( int n )
	{
		m_uiNCPUs = n;
		m_kCPULoads.clear();
		m_kCPULoads.resize( n );
	}

	inline void SetNInterfaces( int n )
	{
		m_uiNInterfaces = n;
		m_kIFStats.clear();
		m_kIFStats.resize( n );
	}

public:
	FvString m_kHostname;
	FvUInt16 m_uiCPUSpeed;
	FvUInt8 m_uiNCPUs;
	std::vector<FvUInt8> m_kCPULoads;

	FvUInt8 m_uiMem;
	FvUInt8 m_uiNInterfaces;

	std::vector< struct FvNetLowPrecisionInterfaceStats >
				m_kIFStats;

	FvUInt8	m_uiInDiscards, m_uiOutDiscards;
	FvUInt8	m_uiVersion;
};

class FvNetSignalMessage;

class FV_NETWORK_API FvNetProcessMessage : public FvNetMachineGuardMessage
{
public:
	enum Type {
		FIND = 0, 
		REGISTER = 1,
		DEREGISTER = 2,
		NOTIFY_BIRTH = 3,
		NOTIFY_DEATH = 4
	};

	enum Param
	{
		PARAM_USE_CATEGORY = 0x1,
		PARAM_USE_UID = 0x2,
		PARAM_USE_PID = 0x4,
		PARAM_USE_PORT = 0x8,
		PARAM_USE_ID = 0x10,
		PARAM_USE_NAME = 0x20,
		PARAM_IS_MSGTYPE = 0x80
	};

	enum Category
	{
		SERVER_COMPONENT = 0,
		WATCHER_NUB = 1
	};

	FvUInt8 m_uiParam;
	FvUInt8 m_uiCategory;
	FvUInt16 m_uiUID;
	FvUInt32 m_uiPID;
	FvUInt16 m_uiPort;
	FvUInt16 m_uiID;	
	FvString m_kName;

	FvNetProcessMessage() :
		FvNetMachineGuardMessage( FvNetMachineGuardMessage::PROCESS_MESSAGE ) {}

	virtual ~FvNetProcessMessage() {}

	virtual void Write( FvBinaryOStream &os );
	virtual void Read( FvBinaryIStream &is );
	virtual const char *c_str() const;

	bool matches( const FvNetProcessMessage &query ) const;

	static const char *categoryToString( FvUInt8 category );

	FvNetProcessMessage & operator<<( const FvNetProcessMessage &pm )
	{
		m_uiParam = pm.m_uiParam;
		m_uiCategory = pm.m_uiCategory;
		m_uiUID = pm.m_uiUID;
		m_uiPID = pm.m_uiPID;
		m_uiPort = pm.m_uiPort;
		m_uiID = pm.m_uiID;
		m_kName = pm.m_kName;
		return *this;
	}
};

class FV_NETWORK_API FvNetProcessStatsMessage : public FvNetProcessMessage
{
public:
	FvUInt8	m_uiCPU;
	FvUInt8 m_uiMem;

	FvNetProcessStatsMessage() : FvNetProcessMessage()
	{
		m_uiMessage = FvNetMachineGuardMessage::PROCESS_STATS_MESSAGE;
	}

	virtual ~FvNetProcessStatsMessage() {}

	virtual void Write( FvBinaryOStream &os );
	virtual void Read( FvBinaryIStream &is );
	virtual const char *c_str() const;
};

class FV_NETWORK_API FvNetListenerMessage : public FvNetProcessMessage
{
public:
	enum Type
	{
		ADD_BIRTH_LISTENER = 0,
		ADD_DEATH_LISTENER = 1
	};

	static const FvUInt16 ANY_UID = 0xffff;

	FvString m_kPreAddr;
	FvString m_kPostAddr;

	FvNetListenerMessage()
	{
		m_uiMessage = FvNetMachineGuardMessage::LISTENER_MESSAGE;
	}

	virtual ~FvNetListenerMessage() {}

	virtual void Write( FvBinaryOStream &os );
	virtual void Read( FvBinaryIStream &is );
	virtual const char *c_str() const;
};

class FV_NETWORK_API FvNetCreateMessage : public FvNetMachineGuardMessage
{
public:
	FvString m_kName;
	FvString m_kConfig;
	FvUInt16 m_uiUID;
	FvUInt8 m_uiRecover;
	FvUInt32 m_uiFWDIP;
	FvUInt16 m_uiFWDPort;

	FvNetCreateMessage( FvNetMachineGuardMessage::Message messageType =
			FvNetMachineGuardMessage::CREATE_MESSAGE )
	{
		m_uiMessage = messageType;
	}

	virtual ~FvNetCreateMessage() {}

	virtual void Write( FvBinaryOStream &os );
	virtual void Read( FvBinaryIStream &is );
	virtual const char *c_str() const;

	const char *c_str_name( const char * className ) const;
};

class FV_NETWORK_API FvNetCreateWithArgsMessage : public FvNetCreateMessage
{
public:
	typedef std::vector< FvString >	Args;

	Args m_kArgs;

	FvNetCreateWithArgsMessage() : FvNetCreateMessage( CREATE_WITH_ARGS_MESSAGE ) {}
	virtual ~FvNetCreateWithArgsMessage() {}

	virtual void Write( FvBinaryOStream &os );
	virtual void Read( FvBinaryIStream &is );
	virtual const char *c_str() const;
};

class FV_NETWORK_API FvNetSignalMessage : public FvNetProcessMessage
{
public:
	FvUInt8 m_uiSignal;

	FvNetSignalMessage()
	{
		m_uiMessage = FvNetMachineGuardMessage::SIGNAL_MESSAGE;
	}

	virtual ~FvNetSignalMessage() {}

	virtual void Write( FvBinaryOStream &os );
	virtual void Read( FvBinaryIStream &is );
	virtual const char *c_str() const;

	void setControlledShutdown() { m_uiSignal = SIGUSR1; }
	void setKill() { m_uiSignal = SIGINT; }
	void setHardKill() { m_uiSignal = SIGQUIT; }
};

typedef std::vector< FvString > Tags;

class FV_NETWORK_API FvNetTagsMessage : public FvNetMachineGuardMessage
{
public:
	Tags m_kTags;
	FvUInt8	m_uiExists;

	FvNetTagsMessage() :
		FvNetMachineGuardMessage( FvNetMachineGuardMessage::TAGS_MESSAGE ) {}

	virtual ~FvNetTagsMessage() {}

	virtual void Write( FvBinaryOStream &os );
	virtual void Read( FvBinaryIStream &is );
	virtual const char *c_str() const;
};

class FV_NETWORK_API FvNetUserMessage : public FvNetMachineGuardMessage
{
public:
	enum Param {
		PARAM_USE_UID = 0x1,
		PARAM_USE_NAME = 0x2,
		PARAM_CHECK_COREDUMPS = 0x4,
		PARAM_REFRESH_ENV = 0x8,
		PARAM_SET = 0x80
	};

	static const int UID_NOT_FOUND = 0xffff;

	struct CoreDump
	{
		FvString m_kFilename;
		FvString m_kAssert;
		FvUInt32 m_uiTime;
	};

	typedef std::vector< CoreDump > CoreDumps;

	FvUInt8 m_uiParam;
	FvUInt16 m_uiUID;
	FvUInt16 m_uiGID;
	FvString m_kUsername;
	FvString m_kFullname;
	FvString m_kHome;
	FvString m_kRoot;
	FvString m_kResPath;
	CoreDumps m_kCoreDumps;

	FvNetUserMessage() :
		FvNetMachineGuardMessage( FvNetMachineGuardMessage::USER_MESSAGE ) {}

	virtual ~FvNetUserMessage() {}

	virtual void Write( FvBinaryOStream &os );
	virtual void Read( FvBinaryIStream &is );
	virtual const char *c_str() const;

#ifndef _WIN32
	void Init( const struct passwd &pwent );
#endif

	bool Matches( const FvNetUserMessage &query ) const;
	const char *GetConfFilename() const;
};

FvBinaryIStream& operator>>( FvBinaryIStream &is, FvNetUserMessage::CoreDump &cd );
FvBinaryOStream& operator<<( FvBinaryOStream &os, const FvNetUserMessage::CoreDump &cd );

class FvNetResetMessage : public FvNetMachineGuardMessage
{
public:
	FvNetResetMessage() :
		FvNetMachineGuardMessage( FvNetMachineGuardMessage::RESET_MESSAGE ) {}
};

class FV_NETWORK_API FvNetErrorMessage : public FvNetMachineGuardMessage
{
public:
	FvNetErrorMessage() :
		FvNetMachineGuardMessage( FvNetMachineGuardMessage::ERROR_MESSAGE ),
		m_uiSeverity( FV_DEBUG_MESSAGE_PRIORITY_ERROR ) {}

	FvUInt8 m_uiSeverity;
	FvString m_kMsg;

	virtual void Write( FvBinaryOStream &os );
	virtual void Read( FvBinaryIStream &is );
	virtual const char *c_str() const;
};

class FV_NETWORK_API FvNetMachinedAnnounceMessage : public FvNetMachineGuardMessage
{
public:
	enum Type
	{
		ANNOUNCE_BIRTH = 0,
		ANNOUNCE_DEATH = 1,
		ANNOUNCE_EXISTS = 2
	};

	FvUInt8 m_uiType;
	union {
		FvUInt32 m_uiCount; 
		FvUInt32 m_uiAddr;
	};

	FvNetMachinedAnnounceMessage() :
		FvNetMachineGuardMessage( FvNetMachineGuardMessage::MACHINED_ANNOUNCE_MESSAGE )
	{}

	virtual ~FvNetMachinedAnnounceMessage() {}

	virtual void Write( FvBinaryOStream &os );
	virtual void Read( FvBinaryIStream &is );
	virtual const char *c_str() const;
};

class FV_NETWORK_API FvNetPidMessage : public FvNetMachineGuardMessage
{
public:
	FvUInt32 m_uiPID;
	FvUInt8	m_uiRunning;

	FvNetPidMessage() :
		FvNetMachineGuardMessage( FvNetMachineGuardMessage::PID_MESSAGE )
	{}

	virtual ~FvNetPidMessage() {}

	virtual void Write( FvBinaryOStream &os );
	virtual void Read( FvBinaryIStream &is );
	virtual const char *c_str() const;
};

class FV_NETWORK_API UnknownMessage : public FvNetMachineGuardMessage
{
public:
	UnknownMessage() : m_pcData( NULL ), m_uiLen( 0 ) {}

	virtual ~UnknownMessage();

	virtual void Write( FvBinaryOStream &os );
	virtual void Read( FvBinaryIStream &is );
	virtual const char *c_str() const;

	char *m_pcData;
	FvUInt16 m_uiLen;
};

#endif // __FvNetMachineGuard_H__
