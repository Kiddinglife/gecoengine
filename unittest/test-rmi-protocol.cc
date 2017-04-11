/*
 * test-rmi-protocol.cc
 *
 *  Created on: 15Nov.,2016
 *      Author: jackiez
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <functional>
#include <hash_map>
#include <unordered_map>

#include "gtest/gtest.h"
#include "common/debugging/debug.h"
#include "common/ultils/geco-engine-auth.h"

#include <math.h>
#ifdef _WIN32
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif

using namespace geco::debugging;
using namespace geco::ultils;
using namespace geco::ds;

typedef void (*decoder_t)(char* is);
static void p1_decoder_int(char* is)
{
	int a = *(int*) is;
	printf("p1_decoder_int()::single param int a = %d\n", a);
}

static void p1_decoder_int_array(char* is)
{
	int size = *(int*) is;
	is += 4;
	printf("p1_decoder_int_array()::single param int arr size = %d\n", size);
	int val;
	for (int i = 0; i < size; i++)
	{
		val = *(int*) (is + i * sizeof(int));
		printf("p1_decoder_int_array()::single param int arr val = %d\n", val);
	}
}

struct arg_type_key
{
		uchar arg0_type;
		uchar arg1_type;
};
struct arg_type_key_hash_functor
{
		size_t operator()(const arg_type_key& addr) const
		{
			uint local_saaf = addr.arg0_type;
			unsigned int lastHash = 100;
			//unsigned int lastHash =
			//SuperFastHashIncremental((const char*) &addr.arg1_type, sizeof(uchar), local_saaf);
			return lastHash;
		}
};
struct arg_type_key_comparaor
{
		bool operator()(const arg_type_key& addr1,
				const arg_type_key &addr2) const
		{
			return addr1.arg0_type == addr2.arg0_type
					&& addr1.arg1_type == addr2.arg1_type;
		}
};

#ifdef _WIN32
std::unordered_map<arg_type_key, void*> channel_map_;
#else
std::unordered_map<arg_type_key, decoder_t, arg_type_key_hash_functor,
		arg_type_key_comparaor> parsemap;
#endif
#define INT_TYPE 1
#define FLOAT_TYPE 2
#define STR_TYPE 3
#define VECT_TYPE 4
const int type_size = 3; // int float str
struct mtd
{
		decoder_t parser;
};
TEST(RMI, test_protocol)
{
	//fill up map when program starts
	arg_type_key key;
	key.arg0_type = INT_TYPE;	// parse int
	parsemap.insert(std::make_pair(key, &p1_decoder_int));

	key.arg0_type = VECT_TYPE;	// parse float
	parsemap.insert(std::make_pair(key, &p1_decoder_int_array));

	// parse entity.def and fill up key
	key.arg0_type = INT_TYPE;	// parse int
	//find the decoder ptr and fill it into entity mtd descpts
	mtd mtdt;
	mtdt.parser = parsemap[key];

	//assume we find the mtd when receive msg from network
	char is[128];
	*(int*) is = 12;
	mtdt.parser(is);

	// parse entity.def and fill up key
	key.arg0_type = VECT_TYPE;	// parse int
	//find the decoder ptr and fill it into entity mtd descpts
	mtdt.parser = parsemap[key];

	*(int*) is = 2;
	*(int*) (is + 4) = 12;
	*(int*) (is + 8) = 13;
	mtdt.parser(is);
}

