/*
* Copyright (c) 2016
* Geco Gaming Company
*
* Permission to use, copy, modify, distribute and sell this software
* and its documentation for GECO purpose is hereby granted without fee,
* provided that the above copyright notice appear in all copies and
* that both that copyright notice and this permission notice appear
* in supporting documentation. Geco Gaming makes no
* representations about the suitability of this software for GECO
* purpose.  It is provided "as is" without express or implied warranty.
*
*/

/**
* Created on 22 April 2016 by Jake Zhang
* Reviewed on 07 May 2016 by Jakze Zhang
*/

#ifndef __INCLUDE_MSG_H
#define __INCLUDE_MSG_H

#include <stdint.h>
#include "../common/geco-plateform.h"
#include "../protocol/geco-net-common.h"

#define PORT_LOGIN				20013
#define PORT_MACHINED_OLD		20014
#define PORT_MACHINED			20018
#define PORT_BROADCAST_DISCOVERY	20019
#define PORT_WATCHER				20017
#define PORT_PYTHON_PROXY		40000
#define PORT_PYTHON_CELL			50000

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

struct FvEntityMailBoxRef
{
	entity_id_t m_iID;
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
#endif

