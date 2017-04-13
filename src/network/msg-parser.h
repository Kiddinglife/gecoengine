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
extern void msg_call();
#endif /* __MSG_PARSER_H__ */

#include "msg-parser-generator.h"

// many-to-many dynamic dispatch model (eg. call entity method). request must know reply id for response
// one-to-one static dispatch model (eg. the interface defined by macros at compile time they are fixed handlers)

// below explains use dynamic reply id and handler for many-to-many dispatch model .
// main reason is dynamically created interface eg, script layer functions and cbs.
// 1. def store_items_cb(success_stored):  this is user defined cb func used to receive store fail or good in script level
// 2. self.cell.store_items(items[], store_items_cb): this is user inits rmi of store_items on its cell part
// c++ impl looks like this:
// replyid = clientapp.register_cb(store_items_cb, eid, mtdid); //register cb and will genersate a new reply id and a new reply handler that contains eid and mtdid
// cellappinterface::invoke_entity_mtdStructArgs.eid = 1;
// cellappinterface::invoke_entity_mtdStructArgs.mtdid = 2;
// cellappinterface::invoke_entity_mtdStructArgs.start_request();
// os << replyid << invoke_entity_mtdStructArgs; os << [item1,item2,item3]
// baseappchannel.send(os);
// .....
// in cell, its invoke_entity_mtd_handler handles the received msg and it then will callback peerlike this
//  os << replyid << invoke_entity_mtdStructArgs; os << [item1,item2,item3]
// 
// in cient, it invoke reply_mtd_handler to handle cb
//  client::replyhandlers[replyid].handle(is); //as handler has eid and cb mtd id so it can dispatch to  store_items_cb
//
// this design requires request and reply must be defined in entity.def to have a unique mtd id created for itself.

BEGIN_GECO_INTERFACE(CELLAPP)
BEGIN_STRUCT_MESSAGE(invoke_entity_mtd_with_cb, invoke_entity_mtd_cb_handler)
uint eid;
ushort mtdid;
ushort cbmtdid;
geco_bit_stream_t* data;
END_STRUCT_MESSAGE()
BEGIN_GECO_OSTREAM(invoke_entity_mtd_with_cb)
os.Write(args.eid);
os.Write(args.mtdid);
os.Write(args.cbmtdid);
args.data = &os;
END_GECO_OSTREAM()
BEGIN_GECO_ISTREAM(invoke_entity_mtd_with_cb)
is.Read(args.eid);
is.Read(args.mtdid);
is.Read(args.cbmtdid);
args.data = &is;
END_GECO_ISTREAM()
END_GECO_INTERFACE()

BEGIN_GECO_INTERFACE(CLIENTAPP)
BEGIN_STRUCT_MESSAGE(invoke_entity_mtd, invoke_entity_mtd_handler)
uint eid;
uint mtdid;
END_STRUCT_MESSAGE()
BEGIN_GECO_OSTREAM(invoke_entity_mtd)
os.Write(args.eid);
os.Write(args.mtdid);
END_GECO_OSTREAM()
BEGIN_GECO_ISTREAM(invoke_entity_mtd)
is.Read(args.eid);
is.Read(args.mtdid);
END_GECO_ISTREAM()
END_GECO_INTERFACE()
