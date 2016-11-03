#include "FvTargetSelector.h"
#include "FvGameUnitAppearance.h"

FvAreaTargetSelector::FvAreaTargetSelector()
{

}

FvAreaTargetSelector::~FvAreaTargetSelector()
{

}

void FvAreaTargetSelector::Select(const FvGameUnitLogic& kFocus, const FvDirection3& kDir, const FvVector3& kCenter, const FvUInt32 uiMask, const FvAreaRange& kRange)
{
	Select(kFocus, kDir, kCenter, uiMask, kRange, m_kTargetList);
}
