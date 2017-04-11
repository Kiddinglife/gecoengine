/*
 * test-geco-bit-stream.cc
 *
 *  Created on: 22Jul.,2016
 *      Author: jackiez
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <functional>

#include "gtest/gtest.h"
#include "common/debugging/debug.h"
#include "common/ds/geco-bit-stream.h"
#include "common/geco-plateform.h"

using namespace geco::debugging;
using namespace geco::ultils;
using namespace geco::ds;

const uint csm_charFreqs[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 329, 21, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2809, 68, 0, 27,
		0, 58, 3, 62, 4, 7, 0, 0, 15, 65, 554, 3, 394, 404, 189, 117, 30, 51,
		27, 15, 34, 32, 80, 1, 142, 3, 142, 39, 0, 144, 125, 44, 122, 275, 70,
		135, 61, 127, 8, 12, 113, 246, 122, 36, 185, 1, 149, 309, 335, 12, 11,
		14, 54, 151, 0, 0, 2, 0, 0, 211, 0, 2090, 344, 736, 993, 2872, 701, 605,
		646, 1552, 328, 305, 1240, 735, 1533, 1713, 562, 3, 1775, 1149, 1469,
		979, 407, 553, 59, 279, 31, 0, 0, 0, 68, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
TEST(GecoMemoryStreamTestCase, test_haffman_compression) {
	HuffmanEncodingTree tree;
	tree.GenerateFromFrequencyTable();
	const char input[] =
			"By default, an endpoint SHOULD always transmit to the primary path,"
					"unless the SCTP user explicitly specifies the destination transport address "
					"(and possibly source transport address) to use.!";
	std::string buf;
	while (buf.size() < 512)
		buf += input;

	geco_bit_stream_t compressed;
	tree.EncodeArray((uchar*) buf.data(), buf.length(), &compressed);

	char output[1024];
	uint end = tree.DecodeArray(&compressed, compressed.get_written_bits(),
			sizeof(output), (uchar*) output);
	output[end] = '\0';
	EXPECT_STREQ(buf.c_str(), (char* )output);

	geco_bit_stream_t ouput2;
	tree.DecodeArray(compressed.uchar_data(), compressed.get_written_bits(),
			&ouput2);
	char endd = '\0';
	ouput2.Write(endd);
	printf("original bytes: %d, comprssed byts %d, compression ratio %.2f%\n",
			buf.length(), compressed.get_written_bytes(),
			(buf.length() - compressed.get_written_bytes())
					/ (float) buf.length() * 100);
}

TEST(GecoMemoryStreamTestCase, test_geco_string_compressor) {
    std::string originstr;
    originstr.resize(1024);
    std::string recvstr;
    recvstr.resize(1024);
    geco_bit_stream_t writer;
    //originstr =
    //    "By default, an endpoint SHOULD always transmit to"
    //    "the primary path,unless the SCTP user explicitly specifies the destination "
    //    "transport address(and possibly source transport address) to use."
    //    "An endpoint SHOULD transmit reply chunks (e.g., SACK, HEARTBEAT ACK";
    originstr = "this watcher watches the method return values for the player positions";

    writer.Write(originstr);
    writer.Read(recvstr);
    EXPECT_STREQ(originstr.c_str(), recvstr.c_str());
        printf("original bytes: %d, comprssed byts %d, compression ratio %.2f\n",
            originstr.length(), writer.get_written_bytes(),
            (originstr.length() - writer.get_written_bytes())
            / (float)originstr.length() * 100);
    writer.reset();

    for (int i = 0; i < 100000; i++)
    {
        writer.Write(originstr);
        writer.Read(recvstr);
        EXPECT_STREQ(originstr.c_str(), recvstr.c_str());
         writer.reset();
    }
}

/**
 * prefer the list below:
 *  delta *-changed()
 *  ranged *-ranged-*()
 *  bit packed *Mini()
 */
TEST(GecoMemoryStreamTestCase, test_float_compression) {
	int fi = geco_bit_stream_t::FloatToBits(0.789);
	float intf = geco_bit_stream_t::BitsToFloat(fi);
	geco_bit_stream_t s8((uchar*) &fi, sizeof(int), false);
	s8.Bitify();
	printf("%d, %.6f\n", fi, intf);
	geco_bit_stream_t s9((uchar*) &intf, sizeof(uchar), false);
	s9.Bitify();
}

DECLARE_DEBUG_COMPONENT2("UNIT-TEST", 0);

