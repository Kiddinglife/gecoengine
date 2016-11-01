#include "FvSpellTargetEffect.h"

#include "FvGameUnitAppearance.h"

#include <FvDestroyManager.h>
#include <FvGameTimer.h>

FvSpellTargetEffect::FvSpellTargetEffect(void)
:
#pragma warning(push)
#pragma warning(disable: 4355)
m_kEndTimeNode(*this)
#pragma warning(pop)
{}
FvSpellTargetEffect::~FvSpellTargetEffect(void)
{
}

void FvSpellTargetEffect::End()
{
	m_kEndTimeNode.Detach();
	_OnEnd();
	FvDestroyManager::Destroy(*this);
}
void FvSpellTargetEffect::End(const float fDelayTime)
{
	FvGameTimer::SetTime(fDelayTime, m_kEndTimeNode);
}
void FvSpellTargetEffect::_OnEndTime(FvTimeEventNodeInterface&)
{
	_OnEnd();
	FvDestroyManager::Destroy(*this);
}