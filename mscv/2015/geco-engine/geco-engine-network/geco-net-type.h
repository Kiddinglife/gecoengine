//{future header message}
#ifndef __FvNetTypes_H__
#define __FvNetTypes_H__

#include "common/geco-plateform.h"
#include "common/ds/geco-bit-stream.h"
#include "math/geco-math-vector.h"

// #include <FvDirection3.h>

#ifdef _XBOX360
#include "FvNet360.h"
#endif

#include <vector>
#include <limits>

class FvWatcher;

class FvBinaryIStream;
class FvBinaryOStream;

const int FV_NET_UDP_OVERHEAD = 28;

const int FV_NET_BITS_PER_BYTE = 8;

const FvUInt32 FV_NET_LOCALHOST = 0x0100007F;

const FvUInt32 FV_NET_BROADCAST = 0xFFFFFFFF;

static const int FV_NET_WN_PACKET_SIZE = 0x10000;

typedef FvUInt32 FvTimeStamp;

typedef FvInt32 FvEntityID;

const FvUInt8 FV_ENTITY_DEF_INIT_FLG = 0;

const FvEntityID FV_NULL_ENTITY = ((FvEntityID)0);
//! Server (0, 0x20000000)
const FvEntityID FV_MIN_SERVER_ENTITY = ((FvEntityID)0);
const FvEntityID FV_MAX_SERVER_ENTITY = ((FvEntityID)0x20000000);
//! Client (0x1FFFFFFF, 0x40000000)
const FvEntityID FV_MIN_CLIENT_ENTITY = ((FvEntityID)0x1FFFFFFF);
const FvEntityID FV_MAX_CLIENT_ENTITY = ((FvEntityID)0x40000000);
//! Global (0x3FFFFFFF, 0x7FFFFFFF)
const FvEntityID FV_MIN_GLOBAL_ENTITY = ((FvEntityID)0x3FFFFFFF);
const FvEntityID FV_MAX_GLOBAL_ENTITY = ((FvEntityID)0x7FFFFFFF);

typedef FvInt32 FvSpaceID;

const FvSpaceID FV_NULL_SPACE = ((FvSpaceID)0);
//! Server (0, 0x40000000)
const FvSpaceID FV_MIN_SERVER_SPACE = ((FvSpaceID)0);
const FvSpaceID FV_MAX_SERVER_SPACE = ((FvSpaceID)0x40000000);
//! Global (0x3FFFFFFF, 0x7FFFFFFF)
const FvSpaceID FV_MIN_GLOBAL_SPACE = ((FvSpaceID)0x3FFFFFFF);
const FvSpaceID FV_MAX_GLOBAL_SPACE = ((FvSpaceID)0x7FFFFFFF);

typedef FvInt32 FvCellAppID;

typedef FvInt32 FvBaseAppID;

typedef FvUInt32 FvSessionKey;

typedef FvVector3 FvPosition3D;

typedef FvUInt16 FvEntityTypeID;
const FvEntityTypeID FV_INVALID_ENTITY_TYPE_ID = FvEntityTypeID(-1);

typedef FvInt64 FvDatabaseID;

#ifndef _WIN32
#ifndef _LP64
#define FMT_DBID "lld"
#else
#define FMT_DBID "ld"
#endif
#else
#define FMT_DBID "I64d"
#endif

typedef FvUInt32 FvGridID;

typedef FvUInt32 FvEventNumber;
const FvEventNumber FV_INITIAL_EVENT_NUMBER = 1;

typedef std::vector< FvEventNumber > FvCacheStamps;

typedef FvUInt16 FvVolatileNumber;

typedef FvUInt8 FvDetailLevel;

const int FV_MAX_DATA_LOD_LEVELS = 6;

#ifdef _WIN32//! win32
#define	FV_NET_LOCALHOSTNAME "localhost"
#else
#define FV_NET_LOCALHOSTNAME "lo"
#endif

//
//struct FV_NETWORK_API FvDirection3
//{
//	FvDirection3() {};
//	FvDirection3( const FvVector3 & v ) :
//		m_fRoll ( v[0] ),
//		m_fPitch( v[1] ),
//		m_fYaw  ( v[2] ) {}
//
//	FvVector3 AsVector3() const { return FvVector3( m_fRoll, m_fPitch, m_fYaw ); }
//
//	float m_fRoll;		
//	float m_fPitch;	
//	float m_fYaw;
//};

FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvDirection3)


