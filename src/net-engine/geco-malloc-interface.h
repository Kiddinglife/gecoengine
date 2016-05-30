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

#ifndef __INCLUDE_GECO_MALLOC_INTERFACE_H
#define __INCLUDE_GECO_MALLOC_INTERFACE_H

#include <new>

#if defined(__FreeBSD__)
#include <stdlib.h>
#elif defined ( __APPLE__ ) || defined ( __APPLE_CC__ )
#include <malloc/malloc.h>
#include <alloca.h>
#elif defined(_WIN32)
#include <malloc.h>
#else
#include <malloc.h>
// Alloca needed on Ubuntu apparently
#include <alloca.h>
#endif

#include "geco-export.h"
#include "geco-net-config.h"
#include "geco-namesapces.h"


// These pointers are statically and globally defined in RakMemoryOverride.cpp
// Change them to point to your own allocators if you want.
// Use the functions for a DLL, or just reassign the variable if using source
typedef void * (*JackieMalloc) (size_t size);
typedef void * (*JackieRealloc) (void *p, size_t size);
typedef void(*JackieFree) (void *p);
extern GECO_EXPORT JackieMalloc jackieMalloc;
extern GECO_EXPORT JackieRealloc jackieRealloc;
extern GECO_EXPORT JackieFree jackieFree;
extern GECO_EXPORT void SetMalloc(JackieMalloc userFunction);
extern GECO_EXPORT void SetRealloc(JackieRealloc userFunction);
extern GECO_EXPORT void SetFree(JackieFree userFunction);
extern GECO_EXPORT JackieMalloc GetMalloc();
extern GECO_EXPORT JackieRealloc GetRealloc();
extern GECO_EXPORT JackieFree GetFree();

typedef  void * (*JackieMalloc_Ex) (size_t size, const char *file, unsigned int line);
typedef  void * (*JackieRealloc_Ex) (void *p, size_t size, const char *file, unsigned int line);
typedef  void(*JackieFree_Ex) (void *p, const char *file, unsigned int line);
extern GECO_EXPORT JackieMalloc_Ex gMallocEx;
extern GECO_EXPORT JackieRealloc_Ex gReallocEx;
extern GECO_EXPORT JackieFree_Ex gFreeEx;
extern GECO_EXPORT void SetMalloc_Ex(JackieMalloc_Ex userFunction);
extern GECO_EXPORT void SetRealloc_Ex(JackieRealloc_Ex userFunction);
extern GECO_EXPORT void SetFree_Ex(JackieFree_Ex userFunction);
extern GECO_EXPORT JackieMalloc_Ex GetMalloc_Ex();
extern GECO_EXPORT JackieRealloc_Ex GetRealloc_Ex();
extern GECO_EXPORT JackieFree_Ex GetFree_Ex();

typedef void(*NotifyOutOfMemory) (const char *file, const long line);
typedef void* (*DlMallocMMap) (size_t size);
typedef void* (*DlMallocDirectMMap) (size_t size);
typedef int(*DlMallocMUnmap) (void* ptr, size_t size);
extern GECO_EXPORT NotifyOutOfMemory notifyOutOfMemory;
extern GECO_EXPORT DlMallocMMap dlMallocMMap;
extern GECO_EXPORT DlMallocDirectMMap dlMallocDirectMMap;
extern GECO_EXPORT DlMallocMUnmap dlMallocMUnmap;
extern GECO_EXPORT void SetNotifyOutOfMemory(NotifyOutOfMemory userFunction);
extern GECO_EXPORT void SetDLMallocMMap(DlMallocMMap userFunction);
extern GECO_EXPORT void SetDLMallocDirectMMap(DlMallocDirectMMap userFunction);
extern GECO_EXPORT void SetDLMallocMUnmap(DlMallocMUnmap userFunction);
extern GECO_EXPORT DlMallocMMap GetDLMallocMMap();
extern GECO_EXPORT DlMallocDirectMMap GetDLMallocDirectMMap();
extern GECO_EXPORT DlMallocMUnmap GetDLMallocMUnmap();

