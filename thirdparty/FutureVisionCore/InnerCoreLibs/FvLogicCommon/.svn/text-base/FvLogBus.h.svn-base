//{future header message}
#ifndef __FvLogBus_H__
#define __FvLogBus_H__


#include "FvLogicCommonDefine.h"
#include <FvKernel.h>


class FvLogUser
{
public:
	enum Mask
	{
		DEFAULT = 0X01,
		LvG = 0X02,
		XuDong = 0X04,
		DunDao = 0x08,
		Yisa = 0X10,
		ALL = 0XFFFFFFFF,
	};
};

enum LogColor
{
	BLACK,
	RED,
	GREEN,
	BROWN,
	BLUE,
	MAGENTA,
	CYAN,
	GREY,
	YELLOW,
	LRED,
	LGREEN,
	LBLUE,
	LMAGENTA,
	LCYAN,
	WHITE,
	LOG_COLOR_TOTAL,
};

class FvLogType
{
public:
	enum Idx
	{
		OK,
		CRIT_OK,
		WARNING,
		ERR,
		TOTAL,
	};
};

class FV_LOGIC_COMMON_API FvLogBus
{
public:
	static void SetLogLevel(const FvLogType::Idx eLevel);
	static void SetColor(const LogColor uiIdx);
	static void Note(const FvUInt32 eMask, const FvLogType::Idx eType, const char * str, ... );
	static void OK(const FvUInt32 eMask, const char * str, ... );
	static void CritOk(const char * str, ... );
	static void Error(const char * str, ... );
	static void Warning(const char * str, ... );
	static void SetMask(const FvUInt32 eMask);

private:
	FvLogBus();
	static FvUInt32 m_uiOutputMask;
	static FvLogType::Idx m_uiLogLevel;
};

#endif //__FvLogBus_H__
