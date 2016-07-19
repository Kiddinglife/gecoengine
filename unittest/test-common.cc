#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <functional>
#include <binders.h>
#include "gtest/gtest.h"
#include "common/plateform.h"
#include "common/ultils/ultils.h"
#include "common/ultils/geco-ds-wheel-timer.h"
#include "common/debugging/timestamp.h"
#include "common/ds/array.h"
#include "common/debugging/debug.h"

using namespace geco::debugging;
using namespace geco::ultils;
using namespace geco::ds;

DECLARE_DEBUG_COMPONENT2("ENGINE-UNITTEST-LOGGER", 0);

/**
 *  unit tests have:
 *  1) TEST(GECO_ENGINE_ULTILS, TEST_WHEEL_TIMER_BIT_OPS)
 *
 */

static int naive_clz(int bits, uint64_t v)
{
    int r = 0;
    uint64_t bit = ((uint64_t) 1) << (bits - 1);
    while (bit && 0 == (v & bit))
    {
        r++;
        bit >>= 1;
    }
    return r;
}
static int naive_ctz(int bits, uint64_t v)
{
    int r = 0;
    uint64_t bit = 1;
    while (bit && 0 == (v & bit))
    {
        r++;
        bit <<= 1;
        if (r == bits) break;
    }
    return r;
}
static int check(uint64_t vv)
{
    uint32_t v32 = (uint32_t) vv;
    if (vv == 0)
    {
        return 1; /* c[tl]z64(0) is undefined. */
    }

    if (ctz64(vv) != naive_ctz(64, vv))
    {
        printf("mismatch with %d ctz64: %d, naive_ctz: %d\n", vv, ctz64(vv), naive_ctz(64, vv));
        exit(1);
        return 0;
    }
    if (clz64(vv) != naive_clz(64, vv))
    {
        printf("mismatch with %d clz64: %d, %d\n", vv, clz64(vv), naive_clz(64, vv));
        exit(1);
        return 0;
    }

    if (v32 == 0) return 1; /* c[lt]z(0) is undefined. */

    if (ctz32(v32) != naive_ctz(32, v32))
    {
        printf("mismatch with ctz32: %d\n", ctz32(v32));
        exit(1);
        return 0;
    }
    if (clz32(v32) != naive_clz(32, v32))
    {
        printf("mismatch with clz32: %d\n", clz32(v32));
        exit(1);
        return 0;
    }
    return 1;
}
TEST(GECO_ENGINE_ULTILS, TEST_WHEEL_TIMER_BIT_OPS)
{
    uint64_t testcases[] =
    { 13371337 * 10, 100, 385789752, 82574, (((uint64_t) 1) << 63) + (((uint64_t) 1) << 31) + 10101 };

    unsigned int i;
    const unsigned int n = sizeof(testcases) / sizeof(testcases[0]);
    int result = 0;

    for (i = 0; i <= 63; ++i)
    {
        uint64_t x = 1 << i;
        if (!check(x)) result = 1;
        --x;
        if (!check(x)) result = 1;
    }
    for (i = 0; i < n; ++i)
    {
        if (!check(testcases[i])) result = 1;
    }
    EXPECT_EQ(result, 0);
}

static int& test_bind(int& timer_type)
{
    int* a = new int;
    *a = 1;
    return *a;
}
static int* test_bind2(int* timer_type)
{
    int* a = new int;
    *a = 1;
    return a;
}

