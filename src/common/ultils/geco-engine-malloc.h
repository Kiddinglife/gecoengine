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

#include <new>
#include "../geco-engine-export.h"

// These pointers are statically and globally defined in RakMemoryOverride.cpp
// Change them to point to your own allocators if you want.
// Use the functions for a DLL, or just reassign the variable if using source
typedef void*(*GecoMalloc) (size_t size);
typedef void*(*GecoRealloc) (void *p, size_t size);
typedef void(*GecoFree) (void *p, size_t size);
extern GECO_EXPORT GecoMalloc geco_malloc;
extern GECO_EXPORT GecoRealloc geco_realloc;
extern GECO_EXPORT GecoFree geco_free;

typedef  void * (*GecoMallocExt) (size_t size, const char *file, unsigned int line);
typedef  void * (*GecoReallocExt) (void *p, size_t size,
    const char *file, unsigned int line);
typedef  void(*GecoFreeExt) (void *p, size_t size, const char *file, unsigned int line);
extern GECO_EXPORT GecoMallocExt geco_malloc_ext;
extern GECO_EXPORT GecoReallocExt geco_realloc_ext;
extern GECO_EXPORT GecoFreeExt geco_free_ext;

#endif