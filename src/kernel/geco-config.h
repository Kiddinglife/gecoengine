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

//{future header message}
#ifndef __GECO_CONFIG_H__
#define __GECO_CONFIG_H__

#if (!defined(_DEBUG) && !defined(NDEBUG) && !defined(SHIPPING))
#error exactly one of GECO_DEBUG,GECO_REALSE,GECO_SHIPPING must be defined.
#endif 

#if (!defined(GECO_BUILD_STATIC_LIBS) && !defined(GECO_BUILD_DLL_LIBS))
#error exactly one of GECO_BUILD_STATIC_LIBS, GECO_BUILD_DLL_LIBS must be defined.
#endif

#if (defined(GECO_BUILD_STATIC_LIBS) && defined(GECO_BUILD_DLL_LIBS))
#error exactly one of GECO_BUILD_STATIC_LIBS, GECO_BUILD_DLL_LIBS must be defined.
#endif 

#if (defined(GECO_BUILD_DLL_LIBS) && !defined(GECO_BUILD_DLL) && !defined(GECO_EXPORT_DLL))
#error exactly one of GECO_BUILD_DLL, GECO_EXPORT_DLL must be defined if GECO_BUILD_DLL_LIBS is defined.
#endif 

#include "geco-config-win-common.h"

#endif /* __GECO_CONFIG_H__ */
