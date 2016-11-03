//{future header message}
#ifndef __FvSmartPointerTest_H__
#define __FvSmartPointerTest_H__



#include "FvSmartPointer.h"
#include <iostream>

class PointerObject : public FvReferenceCount
{
public:
	PointerObject()
	{
		std::cout << "PointerObject Construct" << std::endl;
	}

	~PointerObject()
	{
		std::cout << "PointerObject Unconstruct" << std::endl;
	}
};

class FvSmartPointerTest
{
public:

	bool RunTest();
};



#endif // __FvSmartPointerTest_H__ 