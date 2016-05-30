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

#ifndef __INCLUDE_COMPILE_FEATURES_H
#define __INCLUDE_COMPILE_FEATURES_H

// If you want to change these defines, 
/// put them in CompileFeaturesOverride.h 
/// so your changes are not lost when updating GECO
// The user should not edit this file
#include "geco-features-overrided.h"

// Uncomment below defines, and paste to CompileFeaturesOverride.h, 
/// to exclude plugins that you do not want to build into the static library, or DLL
// These are not all the plugins, only those that are in the core library
// Other plugins are located in DependentExtensions
// #define GECO_SUPPORT_ConnectionGraph2 0
// #define GECO_SUPPORT_DirectoryDeltaTransfer 0
// #define GECO_SUPPORT_FileListTransfer 0
// #define GECO_SUPPORT_FullyConnectedMesh2 0
// #define GECO_SUPPORT_MessageFilter 0
// #define GECO_SUPPORT_NatPunchthroughClient 0
// #define GECO_SUPPORT_NatPunchthroughServer 0
// #define GECO_SUPPORT_NatTypeDetectionClient 0
// #define GECO_SUPPORT_NatTypeDetectionServer 0
// #define GECO_SUPPORT_PacketLogger 0
// #define GECO_SUPPORT_ReadyEvent 0
// #define GECO_SUPPORT_ReplicaManager3 0
// #define GECO_SUPPORT_Router2 0
// #define GECO_SUPPORT_RPC4Plugin 0
// #define GECO_SUPPORT_TeamBalancer 0
// #define GECO_SUPPORT_TeamManager 0
// #define GECO_SUPPORT_UDPProxyClient 0
// #define GECO_SUPPORT_UDPProxyCoordinator 0
// #define GECO_SUPPORT_UDPProxyServer 0
// #define GECO_SUPPORT_ConsoleServer 0
// #define GECO_SUPPORTGECOTransport 0
// #define GECO_SUPPORT_TelnetTransport 0
// #define GECO_SUPPORT_TCPInterface 0
// #define GECO_SUPPORT_LogCommandParser 0
// #define GECO_SUPPORTGECOCommandParser 0
// #define GECO_SUPPORT_EmailSender 0
// #define GECO_SUPPORT_HTTPConnection 0
// #define GECO_SUPPORT_HTTPConnection2 0
// #define GECO_SUPPORT_PacketizedTCP 0
// #define GECO_SUPPORT_TwoWayAuthentication 0

