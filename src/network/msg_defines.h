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
#include "../protocol/geco-net-msg.h"
#include "../common/geco-plateform.h"
#include "../common/debugging/debug.h"
#include "../common/debugging/timer_queue_t.h"
#include "callback_defines.h"
#include "../common/ds/geco-bit-stream.h"
#include "networkstats.h"

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
#define	LOCALHOSTNAME "localhost"
#else
#define LOCALHOSTNAME "lo"
#endif

#define PORT_LOGIN				20013
#define PORT_MACHINED_OLD		20014
#define PORT_MACHINED			20018
#define PORT_BROADCAST_DISCOVERY	20019
#define PORT_WATCHER				20017
#define PORT_PYTHON_PROXY		40000
#define PORT_PYTHON_CELL			50000

const int UDP_OVERHEAD = 28;
const int RAWSOCK_OVERHEAD = 20;
const int BITS_PER_BYTE = 8;
const uint LOCALHOST = 0x0100007F;
const uint BROADCAST = 0xFFFFFFFF;
static const int WN_PACKET_SIZE = 0x10000;
typedef uint TimeStamp;
typedef int entity_id;
const uchar ENTITY_DEF_INIT_FLG = 0;
const entity_id NULL_ENTITY = ((entity_id) 0);
//! Server (0, 0x20000000)
const entity_id MIN_SERVER_ENTITY = ((entity_id) 0);
const entity_id MAX_SERVER_ENTITY = ((entity_id) 0x20000000);
//! Client (0x1FFFFFFF, 0x40000000)
const entity_id MIN_CLIENT_ENTITY = ((entity_id) 0x1FFFFFFF);
const entity_id MAX_CLIENT_ENTITY = ((entity_id) 0x40000000);
//! Global (0x3FFFFFFF, 0x7FFFFFFF)
const entity_id MIN_GLOBAL_ENTITY = ((entity_id) 0x3FFFFFFF);
const entity_id MAX_GLOBAL_ENTITY = ((entity_id) 0x7FFFFFFF);
typedef int SpaceID;
const SpaceID NULL_SPACE = ((SpaceID) 0);
//! Server (0, 0x40000000)
const SpaceID MIN_SERVER_SPACE = ((SpaceID) 0);
const SpaceID MAX_SERVER_SPACE = ((SpaceID) 0x40000000);
//! Global (0x3FFFFFFF, 0x7FFFFFFF)
const SpaceID MIN_GLOBAL_SPACE = ((SpaceID) 0x3FFFFFFF);
const SpaceID MAX_GLOBAL_SPACE = ((SpaceID) 0x7FFFFFFF);
typedef int CellAppID;
typedef int BaseAppID;
typedef uint SessionKey;
//typedef Vector3 Position3D;
typedef ushort entity_type;
const entity_type INVALID_ENTITY_TYPE_ID = entity_type(-1);
typedef int64 DatabaseID;

typedef uint SeqNum;
typedef uchar msg_id;
//typedef void* TimerID;
//const TimerID TIMER_ID_NONE = 0;
typedef int ChannelID;
typedef SeqNum ChannelVersion;
typedef uint GridID;
typedef uint EventNumber;
const EventNumber INITIAL_EVENT_NUMBER = 1;
typedef std::vector<EventNumber> CacheStamps;
typedef ushort VolatileNumber;
typedef uchar DetailLevel;
const int MAX_DATA_LOD_LEVELS = 6;

const uint CHANNEL_ID_NULL = UINT32_MAX;
const ushort REPLY_ID_NONE = UINT16_MAX;
const ushort REPLY_ID_MAX = UINT16_MAX - 1;
const int DEFAULT_ONCEOFF_RESEND_PERIOD = 200 * 1000; /* 200 ms */
const int DEFAULT_ONCEOFF_MAX_RESENDS = 50;
const int NO_REPLY = -1;

enum geco_engine_reason
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

