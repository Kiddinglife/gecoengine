#include "FvUnitHitEffect.h"
#include "FvGameUnitLogic.h"

FvUnitHitEffect::FvUnitHitEffect()
:FvHitEffect()
,m_rpTarget(NULL)
{
	m_kTargetAttachNode.m_Content = this;
}

FvUnitHitEffect::~FvUnitHitEffect(void)
{
}
void FvUnitHitEffect::SetTarget(FvGameUnitLogic& qTarget)
{
	qTarget._OnAddHitEffect(m_kTargetAttachNode);	
	m_rpTarget = &qTarget;
}
FvGameUnitLogic& FvUnitHitEffect::Target()const
{
	FV_ASSERT_ERROR(m_rpTarget);
	return *m_rpTarget;
}
void FvUnitHitEffect::_Handle()
{
	//if(IsAttributeMatch(Target().Appearance(), GetInfo()))
	{
		Target().OnHitEffect(*this);
		_OnHandle();
	}
}
void FvUnitHitEffect::_OnEnd()
{
	m_kTargetAttachNode.Detach();
}