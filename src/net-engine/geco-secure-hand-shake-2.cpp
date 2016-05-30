#include "geco-features.h"
#if ENABLE_SECURE_HAND_SHAKE == 1
#define LIBCAT_SECURITY
#else
#undef LIBCAT_SECURITY
#endif
#if defined(LIBCAT_SECURITY)
#include "cat/src/port/EndianNeutral.cpp"
#include "cat/src/mem/AlignedAllocator.cpp"
#include "cat/src/time/Clock.cpp"
#include "cat/src/threads/Mutex.cpp"
#include "cat/src/threads/Thread.cpp"
#include "cat/src/threads/WaitableFlag.cpp"
#include "cat/src/hash/Murmur.cpp"
#include "cat/src/lang/Strings.cpp"
#endif // LIBCAT_SECURITY