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

// created on 28-June-2016 by Jackie Zhang
#ifndef TIMESTAMP_HPP
#define TIMESTAMP_HPP

#include "../plateform.h"
#include "debug.h"

namespace geco
{
    namespace debugging
    {
        // Indicates whether or not to use a call to RDTSC (Read Time Stamp Counter)
        // to calculate timestamp. The benefit of using this is that it is fast and
        // accurate, returning actual clock ticks. The downside is that this does not
        // work well with CPUs that use Speedstep technology to vary their clock speeds.
        //
        // Alternate Linux implementation uses gettimeofday. In rough tests, this can
        // be between 20 and 600 times slower than using RDTSC. Also, there is a problem
        // under 2.4 kernels where two consecutive calls to gettimeofday may actually
        // return a result that goes backwards.
#ifndef _XBOX360
# define GECO_USE_RDTSC
#endif // _XBOX360

        /**
        * This function returns the processor's (real-time) clock cycle counter.
        */
#if defined(__unix__) || defined(__linux__)
        inline uint64 gettimestamp()
        {
            uint32 rethi, retlo;
            __asm__ __volatile__(
                // Read Time-Stamp Counter
                // loads current value of processor's timestamp counter into EDX:EAX
                "rdtsc":
            "=d"    (rethi),
                "=a"    (retlo)
                );
            return uint64(rethi) << 32 | retlo;
        }
#elif defined(_WIN32)
#ifdef GECO_USE_RDTSC
#pragma warning (push)
#pragma warning (disable: 4035)
#ifdef _AMD64_
        extern "C" uint64 _stdcall asm_time();
#define gettimestamp() asm_time()
#else
        inline uint64 gettimestamp()
        {
            __asm rdtsc
        }
#endif
#pragma warning (pop)
#else // GECO_USE_RDTSC
#ifdef _XBOX360
#include <xtl.h>
#else // _XBOX360
#include <windows.h>
#endif // _XBOX360
        inline uint64 gettimestamp()
        {
            LARGE_INTEGER counter;
            QueryPerformanceCounter(&counter);
            return counter.QuadPart;
        }
#endif
#elif defined( PLAYSTATION3 )
        inline uint64 gettimestamp()
        {
            uint64 ts;
            SYS_TIMEBASE_GET(ts);
            return ts;
        }
#else
#error Unsupported platform!
#endif

        /**
        *	This function tells you how many there are in a second. It caches its reply
        *	after being called for the first time, however that call may take some time.
        */
        uint64 stamps_per_sec();

        /**
        *	This function tells you how many there are in a second as a double precision
        *	floating point value. It caches its reply after being called for the first
        *	time, however that call may take some time.
        */
        inline double stamps_per_sec_double()
        {
            static double stampsPerSecondCacheD = double(stamps_per_sec());
            return stampsPerSecondCacheD;
        }
        inline double stamps2sec(uint64 stamps)
        {
            return double(stamps) / stamps_per_sec_double();
        }

        // ---------------------------------------------------------------------
        // Section: TimeStamp
        // ---------------------------------------------------------------------

        /**
         *	This class stores a value in stamps but has access functions in seconds.
         */
        struct time_stamp_t
        {
            time_stamp_t(uint64 stamps = 0) : stamp_(stamps) {}

            operator uint64 &() { return stamp_; }
            operator uint64() const { return stamp_; }

            /**
            *	This method returns this timestamp in seconds.
            */
            double inSeconds() const
            {
                return toSeconds(stamp_);
            }
            /**
            *	This method sets this timestamp from seconds.
            */
            inline void setInSeconds(double seconds)
            {
                stamp_ = fromSeconds(seconds);
            }

            /**
            *	This method returns the number of stamps from this TimeStamp to now.
            */
            inline time_stamp_t ageInStamps() const
            {
                return gettimestamp() - stamp_;
            }
            /**
            *	This method returns the number of seconds from this TimeStamp to now.
            */
            inline double ageInSeconds() const
            {
                return toSeconds(this->ageInStamps());
            }

            /**
            *	This static method converts a timestamp value into seconds.
            */
            static double toSeconds(uint64 stamps)
            {
                return double(stamps) / stamps_per_sec_double();
            }
            /**
            *	This static method converts seconds into timestamps.
            */
            static time_stamp_t fromSeconds(double seconds)
            {
                return uint64(seconds * stamps_per_sec_double());
            }

            uint64	stamp_;
        };
    }
}
#endif