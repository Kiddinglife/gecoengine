
#ifndef __GUARDER_NET_TYPES_H__
#define __GUARDER_NET_TYPES_H__

#include "network/end-point.h"
#include "common/ds/geco-bit-stream.h"
#include "common/ds/eastl/EASTL/set.h"

#if !defined( PLAYSTATION3 )
#include <signal.h>
#endif

#if !defined( _WIN32 ) && !defined( PLAYSTATION3 )
# include <sys/types.h>
// getpwuid函数是通过用户的uid查找用户的passwd数据。
// 如果出错时，它们都返回一个空指针并设置errno的值，
// 用户可以根据perror函数查看出错的信息。
# include <pwd.h>
#else
#define SIGINT 2
#define SIGQUIT 3
#define SIGUSR1 10
#endif

class guarder_msg_t;

/**
 *  This class represents a packet containing GuarderMessage.  The only
 *  packet level data is the packet flags and the buddy field.  All other data
 *  (including sequence numbers) is contained in the messages themselves.
 */
class GECOAPI guarder_packet_t
{
public:
	const static int MAX_SIZE = 65535;
	static char recvbuf[MAX_SIZE];
	const static uchar PACKET_STAGGER_REPLIES = 0x1;
	typedef std::vector< guarder_msg_t* > guarder_msgs_t;
	uint	m_uiFlags;
	GecoNetAddress m_uiBuddy;
	guarder_msgs_t	m_kMessages;
	// The buddy that is automatically inserted into replies
	static GecoNetAddress  ms_uiBuddy;
	// The set of addresses we've heard from, and expect to
	static eastl::set<GecoNetAddress, geco_net_addr_cmp_functor> replied, waiting;
protected:
	// Each message is tagged with a flag of whether or not to delete it when this packet is destroyed
	std::vector< bool > m_kDelInfo;
	// If this is true then nothing is cleaned up when this packet is destroyed
	bool	dont_delete_msgs_;
public:
	guarder_packet_t();
	guarder_packet_t(geco_bit_stream_t &is);
	~guarder_packet_t();
	void add(guarder_msg_t* msg, bool should_delete = false);
	// Machined's use this to set a buddy field that will be written into every
	// reply message instead of the buddy that is actually on the message
	static void set_buddy(GecoNetAddress addr) { ms_uiBuddy = addr; }
	void steal_msgs() { dont_delete_msgs_ = true; }
	void read(geco_bit_stream_t &is);
	/// Write the contents of PACKET to the output stream to send. 
	/// @returns True on success, False on error.
	bool write(geco_bit_stream_t &os) const;
};

