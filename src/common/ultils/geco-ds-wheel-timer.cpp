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
#include "../../common/debugging/debug.h"
DECLARE_DEBUG_COMPONENT2("ULTILS_MODULE/TIMER", 0);
namespace geco
{

    namespace ultils
    {
         wheel_t rotl(const wheel_t v, int c)
        {
            // first time enter
            // v = 11111 --- 31, c = 0
            // vchar_minus_1 = 111111 --- 63
            // c_yu_vchar_minus_1 = c &vchar_minus_1 = 0&63 = 0
            // c = c_yu_vchar_minus_1 = 0
            // return v = 11111 --- 31

            // second time enter
            // v = 11111 --- 31, c = 101 --- 5
            // vchar_minus_1 = 111111 --- 63
            // c_yu_vchar_minus_1 = c &vchar_minus_1 = 5&63 = 101 --- 5
            // c = c_yu_vchar_minus_1 = 101 --- 5
            // wheel_t v_ls_c = (v << c) = 00011111 << 5 = 00000111 --- 7
            wheel_t vchar_minus_1 = (sizeof v * CHAR_BIT - 1);
            wheel_t c_yu_vchar_minus_1 = c & vchar_minus_1;
            c = c_yu_vchar_minus_1;
            if (!c)
                return v;

            wheel_t v_ls_c = (v << c);
            wheel_t vchar_minus_c = (sizeof v * CHAR_BIT - c);
            wheel_t v_rs_vchar_minus_c = v >> vchar_minus_c;
            wheel_t ret = v_ls_c | v_rs_vchar_minus_c;
            return ret;
        } /* rotl() */

         wheel_t rotr(const wheel_t v, int c)
        {
            //if (!(c &= (sizeof v * CHAR_BIT - 1)))
            //    return v;
            //return (v >> c) | (v << (sizeof v * CHAR_BIT - c));

            wheel_t vchar_minus_1 = (sizeof v * CHAR_BIT - 1);
            wheel_t c_yu_vchar_minus_1 = c & vchar_minus_1;
            c = c_yu_vchar_minus_1;
            if (!c)
                return v;

            wheel_t v_rs_c = (v >> c);
            wheel_t vchar_minus_c = (sizeof v * CHAR_BIT - c);
            wheel_t v_ls_vchar_minus_c = v << vchar_minus_c;
            wheel_t ret = v_rs_c | v_ls_vchar_minus_c;
            return ret;
        } /* rotr() */

        void wtimer_t::init(int wt_flags)
        {
            memset(this, 0, sizeof(wtimer_t));
            flags = wt_flags;
        }
#ifndef TIMEOUT_DISABLE_RELATIVE_ACCESS
        /* true if on timing wheel, false otherwise */
        bool wtimer_t::pending_wheel()
        {
            return this->pending != NULL
                    && this->pending != &this->wtimers->expired;
        }
        /* true if on expired queue, false otherwise */
        bool wtimer_t::pending_expired()
        {
            return this->pending != NULL && this->pending == &wtimers->expired;
        }
        /* remove timeout from any timing wheel (okay if not member of any) */
        void wtimer_t::stop()
        {
            wtimers->stop_timer(this);
        }
#endif

        timeout_t wtimers_t::get_interval()
        {
            /*~TIMEOUT_C(0) is the max value of uint64_t */
            timeout_t timeout = ~TIMEOUT_C(0);
            timeout_t _timeout;
            timeout_t relmask = 0;
            int wheel, slot;

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
                    int scale = this->curtime >> (wheel * WHEEL_BIT);
                    slot = WHEEL_MASK & scale;

                    /*
                     * determine the relative timout of the latest timer on current wheel
                     * ctz input cannot be zero: this->pending[wheel] is
                     * nonzero, so rotr() is nonzero.
                     * 双感叹号!!是为了把"非0值"转换成1，而0值还是0
                     * this->pending[wheel] = 10010000, the current time locates at 3th bit
                     * from right to left, we present it as 00010010,
                     * now rotr () will shift to right and give us the span bits between cur time
                     * and the lasteset timer, which is bits set 100
                     * ctz(100) = 2 = the number of scales between curr time and latest timeout
                     * then we multiple this with jiffies on this wheel, 2 << (wheel * WHEEL_BIT)
                     * will get the timeout interval.
                     * plus !!wheel is to add one scale timeoutof the lower wheel
                     * eg. assume on wheel 0, timout = 2 << 0 = 2. which is corect
                     * assume on wheel 1, timeout = 2 << 6(wheel 1 timeout)
                     * + 1 << 6(wheel0 timeout)
                     **/
                    wheel_t val = rotr(this->pending[wheel], slot);
                    int tzeros = ctz(val);
                    _timeout = (tzeros + !!wheel) << (wheel * WHEEL_BIT);

                    /* relmask & this->curtime is another bit operation that can calculate
                     * relative time, we reduce this value from relative timeout,
                     *  we can get the relative time before the next timeout*/
                    _timeout -= relmask & this->curtime;

                    /* swap if it is the current rel timeout is smaller than previous one
                     * this is to find the smallest rel timeout for all timers on all wheels*/

                    timeout = MIN(_timeout, timeout);
                }

