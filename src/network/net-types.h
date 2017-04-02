//{future header message}
#ifndef __GecoNetTypes_H__
#define __GecoNetTypes_H__

#include "common/geco-plateform.h"
#include "math/geco-math-vector.h"
#include "math/geco-math-direction.h"
#include "common/ds/geco-bit-stream.h"
#include "common/ds/eastl/EASTL/string.h"

#ifdef _XBOX360
#include "GecoNet360.h"
#endif

#include <vector>
#include <limits>

class GecoWatcher;

class GecoBinaryIStream;
class GecoBinaryOStream;

const int FV_NET_UDP_OVERHEAD = 28;

const int FV_NET_BITS_PER_BYTE = 8;

const uint FV_NET_LOCALHOST = 0x0100007F;

const uint FV_NET_BROADCAST = 0xFFFFFFFF;

static const int FV_NET_WN_PACKET_SIZE = 0x10000;

typedef uint GecoTimeStamp;

typedef int GecoEntityID;

const uchar FV_ENTITY_DEF_INIT_FLG = 0;

const GecoEntityID FV_NULL_ENTITY = ((GecoEntityID)0);
//! Server (0, 0x20000000)
const GecoEntityID FV_MIN_SERVER_ENTITY = ((GecoEntityID)0);
const GecoEntityID FV_MAX_SERVER_ENTITY = ((GecoEntityID)0x20000000);
//! Client (0x1FFFFFFF, 0x40000000)
const GecoEntityID FV_MIN_CLIENT_ENTITY = ((GecoEntityID)0x1FFFFFFF);
const GecoEntityID FV_MAX_CLIENT_ENTITY = ((GecoEntityID)0x40000000);
//! Global (0x3FFFFFFF, 0x7FFFFFFF)
const GecoEntityID FV_MIN_GLOBAL_ENTITY = ((GecoEntityID)0x3FFFFFFF);
const GecoEntityID FV_MAX_GLOBAL_ENTITY = ((GecoEntityID)0x7FFFFFFF);

typedef int GecoSpaceID;

const GecoSpaceID FV_NULL_SPACE = ((GecoSpaceID)0);
//! Server (0, 0x40000000)
const GecoSpaceID FV_MIN_SERVER_SPACE = ((GecoSpaceID)0);
const GecoSpaceID FV_MAX_SERVER_SPACE = ((GecoSpaceID)0x40000000);
//! Global (0x3FFFFFFF, 0x7FFFFFFF)
const GecoSpaceID FV_MIN_GLOBAL_SPACE = ((GecoSpaceID)0x3FFFFFFF);
const GecoSpaceID FV_MAX_GLOBAL_SPACE = ((GecoSpaceID)0x7FFFFFFF);

typedef int GecoCellAppID;

typedef int GecoBaseAppID;

typedef uint GecoSessionKey;

typedef GecoVector3 GecoPosition3D;

typedef ushort GecoEntityTypeID;
const GecoEntityTypeID FV_INVALID_ENTITY_TYPE_ID = GecoEntityTypeID(-1);

typedef int64 GecoDatabaseID;

#ifndef _WIN32
#ifndef _LP64
#define FMT_DBID "lld"
#else
#define FMT_DBID "ld"
#endif
#else
#define FMT_DBID "I64d"
#endif

typedef uint GecoGridID;

typedef uint GecoEventNumber;
const GecoEventNumber FV_INITIAL_EVENT_NUMBER = 1;

typedef std::vector< GecoEventNumber > GecoCacheStamps;

typedef ushort GecoVolatileNumber;

typedef uchar GecoDetailLevel;

const int FV_MAX_DATA_LOD_LEVELS = 6;

#ifdef _WIN32//! win32
#define	FV_NET_LOCALHOSTNAME "localhost"
#else
#define FV_NET_LOCALHOSTNAME "lo"
#endif

//
//struct GECOAPI GecoDirection3
//{
//	GecoDirection3() {};
//	GecoDirection3( const GecoVector3 & v ) :
//		m_fRoll ( v[0] ),
//		m_fPitch( v[1] ),
//		m_fYaw  ( v[2] ) {}
//
//	GecoVector3 AsVector3() const { return GecoVector3( m_fRoll, m_fPitch, m_fYaw ); }
//
//	float m_fRoll;		
//	float m_fPitch;	
//	float m_fYaw;
//};

//FV_IOSTREAM_IMP_BY_MEMCPY(INLINE, GecoDirection3)


