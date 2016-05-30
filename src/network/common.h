/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2016 KBEngine.

KBEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

KBEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KBE_NETWORK_COMMON_H
#define KBE_NETWORK_COMMON_H

// common include
#include "common/common.h"
#include "common/debug_option.h"

namespace GecoEngine {
    namespace Network
    {
        const uint32 BROADCAST = 0xFFFFFFFF;
        const uint32 LOCALHOST = 0x0100007F;
        const extern char *IPV6_LOOPBACK = "::1";
        const extern char *IPV4_LOOPBACK = "127.0.0.1";

        // 消息的ID
        typedef uint16	MessageID;

        // 消息长度，目前长度有2种，默认消息长度最大MessageLength
        // 当超过这个数时需要扩展长度，底层使用MessageLength1
        typedef uint16	MessageLength;		// 最大65535
        typedef uint32	MessageLength1;		// 最大4294967295

        typedef int32	ChannelID;
        const ChannelID CHANNEL_ID_NULL = 0;

        // 通道超时时间
        extern float g_channelInternalTimeout;
        extern float g_channelExternalTimeout;

        // 通道发送超时重试
        extern uint32 g_intReSendInterval;
        extern uint32 g_intReSendRetries;
        extern uint32 g_extReSendInterval;
        extern uint32 g_extReSendRetries;

        // 外部通道加密类别
        extern int8 g_channelExternalEncryptType;

        // listen监听队列最大值
        extern uint32 g_SOMAXCONN;

        // 不做通道超时检查
#define CLOSE_CHANNEL_INACTIVITIY_DETECTION()										\
                                {																					\
	Network::g_channelExternalTimeout = Network::g_channelInternalTimeout = -1.0f;	\
                                }																					\


        namespace udp{
        }

        namespace tcp{
        }

        // 加密额外存储的信息占用字节(长度+填充)
#define ENCRYPTTION_WASTAGE_SIZE			(1 + 7)

#define PACKET_MAX_SIZE						1500
#ifndef PACKET_MAX_SIZE_TCP
#define PACKET_MAX_SIZE_TCP					1460
#endif
#define PACKET_MAX_SIZE_UDP					1472

        typedef uint16								PacketLength;				// 最大65535
#define PACKET_LENGTH_SIZE					sizeof(PacketLength)

#define NETWORK_MESSAGE_ID_SIZE				sizeof(Network::MessageID)
#define NETWORK_MESSAGE_LENGTH_SIZE			sizeof(Network::MessageLength)
#define NETWORK_MESSAGE_LENGTH1_SIZE		sizeof(Network::MessageLength1)
#define NETWORK_MESSAGE_MAX_SIZE			65535
#define NETWORK_MESSAGE_MAX_SIZE1			4294967295

        // 游戏内容可用包大小
#define GAME_PACKET_MAX_SIZE_TCP			PACKET_MAX_SIZE_TCP - NETWORK_MESSAGE_ID_SIZE - \
											NETWORK_MESSAGE_LENGTH_SIZE - ENCRYPTTION_WASTAGE_SIZE

        /** kbe machine端口 */
#define KBE_PORT_START						20000
#define KBE_MACHINE_BROADCAST_SEND_PORT		KBE_PORT_START + 86			// machine接收广播的端口
#define KBE_PORT_BROADCAST_DISCOVERY		KBE_PORT_START + 87
#define KBE_MACHINE_TCP_PORT				KBE_PORT_START + 88

#define KBE_INTERFACES_TCP_PORT				30099

        /*
            网络消息类型， 定长或者变长。
            如果需要自定义长度则在NETWORK_INTERFACE_DECLARE_BEGIN中声明时填入长度即可。
            */
#ifndef NETWORK_FIXED_MESSAGE
#define NETWORK_FIXED_MESSAGE 0
#endif

#ifndef NETWORK_VARIABLE_MESSAGE
#define NETWORK_VARIABLE_MESSAGE -1
#endif

        // 网络消息类别
        enum NETWORK_MESSAGE_TYPE
        {
            NETWORK_MESSAGE_TYPE_COMPONENT = 0,	// 组件消息
            NETWORK_MESSAGE_TYPE_ENTITY = 1,	// entity消息
        };

        enum ProtocolType
        {
            PROTOCOL_TCP = 0,
            PROTOCOL_UDP = 1,
        };

