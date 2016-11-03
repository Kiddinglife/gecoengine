//{future header message}
#ifndef __FvNetInterfaceMinderMacros_H__
#define __FvNetInterfaceMinderMacros_H__

#include "FvNetInterfaceMinder.h"
#include "FvNetBundle.h"
#include "FvNetMisc.h"

#define MERCURY_FIXED_MESSAGE( NAME, PARAM, HANDLER )					\
	MERCURY_MESSAGE( NAME, FV_NET_FIXED_LENGTH_MESSAGE, PARAM, HANDLER)			\

#define MERCURY_VARIABLE_MESSAGE( NAME, PARAM, HANDLER )				\
	MERCURY_MESSAGE( NAME, FV_NET_VARIABLE_LENGTH_MESSAGE, PARAM, HANDLER)		\

#define MERCURY_EMPTY_MESSAGE( NAME, HANDLER )							\
	MERCURY_MESSAGE( NAME, FV_NET_FIXED_LENGTH_MESSAGE, 0, HANDLER )			\

#define BEGIN_HANDLED_STRUCT_MESSAGE( NAME, HANDLERTYPE, HANDLERARG )	\
	MERCURY_HANDLED_STRUCT_MESSAGE( NAME, HANDLERTYPE, HANDLERARG )		\
	{																	\
		MERCURY_STRUCT_GOODIES( NAME )									\

#define END_HANDLED_STRUCT_MESSAGE()									\
	};																	\

#define BEGIN_STRUCT_MESSAGE( NAME, HANDLER )							\
	MERCURY_STRUCT_MESSAGE( NAME, HANDLER )								\
	{																	\
		MERCURY_STRUCT_GOODIES( NAME )									\

#define END_STRUCT_MESSAGE()											\
	};																	\

#define BEGIN_HANDLED_PREFIXED_MESSAGE(									\
		NAME, PREFIXTYPE, HANDLERTYPE, HANDLERARG )						\
	MERCURY_HANDLED_PREFIXED_MESSAGE(									\
		NAME, PREFIXTYPE, HANDLERTYPE, HANDLERARG )						\
	{																	\
		MERCURY_PREFIXED_GOODIES( NAME, PREFIXTYPE )					\

#define END_HANDLED_PREFIXED_MESSAGE()									\
	};																	\

#define BEGIN_PREFIXED_MESSAGE( NAME, PREFIXTYPE, HANDLER )				\
	MERCURY_PREFIXED_MESSAGE( NAME, PREFIXTYPE, HANDLER )				\
	{																	\
		MERCURY_PREFIXED_GOODIES( NAME, PREFIXTYPE )					\

#define END_PREFIXED_MESSAGE()											\
	};																	\



#define MERCURY_HANDLED_PREFIXED_EMPTY_MESSAGE(							\
			NAME, PREFIXTYPE, HANDLERTYPE, HANDLERARG )					\
	HANDLER_STATEMENT( NAME, HANDLERTYPE, HANDLERARG )					\
	MERCURY_PREFIXED_EMPTY_MESSAGE(										\
		NAME, PREFIXTYPE, HANDLER_ARGUMENT( NAME ) )					\

#define MERCURY_PREFIXED_EMPTY_MESSAGE(									\
			NAME, PREFIXTYPE, HANDLER )									\
	MERCURY_MESSAGE( NAME, FV_NET_FIXED_LENGTH_MESSAGE,					\
		sizeof(PREFIXTYPE), HANDLER )									\

#define MERCURY_HANDLED_STRUCT_MESSAGE(									\
			NAME, HANDLERTYPE, HANDLERARG )								\
	HANDLER_STATEMENT( NAME, HANDLERTYPE, HANDLERARG)					\
	MERCURY_STRUCT_MESSAGE( NAME, HANDLER_ARGUMENT( NAME ) )			\

#define MERCURY_HANDLED_PREFIXED_MESSAGE(								\
		NAME, PREFIXTYPE, HANDLERTYPE, HANDLERARG )						\
	HANDLER_STATEMENT( NAME, HANDLERTYPE, HANDLERARG )					\
	MERCURY_PREFIXED_MESSAGE( NAME, PREFIXTYPE,							\
			HANDLER_ARGUMENT( NAME ) )									\

