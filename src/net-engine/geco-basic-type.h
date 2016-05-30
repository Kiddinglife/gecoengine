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

#ifndef  __INCLUDE_GECO_BASIC_TYPE_DEFS_H
#define __INCLUDE_GECO_BASIC_TYPE_DEFS_H

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

#if defined(__GNUC__) || defined(__GCCXML__) || defined(__SNC__) || defined(__S3E__)
#include <stdint.h>
typedef uint64_t ulonglong;
typedef int64_t longlong;
#else
#   if defined(_MSC_VER) && _MSC_VER < 1300
typedef unsigned __int64 ulonglong;
typedef  __int64 longlong;
typedef unsigned __int64 ulonglong;
typedef __int64 longlong;
#  else
typedef long long longlong;
typedef unsigned long long ulonglong;
#  endif
#endif

#endif//end