        enum Reason
        {
            REASON_SUCCESS = 0,				 // No reason.
            REASON_TIMER_EXPIRED = -1,		 // Timer expired.
            REASON_NO_SUCH_PORT = -2,		 // Destination port is not open.
            REASON_GENERAL_NETWORK = -3,	 // The network is stuffed.
            REASON_CORRUPTED_PACKET = -4,	 // Got a bad packet.
            REASON_NONEXISTENT_ENTRY = -5,	 // Wanted to call a null function.
            REASON_WINDOW_OVERFLOW = -6,	 // Channel send window overflowed.
            REASON_INACTIVITY = -7,			 // Channel inactivity timeout.
            REASON_RESOURCE_UNAVAILABLE = -8,// Corresponds to EAGAIN
            REASON_CLIENT_DISCONNECTED = -9, // Client disconnected voluntarily.
            REASON_TRANSMIT_QUEUE_FULL = -10,// Corresponds to ENOBUFS
            REASON_CHANNEL_LOST = -11,		 // Corresponds to channel lost
            REASON_SHUTTING_DOWN = -12,		 // Corresponds to shutting down app.
            REASON_WEBSOCKET_ERROR = -13,    // html5 error.
            REASON_CHANNEL_CONDEMN = -14,	 // condemn error.
        };

