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
#include "protocolstack/net_common.h"
#include "appnet/nettypes.h"
#include "common/ultils/ultils.h"
#include "common/debugging/debug.h"
#include "common/debugging/gecowatchert.h"
using namespace geco::debugging;
using namespace geco::ultils;
using namespace geco::ds;

DECLARE_DEBUG_COMPONENT2("UNIT-TEST", LOG_MSG_VERBOSE);

// this should be make another method in watcher class  to update a watcher path or comment remotely
//if (comment != "None" && comment != "") comment_ = comment.c_str();
//if (!mypath.empty() && mypath != "" && mypath != pathRequest.request_path_)
//{
//    std::string::size_type pos = pathRequest.request_path_.find_last_of('/');
//    ((geco_watcher_director_t&)geco_watcher_base_t::get_root_watcher()).find_child(
//        pathRequest.request_path_.c_str())->label =
//        pathRequest.request_path_.substr(pos);
//}

TEST(GECO_DEBUGGING_WATCHER, test_sequence_watcher)
{
    WatcherMode mode;
    std::string ret;
    std::string desc;
    std::vector<int> vec;
    sequence_watcher_t<std::vector<int>>* seqwt = new sequence_watcher_t<std::vector<int>>(vec);
    seqwt->add_watcher("*", geco_watcher_base_t::get_root_watcher());
    seqwt->get_as_string(0,"*", ret,desc,mode);

}

