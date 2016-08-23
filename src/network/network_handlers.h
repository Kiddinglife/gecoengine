/*
 * network_handlers.h
 *
 *  Created on: 22Aug.,2016
 *      Author: jackiez
 */

#ifndef SRC_NETWORK_HANDLERS_H_
#define SRC_NETWORK_HANDLERS_H_
#include <functional>

/**
 *   an interface for receiving Mercury messages.
 *  Classes that can handle general messages from Mercury needs to
 *  implement this.
 *  @ingroup network
 */

/**
 *  This method is called by event dispatcher to deliver a message.
 *  @param source   The address at which the message originated.
 *  @param data     This contains the message type, size, and flags. and  actual message data.
 */
class Address;
class geco_bit_stream_t;
typedef std::function<void(const Address & source, geco_bit_stream_t& data)> msg_handler_t;
//class InputMessageHandler
//{
//    public:
//        virtual ~InputMessageHandler()
//        {
//        }
//        ;
//
//        /**
//         *  This method is called by Mercury to deliver a message.
//         *
//         *  @param source   The address at which the message originated.
//         *  @param header   This contains the message type, size, and flags.
//         *  @param data     The actual message data.
//         */
//        virtual void handleMessage(const Address & source,
//                UnpackedMessageHeader & header,
//                BinaryIStream & data) = 0;
//};

/**
 *  This class defines an interface for receiving socket events.
 *  Since Mercury runs the event loop, it is useful to be able
 *  to register additional file descriptors, and receive callbacks
 *  when they are ready for IO.
 *
 *  @see EventDispatcher::registerFileDescriptor
 *
 *  @ingroup mercury
 */
typedef std::function<int(int fd)> network_data_arrived_handler_t;
//class InputNotificationHandler
//{
//    public:
//        virtual ~InputNotificationHandler()
//        {
//        }
//        ;
//
//        /**
//         *  This method is called when a file descriptor is ready for
//         *  reading.
//         *
//         *  @param fd   The file descriptor
//         *
//         *  @return The return value is ignored. Implementors should return 0.
//         */
//        virtual int handleInputNotification(int fd) = 0;
//};

/**
 *  This class declares an interface for receiving reply messages.
 *  When a client issues a request, an interface of this type should
 *  be provided to handle the reply.
 *
 *  @see Bundle::startRequest
 *  @see Bundle::startReply
 *
 *  @ingroup mercury
 */
class NubException;
typedef std::function<void(const Address & source, geco_bit_stream_t& data, void* args)>
reply_msg_handler_t;
typedef std::function<void(const NubException & exception, void * arg)>
reply_exception_handler_t;
typedef std::function<void(const NubException & exception, void * arg)>
reply_shutdown_handler_t;

//class ReplyMessageHandler
//{
//    public:
//        virtual ~ReplyMessageHandler()
//        {
//        }
//        ;
//
//        /**
//         *  This method is called by Mercury to deliver a reply message.
//         *
//         *  @param source   The address at which the message originated.
//         *  @param header   This contains the message type, size, and flags.
//         *  @param data     The actual message data.
//         *  @param arg      This is user-defined data that was passed in with
//         *                  the request that generated this reply.
//         */
//        virtual void handleMessage(const Address & source,
//                UnpackedMessageHeader & header,
//                BinaryIStream & data,
//                void * arg) = 0;
//
//        /**
//         *  This method is called by Mercury when the request fails. The
//         *  normal reason for this happening is a timeout.
//         *
//         *  @param exception    The reason for failure.
//         *  @param arg          The user-defined data associated with the request.
//         */
//        virtual void handleException(const NubException & exception,
//                void * arg) = 0;
//
//        virtual void handleShuttingDown(const NubException & exception,
//                void * arg)
//        {
//            WARNING_MSG( "ReplyMessageHandler::handleShuttingDown: "
//            "Not handled. Possible memory leak.\n");
//        }
//    };

#endif /* SRC_NETWORK_HANDLERS_H_ */
