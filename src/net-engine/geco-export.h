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

#ifndef __INCLUDE_GECO_EXPORT_H
#define __INCLUDE_GECO_EXPORT_H

#if defined(_WIN32) && !(defined(__GNUC__)  || defined(__GCCXML__)) \
      && !defined(GECO_LIB) && defined(GECO_DLL)
#define GECO_EXPORT __declspec(dllexport)
#else
#define GECO_EXPORT
#endif

#endif
