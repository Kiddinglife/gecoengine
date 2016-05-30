#include "geco-malloc-interface.h"

#ifdef SUPPORT_DL_MALLOC
#include "rdlmalloc.h"
#endif

#if USE_MEM_OVERRIDE==1
#if defined(malloc)
#pragma push_macro("malloc")
#undef malloc
#define RMO_MALLOC_UNDEF
#endif

#if defined(realloc)
#pragma push_macro("realloc")
#undef realloc
#define RMO_REALLOC_UNDEF
#endif

#if defined(free)
#pragma push_macro("free")
#undef free
#define RMO_FREE_UNDEF
#endif
#endif

using namespace geco::ultils;

void DefaultOutOfMemoryHandler(const char *file, const long line)
{
    (void) file;
    (void) line;
    assert(0);
}

JackieMalloc jackieMalloc = _DefaultMalloc;
JackieRealloc jackieRealloc = _DefaultRealloc;
JackieFree jackieFree = _DefaultFree;

inline void SetMalloc(void* (*userFunction)(size_t size))
{
    jackieMalloc = userFunction;
}
inline void SetRealloc(void* (*userFunction)(void *p, size_t size))
{
    jackieRealloc = userFunction;
}
inline void SetFree(void (*userFunction)(void *p))
{
    jackieFree = userFunction;
}
inline JackieMalloc GetMalloc()
{
    return jackieMalloc;
}
inline JackieRealloc GetRealloc()
{
    return jackieRealloc;
}
inline JackieFree GetFree()
{
    return jackieFree;
}

JackieMalloc_Ex gMallocEx = _DefaultMalloc_Ex;
JackieRealloc_Ex gReallocEx = _DefaultRealloc_Ex;
JackieFree_Ex gFreeEx = _DefaultFree_Ex;

inline void SetMalloc_Ex(JackieMalloc_Ex userFunction)
{
    gMallocEx = userFunction;
}
inline void SetRealloc_Ex(JackieRealloc_Ex userFunction)
{
    gReallocEx = userFunction;
}
inline void SetFree_Ex(JackieFree_Ex userFunction)
{
    gFreeEx = userFunction;
}
inline JackieMalloc_Ex GetMalloc_Ex()
{
    return gMallocEx;
}
inline JackieRealloc_Ex GetRealloc_Ex()
{
    return gReallocEx;
}
inline JackieFree_Ex GetFree_Ex()
{
    return gFreeEx;
}

NotifyOutOfMemory notifyOutOfMemory = DefaultOutOfMemoryHandler;
DlMallocMMap dlMallocMMap = _DLMallocMMap;
DlMallocDirectMMap dlMallocDirectMMap = _DLMallocDirectMMap;
DlMallocMUnmap dlMallocMUnmap = _DLMallocMUnmap;
inline void SetNotifyOutOfMemory(NotifyOutOfMemory userFunction)
{
    notifyOutOfMemory = userFunction;
}
void SetDLMallocMMap(DlMallocMMap userFunction)
{
    dlMallocMMap = userFunction;
}
void SetDLMallocDirectMMap(DlMallocDirectMMap userFunction)
{
    dlMallocDirectMMap = userFunction;
}
void SetDLMallocMUnmap(DlMallocMUnmap userFunction)
{
    dlMallocMUnmap = userFunction;
}
DlMallocMMap GetDLMallocMMap()
{
    return dlMallocMMap;
}
DlMallocDirectMMap GetDLMallocDirectMMap()
{
    return dlMallocDirectMMap;
}
DlMallocMUnmap GetDLMallocMUnmap()
{
    return dlMallocMUnmap;
}

void* _DefaultMalloc(size_t size)
{
    return malloc(size);
}

void* _DefaultRealloc(void *p, size_t size)
{
    return realloc(p, size);
}

void _DefaultFree(void *p)
{
    free(p);
}

void* _DefaultMalloc_Ex(size_t size, const char *file, unsigned int line)
{
    return malloc(size);
}
void* _DefaultRealloc_Ex(void *p, size_t size, const char *file,
        unsigned int line)
{
    (void) file;
    (void) line;
    return realloc(p, size);
}
void _DefaultFree_Ex(void *p, const char *file, unsigned int line)
{
    (void) file;
    (void) line;
    free(p);
}

#ifdef SUPPORT_DL_MALLOC
void * _DLMallocMMap(size_t size)
{
    return RAK_MMAP_DEFAULT(size);
}
void * _DLMallocDirectMMap(size_t size)
{
    return RAK_DIRECT_MMAP_DEFAULT(size);
}
int _DLMallocMUnmap(void *p, size_t size)
{
    return RAK_MUNMAP_DEFAULT(p, size);
}

