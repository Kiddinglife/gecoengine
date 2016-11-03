#include "FvValueModifier.h"
#include <FvMath.h>

FvValueModifier::FvValueModifier()
:m_iDeltaValue(0)
{
}
void FvValueModifier::Init(const FvInt32 iDeltaValue)
{
	m_iDeltaValue = iDeltaValue;
}

//+------------------------------------------------------------------------------------------------

FvValueModifierList::FvValueModifierList()
:m_iBaseValue(0)
,m_iTotalValue(0)
,m_iMaxValue(1000000)
,m_iMinValue(-1000000)
{

}
FvValueModifierList::~FvValueModifierList()
{
	FV_ASSERT(m_Modifiers.Size() == 0 && "没有清理");
}

void FvValueModifierList::SetBaseValue(const FvInt32 value)
{
	m_iBaseValue = value;
	_Update();
}
FvInt32 FvValueModifierList::GetBaseValue()const
{
	return m_iBaseValue;
}

FvInt32 FvValueModifierList::GetTotalValue()const
{
	return m_iTotalValue;
}
void FvValueModifierList::_Update()
{
	FvInt32 deltaValueUp = 0;
	FvInt32 deltaValueDown = 0;
	m_Modifiers.EndIterator();
	m_Modifiers.BeginIterator();
	while (!m_Modifiers.IsEnd())
	{
		List::iterator iter = m_Modifiers.GetIterator();
		m_Modifiers.Next();

		FvValueModifier* mod = static_cast<FvValueModifier*>(iter);
		FV_ASSERT(mod);
		if(mod->m_iDeltaValue > 0)
		{
			//! 正向调节
			deltaValueUp += mod->m_iDeltaValue;
		}
		else if(mod->m_iDeltaValue < 0)
		{
			//! 负向调节
			deltaValueDown += mod->m_iDeltaValue;
		}
	}
	m_iTotalValue = m_iBaseValue + deltaValueUp + deltaValueDown;
	m_iTotalValue = FvMathTool::Clamp(m_iTotalValue, m_iMinValue, m_iMaxValue);
}
void FvValueModifierList::Attach(FvValueModifier& mod)
{
	m_Modifiers.AttachBack(mod);
	_Update();
}
void FvValueModifierList::Detach(FvValueModifier& mod)
{
	FV_ASSERT(mod.IsAttach(m_Modifiers));
	mod.Detach();
	_Update();
}
void FvValueModifierList::SetMaxValue(const FvInt32 iValue)
{
	m_iMaxValue = iValue;
	_Update();
}
void FvValueModifierList::SetMinValue(const FvInt32 iValue)
{
	m_iMinValue = iValue;
	_Update();
}