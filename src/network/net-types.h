//{future header message}
#ifndef __GecoNetTypes_H__
#define __GecoNetTypes_H__

#include "common/geco-plateform.h"
#include "math/ema.h"
#include "math/geco-math-vector.h"
#include "math/geco-math-direction.h"
#include "common/ds/geco-bit-stream.h"
#include "common/ds/eastl/EASTL/string.h"

#ifdef _XBOX360
#include "GecoNet360.h"
#endif

#include <vector>
#include <limits>

#ifndef _WIN32
#ifndef _LP64
#define FMT_DBID "lld"
#else
#define FMT_DBID "ld"
#endif
#else
#define FMT_DBID "I64d"
#endif

#ifdef _WIN32//! win32
#define	GECO_NET_LOCALHOSTNAME "localhost"
#else
#define GECO_NET_LOCALHOSTNAME "lo"
#endif

#include "common/debugging/spdlog/spdlog.h"
extern std::shared_ptr<spdlog::logger> g_network_logger;

class GecoWatcher;
class GecoNetBundle;
class GecoNetPacket;
class GecoNetInterfaceElement;

const uchar REPLY_MESSAGE_IDENTIFIER = 0xFF;
const char FIXED_LENGTH_MESSAGE = 0;
const char VARIABLE_LENGTH_MESSAGE = 1;
const char INVALID_MESSAGE = 2;
const int PACKET_MAX_SIZE = 1472; // 1500 - iphdr 20 - updhdr 8 = 1472
const int GECO_NET_UDP_OVERHEAD = 28;
const int GECO_NET_BITS_PER_BYTE = 8;
const uint GECO_NET_LOCALHOST = 0x0100007F;
const uint GECO_NET_BROADCAST = 0xFFFFFFFF;
static const int GECO_NET_WN_PACKET_SIZE = 0x10000;
typedef uint GecoTimeStamp;
typedef int GecoEntityID;
const uchar GECO_ENTITY_DEF_INIT_FLG = 0;
const GecoEntityID GECO_NULL_ENTITY = ((GecoEntityID)0);
//! Server (0, 0x20000000)
const GecoEntityID GECO_MIN_SERVER_ENTITY = ((GecoEntityID)0);
const GecoEntityID GECO_MAX_SERVER_ENTITY = ((GecoEntityID)0x20000000);
//! Client (0x1FFFFFFF, 0x40000000)
const GecoEntityID GECO_MIN_CLIENT_ENTITY = ((GecoEntityID)0x1FFFFFFF);
const GecoEntityID GECO_MAX_CLIENT_ENTITY = ((GecoEntityID)0x40000000);
//! Global (0x3FFFFFFF, 0x7FFFFFFF)
const GecoEntityID GECO_MIN_GLOBAL_ENTITY = ((GecoEntityID)0x3FFFFFFF);
const GecoEntityID GECO_MAX_GLOBAL_ENTITY = ((GecoEntityID)0x7FFFFFFF);
typedef int GecoSpaceID;
const GecoSpaceID GECO_NULL_SPACE = ((GecoSpaceID)0);
//! Server (0, 0x40000000)
const GecoSpaceID GECO_MIN_SERVER_SPACE = ((GecoSpaceID)0);
const GecoSpaceID GECO_MAX_SERVER_SPACE = ((GecoSpaceID)0x40000000);
//! Global (0x3FFFFFFF, 0x7FFFFFFF)
const GecoSpaceID GECO_MIN_GLOBAL_SPACE = ((GecoSpaceID)0x3FFFFFFF);
const GecoSpaceID GECO_MAX_GLOBAL_SPACE = ((GecoSpaceID)0x7FFFFFFF);
typedef int GecoCellAppID;
typedef int GecoBaseAppID;
typedef uint GecoSessionKey;
typedef GecoVector3 GecoPosition3D;
typedef ushort GecoEntityTypeID;
const GecoEntityTypeID GECO_INVALID_ENTITY_TYPE_ID = GecoEntityTypeID(-1);
typedef int64 GecoDatabaseID;
typedef uint GecoGridID;
typedef uint GecoEventNumber;
const GecoEventNumber GECO_INITIAL_EVENT_NUMBER = 1;
typedef std::vector< GecoEventNumber > GecoCacheStamps;
typedef ushort GecoVolatileNumber;
typedef uchar GecoDetailLevel;
const int GECO_MAX_DATA_LOD_LEVELS = 6;
typedef int GecoNetSeqNum;
typedef uchar GecoNetMessageID;
typedef void * GecoNetTimerID;
const GecoNetTimerID GECO_NET_TIMER_ID_NONE = 0;
typedef int GecoNetChannelID;
const GecoNetChannelID GECO_NET_CHANNEL_ID_NULL = 0;
typedef GecoNetSeqNum GecoNetChannelVersion;
typedef int GecoNetReplyID;
const GecoNetReplyID GECO_NET_REPLY_ID_NONE = -1;
const GecoNetReplyID GECO_NET_REPLY_ID_MAX = 1000000;
const int GECO_NET_DEFAULT_ONCEOFF_RESEND_PERIOD = 200 * 1000; /* 200 ms */
const int GECO_NET_DEFAULT_ONCEOFF_MAX_RESENDS = 50;

