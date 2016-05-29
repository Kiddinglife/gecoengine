#ifndef ThreadConditionSignalEvent_H_
#define ThreadConditionSignalEvent_H_

#include "geco-export.h"
#include "geco-namesapces.h"
#ifdef _WIN32
#include "geco-wins-includes.h"
#else
#include <pthread.h>
#include <sys/types.h>
#include "JackieSimpleMutex.h"
#endif
GECO_NET_BEGIN_NSPACE
class GECO_EXPORT JackieWaitEvent
{
    private:
#ifdef _WIN32
    HANDLE eventList;
#else
    JackieSimpleMutex isSignaledMutex;
    bool isSignaled;
#if !defined(ANDROID)
    pthread_condattr_t condAttr;
#endif
    pthread_cond_t eventList;
    pthread_mutex_t hMutex;
    pthread_mutexattr_t mutexAttr;
#endif

    public:
    JackieWaitEvent();
    ~JackieWaitEvent();

    void Init(void);
    void Close(void);
    void TriggerEvent(void);
    void WaitEvent(int timeoutMs);
};
GECO_NET_END_NSPACE
#endif
