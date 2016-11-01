//{future header message}
#ifndef __GxScriptDefine_H__
#define __GxScriptDefine_H__

#include <FvPowerDefines.h>


#ifdef FV_AS_STATIC_LIBS
#define GX_GAME_API
#else
#ifdef GX_GAME_EXPORT
#define GX_GAME_API FV_Proper_Export_Flag
#else
#define GX_GAME_API FV_Proper_Import_Flag
#endif
#endif


class GxEntityType
{
public:
	enum
	{
		TESTENTITY,
		GXUNITENTITY,
		GXAREAAURAENTITY,
		MAX
	};
};

#endif//__GxScriptDefine_H__