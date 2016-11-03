#include "FvGameUnitLogic.h"

#include <FvGameTimer.h>
#include <FvDestroyManager.h>
#include <FvLogBus.h>
#include <FvStaticCaster.h>


FvGameUnitLogic& FvGameUnitLogic::Self(FvUnitSpell& kSpell)
{
	FvGameUnitLogic& kSelf = FvStaticCaster::GetFather<FvGameUnitLogic, FvUnitSpell, &FvGameUnitLogic::m_kSpell>(kSpell);
	return kSelf;
}

const FvGameUnitLogic& FvGameUnitLogic::Self(const FvUnitSpell& kSpell)
{
	const FvGameUnitLogic& kSelf = FvStaticCaster::GetFather<FvGameUnitLogic, FvUnitSpell, &FvGameUnitLogic::m_kSpell>(kSpell);
	return kSelf;
}

FvGameUnitLogic::FvGameUnitLogic(FvGameUnitAppearance& kAppearance)
:m_kAppearance(kAppearance)
{
}

FvGameUnitLogic::~FvGameUnitLogic(void)
{
	FvLogBus::CritOk("FvGameUnitLogic Îö¹¹");
	FV_ASSERT_WARNING(m_HitEffectList.Size() == 0);
}
void FvGameUnitLogic::Save()
{
	_ClearHitEffects();
	m_kValue32Mappings.clear();
}
void FvGameUnitLogic::Clear()
{
	_ClearReference();
}