GECO_ULTILS_BEGIN_NSPACE
/// new functions with different number of ctor params, up to 4
template <class Type>
GECO_EXPORT Type* OP_NEW(const char *file, unsigned int line)
{
#if USE_MEM_OVERRIDE==1
    char *buffer = (char *) ( GetMalloc_Ex() )( sizeof(Type), file, line );
    Type *t = new (buffer) Type;
    return t;
#else
    (void)file;
    (void)line;
    return new Type;
#endif
}
template <class Type, class P1>
GECO_EXPORT Type* OP_NEW_1(const char *file, unsigned int line,
    const P1 &p1)
{
#if USE_MEM_OVERRIDE==1
    char *buffer = (char *) ( GetMalloc_Ex() )( sizeof(Type), file, line );
    Type *t = new (buffer) Type(p1);
    return t;
#else
    (void)file;
    (void)line;
    return new Type(p1);
#endif
}
template <class Type, class P1, class P2>
GECO_EXPORT Type* OP_NEW_2(const char *file, unsigned int line,
    const P1 &p1, const P2 &p2)
{
#if USE_MEM_OVERRIDE==1
    char *buffer = (char *) ( GetMalloc_Ex() )( sizeof(Type), file, line );
    Type *t = new (buffer) Type(p1, p2);
    return t;
#else
    (void)file;
    (void)line;
    return new Type(p1, p2);
#endif
}
template <class Type, class P1, class P2, class P3>
GECO_EXPORT Type* OP_NEW_3(const char *file, unsigned int line,
    const P1 &p1, const P2 &p2, const P3 &p3)
{
#if USE_MEM_OVERRIDE==1
    char *buffer = (char *) ( GetMalloc_Ex() )( sizeof(Type), file, line );
    Type *t = new (buffer) Type(p1, p2, p3);
    return t;
#else
    (void)file;
    (void)line;
    return new Type(p1, p2, p3);
#endif
}
template <class Type, class P1, class P2, class P3, class P4>
GECO_EXPORT Type* OP_NEW_4(const char *file, unsigned int line,
    const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4)
{
#if USE_MEM_OVERRIDE==1
    char *buffer = (char *) ( GetMalloc_Ex() )( sizeof(Type), file, line );
    Type *t = new (buffer) Type(p1, p2, p3, p4);
    return t;
#else
    (void)file;
    (void)line;
    return new Type(p1, p2, p3, p4);
#endif
}


template <class Type>
GECO_EXPORT Type* OP_NEW_ARRAY(const int count, const char *file, unsigned int line)
{
    if (count == 0)
        return 0;

#if USE_MEM_OVERRIDE==1
    //		Type *t;
    char *buffer = (char *) ( GetMalloc_Ex() )( sizeof(int) + sizeof(Type)*count, file, line );
    ( (int*) buffer )[0] = count;
    for( int i = 0; i<count; i++ )
    {
        //t = 
        new( buffer + sizeof(int) + i*sizeof(Type) ) Type;
    }
    return (Type *) ( buffer + sizeof(int) );
#else
    (void)file;
    (void)line;
    return new Type[count];
#endif

}

template <class Type>
GECO_EXPORT void OP_DELETE(Type *buff, const char *file, unsigned int line)
{
#if USE_MEM_OVERRIDE==1
    if( buff == 0 ) return;
    buff->~Type();
    ( GetFree_Ex() )( (char*) buff, file, line );
#else
    (void)file;
    (void)line;
    delete buff;
#endif

}

template <class Type>
GECO_EXPORT void OP_DELETE_ARRAY(Type *buff, const char *file, unsigned int line)
{
#if USE_MEM_OVERRIDE==1
    if( buff == 0 )
        return;

    int count = ( (int*) ( (char*) buff - sizeof(int) ) )[0];
    Type *t;
    for( int i = 0; i<count; i++ )
    {
        t = buff + i;
        t->~Type();
    }
    ( GetFree_Ex() )( (char*) buff - sizeof(int), file, line );
#else
    (void)file;
    (void)line;
    delete[] buff;
#endif

}

GECO_ULTILS_END_NSPACE

void GECO_EXPORT * _DefaultMalloc(size_t size);
void GECO_EXPORT * _DefaultRealloc(void *p, size_t size);
void GECO_EXPORT _DefaultFree(void *p);
void GECO_EXPORT * _DefaultMalloc_Ex(size_t size, const char *file, unsigned int line);
void GECO_EXPORT * _DefaultRealloc_Ex(void *p, size_t size, const char *file, unsigned int line);
void GECO_EXPORT _DefaultFree_Ex(void *p, const char *file, unsigned int line);
void GECO_EXPORT * _DLMallocMMap(size_t size);
void GECO_EXPORT * _DLMallocDirectMMap(size_t size);
int GECO_EXPORT _DLMallocMUnmap(void *p, size_t size);



// Call to make JACKIE_INET allocate a large block of memory, 
// and do all subsequent allocations in that memory block
// Initial and reallocations will be done through whatever function is pointed to by 
// yourMMapFunction, and yourDirectMMapFunction (default is malloc)
// Allocations will be freed through whatever function is pointed to by yourMUnmapFunction
// (default free)
void UseRaknetFixedHeap(size_t initialCapacity,
    void * (*yourMMapFunction) (size_t size) = _DLMallocMMap,
    void * (*yourDirectMMapFunction) (size_t size) = _DLMallocDirectMMap,
    int(*yourMUnmapFunction) (void *p, size_t size) = _DLMallocMUnmap);

// Free memory allocated from UseRaknetFixedHeap
void FreeRakNetFixedHeap(void);
#endif