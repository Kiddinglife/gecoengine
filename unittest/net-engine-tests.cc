#include "gtest/gtest.h"
#include "net-engine/geco-bit-stream.h"
#include "net-engine/geco-net-type.h"
#include "net-engine/geco-basic-type.h"
#include "net-engine/geco-msg-ids.h"
#include "net-engine/geco-secure-hand-shake.h"
#include "net-engine/network_socket_t.h"

using namespace geco::net;
/*
 * geco-application.cc
 *
 *  Created on: 12 Apr 2016
 *      Author: jakez
 */

static const unsigned char OFFLINE_MESSAGE_DATA_ID[16] =
{ 0x00, 0xFF, 0xFF, 0x00, 0xFE, 0xFE, 0xFE, 0xFE, 0xFD, 0xFD, 0xFD, 0xFD, 0x12,
0x34, 0x56, 0x78 };
TEST(IsDomainIPAddrTest, when_given_numberic_addr_return_false)
{
    const char* host_Num = "192.168.1.168";
    EXPECT_FALSE(isDomainIPAddr(host_Num));
}

TEST(IsDomainIPAddrTest, when_given_domain_addr_return_true)
{
    const char* host_domain = "www.baidu.com";
    EXPECT_TRUE(isDomainIPAddr(host_domain));
}

TEST(ItoaTest, when_given_positive_and_nagative_integers_return_correct_string)
{
    char result[8];

    Itoa(12, result, 10);
    EXPECT_STREQ("12", result);

    Itoa(-12, result, 10);
    EXPECT_STREQ("-12", result);

    Itoa(-12.5, result, 10);
    EXPECT_STREQ("-12", result);
}

TEST(DomainNameToIPTest, when_given_localhost_string_return_127001)
{
    char result[65] = { 0 };
    DomainNameToIP("localhost", result);
    EXPECT_STREQ("127.0.0.1", result);
    printf("localhost ip addr ('%s')\n", result);
}

TEST(DomainNameToIPTest, when_given_hostname_return_bound_ip_for_that_nic)
{
    char result[65] = { 0 };
    DomainNameToIP("DESKTOP-E2KL25B", result);
    //EXPECT_STREQ("192.168.56.1", result);
    printf("hostname ip addr ('%s')\n", result);
}

TEST(DomainNameToIPTest, when_given_numberic_addr_return_same_ip_addr)
{
    char result[65] = { 0 };
    DomainNameToIP("192.168.2.5", result);
    EXPECT_STREQ("192.168.2.5", result);
}

TEST(DomainNameToIPTest, when_given_external_domain_return_correct_ip_addr)
{
    char result[65] = { 0 };
    DomainNameToIP("www.baidu.com", result);
    printf("baidu ip addr ('%s')\n", result);
}

TEST(NetworkAddressTests, test_NetworkAddress_size_equals_7)
{
    EXPECT_EQ(7, network_address_t::size());
}

TEST(NetworkAddressTests, TestToHashCode)
{
    network_address_t addr3("localhost", 32000);
    printf("hash code for addr '%s' is %ld\n'", addr3.ToString(), network_address_t::ToHashCode(addr3));

    network_address_t addr4("192.168.56.1", 32000);
    printf("hash code for addr '%s' is %ld\n'", addr4.ToString(), network_address_t::ToHashCode(addr4));
}

/// usually seprate the ip addr and port number and you will ne fine
TEST(NetworkAddressTests, TestCtorToStringFromString)
{
    network_address_t default_ctor_addr;
    const char* str1 = default_ctor_addr.ToString();

    network_address_t param_ctor_addr_localhost("localhost", 12345);
    const char* str2 = param_ctor_addr_localhost.ToString();
    EXPECT_STREQ("127.0.0.1|12345", str2);

    // THIS IS WRONG, so when you use domain name, you have to seprate two-params ctor
    //JACKIE_INET_Address param_ctor_addr_domain("ZMD-SERVER:1234");

    // If you have multiple ip address bound on hostname, this will return the first one,
    // so sometimes, it will not be the one you want to use, so better way is to assign the ip address
    // manually.
    network_address_t param_ctor_addr_domain("DESKTOP-E2KL25B", 1234);
    const char* str3 = param_ctor_addr_domain.ToString();
    //EXPECT_STREQ("192.168.56.1|1234", str3);
}

static void test_superfastfunction_func()
{
    std::cout << "\nGlobalFunctions_h::test_superfastfunction_func() starts...\n";
    char* name = "jackie";
    std::cout << "name hash code = " << (name, strlen(name) + 1, strlen(name) + 1);
}


TEST(NetworkAddressTests, SetToLoopBack_when_given_ip4_return_ip4_loopback)
{
    network_address_t addr("192.168.1.108", 12345);
    addr.SetToLoopBack(4);
    EXPECT_STREQ("127.0.0.1|12345", addr.ToString());
}