typedef int& (*fn1)(int&);  // cannot put fn1 to typedef function not typed !
typedef std::function<int&(int&)> mycb;
typedef std::function<int*(int*)> mycb2;
using namespace std::placeholders;
// adds visibility of _1, _2, _3,...
TEST(GECO_ENGINE_ULTILS, TEST_WHEEL_TIMER_CB)
{

    mycb f = std::bind(test_bind, _1);
    int a = 1;
    int& ret = f(a);
    EXPECT_EQ(1, ret);
    delete &ret;

    mycb2 f2 = std::bind(test_bind2, _1);
    int* ret2 = f2(&a);
    EXPECT_EQ(1, *ret2);
    delete ret2;

}
TEST(GECO_DEBUGGING_TIMESTAMP, test_gettimestamp_func)
{
    time_stamp_t stampt = time_stamp_t::fromSeconds(1.0);
    EXPECT_EQ(time_stamp_t::toSeconds(stampt.stamp_), 1.0f);
    stampt = gettimestamp();
    geco_sleep(1);
    time_stamp_t ageinstamp = stampt.ageInStamps();
    double ageinsec = stampt.ageInSeconds();
    printf("%0.2f -> %lu\n", ageinsec, ageinstamp.stamp_);
}
TEST(GECO_DEBUGGING_MSGLOG, test_msg_macros)
{
    Leave("test_msg_macros", "test_msg_macros");
    Enter("test_msg_macros", "test_msg_macros");
    TRACE_MSG("TRACE_MSG\n");
    DEBUG_MSG("DEBUG_MSG %d\n", 12);
    INFO_MSG("INFO_MSG %s\n", "hello world");
    NOTICE_MSG("NOTICE_MSG %s\n", "hello world");
    ERROR_MSG("ERROR_MSG %s\n", "hello world");
}
//TEST(GECO_DEBUGGING_MSGLOG, test_geco_asset_dev)
//{
//	// Runtime disable fatal assertions
//	geco::debugging::log_msg_filter_t::get_instance().has_dev_assert_ = false;
//	// when has_dev_assert_ = false, GECO_ASSERT will not cause a REAL assert
//	GECO_ASSERT_DEV(false);
//
//	geco::debugging::log_msg_filter_t::get_instance().has_dev_assert_ = true;
//	// when has_dev_assert_ = true (default), GECO_ASSERT will cause a REAL assert like GECO_ASSERT_DEV
//	GECO_ASSERT_DEV(false);
//}
//
//TEST(GECO_DEBUGGING_MSGLOG, test_geco_asset_dev_if_not)
//{
//	// Runtime disable fatal assertions
//	log_msg_filter_t::get_instance().has_dev_assert_ = false;
//	// when has_dev_assert_ = false, GECO_ASSERT_DEV_IFNOT will not cause a REAL assert and the block ofcodes will be run
//GECO_ASSERT_DEV_IFNOT(false)
//{
//	INFO_MSG("GECO_ASSERT_DEV_IFNOT!\n");
//}
//GECO_ASSERT_DEV_IFNOT(true)
//{
//	INFO_MSG("you should not see this!\n");
//}
//
//log_msg_filter_t::get_instance()
//.has_dev_assert_ = true;
//// when has_dev_assert_ = true (default), GECO_ASSERT_DEV_IFNOT will cause a REAL assert like GECO_ASSERT_DEV
//GECO_ASSERT_DEV_IFNOT(true)
//{
//	INFO_MSG("i MA HERE\n");
//}
//GECO_ASSERT_DEV_IFNOT(false)
//{
//	INFO_MSG("you should not see this! beause already asseeted\n");
//}
//}

#include "common/ultils/geco-engine-auth.h"
TEST(GECO_ULTILS, test_base64)
{
    std::string sample("Hellow world");
    std::string out;
    geco::ultils::decode_base64(geco::ultils::encode_base64(sample), out);
    EXPECT_STRCASEEQ(sample.c_str(), out.c_str());
}

TEST(GECO_ULTILS, test_md5)
{
    std::string sample("202cb962ac59075b964b07152d234b70");
    geco::ultils::MD5 md5;
    geco::ultils::MD5::Digest digest;
    md5.append(sample.c_str(), sample.size());
    md5.getDigest(digest);

    digest.quote();
    EXPECT_STRCASEEQ(digest.quote().c_str(), "d9b1d7db4cd6e70935368a1efb10e377");

    geco::ultils::MD5::Digest digest1;
    digest1.unquote(digest.quote());
    EXPECT_TRUE(digest1 == digest);
    EXPECT_STRCASEEQ(digest1.quote().c_str(), "d9b1d7db4cd6e70935368a1efb10e377");
}

