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
       //::testing::GTEST_FLAG(filter) = "GECO_DEBUGGING_WATCHER.test_watcher_path_request_v2";
        int retval = 0;
        retval = RUN_ALL_TESTS();
        return retval;
    }
