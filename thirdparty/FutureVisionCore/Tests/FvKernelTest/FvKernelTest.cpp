#include "FvDebugTest.h"
#include "FvWatcherTest.h"
#include "FvSmartPointerTest.h"
#include "FvTimeStampTest.h"
#include "FvCommandTest.h"
#include "vld.h"

int main(int iArgc, char** ppcArgv)
{
	//FvDebugTest kDebugTest;
	//kDebugTest.RunTest();

	FvWatcherTest kWatcherTest;
	kWatcherTest.RunTest();

	//FvSmartPointerTest kSmartPointerTest;
	//kSmartPointerTest.RunTest();

	//FvTimeStampTest kTimeStampTest;
	//kTimeStampTest.RunTest();
	
	//FvCommandTest kFvCommandTest;
	//kFvCommandTest.RunTest(iArgc, ppcArgv);

}