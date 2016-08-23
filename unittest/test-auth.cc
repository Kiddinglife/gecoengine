/*
 * test-auth.cc
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
#include "common/ultils/geco-engine-auth.h"

using namespace geco::debugging;
using namespace geco::ultils;
using namespace geco::ds;

DECLARE_DEBUG_COMPONENT2("UNIT-TEST", 0);

TEST(GECO_ULTILS, encode64_when_given_string_and_then_decode_back_to_string_result_should_be_same)
{
	std::string sample("Hellow world");
	std::string out;
	decode_base64(encode_base64(sample), out);
	EXPECT_STRCASEEQ(sample.c_str(), out.c_str());
}

TEST(GECO_ULTILS, md5_when_digest_quote_to_hexstr_and_then_unquote_it_result_should_be_same)
{
	//std::string sample("202cb962ac59075b964b07152d234b70");
	//geco::ultils::MD5 md5;
	//geco::ultils::MD5::Digest digest;
	//md5.append(sample.c_str(), sample.size());
	//md5.getDigest(digest);

	//digest.quote();
	//EXPECT_STRCASEEQ(digest.quote().c_str(), "d9b1d7db4cd6e70935368a1efb10e377");

	//geco::ultils::MD5::Digest digest1;
	//digest1.unquote(digest.quote());
	//EXPECT_TRUE(digest1 == digest);
	//EXPECT_STRCASEEQ(digest1.quote().c_str(), "d9b1d7db4cd6e70935368a1efb10e377");
}

