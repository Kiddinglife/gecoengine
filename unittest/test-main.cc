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
	std::cout << "Running main() from gmock_main.cc, argc=" << argc << std::endl;

	// Since Google Mock depends on Google Test, InitGoogleMock() is
	// also responsible for initializing Google Test.  Therefore there's
	// no need for calling testing::InitGoogleTest() separately.
	testing::InitGoogleMock(&argc, argv);

	//::testing::GTEST_FLAG(filter) = "GECO_ULTILS.encode64_when_given_string_and_then_decode64_back_to_string_result_should_be_same";
	//::testing::GTEST_FLAG(filter) = "GECO_ULTILS.test_geco_cmdline_t";

	//::testing::GTEST_FLAG(filter) = "RMI.*";

	// caution rdtsc imestamp is always accuate in linux but very rough in windows
	// ::testing::GTEST_FLAG(filter) = "TIME.test_profile";
	//::testing::GTEST_FLAG(filter) = "TIME.test_gettimestamp_func";
	//::testing::GTEST_FLAG(filter) = "TIME.test_timer_queue";

	//last run and pass on 14:18 PM 4 Oct 2016
	//::testing::GTEST_FLAG(filter) = "GecoMemoryStreamTestCase.*";
	//::testing::GTEST_FLAG(filter) = "GecoMemoryStreamTestCase.test_haffman_compression";
	//::testing::GTEST_FLAG(filter) ="GecoMemoryStreamTestCase.test_geco_string_compressor";
	//::testing::GTEST_FLAG(filter) = "GecoMemoryStreamTestCase.test_float_compression";
	// ::testing::GTEST_FLAG(filter) = "GecoMemoryStreamTestCase.test_run_length";
	// ::testing::GTEST_FLAG(filter) = "GecoMemoryStreamTestCase.test_all_reads_and_writes";
	//::testing::GTEST_FLAG(filter) = "GecoMemoryStreamTestCase.test_all_reads_and_writes_un_compressed";

	//::testing::GTEST_FLAG(filter) = "geco_engine_network.test_msg_handler_macros";
	//::testing::GTEST_FLAG(filter) = "network.test_geco_net_addr";
	::testing::GTEST_FLAG(filter) = "network.test_geco_endpoint";

	//last run and pass on 14:18 PM 4 Oct 2016
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_TIMESTAMP.test_gettimestamp_func";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_MSGLOG.test_msg_macros";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_MSGLOG.test_msg_cb";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_MSGLOG.test_critical_msg";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_MSGLOG.geco_asset_dev_if_not_does_block_codes_will_not_run_when_has_dev_assert_is_true";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_MSGLOG.geco_asset_dev_if_not_does_block_codes_will_run_when_has_dev_assert_is_false_and_assert_value_false";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_MSGLOG.geco_asset_dev_if_not_does_block_codes_will_not_run_when_has_dev_assert_is_false_and_assert_value_true";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_MSGLOG.geco_asset_dev_if_not_does_block_codes_will_not_run_when_has_dev_assert_is_false_and_assert_value_true";

	//last run and pass on 14:18 PM 4 Oct 2016
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_WATCHER.*";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_WATCHER.transformation_between_watchervalue_and_stringval_result_should_be_same";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_WATCHER.transformation_between_watchervalue_and_request_stream__result_should_be_same";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_WATCHER.add_remove_value_func_method_watchers";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_WATCHER.test_watcher_path_request_v2";
	//::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_WATCHER.test_sequence_watcher";

	return RUN_ALL_TESTS();
}
