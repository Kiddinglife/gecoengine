#include "FvCommandTest.h"

bool FvCommandTest::RunTest(int iArgc, char** ppcArgv)
{
	FvCommand kCommand(iArgc, ppcArgv);
	bool bC;
	kCommand.Boolean("c",bC);
	float fA;
	kCommand.Float("a",fA);
	char acT[128];
	kCommand.String("t",acT,128);
	return true;
}
