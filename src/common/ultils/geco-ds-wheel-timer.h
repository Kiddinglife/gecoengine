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

/**
 * wheel 1 has 256 slots
 * wheel 2 has 64  slots
 * wheel 3 has 64  slots
 * wheel 4 has 64  slots
 * wheel 5 has 64  slots
 *
 *In practice it's done by categorizing all future jiffies into 5 wheels:
 * wheel 1  jiffies of [1, 2^8]  1..256,
 * wheel 2  jiffies of [2^8, 2^14] 257..16384,
 * wheel 3  jiffies of [2^14, 2^20] 16385..1048576,
 * wheel 4  jiffies of [2^20, 2^26] 1048577..67108864,
 * wheel 5  jiffies of [2^26, 2^32] 67108865..4294967295
 *
 * the first wheel consists of 256 buckets
 * (each bucket representing a single jiffy),
 * the second category consists of 64 buckets equally divided
 * (each bucket represents 256 subsequent jiffies),
 * the third category consists of 64 buckets too
 * (each bucket representing 256*64 ==16384 jiffies),
 * the fourth category consists of 64 buckets too
 * (each bucket representing 256*64*64 == 1048576 jiffies),
 * the fifth category consists of 64 buckets too (each bucket representing 67108864 jiffies).
 *
 */

/*
 * created on 06-June-2016 by Jackie Zhang
 */

#ifndef SRC_COMMON_DS_WHEELTIMERMGRT_H_
#define SRC_COMMON_DS_WHEELTIMERMGRT_H_
#include <stdbool.h>    /* bool */
#include <stdio.h>      /* FILE */
#include <inttypes.h>   /* PRIu64 PRIx64 PRIX64 uint64_t */
#include <list>
#include <functional>
#include <limits.h>    /* CHAR_BIT */
#include <stddef.h>    /* NULL */
#include <stdlib.h>    /* malloc(3) free(3) */
#include <string.h>    /* memset(3) */
#if defined(__linux__) || defined(__unix__)
#include <errno.h>     /* errno */
#define get_last_error() errno
#else
#define get_last_error() WSAGetLastError()
#endif
#include "geco-engine-ultils.h"
#include "geco-malloc.h"

/* type defines */
#define TIMEOUT_C(n) UINT64_C(n)
#define TIMEOUT_PRIu PRIu64
#define TIMEOUT_PRIx PRIx64
#define TIMEOUT_PRIX PRIX64
#define TIMEOUT_mHZ TIMEOUT_C(1000)
#define TIMEOUT_uHZ TIMEOUT_C(1000000)
#define TIMEOUT_nHZ TIMEOUT_C(1000000000)
#define timeout_error_t int /* for documentation purposes */

#define REPEAT_TIMER 0x01 /* interval (repeating) TIMER */
#define ABS_TIMEOUT 0x02 /* treat timeout values as absolute */
#define TIMEOUT_INITIALIZER(flags) { (flags) }
#define timeout_setcb(to, fn, arg) do { \
    (to)->callback.fn = (fn);       \
    (to)->callback.arg = (arg);     \
} while (0)

/*
 * helpers
 * * * * * */
#define countof(a) (sizeof (a) / sizeof *(a))
#define endof(a) (&(a)[countof(a)])
#define MIN(a, b) (((a) < (b))? (a) : (b))
#define MAX(a, b) (((a) > (b))? (a) : (b))

/*
 * B I T  M A N I P U L A T I O N  R O U T I N E S
 *
 * The macros and routines below implement wheel parameterization. The
 * inputs are:
 *
 *   WHEEL_BIT - The number of value bits mapped in each wheel. The
 *               lowest-order WHEEL_BIT bits index the lowest-order (highest
 *               resolution) wheel, the next group of WHEEL_BIT bits the
 *               higher wheel, etc.
 *
 *   WHEEL_NUM - The number of wheels. WHEEL_BIT * WHEEL_NUM = the number of
 *               value bits used by all the wheels. For the default of 6 and
 *               4, only the low 24 bits are processed. Any timeout value
 *               larger than this will cycle through again.
 *
 * The implementation uses bit fields to remember which slot in each wheel
 * is populated, and to generate masks of expiring slots according to the
 * current update interval (i.e. the "tickless" aspect). The slots to
 * process in a wheel are (populated-set & interval-mask).
 *
 * WHEEL_BIT cannot be larger than 6 bits because 2^6 -> 64 is the largest
 * number of slots which can be tracked in a uint64_t integer bit field.
 * WHEEL_BIT cannot be smaller than 3 bits because of our rotr and rotl
 * routines, which only operate on all the value bits in an integer, and
 * there's no integer smaller than uint8_t.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define WHEEL_BIT 6
#define WHEEL_NUM 4
#define WHEEL_LEN (1U << WHEEL_BIT) // 2^6 = 64
#define WHEEL_MAX (WHEEL_LEN - 1) // 63
#define WHEEL_MASK (WHEEL_LEN - 1) // 63 = 01111111
#define TIMEOUT_MAX \
((TIMEOUT_C(1) << (WHEEL_BIT * WHEEL_NUM)) - 1) //2^24-1

#if WHEEL_BIT == 6
#define ctz(n) ctz64(n)
#define clz(n) clz64(n)
#define fls(n) ((int)(64 - clz64(n)))
#else
#define ctz(n) ctz32(n)
#define clz(n) clz32(n)
#define fls(n) ((int)(32 - clz32(n)))
#endif

#if WHEEL_BIT == 6
#define WHEEL_C(n) UINT64_C(n)
#define WHEEL_PRIu PRIu64
#define WHEEL_PRIx PRIx64
typedef uint64_t wheel_t;
#elif WHEEL_BIT == 5
#define WHEEL_C(n) UINT32_C(n)
#define WHEEL_PRIu PRIu32
#define WHEEL_PRIx PRIx32
typedef uint32_t wheel_t;
#elif WHEEL_BIT == 4
#define WHEEL_C(n) UINT16_C(n)
#define WHEEL_PRIu PRIu16
#define WHEEL_PRIx PRIx16
typedef uint16_t wheel_t;
#elif WHEEL_BIT == 3
#define WHEEL_C(n) UINT8_C(n)
#define WHEEL_PRIu PRIu8
#define WHEEL_PRIx PRIx8
typedef uint8_t wheel_t;
#else
#error invalid WHEEL_BIT value
#endif

#define wheel_list_type 0
#define expired_list_type 1
#define unset_list_type 2

namespace geco
{
    namespace ds
    {
        /*
         * Callback function parameters unspecified to make embedding into existing
         * applications easier, you can always force cast the func ptr to the one you need
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
        struct event_handler_t
        {
                unsigned int event_type;
                unsigned int owner_type;
                void* owner;
                void (*fn)();
                void *arg;
        };

        struct timeout_cb
        {
                void (*fn)();
                void *arg;
        };
    } /* namespace ds */
} /* namespace geco */

#endif /* SRC_COMMON_DS_WHEELTIMERMGRT_H_ */
