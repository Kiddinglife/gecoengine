#include "gtest/gtest.h"
#include <stdio.h>
#include <stdlib.h>

//#define TIMEOUT_DISABLE_BUILTIN_BITOPS
#include "common/ultils/geco-engine-ultils.h"
using namespace geco::ultils;
static int naive_clz(int bits, uint64_t v)
{
    int r = 0;
    uint64_t bit = ((uint64_t) 1) << (bits-1);
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
        if (r == bits)
            break;
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
        printf("mismatch with %d ctz64: %d, naive_ctz: %d\n",
                vv, ctz64(vv),naive_ctz(64, vv));
        exit(1);
        return 0;
    }
    if (clz64(vv) != naive_clz(64, vv))
    {
        printf("mismatch with %d clz64: %d, %d\n",
                vv, clz64(vv),naive_clz(64, vv));
        exit(1);
        return 0;
    }

    if (v32 == 0)
        return 1; /* c[lt]z(0) is undefined. */

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
    uint64_t testcases[] = { 13371337 * 10, 100, 385789752, 82574,
            (((uint64_t) 1) << 63) + (((uint64_t) 1) << 31) + 10101 };

    unsigned int i;
    const unsigned int n = sizeof(testcases) / sizeof(testcases[0]);
    int result = 0;

    for (i = 0; i <= 63; ++i)
    {
        uint64_t x = 1 << i;
        if (!check(x))
            result = 1;
        --x;
        if (!check(x))
            result = 1;
    }
    for (i = 0; i < n; ++i)
    {
        if (!check(testcases[i]))
            result = 1;
    }
    EXPECT_EQ(result, 0);
    uint64_t a = 12;
    int count = ctz64(a);
    printf("ctz64(%lu)->%d", a, count);
}
