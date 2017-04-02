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
#ifndef __GECO_CONFIG_WIN_H__
#define __GECO_CONFIG_WIN_H__

#if !defined (__BORLANDC__)
#    include "geco-config-win-common.h"
#endif /* !__BORLANDC__ */

#if defined (__BORLANDC__)
#    include "geco-config-win-borland.h"
#elif defined (_MSC_VER)
#    include "geco-config-win-msvc.h"
#elif defined (ghs)
#    include "geco-config-win-ghs.h"
#elif defined (__MINGW32__)
#    include "geco-config-win-mingw.h"
#elif defined (__DMC__)
#    include "geco-config-win-dmc.h"
#else
#    error Compiler is not supported
#endif

#endif /* __GECO_CONFIG_WIN_H__ */