        inline
            const char * reasonToString(Reason reason)
        {
            const char * reasons[] =
            {
                "REASON_SUCCESS",
                "REASON_TIMER_EXPIRED",
                "REASON_NO_SUCH_PORT",
                "REASON_GENERAL_NETWORK",
                "REASON_CORRUPTED_PACKET",
                "REASON_NONEXISTENT_ENTRY",
                "REASON_WINDOW_OVERFLOW",
                "REASON_INACTIVITY",
                "REASON_RESOURCE_UNAVAILABLE",
                "REASON_CLIENT_DISCONNECTED",
                "REASON_TRANSMIT_QUEUE_FULL",
                "REASON_CHANNEL_LOST",
                "REASON_SHUTTING_DOWN",
                "REASON_WEBSOCKET_ERROR",
                "REASON_CHANNEL_CONDEMN"
            };

            unsigned int index = -reason;

            if (index < sizeof(reasons) / sizeof(reasons[0]))
            {
                return reasons[index];
            }

            return "REASON_UNKNOWN";
        }


#define NETWORK_SEND_TO_ENDPOINT(ep, op, pPacket)															\
                                {																											\
	int retries = 0;																						\
	Network::Reason reason;																					\
																											\
                                                                                                                                                                                                                                                                                                                                                                                                                                                	while(true)																								\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        	{																										\
		retries++;																							\
		int slen = ep->op(pPacket->data(), pPacket->totalSize());											\
																											\
		if(slen != (int)pPacket->totalSize())																\
                                		{																									\
			reason = Network::PacketSender::checkSocketErrors(ep, slen, pPacket->totalSize());				\
			/* 如果发送出现错误那么我们可以继续尝试一次， 超过3次退出	*/										\
			if (reason == Network::REASON_NO_SUCH_PORT && retries <= 3)										\
                                                			{																								\
				continue;																					\
                                                			}																								\
																											\
			/* 如果系统发送缓冲已经满了，则我们等待10ms	*/													\
			if ((reason == REASON_RESOURCE_UNAVAILABLE || reason == REASON_GENERAL_NETWORK)					\
															&& retries <= 3)								\
                                                			{																								\
				WARNING_MSG(fmt::format("{}: "																\
					"Transmit queue full, waiting for space... ({})\n",										\
					__FUNCTION__, retries));																\
																											\
				KBEngine::sleep(10);																		\
				continue;																					\
                                                			}																								\
																											\
			if(retries > 3 && reason != Network::REASON_SUCCESS)											\
                                                			{																								\
				ERROR_MSG(fmt::format("NETWORK_SEND::send: packet discarded(reason={}).\n",					\
															(reasonToString(reason))));						\
				break;																						\
                                                			}																								\
                                		}																									\
                                                                                                                        		else																								\
                                		{																									\
			break;																							\
                                		}																									\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        	}																										\
																											\
                                }																											\


#define SEND_BUNDLE_COMMON(SND_FUNC, BUNDLE)																\
	BUNDLE.finiMessage();																					\
																											\
	Network::Bundle::Packets::iterator iter = BUNDLE.packets().begin();										\
	for (; iter != BUNDLE.packets().end(); ++iter)															\
                	{																										\
		Packet* pPacket = (*iter);																			\
		int retries = 0;																					\
		Reason reason;																						\
		pPacket->sentSize = 0;																				\
																											\
                                                                                                                                                                                                                                                                                                                                                                                                                                                		while(true)																							\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        		{																									\
			++retries;																						\
			int slen = SND_FUNC;																			\
																											\
			if(slen > 0)																					\
				pPacket->sentSize += slen;																	\
																											\
			if(pPacket->sentSize != pPacket->length())														\
                                                			{																								\
				reason = PacketSender::checkSocketErrors(&ep);												\
				/* 如果发送出现错误那么我们可以继续尝试一次， 超过60次退出	*/									\
				if (reason == REASON_NO_SUCH_PORT && retries <= 3)											\
                                                                				{																							\
					continue;																				\
                                                                				}																							\
																											\
				/* 如果系统发送缓冲已经满了，则我们等待10ms	*/												\
				if ((reason == REASON_RESOURCE_UNAVAILABLE || reason == REASON_GENERAL_NETWORK)				\
																					&& retries <= 60)		\
                                                                				{																							\
					WARNING_MSG(fmt::format("{}: "															\
						"Transmit queue full, waiting for space... ({})\n",									\
						__FUNCTION__, retries));															\
																											\
					ep.waitSend();																			\
					continue;																				\
                                                                				}																							\
																											\
				if(retries > 60 && reason != REASON_SUCCESS)												\
                                                                				{																							\
					ERROR_MSG(fmt::format("Bundle::basicSendWithRetries: packet discarded(reason={}).\n",	\
															(reasonToString(reason))));						\
					break;																					\
                                                                				}																							\
                                                			}																								\
                                                                                                                                                                                    			else																							\
                                			{																								\
				break;																						\
                                			}																								\
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        		}																									\
																											\
                	}																										\
																											\
	BUNDLE.clearPackets();																					\
																											\


#define SEND_BUNDLE(ENDPOINT, BUNDLE)																		\
                                {																											\
	EndPoint& ep = ENDPOINT;																				\
	SEND_BUNDLE_COMMON(ENDPOINT.send(pPacket->data() + pPacket->sentSize,									\
	pPacket->length() - pPacket->sentSize), BUNDLE);														\
                                }																											\


#define SENDTO_BUNDLE(ENDPOINT, ADDR, PORT, BUNDLE)															\
                                {																											\
	EndPoint& ep = ENDPOINT;																				\
	SEND_BUNDLE_COMMON(ENDPOINT.sendto(pPacket->data() + pPacket->sentSize,									\
	pPacket->length() - pPacket->sentSize, PORT, ADDR), BUNDLE);											\
                                }																											\

#define MALLOC_PACKET(outputPacket, isTCPPacket)															\
                                {																											\
	if(isTCPPacket)																							\
		outputPacket = TCPPacket::createPoolObject();														\
                                                            	else																									\
		outputPacket = UDPPacket::createPoolObject();														\
                                }																											\


#define RECLAIM_PACKET(isTCPPacket, pPacket)																\
                                {																											\
	if(isTCPPacket)																							\
		TCPPacket::reclaimPoolObject(static_cast<TCPPacket*>(pPacket));										\
                                                            	else																									\
		UDPPacket::reclaimPoolObject(static_cast<UDPPacket*>(pPacket));										\
                                }																											\


