#include "FvSmartPointerTest.h"

bool FvSmartPointerTest::RunTest()
{
	FvSmartPointer<PointerObject> psPtr1 = new PointerObject;
	FvSmartPointer<PointerObject> psPtr2 = psPtr1;

	return true;
}