//{future header message}
#ifndef __FvAreaRange_H__
#define __FvAreaRange_H__

#include <FvPowerDefines.h>
#include <FvKernel.h>


struct FvAreaRange
{
	enum GameAreaType
	{
		AREA_ROUND,//! 圆形
		AREA_RECT,//! 矩形
		AREA_SECTOR,//! 扇形
	};
	FvAreaRange():m_uiType(AREA_ROUND), m_uiMinRange(0), m_uiMaxRange(0)
	{}
	FvAreaRange(const GameAreaType uiType, const FvInt32 uiMinRange, const FvInt32 uiMaxRange)
		:m_uiType(uiType), m_uiMinRange(uiMinRange), m_uiMaxRange(uiMaxRange)
	{}
	FvUInt32 m_uiType;								// 范围类型
	FvInt32 m_uiMinRange;									// 最小距离(圆)最小半径:米(矩形)横:米(扇形)弧度
	FvInt32 m_uiMaxRange;								// 最大距离(圆)最大半径:米(矩形)竖:米(扇形)半径

};
#endif //__FvAreaRange_H__