TEST(GECO_DEBUGGING_WATCHER, transformation_between_watchervalue_and_stringval_result_should_be_same)
{
    std::string valstr;

    for (int i = 0; i < 100000; i++)
    {
        int int_ret;
        valstr = watcher_value_to_string((int)12);
        watcher_string_to_value(valstr.c_str(), int_ret);
        EXPECT_EQ((int)12, int_ret);

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

TEST(GECO_DEBUGGING_WATCHER, transformation_between_watchervalue_and_request_stream__result_should_be_same)
{
    geco_bit_stream_t ret;
    uchar type;
    uchar mode;


    for (int i = 0; i < 3; i++)
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

        int int_out=0;
        read_watcher_value_from_stream(ret,int_out,type, mode);

        uint64 uint64_out;
        read_watcher_value_from_stream(ret, uint64_out, type, mode);

        float float_out;
        read_watcher_value_from_stream(ret, float_out, type, mode);

        double double_out;
        read_watcher_value_from_stream(ret, double_out, type, mode);

        char str_out[1024];
        read_watcher_value_from_stream(ret, str_out, type, mode);
        EXPECT_STREQ(str_in, str_out);

        std::string string_out;
        read_watcher_value_from_stream(ret, string_out, type, mode);
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

static void req2_cb(watcher_value_query_t & pathRequest, int32 retcode)
{
    if (retcode < 0)
    {
        ERROR_MSG("retcode = -1, operation failed !\n");
        return;
    }

    geco_bit_stream_t& is = pathRequest.get_result_stream();
    uchar valtype;
    uchar mode;
    while (is.get_payloads() > 0)
    {
        is.ReadMini(valtype);
        if (valtype == WT_DIRECTORY)
        {
            uint child_size;
            std::string mypath;
            std::string comment;
            is.ReadMini(child_size);
            is.Read(mypath);
            is.Read(comment);
            INFO_MSG("req2_cb [WT_DIRECTORY %d, child size %d, path %s,comment %s]\n", WT_DIRECTORY, child_size, mypath.c_str(), comment.c_str());
        }
        else  // watcher
        {
            is.ReadMini(mode);
            switch (valtype)
            {
            case WatcherValueType::WVT_INT32:
                int int32v;
                is.ReadMini(int32v);
                INFO_MSG("req2_cb [int32 %d,mode %d,val " PRId32 "]\n", valtype, mode, int32v);
                break;
            case WatcherValueType::WVT_UINT32:
                uint uint32v;
                is.ReadMini(uint32v);
                INFO_MSG("req2_cb [uint32 %d,mode %d,val " PRIu32 "]\n", valtype, mode, uint32v);
                break;
            case WatcherValueType::WVT_STRING:
                std::string strval;
                is.Read(strval);
                INFO_MSG("req2_cb [string %d,mode %d,val %s]\n", valtype, mode, strval.c_str());
                break;
            }
        }
    }
}
TEST(GECO_DEBUGGING_WATCHER, test_watcher_path_request_v2)
{
    /*
     *  when a non-dir watcher is written to request stream, the formate is like this:
     *  [watcher_type : WatcherValueType]
     *  [watcher_mode : WatcherMode]
     *  [watcher_value]
     *
     *  when a dir watcher is written to request stream, the formate is like this:
     *  [watcher_type :  single value of WVT_DIRECTORY]
     *  [dir_size : uint]
     *  [path : string]
     *  [comment : string]
     */

     ///////////////////////////////////////////////////////////////////////////////////////////////////////
    uint hp = 12345;
    const char* path = "logger/cppThresholds/test_watcher_path_request_v2";
    GECO_WATCH(path, hp, WT_READ_WRITE, "log cc value");
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    const char* path1 = "WVT_INTEGER/WT_READ_ONLY/GECO_WATCH_VALUE";
    GECO_WATCH(path1, value, WT_READ_ONLY, "GECO_WATCH_VALUE->WVT_INTEGER->WT_READ_ONLY");
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ExampleClass example;
    int a = 1;
    example.setValue(a);
    const char* path2 = "WVT_INTEGER/WT_READ_WRITE/GECO_WATCH_METHOD";
    GECO_WATCH(path2, example, CAST_METHOD_RW(int, ExampleClass, getValue, setValue),
        "this watcher watches the method return values for the player's positions");
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    const char* path3 = "WVT_STRING/WT_READ_WRITE/GECO_WATCH_FUNC";
    GECO_WATCH(path3, CAST_FUNC_RW(std::string, get_val, set_val), "GECO_WATCH_FUNC->WVT_STRING->CAST_FUNC_RW");
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string com;
    std::string myret;
    WatcherMode mode;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    geco_watcher_base_t::get_root_watcher().get_as_string(0, path, myret, com, mode);
    EXPECT_STREQ(myret.c_str(), "12345");
    EXPECT_STREQ(com.c_str(), "log cc value");
    EXPECT_EQ(mode, WatcherMode::WT_READ_WRITE);
    geco_watcher_base_t::get_root_watcher().set_from_string(0, path, "12345");
    geco_watcher_base_t::get_root_watcher().get_as_string(0, path, myret, com, mode);
    EXPECT_STREQ(myret.c_str(), "12345");
    EXPECT_STREQ(com.c_str(), "log cc value");
    EXPECT_EQ(mode, WatcherMode::WT_READ_WRITE);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    geco_watcher_base_t::get_root_watcher().get_as_string(0, path1, myret, com, mode);
    EXPECT_STREQ(myret.c_str(), "11");
    EXPECT_STREQ(com.c_str(), "GECO_WATCH_VALUE->WVT_INTEGER->WT_READ_ONLY");
    EXPECT_EQ(mode, WatcherMode::WT_READ_ONLY);
    // because this watcher is ready only set will not work
    geco_watcher_base_t::get_root_watcher().set_from_string(0, path1, "200");
    geco_watcher_base_t::get_root_watcher().get_as_string(0, path1, myret, com, mode);
    EXPECT_STREQ(myret.c_str(), "11");
    EXPECT_STREQ(com.c_str(), "GECO_WATCH_VALUE->WVT_INTEGER->WT_READ_ONLY");
    EXPECT_EQ(mode, WatcherMode::WT_READ_ONLY);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    geco_watcher_base_t::get_root_watcher().get_as_string(0, path2, myret, com, mode);
    EXPECT_STREQ(myret.c_str(), "1");
    EXPECT_STREQ(com.c_str(), "this watcher watches the method return values for the player's positions");
    EXPECT_EQ(mode, WatcherMode::WT_READ_WRITE);
    geco_watcher_base_t::get_root_watcher().set_from_string(0, path2, "2");
    geco_watcher_base_t::get_root_watcher().get_as_string(0, path2, myret, com, mode);
    EXPECT_STREQ(myret.c_str(), "2");
    EXPECT_STREQ(com.c_str(), "this watcher watches the method return values for the player's positions");
    EXPECT_EQ(mode, WatcherMode::WT_READ_WRITE);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    geco_watcher_base_t::get_root_watcher().get_as_string(0, path3, myret, com, mode);
    EXPECT_STREQ(myret.c_str(), stringval.c_str());
    EXPECT_STREQ(com.c_str(), "GECO_WATCH_FUNC->WVT_STRING->CAST_FUNC_RW");
    EXPECT_EQ(mode, WatcherMode::WT_READ_WRITE);
    geco_watcher_base_t::get_root_watcher().set_from_string(0, path3, "hello");
    geco_watcher_base_t::get_root_watcher().get_as_string(0, path3, myret, com, mode);
    EXPECT_STREQ(myret.c_str(), "hello");
    EXPECT_STREQ(com.c_str(), "GECO_WATCH_FUNC->WVT_STRING->CAST_FUNC_RW");
    EXPECT_EQ(mode, WatcherMode::WT_READ_WRITE);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string tmp;
    std::string paths;
    int num = 0;
    geco_watcher_base_t::get_root_watcher().walk_all_files(tmp, paths, num, false);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    // true means this is a request for dir watcher
    watcher_value_query_t dirreq("logger/cppThresholds", req2_cb, true);
    dirreq.get_watcher_value();
    //false means this request is for a value watcher
    watcher_value_query_t valuereq(path1, req2_cb, false);
    valuereq.get_watcher_value();
    valuereq.set_watcher_value(); 
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
}
