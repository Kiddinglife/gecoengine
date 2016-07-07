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

#ifndef __INCLUDE_GECO_EXPORT_H
#define __INCLUDE_GECO_EXPORT_H

#if defined(_WIN32) && !(defined(__GNUC__)  || defined(__GCCXML__)) \
      && !defined(STATIC_LIB_BUILD) && defined(DYNAMIC_LIB_BUILD)
#define GECO_EXPORT __declspec(dllexport)
#else
#define GECO_EXPORT
#endif

#endif