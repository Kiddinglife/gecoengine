/*
 * test-ultils.cc
 *
 *  Created on: 3Nov.,2016
 *      Author: jackiez
 */

#include "gtest/gtest.h"
#include "common/debugging/debug.h"
#include "common/ultils/geco-cmdline.h"

TEST(GECO_ULTILS, test_geco_cmdline_t)
{
	int iArgc = 6;
	const char* ppcArgv[]  = {"name","-float","1.23","-boolean","-strr","hello"};
	//equvient to geco_cmdline_t kCommand("-float 1.23 -boolean -strr hello");
	// you can use short name like geco_cmdline_t kCommand("-f 1.23 -b -s hello");
	geco_cmdline_t kCommand(iArgc,(char**)ppcArgv);
	bool bC;
	kCommand.Boolean("boolean",bC);
	ASSERT_EQ(bC, true);
	float fA;
	kCommand.Float("float",fA);
	ASSERT_EQ(fA, 1.23f);
	char acT[128];
	kCommand.String("strr",acT,128);
	ASSERT_STREQ(acT, "hello");


}


