#include "../common/geco-plateform.h"
#include <stdint.h>

const uint CHANNEL_ID_NULL = UINT32_MAX;
const ushort REPLY_ID_NONE = UINT16_MAX;
const ushort REPLY_ID_MAX = UINT16_MAX-1;
const int DEFAULT_ONCEOFF_RESEND_PERIOD = 200 * 1000; /* 200 ms */
const int DEFAULT_ONCEOFF_MAX_RESENDS = 50;
const int NO_REPLY = -1;

enum GECONetReason
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

inline const char * NetReasonToString(GECONetReason reason)
{
	const char * reasons[] =
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

struct msg_fixed_t // 4 BYTES
{
	uchar m_uiIdentifier;
	uchar m_cFlags;
	ushort len;
	uint m_iReplyID;
	//GECONetNub *m_pkNub;
	//GECONetChannel *m_pkChannel;
	//msg_fixed_t() :
	//	m_uiIdentifier(0), m_cFlags(0),
	//	m_iReplyID(UINT32_MAX), m_iLength(0), m_pkNub(NULL), m_pkChannel(NULL)
	//{}
	//const char *MsgName() const;
};