struct guarder_input_msg_handler_t;
class GECOAPI guarder_msg_t
{
public:
	enum Message : uint
	{
		// tool/server to guarder messages
		MACHINE_MESSAGE = 1,
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
		HIGH_PRECISION_MESSAGE = 14,
		// guarder -> guarder messages
		GUARDER_ANNOUNCE_MESSAGE = 64,
	};
	enum Flags : uint
	{
		MESSAGE_DIRECTION_OUTGOING = 0x1,
		MESSAGE_NOT_UNDERSTOOD = 0x2
	};
public:
	uint	m_uiMessage;
	uint m_uiFlags;
protected:
	static char ms_acBuf[1024];	// Scratchpad for derived class implementations of c_str()
	static ushort ms_uiSeqTicker; 	// The sequence number ticker
	// Flag indicating whether the packet has already been sent with its current
	// sequence number.  We use this to automatically generate new sequence
	// numbers for messages that are stored and sent multiple times.
	bool m_bSeqSent;
private:
	uint m_uiSeq; // this is generally set by the sender
public:
	guarder_msg_t() {};
	guarder_msg_t(uchar message, uchar flags = 0, ushort seq = 0);
	virtual ~guarder_msg_t() {}
	void outgoing(bool b) { this->flags(MESSAGE_DIRECTION_OUTGOING, b); }
	bool outgoing() const { return this->flags(MESSAGE_DIRECTION_OUTGOING); }
	void flags(uint flag, bool status) { status ? m_uiFlags |= flag : m_uiFlags &= ~flag; }
	bool flags(uint flag) const { return (m_uiFlags & flag) ? true : false; }
	const char* c_str() const;
	const char* type2str() const;
	virtual uint write(geco_bit_stream_t & os);
	virtual void read(geco_bit_stream_t &is);
	// These method are used to read and write extension data that has been
	// added after the initial version. This is particularly for ProcessMessage
	// as it has messages derived from it.
	//virtual int write_extra(geco_bit_stream_t &os) { return 0; }
	//virtual void read_extra(geco_bit_stream_t &is) {}
	// Create an MGM from the provided stream (this does a 'new' so you must delete later).
	static guarder_msg_t *from(geco_bit_stream_t &is);
	// Same as above, but creates a geco_bit_stream_t for you before passing to ^^^
	static guarder_msg_t *from(void *buf, int length);
	// Get a fresh sequence number
	void refresh_seq();
	// Copy sequence number from another message (usually used for tagging
	// pre-generated replies with the seq of an incoming message)
	void copy_seq(const guarder_msg_t &mgm);
	// Wrap this message in an MGMPacket and send it (i.e. does all the
	// MGMPacket and BinaryStream management for you)
	bool sendto(GecoNetEndpoint &ep, GecoNetAddress& addr, uint packFlags);
	GecoNetReason send_and_recv(const char* bindipaddr, GecoNetAddress& dest, guarder_input_msg_handler_t *pHandler = NULL);
	/**
	*	This method sends this MachineGuardMessage message to the guarder at the
	*	input address. The reply messages received are handled by the provided
	*	handler.
	*
	*	Note: If sending to BROADCAST, REASON_TIMER_EXPIRED will be returned if not
	*	all bwmachined daemons reply, even if some are successful.
	*/
	GecoNetReason send_and_recv(GecoNetEndpoint &ep, GecoNetAddress& dest, guarder_input_msg_handler_t *pHandler = NULL);
	/**
	*  This method does and send and recv from the same IP address as the provided
	*  endpoint, but creates a new socket, guaranteeing that it will be on a
	*  different port.
	*/
	GecoNetReason send_and_recv_with_endpoint_address(GecoNetEndpoint & ep, GecoNetAddress& dest, guarder_input_msg_handler_t * pHandler = NULL);
};

// Network statistics for a single interface.
struct high_precision_msg_t : public guarder_msg_t
{
	virtual uint write(geco_bit_stream_t &os);
	virtual void read(geco_bit_stream_t &is);
};

// A WholeMachineMessage provides info about a machine, such as CPU load,
// memory load, network throughput, and other system level stuff.
struct machine_msg_t : public guarder_msg_t
{

};
struct process_msg_t : public guarder_msg_t
{
	enum Type :uint
	{
		FIND = 0,
		REGISTER = 1,
		DEREGISTER = 2,
		NOTIFY_BIRTH = 3,
		NOTIFY_DEATH = 4
	};
	enum Param : uint
	{
		PARAM_USE_CATEGORY = 0x1,
		PARAM_USE_UID = 0x2,
		PARAM_USE_PID = 0x4,
		PARAM_USE_PORT = 0x8,
		PARAM_USE_ID = 0x10,
		PARAM_USE_NAME = 0x20,
		PARAM_IS_MSGTYPE = 0x80
	};
	enum Category : uint
	{
		SERVER_COMPONENT = 0,
		WATCHER_NUB = 1
	};
	uint m_uiParam;
	uint m_uiCategory;
	uint m_uiUID;
	uint m_uiPID;
	uint m_uiPort;
	uint m_uiID;
	char m_kName[256];
	process_msg_t();
	virtual ~process_msg_t();
	virtual uint write(geco_bit_stream_t &os);
	virtual void read(geco_bit_stream_t &is);
	virtual const char *c_str() const;
	bool matches(const process_msg_t &query) const;
	const char *cate2str(uint category) const;
	process_msg_t & operator<<(const process_msg_t &pm);
};

struct process_stats_msg_t : public process_msg_t
{

};