// this function will not work if you do not define NET_SUPPORT_IP6 MACRO
TEST(NetworkAddressTests, SetToLoopBack_when_given_ip6_return_ip6_loopback)
{
    network_address_t addr("192.168.1.108", 12345);
    addr.SetToLoopBack(6);
    EXPECT_STREQ("192.168.1.108|12345", addr.ToString());
}
TEST(NetworkAddressTests, IsLANAddress_when_given_non_localhost_return_false)
{
    network_address_t addr("192.168.1.108", 12345);
    EXPECT_TRUE(addr.IsLANAddress()) << " 192.168.1.108";
}
TEST(JackieGUIDTests, ToUInt32_)
{
    guid_t gui(12);
    EXPECT_STREQ("12", gui.ToString());
    EXPECT_EQ(12, guid_t::ToUInt32(gui));
}
TEST(JackieGUIDTests, TestToString)
{
    EXPECT_STREQ("JACKIE_INet_GUID_Null", JACKIE_NULL_GUID.ToString());
    guid_t gui(12);
    EXPECT_STREQ("12", gui.ToString());
}
TEST(JackieGUIDTests, TestToHashCode)
{
    guid_t gui(12);
    guid_address_wrapper_t wrapper1(gui);
    EXPECT_STREQ("12", wrapper1.ToString());
    EXPECT_EQ(12, guid_address_wrapper_t::ToHashCode(wrapper1));

    network_address_t adrr("localhost", 12345);
    guid_address_wrapper_t wrapper2(adrr);
    EXPECT_STREQ("127.0.0.1|12345", wrapper2.ToString());
    EXPECT_EQ(network_address_t::ToHashCode(adrr), guid_address_wrapper_t::ToHashCode(wrapper2));;
}
TEST(NetTimeTests, test_gettimeofday)
{
    time_t rawtime;
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    rawtime = tv.tv_sec;

    char buffer[128];
    struct tm * timeinfo;
    timeinfo = localtime(&rawtime);
    strftime(buffer, 128, "%x %X", timeinfo);

    char buff[32];
    sprintf(buff, ":%i", tv.tv_usec / 1000);
    strcat(buffer, buff);

    printf("JackieGettimeofday(%s)\n", buffer);

    printf("GetTime(%i)\n", GetTimeMS());
    GecoSleep(1000);
    printf("GetTime(%i)\n", GetTimeMS());
}
TEST(JISBerkleyTests, test_GetMyIPBerkley)
{
    network_address_t addr[MAX_COUNT_LOCAL_IP_ADDR];
    berkley_socket_t::GetMyIPBerkley(addr);
    for (int i = 0; i < MAX_COUNT_LOCAL_IP_ADDR; i++)
    {
        printf("(%s)\n", addr[i].ToString());
    }
}


struct vec { float x; float y; float z; };
TEST(GecoMemoryStreamTestCase, test_all_reads_and_writes)
{
    geco_bit_stream_t s8;

    uint24_t uint24 = 24;
    uchar uint8 = 8;
    ushort uint16 = 16;
    uint uint32 = 32;
    ulonglong uint64 = 64;
    char int8 = -8;
    short int16 = -16;
    int int32 = -32;
    longlong int64 = -64;

    uchar particialByte = 0xf0; /// 11110000
    guid_t guid(123);
    //NetworkAddress addr("192.168.1.107", 32000);
    vec vector_ = { 0.2f, -0.4f, -0.8f };
    vec vector__ = { 2.234f, -4.78f, -32.2f };

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

            s8.WriteMini(guid);

            s8.WriteMini(uint24);

            //s8.WriteMini(addr);

            s8.WriteMini(uint24);

            s8.Write(uint8);
            s8.Write(int64);
            s8.WriteMini(uint8);
            s8.WriteMini(int64, false);

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

            s8.Write(uint64);
            s8.Write(int8);
            s8.WriteMini(uint64);
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

            guid_t guidd;
            s9.ReadMini(guidd);
            EXPECT_TRUE(guid == guidd);

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
            s9.Read(int64);
            uchar mini_uint8;
            s9.ReadMini(mini_uint8);
            EXPECT_TRUE(mini_uint8 == uint8);

            longlong mini_int64;
            s9.ReadMini(mini_int64, false);
            EXPECT_TRUE(mini_int64 == int64);

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

            s9.Read(uint64);
            s9.Read(int8);
            ulonglong mini_uint64;
            s9.ReadMini(mini_uint64);
            EXPECT_TRUE(mini_uint64 == uint64);

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
            EXPECT_TRUE(uint64 == 64);
            EXPECT_TRUE(int64 == -64);
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
    ulonglong uint64 = 64;
    char int8 = -8;
    short int16 = -16;
    int int32 = -32;
    longlong int64 = -64;

    uchar particialByte = 0xf0; /// 11110000
    guid_t guid(123);
    //NetworkAddress addr("192.168.1.107", 32000);
    vec vector_ = { 0.2f, -0.4f, -0.8f };
    vec vector__ = { 2.234f, -4.78f, -32.2f };

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

            s8.Write(guid);

            s8.Write(uint24);

            //s8.Write(addr);

            s8.Write(uint24);

            s8.Write(uint8);
            s8.Write(int64);
            s8.Write(uint8);
            s8.Write(int64);

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

            s8.Write(uint64);
            s8.Write(int8);
            s8.Write(uint64);
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

            guid_t guidd;
            s9.Read(guidd);
            EXPECT_TRUE(guid == guidd);

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
            s9.Read(int64);
            uchar mini_uint8;
            s9.Read(mini_uint8);
            EXPECT_TRUE(mini_uint8 == uint8);

            longlong mini_int64;
            s9.Read(mini_int64);
            EXPECT_TRUE(mini_int64 == int64);

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

            s9.Read(uint64);
            s9.Read(int8);
            ulonglong mini_uint64;
            s9.Read(mini_uint64);
            EXPECT_TRUE(mini_uint64 == uint64);

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
            EXPECT_TRUE(uint64 == 64);
            EXPECT_TRUE(int64 == -64);
        }

        s8.reset();
        s9.reset();
    }
}
