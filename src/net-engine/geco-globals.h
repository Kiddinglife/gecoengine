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

// created on 20-March-2016 by Jackie Zhang

#ifndef __INCLUDE_GECO_FAST_HASH_H
#define __INCLUDE_GECO_FAST_HASH_H

#include <cstddef>
#include <cstdint>
#include <emmintrin.h>

#include "geco-export.h"
#include "geco-namesapces.h"


GECO_ULTILS_BEGIN_NSPACE

//! From http://www.azillionmonkeys.com/qed/hash.html
//! Author of main code is Paul Hsieh. I just added some convenience functions
//! Also note http://burtleburtle.net/bob/hash/doobs.html, which shows that this is 20%
//! faster than the one on that page but has more collisions
GECO_EXPORT extern  unsigned long  SuperFastHash(const char * data, int length);
GECO_EXPORT extern  unsigned long  SuperFastHashIncremental(const char * data, int len, unsigned int lastHash);
GECO_EXPORT extern  unsigned long  SuperFastHashFile(const char * filename);
GECO_EXPORT extern  unsigned long  SuperFastHashFilePtr(FILE *fp);


//! Fast itoa from http://www.jb.man.ac.uk/~slowe/cpp/itoa.html for Linux since it seems like
//! Linux doesn't support this function. I modified it to remove the std dependencies.
GECO_EXPORT extern char*  Itoa(int value, char* result, int base);


inline GECO_EXPORT extern unsigned long long AlignBinary(unsigned long long ptr, unsigned char alignment)
{
    unsigned int const tmp = alignment - 1;
    return (ptr + tmp) & (~tmp);
}

//! Return the next address aligned to a required boundary
inline GECO_EXPORT extern char* PointerAlignBinary(char const * ptr, unsigned char alignment)
{
    return reinterpret_cast<char *> (AlignBinary(reinterpret_cast<unsigned long long> (ptr),
        alignment));
}

//! Return the next address aligned to a required boundary
inline GECO_EXPORT extern char* PointerAlignBinary(unsigned char const * ptr, unsigned char alignment)
{
    return PointerAlignBinary(reinterpret_cast<char const *> (ptr), alignment);
}

//---------------------------------------------------------------------
// main routine
//---------------------------------------------------------------------
GECO_EXPORT extern void* SuperFastMemCpy(void *destination, const void *source, size_t size);

GECO_ULTILS_END_NSPACE
#endif
