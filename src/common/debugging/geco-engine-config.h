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

// created on 02-June-2016 by Jackie Zhang

#ifndef _INCLUDE_GECO_ENGINE_CONFIG
#define _INCLUDE_GECO_ENGINE_CONFIG

/**
 * This define is used to control the conditional compilation of features
 * that will be removed from the client builds provided to the public.
 * Examples of these would be the in-game Python console, the Watcher and
 * Nub interface.
 *
 * If SERVER_BUILD is equal to zero
 * then the array of development features should be compiled in.
 * If CLIENT_BUILD is equal to one
 * then development and maintenance only features should be excluded.
 *
 * build server and client at same time is wrong
 */
#if defined( SERVER_BUILD ) && defined( CLIENT_BUILD )
#error "CLIENT_BUILD and SERVER_BUILD macros should not be used at same time"
#endif
#if !defined( SERVER_BUILD ) && !defined( CLIENT_BUILD )
#error "both of CLIENT_BUILD and SERVER_BUILD macro are not be used !"
#endif

/**
 * These settings enable have to be manually turned on or off
 * (no matter whether we're compiling the consumer client build or not).
 */
#define ENABLE_RESOURCE_COUNTERS					0
#define ENABLE_CULLING_HUD							0

/**
 * By setting one of the following FORCE_ENABLE_ defines to 1
 * then support for the corresponding feature will be compiled even though
 * in case that CLIENT_BUILD is defined.
 */
#define FORCE_ENABLE_DEBUG_KEY_HANDLER				0
#define FORCE_ENABLE_MSG_LOGGING					0
#define FORCE_ENABLE_DPRINTF						1
#define FORCE_ENABLE_PYTHON_TELNET_SERVICE			0
#define FORCE_ENABLE_WATCHERS						0
#define FORCE_ENABLE_DOG_WATCHERS					0
#define FORCE_ENABLE_PROFILER						0
#define FORCE_ENABLE_ACTION_QUEUE_DEBUGGER			0
#define FORCE_ENABLE_DOC_STRINGS					0
#define FORCE_ENABLE_DIARIES						0
#define FORCE_ENABLE_DDS_GENERATION					0
#define FORCE_ENABLE_FILE_CASE_CHECKING				0
#define FORCE_ENABLE_ENVIRONMENT_SYNC				0
#define FORCE_ENABLE_ENTER_DEBUGGER_MESSAGE			0
#define FORCE_ENABLE_PYTHON_LOG						0
#define FORCE_ENABLE_STACK_TRACKER					0

#define ENABLE_DPRINTF	\
(!CLIENT_BUILD || FORCE_ENABLE_DPRINTF)

#endif