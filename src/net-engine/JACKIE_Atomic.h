#ifndef JACKIE_ATOMIC_H
#define JACKIE_ATOMIC_H

#include "geco-export.h"
#include "geco-basic-type.h"
#include "geco-sock-includes.h"


#if defined(ANDROID) || defined(__S3E__) || defined(__APPLE__)
// __sync_fetch_and_add not supported apparently
#include "JACKIE_Simple_Mutex.h"
#endif

struct GECO_EXPORT atomic_long_t
{
#ifdef _WIN32
    LONG value;
#elif defined(ANDROID) || defined(__S3E__) || defined(__APPLE__)
    // __sync_fetch_and_add not supported apparently
    JackieSimpleMutex mutex;
    unsigned int value;
#else
    unsigned int value;
#endif

    // Returns variable value after changing it
    unsigned int Increment(void);
    // Returns variable value after changing it
    unsigned int Decrement(void);
    unsigned int GetValue(void) const { return value; }
};
#endif
