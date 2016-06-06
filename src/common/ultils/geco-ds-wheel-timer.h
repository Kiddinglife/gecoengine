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

// created on 06-June-2016 by Jackie Zhang
#ifndef SRC_COMMON_DS_WHEELTIMERMGRT_H_
#define SRC_COMMON_DS_WHEELTIMERMGRT_H_
#include <stdbool.h>    /* bool */
#include <stdio.h>      /* FILE */
#include <inttypes.h>   /* PRIu64 PRIx64 PRIX64 uint64_t */
#include <list>

/* type defines */
#define TIMEOUT_C(n) UINT64_C(n)
#define TIMEOUT_PRIu PRIu64
#define TIMEOUT_PRIx PRIx64
#define TIMEOUT_PRIX PRIX64
#define TIMEOUT_mHZ TIMEOUT_C(1000)
#define TIMEOUT_uHZ TIMEOUT_C(1000000)
#define TIMEOUT_nHZ TIMEOUT_C(1000000000)
typedef uint64_t timeout_t;
#define timeout_error_t int /* for documentation purposes */

#define REPEAT_PERIOD 0x01 /* interval (repeating) timeout */
#define TIMEOUT_ABS 0x02 /* treat timeout values as absolute */
#define TIMEOUT_INITIALIZER(flags) { (flags) }
#define timeout_setcb(to, fn, arg) do { \
    (to)->callback.fn = (fn);       \
    (to)->callback.arg = (arg);     \
} while (0)

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
                /*if NULL, fn is c style function, otherwise member method*/
                virtual int handle(int etype, void* args) = 0;
        };
        struct timeout_cb
        {
                void (*fn)();
                void *arg;
        };

        struct timeout
        {
                int flags;
                timeout_t expires;/* absolute expiration time */
                std::list<timeout*>* pending;/* timeout list if pending on wheel or expiry queue */
                std::list<timeout*>** timeout_list_list;
                struct timeout_cb callback; /* optional callback information */
                timeout_t interval;/* timeout interval if periodic */
                struct timeouts *timeouts;/* timeouts collection if member of */
        };

        struct wheel_timer_mgr_t
        {
                wheel_timer_mgr_t();
                virtual ~wheel_timer_mgr_t();
        };

    } /* namespace ds */
} /* namespace geco */

#endif /* SRC_COMMON_DS_WHEELTIMERMGRT_H_ */