enum GecoNetReason
{
	GECO_NET_REASON_SUCCESS = 0,
	GECO_NET_REASON_TIMER_EXPIRED = -1,
	GECO_NET_REASON_NO_SUCH_PORT = -2,
	GECO_NET_REASON_GENERAL_NETWORK = -3,
	GECO_NET_REASON_CORRUPTED_PACKET = -4,
	GECO_NET_REASON_NONEXISTENT_ENTRY = -5,
	GECO_NET_REASON_WINDOW_OVERFLOW = -6,
	GECO_NET_REASON_INACTIVITY = -7,
	GECO_NET_REASON_RESOURCE_UNAVAILABLE = -8,
	GECO_NET_REASON_CLIENT_DISCONNECTED = -9,
	GECO_NET_REASON_TRANSMIT_QUEUE_FULL = -10,
	GECO_NET_REASON_CHANNEL_LOST = -11
};
INLINE const char * NetReasonToString(GecoNetReason reason)
{
	static const char * reasons[] =
	{
		"GECO_NET_REASON_SUCCESS",
		"GECO_NET_REASON_TIMER_EXPIRED",
		"GECO_NET_REASON_NO_SUCH_PORT",
		"GECO_NET_REASON_GENERAL_NETWORK",
		"GECO_NET_REASON_CORRUPTED_PACKET",
		"GECO_NET_REASON_NONEXISTENT_ENTRY",
		"GECO_NET_REASON_WINDOW_OVERFLOW",
		"GECO_NET_REASON_INACTIVITY",
		"GECO_NET_REASON_RESOURCE_UNAVAILABLE",
		"GECO_NET_REASON_CLIENT_DISCONNECTED",
		"GECO_NET_REASON_TRANSMIT_QUEUE_FULL",
		"GECO_NET_REASON_CHANNEL_LOST"
	};
	unsigned int index = -reason;
	if (index < sizeof(reasons) / sizeof(reasons[0]))
	{
		return reasons[index];
	}
	return "GECO_NET_REASON_UNKNOWN";
}

class GECOAPI GecoNetAddress
{
private:
	static const int MAX_STRLEN = 32;
	static char ms_pcStringBuf[2][MAX_STRLEN];
	static int ms_iCurrStringBuf;
	static char * NextStringBuf();

public:
	uint m_uiIP;
	ushort m_uiPort;
	ushort m_uiSalt;

public:
	GecoNetAddress();
	GecoNetAddress(uint ipArg, ushort portArg);
	GecoNetAddress(char* str);

	int WriteToString(char * str, int length) const;
	operator char*() const { return this->c_str(); }
	char *c_str() const;
	const char *IPAsString() const;

	bool SetFromString(char* str);

	bool IsNone() const { return this->m_uiIP == 0; }

	static GecoWatcher* GetWatcher();

	static const GecoNetAddress NONE;
};

