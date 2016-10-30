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

#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#include <time.h>
#include <process.h>
#pragma warning( disable: 4995 ) // disable warning for unsafe string functions
//#include "critical_message_box.hpp"
#pragma comment( lib, "winmm" )
#elif defined( PLAYSTATION3 )
#else
#include <unistd.h>
#include <syslog.h>
#endif

#include "debug.h"
#include "stack_tracker_t.h"
#include "timestamp.h"
#include "../../plateform.h"
#include "../ultils/geco-malloc.h"

DECLARE_DEBUG_COMPONENT2("COMM", 0);

namespace geco
{
    namespace debugging
    {
        //-------------------------------------------------------
        //	Section: globals
        //-------------------------------------------------------
        bool g_write2syslog = false;
        std::string g_syslog_name("default_syslog_name");
        static const char dev_assertion_msg[] = "Development assertions may indicate failures caused by incorrect "
            "engine usage.\n"
            "In "
#ifdef SERVER_BUILD
            "production mode"
#else
            "Release builds"
#endif
            ", they do not cause the application to exit.\n"
            "Please investigate potential misuses of the engine at the time of the "
            "failure.\n";

        static const char * prefixes[] =
        { "VERBOSE: ", "DEBUG: ", "INFO: ", "NOTICE: ", "WARN: ", "ERR: ", "CRIT: ", "HACK: ", "SCRIPT: ", "ASSET: " };

        inline const char * get_logmsg_prefix_str(LogMsgPriority p)
        {
            return (p >= 0 && (size_t)p < sizeof(prefixes)) ? prefixes[(int)p] : "";
        }

        //-------------------------------------------------------
        //	Section: printf functions
        //-------------------------------------------------------
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
            if (!geco::ultils::win_geco_acp2w(buf, wbuf))
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

                    if (strftime(timebuff, sizeof(timebuff), "%F %T: ", localtime(&ttime)) != 0)
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
        void vdprintf(int componentPriority, int messagePriority, const char * format, va_list argPtr,
            const char * prefix)
        {
            if (messagePriority >= componentPriority)
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
            if (severity < componentLevel) return;
            va_list argPtr;
            va_start(argPtr, format);
            vdprintf(componentLevel, severity, format, argPtr);
            va_end(argPtr);
        }
#endif

        //-------------------------------------------------------
        //	Section: main_thread_tracker_t
        //-------------------------------------------------------
        bool thread_local main_thread_tracker_t::is_curr_thread_main_thread_ = false;
        // Instantiate it, so it initialises the flag to the main thread
        static main_thread_tracker_t s_main_thread_tracker;

        //-------------------------------------------------------
        //	Section: default_critical_msg_handler_t
        //-------------------------------------------------------
#if defined(_WIN32) && !defined(_XBOX)
        class win32_critical_msg_handler_t : public default_critical_msg_handler_t
        {
            virtual Result ask(const char* msg)
            {
#ifdef BUILT_BY_GECO
                CriticalMsgBox mb(msg, true);
#else
                // TODO CriticalMsgBox mb(msg, false);
#endif
                //if (mb.doModal())
                //    return ENTERDEBUGGER;

                return EXITDIRECTLY;
            }
            virtual void recordInfo(bool willExit)
            {}
        };
        // win32 has default handler inilized
        static win32_critical_msg_handler_t win32_critical_msg_handler;
        default_critical_msg_handler_t*
            default_critical_msg_handler_t::handler_ = &win32_critical_msg_handler;
#else//defined(_WIN32)
        default_critical_msg_handler_t* default_critical_msg_handler_t::handler_ =
            NULL;
#endif//defined(_WIN32)

        //-------------------------------------------------------
        //	Section: log_msg_filter_t
        //-------------------------------------------------------
        log_msg_filter_t* log_msg_filter_t::s_instance_ = NULL;
        bool log_msg_filter_t::shouldWriteTimePrefix = true;
#if defined( SERVER_BUILD ) || defined( PLAYSTATION3 )
        bool log_msg_filter_t::shouldWriteToConsole = true;
#else
        bool log_msg_filter_t::shouldWriteToConsole = false;
#endif

        //-------------------------------------------------------
        // Section: log_msg_helper
        //-------------------------------------------------------

        /*static*/bool log_msg_helper::show_error_dialogs_ = true;
        /*static*/bool log_msg_helper::critical_msg_occurs_ = false;
        /*static*/std::mutex* log_msg_helper::mutex_ = NULL;
#ifdef WIN32
        /*static*/bool log_msg_helper::automated_test_ = false;
        void log_msg_helper::log2file(const char * line)
        {
            if (!automated_test_) return;

            FILE* log = fopen("debug.log", "a");
            if (!log) return;
            time_t rawtime;
            time(&rawtime);
            fprintf(log, "\n%s%s\n", ctime(&rawtime), line);
            fclose(log);
        }
#endif

