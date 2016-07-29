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

DECLARE_DEBUG_COMPONENT2("UNIT-TEST", 0);

TEST(GECO_DEBUGGING_WATCHER, test_value_string)
{
    std::string valstr;

    for (int i = 0; i < 100000; i++)
    {
        int int_ret;
        valstr = watcher_value_to_string((int) 12);
        watcher_string_to_value(valstr.c_str(), int_ret);
        EXPECT_EQ((int )12, int_ret);

        float float_ret;
        valstr = watcher_value_to_string(12.001f);
        watcher_string_to_value(valstr.c_str(), float_ret);
        EXPECT_EQ(12.001f, float_ret);

        std::string str_ret;
        std::string str_input("hello world");
        valstr = watcher_value_to_string(str_input);
        watcher_string_to_value(valstr.c_str(), str_ret);
        EXPECT_STRCASEEQ(str_input.c_str(), str_ret.c_str());

        bool bool_ret;
        valstr = watcher_value_to_string(true);
        watcher_string_to_value(valstr.c_str(), bool_ret);
        EXPECT_TRUE(bool_ret);
        valstr = watcher_value_to_string(false);
        watcher_string_to_value(valstr.c_str(), bool_ret);
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
        write_watcher_value_to_stream(ret, int_in, WT_READ_WRITE);
        uint64 uint64_in = 234726728;
        write_watcher_value_to_stream(ret, uint64_in, WT_READ_WRITE);
        float float_in = 12.001f;
        write_watcher_value_to_stream(ret, float_in, WT_READ_ONLY);
        double double_in = 12.00000001;
        write_watcher_value_to_stream(ret, double_in, WT_READ_ONLY);
        char str_in[] = "hello worldhello world2hello world2hello world2";
        write_watcher_value_to_stream(ret, str_in, WT_READ_ONLY);
        std::string string_in = "hello world2hello world2hello world2hello world2hello world2";
        write_watcher_value_to_stream(ret, string_in, WT_READ_ONLY);

        uint bytes;
        int int_out;
        read_watcher_value_from_stream(ret, int_out);
        EXPECT_EQ(int_in, int_out);
        uint64 uint64_out;
        read_watcher_value_from_stream(ret, uint64_out);
        float float_out;
        read_watcher_value_from_stream(ret, float_out);
        double double_out;
        read_watcher_value_from_stream(ret, double_out);
        char str_out[1024];
        read_watcher_value_from_stream(ret, str_out);
        EXPECT_STREQ(str_in, str_out);
        std::string string_out;
        read_watcher_value_from_stream(ret, string_out);
        EXPECT_STREQ(string_in.c_str(), string_out.c_str());
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
static std::string stringval("hello man");
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
    GECO_WATCH(path1, value, WT_READ_ONLY, "GECO_WATCH_VALUE->WVT_INTEGER->WT_READ_ONLY");

    ExampleClass example;
    const char* path2 = "WVT_INTEGER/WT_READ_WRITE/GECO_WATCH_METHOD";
    GECO_WATCH(path2, example, CAST_METHOD_RW(int, ExampleClass, getValue, setValue),
            "GECO_WATCH_METHOD->WVT_INTEGER->GECO_WATCH_METHOD");

    const char* path3 = "WVT_STRING/WT_READ_WRITE/GECO_WATCH_FUNC";
    GECO_WATCH(path3, CAST_FUNC_RW(std::string, get_val, set_val), "GECO_WATCH_FUNC->WVT_STRING->CAST_FUNC_RW");

    std::string path;
    std::string paths;
    int num = 0;
    geco_watcher_base_t::get_root_watcher().walk_all_files(path, paths, num, true);

    EXPECT_TRUE(geco_watcher_base_t::get_root_watcher().remove_watcher(path3));
    EXPECT_TRUE(geco_watcher_base_t::get_root_watcher().remove_watcher(path1));
    EXPECT_TRUE(geco_watcher_base_t::get_root_watcher().remove_watcher(path2));

    path.clear();
    paths.clear();
    num = 0;
    geco_watcher_base_t::get_root_watcher().walk_all_files(path, paths, num, true);
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
    read_watcher_value_from_stream(is, reqpath);
    read_watcher_value_from_stream(is, result);
    //read_watcher_value_from_stream(is, type, desc, mode, bytes);
    if (desc) read_watcher_value_from_stream(is, descstr);

    EXPECT_STREQ(reqpath.c_str(), "logger/cppThresholds/ENGINE-UNITTEST-LOGGER/cc");
    EXPECT_STREQ(result.c_str(), "25000");
    if (desc) EXPECT_STREQ(descstr.c_str(), "log cc value");

    std::string path;
    std::string paths;
    int num = 0;
    geco_watcher_base_t::get_root_watcher().walk_all_files(path, paths, num, true);

    EXPECT_TRUE(geco_watcher_base_t::get_root_watcher().remove_watcher(reqpath.c_str()));
}
TEST(GECO_DEBUGGING_WATCHER, test_watcher_path_request_v1)
{
    //this is done in remote endpoint
    int hp = 100;
    const char* path = "logger/cppThresholds/ENGINE-UNITTEST-LOGGER/cc";
    GECO_WATCH(path, hp, WT_READ_WRITE, "log cc value");

    // this is done in another endpoint to query
    watcher_path_request_v1 req1(path, true);
    req1.set_request_complete_cb(req1_cb);  // do network things in this cb
    req1.set_setopt_string("25000");
    req1.set_watcher_value();    // set value that will be sent to remote endpoint and update the value

    watcher_path_request_v1 req1_2("");
    std::string tmp;
    std::string paths;
    int num = 0;
    geco_watcher_base_t::get_root_watcher().walk_all_files(tmp, paths, num, true);
    geco_watcher_base_t::get_root_watcher().visit_children(0, 0, req1_2);
}

static void req2_cb(watcher_path_request & pathRequest, int32 count)
{
    return;
    printf("---------------\n");  // mode, size/value, path. comment
    // this will be called when on comple path is walked
    geco_bit_stream_t& is = pathRequest.get_result_stream();
    uchar mode;
    uint child_size;
    std::string mypath;
    std::string val;
    std::string comment;
    while (is.get_payloads() > 0)
    {
        is.ReadMini(mode);
        if (mode == WT_DIRECTORY)
        {
            is.ReadMini(child_size);
            is.Read(mypath);
            is.Read(comment);
            TRACE_MSG("req2_cb::read [%d,%d,%s,%s]\n", WT_DIRECTORY,child_size, mypath.c_str(),comment.c_str());
        }
        else  // watcher
        {
            is.Read(val);
            is.Read(mypath);
            is.Read(comment);
            TRACE_MSG("req2_cb::read [ %d,%s,%s,%s]\n", mode, val.c_str(),mypath.c_str(),comment.c_str());
        }
    }
    printf("---------------\n");
}
TEST(GECO_DEBUGGING_WATCHER, test_watcher_path_request_v2)
{
    /*
     *  request stream is comprised of
     *  [watcher type uchar] - (string/bool/int ...for data watcher, WVT_DIRECTORY for dir watcher)
     *  [dir size uint] only dir watcher has this
     *  [watcher mode uchar] - (read only/read write for data watcher, none for dir watcher)
     *  [watcher value variable] - none for dir watcher
     *  [watcher path string] - all have this
     *  [comment existed bool]
     *  [watcher comment string] none
     */

    //this is done in remote endpoint
    int hp = 100;
    const char* path = "logger/cppThresholds/test_watcher_path_request_v2";
    GECO_WATCH(path, hp, WT_READ_WRITE, "log cc value");

    const char* path1 = "WVT_INTEGER/WT_READ_ONLY/GECO_WATCH_VALUE";
    GECO_WATCH(path1, value, WT_READ_ONLY, "GECO_WATCH_VALUE->WVT_INTEGER->WT_READ_ONLY");

    ExampleClass example;
    int a = 1;
    example.setValue(a);
    const char* path2 = "WVT_INTEGER/WT_READ_WRITE/GECO_WATCH_METHOD";
    GECO_WATCH(path2, example, CAST_METHOD_RW(int, ExampleClass, getValue, setValue),
            "GECO_WATCH_METHOD->WVT_INTEGER->GECO_WATCH_METHOD");

    const char* path3 = "WVT_STRING/WT_READ_WRITE/GECO_WATCH_FUNC";
    GECO_WATCH(path3, CAST_FUNC_RW(std::string, get_val, set_val), "GECO_WATCH_FUNC->WVT_STRING->CAST_FUNC_RW");
    printf("============================================\n");

    std::string tmp;
    std::string paths;
    int num = 0;
    geco_watcher_base_t::get_root_watcher().walk_all_files(tmp, paths, num, true);

    printf("============================================\n");

    // this is done in another endpoint to query
    watcher_path_request_v2 req2(path);
    req2.set_request_complete_cb(req2_cb);
    geco_watcher_base_t::get_root_watcher().visit_children(0, "logger/cppThresholds", req2);

    printf("============================================\n");
    // now all paths are stored in is
    printf("---------------\n");  // mode, size/value, path. comment
    // this will be called when on comple path is walked
    geco_bit_stream_t& is = req2.get_result_stream();
    uchar valtype;
    uchar mode;
    std::string val;
    uint child_size;
    std::string mypath;
    std::string comment;
    while (is.get_payloads() > 0)
    {
        is.ReadMini(valtype);
        if (valtype == WT_DIRECTORY)
        {
            is.ReadMini(child_size);
            is.Read(mypath);
            is.Read(comment);
            TRACE_MSG("req2_cb::read[type %d,child size %d,path %s,comment %s]\n", WT_DIRECTORY,child_size, mypath.c_str(),comment.c_str());
        }
        else  // watcher
        {
            is.ReadMini(mode);
            switch(valtype)
            {
                case WatcherValueType::WVT_INT32:
                int int32v;
                is.ReadMini(int32v);
                val = watcher_value_to_string(int32v);
                break;
                case WatcherValueType::WVT_UINT32:
                uint uint32v;
                is.ReadMini(uint32v);
                val = watcher_value_to_string(uint32v);
                break;
                case WatcherValueType::WVT_STRING:
                uint bytes2Write;
                is.ReadMini(bytes2Write);
                val.resize(bytes2Write);
                is.ReadAlignedBytes((uchar*) val.data(), bytes2Write);
                break;
            }
            is.Read(mypath);
            is.Read(comment);
            TRACE_MSG("req2_cb::read[type %d,mode %d,val %s,path %s,comment %s]\n", valtype, mode, val.c_str(),mypath.c_str(),comment.c_str());
        }
    }
    printf("---------------\n");

}