                /* different wheels have different rel mask to be used*/
                relmask <<= WHEEL_BIT;
                relmask |= WHEEL_MASK;
            }
            //printf("before timeout %lu, _timeout %lu\n", timeout, _timeout);
            return timeout;
        }

        void wtimers_t::update(timeout_t abstime)
        {
            /*                       w0        w1        w2        w3
             * currtime : |------------|------------|-----------|-----------|
             *  jiffies      1         2^6       2^12    2^18    2^24
             *  eg. if we have abstime = 2^6 + 100 jiffies, curr time = 2^6
             *  so all timrs on wheel 0 are expired,
             *  timers on wheel 1 in range of [2^6, 2^6+100] are also expired.
             */
            timeout_t elapsed = abstime - this->curtime;
            wheel_t pending;
            int wheel;
            wheel_t _elapsed;
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
                    _elapsed = WHEEL_MASK & (elapsed >> (wheel * WHEEL_BIT));

                    /*
                     * TODO: check if curr time is included in time max range
                     * or it is always the start point of tmeout range,
                     * in addition, check if _elapsed == currtime_newslot,
                     *
                     * It's likely that at least one of the
                     * following three bit fill operations is redundant
                     * or can be replaced with a simpler operation.*/

                    /*2.2 get curr slot index in the wheel of the curr time*/
                    int oslot = WHEEL_MASK
                            & (this->curtime >> wheel * WHEEL_BIT);
                    /* 2.3 set all timer as expired bwtween curr time and
                     * _elapsed time
                     * (WHEEL_C(1) << _elapsed) = 00000001 << 5 = 00100000 = 32
                     * 32-1 = 31 = 000 11111 = 31
                     * rotl(31,0) = 31 = 11111
                     */
                    pending = rotl(((WHEEL_C(1) << _elapsed)) - 1, oslot);
                    /*2.4 get new slot index in the wheel of the abstime*/
                    int nslot = WHEEL_MASK & (abstime >> wheel * WHEEL_BIT);
                    /* 2.5 set all timer as expired bwtween old slot index and new slot index*/
                    wheel_t val = rotl(((WHEEL_C(1) << _elapsed)) - 1, nslot);
                    pending |= rotr(val, _elapsed);
                    pending |= WHEEL_C(1) << nslot;
                }

                /*3.1) AND to get the expired timer number*/
                timeout_t slot = pending & this->pending[wheel];
                while (slot) // > 0 means there is expired timer
                {
                    /* 3.2) ctz input cannot be zero: loop condition.
                     * get the non-expired timer number in this wheel */
                    slot = ctz(slot);
                    /*3.3 push it to the expired list */
                    for (auto& ele : this->wheel[wheel][slot])
                    {
                        printf(
                                "update(curtime %lu)::a pending timer(abs_expires %lu) got expired\n",
                                abstime, ele->abs_expires);
                        ele->pending = &this->expired;
                    }
                    this->expired.merge(this->wheel[wheel][slot]);
                    assert(this->wheel[wheel][slot].size() == 0);
                    /*3,4 update old pending by seting all bits 0
                     * where expired slot indexs represent*/
                    this->pending[wheel] &= ~(WHEEL_C(1) << slot);
                    /* get next expired slot*/
                    slot = pending & this->pending[wheel];
                }

                if (!(0x1 & pending))
                {
                    /*4) break if we didn't wrap around end of wheel*/
                    break;
                }

                /* if we're continuing, the next wheel must tick at least once */
                elapsed = MAX(elapsed, (WHEEL_LEN << (wheel * WHEEL_BIT)));
            }

            /*update curr abs time*/
            this->curtime = abstime;
        }

        void wtimers_t::open(timeout_t hz)
        {
            for (unsigned int i = 0; i < WHEEL_NUM; i++)
            {
                this->pending[i] = 0;
            }
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
                wt->intid = 0;
                this->expired.pop_front();

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
        void wtimers_t::readd_timer(wtimer_t *to)
        {
            to->abs_expires += to->interval;
            if (to->abs_expires <= this->curtime)
            {
                /* If we've missed the next firing of this timeout, reschedule
                 * it to occur at the next multiple of its interval after
                 * the last time that it fired.
                 * * * * * * * * * * * * * * * * * * * * */
                timeout_t n = this->curtime - to->abs_expires;
                timeout_t r = n % to->interval;
                to->abs_expires = this->curtime + (to->interval - r);
            }
            sche_timer(to, to->abs_expires);
        } /* timeouts_readd() */

        void wtimers_t::add_timer(wtimer_t *to, timeout_t timeout)
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

        void wtimers_t::sche_timer(wtimer_t *to, timeout_t abs_expires)
        {
            timeout_t rem; /*remaining time before timeout*/
            int wheel, slot; /*row-col-style index to find timeout list to add this timer to*/

            /*1) delete this timer if already existed */
            stop_timer(to);

            to->abs_expires = abs_expires;
            to->wtimers = this;
            if (abs_expires > this->curtime)
            {
                /*2) this is a timer being expired, so we need
                 * determine the wheel and slot where this timer should go*/
                rem = get_rem(to);
                printf(
                        "add_timer(pending)::abstimout=%lu,currtime %lu,rem %d),\n",
                        abs_expires, this->curtime, (int) rem);

                wheel = this->get_wheel_idx(rem);
                slot = this->get_slot_idx(wheel, abs_expires);
                to->pending = &(this->wheel[wheel][slot]);
                to->pending->push_back(to);
                to->id = to->pending->end();
                --to->id;
                to->intid = 1;
                /* 3) set curr slot on curr wheel to bit one to indicate there is
                 * timer exists in this slot  */
                this->pending[wheel] |= (WHEEL_C(1) << slot);
                //		char buf[1024];
                //		geco::ultils::Bitify(buf, sizeof(wheel_t) * 8,
                //				(unsigned char*) &this->pending[wheel]);
                //		printf("wheel %d, slot %d, pending \n%s\n", wheel, slot, buf);
            }
            else
            {
                printf("add_timer(expired)::abstimout=%lu,currtime %lu,\n",
                        abs_expires, this->curtime);
                /*3) this timer is triggered imediately, push it to expired list*/
                to->pending = &this->expired;
                to->pending->push_back(to);
                to->id = to->pending->end();
                --to->id;
                to->intid = 1;
            }
        }
        inline void wtimers_t::stop_timer(wtimer_t* wtimer_ptr) //timeouts_del
        {
            /* 1) valid timer ? */
            if (wtimer_ptr->intid != 0 && wtimer_ptr->pending != NULL)
            {
                /*2) delete it from againest list, either pending or expired list*/
                wtimer_ptr->pending->erase(wtimer_ptr->id);

                /*3) this timer is stored in wheel list that is empty now,
                 * we need do some extra clearup work*/
                if (wtimer_ptr->pending != &(this->expired)
                        && wtimer_ptr->pending->empty())
                {
                    /*3.1) get offset of the wheel where this timer locates*/
                    ptrdiff_t index = wtimer_ptr->pending - &this->wheel[0][0];
                    /*3.2) transform linear index into row-column-style index*/
                    int wheel_index = index >> WHEEL_BIT; // row
                    int slot_index = index & WHEEL_MAX; // col
                    /*3.3) this is funny... the timelist in curr slot on curr wheel
                     * may have more than one timer, we will not clear it until
                     * all timers in this slot are deleted, the againest bit will be set
                     * to bit 0 to indicate no timers existed in curr slot
                     * with other bits not changed.*/
                    this->pending[wheel_index] &= ~(WHEEL_C(1) << slot_index);
                }
                /*4) reset pending list and wtimers to NULL*/
                wtimer_ptr->pending = NULL;
                wtimer_ptr->wtimers = NULL;
                wtimer_ptr->intid = 0;
            }
        }
    } /* namespace ds */
} /* namespace geco */