        // 配合服务端配置选项trace_packet使用，用来跟踪一条即将输出的消息包
#define TRACE_MESSAGE_PACKET(isrecv, pPacket, pCurrMsgHandler, length, addr, readPacketHead)				\
	if(Network::g_trace_packet > 0)																			\
                	{																										\
		if(Network::g_trace_packet_use_logfile)																\
			DebugHelper::getSingleton().changeLogger("packetlogs");											\
																											\
		size_t headsize = 0;																				\
		if(pCurrMsgHandler && readPacketHead)																\
                                		{																									\
			headsize = NETWORK_MESSAGE_ID_SIZE;																\
			if (pCurrMsgHandler->msgLen == NETWORK_VARIABLE_MESSAGE || 										\
				g_packetAlwaysContainLength)																\
                                                			{																								\
				headsize += NETWORK_MESSAGE_LENGTH_SIZE;													\
				if (length >= NETWORK_MESSAGE_MAX_SIZE)														\
					headsize += NETWORK_MESSAGE_LENGTH1_SIZE;												\
                                                			}																								\
                                		}																									\
																											\
		bool isprint = true;																				\
		if(pCurrMsgHandler)																					\
                                		{																									\
			std::vector<std::string>::iterator iter = std::find(Network::g_trace_packet_disables.begin(),	\
														Network::g_trace_packet_disables.end(),				\
															pCurrMsgHandler->name);							\
																											\
			if(iter != Network::g_trace_packet_disables.end())												\
                                                			{																								\
				isprint = false;																			\
                                                			}																								\
                                                                                                                                                                                    			else																							\
                                			{																								\
				DEBUG_MSG(fmt::format("{} {}:msgID:{}, currMsgLength:{}, addr:{}\n",						\
						((isrecv == true) ? "====>" : "<===="),												\
						pCurrMsgHandler->name.c_str(),														\
						pCurrMsgHandler->msgID,																\
						(length + headsize),																\
						addr));																				\
                                			}																								\
                                		}																									\
																											\
		if(isprint)																							\
                                		{																									\
																											\
			size_t rpos = pPacket->rpos();																	\
			if(headsize > 0)																				\
				pPacket->rpos(pPacket->rpos() - headsize);													\
																											\
			switch(Network::g_trace_packet)																	\
                                			{																								\
			case 1:																							\
				pPacket->hexlike();																			\
				break;																						\
			case 2:																							\
				pPacket->textlike();																		\
				break;																						\
			default:																						\
				pPacket->print_storage();																	\
				break;																						\
                                			};																								\
																											\
			pPacket->rpos(rpos);																			\
                                		}																									\
																											\
		if(Network::g_trace_packet_use_logfile)																\
			DebugHelper::getSingleton().changeLogger(COMPONENT_NAME_EX(g_componentType));					\
                	}																										\


        void destroyObjPool();

        // network stats
        extern uint64						g_numPacketsSent;
        extern uint64						g_numPacketsReceived;
        extern uint64						g_numBytesSent;
        extern uint64						g_numBytesReceived;

        // 包接收窗口溢出
        extern uint32						g_receiveWindowMessagesOverflowCritical;
        extern uint32						g_intReceiveWindowMessagesOverflow;
        extern uint32						g_extReceiveWindowMessagesOverflow;
        extern uint32						g_intReceiveWindowBytesOverflow;
        extern uint32						g_extReceiveWindowBytesOverflow;

        extern uint32						g_sendWindowMessagesOverflowCritical;
        extern uint32						g_intSendWindowMessagesOverflow;
        extern uint32						g_extSendWindowMessagesOverflow;
        extern uint32						g_intSendWindowBytesOverflow;
        extern uint32						g_extSendWindowBytesOverflow;

        bool initializeWatcher();
        void finalise(void);

        /// Network address for a system Corresponds to a network address
        /// This is not necessarily a unique identifier. For example, if a system has both LAN and
        /// WAN connections, the system may be identified by either one, depending on who is
        /// communicating Therefore, you should not transmit the JACKIE_INET_Address over the 
        /// network and expect it to  identify a system, or use it to connect to that system, except 
        /// in the case where that system is not behind a NAT (such as with a dedciated server)
        /// Use JACKIE_INet_GUID for a unique per-instance of ServerApplication to identify 
        /// systems
        struct  network_address_t
        {
            /// In6 Or In4 
            /// JACKIE_INET_Address, with RAKNET_SUPPORT_IPV6 defined, 
            /// holds both an sockaddr_in6 and a sockaddr_in
            union
            {
#if NET_SUPPORT_IPV6 ==1
                struct sockaddr_storage sa_stor;
                sockaddr_in6 addr6;
#else
                sockaddr_in addr4;
#endif
                sockaddr addr;
            } address;

            /// @internal Used internally for fast lookup. 
            /// @optional (use -1 to do regular lookup). Don't transmit this.
            system_index_t systemIndex;

            /// This is not used internally, but holds a copy of the port held in the address union,
            /// so for debugging it's easier to check what port is being held
            ushort debugPort;

            /// Constructors
            network_address_t();
            network_address_t(const char *str);
            network_address_t(const char *str, ushort port);

            network_address_t& operator = (const network_address_t& input);
            bool operator==(const network_address_t& right) const;
            bool operator!=(const network_address_t& right) const;
            bool operator > (const network_address_t& right) const;
            bool operator < (const network_address_t& right) const;
            bool EqualsExcludingPort(const network_address_t& right) const;
            /// @internal Return the size to write to a bitStream
            static int size(void);

