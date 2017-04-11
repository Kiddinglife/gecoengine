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
 *  Created on: 18 Nov.,2016
 *      Author: jackiez
 */

#ifndef __MSG_PARSER_H__
#define __MSG_PARSER_H__
#include "net-types.h"
 /// this function is used to parse a single msg (datagram)
 /// @param[in] msg datagram when calling syscall receive(), it returns a complete msg
 /// @param[in] recvlen the msg length travelling on internet. when compressed by peer,
 /// it will be lwss than the actual msg length
extern void parse_msg(uchar* msg, uint recvlen, GecoNetAddress& from, network_recv_stats_t& recv_stats);
extern void msg_call();
#endif /* __MSG_PARSER_H__ */

#define USE_COMPRESS_STREAM // you must define this before 
#include "msg-parser-generator.h"

BEGIN_GECO_INTERFACE(CELLAPP)
BEGIN_STRUCT_MESSAGE(change_health, change_health_handler)
uint health;
END_STRUCT_MESSAGE()
BEGIN_GECO_OSTREAM(change_health)
os.WriteMini(args.health);
END_GECO_OSTREAM()
BEGIN_GECO_ISTREAM(change_health)
is.ReadMini(args.health);
END_GECO_ISTREAM()
END_GECO_INTERFACE()

BEGIN_GECO_INTERFACE(CLIENTAPP)
BEGIN_STRUCT_MESSAGE(change_health_cb, change_health_cb_handler)
std::string status;
END_STRUCT_MESSAGE()
BEGIN_GECO_OSTREAM(change_health_cb)
os.WriteMini(args.status);
END_GECO_OSTREAM()
BEGIN_GECO_ISTREAM(change_health_cb)
is.ReadMini(args.status);
END_GECO_ISTREAM()
END_GECO_INTERFACE()
