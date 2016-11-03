#include "FvTimeStampTest.h"
#include "FvTimeStamp.h"
#include <iostream>

bool FvTimeStampTest::RunTest()
{
	FvUInt64 uiTime = TimeStamp();
	unsigned long uiTimeSec = StampsPerSecond();
	std::cout << uiTime << std::endl;
	std::cout << uiTimeSec << std::endl;

	return true;
}