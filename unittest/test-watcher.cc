/*
 * test-watcher.cc
 *
 *  Created on: 25Jul.,2016
 *      Author: jackiez
 */

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
#include "common/debugging/gecowatchert.h"

using namespace geco::debugging;
using namespace geco::ultils;
using namespace geco::ds;

DECLARE_DEBUG_COMPONENT2("ENGINE-UNITTEST-LOGGER", 0);

TEST(GECO_DEBUGGING_WATCHER, test_value_string)
{
    std::string valstr;

    for (int i = 0; i < 100000; i++)
    {
        int int_ret;
        valstr = write_watcher_value_to_string((int) 12);
        read_watcher_value_from_string(valstr.c_str(), int_ret);
        EXPECT_EQ((int )12, int_ret);

        float float_ret;
        valstr = write_watcher_value_to_string(12.001f);
        read_watcher_value_from_string(valstr.c_str(), float_ret);
        EXPECT_EQ(12.001f, float_ret);

        std::string str_ret;
        std::string str_input("hello world");
        valstr = write_watcher_value_to_string(str_input);
        read_watcher_value_from_string(valstr.c_str(), str_ret);
        EXPECT_STRCASEEQ(str_input.c_str(), str_ret.c_str());

        bool bool_ret;
        valstr = write_watcher_value_to_string(true);
        read_watcher_value_from_string(valstr.c_str(), bool_ret);
        EXPECT_TRUE(bool_ret);
        valstr = write_watcher_value_to_string(false);
        read_watcher_value_from_string(valstr.c_str(), bool_ret);
        EXPECT_FALSE(bool_ret);
    }
}

TEST(GECO_DEBUGGING_WATCHER, test_value_stream)
{
    geco_bit_stream_t ret;
    WatcherMode mode;
    WatcherValueType type;

    for (int i = 0; i < 100000; i++)
    {

        int int_in = 12;
        write_watcher_value_to_stream(ret, WVT_INTEGER, int_in, WT_READ_WRITE, sizeof(int));
        uint64 uint64_in = 234726728;
        write_watcher_value_to_stream(ret, WVT_INTEGER, uint64_in, WT_READ_WRITE, sizeof(uint64));
        float float_in = 12.001f;
        write_watcher_value_to_stream(ret, WVT_FLOATING, float_in, WT_READ_ONLY, sizeof(int));
        double double_in = 12.00000001;
        write_watcher_value_to_stream(ret, WVT_FLOATING, double_in, WT_READ_ONLY, sizeof(double));
        char blob_in[32] =
        { 0 };
        blob_in[0] = 0;
        blob_in[1] = 1;
        blob_in[2] = 2;
        blob_in[15] = 15;
        blob_in[31] = 31;
        write_watcher_value_to_stream(ret, WVT_BLOB, blob_in, WT_READ_ONLY, 32);
        char str_in[] = "hello worldhello world2hello world2hello world2";
        write_watcher_value_to_stream(ret, WVT_STRING, str_in, WT_READ_ONLY, sizeof(str_in));
        std::string string_in = "hello world2hello world2hello world2hello world2hello world2";
        write_watcher_value_to_stream(ret, WVT_STRING, string_in, WT_READ_ONLY, string_in.length());

        uint bytes;
        int int_out;
        read_watcher_value_from_stream(ret, type, int_out, mode, bytes);
        EXPECT_EQ(int_in, int_out);
        EXPECT_EQ(mode, WT_READ_WRITE);
        EXPECT_EQ(type, WVT_INTEGER);
        EXPECT_EQ(bytes, sizeof(int));
        uint64 uint64_out;
        read_watcher_value_from_stream(ret, type, uint64_out, mode, bytes);
        EXPECT_EQ(uint64_in, uint64_out);
        EXPECT_EQ(mode, WT_READ_WRITE);
        EXPECT_EQ(type, WVT_INTEGER);
        EXPECT_EQ(bytes, sizeof(uint64));
        float float_out;
        read_watcher_value_from_stream(ret, type, float_out, mode, bytes);
        EXPECT_EQ(float_in, float_out);
        EXPECT_EQ(mode, WT_READ_ONLY);
        EXPECT_EQ(type, WVT_FLOATING);
        EXPECT_EQ(bytes, sizeof(float));
        double double_out;
        read_watcher_value_from_stream(ret, type, double_out, mode, bytes);
        EXPECT_EQ(double_in, double_out);
        EXPECT_EQ(mode, WT_READ_ONLY);
        EXPECT_EQ(type, WVT_FLOATING);
        EXPECT_EQ(bytes, sizeof(double));
        char blob_out[32] =
        { 0 };
        read_watcher_value_from_stream(ret, type, blob_out, mode, bytes);
        int cmp = memcmp(blob_in, blob_out, 32);
        EXPECT_FALSE(memcmp(blob_in, blob_out, 32));
        EXPECT_EQ(mode, WT_READ_ONLY);
        EXPECT_EQ(type, WVT_BLOB);
        EXPECT_EQ(bytes, 32);
        char str_out[1024];
        read_watcher_value_from_stream(ret, type, str_out, mode, bytes);
        EXPECT_STREQ(str_in, str_out);
        EXPECT_EQ(mode, WT_READ_ONLY);
        EXPECT_EQ(type, WVT_STRING);
        EXPECT_EQ(bytes, sizeof(str_in));
        std::string string_out;
        read_watcher_value_from_stream(ret, type, string_out, mode, bytes);
        EXPECT_STREQ(string_in.c_str(), string_out.c_str());
        EXPECT_EQ(bytes, string_out.length());
    }
}
static int value = 11;
class ExampleClass
{
    public:
        int& getValue()
        {
            return a;
        }
        void setValue(int& setValue)
        {
            a = setValue;
        }

