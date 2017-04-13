/*
 * Geco Gaming Company
 * All Rights Reserved.
 * Copyright (c)  2016 GECOEngine.
 *
 * GECOEngine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GECOEngine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

 /*
  * networkstats.h
  *
  *  Created on: 18Nov.,2016
  *      Author: jackiez
  */

#ifndef __MSG_PARSER_GENERATOR_H__
#define __MSG_PARSER_GENERATOR_H__   //  Prevent include the folowing macros more than one time

  /* Helper macros */
#define GECO_FIXED_MESSAGE( NAME, PARAM, HANDLER )     GECO_MESSAGE( NAME, FIXED_LENGTH_MESSAGE, PARAM, HANDLER)
#define GECO_VARIABLE_MESSAGE( NAME, PARAM, HANDLER )  GECO_MESSAGE( NAME, VARIABLE_LENGTH_MESSAGE, PARAM, HANDLER)
#define GECO_EMPTY_MESSAGE( NAME, HANDLER )			   GECO_MESSAGE( NAME, FIXED_LENGTH_MESSAGE, 0, HANDLER )

#define BEGIN_STRUCT_MESSAGE( NAME, HANDLER ) \
GECO_STRUCT_MESSAGE( NAME, HANDLER) \
{
#define END_STRUCT_MESSAGE() \
};

#define BEGIN_HANDLED_STRUCT_MESSAGE( NAME, HANDLERTYPE, HANDLERARG )\
GECO_HANDLED_STRUCT_MESSAGE( NAME, HANDLERTYPE, HANDLERARG ) \
{
#define END_HANDLED_STRUCT_MESSAGE() \
};

/* msg handler macros */
#define GECO_HANDLED_STRUCT_MESSAGE(NAME, HANDLERTYPE, HANDLERARG )\
HANDLER_STATEMENT( NAME, HANDLERTYPE, HANDLERARG) \
GECO_STRUCT_MESSAGE( NAME, HANDLER_ARGUMENT( NAME ) )

#define GECO_HANDLED_PREFIXED_MESSAGE(NAME, PREFIXTYPE, HANDLERTYPE, HANDLERARG ) \
HANDLER_STATEMENT( NAME, HANDLERTYPE, HANDLERARG ) \
GECO_PREFIXED_MESSAGE( NAME, PREFIXTYPE,	HANDLER_ARGUMENT( NAME ) )									

#endif /* __MSG_PARSER_GENERATOR_H_ */

// Cleanup to prevent user accidently define same macros
#undef BEGIN_GECO_INTERFACE
#undef GECO_MESSAGE
#undef END_GECO_INTERFACE
#undef GECO_STRUCT_MESSAGE
#undef GECO_PREFIXED_MESSAGE
#undef GECO_OSTREAM
#undef GECO_ISTREAM
#undef HANDLER_STATEMENT
#undef HANDLER_ARGUMENT
#undef NULL_IF_NOT_SERVER

// when server, handler will be not null and can be called
// when client,  server's handler should be null in case it is accidently invoked
#ifdef DEFINE_SERVER_HERE
#undef DEFINE_SERVER_HERE

#ifndef DEFINE_INTERFACE_HERE
#define DEFINE_INTERFACE_HERE
#endif

#define HANDLER_STATEMENT(NAME,TYPE,ARG)	TYPE gHandler_##NAME(ARG);
#define HANDLER_ARGUMENT(NAME) &gHandler_##NAME
#define NULL_IF_NOT_SERVER(ARG) ARG

#else

#define HANDLER_STATEMENT(NAME,TYPE,ARG)
#define HANDLER_ARGUMENT(NAME) NULL
#define NULL_IF_NOT_SERVER(ARG) NULL

#endif
// Core macros

//#define DEFINE_INTERFACE_HERE
#ifdef DEFINE_INTERFACE_HERE // definitions are placed here

// dumps
#define BEGIN_GECO_ISTREAM( NAME ) \
namespace { static void dump_decoder_do_not_use_me( geco_bit_stream_t &is, dump_##NAME##_ArgsTypeDoNotUseThis &args ) \
{ 
#define END_GECO_ISTREAM() \
}}
#define BEGIN_GECO_OSTREAM( NAME ) \
namespace { static void dump_encoder_do_not_use_me( geco_bit_stream_t &os,dump_##NAME##_ArgsTypeDoNotUseThis &args ) \
{ 
#define END_GECO_OSTREAM() \
}}

#define GECO_MESSAGE( NAME, STYLE, PARAM, HANDLER)	 \
GecoNetInterfaceElementWithStats& NAME = gMinder.add(#NAME, STYLE, PARAM, NULL_IF_NOT_SERVER(HANDLER));

#define BEGIN_GECO_INTERFACE( INAME ) \
namespace INAME \
{ \
	GecoInterfaceMinder gMinder(#INAME); \
	void registerWithInterface(GecoNetworkInterface& networkInterface ) \
	{ \
		gMinder.registerWithInterface(networkInterface); \
	} \
	GecoNetReason registerWithMachined(GecoNetworkInterface & networkInterface, int id ) \
	{ \
		return gMinder.registerWithMachined(networkInterface.address(), id); \
	}

#define GECO_STRUCT_MESSAGE( NAME, HANDLER)\
GECO_MESSAGE( NAME, FIXED_LENGTH_MESSAGE, sizeof(struct NAME##StructArgsType), HANDLER )\
struct NAME##StructArgsType NAME##StructArgs; \
struct dump_##NAME##_ArgsTypeDoNotUseThis 

#define END_GECO_INTERFACE() \
}

#else // declarations are placed here

// Declares a global GecoNetInterfaceElement with all msg attributes read from config file and add it to gMinder
#define GECO_MESSAGE( NAME, STYLE, PARAM, HANDLER )   extern GecoNetInterfaceElementWithStats& NAME;

#define BEGIN_GECO_INTERFACE( INAME ) \
/** @internal \
   * @{ \
   */ \
namespace INAME \
{ \
	extern GecoInterfaceMinder gMinder;	 /* All custom msg handlers will be stored in gMinder */ \
	void registerWithInterface(GecoNetworkInterface& networkInterface ); \
	GecoNetReason registerWithMachined(GecoNetworkInterface & networkInterface, int id ); 

#define BEGIN_GECO_ISTREAM( NAME ) \
  INLINE geco_bit_stream_t& operator>>( geco_bit_stream_t &is, NAME##StructArgsType &args ) \
  {
// add args compress decoder here ...
#define END_GECO_ISTREAM() \
      return is;\
  }
#define BEGIN_GECO_OSTREAM( NAME) \
  INLINE geco_bit_stream_t& operator<<( geco_bit_stream_t &os, NAME##StructArgsType &args ) \
  { \
    os.Write(NAME.id);
// add args uncompress encoder here ...
#define END_GECO_OSTREAM() \
      return os;\
  }

//  declare GECO_MESSAGE, struct type that represents msg body, encoder and decoder msg body,
#define GECO_STRUCT_MESSAGE( NAME, HANDLER) \
	struct NAME##StructArgsType;	\
	extern NAME##StructArgsType NAME##StructArgs; \
	GECO_MESSAGE( NAME, 0, 0, 0 ) \
	struct NAME##StructArgsType

#define END_GECO_INTERFACE() \
/** @} */	\
}

#endif // DEFINE_INTERFACE_HERE