            /// Hash the JACKIE_INET_Address
            static unsigned int ToHashCode(const network_address_t &sa);

            /// Return the IP version, either IPV4 or IPV6
            unsigned char GetIPVersion(void) const;

            /// @internal Returns either IPPROTO_IP or IPPROTO_IPV6
            unsigned char GetIPProtocol(void) const;

            /// Returns the port in host order (this is what you normally use)
            ushort GetPortHostOrder(void) const;

            /// @internal Returns the port in network order
            ushort GetPortNetworkOrder(void) const;

            /// Sets the port. The port value should be in host order (this is what you normally use)
            /// Renamed from SetPort because of winspool.h http://edn.embarcadero.com/
            /// article/21494
            void SetPortHostOrder(ushort s);

            /// @internal Sets the port. The port value should already be in network order.
            void SetPortNetworkOrder(ushort s);

            /// set the port from another JACKIE_INET_Address structure
            void SetPortNetworkOrder(const network_address_t& right);

            void FixForIPVersion(const network_address_t &boundAddressToSocket)
            {
                char str[128];
                ToString(false, str);
                // TODO - what about 255.255.255.255?
                if (strcmp(str, IPV6_LOOPBACK) == 0)
                {
                    if (boundAddressToSocket.GetIPVersion() == 4)
                    {
                        FromString(IPV4_LOOPBACK, (char)0, (uchar)4);
                    }
                }
                else if (strcmp(str, IPV4_LOOPBACK) == 0)
                {
#if NET_SUPPORT_IPV6==1
                    if (boundAddressToSocket.GetIPVersion() == 6)
                    {
                        FromString(IPV6_LOOPBACK, (char)0, (uchar)6);
                    }
#endif
                }
            }

            /// Call SetToLoopback(), with whatever IP version is currently held. Defaults to IPV4
            void SetToLoopBack(void);

            /// Call SetToLoopback() with a specific IP version
            /// @param[in ipVersion] Either 4 for IPV4 or 6 for IPV6
            void SetToLoopBack(unsigned char ipVersion);

            /// \return If was set to 127.0.0.1 or ::1
            bool IsLoopback(void) const;
            bool IsLANAddress(void);

            /// Old version, for crap platforms that don't support newer socket functions
            bool SetIP4Address(const char *str, char portDelineator = ':');

            /// Set the system address from a printable IP string, for example "192.0.2.1" or 
            /// "2001:db8:63b3:1::3490"  You can write the port as well, using the portDelineator, for 
            /// example "192.0.2.1|1234"
            //
            /// @param[in, str] A printable IP string, for example "192.0.2.1" or "2001:db8:63b3:1::3490". 
            /// Pass 0 for \a str to set to JACKIE_INET_Address_Null
            //
            /// @param[in, portDelineator] if \a str contains a port, delineate the port with this 
            /// character. portDelineator should not be '.', ':', '%', '-', '/', a number, or a-f
            // 
            /// @param[in, ipVersion] Only used if str is a pre-defined address in the wrong format, 
            /// such  as 127.0.0.1 but you want ip version 6, so you can pass 6 here to do the conversion
            //
            /// @note The current port is unchanged if a port is not specified in \a str
            /// @return True on success, false on ipVersion does not match type of passed string
            bool FromString(const char *str, char portDelineator = '|', unsigned char ipVersion = 0);

            /// Same as FromString(), but you explicitly set a port at the same time
            bool FromString(const char *str, ushort port, unsigned char ipVersion = 0);

            // Return the systemAddress as a string in the format <IP>|<Port>
            // Returns a static string
            // NOT THREADSAFE
            // portDelineator should not be [.] [:] [%] [-] [/] [number] [a-z]
            const char *ToString(bool writePort = true, char portDelineator = '|') const;

            // Return the systemAddress as a string in the format <IP>|<Port>
            // dest must be large enough to hold the output
            // portDelineator should not be [.] [:] [%] [-] [/] [number] [a-z]
            // THREADSAFE
            void ToString(bool writePort, char *dest, char portDelineator = '|') const;

            /// @internal 
            void ToString_IPV4(bool writePort, char *dest, char portDelineator = ':') const;
            void ToString_IPV6(bool writePort, char *dest, char portDelineator) const;
        };
    }
}

#endif // KBE_NETWORK_COMMON_H