        /*
         *	This is a helper function used by the CRITICAL_MSG macro.
         */
        void log_msg_helper::critical_msg(const char * format, ...)
        {
            va_list argPtr;
            va_start(argPtr, format);
            this->critical_msg_aux(false, format, argPtr);
            va_end(argPtr);
        }
        void log_msg_helper::dev_critical_msg(const char * format, ...)
        {
            va_list argPtr;
            va_start(argPtr, format);
            this->critical_msg_aux(true, format, argPtr);
            va_end(argPtr);
        }

#if defined(__unix__) || defined(__linux__)
#define MAX_DEPTH 50
#include <execinfo.h>
#include <cxxabi.h>
        void log_msg_helper::msg_back_trace()
        {
            void** trace_buf = new void*[MAX_DEPTH];
            uint32 depth = backtrace(trace_buf, MAX_DEPTH);
            char ** traceStringBuffer = backtrace_symbols(trace_buf, depth);
            if (traceStringBuffer == NULL)
            {
                perror("backtrace_symbols.");
                exit(EXIT_FAILURE);
            }

            for (uint32 i = 0; i < depth; i++)
            {
                // Format: <executable path>(<mangled-function-name>+<function instruction offset>) [<eip>]
                std::string functionName;
                std::string traceString(traceStringBuffer[i]);
                std::string::size_type begin = traceString.find('(');

                bool gotFunctionName = (begin >= 0);
                if (gotFunctionName)
                {
                    // Skip the round bracket start.
                    ++begin;
                    std::string::size_type bracketEnd = traceString.find(')', begin);
                    std::string::size_type end = traceString.rfind('+', bracketEnd);
                    std::string mangled(traceString.substr(begin, end - begin));

                    int status = 0;
                    size_t demangledBufferLength = 0;
                    char * demangledBuffer = abi::__cxa_demangle(mangled.c_str(), 0, &demangledBufferLength, &status);

                    if (demangledBuffer)
                    {
                        functionName.assign(demangledBuffer, demangledBufferLength);
                        // __cxa_demangle allocates the memory for the demangled
                        // output using malloc(), we need to free it.
#ifdef ENABLE_MEMTRACKER
                        free(demangledBuffer);
                        //todo raw_free( demangledBuffer );
#else
                        free(demangledBuffer);
#endif
                    }
                    else
                    {
                        // Didn't demangle, but we did get a function name, use that.
                        functionName = mangled;
                    }
                }
                this->message("Stack: #%d %s\n", i, (gotFunctionName) ? functionName.c_str() : traceString.c_str());
            }
#ifdef ENABLE_MEMTRACKER
            free(traceStringBuffer);  // this is incorrect should fix it up someday
            //todo raw_free(traceStringBuffer);
#else
            free(traceStringBuffer);
#endif
            delete[] trace_buf;
        }
#else
        inline void log_msg_helper::msg_back_trace()
        {}
#endif

        /*
         * 	This is a helper function used by the CRITICAL_MSG macro.
         */
        void log_msg_helper::critical_msg_aux(bool isDevAssertion, const char * format, va_list argPtr)
        {
            char buffer[LOG_BUFSIZ];
            geco_vsnprintf(buffer, sizeof(buffer), format, argPtr);
            buffer[sizeof(buffer) - 1] = '\0';
            log_msg_helper::critical_msg_occurs_ = true;

#if ENABLE_STACK_TRACKER
            if (stack_tracker_t::stackSize() > 0)
            {
                std::string stack = stack_tracker_t::report();
                strcat(buffer, NEW_LINE);
                strcat(buffer, "Stack trace: ");
                strcat(buffer, stack.c_str());
                strcat(buffer, NEW_LINE);
            }
#endif

            // send to syslog if it's been initialised
#if !defined( _WIN32 ) && !defined( PLAYSTATION3 )
            if (g_write2syslog)
            {
                syslog(LOG_CRIT, "%s", buffer);
            }
#endif

            // output it as a normal  message
            this->message("%s", buffer);
            if (isDevAssertion)
            {
                this->message("%s", dev_assertion_msg);
            }
            this->msg_back_trace();
            if (isDevAssertion && !log_msg_filter_t::get_instance().has_dev_assert_)
            {
                // dev assert and we don't have dev asserts enabled
                return;
            }

            // now do special critical message stuff
            for (uint i = 0; i < log_msg_filter_t::get_instance().critical_msg_cbs_.size(); ++i)
            {
                va_list tmpArgPtr;
                geco_va_copy(tmpArgPtr, argPtr);
                (*(log_msg_filter_t::get_instance().critical_msg_cbs_[i]))(cpn_priority_, msg_priority_, format,
                    tmpArgPtr, (critical_msg_cb_tag*)0);
                va_end(tmpArgPtr);
            }

#ifdef _XBOX360
            {
                OutputDebugStringA(buffer);

                LPCWSTR buttons[] =
                { L"Exit" };
                XOVERLAPPED overlapped;  // Overlapped object for message box UI
                MESSAGEBOX_RESULT result;// Message box button pressed result

                ZeroMemory(&overlapped, sizeof(XOVERLAPPED));

                char tcbuffer[BW_DEBUG_BUFSIZ * 2];
                WCHAR wcbuffer[BW_DEBUG_BUFSIZ * 2];

                vsnprintf(tcbuffer, ARRAY_SIZE(tcbuffer), format, argPtr);
                tcbuffer[sizeof(tcbuffer) - 1] = '\0';

                MultiByteToWideChar(CP_UTF8, 0, tcbuffer, -1, wcbuffer, ARRAYSIZE(wcbuffer));

                DWORD dwRet = XShowMessageBoxUI(0,
                    L"Critical Error",// Message box title
                    wcbuffer,// Message string
                    ARRAYSIZE(buttons),// Number of buttons
                    buttons,// Button captions
                    0,// Button that gets focus
                    XMB_ERRORICON,// Icon to display
                    &result,// Button pressed result
                    &overlapped);

                //assert( dwRet == ERROR_IO_PENDING );

                while (!XHasOverlappedIoCompleted(&overlapped))
                {
                    extern IDirect3DDevice9 * g_pd3dDevice;
                    g_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0L);
                    g_pd3dDevice->Present(0, 0, NULL, 0);
                }

                for (int i = 0; i < 60; i++)
                {
                    extern IDirect3DDevice9 * g_pd3dDevice;
                    g_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0L);
                    g_pd3dDevice->Present(0, 0, NULL, 0);
                }

