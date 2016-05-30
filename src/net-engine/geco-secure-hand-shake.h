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

#ifndef __INCLUDE_SECURITY_HAND_SHAKE_H
#define __INCLUDE_SECURITY_HAND_SHAKE_H

#include "geco-features.h"
#if ENABLE_SECURE_HAND_SHAKE == 1

// If building a  DLL, be sure to tweak the CAT_EXPORT macro meaning
#if !defined(GECO_LIB) && defined(GECO_DLL)
# define CAT_BUILD_DLL
#else
#define CAT_NEUTER_EXPORT
#endif
#include <cat/AllTunnel.hpp>
#endif
#endif  //__SECURITYHANDSHAKE_H__