static mspace rakNetFixedHeapMSpace = 0;

void* _DLMalloc(size_t size)
{
    return rak_mspace_malloc(rakNetFixedHeapMSpace, size);
}

void* _DLRealloc(void *p, size_t size)
{
    return rak_mspace_realloc(rakNetFixedHeapMSpace, p, size);
}

void _DLFree(void *p)
{
    if( p )
    rak_mspace_free(rakNetFixedHeapMSpace, p);
}
void* _DLMalloc_Ex(size_t size, const char *file, unsigned int line)
{
    (void) file;
    (void) line;

    return rak_mspace_malloc(rakNetFixedHeapMSpace, size);
}

void* _DLRealloc_Ex(void *p, size_t size, const char *file, unsigned int line)
{
    (void) file;
    (void) line;

    return rak_mspace_realloc(rakNetFixedHeapMSpace, p, size);
}

void _DLFree_Ex(void *p, const char *file, unsigned int line)
{
    (void) file;
    (void) line;

    if( p )
    rak_mspace_free(rakNetFixedHeapMSpace, p);
}

void UseRaknetFixedHeap(size_t initialCapacity,
        void * ( *yourMMapFunction ) ( size_t size ),
        void * ( *yourDirectMMapFunction ) ( size_t size ),
        int(*yourMUnmapFunction) ( void *p, size_t size ))
{
    SetDLMallocMMap(yourMMapFunction);
    SetDLMallocDirectMMap(yourDirectMMapFunction);
    SetDLMallocMUnmap(yourMUnmapFunction);
    SetMalloc(_DLMalloc);
    SetRealloc(_DLRealloc);
    SetFree(_DLFree);
    SetMalloc_Ex(_DLMalloc_Ex);
    SetRealloc_Ex(_DLRealloc_Ex);
    SetFree_Ex(_DLFree_Ex);

    rakNetFixedHeapMSpace = rak_create_mspace(initialCapacity, 0);
}
void FreeRakNetFixedHeap(void)
{
    if( rakNetFixedHeapMSpace )
    {
        rak_destroy_mspace(rakNetFixedHeapMSpace);
        rakNetFixedHeapMSpace = 0;
    }

    SetMalloc(_DefaultMalloc);
    SetRealloc(_DefaultRealloc);
    SetFree(_DefaultFree);
    SetMalloc_Ex(_DefaultMalloc_Ex);
    SetRealloc_Ex(_DefaultRealloc_Ex);
    SetFree_Ex(_DefaultFree_Ex);
}
#else
void * _DLMallocMMap(size_t size)
{
    (void) size;
    return 0;
}
void * _DLMallocDirectMMap(size_t size)
{
    (void) size;
    return 0;
}
int _DLMallocMUnmap(void *p, size_t size)
{
    (void) size;
    (void) p;
    return 0;
}
void* _DLMalloc(size_t size)
{
    (void) size;
    return 0;
}
void* _DLRealloc(void *p, size_t size)
{
    (void) p;
    (void) size;
    return 0;
}
void _DLFree(void *p)
{
    (void) p;
}
void* _DLMalloc_Ex(size_t size, const char *file, unsigned int line)
{
    (void) size;
    (void) file;
    (void) line;
    return 0;
}
void* _DLRealloc_Ex(void *p, size_t size, const char *file, unsigned int line)
{
    (void) p;
    (void) size;
    (void) file;
    (void) line;
    return 0;
}
void _DLFree_Ex(void *p, const char *file, unsigned int line)
{
    (void) p;
    (void) file;
    (void) line;
}

void UseRaknetFixedHeap(size_t initialCapacity,
        void * (*yourMMapFunction)(size_t size),
        void * (*yourDirectMMapFunction)(size_t size),
        int (*yourMUnmapFunction)(void *p, size_t size))
{
    (void) initialCapacity;
    (void) yourMMapFunction;
    (void) yourDirectMMapFunction;
    (void) yourMUnmapFunction;
}
void FreeRakNetFixedHeap(void)
{
}
#endif

#if USE_MEM_OVERRIDE==1
#if defined(RMO_MALLOC_UNDEF)
#pragma pop_macro("malloc")
#undef RMO_MALLOC_UNDEF
#endif

#if defined(RMO_REALLOC_UNDEF)
#pragma pop_macro("realloc")
#undef RMO_REALLOC_UNDEF
#endif

#if defined(RMO_FREE_UNDEF)
#pragma pop_macro("free")
#undef RMO_FREE_UNDEF
#endif
#endif