                XLaunchNewImage("", 0);

                //ENTER_DEBUGGER();
            }

#elif defined ( PLAYSTATION3 )
            printf(buffer);
            printf("\n");
            ENTER_DEBUGGER();
#elif defined(_WIN32)

            if (automated_test_)
            {
                _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
                log2file("Critical Error, aborting test.");
                log2file(buffer);
                abort();
            }

#if ENABLE_ENTER_DEBUGGER_MESSAGE
            // Disable all abort() behaviour in case we call it
            _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
            if (default_critical_msg_handler_t::get() != NULL)
            {
                switch (default_critical_msg_handler_t::get()->ask(buffer))
                {
                case default_critical_msg_handler_t::ENTERDEBUGGER:
                    default_critical_msg_handler_t::get()->recordInfo(false);
                    ENTER_DEBUGGER();
                    break;
                case default_critical_msg_handler_t::EXITDIRECTLY:
                    default_critical_msg_handler_t::get()->recordInfo(true);
                    abort();
                    break;
                }
            }
            else
                abort();
#else // ENABLE_ENTER_DEBUGGER_MESSAGE
            strcat(buffer, "\n\nThe application must exit.\n");
            ::MessageBox(0, geco_utf8tow(buffer).c_str(), L"Critical Error Occurred", MB_ICONHAND | MB_OK);
            abort();
#endif// ENABLE_ENTER_DEBUGGER_MESSAGE
#else
            char filename[512], hostname[256];
            if (gethostname(hostname, sizeof(hostname)) != 0) hostname[0] = 0;

            char exeName[512];
            const char * pExeName = "unknown";

            int len = readlink("/proc/self/exe", exeName, sizeof(exeName) - 1);
            if (len > 0)
            {
                exeName[len] = '\0';

                char * pTemp = strrchr(exeName, '/');
                if (pTemp != NULL)
                {
                    pExeName = pTemp + 1;
                }
            }

            geco_snprintf(filename, sizeof(filename), "assert.%s.%s.%d.log", pExeName, hostname, getpid());

            FILE * assertFile = geco_fopen(filename, "a");
            fprintf(assertFile, "%s", buffer);
            fclose(assertFile);