inline const char * NetReasonToString(geco_engine_reason reason)
{
	const char * reasons[] =
	{ "SUCCESS", "TIMER_EXPIRED", "NO_SUCH_PORT", "GENERAL_NETWORK",
			"CORRUPTED_PACKET", "NONEXISTENT_ENTRY", "WINDOW_OVERFLOW",
			"INACTIVITY", "RESOURCE_UNAVAILABLE", "CLIENT_DISCONNECTED",
			"TRANSMIT_QUEUE_FULL", "CHANNEL_LOST" };
	unsigned int index = -reason;
	if (index < sizeof(reasons) / sizeof(reasons[0]))
	{
		return reasons[index];
	}
	return "UNKNOWN";
}

struct my_addr
{
		sockaddrunion su;
		ushort m_uiSalt;
};
typedef my_addr space_entry_id;

struct entity_mb
{
		entity_id m_iID;
		my_addr m_kAddr;

		enum Component
		{
			CELL = 0, BASE = 1, GLOBAL = 2,
		};

		Component GetComponent() const
		{
			return (Component) (m_kAddr.m_uiSalt >> 13);
		}
		void SetComponent(Component c)
		{
			m_kAddr.m_uiSalt = GetType() | (ushort(c) << 13);
		}

		entity_type GetType() const
		{
			return m_kAddr.m_uiSalt & 0x1FFF;
		}
		void SetType(entity_type t)
		{
			m_kAddr.m_uiSalt = (m_kAddr.m_uiSalt & 0xE000) | t;
		}

		void Init()
		{
			m_iID = 0;
			memset(&m_kAddr, 0, sizeof(my_addr));
		}

		void Init(entity_id i, const my_addr& a, Component c, entity_type t)
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
		ushort uiUserID;
		char kName[MAX_IPADDR_STR_LEN];

		interface_listener_msg(sockaddrunion& su_, ushort uiUserID_,
				char* kName) :
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
		static const unsigned int ms_uiMaxCap =
				std::numeric_limits<uint>::digits - 1;
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

		bool Match(const bits_opt_t& on, const bits_opt_t& off) const
		{
			return (on.m_uiCaps & m_uiCaps) == on.m_uiCaps
					&& (off.m_uiCaps & ~m_uiCaps) == off.m_uiCaps;
		}