INLINE bool operator==(const GecoNetAddress & a, const GecoNetAddress & b);
INLINE bool operator!=(const GecoNetAddress & a, const GecoNetAddress & b);
INLINE bool operator<(const GecoNetAddress & a, const GecoNetAddress & b);
INLINE geco_bit_stream_t& operator >> (geco_bit_stream_t& kIS, GecoNetAddress& kDes)
{
	if (kIS.is_compression_mode())
	{
		kIS.ReadMini(kDes.m_uiIP);
		kIS.ReadMini(kDes.m_uiPort);
		kIS.ReadMini(kDes.m_uiSalt);
	}
	else
	{
		kIS.Read(kDes.m_uiIP);
		kIS.Read(kDes.m_uiPort);
		kIS.Read(kDes.m_uiSalt);
	}
	return kIS;
}
INLINE geco_bit_stream_t& operator << (geco_bit_stream_t& kOS, const GecoNetAddress& kDes)
{
	if (kOS.is_compression_mode())
	{
		kOS.WriteMini(kDes.m_uiIP);
		kOS.WriteMini(kDes.m_uiPort);
		kOS.WriteMini(kDes.m_uiSalt);
	}
	else
	{
		kOS.WriteMini(kDes.m_uiIP);
		kOS.WriteMini(kDes.m_uiPort);
		kOS.WriteMini(kDes.m_uiSalt);
	}
	return kOS;
}


GECOAPI bool  WatcherStringToValue(const char *, GecoNetAddress &);
GECOAPI eastl::string  WatcherValueToString(const GecoNetAddress &);

struct GECOAPI GecoEntityMailBoxRef
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
		SetType(GECO_INVALID_ENTITY_TYPE_ID);
	}
};
INLINE bool operator==(const GecoEntityMailBoxRef & a, const GecoEntityMailBoxRef & b);
INLINE bool operator!=(const GecoEntityMailBoxRef & a, const GecoEntityMailBoxRef & b);
INLINE geco_bit_stream_t& operator >> (geco_bit_stream_t& kIS, GecoEntityMailBoxRef& kDes)
{
	if (kIS.is_compression_mode())
	{
		kIS.ReadMini(kDes.m_iID);
		kIS.ReadMini(kDes.m_kAddr.m_uiIP);
		kIS.ReadMini(kDes.m_kAddr.m_uiPort);
		kIS.ReadMini(kDes.m_kAddr.m_uiSalt);
	}
	else
	{
		kIS.Read(kDes.m_iID);
		kIS.Read(kDes.m_kAddr.m_uiIP);
		kIS.Read(kDes.m_kAddr.m_uiPort);
		kIS.Read(kDes.m_kAddr.m_uiSalt);
	}
	return kIS;
}
INLINE geco_bit_stream_t& operator << (geco_bit_stream_t& kOS, const GecoEntityMailBoxRef& kDes)
{
	if (kOS.is_compression_mode())
	{
		kOS.WriteMini(kDes.m_iID);
		kOS.WriteMini(kDes.m_kAddr.m_uiIP);
		kOS.WriteMini(kDes.m_kAddr.m_uiPort);
		kOS.WriteMini(kDes.m_kAddr.m_uiSalt);
	}
	else
	{
		kOS.Write(kDes.m_iID);
		kOS.Write(kDes.m_kAddr.m_uiIP);
		kOS.Write(kDes.m_kAddr.m_uiPort);
		kOS.Write(kDes.m_kAddr.m_uiSalt);
	}
	return kOS;
}

class GECOAPI GecoCapabilities
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

class GECOAPI GecoSpaceEntryID : public GecoNetAddress { };
INLINE bool operator==(const GecoSpaceEntryID & a, const GecoSpaceEntryID & b);
INLINE bool operator!=(const GecoSpaceEntryID & a, const GecoSpaceEntryID & b);
INLINE bool operator<(const GecoSpaceEntryID & a, const GecoSpaceEntryID & b);
INLINE geco_bit_stream_t& operator >> (geco_bit_stream_t& kIS, GecoSpaceEntryID& kDes)
{
	if (kIS.is_compression_mode())
	{
		kIS.ReadMini(kDes.m_uiIP);
		kIS.ReadMini(kDes.m_uiPort);
		kIS.ReadMini(kDes.m_uiSalt);
	}
	else
	{
		kIS.Read(kDes.m_uiIP);
		kIS.Read(kDes.m_uiPort);
		kIS.Read(kDes.m_uiSalt);
	}
	return kIS;
}
INLINE geco_bit_stream_t& operator << (geco_bit_stream_t& kOS, const GecoSpaceEntryID& kDes)
{
	if (kOS.is_compression_mode())
	{
		kOS.WriteMini(kDes.m_uiIP);
		kOS.WriteMini(kDes.m_uiPort);
		kOS.WriteMini(kDes.m_uiSalt);
	}
	else
	{
		kOS.WriteMini(kDes.m_uiIP);
		kOS.WriteMini(kDes.m_uiPort);
		kOS.WriteMini(kDes.m_uiSalt);
	}
	return kOS;
}

