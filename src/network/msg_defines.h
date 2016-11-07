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

#ifndef __INCLUDE_MSG_DEFINES_H
#define __INCLUDE_MSG_DEFINES_H

// this header includes <windows.h> that has vialations with wsock2.h. Therefor
// must be placed in the first place otherwise complie errors related to  wsock2.h
#include "../protocol/geco-net-common.h" 
#include "../common/geco-plateform.h"
#include "../common/debugging/gecowatchert.h"
#include "callback_defines.h"
#include <stdint.h>

namespace geco
{
	namespace engine
	{

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
#define	NET_LOCALHOSTNAME "localhost"
#else
#define NET_LOCALHOSTNAME "lo"
#endif

#define PORT_LOGIN				20013
#define PORT_MACHINED_OLD		20014
#define PORT_MACHINED			20018
#define PORT_BROADCAST_DISCOVERY	20019
#define PORT_WATCHER				20017
#define PORT_PYTHON_PROXY		40000
#define PORT_PYTHON_CELL			50000

		const int NET_UDP_OVERHEAD = 28;
		const int NET_BITS_PER_BYTE = 8;
		const uint NET_LOCALHOST = 0x0100007F;
		const uint NET_BROADCAST = 0xFFFFFFFF;
		static const int NET_WN_PACKET_SIZE = 0x10000;
		typedef uint TimeStamp;
		typedef int entity_id;
		const uchar ENTITY_DEF_INIT_FLG = 0;
		const entity_id NULL_ENTITY = ((entity_id)0);
		//! Server (0, 0x20000000)
		const entity_id MIN_SERVER_ENTITY = ((entity_id)0);
		const entity_id MAX_SERVER_ENTITY = ((entity_id)0x20000000);
		//! Client (0x1FFFFFFF, 0x40000000)
		const entity_id MIN_CLIENT_ENTITY = ((entity_id)0x1FFFFFFF);
		const entity_id MAX_CLIENT_ENTITY = ((entity_id)0x40000000);
		//! Global (0x3FFFFFFF, 0x7FFFFFFF)
		const entity_id MIN_GLOBAL_ENTITY = ((entity_id)0x3FFFFFFF);
		const entity_id MAX_GLOBAL_ENTITY = ((entity_id)0x7FFFFFFF);
		typedef int SpaceID;
		const SpaceID NULL_SPACE = ((SpaceID)0);
		//! Server (0, 0x40000000)
		const SpaceID MIN_SERVER_SPACE = ((SpaceID)0);
		const SpaceID MAX_SERVER_SPACE = ((SpaceID)0x40000000);
		//! Global (0x3FFFFFFF, 0x7FFFFFFF)
		const SpaceID MIN_GLOBAL_SPACE = ((SpaceID)0x3FFFFFFF);
		const SpaceID MAX_GLOBAL_SPACE = ((SpaceID)0x7FFFFFFF);
		typedef int CellAppID;
		typedef int BaseAppID;
		typedef uint SessionKey;
		//typedef Vector3 Position3D;
		typedef ushort entity_type;
		const entity_type INVALID_ENTITY_TYPE_ID = entity_type(-1);
		typedef int64 DatabaseID;

		typedef uint SeqNum;
		typedef uchar msg_id;
		typedef void * TimerID;
		const TimerID NET_TIMER_ID_NONE = 0;
		typedef int ChannelID;
		const ChannelID NET_CHANNEL_ID_NULL = 0;
		typedef SeqNum ChannelVersion;
		typedef uint GridID;
		typedef uint EventNumber;
		const EventNumber INITIAL_EVENT_NUMBER = 1;
		typedef std::vector< EventNumber > CacheStamps;
		typedef ushort VolatileNumber;
		typedef uchar DetailLevel;
		const int MAX_DATA_LOD_LEVELS = 6;

		const uint CHANNEL_ID_NULL = UINT32_MAX;
		const ushort REPLY_ID_NONE = UINT16_MAX;
		const ushort REPLY_ID_MAX = UINT16_MAX - 1;
		const int DEFAULT_ONCEOFF_RESEND_PERIOD = 200 * 1000; /* 200 ms */
		const int DEFAULT_ONCEOFF_MAX_RESENDS = 50;
		const int NO_REPLY = -1;