struct listenner_msg_t : public process_msg_t
{
	enum Type
	{
		ADD_BIRTH_LISTENER = 0,
		ADD_DEATH_LISTENER = 1
	};
	static const ushort ANY_UID = 0xffff;
	char m_kPreAddr[128];
	char m_kPostAddr[128];
	listenner_msg_t();
	virtual ~listenner_msg_t();
	virtual uint write(geco_bit_stream_t &os);
	virtual void read(geco_bit_stream_t &is);
	virtual const char *c_str() const;
};
struct create_msg_t : public guarder_msg_t
{

};
struct signal_msg_t : public guarder_msg_t
{

};
struct tag_msg_t : public guarder_msg_t
{

};
struct user_msg_t : public guarder_msg_t
{

};
struct pid_msg_t : public guarder_msg_t
{

};
struct reset_msg_t : public guarder_msg_t
{

};
struct error_msg_t : public guarder_msg_t
{

};
struct announce_msg_t : public guarder_msg_t
{
};

// Used to query guarder for the address of a specific interface.
struct GECOAPI query_interface_msg_t : public guarder_msg_t
{
	enum { INTERNAL = 0x0 };
	GecoNetAddress address_;	//!< Address of the requesting interface

	query_interface_msg_t();
	virtual ~query_interface_msg_t() {}
	virtual uint write(geco_bit_stream_t & os);
	virtual void read(geco_bit_stream_t &is);
	virtual const char *c_str() const;
};

struct create_with_args_msg_t : public guarder_msg_t
{
};
/**
* Calling contexts that are interested in the content of MGM replies need
* to subclass this and implement whichever of the handlers they are
* interested in.  Each handler should return true to keep processing the
* packet or false to terminate processing early.
*/
struct  guarder_input_msg_handler_t
{
	virtual ~guarder_input_msg_handler_t() {};
	bool handle(guarder_msg_t& reply, GecoNetAddress& srcaddr);
	virtual bool onUnhandledMsg(guarder_msg_t &mgm, GecoNetAddress& addr)
	{
		network_logger()->error("MGM::guarder_input_msg_handler_t: Unhandled message from %s: %s\n", addr.c_str(), mgm.c_str());
		return true;
	}
	virtual bool onHighPrecisionMachineMessage(high_precision_msg_t &pm, GecoNetAddress& addr) { return onUnhandledMsg(pm, addr); }
	virtual bool onWholeMachineMessage(machine_msg_t &pm, GecoNetAddress& addr) { return onUnhandledMsg(pm, addr); }
	virtual bool onProcessMessage(process_msg_t &pm, GecoNetAddress& addr) { return onUnhandledMsg(pm, addr); }
	virtual bool onProcessStatsMessage(process_stats_msg_t &pm, GecoNetAddress& addr) { return onUnhandledMsg(pm, addr); }
	virtual bool onListenerMessage(listenner_msg_t &pm, GecoNetAddress& addr) { return onUnhandledMsg(pm, addr); }
	virtual bool onCreateMessage(create_msg_t &pm, GecoNetAddress& addr) { return onUnhandledMsg(pm, addr); }
	virtual bool onSignalMessage(signal_msg_t &pm, GecoNetAddress& addr) { return onUnhandledMsg(pm, addr); }
	virtual bool onTagsMessage(tag_msg_t &pm, GecoNetAddress& addr) { return onUnhandledMsg(pm, addr); }
	virtual bool onUserMessage(user_msg_t &pm, GecoNetAddress& addr) { return onUnhandledMsg(pm, addr); }
	virtual bool onPidMessage(pid_msg_t &pm, GecoNetAddress& addr) { return onUnhandledMsg(pm, addr); }
	virtual bool onResetMessage(reset_msg_t &pm, GecoNetAddress& addr) { return onUnhandledMsg(pm, addr); }
	virtual bool onErrorMessage(error_msg_t &pm, GecoNetAddress& addr) { return onUnhandledMsg(pm, addr); }
	virtual bool onMachinedAnnounceMessage(announce_msg_t &pm, GecoNetAddress& addr) { return onUnhandledMsg(pm, addr); }
	virtual bool onQueryInterfaceMessage(query_interface_msg_t &pm, GecoNetAddress& addr) { return onUnhandledMsg(pm, addr); }
	virtual bool onCreateWithArgsMessage(create_with_args_msg_t &pm, GecoNetAddress& addr) { return onUnhandledMsg(pm, addr); }
};
#endif // __GUARDER_DECLARATIONS_H__