struct GECOAPI InterfaceListenerMsg
{
	uint		uiIP;
	ushort		uiPort;
	ushort		uiUserID;
	char			kName[32];

	InterfaceListenerMsg(uint uiIP_, ushort uiPort_, ushort uiUserID_, const eastl::string& kName_)
		:uiIP(uiIP_), uiPort(uiPort_), uiUserID(uiUserID_)
	{
		strncpy(kName, kName_.c_str(), sizeof(kName));
	}
	InterfaceListenerMsg()
		:uiIP(0), uiPort(0), uiUserID(1)
	{
		kName[0] = 0;
	}
};
INLINE geco_bit_stream_t& operator >> (geco_bit_stream_t& kIS, InterfaceListenerMsg& kDes)
{
	if (kIS.is_compression_mode())
	{
		kIS.ReadMini(kDes.uiIP);
		kIS.ReadMini(kDes.uiPort);
		kIS.ReadMini(kDes.uiUserID);
		kIS.ReadMini(kDes.kName);
	}
	else
	{
		kIS.Read(kDes.uiIP);
		kIS.Read(kDes.uiPort);
		kIS.Read(kDes.uiUserID);
		kIS.Read(kDes.kName);
	}
	return kIS;
}
INLINE geco_bit_stream_t& operator << (geco_bit_stream_t& kOS, const InterfaceListenerMsg& kDes)
{
	if (kOS.is_compression_mode())
	{
		kOS.WriteMini(kDes.uiIP);
		kOS.WriteMini(kDes.uiPort);
		kOS.WriteMini(kDes.uiUserID);
		kOS.WriteMini(kDes.kName);
	}
	else
	{
		kOS.Write(kDes.uiIP);
		kOS.Write(kDes.uiPort);
		kOS.Write(kDes.uiUserID);
		kOS.Write(kDes.kName);
	}
	return kOS;
}

/************************************************************************/
/*
����=�Ƕȳ��Ԧк��ٳ���180
�Ƕ�=���ȳ��Ԧ��ٳ���180
*/
/************************************************************************/
typedef GecoVector3 GecoCoord;

struct GECOAPI GecoNetInputMessageHandler
{
	virtual ~GecoNetInputMessageHandler() {};
	virtual void HandleMessage(const GecoNetAddress & source, geco_bit_stream_t & data) = 0;
};

struct GecoNetPacket
{
	typedef ushort Flags;
	enum
	{
		FLAG_HAS_REQUESTS = 0x0001,
		FLAG_HAS_PIGGYBACKS = 0x0002,
		FLAG_HAS_ACKS = 0x0004,
		FLAG_ON_CHANNEL = 0x0008,
		FLAG_IS_RELIABLE = 0x0010,
		FLAG_IS_FRAGMENT = 0x0020,
		FLAG_HAS_SEQUENCE_NUMBER = 0x0040,
		FLAG_INDEXED_CHANNEL = 0x0080,
		FLAG_HAS_CHECKSUM = 0x0100,
		FLAG_CREATE_CHANNEL = 0x0200,
		KNOWN_FLAGS = 0x03FF
	};

	typedef ushort Offset;
	GecoNetPacket* m_spNext;
	int m_iMsgEndOffset;
	int m_iFooterSize;
	int m_iExtraFilterSize;
	Offset m_uiFirstRequestOffset;
	Offset *m_puiLastRequestOffset;
	char m_acData[PACKET_MAX_SIZE];

