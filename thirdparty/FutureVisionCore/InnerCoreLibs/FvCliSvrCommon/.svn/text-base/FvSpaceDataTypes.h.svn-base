//{future header message}
#ifndef __FvSpaceDataTypes_H__
#define __FvSpaceDataTypes_H__

#include "FvCliSvrCommonDefines.h"
#include <FvPowerDefines.h>
#include <FvBinaryStream.h>

const FvUInt16 SPACE_DATA_TOD_KEY = 0;

class FV_CLI_SVR_COMMON_API FvTimeOfDay
{
public:
	FvTimeOfDay();

	bool	IsRunning() const { return m_bRunning; }
	bool	Start(double fServerTime);										//! 返回T表示有改变,需要同步
	bool	Stop(double fServerTime);										//! 返回T表示有改变,需要同步

	bool	Set(double fServerTime, double fDayTime, float fTimeScaling);	//! 返回T表示有改变,需要同步
	double	GetDayTime(double fServerTime) const;
	float	GetTimeScaling() const { return m_fTimeScaling; }

protected:
	double	m_fServerTimeStart;
	double	m_fDayTimeStart;
	float	m_fTimeScaling;
	bool	m_bRunning;
};
FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvTimeOfDay)

const FvUInt16 SPACE_DATA_MAPPING_KEY_CLIENT_SERVER = 1;

const FvUInt16 SPACE_DATA_MAPPING_KEY_CLIENT_ONLY = 2;

struct FvSpaceDataMappingData
{
	float m_afMatrix[4][4];
};


const FvUInt16 SPACE_DATA_FIRST_USER_KEY = 256;
const FvUInt16 SPACE_DATA_FINAL_USER_KEY = 32511;

const FvUInt16 SPACE_DATA_FIRST_CELL_ONLY_KEY = 16384;


#endif // __FvSpaceDataTypes_H__