#include "common/ultils/geco-ds-wheel-timer.h"
using namespace geco::ultils;
#define THE_END_OF_TIME ((timeout_t)-1)
#define FAIL() do{printf("Failure on line %d\n", __LINE__);	goto done;} while (0)
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
static timeout_t random_to(timeout_t min, timeout_t max)
{
    if (max <= min) return min;
    /* Not actually all that random, but should exercise the code. */
    timeout_t rand64 = rand() * (timeout_t) INT_MAX + rand();
    return min + (rand64 % (max - min));
}

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
static wtimer_t* min_timeout(wtimers_t* T)
{
    struct wtimer_t *min = NULL;
    unsigned i, j;

    for (i = 0; i < countof(T->wheel); i++)
    {
        for (j = 0; j < countof(T->wheel[i]); j++)
        {
            for (auto& to : T->wheel[i][j])
            {
                if (min == NULL) min = to;
                if (to->abs_expires < min->abs_expires) min = to;
            }
        }
    }
    return min;
} /* timeouts_min() */

/*the earlist pending timeout should greater than curr time after
 * get_exired_timer() called.*/
bool timeouts_check(wtimers_t *T, FILE *fp)
{
    timeout_t timeout;
    wtimer_t* to = min_timeout(T);
    if (to != NULL)
    {
        check(to->abs_expires > T->curtime, "missed timeout (expires:%" TIMEOUT_PRIu " <= curtime:%" TIMEOUT_PRIu ")\n",
                to->abs_expires, T->curtime);

        timeout = T->get_interval();
        check(timeout <= to->abs_expires - T->curtime,
                "wrong soft timeout (soft:%" TIMEOUT_PRIu " > hard:%" TIMEOUT_PRIu ") (expires:%" TIMEOUT_PRIu "; curtime:%" TIMEOUT_PRIu ")\n",
                timeout, (to->abs_expires - T->curtime), to->abs_expires, T->curtime);

        timeout = T->timout();
        check(timeout <= to->abs_expires - T->curtime,
                "wrong soft timeout (soft:%" TIMEOUT_PRIu " > hard:%" TIMEOUT_PRIu ") (expires:%" TIMEOUT_PRIu "; curtime:%" TIMEOUT_PRIu ")\n",
                timeout, (to->abs_expires - T->curtime), to->abs_expires, T->curtime);
    }
    else
    {
        timeout = T->timout();
        if (!T->expired.empty())
        check(timeout == 0, "wrong soft timeout (soft:%" TIMEOUT_PRIu " != hard:%" TIMEOUT_PRIu ")\n", timeout,
                TIMEOUT_C(0));
        else
            check(timeout == ~TIMEOUT_C(0), "wrong soft timeout (soft:%" TIMEOUT_PRIu " != hard:%" TIMEOUT_PRIu ")\n",
                    timeout, ~TIMEOUT_C(0));
    }
    return 1;
}