	char *Back() { return m_acData + m_iMsgEndOffset; }
	const char *Body() const { return m_acData + 2; } // start of first msg hdr

	GecoNetPacket* Next() { return m_spNext; }
	const GecoNetPacket* Next() const { return m_spNext; }
};

class GECOAPI GecoNetInterfaceElement
{
public:
	static const GecoNetInterfaceElement REPLY;

protected:
	GecoNetMessageID m_uiID;
	uchar m_uiLengthStyle;
	int	m_iLengthParam;
	const char *m_pcName;
	GecoNetInputMessageHandler *m_pkHandler;

protected:
	int SpecialExpandLength(void * header, GecoNetPacket * pPacket) const;
	int SpecialCompressLength(void * header, int length,
		GecoNetBundle & bundle) const;

public:
	GecoNetInterfaceElement(const char * name = "", GecoNetMessageID id = 0,
		uchar lengthStyle = INVALID_MESSAGE, int lengthParam = 0,
		GecoNetInputMessageHandler * pHandler = NULL) :
		m_uiID(id),
		m_uiLengthStyle(lengthStyle),
		m_iLengthParam(lengthParam),
		m_pcName(name),
		m_pkHandler(pHandler)
	{}

	void Set(const char * name, GecoNetMessageID id, uchar lengthStyle,
		int lengthParam)
	{
		m_uiID = id;
		m_uiLengthStyle = lengthStyle;
		m_iLengthParam = lengthParam;
		m_pcName = name;
	}

	int HeaderSize() const;
	int NominalBodySize() const;
	int CompressLength(void * header, int length, GecoNetBundle & bundle) const;
	int ExpandLength(void * header, GecoNetPacket * pPacket) const;
	bool CanHandleLength(int length) const
	{
		return (m_uiLengthStyle != VARIABLE_LENGTH_MESSAGE) ||
			(m_iLengthParam >= int(sizeof(int32))) ||
			(length < (1 << (8 * m_iLengthParam)) - 1);
	}

	GecoNetMessageID GetID() const { return m_uiID; }
	void SetID(GecoNetMessageID id) { m_uiID = id; }

	uchar LengthStyle() const { return m_uiLengthStyle; }
	int LengthParam() const { return m_iLengthParam; }

	const char *GetName() const { return m_pcName; }
	const char *c_str() const
	{
		static char buf[256];
		geco_snprintf(buf, sizeof(buf), "%s/%d", m_pcName, m_uiID);
		return buf;
	}

	GecoNetInputMessageHandler *GetHandler() const { return m_pkHandler; }
	void SetHandler(GecoNetInputMessageHandler * pHandler) { m_pkHandler = pHandler; }
};

class GECOAPI GecoNetInterfaceElementWithStats : public GecoNetInterfaceElement
{
private:
	unsigned int m_uiMaxBytesReceived;
	unsigned int m_uiNumBytesReceived;
	unsigned int m_uiNumMessagesReceived;
	cumulative_ema_t< unsigned int > m_kAvgBytesReceivedPerSecond;
	cumulative_ema_t< unsigned int > m_kAvgMessagesReceivedPerSecond;
	static const float AVERAGE_BIAS;

private:
	int IDAsInt() const { return this->GetID(); }

public:
	static GecoWatcher* GetWatcher();

public:
	GecoNetInterfaceElementWithStats() :
		m_uiMaxBytesReceived(0),
		m_uiNumBytesReceived(0),
		m_uiNumMessagesReceived(0),
		m_kAvgBytesReceivedPerSecond(AVERAGE_BIAS),
		m_kAvgMessagesReceivedPerSecond(AVERAGE_BIAS)
	{
	}

	void MessageReceived(unsigned int msgLen)
	{
		++m_uiNumMessagesReceived;
		++m_kAvgMessagesReceivedPerSecond.value();
		m_uiNumBytesReceived += msgLen;
		m_kAvgBytesReceivedPerSecond.value() += msgLen;
		m_uiMaxBytesReceived = std::max(msgLen, m_uiMaxBytesReceived);
	}

	void Tick()
	{
		m_kAvgBytesReceivedPerSecond.sample();
		m_kAvgMessagesReceivedPerSecond.sample();
	}

