/*
 * msg_macros.h
 *
 *  Created on: 14Nov.,2016
 *      Author: jackiez
 */

#ifndef SRC_NETWORK_MSG_MACROS_H_
#define SRC_NETWORK_MSG_MACROS_H_

#endif /* SRC_NETWORK_MSG_MACROS_H_ */

// avoid repeat deines
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

#define HANDLER_STATEMENT(NAME,TYPE,ARG) TYPE gHandler_##NAME(ARG);
#define NULL_IF_NOT_SERVER(ARG) ARG

#else
#define HANDLER_STATEMENT(NAME,TYPE,ARG)
#define HANDLER_ARGUMENT(NAME) NULL
#define NULL_IF_NOT_SERVER(ARG) NULL
#endif

#ifdef DEFINE_INTERFACE_HERE
#undef DEFINE_INTERFACE_HERE

#define BEGIN_MERCURY_INTERFACE( INAME ) 						    \
namespace INAME { 													\
	FvNetInterfaceMinder gMinder( #INAME );						    \
	void RegisterWithNub( FvNetNub & nub )						    \
	{																\
		gMinder.RegisterWithNub( nub );								\
	}																\
	FvNetReason RegisterWithMachined( FvNetNub & nub, int id )      \
	{																\
		return gMinder.RegisterWithMachined( nub, id );				\
	}

#define END_MERCURY_INTERFACE()	}

#define MERCURY_MESSAGE(NAME,STYLE,PARAM,HANDLER)\
const msg_handler_t& NAME = gMinder.Add(#NAME,STYLE,PARAM,NULL_IF_NOT_SERVER(HANDLER));

#define MERCURY_STRUCT_MESSAGE( NAME, HANDLER )							\
		MERCURY_MESSAGE(NAME,NET_FIXED_LENGTH_MESSAGE,sizeof(struct NAME##Args),HANDLER )\
		geco_bit_stream_t& operator<<( geco_bit_stream_t& b,const struct NAME##Args &s )\
		{																	\
			b.StartMessage( NAME );											\
			(*(FvBinaryOStream*)( &b )) << s;								\
			return b;														\
		}																	\
		struct __Garbage__##NAME##Args
#else
#define GECO_ISTREAM( NAME, XSTREAM ) geco_bit_stream_t& operator >>(geco_bit_stream_t &in,NAME##Args &x);
#define GECO_OSTREAM( NAME, XSTREAM ) geco_bit_stream_t& operator <<(geco_bit_stream_t &out,const NAME##Args &x);
#endif