		enum GECONetReason
		{
			SUCCESS = 0,
			TIMER_EXPIRED = -1,
			NO_SUCH_PORT = -2,
			GENERAL_NETWORK = -3,
			CORRUPTED_PACKET = -4,
			NONEXISTENT_ENTRY = -5,
			WINDOW_OVERFLOW = -6,
			INACTIVITY = -7,
			RESOURCE_UNAVAILABLE = -8,
			CLIENT_DISCONNECTED = -9,
			TRANSMIT_QUEUE_FULL = -10,
			CHANNEL_LOST = -11
		};

		inline const char * NetReasonToString(GECONetReason reason)
		{
			const char * reasons[] =
			{
				"SUCCESS",
				"TIMER_EXPIRED",
				"NO_SUCH_PORT",
				"GENERAL_NETWORK",
				"CORRUPTED_PACKET",
				"NONEXISTENT_ENTRY",
				"WINDOW_OVERFLOW",
				"INACTIVITY",
				"RESOURCE_UNAVAILABLE",
				"CLIENT_DISCONNECTED",
				"TRANSMIT_QUEUE_FULL",
				"CHANNEL_LOST"
			};
			unsigned int index = -reason;
			if (index < sizeof(reasons) / sizeof(reasons[0]))
			{
				return reasons[index];
			}
			return "UNKNOWN";
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
		struct my_addr
		{
			sockaddrunion su;
			ushort m_uiSalt;
		};
		typedef  my_addr space_entry_id;
		struct entity_mb
		{
			entity_id m_iID;
			my_addr m_kAddr;

			enum Component
			{
				CELL = 0,
				BASE = 1,
				GLOBAL = 2,
			};

			Component GetComponent() const { return (Component)(m_kAddr.m_uiSalt >> 13); }
			void SetComponent(Component c) { m_kAddr.m_uiSalt = GetType() | (ushort(c) << 13); }

			entity_type GetType() const { return m_kAddr.m_uiSalt & 0x1FFF; }
			void SetType(entity_type t) { m_kAddr.m_uiSalt = (m_kAddr.m_uiSalt & 0xE000) | t; }

			void Init()
			{
				m_iID = 0;
				memset(&m_kAddr, 0, sizeof(my_addr));
			}

			void Init(entity_id i, const  my_addr& a, Component c, entity_type t)
			{
				m_iID = i;
				m_kAddr = a;
				m_kAddr.m_uiSalt = (ushort(c) << 13) | t;
			}

			entity_mb()
			{
				SetType(INVALID_ENTITY_TYPE_ID);
				Init();
			}
		};
		struct interface_listener_msg
		{
			sockaddrunion su; //always network endian
			ushort		uiUserID;
			char			kName[MAX_IPADDR_STR_LEN];

			interface_listener_msg(sockaddrunion& su_, ushort uiUserID_, char* kName) :
				su(su_), uiUserID(uiUserID_)
			{
				strncpy(kName, kName, MAX_IPADDR_STR_LEN);
			}
			interface_listener_msg()
			{
				kName[0] = 0;
				uiUserID = 1;
				memset(&su, 0, sizeof(sockaddrunion));
			}
		};
		struct bits_opt_t //FvCapabilities
		{
			static const unsigned int ms_uiMaxCap = std::numeric_limits<uint>::digits - 1;
			uint m_uiCaps;
			bits_opt_t() :
				m_uiCaps(0)
			{
			}

			void Add(unsigned int cap)
			{
				m_uiCaps |= 1 << cap;
			}

			bool Has(unsigned int cap) const
			{
				return !!(m_uiCaps & (1 << cap));
			}

			bool Empty() const
			{
				return m_uiCaps == 0;
			}

			bool Match(const bits_opt_t& on,
				const bits_opt_t& off) const
			{
				return (on.m_uiCaps & m_uiCaps) == on.m_uiCaps &&
					(off.m_uiCaps & ~m_uiCaps) == off.m_uiCaps;
			}

			bool MatchAny(const bits_opt_t& on,
				const bits_opt_t& off) const
			{
				return !!(on.m_uiCaps & m_uiCaps) && (off.m_uiCaps & ~m_uiCaps) == off.m_uiCaps;
			}
		};

		inline bool operator==(const my_addr & a, const my_addr & b)
		{
			return (a.m_uiSalt == b.m_uiSalt) && saddr_equals(&a.su, &b.su);
		}
		inline bool operator!=(const my_addr & a, const my_addr & b)
		{
			return (a.m_uiSalt != b.m_uiSalt) || (!saddr_equals(&a.su, &b.su));
		}
		inline geco_bit_stream_t& operator << (geco_bit_stream_t &os, const my_addr &d)
		{
			ushort salt = htons(d.m_uiSalt);
			os.WriteRaw((uchar*)&d.su, sizeof(sockaddrunion));
			os.Write(salt);
			return os;
		}
		inline geco_bit_stream_t& operator >> (geco_bit_stream_t &is, my_addr &d)
		{
			ushort salt;
			is.ReadRaw((uchar*)&d.su, sizeof(sockaddrunion));
			is.Read(salt);
			d.m_uiSalt = ntohs(salt);
			return is;
		}
		inline geco_bit_stream_t& operator << (geco_bit_stream_t &os, const interface_listener_msg &d)
		{
			ushort ns = htons(d.uiUserID);
			os.WriteRaw((uchar*)&d.su, sizeof(sockaddrunion));
			os.Write(ns);
			os.Write(d.kName);
			return os;
		}
		inline geco_bit_stream_t& operator >> (geco_bit_stream_t &is, interface_listener_msg &d)
		{
			ushort ns;
			is.ReadRaw((uchar*)&d.su, sizeof(sockaddrunion));
			is.Read(ns);
			is.Read(d.kName);
			d.uiUserID = ntohs(ns);
			return is;
		}
		inline geco_bit_stream_t& operator << (geco_bit_stream_t &os, const entity_mb &d)
		{
			os << d.m_kAddr;
			int entitid = htonl(d.m_iID);
			os.Write(entitid);
			return os;
		}
		inline geco_bit_stream_t& operator >> (geco_bit_stream_t &is, entity_mb &d)
		{
			is >> d.m_kAddr;
			int entitid;
			is.ReadRaw((uchar*)&d.m_kAddr.su, sizeof(sockaddrunion));
			is.Read(entitid);
			d.m_iID = ntohl(entitid);
			return is;
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const char NET_FIXED_LENGTH_MESSAGE = 0;
		const char NET_VARIABLE_LENGTH_MESSAGE = 1;
		const char NET_INVALID_MESSAGE = 2;
		struct GECOAPI msg_handler //NetInterfaceElement
		{
			static const msg_handler REPLY;

			msg_id m_uiID;
			uchar m_uiLengthStyle;
			int m_iLengthParam;
			const char *m_pcName;
			msg_handler_cb m_pkHandler;

			msg_handler(const char * name = "", msg_id id = 0,
				uchar lengthStyle = NET_INVALID_MESSAGE, int lengthParam = 0,
				msg_handler_cb pHandler = NULL) :
				m_uiID(id),
				m_uiLengthStyle(lengthStyle),
				m_iLengthParam(lengthParam),
				m_pcName(name),
				m_pkHandler(pHandler)
			{}

			void Set(const char * name, msg_id id, uchar lengthStyle,
				int lengthParam)
			{
				m_uiID = id;
				m_uiLengthStyle = lengthStyle;
				m_iLengthParam = lengthParam;
				m_pcName = name;
			}
			int read_hdr_len() const
			{
				return m_uiLengthStyle == NET_FIXED_LENGTH_MESSAGE ?
					sizeof(msg_id) : m_iLengthParam + sizeof(msg_id);
			}
			bool is_valid_len(uint length) const
			{
				return (m_uiLengthStyle != NET_VARIABLE_LENGTH_MESSAGE) ||
					(length > 0 && length < (1 << (8 * m_iLengthParam)) - 1);
			}
			//int SpecialExpandLength(void * header, FvNetPacket * pPacket) const;
			//int SpecialCompressLength(void * header, int length,
			//	FvNetBundle & bundle) const;

			const char* c_str() const
			{
				static char buf[128];
				snprintf(buf, sizeof(buf), "%d/%s", m_pcName, m_uiID);
				return buf;
			}
		};
	}
}
#endif