/*
 * test-app-msg-handlers.h
 *
 *  Created on: 12Apr.,2017
 *      Author: jackiez
 */

#include "network/msg-handler-defines.h"

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
BEGIN_HANDLED_STRUCT_MESSAGE(cell_invoke_entity_mtd_with_cbid, CellEntityMsgHandler, &cell::invoke_entity_mtd)
uint eid;
ushort mtdid;
ushort cbmtdid;
geco_bit_stream_t* data;
END_HANDLED_STRUCT_MESSAGE()
BEGIN_GECO_OSTREAM(cell_invoke_entity_mtd_with_cbid)
os.Write(args.eid);
os.Write(args.mtdid);
os.Write(args.cbmtdid);
args.data = &os;
END_GECO_OSTREAM()
BEGIN_GECO_ISTREAM(cell_invoke_entity_mtd_with_cbid)
is.Read(args.eid);
is.Read(args.mtdid);
is.Read(args.cbmtdid);
args.data = &is;
END_GECO_ISTREAM()
END_GECO_INTERFACE()

BEGIN_GECO_INTERFACE(CLIENTAPP)
BEGIN_STRUCT_MESSAGE(client_invoke_entity_mtd, invoke_entity_mtd_handler)
uint eid;
uint mtdid;
std::string ret;
geco_bit_stream_t* data;
END_STRUCT_MESSAGE()
BEGIN_GECO_OSTREAM(client_invoke_entity_mtd)
os.Write(args.eid);
os.Write(args.mtdid);
os.Write(args.ret);
args.data = &os;
END_GECO_OSTREAM()
BEGIN_GECO_ISTREAM(client_invoke_entity_mtd)
is.Read(args.eid);
is.Read(args.mtdid);
is.Read(args.ret);
geco_bit_stream_t* data;
END_GECO_ISTREAM()
END_GECO_INTERFACE()
