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
#include "../ultils/geco-engine-ultils.h"

namespace geco
{
    namespace debugging
    {
        // global variables
        bool g_write2syslog = false;
        std::string  g_syslog_name;
        static const char dev_assertion_msg[] =
            "Development assertions may indicate failures caused by incorrect "
            "engine usage.\n"
            "In "
#ifdef MF_SERVER
            "production mode"
#else
            "Release builds"
#endif
            ", they do not cause the application to exit.\n"
            "Please investigate potential misuses of the engine at the time of the "
            "failure.\n";

        // init static variables
        log_msg_filter_t* log_msg_filter_t::s_instance_ = NULL;
        bool log_msg_filter_t::shouldWriteTimePrefix = false;
#if defined( SERVER_BUILD ) || defined( PLAYSTATION3 )
        bool log_msg_filter_t::shouldWriteToConsole = true;
#else
        bool log_msg_filter_t::shouldWriteToConsole = false;
#endif

#if ENABLE_DPRINTF
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
                if (log_msg_filter_t::shouldWriteToConsole)
                {
                    fprintf(stderr, "%s", prefix);
                }
            }

            _vsnprintf(pBuf, sizeof(buf) - len, format, argPtr);
            buf[sizeof(buf) - 1] = 0;

            std::wstring wbuf;
            if (!geco::ultils::multibyte2wchar(buf, wbuf))
            {
                wbuf = L"[Error converting message string to wide]\n";
            }

            OutputDebugString(wbuf.c_str());
            if (log_msg_filter_t::shouldWriteToConsole)
            {
                vfprintf(stderr, format, argPtr);
            }
#else
            if (log_msg_filter_t::shouldWriteToConsole)
            {
                if (log_msg_filter_t::shouldWriteTimePrefix)
                {
                    time_t ttime = time(NULL);
                    char timebuff[32];

                    if (strftime(timebuff, sizeof(timebuff), "%F %T: ",
                        localtime(&ttime)) != 0)
                    {
                        fprintf(stderr, "%s", timebuff);
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

        void dprintf(const char * format, ...)
        {
            va_list argPtr;
            va_start(argPtr, format);
            vdprintf(format, argPtr);
            va_end(argPtr);
        }

        /**
         *	This function prints a debug message if the input priorities satisfies the
         *	filter.
         */
        void dprintf(int componentLevel, int severity, const char * format, ...)
        {
            va_list argPtr;
            va_start(argPtr, format);
            vdprintf(componentLevel, severity, format, argPtr);
            va_end(argPtr);
        }
#endif


        /**
        *	This is a helper function used by the CRITICAL_MSG macro.
        */
        void log_msg_helper::critical_msg(const char * format, ...)
        {
            va_list argPtr;
            va_start(argPtr, format);
            this->critical_msg_aux(true, format, argPtr);
            va_end(argPtr);
        }

        void log_msg_helper::dev_critical_msg(const char * format, ...)
        {
            va_list argPtr;
            va_start(argPtr, format);
            this->critical_msg_aux(true, format, argPtr);
            va_end(argPtr);
        }

        /**
        *	This is a helper function used by the CRITICAL_MSG macro.
        */
        void log_msg_helper::critical_msg_aux(bool isDevAssertion, const char * format, va_list argPtr)
        {
            char buf[LOG_BUFSIZ];
            geco_vsnprintf(buf, sizeof(buf), format, argPtr);
            buf[sizeof(buf) - 1] = '\0';
            log_msg_helper::critical_msg_occurs_ = true;

#if ENABLE_STACK_TRACKER


#endif
        }

    }
}