static int check_randomized(const struct rand_cfg *cfg)
{
    printf("+++++++++++++++=CheckRandomized+++++++++++++++++\n");
    uint64_t i, j, err;
    int rv = 1;
    wtimer_t* t = new wtimer_t[cfg->n_timeouts];
    timeout_t* timeouts = new timeout_t[cfg->n_timeouts];
    uint8_t *fired = new uint8_t[cfg->n_timeouts];
    uint8_t *found = new uint8_t[cfg->n_timeouts];
    uint8_t *deleted = new uint8_t[cfg->n_timeouts];
    memset(t, 0, sizeof(wtimer_t) * cfg->n_timeouts);
    memset(timeouts, 0, sizeof(timeout_t) * cfg->n_timeouts);
    memset(fired, 0, sizeof(uint8_t) * cfg->n_timeouts);
    memset(deleted, 0, sizeof(uint8_t) * cfg->n_timeouts);
    memset(found, 0, sizeof(uint8_t) * cfg->n_timeouts);

//check open
    wtimers_t tos;
    tos.open(TIMEOUT_mHZ);
    EXPECT_EQ(tos.get_hz(), TIMEOUT_mHZ);
    EXPECT_EQ(tos.curtime, 0);
    for (i = 0; i < WHEEL_NUM; i++)
    {
        EXPECT_EQ(tos.pending[i], 0);
    }

    timeout_t now = cfg->start_at;
    int n_added_pending = 0, cnt_added_pending = 0;
    int n_added_expired = 0, cnt_added_expired = 0;
    wtimer_iterator_t it_p, it_e, it_all;
    int p_done = 0, e_done = 0, all_done = 0;
    wtimer_t *to = NULL;
    const int rel = cfg->relative;

    EXPECT_EQ(!t || !timeouts || !fired || !found || !deleted, false);

    printf("---------------init and add all sample timers ---------------\n");
    tos.update(cfg->start_at);
    EXPECT_EQ(!!n_added_pending, tos.has_expiring_timer());
    EXPECT_EQ(!!n_added_expired, tos.has_expired_timer());

    for (i = 0; i < cfg->n_timeouts; ++i)
    {
        t[i].init(rel ? 0 : ABS_TIMEOUT);
        EXPECT_EQ(t[i].pending_wheel(), false);
        EXPECT_EQ(t[i].pending_expired(), false);

        timeouts[i] = random_to(cfg->min_timeout, cfg->max_timeout);
        tos.add_timer(t + i, timeouts[i] - (rel ? now : 0));
        if (timeouts[i] <= cfg->start_at)
        {
            EXPECT_EQ(t[i].pending_wheel(), false);
            EXPECT_EQ(t[i].pending_expired(), true);
            ++n_added_expired;
        }
        else
        {
            EXPECT_EQ(t[i].pending_wheel(), true);
            EXPECT_EQ(t[i].pending_expired(), false);
            ++n_added_pending;
        }
    }

    EXPECT_EQ(!!n_added_pending, tos.has_expiring_timer());
    EXPECT_EQ(!!n_added_expired, tos.has_expired_timer());

    for (auto& to : tos.expired)
    {
        if (to)
        {
            i = to - t;
            ++found[i];
            ++cnt_added_expired;
        }
    }

    for (i = 0; i < WHEEL_NUM; i++)
    {
        for (j = 0; j < WHEEL_LEN; j++)
        {
            int idx;
            for (auto& to : tos.wheel[i][j])
            {
                if (to != NULL)
                {
                    idx = to - t;
                    ++found[idx];
                    ++cnt_added_pending;
                }
            }
        }
    }

    EXPECT_EQ(cnt_added_expired, n_added_expired);
    EXPECT_EQ(n_added_pending, cnt_added_pending);

    printf("\n---------------filter out all expired timers ---------------\n");
    while (NULL != (to = tos.get_expired_timer()))
    {
        i = to - &t[0];
        EXPECT_EQ(&t[i], to);
        EXPECT_GE(now, timeouts[i]);/* shouldn't have happened yet */
        --n_added_expired; /* drop expired timeouts. */
        ++fired[i];
    }
    EXPECT_EQ(n_added_expired, 0);

    printf("\n---------------now we test pending timers---------------\n");
    while (now < cfg->end_at)
    {
        int n_fired_this_time = 0;
        timeout_t td = tos.timout();
        timeout_t first_at = td + now;
        timeout_t oldtime = now;
        timeout_t step = random_to(1, cfg->max_step);
        now += step;
        int another;

        if (rel) tos.step(step);
        else
            tos.update(now);

        for (i = 0; i < cfg->try_removing; ++i)
        {
            int idx = rand() % cfg->n_timeouts;
            if (!fired[idx])
            {
                tos.stop_timer(&t[idx]);
                ++deleted[idx];
            }
        }

        timeout_t tm = tos.timout();
        another = (tm == 0);
        while (NULL != (to = tos.get_expired_timer()))
        {
            EXPECT_EQ(another, 1); /* Thought we saw the last one! */
            i = to - t;
            EXPECT_EQ(t + i, to);
            EXPECT_LE(timeouts[i], now);
            EXPECT_GT(timeouts[i], oldtime);
            EXPECT_GE(timeouts[i], first_at);
            fired[i]++;
            timeout_t tm = tos.timout();
            another = (tm == 0);
        }
        EXPECT_EQ(n_fired_this_time && first_at > now, false);
        EXPECT_EQ(another, 0);
        EXPECT_TRUE(timeouts_check(&tos, stderr));
    }

    for (i = 0; i < cfg->n_timeouts; ++i)
    {
        EXPECT_LE(fired[i], 1);/* Nothing fired twice. */
        if (timeouts[i] <= now)
        {
            EXPECT_TRUE(fired[i] || deleted[i]);  //CHECK ALREADY TIMEOUT
        }
        else
        {
            EXPECT_EQ(fired[i], 0);  // CHECK NOT TIMEOUT
        }

        // FIRED !DELETED
        // !FIRED !DELETED
        // !FIRED DELETED
        EXPECT_FALSE(fired[i] && deleted[i]);

        if (cfg->finalize > 1)
        {
            if (!fired[i]) tos.stop_timer(&t[i]);
        }
    }

    /* Now nothing more should fire between now and the end of time. */
    if (cfg->finalize)
    {
        tos.update(THE_END_OF_TIME);
        if (cfg->finalize > 1)
        {
            EXPECT_EQ(tos.get_expired_timer(), (wtimer_t* )0);
            EXPECT_EQ(false, tos.has_expiring_timer());
            EXPECT_EQ(false, tos.has_expired_timer());
        }
    }
    rv = 0;

    tos.close();
    EXPECT_EQ(false, tos.has_expiring_timer());
    EXPECT_EQ(false, tos.has_expired_timer());

    printf("\n---------------clearup---------------\n");
    done: if (t) delete[] t;
    if (timeouts) delete[] timeouts;
    if (fired) delete[] fired;
    if (found) delete[] found;
    if (deleted) delete[] deleted;

    return rv;
}

