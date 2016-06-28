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

// created on 04-June-2016 by Jackie Zhang
#ifndef __INCLUDE_GECO_MALLOC_H
#define __INCLUDE_GECO_MALLOC_H

#if defined(__FreeBSD__)
#include <stdlib.h>
#elif defined ( __APPLE__ ) || defined ( __APPLE_CC__ )
#include <malloc/malloc.h>
#include <alloca.h>
#elif defined(_WIN32)
#include <malloc.h>
#else
#include <malloc.h>
#include <alloca.h> // Alloca needed on Ubuntu apparently
#endif
#include <new>

#include "../geco-engine-export.h"
#include "../geco-engine-config.h"

// These pointers are statically and globally defined in RakMemoryOverride.cpp
// Change them to point to your own allocators if you want.
// Use the functions for a DLL, or just reassign the variable if using source
typedef void*(*GecoMalloc)(size_t size);
typedef void*(*GecoRealloc)(void *p, size_t size);
typedef void (*GecoFree)(void *p, size_t size);
extern GECO_EXPORT GecoMalloc geco_malloc;
extern GECO_EXPORT GecoRealloc geco_realloc;
extern GECO_EXPORT GecoFree geco_free;

typedef void * (*GecoMallocExt)(size_t size, const char *file,
        unsigned int line);
typedef void * (*GecoReallocExt)(void *p, size_t old, size_t size, const char *file,
        unsigned int line);
typedef void (*GecoFreeExt)(void *p, size_t size, const char *file,
        unsigned int line);
extern GECO_EXPORT GecoMallocExt geco_malloc_ext;
extern GECO_EXPORT GecoReallocExt geco_realloc_ext;
extern GECO_EXPORT GecoFreeExt geco_free_ext;

extern GECO_EXPORT GecoMallocExt geco_malloc_ext_threads;
extern GECO_EXPORT GecoReallocExt geco_realloc_ext_threads;
extern GECO_EXPORT GecoFreeExt geco_free_ext_threads;

/// new functions with different number of ctor params, up to 4
template<class Type>
GECO_EXPORT Type* geco_new(const char *file, unsigned int line)
{
    char *buffer = (char *) (geco_malloc_ext)(sizeof(Type), file, line);
    Type *t = new (buffer) Type;
    return t;
}
template<class Type, class P1>
GECO_EXPORT Type* geco_new(const char *file, unsigned int line, const P1 &p1)
{
    char *buffer = (char *) (geco_malloc_ext)(sizeof(Type), file, line);
    Type *t = new (buffer) Type(p1);
    return t;
}
template<class Type, class P1, class P2>
GECO_EXPORT Type* geco_new(const char *file, unsigned int line, const P1 &p1,
        const P2 &p2)
{
    char *buffer = (char *) (geco_malloc_ext)(sizeof(Type), file, line);
    Type *t = new (buffer) Type(p1, p2);
    return t;
}
template<class Type, class P1, class P2, class P3>
GECO_EXPORT Type* geco_new(const char *file, unsigned int line, const P1 &p1,
        const P2 &p2, const P3 &p3)
{
    char *buffer = (char *) (geco_malloc_ext)(sizeof(Type), file, line);
    Type *t = new (buffer) Type(p1, p2, p3);
    return t;
}
template<class Type, class P1, class P2, class P3, class P4>
GECO_EXPORT Type* geco_new(const char *file, unsigned int line, const P1 &p1,
        const P2 &p2, const P3 &p3, const P4 &p4)
{
    char *buffer = (char *) (geco_malloc_ext)(sizeof(Type), file, line);
    Type *t = new (buffer) Type(p1, p2, p3, p4);
    return t;
}

template<class Type>
GECO_EXPORT Type* geco_new_array(const int count, const char *file,
        unsigned int line)
{
    if (count == 0)
        return 0;

    //		Type *t;
    char *buffer = (char *) (geco_malloc_ext)(
            sizeof(int) + sizeof(Type) * count, file, line);
    ((int*) buffer)[0] = count;
    for (int i = 0; i < count; i++)
    {
        //t =
        new (buffer + sizeof(int) + i * sizeof(Type)) Type;
    }
    return (Type *) (buffer + sizeof(int));
}

template<class Type>
GECO_EXPORT void geco_delete(Type *buff, const char *file, unsigned int line)
{
    if (buff == 0)
        return;
    buff->~Type();
    (geco_free_ext)((char*) buff, sizeof(Type), file, line);

}

template<class Type>
GECO_EXPORT void geco_delete_array(Type *buff, const char *file,
        unsigned int line)
{
    if (buff == 0)
        return;

    int count = ((int*) ((char*) buff - sizeof(int)))[0];
    Type *t;
    for (int i = 0; i < count; i++)
    {
        t = buff + i;
        t->~Type();
    }
    (geco_free_ext)(buff - sizeof(int), sizeof(Type), file, line);

}
#endif
