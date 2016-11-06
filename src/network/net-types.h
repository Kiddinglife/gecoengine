
/*
* Copyright (c) 2016
* Geco Gaming Company
*
* Permission to use, copy, modify, distribute and sell this software
* and its documentation for GECO purpose is hereby granted without fee,
* provided that the above copyright notice appear in all copies and
* that both that copyright notice and this permission notice appear
* in supporting documentation. Geco Gaming makes no
* representations about the suitability of this software for GECO
* purpose.  It is provided "as is" without express or implied warranty.
*
*/

/**
* Created on 22 April 2016 by Jake Zhang
* Reviewed on 07 May 2016 by Jakze Zhang
*/

#ifndef __INCLUDE_NET_TYPES_H
#define __INCLUDE_NET_TYPES_H

#include "../common/geco-plateform.h"

#ifdef _WIN32//! win32
#define	NET_LOCALHOSTNAME "localhost"
#else
#define NET_LOCALHOSTNAME "lo"
#endif
#ifndef _WIN32
#ifndef _LP64
#define FMT_DBID "lld"
#else
#define FMT_DBID "ld"
#endif
#else
#define FMT_DBID "I64d"
#endif

const int NET_UDP_OVERHEAD = 28;
const int NET_BITS_PER_BYTE = 8;
const uint NET_LOCALHOST = 0x0100007F;
const uint NET_BROADCAST = 0xFFFFFFFF;
static const int NET_WN_PACKET_SIZE = 0x10000;
typedef uint TimeStamp;
typedef int EntityID;
const unsigned char ENTITY_DEF_INIT_FLG = 0;

const EntityID NULL_ENTITY = ((EntityID)0);
//! Server (0, 0x20000000)
const EntityID MIN_SERVER_ENTITY = ((EntityID)0);
const EntityID MAX_SERVER_ENTITY = ((EntityID)0x20000000);
//! Client (0x1FFFFFFF, 0x40000000)
const EntityID MIN_CLIENT_ENTITY = ((EntityID)0x1FFFFFFF);
const EntityID MAX_CLIENT_ENTITY = ((EntityID)0x40000000);
//! Global (0x3FFFFFFF, 0x7FFFFFFF)
const EntityID MIN_GLOBAL_ENTITY = ((EntityID)0x3FFFFFFF);
const EntityID MAX_GLOBAL_ENTITY = ((EntityID)0x7FFFFFFF);

typedef int SpaceID;
const SpaceID NULL_SPACE = ((SpaceID)0);
//! Server (0, 0x40000000)
const SpaceID MIN_SERVER_SPACE = ((SpaceID)0);
const SpaceID MAX_SERVER_SPACE = ((SpaceID)0x40000000);
//! Global (0x3FFFFFFF, 0x7FFFFFFF)
const SpaceID MIN_GLOBAL_SPACE = ((SpaceID)0x3FFFFFFF);
const SpaceID MAX_GLOBAL_SPACE = ((SpaceID)0x7FFFFFFF);

typedef int CellAppID;
typedef int BaseAppID;
typedef uint SessionKey;
typedef ushort EntityTypeID;
const EntityTypeID INVALID_ENTITY_TYPE_ID = EntityTypeID(-1);
typedef int64 DatabaseID;

typedef uint GridID;
typedef uint EventNumber;
const EventNumber INITIAL_EVENT_NUMBER = 1;
//typedef std::vector< EventNumber > CacheStamps;
typedef ushort VolatileNumber;
typedef unsigned char DetailLevel;
const int MAX_DATA_LOD_LEVELS = 6;

#endif