// SET DEFAULTS IF UNDEFINED
#ifndef ENABLE_SECURE_HAND_SHAKE
#define ENABLE_SECURE_HAND_SHAKE 1
#endif
#ifndef GECO_SUPPORT_ConnectionGraph2
#define GECO_SUPPORT_ConnectionGraph2 1
#endif
#ifndef GECO_SUPPORT_DirectoryDeltaTransfer
#define GECO_SUPPORT_DirectoryDeltaTransfer 1
#endif
#ifndef GECO_SUPPORT_FileListTransfer
#define GECO_SUPPORT_FileListTransfer 1
#endif
#ifndef GECO_SUPPORT_FullyConnectedMesh
#define GECO_SUPPORT_FullyConnectedMesh 1
#endif
#ifndef GECO_SUPPORT_FullyConnectedMesh2
#define GECO_SUPPORT_FullyConnectedMesh2 1
#endif
#ifndef GECO_SUPPORT_MessageFilter
#define GECO_SUPPORT_MessageFilter 1
#endif
#ifndef GECO_SUPPORT_NatPunchthroughClient
#define GECO_SUPPORT_NatPunchthroughClient 1
#endif
#ifndef GECO_SUPPORT_NatPunchthroughServer
#define GECO_SUPPORT_NatPunchthroughServer 1
#endif
#ifndef GECO_SUPPORT_NatTypeDetectionClient
#define GECO_SUPPORT_NatTypeDetectionClient 1
#endif
#ifndef GECO_SUPPORT_NatTypeDetectionServer
#define GECO_SUPPORT_NatTypeDetectionServer 1
#endif
#ifndef GECO_SUPPORT_PacketLogger
#define GECO_SUPPORT_PacketLogger 1
#endif
#ifndef GECO_SUPPORT_ReadyEvent
#define GECO_SUPPORT_ReadyEvent 1
#endif
#ifndef GECO_SUPPORT_ReplicaManager3
#define GECO_SUPPORT_ReplicaManager3 1
#endif
#ifndef GECO_SUPPORT_Router2
#define GECO_SUPPORT_Router2 1
#endif
#ifndef GECO_SUPPORT_RPC4Plugin
#define GECO_SUPPORT_RPC4Plugin 1
#endif
#ifndef GECO_SUPPORT_TeamBalancer
#define GECO_SUPPORT_TeamBalancer 1
#endif
#ifndef GECO_SUPPORT_TeamManager
#define GECO_SUPPORT_TeamManager 1
#endif
#ifndef GECO_SUPPORT_UDPProxyClient
#define GECO_SUPPORT_UDPProxyClient 1
#endif
#ifndef GECO_SUPPORT_UDPProxyCoordinator
#define GECO_SUPPORT_UDPProxyCoordinator 1
#endif
#ifndef GECO_SUPPORT_UDPProxyServer
#define GECO_SUPPORT_UDPProxyServer 1
#endif
#ifndef GECO_SUPPORT_ConsoleServer
#define GECO_SUPPORT_ConsoleServer 1
#endif
#ifndef GECO_SUPPORTGECOTransport
#define GECO_SUPPORTGECOTransport 1
#endif
#ifndef GECO_SUPPORT_TelnetTransport
#define GECO_SUPPORT_TelnetTransport 1
#endif
#ifndef GECO_SUPPORT_TCPInterface
#define GECO_SUPPORT_TCPInterface 1
#endif
#ifndef GECO_SUPPORT_LogCommandParser
#define GECO_SUPPORT_LogCommandParser 1
#endif
#ifndef GECO_SUPPORTGECOCommandParser
#define GECO_SUPPORTGECOCommandParser 1
#endif
#ifndef GECO_SUPPORT_EmailSender
#define GECO_SUPPORT_EmailSender 1
#endif
#ifndef GECO_SUPPORT_HTTPConnection
#define GECO_SUPPORT_HTTPConnection 1
#endif
#ifndef GECO_SUPPORT_HTTPConnection2
#define GECO_SUPPORT_HTTPConnection2 1
#endif
#ifndef GECO_SUPPORT_PacketizedTCP
#define GECO_SUPPORT_PacketizedTCP 1
#endif
#ifndef GECO_SUPPORT_TwoWayAuthentication
#define GECO_SUPPORT_TwoWayAuthentication 1
#endif
#ifndef GECO_SUPPORT_CloudClient
#define GECO_SUPPORT_CloudClient 1
#endif
#ifndef GECO_SUPPORT_CloudServer
#define GECO_SUPPORT_CloudServer 1
#endif
#ifndef GECO_SUPPORT_DynDNS
#define GECO_SUPPORT_DynDNS 1
#endif
#ifndef GECO_SUPPORT_Rackspace
#define GECO_SUPPORT_Rackspace 1
#endif
#ifndef GECO_SUPPORT_FileOperations
#define GECO_SUPPORT_FileOperations 1
#endif
#ifndef GECO_SUPPORT_UDPForwarder
#define GECO_SUPPORT_UDPForwarder 1
#endif
#ifndef GECO_SUPPORT_StatisticsHistory
#define GECO_SUPPORT_StatisticsHistory 1
#endif
#ifndef GECO_SUPPORT_LibVoice
#define GECO_SUPPORT_LibVoice 0
#endif
#ifndef GECO_SUPPORT_RelayPlugin
#define GECO_SUPPORT_RelayPlugin 1
#endif

// Take care of dependencies
#if GECO_SUPPORT_DirectoryDeltaTransfer==1
#undef GECO_SUPPORT_FileListTransfer
#define GECO_SUPPORT_FileListTransfer 1
#endif
#if GECO_SUPPORT_FullyConnectedMesh2==1
#undef GECO_SUPPORT_ConnectionGraph2
#define GECO_SUPPORT_ConnectionGraph2 1
#endif
#if GECO_SUPPORT_TelnetTransport==1
#undef GECO_SUPPORT_PacketizedTCP
#define GECO_SUPPORT_PacketizedTCP 1
#endif
#if GECO_SUPPORT_PacketizedTCP==1 || GECO_SUPPORT_EmailSender==1 || GECO_SUPPORT_HTTPConnection==1
#undef GECO_SUPPORT_TCPInterface
#define GECO_SUPPORT_TCPInterface 1
#endif
#endif