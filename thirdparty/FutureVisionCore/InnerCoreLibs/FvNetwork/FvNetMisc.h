//{future header message}
#ifndef __FvNetMisc_H__
#define __FvNetMisc_H__

#include <FvPowerDefines.h>

class FvBinaryIStream;

class FvNetAddress;
class FvNetBundle;
class FvNetInterfaceElement;
class FvNetNub;
class FvNetNubException;
class FvNetReplyMessageHandler;
class FvNetUnpackedMessageHeader;



typedef FvUInt32 FvNetSeqNum;


typedef FvUInt8 FvNetMessageID;


typedef void * FvNetTimerID;
const FvNetTimerID FV_NET_TIMER_ID_NONE = 0;


typedef FvInt32 FvNetChannelID;
const FvNetChannelID FV_NET_CHANNEL_ID_NULL = 0;


typedef FvNetSeqNum FvNetChannelVersion;


typedef FvInt32 FvNetReplyID;
const FvNetReplyID FV_NET_REPLY_ID_NONE = -1;
const FvNetReplyID FV_NET_REPLY_ID_MAX = 1000000;

const int FV_NET_DEFAULT_ONCEOFF_RESEND_PERIOD = 200 * 1000; /* 200 ms */

const int FV_NET_DEFAULT_ONCEOFF_MAX_RESENDS = 50;


enum FvNetReason
{
	FV_NET_REASON_SUCCESS					= 0,
	FV_NET_REASON_TIMER_EXPIRED				= -1,
	FV_NET_REASON_NO_SUCH_PORT				= -2,	
	FV_NET_REASON_GENERAL_NETWORK			= -3,
	FV_NET_REASON_CORRUPTED_PACKET			= -4,
	FV_NET_REASON_NONEXISTENT_ENTRY			= -5,
	FV_NET_REASON_WINDOW_OVERFLOW			= -6,
	FV_NET_REASON_INACTIVITY				= -7,
	FV_NET_REASON_RESOURCE_UNAVAILABLE		= -8,
	FV_NET_REASON_CLIENT_DISCONNECTED		= -9,
	FV_NET_REASON_TRANSMIT_QUEUE_FULL		= -10,
	FV_NET_REASON_CHANNEL_LOST				= -11
};

FV_INLINE
const char * NetReasonToString( FvNetReason reason )
{
	const char * reasons[] =
	{
		"FV_NET_REASON_SUCCESS",
		"FV_NET_REASON_TIMER_EXPIRED",
		"FV_NET_REASON_NO_SUCH_PORT",
		"FV_NET_REASON_GENERAL_NETWORK",
		"FV_NET_REASON_CORRUPTED_PACKET",
		"FV_NET_REASON_NONEXISTENT_ENTRY",
		"FV_NET_REASON_WINDOW_OVERFLOW",
		"FV_NET_REASON_INACTIVITY",
		"FV_NET_REASON_RESOURCE_UNAVAILABLE",
		"FV_NET_REASON_CLIENT_DISCONNECTED",
		"FV_NET_REASON_TRANSMIT_QUEUE_FULL",
		"FV_NET_REASON_CHANNEL_LOST"
	};

	unsigned int index = -reason;

	if (index < sizeof(reasons)/sizeof(reasons[0]))
	{
		return reasons[index];
	}

	return "FV_NET_REASON_UNKNOWN";
}

#endif // __FvNetMisc_H__