#define DO(fn) do {                             \
		printf("."); fflush(stdout);	\
		fn;					\
            } while (0)

#define DO_N(n, fn) do {			\
		for (j = 0; j < (n); ++j) {	\
			DO(fn);			\
        		}				\
    	} while (0)

#include <stdlib.h>
#include <stdio.h>
static void ut_rotl()
{
    unsigned int testval = (UINT32_C(1) << 1) - 1;
    //printf("%#10X\n", testval);
    testval = _rotl(testval, 0);
    //printf("%#10X\n", testval);
}

TEST(GECO_ENGINE_ULTILS, TEST_WHEEL_TIMER)
{
    int j;
    int n_failed = 0;

    struct rand_cfg cfg1 =
    {
    /*min_timeout*/1,
    /*max_timeout*/381,
    /*start_at*/100,
    /*end_at*/1000,
    /*n_timeouts*/1,
    /*max_step*/10,
    /*relative*/0,
    /*try_removing*/0,
    /*finalize*/2 };
//DO_N(1, check_randomized(&cfg1));
}

TEST(GECO_ENGINE_ULTILS, TEST_ROTL)
{
// assume that currtime has 1 scale, eclapse has 4 scales on wheel 1.
//
    wheel_t timeout_slotidx = (UINT64_C(1) << 4) - 1;
    int currtime_scales = 1;
    wheel_t pending = geco::ultils::rotl(timeout_slotidx, currtime_scales);
    char buf[1024];
    geco::ultils::Bitify(buf, sizeof(wheel_t) * 8, (unsigned char*) &pending);
    printf("%s\n", buf);
}