class FV_NETWORK_API FvNetAddress
{
public:
	FvNetAddress();
	FvNetAddress(FvUInt32 ipArg, FvUInt16 portArg);
	FvNetAddress(char* str);

	FvUInt32 m_uiIP;
	FvUInt16 m_uiPort;
	FvUInt16 m_uiSalt;

	int WriteToString(char * str, int length) const;
	operator char*() const { return this->c_str(); }
	char *c_str() const;
	const char *IPAsString() const;

	bool SetFromString(char* str);

	bool IsNone() const { return this->m_uiIP == 0; }

	static FvWatcher &GetWatcher();

	static const FvNetAddress NONE;

private:

	static const int MAX_STRLEN = 32;
	static char ms_pcStringBuf[2][MAX_STRLEN];
	static int ms_iCurrStringBuf;
	static char * NextStringBuf();
};

FV_INLINE bool operator==(const FvNetAddress & a, const FvNetAddress & b);
FV_INLINE bool operator!=(const FvNetAddress & a, const FvNetAddress & b);
FV_INLINE bool operator<(const FvNetAddress & a, const FvNetAddress & b);
FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvNetAddress)


bool FV_NETWORK_API WatcherStringToValue(const char *, FvNetAddress &);
FvString FV_NETWORK_API WatcherValueToString(const FvNetAddress &);

struct FvEntityMailBoxRef
{
	FvEntityID m_iID;
	FvNetAddress m_kAddr;

	enum Component
	{
		CELL = 0,
		BASE = 1,
		GLOBAL = 2,
	};

	Component GetComponent() const { return (Component)(m_kAddr.m_uiSalt >> 13); }
	void SetComponent(Component c) { m_kAddr.m_uiSalt = GetType() | (FvUInt16(c) << 13); }

	FvEntityTypeID GetType() const { return m_kAddr.m_uiSalt & 0x1FFF; }
	void SetType(FvEntityTypeID t) { m_kAddr.m_uiSalt = (m_kAddr.m_uiSalt & 0xE000) | t; }

	void Init() { m_iID = 0; m_kAddr.m_uiIP = 0; m_kAddr.m_uiPort = 0; m_kAddr.m_uiSalt = 0; }
	void Init(FvEntityID i, const FvNetAddress & a,
		Component c, FvEntityTypeID t)
	{
		m_iID = i; m_kAddr = a; m_kAddr.m_uiSalt = (FvUInt16(c) << 13) | t;
	}

	FvEntityMailBoxRef()
	{
		SetType(FV_INVALID_ENTITY_TYPE_ID);
	}
};

FV_INLINE bool operator==(const FvEntityMailBoxRef & a, const FvEntityMailBoxRef & b);
FV_INLINE bool operator!=(const FvEntityMailBoxRef & a, const FvEntityMailBoxRef & b);
FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvEntityMailBoxRef)


class FvCapabilities
{
	typedef FvUInt32 CapsType;
public:
	FvCapabilities();
	void Add(unsigned int cap);
	bool Has(unsigned int cap) const;
	bool Empty() const;
	bool Match(const FvCapabilities& on, const FvCapabilities& off) const;
	bool MatchAny(const FvCapabilities& on, const FvCapabilities& off) const;

	static const unsigned int ms_uiMaxCap = std::numeric_limits<CapsType>::digits - 1;
private:

	CapsType m_uiCaps;
};


class FvSpaceEntryID : public FvNetAddress { };
FV_INLINE bool operator==(const FvSpaceEntryID & a, const FvSpaceEntryID & b);
FV_INLINE bool operator!=(const FvSpaceEntryID & a, const FvSpaceEntryID & b);
FV_INLINE bool operator<(const FvSpaceEntryID & a, const FvSpaceEntryID & b);
FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvSpaceEntryID)


struct InterfaceListenerMsg
{
	FvUInt32		uiIP;
	FvUInt16		uiPort;
	FvUInt16		uiUserID;
	char			kName[32];

	InterfaceListenerMsg(FvUInt32 uiIP_, FvUInt16 uiPort_, FvUInt16 uiUserID_, const FvString& kName_)
		:uiIP(uiIP_), uiPort(uiPort_), uiUserID(uiUserID_)
	{
		strncpy_s(kName, sizeof(kName), kName_.c_str(), sizeof(kName));
	}
	InterfaceListenerMsg()
		:uiIP(0), uiPort(0), uiUserID(1)
	{
		kName[0] = 0;
	}
};

FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, InterfaceListenerMsg)


#include "FvNetTypes.inl"

#endif // __FvNetTypes_H__