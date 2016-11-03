//{future header message}
#ifndef __FvTargetSelector_H__
#define __FvTargetSelector_H__


#include "FvLogicServerDefine.h"
#include "FvTarget.h"

#include <FvAreaStruct.h>
#include <FvDirection3.h>
#include <FvKernel.h>
#include <FvArea.h>


class FvGameUnitAppearance;
class FvGameUnitLogic;

class FV_LOGIC_SERVER_API FvAreaTargetSelector
{
public:
	~FvAreaTargetSelector();
	typedef FvTargetList<10000> TargetList;

	const TargetList& GetTargets()const{return m_kTargetList;}
	void Select(const FvGameUnitLogic& kFocus, const FvDirection3& kDir, const FvVector3& kCenter, const FvUInt32 uiMask, const FvAreaRange& kRange);

	template<FvUInt32 CNT>
	void Select(const FvGameUnitLogic& kFocus, const FvDirection3& kDir, const FvVector3& kCenter, const FvUInt32 uiMask, const FvAreaRange& kRange, FvTargetList<CNT>& kTargetList);


protected:
	FvAreaTargetSelector();

	virtual void _RoundSelect(const FvArea& kArea, const FvGameUnitLogic& pkFocus, const FvUInt32 uiMask) = 0;
	void _Clear();

	TargetList m_kTargetList;

private:

	FvRoundArea m_kRound;
	FvSectorArea m_kSector;
	FvRotRectArea m_kRotRect;

};

template<FvUInt32 CNT>
void FvAreaTargetSelector::Select(const FvGameUnitLogic& kFocus, const FvDirection3& kDir, const FvVector3& kCenter, const FvUInt32 uiMask, const FvAreaRange& kRange, FvTargetList<CNT>& kTargetList)
{
	m_kTargetList.Clear();
	if(uiMask)
	{
		switch (kRange.m_uiType)
		{
		case FvAreaRange::AREA_ROUND:
			if(kRange.m_uiMaxRange > 0)
			{
				m_kRound.SetCenter(kCenter);
				m_kRound.Init(kRange.m_uiMaxRange*0.01f);
				_RoundSelect(m_kRound, kFocus, uiMask);
			}
			break;
		case FvAreaRange::AREA_RECT:
			if(kRange.m_uiMinRange > 0 && kRange.m_uiMaxRange > 0)
			{
				m_kRotRect.SetCenter(kCenter);
				m_kRotRect.SetDir(FvVector3(sin(kDir.Yaw()), -cos(kDir.Yaw()), 0.0f));
				m_kRotRect.Init(kRange.m_uiMinRange*0.01f, kRange.m_uiMaxRange*0.01f);
				_RoundSelect(m_kRotRect, kFocus, uiMask);
			}
			break;
		case FvAreaRange::AREA_SECTOR:
			if(kRange.m_uiMinRange > 0 && kRange.m_uiMaxRange > 0)
			{
				m_kSector.SetCenter(kCenter);
				m_kSector.SetDir(kDir.Yaw());
				m_kSector.Init(kRange.m_uiMaxRange*0.01f, -kRange.m_uiMinRange*0.01f, kRange.m_uiMinRange*0.01f);
				_RoundSelect(m_kSector, kFocus, uiMask);
			}
			break;
		}
	}
}

#endif //__FvTargetSelector_H__
