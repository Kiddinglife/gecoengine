#pragma once


#include "FvLogicCommonDefine.h"

template<class T>
class ExportClass0
{
public:
	static FvUInt32 S_VALUE;
};

template<class T>
class ExportClass1
{
public:
	static FvUInt8 S_VALUE;
};

class FvLogicCommonExport
{
public:
	FvLogicCommonExport(void);
	~FvLogicCommonExport(void);

	template<class T>
	static void Export_API();
};