#include "common/ds/geco-bit-stream.h"
#include "common/plateform.h"
struct vec
{
        float x;
        float y;
        float z;
};
TEST(GecoMemoryStreamTestCase, test_all_reads_and_writes)
{
    geco_bit_stream_t s8;

    uint24_t uint24 = 24;
    uchar uint8 = 8;
    ushort uint16 = 16;
    uint uint32 = 32;
    uint64 uint64_ = 64;
    char int8 = -8;
    short int16 = -16;
    int int32 = -32;
    int64 int64_ = -64;

    uchar particialByte = 0xf0;  /// 11110000
    // guid_t guid(123);
    //NetworkAddress addr("192.168.1.107", 32000);
    vec vector_ =
    { 0.2f, -0.4f, -0.8f };
    vec vector__ =
    { 2.234f, -4.78f, -32.2f };

    uint curr = 12;
    uint min = 8;
    uint max = 64;
    s8.write_ranged_integer(curr, min, max);
    curr = 0;
    s8.ReadIntegerRange(curr, min, max);
    EXPECT_TRUE(curr == 12);

    s8.Write(uint24);
    uint24 = 0;
    s8.Read(uint24);
    EXPECT_TRUE(uint24.val == 24);

    s8.WriteBits(&particialByte, 7, false);
    uchar v = 0;
    s8.ReadBits(&v, 7, false);
    EXPECT_TRUE(particialByte == v);
    EXPECT_TRUE(s8.get_payloads() == 0);

    for (int i = 10; i >= 0; i--)
    {
        uint looptimes = 100000;
        for (uint i = 1; i <= looptimes; i++)
        {
            s8.Write(uint24);

            //s8.WriteMini(guid);

            s8.WriteMini(uint24);

            //s8.WriteMini(addr);

            s8.WriteMini(uint24);

            s8.Write(uint8);
            s8.Write(int64_);
            s8.WriteMini(uint8);
            s8.WriteMini(int64_, false);

            s8.Write(uint16);
            s8.Write(int32);
            s8.WriteMini(uint16);
            s8.WriteMini(int32, false);

            s8.WriteBits(&particialByte, 4, true);
            s8.Write(uint24);
            s8.write_normal_vector(vector_.x, vector_.y, vector_.z);

            s8.write_ranged_integer(curr, min, max);

            s8.write_vector(vector__.x, vector__.y, vector__.z);

            s8.Write(uint32);
            s8.Write(int16);
            s8.WriteMini(uint32);
            s8.WriteMini(int16, false);

            s8.WriteBits(&particialByte, 4, false);

            s8.Write(uint64_);
            s8.Write(int8);
            s8.WriteMini(uint64_);
            s8.WriteMini(int8);

            s8.WriteBits(&particialByte, 7, false);
        }

        geco_bit_stream_t s9;
        s9.Write(s8);
        if (i == 0)
        {
            printf("uncompressed  '%.5f' MB\n", float(BITS_TO_BYTES(s9.get_payloads()) / 1024 / 1024));
        }

        for (uint i = 1; i <= looptimes; i++)
        {
            uint24 = 0;
            s9.Read(uint24);
            EXPECT_TRUE(uint24.val == 24);

            //guid_t guidd;
            //s9.ReadMini(guidd);
            //EXPECT_TRUE(guid == guidd);

            uint24_t mini_uint24 = 0;
            s9.ReadMini(mini_uint24);
            EXPECT_TRUE(mini_uint24.val == 24);

            //NetworkAddress addrr;
            //s9.ReadMini(addrr);
            // EXPECT_TRUE(addr == addrr);

            mini_uint24 = 0;
            s9.ReadMini(mini_uint24);
            EXPECT_TRUE(mini_uint24.val == 24);

            s9.Read(uint8);
            s9.Read(int64_);
            uchar mini_uint8;
            s9.ReadMini(mini_uint8);
            EXPECT_TRUE(mini_uint8 == uint8);

            int64 mini_int64;
            s9.ReadMini(mini_int64, false);
            EXPECT_TRUE(mini_int64 == int64_);

            s9.Read(uint16);
            s9.Read(int32);
            ushort mini_uint16;
            s9.ReadMini(mini_uint16);
            EXPECT_TRUE(mini_uint16 == uint16);

            int mini_int32;
            s9.ReadMini(mini_int32, false);
            EXPECT_TRUE(mini_int32 == int32);

            uchar v = 0;
            s9.ReadBits(&v, 4, true);
            EXPECT_TRUE(v == 0);

            uint24 = 0;
            s9.Read(uint24);
            EXPECT_TRUE(uint24.val == 24);

            vec vectorr;
            s9.ReadNormVector(vectorr.x, vectorr.y, vectorr.z);
            EXPECT_TRUE(fabs(vectorr.x - vector_.x) <= 0.0001f);
            EXPECT_TRUE(fabs(vectorr.y - vector_.y) <= 0.0001f);
            EXPECT_TRUE(fabs(vectorr.y - vector_.y) <= 0.0001f);

            uint v1;
            s9.ReadIntegerRange(v1, min, max);
            EXPECT_TRUE(v1 == curr);

            vec vectorrr;
            s9.ReadVector(vectorrr.x, vectorrr.y, vectorrr.z);
            EXPECT_TRUE(fabs(vectorrr.x - vector__.x) <= 0.001f);
            EXPECT_TRUE(fabs(vectorrr.y - vector__.y) <= 0.001f);
            EXPECT_TRUE(fabs(vectorrr.y - vector__.y) <= 0.001f);

            s9.Read(uint32);
            s9.Read(int16);
            uint mini_uint32;
            s9.ReadMini(mini_uint32);
            EXPECT_TRUE(mini_uint32 == uint32);

            short mini_int16;
            s9.ReadMini(mini_int16, false);
            EXPECT_TRUE(mini_int16 == int16);

            v = 0;
            s9.ReadBits(&v, 4, false);
            EXPECT_TRUE(particialByte == ((v >> 4) << 4));

            s9.Read(uint64_);
            s9.Read(int8);
            uint64 mini_uint64;
            s9.ReadMini(mini_uint64);
            EXPECT_TRUE(mini_uint64 == uint64_);

            char mini_int8;
            s9.ReadMini(mini_int8, false);
            EXPECT_TRUE(mini_int8 == int8);

            v = 0;
            s9.ReadBits(&v, 7, false);
            EXPECT_TRUE(particialByte == ((v >> 1) << 1));

            EXPECT_TRUE(uint8 == 8);
            EXPECT_TRUE(int8 == -8);
            EXPECT_TRUE(uint16 == 16);
            EXPECT_TRUE(int16 == -16);
            EXPECT_TRUE(uint24.val == 24);
            EXPECT_TRUE(uint32 == 32);
            EXPECT_TRUE(int32 == -32);
            EXPECT_TRUE(uint64_ == 64);
            EXPECT_TRUE(int64_ == -64);
        }

        s8.reset();
        s9.reset();
    }
}
TEST(GecoMemoryStreamTestCase, test_all_reads_and_writes_un_compressed)
{
    geco_bit_stream_t s8;

    uint24_t uint24 = 24;
    uchar uint8 = 8;
    ushort uint16 = 16;
    uint uint32 = 32;
    uint64 uint64_ = 64;
    char int8 = -8;
    short int16 = -16;
    int int32 = -32;
    int64 int64_ = -64;

    uchar particialByte = 0xf0;  /// 11110000
    //guid_t guid(123);
    //NetworkAddress addr("192.168.1.107", 32000);
    vec vector_ =
    { 0.2f, -0.4f, -0.8f };
    vec vector__ =
    { 2.234f, -4.78f, -32.2f };

    uint curr = 12;
    uint min = 8;
    uint max = 64;
    s8.write_ranged_integer(curr, min, max);
    curr = 0;
    s8.ReadIntegerRange(curr, min, max);
    EXPECT_TRUE(curr == 12);

    s8.Write(uint24);
    uint24 = 0;
    s8.Read(uint24);
    EXPECT_TRUE(uint24.val == 24);

    s8.WriteBits(&particialByte, 7, false);
    uchar v = 0;
    s8.ReadBits(&v, 7, false);
    EXPECT_TRUE(particialByte == v);
    EXPECT_TRUE(s8.get_payloads() == 0);

    for (int i = 10; i >= 0; i--)
    {
        uint looptimes = 100000;
        for (uint i = 1; i <= looptimes; i++)
        {
            s8.Write(uint24);

            //s8.Write(guid);

            s8.Write(uint24);

            //s8.Write(addr);

            s8.Write(uint24);

            s8.Write(uint8);
            s8.Write(int64_);
            s8.Write(uint8);
            s8.Write(int64_);

            s8.Write(uint16);
            s8.Write(int32);
            s8.Write(uint16);
            s8.Write(int32);

            s8.WriteBits(&particialByte, 4, true);
            s8.Write(uint24);
            s8.write_normal_vector(vector_.x, vector_.y, vector_.z);

            s8.write_ranged_integer(curr, min, max);

            s8.write_vector(vector__.x, vector__.y, vector__.z);

            s8.Write(uint32);
            s8.Write(int16);
            s8.Write(uint32);
            s8.Write(int16);

            s8.WriteBits(&particialByte, 4, false);

            s8.Write(uint64_);
            s8.Write(int8);
            s8.Write(uint64_);
            s8.Write(int8);

            s8.WriteBits(&particialByte, 7, false);
        }

        geco_bit_stream_t s9;
        s9.Write(s8);

        if (i == 0)
        {
            printf("uncompressed  '%.5f' MB\n", float(BITS_TO_BYTES(s9.get_payloads()) / 1024 / 1024));
        }

        for (uint i = 1; i <= looptimes; i++)
        {
            uint24 = 0;
            s9.Read(uint24);
            EXPECT_TRUE(uint24.val == 24);

//            guid_t guidd;
//            s9.Read(guidd);
//            EXPECT_TRUE(guid == guidd);

            uint24_t mini_uint24 = 0;
            s9.Read(mini_uint24);
            EXPECT_TRUE(mini_uint24.val == 24);

            //NetworkAddress addrr;
            //s9.Read(addrr);
            //EXPECT_TRUE(addr == addrr);

            mini_uint24 = 0;
            s9.Read(mini_uint24);
            EXPECT_TRUE(mini_uint24.val == 24);

            s9.Read(uint8);
            s9.Read(int64_);
            uchar mini_uint8;
            s9.Read(mini_uint8);
            EXPECT_TRUE(mini_uint8 == uint8);

            int64 mini_int64;
            s9.Read(mini_int64);
            EXPECT_TRUE(mini_int64 == int64_);

            s9.Read(uint16);
            s9.Read(int32);
            ushort mini_uint16;
            s9.Read(mini_uint16);
            EXPECT_TRUE(mini_uint16 == uint16);

            int mini_int32;
            s9.Read(mini_int32);
            EXPECT_TRUE(mini_int32 == int32);

            uchar v = 0;
            s9.ReadBits(&v, 4, true);
            EXPECT_TRUE(v == 0);

            uint24 = 0;
            s9.Read(uint24);
            EXPECT_TRUE(uint24.val == 24);

            vec vectorr;
            s9.ReadNormVector(vectorr.x, vectorr.y, vectorr.z);
            EXPECT_TRUE(fabs(vectorr.x - vector_.x) <= 0.0001f);
            EXPECT_TRUE(fabs(vectorr.y - vector_.y) <= 0.0001f);
            EXPECT_TRUE(fabs(vectorr.y - vector_.y) <= 0.0001f);

            uint v1;
            s9.ReadIntegerRange(v1, min, max);
            EXPECT_TRUE(v1 == curr);

            vec vectorrr;
            s9.ReadVector(vectorrr.x, vectorrr.y, vectorrr.z);
            EXPECT_TRUE(fabs(vectorrr.x - vector__.x) <= 0.001f);
            EXPECT_TRUE(fabs(vectorrr.y - vector__.y) <= 0.001f);
            EXPECT_TRUE(fabs(vectorrr.y - vector__.y) <= 0.001f);

            s9.Read(uint32);
            s9.Read(int16);
            uint mini_uint32;
            s9.Read(mini_uint32);
            EXPECT_TRUE(mini_uint32 == uint32);

            short mini_int16;
            s9.Read(mini_int16);
            EXPECT_TRUE(mini_int16 == int16);

            v = 0;
            s9.ReadBits(&v, 4, false);
            EXPECT_TRUE(particialByte == ((v >> 4) << 4));

            s9.Read(uint64_);
            s9.Read(int8);
            uint64 mini_uint64;
            s9.Read(mini_uint64);
            EXPECT_TRUE(mini_uint64 == uint64_);

            char mini_int8;
            s9.Read(mini_int8);
            EXPECT_TRUE(mini_int8 == int8);

            v = 0;
            s9.ReadBits(&v, 7, false);
            EXPECT_TRUE(particialByte == ((v >> 1) << 1));

            EXPECT_TRUE(uint8 == 8);
            EXPECT_TRUE(int8 == -8);
            EXPECT_TRUE(uint16 == 16);
            EXPECT_TRUE(int16 == -16);
            EXPECT_TRUE(uint24.val == 24);
            EXPECT_TRUE(uint32 == 32);
            EXPECT_TRUE(int32 == -32);
            EXPECT_TRUE(uint64_ == 64);
            EXPECT_TRUE(int64_ == -64);
        }

        s8.reset();
        s9.reset();
    }
}

