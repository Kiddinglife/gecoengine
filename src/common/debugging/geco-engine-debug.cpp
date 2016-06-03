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
//#include "pch.hpp"
//#include "string_utils.hpp"
#include <stdio.h>      /* printf, vprintf*/
#include <stdlib.h>     /* malloc */
#include <string.h>     /* strlen, strcat */
#include <stdarg.h>     /* va_list, va_start, va_copy, va_arg, va_end */
#include <time.h>
#include "geco-engine-debug.h"
#include "geco-engine-ultils.h"

log_msg_mgr_t* log_msg_mgr_t::s_instance_ = NULL;
bool log_msg_mgr_t::shouldWriteTimePrefix = false;
#if defined( SERVER_BUILD ) || defined( PLAYSTATION3 )
bool log_msg_mgr_t::shouldWriteToConsole = true;
#else
bool log_msg_mgr_t::shouldWriteToConsole = false;
#endif

#if ENABLE_DPRINTF
// Commented in header file.
void vdprintf(const char * format, va_list argPtr, const char * prefix)
{
#ifdef _WIN32
    char buf[2048];
    char * pBuf = buf;
    int len = 0;
    if (prefix)
    {
        len = strlen(prefix);
        memcpy(pBuf, prefix, len);
        pBuf += len;
        if (log_msg_mgr_t::shouldWriteToConsole)
        {
            fprintf(stderr, "%s", prefix);
        }
    }

    _vsnprintf(pBuf, sizeof(buf) - len, format, argPtr);
    buf[sizeof(buf) - 1] = 0;

    std::wstring wbuf;
    if (!multibyte2wchar(buf, wbuf))
    {
        wbuf = L"[Error converting message string to wide]\n";
    }

    OutputDebugString(wbuf.c_str());
    if (log_msg_mgr_t::shouldWriteToConsole)
    {
        vfprintf(stderr, format, argPtr);
    }
#else
    if (log_msg_mgr_t::shouldWriteToConsole)
    {
        if (log_msg_mgr_t::shouldWriteTimePrefix)
        {
            time_t ttime = time(NULL);
            char timebuff[32];

            if (0
                    != strftime(timebuff, sizeof(timebuff), "%F %T: ",
                            localtime(&ttime)))
            {
                fprintf(stderr, timebuff);
            }
        }

        if (prefix)
        {
            fprintf(stderr, "%s", prefix);
        }

        // Need to make a copy of the va_list here to avoid crashing on 64bit
        va_list tmpArgPtr;
        va_copy(tmpArgPtr, argPtr);
        vfprintf(stderr, format, tmpArgPtr);
        va_end(tmpArgPtr);
    }
#endif
}

/*prints a debug message if the input priorities satisfies thefilter*/
void vdprintf(int componentPriority, int messagePriority, const char * format,
        va_list argPtr, const char * prefix)
{
    vdprintf(format, argPtr, prefix);
}
#endif