#define MERCURY_HANDLED_VARIABLE_MESSAGE(								\
	NAME, PARAM, HANDLERTYPE, HANDLERARG )								\
	HANDLER_STATEMENT( NAME, HANDLERTYPE, HANDLERARG )					\
	MERCURY_VARIABLE_MESSAGE( NAME, PARAM, HANDLER_ARGUMENT( NAME ) )	\



#define MERCURY_STRUCT_GOODIES( NAME )									\
	static NAME##Args & start( FvNetBundle & b,				\
		FvNetReliableType reliable = MERCURY_DEFAULT_RELIABLE )		\
	{																	\
		return *(NAME##Args*)b.StartStructMessage( NAME, reliable );	\
	}																	\
	static NAME##Args & StartRequest( FvNetBundle & b,			\
		FvNetReplyMessageHandler * handler,						\
		void * arg = NULL,												\
		int timeout = FV_NET_DEFAULT_REQUEST_TIMEOUT,			\
		FvNetReliableType reliable = MERCURY_DEFAULT_RELIABLE )		\
	{																	\
		return *(NAME##Args*)b.StartStructRequest( NAME,				\
			handler, arg, timeout, reliable );							\
	}																	\

#define MERCURY_PREFIXED_GOODIES( NAME, PREFIXTYPE )					\
	static NAME##Args & start( FvNetBundle & b,				\
		const PREFIXTYPE & prefix,										\
		FvNetReliableType reliable = MERCURY_DEFAULT_RELIABLE )		\
	{																	\
		b.StartMessage( NAME, reliable );								\
		b << prefix;													\
		return *(NAME##Args*)b.Reserve( sizeof(NAME##Args) );			\
	}																	\
	static NAME##Args & StartRequest( FvNetBundle & b,			\
		const PREFIXTYPE & prefix,										\
		FvNetReplyMessageHandler * handler,						\
		void * arg = NULL,												\
		int timeout = FV_NET_DEFAULT_REQUEST_TIMEOUT,			\
		FvNetReliableType reliable = MERCURY_DEFAULT_RELIABLE )		\
	{																	\
		b.StartRequest( NAME, handler, arg, timeout, reliable );		\
		b << prefix;													\
		return *(NAME##Args*)b.Reserve( sizeof(NAME##Args) );			\
	}																	\


#endif // __FvNetInterfaceMinderMacros_H__

#ifdef BEGIN_MERCURY_INTERFACE
	#undef BEGIN_MERCURY_INTERFACE
	#undef MERCURY_MESSAGE
	#undef END_MERCURY_INTERFACE

	#undef MERCURY_STRUCT_MESSAGE
	#undef MERCURY_PREFIXED_MESSAGE

	#undef MERCURY_OSTREAM
	#undef MERCURY_ISTREAM

	#undef HANDLER_STATEMENT
	#undef HANDLER_ARGUMENT
	#undef NULL_IF_NOT_SERVER
#endif

#ifdef DEFINE_SERVER_HERE
	#undef DEFINE_SERVER_HERE

	#ifndef DEFINE_INTERFACE_HERE
		#define DEFINE_INTERFACE_HERE
	#endif

	#define HANDLER_STATEMENT(NAME,TYPE,ARG)								\
		TYPE gHandler_##NAME(ARG);
	#define HANDLER_ARGUMENT(NAME) &gHandler_##NAME
	#define NULL_IF_NOT_SERVER(ARG) ARG
#else
	#define HANDLER_STATEMENT(NAME,TYPE,ARG)
	#define HANDLER_ARGUMENT(NAME) NULL
	#define NULL_IF_NOT_SERVER(ARG) NULL
#endif

#ifdef DEFINE_INTERFACE_HERE
	#undef DEFINE_INTERFACE_HERE

	#define BEGIN_MERCURY_INTERFACE( INAME ) 								\
		namespace INAME { 													\
			FvNetInterfaceMinder gMinder( #INAME );						\
																			\
			void RegisterWithNub( FvNetNub & nub )						\
			{																\
				gMinder.RegisterWithNub( nub );								\
			}																\
																			\
			FvNetReason RegisterWithMachined( FvNetNub & nub, int id ) \
			{																\
				return gMinder.RegisterWithMachined( nub, id );				\
			}																\

	#define END_MERCURY_INTERFACE()											}

	#define MERCURY_MESSAGE( NAME, STYLE, PARAM, HANDLER)					\
			const FvNetInterfaceElement & NAME =						\
				gMinder.Add( #NAME, STYLE, PARAM,					\
						NULL_IF_NOT_SERVER(HANDLER) );

	#define MERCURY_STRUCT_MESSAGE( NAME, HANDLER )							\
		MERCURY_MESSAGE( NAME,												\
			FV_NET_FIXED_LENGTH_MESSAGE,											\
			sizeof(struct NAME##Args),										\
			HANDLER )														\
		FvNetBundle & operator<<( FvNetBundle & b,					\
			const struct NAME##Args &s )									\
		{																	\
			b.StartMessage( NAME );											\
			(*(FvBinaryOStream*)( &b )) << s;									\
			return b;														\
		}																	\
		struct __Garbage__##NAME##Args



	#define MERCURY_PREFIXED_MESSAGE( NAME, PREFIXTYPE, HANDLER)			\
		MERCURY_MESSAGE(NAME,												\
			FV_NET_FIXED_LENGTH_MESSAGE,											\
			sizeof(PREFIXTYPE)+sizeof(struct NAME##Args),					\
			HANDLER )														\
		FvNetBundle & operator<<(										\
			FvNetBundle & b,											\
			const std::pair<PREFIXTYPE, struct NAME##Args> & p )			\
		{																	\
			b.StartMessage( NAME );											\
			b << p.first;													\
			b << p.second;													\
			return b;														\
		}																	\
		struct __Garbage__##NAME##Args

#define MERCURY_ISTREAM( NAME, XSTREAM )	 								\
FvBinaryIStream& operator>>( FvBinaryIStream &is, NAME##Args &x )			\
{																			\
	return is >> XSTREAM;													\
}

#define MERCURY_OSTREAM( NAME, XSTREAM ) 									\
FvBinaryOStream& operator<<( FvBinaryOStream &os, const NAME##Args &x )		\
{																			\
	return os << XSTREAM;													\
}

#else

	#define BEGIN_MERCURY_INTERFACE( INAME )								\
		/** @internal														\
		 * @{																\
		 */																	\
		namespace INAME { 													\
			extern FvNetInterfaceMinder gMinder;						\
			void RegisterWithNub( FvNetNub & nub );						\
			FvNetReason RegisterWithMachined( FvNetNub & nub, int id );	\

	#define MERCURY_MESSAGE( NAME, STYLE, PARAM, HANDLER )					\
			/** @internal */												\
			extern const FvNetInterfaceElement & NAME;

	#define END_MERCURY_INTERFACE()											\
		/** @} */															\
		}


	#define MERCURY_STRUCT_MESSAGE( NAME, HANDLER )							\
		struct NAME##Args;													\
		MERCURY_MESSAGE( NAME, 0, 0, 0 )									\
		/** @internal */													\
		FvNetBundle & operator<<(										\
			FvNetBundle & b,											\
			const struct NAME##Args &s );									\
		/** @internal */													\
		struct NAME##Args


	#define MERCURY_PREFIXED_MESSAGE( NAME, PREFIXTYPE, HANDLER )			\
		struct NAME##Args;													\
		MERCURY_MESSAGE( NAME, 0, 0, 0 )									\
		/** @internal */													\
		FvNetBundle & operator<<(										\
			FvNetBundle & b,											\
			const std::pair<PREFIXTYPE, struct NAME##Args> & p);			\
		/** @internal */													\
		struct NAME##Args

#define MERCURY_ISTREAM( NAME, XSTREAM ) 									\
FvBinaryIStream& operator>>( FvBinaryIStream &in, NAME##Args &x );			\

#define MERCURY_OSTREAM( NAME, XSTREAM ) 									\
FvBinaryOStream& operator<<( FvBinaryOStream &out, const NAME##Args &x );	\

#endif
