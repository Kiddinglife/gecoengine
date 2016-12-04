/*
 * test-timer.cc
 *
 *  Created on: 22Jul.,2016
 *      Author: jackiez
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <cstdint>
#include <limits.h>
#include <functional>
#include <math.h>
#include <stdlib.h> 

#include "gtest/gtest.h"

#include "protocol/geco-net-common.h"
#include "common/debugging/debug.h"
#include "common/geco-plateform.h"
#include "common/ultils/geco-ds-wheel-timer.h"
#include "common/debugging/timestamp.h"
#include "common/debugging/timer_queue_t.h"
#include "network/networkstats.h"

using namespace geco::debugging;
using namespace geco::ultils;
using namespace geco::ds;

DECLARE_DEBUG_COMPONENT2("UNIT-TEST", 0);

TEST(TIME, test_gettimestamp_func)
{
	uint times = 10000000;
	double secs = 0;
	bool ret;

	double ageinsec;
	uint64 ageinstamp;

	time_stamp_t stampt;
	time_stamp_t stamptcpy;

	time_t start;
	time_t end;
	for (uint i = 0; i < times; i++)
	{
		secs = (double)1 / (i + 1);
		stampt = time_stamp_t::fromSecs(secs);
		ASSERT_EQ(almost_equal(time_stamp_t::toSecs(stampt.stamp_), secs), true);

		stampt = gettimestamp();
		ageinsec = stampt.agesInSec();
		ageinstamp = stampt.ageInStamps();
		ASSERT_GT(ageinsec, 0);
		ASSERT_GT(ageinstamp, 0);
	}
	stampt = stamps_per_sec();
	if (!almost_equal(time_stamp_t::toSecs(stampt.stamp_), 1))
	{
		printf("%.15f secs, %" PRIu64 " stamps\n", ageinsec, ageinstamp);
	}

	gettimenow_us(&start);
	double accus = 0;
	double maxus = 0;
	uint size = 0;
	for (uint i = 0; i < times; i++)
	{
		stampt = gettimestamp();
		stamptcpy = gettimestamp();

		double us = (fabsl(stamptcpy - stampt) * 1000000) / stamps_per_sec_double();
		maxus = us > maxus ? us : maxus;
		accus += us;
		if (us > 100)
		{
			size++;
			//printf("substraction stampcpy - stamp =  {%f}\n", us);
		}
	}
	gettimenow_us(&end);
	printf("call gettimestamp %d times, greater than 100us  {%d times, %f percentages}, max us %f , average us %f, time stamp spent %d\n",
		times, size, size / (float)times * 100, maxus, accus / times, end - start);


	accus = 0;
	maxus = 0;
	size = 0;
	time_t t1;
	time_t t2;

	gettimenow_us(&start);
	for (uint i = 0; i < times; i++)
	{
		gettimenow_us(&t1);
		gettimenow_us(&t2);

		double us = fabsl(t2 - t1);
		maxus = us > maxus ? us : maxus;
		accus += us;
		if (us > 100)
		{
			size++;
			//printf("substraction stampcpy - stamp =  {%f}\n", us);
		}
	}
	gettimenow_us(&end);
	printf("call sys gettimestamp %d times, greater than 100us  {%d times, %f percentages}, max us %f , average us %f, time stamp spent %d\n",
		times, size, size / (float)times * 100, maxus, accus / times, end - start);
}

TEST(TIME, test_profile)
{
	static ProfileVal _localProfile("test_profile");
	uint count = 1000;
	for (int i = 0;i < count;i++)
	{
		_localProfile.start();
		geco_sleep(2);
		_localProfile.stop();
	}
	if (!almost_equal(_localProfile.lastIntTimeInSeconds(), 1, 1))
		printf("_localProfile.lastIntTimeInSeconds() %.5f\n", _localProfile.lastIntTimeInSeconds());
	std::cout << _localProfile;
}

static void timeoutcb(TimerID id, void * pUser)
{
	static int times = 0;
	times++;
	std::cout << "timeoutcb called\n";
	if (times == 3)
	{
		*(bool*)pUser = false;
		id.cancel();
	}
}
static void on_timer_released(TimerID id, void * pUser)
{
	std::cout << "on_timer_released called\n";
}
TEST(TIME, test_timer_queue)
{
	TimeQueue64 pTimeQueue;
	TimerHandler th;
	bool run = true;
	th.handleTimeout = timeoutcb;
	th.onRelease = on_timer_released;
	uint64 interval = 3 * stamps_per_sec(); //timeout 3 secs 
	uint64 tout = gettimestamp() + interval;
	TimerID tid = pTimeQueue.add(tout, interval, &th, &run);
	std::cout << "stamps_per_sec = " << stamps_per_sec() << " nextExp =" << pTimeQueue.nextExp(gettimestamp()) << "\n";
	uint64 start = gettimestamp();
	uint64 end;
	int secs = 0;
	while (run)
	{
		end = gettimestamp();
		if (end - start >= stamps_per_sec())
		{
			start = end;
			secs++;
			std::cout << secs << "th sec\n";
		}
		pTimeQueue.process(gettimestamp());
	}
}

//static int naive_clz(int bits, uint64_t v)
//{
//    int r = 0;
//    uint64_t bit = ((uint64_t)1) << (bits - 1);
//    while (bit && 0 == (v & bit))
//    {
//        r++;
//        bit >>= 1;
//    }
//    return r;
//}
//static int naive_ctz(int bits, uint64_t v)
//{
//    int r = 0;
//    uint64_t bit = 1;
//    while (bit && 0 == (v & bit))
//    {
//        r++;
//        bit <<= 1;
//        if (r == bits) break;
//    }
//    return r;
//}
//static int check(uint64_t vv)
//{
//    uint32_t v32 = (uint32_t)vv;
//    if (vv == 0)
//    {
//        return 1; /* c[tl]z64(0) is undefined. */
//    }
//
//    if (ctz64(vv) != naive_ctz(64, vv))
//    {
//        printf("mismatch with %d ctz64: %d, naive_ctz: %d\n", vv, ctz64(vv), naive_ctz(64, vv));
//        exit(1);
//        return 0;
//    }
//    if (clz64(vv) != naive_clz(64, vv))
//    {
//        printf("mismatch with %d clz64: %d, %d\n", vv, clz64(vv), naive_clz(64, vv));
//        exit(1);
//        return 0;
//    }
//
//    if (v32 == 0) return 1; /* c[lt]z(0) is undefined. */
//
//    if (ctz32(v32) != naive_ctz(32, v32))
//    {
//        printf("mismatch with ctz32: %d\n", ctz32(v32));
//        exit(1);
//        return 0;
//    }
//    if (clz32(v32) != naive_clz(32, v32))
//    {
//        printf("mismatch with clz32: %d\n", clz32(v32));
//        exit(1);
//        return 0;
//    }
//    return 1;
//}
//TEST(GECO_ENGINE_ULTILS, TEST_WHEEL_TIMER_BIT_OPS)
//{
//    uint64_t testcases[] =
//    { 13371337 * 10, 100, 385789752, 82574, (((uint64_t)1) << 63) + (((uint64_t)1) << 31)
//            + 10101 };
//
//    unsigned int i;
//    const unsigned int n = sizeof(testcases) / sizeof(testcases[0]);
//    int result = 0;
//
//    for (i = 0; i <= 63; ++i)
//    {
//        uint64_t x = 1 << i;
//        if (!check(x)) result = 1;
//        --x;
//        if (!check(x)) result = 1;
//    }
//    for (i = 0; i < n; ++i)
//    {
//        if (!check(testcases[i])) result = 1;
//    }
//    EXPECT_EQ(result, 0);
//}
//
//static int& test_bind(int& timer_type)
//{
//    int* a = new int;
//    *a = 1;
//    return *a;
//}
//static int* test_bind2(int* timer_type)
//{
//    int* a = new int;
//    *a = 1;
//    return a;
//}
//
//typedef int& (*fn1)(int&);  // cannot put fn1 to typedef function not typed !
//typedef std::function<int&(int&)> mycb;
//typedef std::function<int*(int*)> mycb2;
//using namespace std::placeholders;
// adds visibility of _1, _2, _3,...
//TEST(GECO_ENGINE_ULTILS, TEST_WHEEL_TIMER_CB)
//{
//    mycb f = std::bind(test_bind, _1);
//    int a = 1;
//    int& ret = f(a);
//    EXPECT_EQ(1, ret);
//    delete &ret;
//
//    mycb2 f2 = std::bind(test_bind2, _1);
//    int* ret2 = f2(&a);
//    EXPECT_EQ(1, *ret2);
//    delete ret2;
//}

