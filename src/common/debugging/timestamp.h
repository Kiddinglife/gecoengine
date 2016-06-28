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
#ifndef __unix__
# define GECO_USE_RDTSC
#endif 
#endif // _XBOX360

        /**
        * This function returns the processor's (real-time) clock cycle counter.
        */
#ifdef unix
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
        extern "C" uint64 asm_gettimestamp();
#ifdef _AMD64_
#define gettimestamp() asm_gettimestamp()
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

        uint64 stamps_per_sec();
        double double_stamps_per_sec();
        inline double stamps2sec(uint64 stamps)
        {
            return double(stamps) / double_stamps_per_sec();
        }

    }
}
#endif