#include <iostream>
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#if GTEST_OS_WINDOWS_MOBILE
# include <tchar.h>  // NOLINT
GTEST_API_ int _tmain(int argc, TCHAR** argv)
{
#else
GTEST_API_ int main(int argc, char** argv)
{
#endif  // GTEST_OS_WINDOWS_MOBILE
	std::cout << "Running main() from gmock_main.cc\n";

	// Since Google Mock depends on Google Test, InitGoogleMock() is
	// also responsible for initializing Google Test.  Therefore there's
	// no need for calling testing::InitGoogleTest() separately.
	testing::InitGoogleMock(&argc, argv);

   // ::testing::GTEST_FLAG(filter) = "GecoMemoryStreamTestCase.*";
    //::testing::GTEST_FLAG(filter) = "GecoMemoryStreamTestCase.test_haffman_compression";
    //::testing::GTEST_FLAG(filter) = "GecoMemoryStreamTestCase.test_float_compression";
   // ::testing::GTEST_FLAG(filter) = "GecoMemoryStreamTestCase.test_run_length";
	//::testing::GTEST_FLAG(filter) = "GECO_ULTILS.encode64_when_given_string_and_then_decode64_back_to_string_result_should_be_same";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_MSGLOG.test_msg_macros";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_MSGLOG.geco_asset_dev_if_not_does_block_codes_will_not_run_when_has_dev_assert_is_true";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_MSGLOG.geco_asset_dev_if_not_does_block_codes_will_run_when_has_dev_assert_is_false_and_assert_value_false";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_MSGLOG.geco_asset_dev_if_not_does_block_codes_will_not_run_when_has_dev_assert_is_false_and_assert_value_true";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_MSGLOG.geco_asset_dev_if_not_does_block_codes_will_not_run_when_has_dev_assert_is_false_and_assert_value_true";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_WATCHER.transformation_between_watchervalue_and_stringval_result_should_be_same";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_WATCHER.transformation_between_watchervalue_and_request_stream__result_should_be_same";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_WATCHER.add_remove_value_func_method_watchers";
	::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_WATCHER.test_watcher_path_request_v2";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_WATCHER.test_watcher_path_request_v1";

	return RUN_ALL_TESTS();
}