//#define THE_END_OF_TIME ((timeout_t)-1)
//#define FAIL() do{printf("Failure on line %d\n", __LINE__); goto done;} while (0)
/* configuration for check_randomized */
struct rand_cfg
{
	/* When creating timeouts, smallest possible delay */
	timeout_t min_timeout;
	/* When creating timeouts, largest possible delay */
	timeout_t max_timeout;
	/* First time to start the clock at. */
	timeout_t start_at;
	/* Do not advance the clock past this time. */
	timeout_t end_at;
	/* Number of timeouts to create and monitor. */
	int n_timeouts;
	/* Advance the clock by no more than this each step. */
	timeout_t max_step;
	/* Use relative timers and stepping */
	int relative;
	/* Every time the clock ticks, try removing this many timeouts at
	 * random. */
	int try_removing;
	/* When we're done, advance the clock to the end of time. */
	int finalize;
};
/* Not very random */
//static timeout_t random_to(timeout_t min, timeout_t max)
//{
//    if (max <= min) return min;
//    /* Not actually all that random, but should exercise the code. */
//    timeout_t rand64 = rand() * (timeout_t)INT_MAX + rand();
//    return min + (rand64 % (max - min));
//}

/*
 * Check some basic algorithm invariants. If these invariants fail then
 * something is definitely broken.
 */