    private:
        int a;
};
static std::string stringval("");
static std::string& get_val()
{
    return stringval;
}
static void set_val(std::string& a)
{
    stringval = a;
}
TEST(GECO_DEBUGGING_WATCHER, test_add_remove_value_func_method_watchers)
{
    const char* path1 = "WVT_INTEGER/WT_READ_ONLY/GECO_WATCH_VALUE";
    GECO_WATCH_VALUE(path1,value,WVT_INTEGER, (uint)sizeof(int),WT_READ_ONLY, "GECO_WATCH_VALUE->WVT_INTEGER->WT_READ_ONLY");

    ExampleClass example;
    const char* path2 = "WVT_INTEGER/WT_READ_WRITE/GECO_WATCH_METHOD";
    GECO_WATCH_METHOD(path2,WVT_INTEGER, (uint)sizeof(int),example, CAST_METHOD_RW(int, ExampleClass, getValue, setValue),
    "GECO_WATCH_METHOD->WVT_INTEGER->GECO_WATCH_METHOD");

    const char* path3 = "WVT_STRING/WT_READ_WRITE/GECO_WATCH_FUNC";
    GECO_WATCH_FUNC(path3,WVT_STRING, (uint)stringval.length(),CAST_FUNC_RW(std::string, get_val, set_val), "GECO_WATCH_FUNC->WVT_STRING->CAST_FUNC_RW");

    std::string path;
    std::string paths;
    int num = 0;
    geco_watcher_base_t::get_root_watcher().walk_all_paths(path, paths, num, true);

    EXPECT_TRUE(geco_watcher_base_t::get_root_watcher().remove_watcher(path3));
    EXPECT_TRUE(geco_watcher_base_t::get_root_watcher().remove_watcher(path1));
    EXPECT_TRUE(geco_watcher_base_t::get_root_watcher().remove_watcher(path2));

    path.clear();
    paths.clear();
    num = 0;
    geco_watcher_base_t::get_root_watcher().walk_all_paths(path, paths, num, true);
}

static void req1_cb(watcher_path_request & pathRequest, int32 count)
{
    ASSERT_EQ(count, 1);

    WatcherMode mode;
    WatcherValueType type;
    uint bytes;
    bool desc;
    std::string result;
    std::string reqpath;
    std::string descstr;

    geco_bit_stream_t& is = ((watcher_path_request_v1&) pathRequest).get_result_stream();
    read_watcher_value_from_stream(is, type, reqpath, mode, bytes);
    read_watcher_value_from_stream(is, type, result, mode, bytes);
    read_watcher_value_from_stream(is, type, desc, mode, bytes);
    if (desc) read_watcher_value_from_stream(is, type, descstr, mode, bytes);

    EXPECT_STREQ(reqpath.c_str(), "logger/cppThresholds/ENGINE-UNITTEST-LOGGER/cc");
    EXPECT_STREQ(result.c_str(), "25000");
    if (desc) EXPECT_STREQ(descstr.c_str(), "log cc value");

    std::string path;
    std::string paths;
    int num = 0;
    geco_watcher_base_t::get_root_watcher().walk_all_paths(path, paths, num, true);

    EXPECT_TRUE(geco_watcher_base_t::get_root_watcher().remove_watcher(reqpath.c_str()));
}
TEST(GECO_DEBUGGING_WATCHER, test_watcher_path_request_v1)
{
    //this is done in remote endpoint
    int hp = 100;
    const char* path = "logger/cppThresholds/ENGINE-UNITTEST-LOGGER/cc";
    GECO_WATCH_VALUE(path, hp,WVT_INTEGER, sizeof(int), WT_READ_WRITE, "log cc value");

    // this is done in another endpoint to query
    watcher_path_request_v1 req1(path, true);
    req1.set_request_complete_cb(req1_cb);  // do network things in this cb
    req1.set_setopt_string("25000");
    req1.set_watcher_value();    // set value that will be sent to remote endpoint and update the value
}

static void req2_cb(watcher_path_request & pathRequest, int32 count)
{
    ASSERT_EQ(count, 1);

    WatcherMode mode;
    WatcherValueType type;
    uint bytes;
    bool desc;
    std::string result;
    std::string reqpath;
    std::string descstr;

    geco_bit_stream_t& is = ((watcher_path_request_v1&) pathRequest).get_result_stream();
    read_watcher_value_from_stream(is, type, reqpath, mode, bytes);
    read_watcher_value_from_stream(is, type, result, mode, bytes);
    read_watcher_value_from_stream(is, type, desc, mode, bytes);
    if (desc) read_watcher_value_from_stream(is, type, descstr, mode, bytes);

    EXPECT_STREQ(reqpath.c_str(), "logger/cppThresholds/ENGINE-UNITTEST-LOGGER/cc");
    EXPECT_STREQ(result.c_str(), "25000");
    if (desc) EXPECT_STREQ(descstr.c_str(), "log cc value");

    std::string path;
    std::string paths;
    int num = 0;
    geco_watcher_base_t::get_root_watcher().walk_all_paths(path, paths, num, true);

    EXPECT_TRUE(geco_watcher_base_t::get_root_watcher().remove_watcher(reqpath.c_str()));
}
TEST(GECO_DEBUGGING_WATCHER, test_watcher_path_request_v2)
{
    //this is done in remote endpoint
    int hp = 100;
    const char* path = "logger/cppThresholds/ENGINE-UNITTEST-LOGGER/cc";
    GECO_WATCH_VALUE(path, hp,WVT_INTEGER, sizeof(int), WT_READ_WRITE, "log cc value");

    // this is done in another endpoint to query
    watcher_path_request_v2 req2(path);
}



