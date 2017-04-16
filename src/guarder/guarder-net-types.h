
#ifndef __GUARDER_NET_TYPES_H__
#define __GUARDER_NET_TYPES_H__

#include "network/end-point.h"
#include "common/ds/geco-bit-stream.h"

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
	const static int MAX_SIZE = 32768;
	const static uchar PACKET_STAGGER_REPLIES = 0x1;
	typedef std::vector< guarder_msg_t* > guarder_msgs_t;
	uint	m_uiFlags;
	sockaddrunion m_uiBuddy;
	guarder_msgs_t	m_kMessages;
	// The buddy that is automatically inserted into replies
	static sockaddrunion  ms_uiBuddy;
protected:
	// Each message is tagged with a flag of whether or not to delete it when this packet is destroyed
	std::vector< bool > m_kDelInfo;
	// If this is true then nothing is cleaned up when this packet is destroyed
	bool	dont_delete_msgs_;
public:
	guarder_packet_t();
	guarder_packet_t(geco_bit_stream_t &is);
	~guarder_packet_t();
	void add(guarder_msg_t* msg, bool should_delete);
	// Machined's use this to set a buddy field that will be written into every
	// reply message instead of the buddy that is actually on the message
	static void set_buddy(sockaddrunion addr) { ms_uiBuddy = addr; }
	void steal_msgs() { dont_delete_msgs_ = true; }
	void read(geco_bit_stream_t &is);
	/// Write the contents of PACKET to the output stream to send. 
	/// @returns True on success, False on error.
	bool write(geco_bit_stream_t &os) const;
};

class GECOAPI guarder_msg_t
{
public:
	enum Message : uint
	{
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
		ANNOUNCE_MESSAGE = 64,
	};
	enum Flags : uint
	{
		MESSAGE_DIRECTION_OUTGOING = 0x1,
		MESSAGE_NOT_UNDERSTOOD = 0x2
	};
	uint	m_uiMessage;
	uint m_uiFlags;
protected:
	static char ms_acBuf[1024];
	static ushort ms_uiSeqTicker;
	bool m_bSeqSent;
private:
	ushort m_uiSeq;
public:
	guarder_msg_t() {};
	guarder_msg_t(uchar message, uchar flags = 0, ushort seq = 0);
	virtual ~guarder_msg_t() {}
	static guarder_msg_t *from(geco_bit_stream_t &is);
	static guarder_msg_t *from(void *buf, int length);
	const char* c_str() const;
	const char* msg_type2str() const;
	uint write(geco_bit_stream_t & os);
};
/// provides a more detailed version of the 
/// machine statistics that are provided by a MachineMessage.
class high_precision_msg_t : public guarder_msg_t
{
		
};
/// A WholeMachineMessage provides info about a machine, such as CPU load,
 /// memory load, network throughput, and other system level stuff.
class machine_msg_t : public guarder_msg_t
{

};
class process_msg_t : public guarder_msg_t
{

};
class process_stats_msg_t : public guarder_msg_t
{

};
struct listenner_msg_t : public guarder_msg_t
{

};
class create_msg_t : public guarder_msg_t
{

};
class signal_msg_t : public guarder_msg_t
{

};
class tag_msg_t : public guarder_msg_t
{

};
class user_msg_t : public guarder_msg_t
{

};
class pid_msg_t : public guarder_msg_t
{

};
class reset_msg_t : public guarder_msg_t
{

};
class error_msg_t : public guarder_msg_t
{

};
struct announce_msg_t : public guarder_msg_t
{

};
/// Used to query guarder for the address of a specific interface.
class query_interface_msg_t : public guarder_msg_t
{

};
class create_with_args_msg_t : public guarder_msg_t
{

};
#endif // __GUARDER_DECLARATIONS_H__
