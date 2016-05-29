/*
*  Copyright (c) 2014, Oculus VR, Inc.
*  All rights reserved.
*
*  This source code is licensed under the BSD-style license found in the
*  LICENSE file in the root directory of this source tree. An additional grant
*  of patent rights can be found in the PATENTS file in the same directory.
*
*/

#define JACKIE_INET_VERSION "1.0"
#define JACKIE_INET_VERSION_NUMBER 1.0
#define JACKIE_INET_VERSION_NUMBER_INT 10

#define JACKIE_INET_DATE "10/19/2015"

// What compatible protocol version RakNet is using. When this value changes, it indicates this version of RakNet cannot connection to an older version.
// ID_INCOMPATIBLE_PROTOCOL_VERSION will be returned on connection attempt in this case
#define JACKIE_INET_PROTOCOL_VERSION 6