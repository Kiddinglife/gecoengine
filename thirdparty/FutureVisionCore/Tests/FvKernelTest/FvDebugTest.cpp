//{future source message}
#include "FvDebugTest.h"
#include <iostream>

FV_DECLARE_DEBUG_COMPONENT2("FvDebugTest",0);

bool FvDebugTest::RunTest()
{
	std::cout << "Test functon : MessagePrefix" << std::endl;
	std::cout << MessagePrefix(FV_DEBUG_MESSAGE_PRIORITY_TRACE) << std::endl;
	std::cout << MessagePrefix(FV_DEBUG_MESSAGE_PRIORITY_DEBUG) << std::endl;
	std::cout << MessagePrefix(FV_DEBUG_MESSAGE_PRIORITY_INFO) << std::endl;
	std::cout << MessagePrefix(FV_DEBUG_MESSAGE_PRIORITY_NOTICE) << std::endl;
	std::cout << MessagePrefix(FV_DEBUG_MESSAGE_PRIORITY_WARNING) << std::endl;
	std::cout << MessagePrefix(FV_DEBUG_MESSAGE_PRIORITY_ERROR) << std::endl;
	std::cout << MessagePrefix(FV_DEBUG_MESSAGE_PRIORITY_CRITICAL) << std::endl;
	std::cout << MessagePrefix(FV_DEBUG_MESSAGE_PRIORITY_HACK) << std::endl;
	std::cout << MessagePrefix(FV_DEBUG_MESSAGE_PRIORITY_SCRIPT) << std::endl;

	std::cout << "Test functon : DebugMessageHelper" << std::endl;
	//FvDebugFilter::Instance().AddCriticalCallback(this);
	//FvDebugFilter::Instance().AddMessageCallback(this);
	//DebugMessageHelper("Test functon is %s","DebugMessageHelper \n");
	//DebugMessageHelper("Test functon is %s","DebugMessageHelper \n");

	FV_INFO_MSG("Info Test \n");
	FV_DEBUG_MSG("Debug Test \n");
	FV_NOTICE_MSG("Notice Test \n");
	FV_WARNING_MSG("Warning Test \n");
	FV_ERROR_MSG("Error Test \n");
	FV_CRITICAL_MSG("Critical Test \n");
	FV_HACK_MSG("Hack Test \n");
	FV_SCRIPT_MSG("Script Test \n");

	FV_ASSERT(false);
	return true;
}