/*
*  This source code is licensed under the BSD-style license found in the
*  LICENSE file in the root directory of this source tree. An additional grant
*  of patent rights can be found in the PATENTS file in the same directory.
*
*/

/// \file
/// \brief \b [Internal] Encapsulates a mutex
///

#ifndef __SIMPLE_MUTEX_H
#define __SIMPLE_MUTEX_H

#include "geco-export.h"
#include "geco-sock-includes.h"
#include "geco-malloc-interface.h"

/// @brief An easy to use mutex.
/// 
/// I wrote this because the version that comes with Windows is too complicated and requires
/// too much code to use.
//
/// @remark Previously I used this everywhere, and in fact for a year or two RakNet was totally 
/// threadsafe.  While doing profiling, I saw that this function was incredibly slow compared to
/// theblazing performance of everything else, so switched to single producer / consumer 
/// everywhere.  Now the user thread of RakNet is not threadsafe, but it's 100X faster than 
/// before.
class GECO_EXPORT JackieSimpleMutex
{
    public:
    JackieSimpleMutex();
    ~JackieSimpleMutex();

    // Locks the mutex.  Slow!
    void Lock(void);

    // Unlocks the mutex.
    void Unlock(void);

#ifdef _WIN32
    CRITICAL_SECTION criticalSection;
    /// Docs say this is faster than a mutex for single process access
#else
    pthread_mutex_t m_mutex ;
    pthread_t m_threadID;
    unsigned int m_count;
#endif

    // Not threadsafe
    //	bool isInitialized;
};

#endif

