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
        static inline wheel_t rotl(const wheel_t v, int c)
        {
            /*
             * _timeout =
             * (ctz(rotr(this->pending[wheel], slot)) + !!wheel)
             * << (wheel * WHEEL_BIT);*/
            // 1) c < 63
            // (sizeof v * CHAR_BIT - 1) = 63 =   00 111111,
            // c                                     =  8 =   00 001000
            // 63&8 = 00 001000 = c = 8 > 0,
            //  (v << c) = 111 00000 << 8 = 0 | 56
            // 
            // 2) c == 63
            // (sizeof v * CHAR_BIT - 1) = 63 =   00 111111,
            // c                                     = 63 =   00 111111,
            // 63&63 = 00 000000 = c = 63 > 0, return v 
            //
            // 2) c >= 63
            // (sizeof v * CHAR_BIT - 1) = 63 =   00 111111,
            // c                                     = 128 = 01 000000,
            // 63&128 = 00 000000 = c = 0, return v 
            //
            if (!(c &= (sizeof v * CHAR_BIT - 1)))
                return v;
            return (v << c) | (v >> (sizeof v * CHAR_BIT - c));
        } /* rotl() */

        static inline wheel_t rotr(const wheel_t v, int c)
        {
            if (!(c &= (sizeof v * CHAR_BIT - 1)))
                return v;
            return (v >> c) | (v << (sizeof v * CHAR_BIT - c));
        } /* rotr() */

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

        time_t wtimers_t::get_interval()
        {
            /*~TIMEOUT_C(0) is the max value of uint64_t */
            time_t timeout = ~TIMEOUT_C(0), relmask = 0, _timeout;
            int wheel, slot, scale;
            for (wheel = 0; wheel < WHEEL_NUM; wheel++)
            {
                /*1) there are existing timers pended in this wheel*/
                if (this->pending[wheel])
                {
                    /*
                     * WHEEL SLOTS            JIFFIES            SCALE(number of jiffies of each scale)
                     *     0      [0,63]         [1,        2^6-1 ]      1
                     *     1      [0,63]         [2^6,   2^12-1]      2^6
                     *     2      [0,63]         [2^12, 2^18-1]      2^12
                     *     3      [0,63]         [2^18, 2^24-1]      2^18
                     * */
                    /*1) determine initial slot index current abs time locates on a certain wheel
                     * (this->curtime >> (wheel*WHEEL_BIT) is to calculate the scales
                     * eg. if given an abs timout 645ms, and if on wheel 0,
                     * we can know its scales is 645ms/1 = 645,
                     * now we can use scales to determine slot index on wheel 0,
                     * it should be 645 % 64 = 5 = slot index,
                     *  this calculation has a fast bit operation, unless the range is 2 times big
                     * that is 645 & (2^6-1) = 645 & WHEEL_MASK = 5 */
                    scale = this->curtime >> (wheel * WHEEL_BIT);
                    slot = WHEEL_MASK & scale;

                    /* abs timeout of the bigger one between current time and existing timer
                     * ctz input cannot be zero: this->pending[wheel] is
                     * nonzero, so rotr() is nonzero.
                     * +1 to higher order wheels as those timeouts are one rotation
                     * in the future (otherwise they'd be on a lower wheel or expired)
                     * 双感叹号!!是为了把"非0值"转换成1，而0值还是0 */
                    _timeout = (ctz(rotr(this->pending[wheel], slot)) + !!wheel)
                            << (wheel * WHEEL_BIT);

                    /* relmask & this->curtime is another bit operation that can calculate
                     * abs time that all lower wheels have represent.
                     * we reduce this value from total abs time,
                     *  we can get the time delta before the next timeout */
                    _timeout -= relmask & this->curtime;

                    /* update to find the smallest time delta*/
                    timeout = MIN(_timeout, timeout);
                }
                /* different wheels have different mask to be used*/
                relmask <<= WHEEL_BIT;
                relmask |= WHEEL_MASK;
            }
            return timeout;
        }

        void wtimers_t::update(time_t abstime)
        {
            time_t elapsed = abstime - this->curtime;
            wheel_t pending;
            int wheel;
            for (wheel = 0; wheel < WHEEL_NUM; wheel++)
            {
                if ((elapsed >> (wheel * WHEEL_BIT)) > WHEEL_MAX)
                {
                    /* 1) If the elapsed time is greater than the maximum period of
                     * the wheel, mark every position as expiring (bit 1 means expired) */
                    pending = (wheel_t) (~WHEEL_C(0));
                }
                else
                {
                    /* 2) Otherwise, to determine the expired slots fill in all the
                     * bits between the last slot processed and the current
                     * slot, inclusive of the last slot. We'll bitwise-AND this
                     * with our pending set below.*/

                    /*2.1) get the elpased time slot index*/
                    wheel_t _elapsed = WHEEL_MASK
                            & (elapsed >> (wheel * WHEEL_BIT));

                    /*
                     * TODO: It's likely that at least one of the
                     * following three bit fill operations is redundant
                     * or can be replaced with a simpler operation.*/
                    /*2.2 get curr slot index in the wheel of the curr time*/
                    int currtime_oldslot = WHEEL_MASK
                            & (this->curtime >> wheel * WHEEL_BIT);
                    /* 2.3 set all timer as expired bwtween old slot index and
                     * _elapsed slot index*/
                    pending = rotl(((TIMEOUT_C(1) << _elapsed)) - 1,
                            currtime_oldslot);

                    /*2.4 get new slot index in the wheel of the abstime*/
                    int currtime_newslot = WHEEL_MASK
                            & (abstime >> wheel * WHEEL_BIT);
                    /* 2.5 set all timer as expired bwtween old slot index and new slot index*/
                    pending |= rotl(((TIMEOUT_C(1) << _elapsed)) - 1,
                            currtime_newslot);
                }

                /*3.1) AND to get the expired timer number*/
                time_t slot = pending & this->pending[wheel];
                while (slot) // > 0 means there is expired timer
                {
                    /* 3.2) ctz input cannot be zero: loop condition.
                     * get the non-expired timer number in this wheel */
                    slot = ctz(slot);
                    /*3.3 push it to the expired list */
                    for (auto& ele : this->wheel[wheel][slot])
                    {
                        ele->pending = &this->expired;
                    }
                    this->expired.merge(this->wheel[wheel][slot]);

                    //this->expired.merge(this->wheel[wheel][slot]);
                    /*3,4 update old pending by seting all bits 0
                     * where expired slot indexs represent*/
                    this->pending[wheel] &= ~(TIMEOUT_C(1) << slot);
                    /*update slot*/
                    slot = pending & this->pending[wheel];
                }

                if (!(0x1 & pending))
                {
                    /*4) break if we didn't wrap around end of wheel
                     * this means we should stop at this wheel
                     * this is the farest slot position where the abstime locates
                     * because if we wrap around, we must cover all 64 slots on that wheel,
                     * so pending must have its highest bit of one, so 0x1 & pending must
                     * >= 1, so we will continue to next higher wheel, but when we are on
                     * the farest wheel we can go, we must NOT cover all 64 slots, so pending
                     * must have its highest bit of zero, so 0x1 & pending must = 0, so we
                     * will stop it here*/
                    break;
                }

                /* if we're continuing, the next wheel must tick at least once */
                elapsed = MAX(elapsed, (WHEEL_LEN << (wheel * WHEEL_BIT)));
            }

            /*update curr abs time*/
            this->curtime = abstime;
//            while (!this->todo.empty())
//            {
//                wtimer_t* to = this->todo.front();
//                this->todo.pop_front();
//                to->pending = NULL;
//                /*add the expired timers to expired list for timeout imediatey
//                 * at this moment,  to->abs_expires MUST <   this->curtime */
//                sche_timer(to, to->abs_expires);
//            }
        }

        void wtimers_t::open(time_t hz)
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

        wtimer_t * wtimers_t::get_expired_timer()
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
