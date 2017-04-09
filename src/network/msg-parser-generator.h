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
#define __MSG_PARSER_GENERATOR_H__
#endif /* __MSG_PARSER_GENERATOR_H_ */

#ifndef INTERFACE_MACROS_HPP  //  Prevent include the folowing macros more than one time

#define GECO_FIXED_MESSAGE( NAME, PARAM, HANDLER )         GECO_MESSAGE( NAME, FIXED_LENGTH_MESSAGE, PARAM, HANDLER)
#define GECO_VARIABLE_MESSAGE( NAME, PARAM, HANDLER )  GECO_MESSAGE( NAME, VARIABLE_LENGTH_MESSAGE, PARAM, HANDLER)
#define GECO_EMPTY_MESSAGE( NAME, HANDLER )				     GECO_MESSAGE( NAME, FIXED_LENGTH_MESSAGE, 0, HANDLER )	

 // declares static methods in struct NAME##Args
#define GECO_STRUCT_GOODIES( NAME )	\
static NAME##Args& start( geco_bit_stream_t & kOS, void * arg = NULL,int timeout = DEFAULT_REQUEST_TIMEOUT )\
{\
	if (kOS.is_compression_mode())\
	{\
		kOS.WriteMini(NAME.m_uiID);\
		is_request ? kOS.WriteBitOne() : kOS.WriteBitZero();\
	}\
	else\
	{\
		kOS.Write(kDes.m_uiYaw);\
		kOS.Write(kDes.m_uiPitch);\
	}\
	b.WriteMini(NAME.m_uiID); b.WriteMini(); if(NAME.)  \
	return *(NAME##Args*)b.startStructRequest( NAME, arg, timeout ); \
}\
static NAME##Args& end(geco_bit_stream_t & kOS, void * arg = NULL, int timeout = DEFAULT_REQUEST_TIMEOUT)\
{\
	if (kOS.is_compression_mode())\
	{\
		kOS.WriteMini(NAME.m_uiID);\
		kOS.WriteMini(NAME.);\
	}\
	else\
	{\
		kOS.Write(kDes.m_uiYaw);\
		kOS.Write(kDes.m_uiPitch);\
	}\
	b.WriteMini(NAME.m_uiID); b.WriteMini(); if(NAME.)  \
	return *(NAME##Args*)b.startStructRequest( NAME, arg, timeout ); \
}

#define BEGIN_STRUCT_MESSAGE( NAME, HANDLER ) \
GECO_STRUCT_MESSAGE( NAME, HANDLER )	\
{ \
	GECO_STRUCT_GOODIES( NAME )									
#define END_STRUCT_MESSAGE()\
};

 //  declare GECO_STRUCT_MESSAGE, plus handler statement
#define GECO_HANDLED_STRUCT_MESSAGE(	NAME, HANDLERTYPE, HANDLERARG ) \
HANDLER_STATEMENT( NAME, HANDLERTYPE, HANDLERARG) \
GECO_STRUCT_MESSAGE( NAME, HANDLER_ARGUMENT( NAME ) ) 
 // Helper macro that declares GECO_HANDLED_STRUCT_MESSAGE plus GECO_STRUCT_GOODIES 
#define BEGIN_HANDLED_STRUCT_MESSAGE( NAME, HANDLERTYPE, HANDLERARG )	\
GECO_HANDLED_STRUCT_MESSAGE( NAME, HANDLERTYPE, HANDLERARG )		\
{ \
	GECO_STRUCT_GOODIES( NAME )								
#define END_HANDLED_STRUCT_MESSAGE()	\
};	

#endif

 // Cleanup to prevent user accidently define same macros
#ifdef BEGIN_GECO_INTERFACE
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
#undef 
#endif

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
#define DEFINE_INTERFACE_HERE
#ifdef DEFINE_INTERFACE_HERE // definitions are placed here

#define GECO_MESSAGE( NAME, STYLE, PARAM, HANDLER)	 \
const GecoNetInterfaceElement & NAME = gMinder.add(#NAME, STYLE, PARAM, NULL_IF_NOT_SERVER(HANDLER));	

#define GECO_ISTREAM( NAME,   ISTREAM ) geco_bit_stream_t& operator >> (geco_bit_stream_t &is, NAME##Args &x) {return is >> XSTREAM;}
#define GECO_OSTREAM( NAME, OSTREAM ) geco_bit_stream_t& operator<<( geco_bit_stream_t &os, const NAME##Args &x ) {return os << OSTREAM;}

#define BEGIN_GECO_INTERFACE( INAME ) \
namespace INAME \
{ \
	void registerWithInterface(GecoNetworkInterface& networkInterface ) \
	{ \
		gMinder.registerWithInterface(networkInterface); \
	} \
	GecoNetReason registerWithMachined(GecoNetworkInterface & networkInterface, int id ) \
	{ \
		return gMinder.registerWithMachined(networkInterface.address(), id); \
	}

#define GECO_STRUCT_MESSAGE( NAME, HANDLER, ISTREAM,OSTREAM)\
		GECO_MESSAGE( NAME, FIXED_LENGTH_MESSAGE, sizeof(struct NAME##Args), HANDLER )	\
		GECO_ISTREAM( NAME, ISTREAM ) \
		GECO_OSTREAM( NAME, XSTREAM ) \
		struct __Garbage__##NAME##Args
#define END_GECO_INTERFACE() \
}

#else // declarations are placed here

 // Declares a global GecoNetInterfaceElement with all msg attributes read from config file and add it to gMinder
#define GECO_MESSAGE( NAME, STYLE, PARAM, HANDLER )   extern const GecoNetInterfaceElement& NAME;	

#define GECO_ISTREAM( NAME,   ISTREAM )   geco_bit_stream_t& operator>>( geco_bit_stream_t &is, NAME##Args &x );				
#define GECO_OSTREAM( NAME, OSTREAM ) geco_bit_stream_t& operator<<( geco_bit_stream_t &os, const NAME##Args &x );		

#define BEGIN_GECO_INTERFACE( INAME ) \
/** @internal \
   * @{ \
   */ \
namespace INAME \
{ \
	extern GecoInterfaceMinder gMinder;	 /* All custom msg handlers will be stored in gMinder */ \
	void registerWithInterface(GecoNetworkInterface& networkInterface ); \
	GecoNetReason registerWithMachined(GecoNetworkInterface & networkInterface, int id ); 
//  declare GECO_MESSAGE, struct type that represents msg body, encoder and decoder msg body,
#define GECO_STRUCT_MESSAGE( NAME, HANDLER, ISTREAM,OSTREAM) \
	struct NAME##Args;	\
	GECO_MESSAGE( NAME, 0, 0, 0 ) \
	/** @internal */ \
	GECO_ISTREAM( NAME, ISTREAM ) \
	GECO_OSTREAM( NAME, XSTREAM ) \
	/** @internal */ \
	struct NAME##Args
#define END_GECO_INTERFACE() \
/** @} */	\
}

#endif // DEFINE_INTERFACE_HERE
