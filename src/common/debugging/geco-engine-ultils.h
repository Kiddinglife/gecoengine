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

// created on 02-June-2016 by Jackie Zhang

#ifndef _INCLUDE_GECO_ENGINE_ULTILS
#define _INCLUDE_GECO_ENGINE_ULTILS

#include <string>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
/**
*	Converts the given narrow string to the wide representation, using the
*  active code page on this system. Returns true if it succeeded, otherwise
*	false if there was a decoding error.
*  bw_acptow() string_ultils.cpp line 67
*/
bool multibyte2wchar(const char * src, std::wstring& output);
#endif


#endif