            volatile uint64 crashTime = gettimestamp();  // For reference in the coredump.
            crashTime *= 1;
            *(int*)NULL = 0;
            typedef void(*BogusFunc)();
            ((BogusFunc)NULL)();
#endif // defined(_WIN32)
        }

        void log_msg_helper::message(const char * format, ...)
        {
            // Break early if this message should be filtered out.
            if (!log_msg_filter_t::should_accept(cpn_priority_, msg_priority_)) return;

            bool handled = false;
            va_list argPtr;
            va_start(argPtr, format);

#if !defined( _WIN32 ) && !defined( PLAYSTATION3 )
            // send to syslog if it's been initialised
            if (should_write2syslog && ((msg_priority_ == LOG_MSG_ERROR) || (msg_priority_ == LOG_MSG_CRITICAL)))
            {
                char buffer[LOG_BUFSIZ * 2];
                // Need to make a copy of the va_list here to avoid crashing on 64bit
                va_list tmpArgPtr;
                geco_va_copy(tmpArgPtr, argPtr);
                vsnprintf(buffer, sizeof(buffer), format, tmpArgPtr);
                buffer[sizeof(buffer) - 1] = '\0';
                va_end(tmpArgPtr);

                if (msg_priority_ == LOG_MSG_CRITICAL)
                {
                    syslog(LOG_CRIT, "%s", buffer);
                }
                else
                {
                    syslog(LOG_ERR, "%s", buffer);
                }
            }
#endif

            switch (msg_priority_)
            {
            case LOG_MSG_CRITICAL:
                // handle this individually in critical msg()
                break;
            case LOG_MSG_ERROR:
                for (uint i = 0; i < log_msg_filter_t::get_instance().error_msg_cbs_.size(); ++i)
                {
                    if (!handled)
                    {
                        va_list tmpArgPtr;
                        geco_va_copy(tmpArgPtr, argPtr);
                        handled = (*(log_msg_filter_t::get_instance().error_msg_cbs_[i]))(cpn_priority_,
                            msg_priority_, format, tmpArgPtr, (error_msg_cb_tag*)0);
                        va_end(tmpArgPtr);
                    }
                }
                break;
            case LOG_MSG_WARNING:
                for (uint i = 0; i < log_msg_filter_t::get_instance().warnning_msg_cbs_.size(); ++i)
                {
                    if (!handled)
                    {
                        va_list tmpArgPtr;
                        geco_va_copy(tmpArgPtr, argPtr);
                        handled = (*(log_msg_filter_t::get_instance().warnning_msg_cbs_[i]))(cpn_priority_,
                            msg_priority_, format, tmpArgPtr, (warnning_msg_cb_tag*)0);
                        va_end(tmpArgPtr);
                    }
                }
                break;
            case LOG_MSG_NOTICE:
                for (uint i = 0; i < log_msg_filter_t::get_instance().notice_msg_cbs_.size(); ++i)
                {
                    if (!handled)
                    {
                        va_list tmpArgPtr;
                        geco_va_copy(tmpArgPtr, argPtr);
                        handled = (*(log_msg_filter_t::get_instance().notice_msg_cbs_[i]))(cpn_priority_,
                            msg_priority_, format, tmpArgPtr, (notice_msg_cb_tag*)0);
                        va_end(tmpArgPtr);
                    }
                }
                break;
            case LOG_MSG_INFO:
                for (uint i = 0; i < log_msg_filter_t::get_instance().info_msg_cbs_.size(); ++i)
                {
                    if (!handled)
                    {
                        va_list tmpArgPtr;
                        geco_va_copy(tmpArgPtr, argPtr);
                        handled = (*(log_msg_filter_t::get_instance().info_msg_cbs_[i]))(cpn_priority_,
                            msg_priority_, format, tmpArgPtr, (info_msg_cb_tag*)0);
                        va_end(tmpArgPtr);
                    }
                }
                break;
            case LOG_MSG_DEBUG:
                for (uint i = 0; i < log_msg_filter_t::get_instance().debug_msg_cbs_.size(); ++i)
                {
                    if (!handled)
                    {
                        va_list tmpArgPtr;
                        geco_va_copy(tmpArgPtr, argPtr);
                        handled = (*(log_msg_filter_t::get_instance().debug_msg_cbs_[i]))(cpn_priority_,
                            msg_priority_, format, tmpArgPtr, (debug_msg_cb_tag*)0);
                        va_end(tmpArgPtr);
                    }
                }
                break;
            default:  // treat other msg as criti msg
                for (uint i = 0; i < log_msg_filter_t::get_instance().critical_msg_cbs_.size(); ++i)
                {
                    if (!handled)
                    {
                        va_list tmpArgPtr;
                        geco_va_copy(tmpArgPtr, argPtr);
                        handled = (*(log_msg_filter_t::get_instance().critical_msg_cbs_[i]))(cpn_priority_,
                            msg_priority_, format, tmpArgPtr, (critical_msg_cb_tag*)0);
                        va_end(tmpArgPtr);
                    }
                }
                break;
            }

            // defaut handler is simply to print it out if no cb invoked
            if (!handled)
            {
                if (0 <= msg_priority_&&
                    msg_priority_ < int(sizeof(prefixes) / sizeof(prefixes[0])) &&
                    prefixes[msg_priority_] != NULL)
                {
                    vdprintf(cpn_priority_, msg_priority_, format, argPtr, prefixes[msg_priority_]);
                }
                else
                {
                    vdprintf(cpn_priority_, msg_priority_, format, argPtr);
                }
            }
            va_end(argPtr);
        }
    }
}