struct vec {
	float x;
	float y;
	float z;
};
TEST(GecoMemoryStreamTestCase, test_all_reads_and_writes) {
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
	vec vector_ = { 0.2f, -0.4f, -0.8f };
	vec vector__ = { 2.234f, -4.78f, -32.2f };
	float outFloat = -0.123;
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

	for (int i = 10; i >= 0; i--) {
		uint looptimes = 100000;
		for (uint i = 1; i <= looptimes; i++) {
			s8.WriteMini(uint24);
			s8.write_ranged_float(outFloat, -1.0f, 1.0f);
			//s8.WriteMini(guid);

			s8.WriteMini(uint24);
			//s8.WriteMini(addr);

			s8.WriteMini(uint24);

			s8.WriteMini(uint8);
			s8.WriteMini(int64_, false);
			s8.WriteMini(uint8);
			s8.WriteMini(int64_, false);

			s8.WriteMini(uint16);
			s8.WriteMini(int32, false);
			s8.WriteMini(uint16);
			s8.WriteMini(int32, false);

			s8.WriteBits(&particialByte, 4, true);
			s8.WriteMini(uint24);
			s8.WriteNormVector(vector_.x, vector_.y, vector_.z);

			s8.write_ranged_integer(curr, min, max);

			s8.WriteVector(vector__.x, vector__.y, vector__.z);

			s8.WriteMini(uint32);
			s8.WriteMini(int16, false);
			s8.WriteMini(uint32);
			s8.WriteMini(int16, false);

			s8.WriteBits(&particialByte, 4, false);

			s8.WriteMini(uint64_);
			s8.WriteMini(int8, false);
			s8.WriteMini(uint64_);
			s8.WriteMini(int8, false);

			s8.WriteBits(&particialByte, 7, false);

			if (i == 1) {
				//s8.Bitify();
//                geco_bit_stream_t compress;
//                compress.WriteMini(s8);
//                compress.Bitify();
			}
		}

		geco_bit_stream_t s9;
		s9.Write(s8);
		if (i == 0) {
			printf("uncompressed  '%.5f' MB\n",
					float(BITS_TO_BYTES(s9.get_payloads()) / 1024 / 1024));
		}

		for (uint i = 1; i <= looptimes; i++) {
			uint24 = 0;
			s9.ReadMini(uint24);
			EXPECT_TRUE(uint24.val == 24);
			float muf;
			s9.read_ranged_float(muf, -1.0f, 1.0f);
			EXPECT_TRUE(fabs(muf - outFloat) <= 0.0001f);

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

			s9.ReadMini(uint8);
			s9.ReadMini(int64_, false);
			uchar mini_uint8;
			s9.ReadMini(mini_uint8);
			EXPECT_TRUE(mini_uint8 == uint8);

			int64 mini_int64;
			s9.ReadMini(mini_int64, false);
			EXPECT_TRUE(mini_int64 == int64_);

			s9.ReadMini(uint16);
			s9.ReadMini(int32, false);
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
			s9.ReadMini(uint24);
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

			s9.ReadMini(uint32);
			s9.ReadMini(int16, false);
			uint mini_uint32;
			s9.ReadMini(mini_uint32);
			EXPECT_TRUE(mini_uint32 == uint32);

			short mini_int16;
			s9.ReadMini(mini_int16, false);
			EXPECT_TRUE(mini_int16 == int16);

			v = 0;
			s9.ReadBits(&v, 4, false);
			EXPECT_TRUE(particialByte == ((v >> 4) << 4));

			s9.ReadMini(uint64_);
			s9.ReadMini(int8, false);
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
TEST(GecoMemoryStreamTestCase, test_all_reads_and_writes_un_compressed) {
	geco_bit_stream_t s8;
	float ufloat = 0.123;
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

	for (int i = 10; i >= 0; i--) {
		uint looptimes = 100000;
		for (uint i = 1; i <= looptimes; i++) {
			s8.Write(uint24);
			s8.Write(ufloat);
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
			s8.WriteNormVector(vector_.x, vector_.y, vector_.z);

			s8.write_ranged_integer(curr, min, max);

			s8.WriteVector(vector__.x, vector__.y, vector__.z);

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

		if (i == 0) {
			printf("uncompressed  '%.5f' MB\n",
					float(BITS_TO_BYTES(s9.get_payloads()) / 1024 / 1024));
		}

		for (uint i = 1; i <= looptimes; i++) {
			uint24 = 0;
			s9.Read(uint24);
			EXPECT_TRUE(uint24.val == 24);

			float mf;
			s9.Read(mf);
			EXPECT_TRUE(fabs(mf - ufloat) < 0.0001f);

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

#include "network/msg-parser.h"
TEST(GecoMemoryStreamTestCase, test_msg_call)
{
	msg_call();
}
