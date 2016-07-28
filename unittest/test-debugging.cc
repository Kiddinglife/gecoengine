#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <functional>

#include "gtest/gtest.h"
#include "common/plateform.h"
#include "common/ultils/ultils.h"
#include "common/debugging/debug.h"

using namespace geco::debugging;
using namespace geco::ultils;
using namespace geco::ds;

DECLARE_DEBUG_COMPONENT2("UNIT-TEST", 0);

/**
 *  unit tests have:
 *  1) TEST(GECO_ENGINE_ULTILS, TEST_WHEEL_TIMER_BIT_OPS)
 *
 */

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
TEST(GECO_DEBUGGING_MSGLOG, test_geco_asset_dev)
{
    // Runtime disable fatal assertions
    geco::debugging::log_msg_filter_t::get_instance().has_dev_assert_ = false;
    // when has_dev_assert_ = false, GECO_ASSERT will not cause a REAL assert
    GECO_ASSERT_DEV(false);

    geco::debugging::log_msg_filter_t::get_instance().has_dev_assert_ = true;
    // when has_dev_assert_ = true (default), GECO_ASSERT will cause a REAL assert like GECO_ASSERT_DEV
    GECO_ASSERT_DEV(false);
}

TEST(GECO_DEBUGGING_MSGLOG, test_geco_asset_dev_if_not)
{
    // when has_dev_assert_ = false, GECO_ASSERT_DEV_IFNOT will NOT cause a REAL assert and the block ofcodes will be run
    log_msg_filter_t::get_instance().has_dev_assert_ = false;
    GECO_ASSERT_DEV_IFNOT(false)
    {
        INFO_MSG("GECO_ASSERT_DEV_IFNOT!\n");
    }
    GECO_ASSERT_DEV_IFNOT(true)
    {
        INFO_MSG("you should not see this!\n");
    }

    // when has_dev_assert_ = true (default), GECO_ASSERT_DEV_IFNOT will cause a REAL assert like GECO_ASSERT_DEV
    log_msg_filter_t::get_instance().has_dev_assert_ = true;
    GECO_ASSERT_DEV_IFNOT(true)
    {
        INFO_MSG("i MA HERE\n");
    }
    GECO_ASSERT_DEV_IFNOT(false)
    {
        INFO_MSG("you should not see this! beause already asseeted\n");
    }
}