#define report(...) do { \
    if ((fp)) \
        fprintf(fp, __VA_ARGS__); \
} while (0)

#define check(expr, ...) do { \
    if (!(expr)) { \
        report(__VA_ARGS__); \
        return 0; \
            } \
} while (0)

 /*
  * Use dumb looping to locate the earliest timeout pending on the wheel so
  * our invariant assertions can check the result of our optimized code.
  * the earlist pending timeout should greater than curr time after
  * get_exired_timer() called.
  */
  //static wtimer_t* min_timeout(wtimers_t* T)
  //{
  //    struct wtimer_t *min = NULL;
  //    unsigned i, j;
  //
  //    for (i = 0; i < countof(T->wheel); i++)
  //    {
  //        for (j = 0; j < countof(T->wheel[i]); j++)
  //        {
  //            for (auto& to : T->wheel[i][j])
  //            {
  //                if (min == NULL) min = to;
  //                if (to->abs_expires < min->abs_expires) min = to;
  //            }
  //        }
  //    }
  //    return min;
  //} /* timeouts_min() */

  /*the earlist pending timeout should greater than curr time after
   * get_exired_timer() called.*/
   //bool timeouts_check(wtimers_t *T, FILE *fp)
   //{
   //    timeout_t timeout;
   //    wtimer_t* to = min_timeout(T);
   //    if (to != NULL)
   //    {
   //        check(to->abs_expires > T->curtime,
   //            "missed timeout (expires:%" TIMEOUT_PRIu " <= curtime:%" TIMEOUT_PRIu ")\n",
   //            to->abs_expires, T->curtime);
   //
   //        timeout = T->get_interval();
   //        check(timeout <= to->abs_expires - T->curtime,
   //            "wrong soft timeout (soft:%" TIMEOUT_PRIu " > hard:%" TIMEOUT_PRIu ") (expires:%" TIMEOUT_PRIu "; curtime:%" TIMEOUT_PRIu ")\n",
   //            timeout, (to->abs_expires - T->curtime), to->abs_expires, T->curtime);
   //
   //        timeout = T->timout();
   //        check(timeout <= to->abs_expires - T->curtime,
   //            "wrong soft timeout (soft:%" TIMEOUT_PRIu " > hard:%" TIMEOUT_PRIu ") (expires:%" TIMEOUT_PRIu "; curtime:%" TIMEOUT_PRIu ")\n",
   //            timeout, (to->abs_expires - T->curtime), to->abs_expires, T->curtime);
   //    }
   //    else
   //    {
   //        timeout = T->timout();
   //        if (!T->expired.empty())
   //            check(timeout == 0,
   //                "wrong soft timeout (soft:%" TIMEOUT_PRIu " != hard:%" TIMEOUT_PRIu ")\n", timeout,
   //                TIMEOUT_C(0));
   //        else
   //            check(timeout == ~TIMEOUT_C(0),
   //                "wrong soft timeout (soft:%" TIMEOUT_PRIu " != hard:%" TIMEOUT_PRIu ")\n", timeout,
   //                ~TIMEOUT_C(0));
   //    }
   //    return 1;
   //}

   //static int check_randomized(const struct rand_cfg *cfg)
   //{
   //    printf("+++++++++++++++=CheckRandomized+++++++++++++++++\n");
   //    uint64_t i, j, err;
   //    int rv = 1;
   //    wtimer_t* t = new wtimer_t[cfg->n_timeouts];
   //    timeout_t* timeouts = new timeout_t[cfg->n_timeouts];
   //    uint8_t *fired = new uint8_t[cfg->n_timeouts];
   //    uint8_t *found = new uint8_t[cfg->n_timeouts];
   //    uint8_t *deleted = new uint8_t[cfg->n_timeouts];
   //    memset(t, 0, sizeof(wtimer_t) * cfg->n_timeouts);
   //    memset(timeouts, 0, sizeof(timeout_t) * cfg->n_timeouts);
   //    memset(fired, 0, sizeof(uint8_t) * cfg->n_timeouts);
   //    memset(deleted, 0, sizeof(uint8_t) * cfg->n_timeouts);
   //    memset(found, 0, sizeof(uint8_t) * cfg->n_timeouts);
   //
   //    //check open
   //    wtimers_t tos;
   //    tos.open(TIMEOUT_mHZ);
   //    EXPECT_EQ(tos.get_hz(), TIMEOUT_mHZ);
   //    EXPECT_EQ(tos.curtime, 0);
   //    for (i = 0; i < WHEEL_NUM; i++)
   //    {
   //        EXPECT_EQ(tos.pending[i], 0);
   //    }
   //
   //    timeout_t now = cfg->start_at;
   //    int n_added_pending = 0, cnt_added_pending = 0;
   //    int n_added_expired = 0, cnt_added_expired = 0;
   //    wtimer_iterator_t it_p, it_e, it_all;
   //    int p_done = 0, e_done = 0, all_done = 0;
   //    wtimer_t *to = NULL;
   //    const int rel = cfg->relative;
   //
   //    EXPECT_EQ(!t || !timeouts || !fired || !found || !deleted, false);
   //
   //    printf("---------------init and add all sample timers ---------------\n");
   //    tos.update(cfg->start_at);
   //    EXPECT_EQ(!!n_added_pending, tos.has_expiring_timer());
   //    EXPECT_EQ(!!n_added_expired, tos.has_expired_timer());
   //
   //    for (i = 0; i < cfg->n_timeouts; ++i)
   //    {
   //        t[i].init(rel ? 0 : ABS_TIMEOUT);
   //        EXPECT_EQ(t[i].pending_wheel(), false);
   //        EXPECT_EQ(t[i].pending_expired(), false);
   //
   //        timeouts[i] = random_to(cfg->min_timeout, cfg->max_timeout);
   //        tos.add_timer(t + i, timeouts[i] - (rel ? now : 0));
   //        if (timeouts[i] <= cfg->start_at)
   //        {
   //            EXPECT_EQ(t[i].pending_wheel(), false);
   //            EXPECT_EQ(t[i].pending_expired(), true);
   //            ++n_added_expired;
   //        }
   //        else
   //        {
   //            EXPECT_EQ(t[i].pending_wheel(), true);
   //            EXPECT_EQ(t[i].pending_expired(), false);
   //            ++n_added_pending;
   //        }
   //    }
   //
   //    EXPECT_EQ(!!n_added_pending, tos.has_expiring_timer());
   //    EXPECT_EQ(!!n_added_expired, tos.has_expired_timer());
   //
   //    for (auto& to : tos.expired)
   //    {
   //        if (to)
   //        {
   //            i = to - t;
   //            ++found[i];
   //            ++cnt_added_expired;
   //        }
   //    }
   //
   //    for (i = 0; i < WHEEL_NUM; i++)
   //    {
   //        for (j = 0; j < WHEEL_LEN; j++)
   //        {
   //            int idx;
   //            for (auto& to : tos.wheel[i][j])
   //            {
   //                if (to != NULL)
   //                {
   //                    idx = to - t;
   //                    ++found[idx];
   //                    ++cnt_added_pending;
   //                }
   //            }
   //        }
   //    }
   //
   //    EXPECT_EQ(cnt_added_expired, n_added_expired);
   //    EXPECT_EQ(n_added_pending, cnt_added_pending);
   //
   //    printf("\n---------------filter out all expired timers ---------------\n");
   //    while (NULL != (to = tos.get_expired_timer()))
   //    {
   //        i = to - &t[0];
   //        EXPECT_EQ(&t[i], to);
   //        EXPECT_GE(now, timeouts[i]);/* shouldn't have happened yet */
   //        --n_added_expired; /* drop expired timeouts. */
   //        ++fired[i];
   //    }
   //    EXPECT_EQ(n_added_expired, 0);
   //
   //    printf("\n---------------now we test pending timers---------------\n");
   //    while (now < cfg->end_at)
   //    {
   //        int n_fired_this_time = 0;
   //        timeout_t td = tos.timout();
   //        timeout_t first_at = td + now;
   //        timeout_t oldtime = now;
   //        timeout_t step = random_to(1, cfg->max_step);
   //        now += step;
   //        int another;
   //
   //        if (rel) tos.step(step);
   //        else tos.update(now);
   //
   //        for (i = 0; i < cfg->try_removing; ++i)
   //        {
   //            int idx = rand() % cfg->n_timeouts;
   //            if (!fired[idx])
   //            {
   //                tos.stop_timer(&t[idx]);
   //                ++deleted[idx];
   //            }
   //        }
   //
   //        timeout_t tm = tos.timout();
   //        another = (tm == 0);
   //        while (NULL != (to = tos.get_expired_timer()))
   //        {
   //            EXPECT_EQ(another, 1); /* Thought we saw the last one! */
   //            i = to - t;
   //            EXPECT_EQ(t + i, to);
   //            EXPECT_LE(timeouts[i], now);
   //            EXPECT_GT(timeouts[i], oldtime);
   //            EXPECT_GE(timeouts[i], first_at);
   //            fired[i]++;
   //            timeout_t tm = tos.timout();
   //            another = (tm == 0);
   //        }
   //        EXPECT_EQ(n_fired_this_time && first_at > now, false);
   //        EXPECT_EQ(another, 0);
   //        EXPECT_TRUE(timeouts_check(&tos, stderr));
   //    }
   //
   //    for (i = 0; i < cfg->n_timeouts; ++i)
   //    {
   //        EXPECT_LE(fired[i], 1);/* Nothing fired twice. */
   //        if (timeouts[i] <= now)
   //        {
   //            EXPECT_TRUE(fired[i] || deleted[i]);  //CHECK ALREADY TIMEOUT
   //        }
   //        else
   //        {
   //            EXPECT_EQ(fired[i], 0);  // CHECK NOT TIMEOUT
   //        }
   //
   //        // FIRED !DELETED
   //        // !FIRED !DELETED
   //        // !FIRED DELETED
   //        EXPECT_FALSE(fired[i] && deleted[i]);
   //
   //        if (cfg->finalize > 1)
   //        {
   //            if (!fired[i]) tos.stop_timer(&t[i]);
   //        }
   //    }
   //
   //    /* Now nothing more should fire between now and the end of time. */
   //    if (cfg->finalize)
   //    {
   //        tos.update(THE_END_OF_TIME);
   //        if (cfg->finalize > 1)
   //        {
   //            EXPECT_EQ(tos.get_expired_timer(), (wtimer_t*)0);
   //            EXPECT_EQ(false, tos.has_expiring_timer());
   //            EXPECT_EQ(false, tos.has_expired_timer());
   //        }
   //    }
   //    rv = 0;
   //
   //    tos.close();
   //    EXPECT_EQ(false, tos.has_expiring_timer());
   //    EXPECT_EQ(false, tos.has_expired_timer());
   //
   //    printf("\n---------------clearup---------------\n");
   //done: if (t) delete[] t;
   //    if (timeouts) delete[] timeouts;
   //    if (fired) delete[] fired;
   //    if (found) delete[] found;
   //    if (deleted) delete[] deleted;
   //
   //    return rv;
   //}
   //
   //#define DO(fn) do {                             \
   //        printf("."); fflush(stdout);    \
   //        fn;                 \
   //            } while (0)
   //
   //#define DO_N(n, fn) do {            \
   //        for (j = 0; j < (n); ++j) { \
   //            DO(fn);         \
   //                }               \
   //        } while (0)
   //
   //#include <stdlib.h>
   //#include <stdio.h>
   //static void ut_rotl()
   //{
   //    unsigned int testval = (UINT32_C(1) << 1) - 1;
   //    //printf("%#10X\n", testval);
   //    testval = _rotl(testval, 0);
   //    //printf("%#10X\n", testval);
   //}
   //
   //TEST(GECO_ENGINE_ULTILS, TEST_WHEEL_TIMER)
   //{
   //    int j;
   //    int n_failed = 0;
   //
   //    struct rand_cfg cfg1 =
   //    {
   //        /*min_timeout*/1,
   //        /*max_timeout*/381,
   //        /*start_at*/100,
   //        /*end_at*/1000,
   //        /*n_timeouts*/1,
   //        /*max_step*/10,
   //        /*relative*/0,
   //        /*try_removing*/0,
   //        /*finalize*/2 };
   //    //DO_N(1, check_randomized(&cfg1));
   //}
   //
   //TEST(GECO_ENGINE_ULTILS, TEST_ROTL)
   //{
   //    // assume that currtime has 1 scale, eclapse has 4 scales on wheel 1.
   //    //
   //    wheel_t timeout_slotidx = (UINT64_C(1) << 4) - 1;
   //    int currtime_scales = 1;
   //    wheel_t pending = geco::ultils::rotl(timeout_slotidx, currtime_scales);
   //    char buf[1024];
   //    geco::ultils::Bitify(buf, sizeof(wheel_t) * 8, (unsigned char*)&pending);
   //    printf("%s\n", buf);
   //}

