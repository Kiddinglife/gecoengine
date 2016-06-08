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
#include "geco-ds-wheel-timer.h"
#include <assert.h>

namespace geco
{
    namespace ultils
    {
        inline void wtimer_t::init(int wt_flags)
        {
            memset(this, 0, sizeof(wtimer_t));
            flags = wt_flags;
        }
#ifndef TIMEOUT_DISABLE_RELATIVE_ACCESS
        /* true if on timing wheel, false otherwise */
        inline bool wtimer_t::pending_wheel()
        {
            return this->pending != NULL
                    && this->pending != &this->wtimers->expired;
        }
        /* true if on expired queue, false otherwise */
        inline bool wtimer_t::pending_expired(wtimer_t *)
        {
            return pending != NULL && pending == &wtimers->expired;
        }
        /* remove timeout from any timing wheel (okay if not member of any) */
        inline void wtimer_t::stop()
        {
            wtimers->stop_timer(this);
        }
#endif

        wtimers_t::wtimers_t(time_t hz)
        {
            unsigned int i;
            for (i = 0; i < WHEEL_NUM; i++)
            {
                this->pending[i] = 0;
            }
            assert(i == 3);
            this->curtime = 0;
            this->hertz = (hz) ? hz : TIMEOUT_mHZ;
        }

        wtimer_t* wtimers_t::get_first_expired_timer()
        {
            if (!this->expired.empty())
            {
                wtimer_t* wt = this->expired.front();
                wt->pending = NULL;
                wt->wtimers = NULL;
#ifndef TIMEOUT_DISABLE_INTERVALS
                if ((wt->flags & REPEAT_TIMER) && wt->interval > 0)
                    readd_timer(wt);
#endif
                return wt;
            }
            else
            {
                return NULL;
            }
        }

        inline void wtimers_t::add_timer(wtimer_t *to, time_t timeout)
        {
#ifndef TIMEOUT_DISABLE_INTERVALS
            if (to->flags & REPEAT_TIMER)
                to->interval = MAX(1, timeout);
#endif
            if (to->flags & ABS_TIMEOUT)
                sche_timer(to, timeout);
            else
                sche_timer(to, this->curtime + timeout);
        }
        void wtimers_t::readd_timer(wtimer_t *to)
        {
            to->abs_expires += to->interval;
            if (to->abs_expires <= this->curtime)
            {
                /* If we've missed the next firing of this timeout, reschedule
                 * it to occur at the next multiple of its interval after
                 * the last time that it fired.
                 * * * * * * * * * * * * * * * * * * * * */
                time_t n = this->curtime - to->abs_expires;
                time_t r = n % to->interval;
                to->abs_expires = this->curtime + (to->interval - r);
            }
            sche_timer(to, to->abs_expires);
        } /* timeouts_readd() */
        void wtimers_t::sche_timer(wtimer_t *to, time_t abs_expires)
        {
            time_t rem; /*remianing time before timeout*/
            int wheel, slot; /*row-col-style index to find bucket to add this timer to*/

            /*1) delete this timer if already existed */
            stop_timer(to);

            to->abs_expires = abs_expires;
            to->wtimers = this;
            if (abs_expires > this->curtime)
            {
                /*2) determine the wheel and slot where this timer should go*/
                rem = get_rem(to);
                wheel = this->get_wheel_idx(rem);
                slot = this->get_slot_idx(wheel, abs_expires);
                to->pending = &(this->wheel[wheel][slot]);
                to->pending->push_back(to);
                to->id = to->pending->end();
                --to->id;
                /*set againest bit to 1 means curr slot is used on curr wheel*/
                this->pending[wheel] |= WHEEL_C(1) << slot;
            }
            else
            {
                /*3) triggered imediately*/
                to->pending = &this->expired;
                this->expired.push_back(to);
                to->id = this->expired.end();
                --to->id;
            }
        }
        inline void wtimers_t::stop_timer(wtimer_t* wtimer_ptr) //timeouts_del
        {
            /* 1) validate if this timer */
            if (wtimer_ptr->intid != 0 && wtimer_ptr->pending != NULL)
            {
                /*2)delete it from list*/
                wtimer_ptr->pending->erase(wtimer_ptr->id);
                /*3) this timer is stored in wheel list that is empty now,
                 * we need do some extra clearup work*/
                if (wtimer_ptr->pending != &(this->expired)
                        && wtimer_ptr->pending->empty())
                {
                    // get offset of the wheel where this timer locates
                    ptrdiff_t index = wtimer_ptr->pending - &this->wheel[0][0];
                    // transform the index into row-column-style index
                    int wheel_index = index / WHEEL_LEN; // row
                    int slot_index = index / WHEEL_LEN; // col
                    // set highest bit to 0, the rest of bits all 1
                    this->pending[wheel_index] &= ~(WHEEL_C(1) << slot_index);
                }
                /*4) enpty timer list ptr and timers collection ptr */
                wtimer_ptr->pending = NULL;
                wtimer_ptr->wtimers = NULL;
            }
        }

    } /* namespace ds */
} /* namespace geco */