	unsigned int MaxBytesReceived() const { return m_uiMaxBytesReceived; }

	unsigned int NumBytesReceived() const { return m_uiNumBytesReceived; }

	unsigned int NumMessagesReceived() const { return m_uiNumMessagesReceived; }

	float AvgMessagesReceivedPerSecond() const
	{
		return m_kAvgMessagesReceivedPerSecond.average();
	}

	float AvgBytesReceivedPerSecond() const
	{
		return m_kAvgBytesReceivedPerSecond.average();
	}

	float AvgMessageLength() const
	{
		return m_uiNumMessagesReceived ?
			float(m_uiNumBytesReceived) / float(m_uiNumMessagesReceived) :
			0.0f;
	}
};


INLINE GecoNetAddress::GecoNetAddress()
{
}
INLINE GecoNetAddress::GecoNetAddress(uint ipArg, ushort portArg) :
	m_uiIP(ipArg),
	m_uiPort(portArg),
	m_uiSalt(0)
{
}
INLINE bool operator==(const GecoNetAddress & a, const GecoNetAddress & b)
{
	return (a.m_uiIP == b.m_uiIP) && (a.m_uiPort == b.m_uiPort);
}
INLINE bool operator!=(const GecoNetAddress & a, const GecoNetAddress & b)
{
	return (a.m_uiIP != b.m_uiIP) || (a.m_uiPort != b.m_uiPort);
}
INLINE bool operator<(const GecoNetAddress & a, const GecoNetAddress & b)
{
	return (a.m_uiIP < b.m_uiIP) || (a.m_uiIP == b.m_uiIP && (a.m_uiPort < b.m_uiPort));
}
INLINE bool operator==(const GecoEntityMailBoxRef & a, const GecoEntityMailBoxRef & b)
{
	return (a.m_iID == b.m_iID) && (a.m_kAddr == b.m_kAddr) && (a.m_kAddr.m_uiSalt == b.m_kAddr.m_uiSalt);
}
INLINE bool operator!=(const GecoEntityMailBoxRef & a, const GecoEntityMailBoxRef & b)
{
	return (a.m_iID != b.m_iID) || (a.m_kAddr != b.m_kAddr) || (a.m_kAddr.m_uiSalt != b.m_kAddr.m_uiSalt);
}
INLINE GecoCapabilities::GecoCapabilities() :
	m_uiCaps(0)
{
}
INLINE void GecoCapabilities::Add(unsigned int cap)
{
	m_uiCaps |= 1 << cap;
}
INLINE bool GecoCapabilities::Has(unsigned int cap) const
{
	return !!(m_uiCaps & (1 << cap));
}
INLINE bool GecoCapabilities::Empty() const
{
	return m_uiCaps == 0;
}
INLINE bool GecoCapabilities::Match(const GecoCapabilities& on,
	const GecoCapabilities& off) const
{
	return (on.m_uiCaps & m_uiCaps) == on.m_uiCaps &&
		(off.m_uiCaps & ~m_uiCaps) == off.m_uiCaps;
}
INLINE bool GecoCapabilities::MatchAny(const GecoCapabilities& on,
	const GecoCapabilities& off) const
{
	return !!(on.m_uiCaps & m_uiCaps) && (off.m_uiCaps & ~m_uiCaps) == off.m_uiCaps;
}
INLINE bool operator==(const GecoSpaceEntryID & a, const GecoSpaceEntryID & b)
{
	return operator==((GecoNetAddress)a, (GecoNetAddress)b) && a.m_uiSalt == b.m_uiSalt;
}
INLINE bool operator!=(const GecoSpaceEntryID & a, const GecoSpaceEntryID & b)
{
	return operator!=((GecoNetAddress)a, (GecoNetAddress)b) || a.m_uiSalt != b.m_uiSalt;
}
INLINE bool operator<(const GecoSpaceEntryID & a, const GecoSpaceEntryID & b)
{
	return operator<((GecoNetAddress)a, (GecoNetAddress)b) ||
		(operator==((GecoNetAddress)a, (GecoNetAddress)b) && (a.m_uiSalt < b.m_uiSalt));
}
#endif // __GecoNetTypes_H__