		bool MatchAny(const bits_opt_t& on, const bits_opt_t& off) const
		{
			return !!(on.m_uiCaps & m_uiCaps)
					&& (off.m_uiCaps & ~m_uiCaps) == off.m_uiCaps;
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
inline geco_bit_stream_t& operator <<(geco_bit_stream_t &os, const my_addr &d)
{
	ushort salt = htons(d.m_uiSalt);
	os.WriteRaw((uchar*) &d.su, sizeof(sockaddrunion));
	os.Write(salt);
	return os;
}
inline geco_bit_stream_t& operator >>(geco_bit_stream_t &is, my_addr &d)
{
	ushort salt;
	is.ReadRaw((uchar*) &d.su, sizeof(sockaddrunion));
	is.Read(salt);
	d.m_uiSalt = ntohs(salt);
	return is;
}
inline geco_bit_stream_t& operator <<(geco_bit_stream_t &os,
		const interface_listener_msg &d)
{
	ushort ns = htons(d.uiUserID);
	os.WriteRaw((uchar*) &d.su, sizeof(sockaddrunion));
	os.Write(ns);
	os.Write(d.kName);
	return os;
}
inline geco_bit_stream_t& operator >>(geco_bit_stream_t &is,
		interface_listener_msg &d)
{
	ushort ns;
	is.ReadRaw((uchar*) &d.su, sizeof(sockaddrunion));
	is.Read(ns);
	is.Read(d.kName);
	d.uiUserID = ntohs(ns);
	return is;
}
inline geco_bit_stream_t& operator <<(geco_bit_stream_t &os, const entity_mb &d)
{
	os << d.m_kAddr;
	int entitid = htonl(d.m_iID);
	os.Write(entitid);
	return os;
}
inline geco_bit_stream_t& operator >>(geco_bit_stream_t &is, entity_mb &d)
{
	is >> d.m_kAddr;
	int entitid;
	is.ReadRaw((uchar*) &d.m_kAddr.su, sizeof(sockaddrunion));
	is.Read(entitid);
	d.m_iID = ntohl(entitid);
	return is;
}

class geco_bundle_t;
class network_interface_t;
class geco_nub_t;

/////////////////////////////////////// interface_element_t starts ////////////////////////////////////
const uint MSG_HDR_SIZE = 4;
const int RELIABLE_ORDER = 0;
const int RELIABLE_UNORDER = 1;
const int UNRELIABLE_ORDER = 2;
const int UNRELIABLE_UNORDER = 3;
/**
 * 	@internal
 * 	This constant indicates a fixed length message.
 * 	It is used as the lengthStyle in an InterfaceElement.
 * 	For this type of message, lengthParam is the number
 * 	of bytes in the message. This is constant for all
 * 	messages of this type.
 */
const char FIXED_LENGTH_MESSAGE = 0;
/**
 * 	@internal
 * 	This constant indicates a variable length message.
 * 	It is used as the lengthStyle in an InterfaceElement.
 * 	For this type of message, lengthParam is the number
 * 	of bytes used to store the message size. This is
 * 	constant for all messages of this type.
 */
const char VARIABLE_LENGTH_MESSAGE = 1;
/**
 * 	This constant indicates the InterfaceElement has not been initialised.
 */
const char INVALID_MESSAGE = 2;
//00 last bit 0 = var 1=fix
const uchar MSG_VAR = 0;
const uchar MSG_FIX = 1;
//01 last second bit 0 = request 1 = response
const uchar MSG_REQUEST = 0;
const uchar MSG_RESPONSE = 2;
/**
 * 	@internal
 *	This structure describes a single message within an
 *	interface. It describes how to encode and decode the
 *	header of the message, including whether the message
 *	is fixed length or variable.  For fixed length messages, it
 *	defines the length of the message. For variable length
 *	messages, it defines the number of bytes needed to
 *	express the length.
 */
struct GECOAPI interface_element_t
{
		msg_id id_; ///< Unique message ID
		uchar flag_;
		uint nominal_body_size_;
		uint fixed_totals_;

		uchar lengthStyle_;	///< Fixed or variable length
		ushort lengthParam_;	///< This depends on lengthStyle

		const char *name_;	///< The name of the interface method
		msg_handler_cb pHandler_; /// msg handler

		/* value among:
		 const int RELIABLE_ORDER= 0;
		 const int RELIABLE_UNORDER= 1;
		 const int UNRELIABLE_ORDER= 2;
		 const int UNRELIABLE_UNORDER= 3; */
		uint ro_;
		interface_element_stats_t stats_;

		interface_element_t(const char * name = "", msg_id id = 0,
				uchar lengthStyle = INVALID_MESSAGE, ushort lengthParam = 0,
				msg_handler_cb* pHandler = NULL, uint msg_rel_or =
						RELIABLE_ORDER) :
				id_(id), lengthStyle_(lengthStyle), lengthParam_(lengthParam), name_(
						name), pHandler_(*pHandler), ro_(msg_rel_or), flag_(0)
		{
			if (lengthStyle_ == FIXED_LENGTH_MESSAGE)
			{
				flag_ |= MSG_FIX;
				nominal_body_size_ = lengthParam_;
			}
			else
			{
				flag_ |= MSG_VAR;
				nominal_body_size_ = 0;
			}
			fixed_totals_ = nominal_body_size_ + MSG_HDR_SIZE;
		}

		const char* c_str() const
		{
			static char buf[128];
			snprintf(buf, sizeof(buf), "%s/%u", name_, id_);
			return buf;
		}
};
class GECOAPI interface_elements_t
{
	public:
		interface_elements_t(const char * name);
		interface_element_t& Add(interface_element_t& ie);
		msg_handler_cb* handler(int msgid);
		void handler(int msgid, msg_handler_cb * pHandler);
		const interface_element_t & InterfaceElement(uchar id) const;
		void RegisterWithNub(geco_nub_t& nub);
		geco_engine_reason RegisterWithMachined(geco_nub_t& nub, int id) const;
	private:
		typedef eastl::vector<interface_element_t> interface_elements;
		interface_elements m_kElements;
		const char *m_pcName;
};
/////////////////////////////////////// interface_element_t ends ////////////////////////////////////

/////////////////////////////////////// packet starts ////////////////////////////////////
#include "networkstats.h"
// THIS IS DEFAULT 1500 -20 IPHDR - 8 UDPHDR - 12 GECOHDR,
// SHOULD query pmtu from protocol stack and set this for each connection
const uint DEFAULT_MAX_PACKET_SIZE = 1460;
struct FvNetDataField
{
		char *m_pcBeg;
		ushort m_uiLength;
};

struct GECOAPI unpacked_msg_hdr_t // 4 BYTES
{
		static const int NO_REPLY = -1;
		uchar m_uiIdentifier;
		uchar m_cFlags;
		ushort len;
		uint replay_id;
};

/**
 *	This class manages sets of channels.
 */
class GECOAPI network_interface_t
{
};

/**
 *	responsible for receiving game packets from protocol stack (sctp, udp, tcp and so on).
 */
class GECOAPI packet_recv_t
{
		uint curr_channel_id_;
		sockaddrunion curr_saddr_;
		network_interface_t* network_interface_;
		unpacked_msg_hdr_t* curr_packet_;

		network_recv_stats_t stats_;
		geco_engine_reason process_packet();
};

const int DEFAULT_BUNDLE_SEND_BUF_SIZE = 1500 - 20 - 8;
const int DEFAULT_REPLY_MSG_TIMEOUT = 3000; // default request timeout in ms 5 senonds

class GECOAPI geco_channel_t
{
	public:
		/*
		 *	decide the reliablity to use.
		 *	There are two types of channels that we handle. The first is a
		 *	channel between server and server. These channels are low latency,
		 *	high bandwidth, and low loss. The second is a channel between client
		 *	and server, which is high latency, low bandwidth, and high loss.
		 *	Since bandwidth is scarce on client/server channels, only reliable
		 *	data is resent on these channels. Unreliable data is stripped from
		 *	dropped packets and discarded.
		 */
		enum
			:uint
			{
				WAN = 0, //server and server.
			LAN = 1 //client and server.
		};
		uint m_uiType;

		//streams RELIABLE_ORDER= 0
		//streams RELIABLE_UNORDER= 1
		//streams UNRELIABLE_ORDER= 2
		//streams UNRELIABLE_UNORDER= 3
		typedef int stream_id_t;
		typedef eastl::vector<stream_id_t> stream_ids_t;
		const int m_ostreams_total_size;
		const int m_ostreams_avg_size;
		stream_ids_t m_sids[4];
		int curr_stream_idx[4];

		geco_channel_t(int ostreams_total_size = 8) :
				m_ostreams_total_size(ostreams_total_size), m_ostreams_avg_size(
						m_ostreams_total_size / 4)
		{
			//if (m_ostreams_total_size < 4 || m_ostreams_total_size & 3>0) return;
			m_sids[0].reserve(m_ostreams_avg_size);
			m_sids[1].reserve(m_ostreams_avg_size);
			m_sids[2].reserve(m_ostreams_avg_size);
			m_sids[3].reserve(m_ostreams_avg_size);
			curr_stream_idx[0] = -1;
			curr_stream_idx[1] = -1;
			curr_stream_idx[2] = -1;
			curr_stream_idx[3] = -1;
		}

		const char* c_str() const
		{
			return "c_str";
		}
};

/// Interface for a task to be added to the FrequentTasks collection.
typedef std::function<void()> TickTask;

/**
 *	This class is used to maintain the collection of frequent tasks.
 */
class TickTasks
{
	public:
		TickTasks() :
				isDirty_(false), pGotDestroyed_(NULL)
		{
		}

		~TickTasks()
		{
			if (pGotDestroyed_ != NULL)
			{
				*pGotDestroyed_ = true;
			}
		}

		void add(TickTask* pTask)
		{
			isDirty_ = true;
			container_.push_back(pTask);
		}
		/**
		 *	Remove a frequent task.
		 *
		 *	@param pTask 		The frequent task to remove.
		 *
		 *	@return			Whether the frequent task previously existed.
		 */
		bool cancel(TickTask* pTask)
		{
			PROFILER_SCOPED(FrequentTasks_cancel);
			isDirty_ = true;
			Container::iterator iter = eastl::find(container_.begin(),
					container_.end(), pTask);
			if (iter != container_.end())
			{
				container_.erase(iter);
				return true;
			}
			return false;
		}

		void process()
		{
			bool wasInProcess = pGotDestroyed_ != NULL;
			if (wasInProcess && isDirty_)
			{
				// not processing
				return;
			}
			isDirty_ = false;
			// This automatic boolean stores whether we get destroyed as a result of executing a FrequentTask.
			bool gotDestroyed = false;
			if (!wasInProcess)
			{
				pGotDestroyed_ = &gotDestroyed;
			}
			else
			{
				GECO_ASSERT(pGotDestroyed_ != NULL);
			}

			Container::iterator iter = container_.begin();
			while (iter != container_.end())
			{
				TickTask* pTask = *iter;
				// Save away in this call's stack in case we are destructed in do Task.
				bool * pGotDestroyed = pGotDestroyed_;
				GECO_ASSERT(pGotDestroyed != NULL);
				(*pTask)();
				if (*pGotDestroyed)
				{
					// Don't access any member state, exit immediately.
					return;
				}
				// If the vector has been modified, the iterator is now invalid.
				if (isDirty_)
				{
					break;
				}
				++iter;
			}
			if (!wasInProcess)
			{
				pGotDestroyed_ = NULL;
			}
		}

	private:
		typedef eastl::vector<TickTask*> Container;
		Container container_;
		bool isDirty_;
		bool * pGotDestroyed_;
};

class GECOAPI geco_nub_t
{
	private:
		struct InterfaceInfo
		{
				ushort uiUserID;
				ushort uiID;
				sockaddrunion kAddr;
				eastl::string kName;

				InterfaceInfo(ushort uiUserID_, ushort uiID_,
						sockaddrunion& kAddr_, eastl::string& kName_) :
						uiUserID(uiUserID_), uiID(uiID_), kAddr(kAddr_), kName(
								kName_)
				{
				}
		};
		typedef eastl::vector<InterfaceInfo> InterfaceInfoVec;

		TimeQueue64 pTimeQueue_;
		TickTasks pFrequentTasks_;

		// Statistics
		time_stamp_t accSpareTime_;
		time_stamp_t oldSpareTime_;
		time_stamp_t totSpareTime_;
		time_stamp_t lastStatisticsGathered_;

		time_stamp_t startSelect_;
		time_stamp_t endSelect_;
		time_stamp_t spareTime_;

		uint32 numTimerCalls_;
		network_recv_stats_t recv_stats_;

		TimerHandler stat_timer_;
		TimerID stat_timerid_;

		bool breakProcessing_;
		bool shouldIdle_;
		double maxWait_;

	private:

		void clear_spare_time()
		{
			accSpareTime_ += spareTime_;
			spareTime_ = 0;
		}

		void stat_timoeout_cb(TimerID id, void * pUser)
		{
			std::cout << "stat_timoeout_cb called\n";
			// gather statistics every second
			oldSpareTime_ = totSpareTime_;
			totSpareTime_ = accSpareTime_ + spareTime_;
		}

		// assign this function to mtra socket_recv_cb
		//std::function<int(void* args)> recv_event_cb_t
		// this will be called before recv and after receive in mtra module
		// which gives me a chance to update stats
		int socket_read_start(void* args)
		{
			recv_stats_.update_stats();
			recv_stats_.mercuryTimer_.start();
			recv_stats_.systemTimer_.start();
			return 0;
		}
		int socket_read_end(int sfd, bool isudpsocket, char* data, int datalen,
				sockaddrunion* from, sockaddrunion* to, void* user_data)
		{
			bool ret = datalen > 0;
			recv_stats_.systemTimer_.stop(ret);
			if (ret)
			{
				++recv_stats_.numPacketsReceivedPerSecond_;
				if (isudpsocket)
					datalen += UDP_OVERHEAD;
				else
					datalen += RAWSOCK_OVERHEAD;
				// Payload subtracted later
				recv_stats_.numOverheadBytesReceived_ += datalen;
				recv_stats_.numBytesReceivedPerSecond_ += datalen;
				return 0;
			}
			else
			{
				recv_stats_.mercuryTimer_.stop(ret);
			}
			return -1;
		}

		int select_start(void* user_data)
		{
			printf("select_start\n");
			startSelect_ = gettimestamp();
			return 0;
		}
		int select_end(void* user_data)
		{
			printf("select_end\n");
			endSelect_ = gettimestamp();
			spareTime_ += startSelect_ - endSelect_;
			return 0;
		}

		// select for network activity until earliest timer
		int poll_network()
		{
			double maxWait; //sec
			int mswait;

			if (shouldIdle_)
			{
				maxWait = maxWait_;
				if (!pTimeQueue_.empty())
				{
					maxWait = eastl::min(maxWait,
							pTimeQueue_.nextExp(gettimestamp())
									/ stamps_per_sec_double());
				}
				// translate to ms
				mswait = maxWait * 1000;
			}
			else
			{
				mswait = 0;
			}

			//TODO CALL mtra_poll(mswait)  and pass mswait to it do not impl very soon
			// need to add hook for recv and send so that we can have a chance to update recv stats and send stats
			return 0;
		}
		void loop()
		{
			while (!breakProcessing_)
			{
				pFrequentTasks_.process();
				pTimeQueue_.process(gettimestamp());
				poll_network();
			}
		}

	public:
		static const char *USE_FVMACHINED;
		interface_elements_t* m_InterfaceElements;

		geco_nub_t() :
				breakProcessing_(false), shouldIdle_(true),
				//pErrorReporter_(NULL),
				accSpareTime_(0), oldSpareTime_(0), totSpareTime_(0), lastStatisticsGathered_(
						0), numTimerCalls_(0), maxWait_(0.01) // default max wait is 100ms
		{
			stat_timer_.handleTimeout = std::bind(&geco_nub_t::stat_timoeout_cb,
					*this, std::placeholders::_1, std::placeholders::_2);
			stat_timerid_ = pTimeQueue_.add(gettimestamp() + stamps_per_sec(), stamps_per_sec(), &stat_timer_, 0);
		}
		~geco_nub_t()
		{
			stat_timerid_.cancel();
		}

		void ServeInterfaceElement(interface_elements_t& ies);
		geco_engine_reason RegisterWithMachined(const eastl::string& name,
				int id, bool isRegister = true);

		void ShouldIdle(bool val)
		{
			shouldIdle_ = val;
		}
		void BreakProcessing(bool val)
		{
			breakProcessing_ = val;
		}
};

struct geco_transport_t
{
		// Statistics
		time_stamp_t accSpareTime_;
		time_stamp_t oldSpareTime_;
		time_stamp_t totSpareTime_;
		time_stamp_t lastStatisticsGathered_;
		uint numTimerCalls_;

		double maxWait_;
		bool breakProcessing_;

		void processFrequentTasks();
		void processTimers();
		void processStats();
		int processNetwork();

		static geco_watcher_director_t* pWatcher();

};

struct GECOAPI geco_network_interface_t
{
		/**
		 * 	This method sends a bundle to the given address.
		 *
		 * 	Note: any pointers you have into the packet may become invalid after this
		 * 	call (and whenever a channel calls this too).
		 *
		 * 	@param address	The address to send to.
		 * 	@param bundle	The bundle to send
		 *	@param pChannel	The Channel that is sending the bundle.
		 *				(even if the bundle is not sent reliably, it is still passed
		 *				through the filter associated with the channel).
		 */
		void send(const sockaddrunion& address, geco_bundle_t& bundle,
				geco_channel_t * pChannel)
		{
		}
};

#include <functional>
typedef std::function<void(const sockaddrunion& addr, uchar* packet)> packet_monitor_handler_t; //in and out

/**
 *  declares an interface for receiving response messages.
 *  When a client issues a request with response, an interface of this type should
 *  be provided to handle the reply.
 */
struct GECOAPI response_handler_t
{
		/*
		 * 	called by Mercury to deliver a reply message.
		 * 	@param source	The address at which the message originated.
		 * 	@param header	This contains the message type, size, and flags.
		 * 	@param data		The actual message data.
		 * 	@param arg		This is user-defined data that was passed in with the request that generated this reply.
		 */
		std::function<
				void(sockaddrunion& source, unpacked_msg_hdr_t& msghdr,
						geco_bit_stream_t& is, void* args)> msg_parser_;

		/**
		 * 	This method is called by Mercury when the request fails. The
		 * 	normal reason for this happening is a timeout.
		 * 	@param exception	The reason for failure.
		 * 	@param arg			The user-defined data associated with the request.
		 */
		std::function<void(const char* exception, void * arg)> exception_handler_;
		std::function<void(const char* exception, void * arg)> shutdown_handler_;
};

///  represents a request that requires a reply.
struct GECOAPI response_order_t
{
		response_handler_t response_handler;
		void *arg;	//User argument passed to the handler.
		uint timeout_ms;	// Timeout in ms.
};

struct GECOAPI piggy_back_t
{
		uchar* m_spPacket;
		ushort m_iLen;
};

/*-----------------------------------------------------------------------------
 Section: Bundle

 *	A bundle is a sequence of messages. You stream or otherwise
 *	add your messages onto the bundle. When you want to send
 *	a group of messages (possibly just one), you tell a nub
 *	to send the bundle. Bundles can be sent multiple times
 *	to different hosts, but beware that any requests inside
 *	will also be made multiple times.

 How requests and replies work (so I can get it straight):

 When you make a request you put it on the bundle with a 'startRequest' message.
 This means the bundle takes note of it and puts extra information (a reply id)
 in the message's header.

 When a request handler replies to a request, it puts it on the bundle with a
 'startReply' message, passing in the replyID from the broken-out header info
 struct passed to it. This means the bundle adds the special message of type
 'REPLY_MESSAGE_IDENTIFIER', which is always handled by the system.
 -----------------------------------------------------------------------------*/
class GECOAPI geco_bundle_t
{
	public:
		// stores all the requests for this bundle.
		typedef eastl::vector<response_order_t> response_orders_t;
		response_orders_t m_kReplyOrders;

		bool m_bFinalised;
		bool m_isExternal;
		uchar m_uiExtraSize;

	private:
		geco_channel_t* m_pkChannel;// This is the Channel that owns this Bundle, or NULL if not on a Channel.

		// per message stuff
		interface_element_t *m_pkCurIE;
		uint m_iMsgLen;
		uint m_iMsgExtra;
		msg_id* m_pHeader;
		uint m_uiHeaderLen;
		uchar *m_puiMsgBeg;
		bool m_bMsgRequest;
		unpacked_msg_hdr_t m_unpacked_msg_hdr;

		// statistics
		uint m_iNumMessages;	//numMessages
		uint m_iNumReliableOrderedMessages;
		uint m_iNumReliableUnOrderedMessages;
		uint m_iNumUnReliableOrderedMessages;
		uint m_iNumUnReliableUnOrderedMessages;
		uint m_iNumMessagesTotalBytes;

		geco_bit_stream_t m_ucSendBuffers[4]; // 4 buffers for messages with different reliabilities (unrel&uno, unrel&o, rel&o, rel&uno)
		//uint m_write_positions[4];
		geco_bit_stream_t* m_CurrBuf;
		//uint m_CurrWritePos;
		//uchar	*m_MsgLenPtr;

	public:
		// initialises an empty bundle for writing.
		geco_bundle_t(uchar spareSize = 0, geco_channel_t* pChannel = NULL);
		geco_bundle_t(uchar* packetChain);
		virtual ~geco_bundle_t();

		/// flushes the messages from this bundle making it empty.
		void clear(bool do_not_dispose = false);
		/// cancels outstanding requests associated with this bundle.
		void cancel_requests();
		/// returns true if the bundle is empty of messages.
		bool empty() const;
		/// returns the accumulated size of the bundle (including all messages).
		uint size() const;
		/// returns true if this Bundle is owned by an external channel.
		bool is_external_channel() const;

		/**
		 * 	write a request message without response on the bundle. The expected length
		 *	should only be filled in if known (and only for variable-length messages)
		 *	as a hint to whether to start this message on the current packet or to
		 *	send the current bundle then bring in this message into current packet again
		 * 	usually, a void message is reliable but unordered. only when messages are all reliable and ordered,
		 * it is gueeted they are handled in sequence by receiver.
		 *
		 * 	@param ie			The type of message to start.
		 */
		geco_bit_stream_t* start_request_message(interface_element_t& ie);

		/**
		 * 	write a request message with response on the bundle, and call
		 * 	response_handler when the reply comes in or the
		 * 	timeout (in microseconds) expires, whichever comes first.
		 * 	A timeout of <= 0 means never time out (NOT recommended)
		 * 	usually, a void message is reliable but unordered. only when messages are all reliable and ordered,
		 *  it is gueeted they are handled in sequence by receiver.
		 *
		 * 	@param ie			The type of request to start.
		 * 	@param handler		This handler receives the reply.
		 * 	@param arg			User argument that is sent to the handler.
		 * 	@param timeout		Time before a timeout exception is generated.
		 */
		geco_bit_stream_t* start_request_message(interface_element_t& ie,
				response_handler_t& response_handler, void * arg = 0,
				uint timeout = DEFAULT_REPLY_MSG_TIMEOUT);

		/**
		 * 	write a response to a request message. All reply id are should be the replyID
		 * 	from the message header of the request you're replying to.
		 * 	usually, a void message is reliable but unordered. only when messages are all reliable and ordered,
		 *  it is gueeted they are handled in sequence by receiver.
		 *
		 * 	@param id			The id of the message being replied to
		 */
		geco_bit_stream_t* start_response_message(interface_element_t& ie);

	private:
		/**
		 * 	This message begins a new message, with the given number of extra bytes in
		 * 	the header. These extra bytes are normally used for request information.
		 *
		 * 	@param extra	Number of extra bytes to reserve.
		 * 	@return	Pointer to the body of the message.
		 */
		void new_message();
		void dispose();
};

/**
 *  This class is useful when you have a lot of data you want to send to a
 *  collection of other apps, but want to group the sends to each app together.
 */
class GECOAPI bundle_send_map_t
{
	public:
		bundle_send_map_t(geco_nub_t & nub) :
				m_kNub(nub)
		{
		}
		geco_bundle_t & operator[](const sockaddrunion & addr);
		void SendAll();

	private:
		geco_nub_t &m_kNub;

		typedef std::map<sockaddrunion, geco_channel_t*> Channels;
		Channels m_kChannels;
};

/////////////////////////////////////// packet ends ////////////////////////////////////

#endif