class GECOAPI GecoNetAddress
{
public:
	GecoNetAddress();
	GecoNetAddress(uint ipArg, ushort portArg);
	GecoNetAddress(char* str);

	uint m_uiIP;
	ushort m_uiPort;
	ushort m_uiSalt;

	int WriteToString(char * str, int length) const;
	operator char*() const { return this->c_str(); }
	char *c_str() const;
	const char *IPAsString() const;

	bool SetFromString(char* str);

	bool IsNone() const { return this->m_uiIP == 0; }

	static GecoWatcher &GetWatcher();

	static const GecoNetAddress NONE;

private:

	static const int MAX_STRLEN = 32;
	static char ms_pcStringBuf[2][MAX_STRLEN];
	static int ms_iCurrStringBuf;
	static char * NextStringBuf();
};

INLINE bool operator==(const GecoNetAddress & a, const GecoNetAddress & b);
INLINE bool operator!=(const GecoNetAddress & a, const GecoNetAddress & b);
INLINE bool operator<(const GecoNetAddress & a, const GecoNetAddress & b);
//FV_IOSTREAM_IMP_BY_MEMCPY(INLINE, GecoNetAddress)


GECOAPI bool  WatcherStringToValue(const char *, GecoNetAddress &);
GECOAPI eastl::string  WatcherValueToString(const GecoNetAddress &);

struct GecoEntityMailBoxRef
{
	GecoEntityID m_iID;
	GecoNetAddress m_kAddr;

	enum Component
	{
		CELL = 0,
		BASE = 1,
		GLOBAL = 2,
	};

	Component GetComponent() const { return (Component)(m_kAddr.m_uiSalt >> 13); }
	void SetComponent(Component c) { m_kAddr.m_uiSalt = GetType() | (ushort(c) << 13); }

	GecoEntityTypeID GetType() const { return m_kAddr.m_uiSalt & 0x1FFF; }
	void SetType(GecoEntityTypeID t) { m_kAddr.m_uiSalt = (m_kAddr.m_uiSalt & 0xE000) | t; }

	void Init() { m_iID = 0; m_kAddr.m_uiIP = 0; m_kAddr.m_uiPort = 0; m_kAddr.m_uiSalt = 0; }
	void Init(GecoEntityID i, const GecoNetAddress & a,
		Component c, GecoEntityTypeID t)
	{
		m_iID = i; m_kAddr = a; m_kAddr.m_uiSalt = (ushort(c) << 13) | t;
	}

	GecoEntityMailBoxRef()
	{
		SetType(FV_INVALID_ENTITY_TYPE_ID);
	}
};

INLINE bool operator==(const GecoEntityMailBoxRef & a, const GecoEntityMailBoxRef & b);
INLINE bool operator!=(const GecoEntityMailBoxRef & a, const GecoEntityMailBoxRef & b);
//FV_IOSTREAM_IMP_BY_MEMCPY(INLINE, GecoEntityMailBoxRef)


class GecoCapabilities
{
	typedef uint CapsType;
public:
	GecoCapabilities();
	void Add(unsigned int cap);
	bool Has(unsigned int cap) const;
	bool Empty() const;
	bool Match(const GecoCapabilities& on, const GecoCapabilities& off) const;
	bool MatchAny(const GecoCapabilities& on, const GecoCapabilities& off) const;

	static const unsigned int ms_uiMaxCap = std::numeric_limits<CapsType>::digits - 1;
private:
	CapsType m_uiCaps;
};


class GecoSpaceEntryID : public GecoNetAddress { };
INLINE bool operator==(const GecoSpaceEntryID & a, const GecoSpaceEntryID & b);
INLINE bool operator!=(const GecoSpaceEntryID & a, const GecoSpaceEntryID & b);
INLINE bool operator<(const GecoSpaceEntryID & a, const GecoSpaceEntryID & b);
//FV_IOSTREAM_IMP_BY_MEMCPY(INLINE, GecoSpaceEntryID)


struct InterfaceListenerMsg
{
	uint		uiIP;
	ushort		uiPort;
	ushort		uiUserID;
	char			kName[32];

	InterfaceListenerMsg(uint uiIP_, ushort uiPort_, ushort uiUserID_, const eastl::string& kName_)
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

//FV_IOSTREAM_IMP_BY_MEMCPY(INLINE, InterfaceListenerMsg)
//#include "GecoNetTypes.inl"

#endif // __GecoNetTypes_H__
