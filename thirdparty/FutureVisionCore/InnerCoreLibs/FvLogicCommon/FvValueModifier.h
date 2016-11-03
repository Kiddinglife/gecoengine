//{future header message}
#ifndef __FvValueModifier_H__
#define __FvValueModifier_H__

#include <FvRefList.h>

class FvValueModifier: public FvRefNode1<FvValueModifier*> 
{
public:
	FvValueModifier();
	void Init(const FvInt32 iDeltaValue);
	FvInt32 m_iDeltaValue;
};



class FvValueModifierList
{
public:
	typedef FvRefList<FvValueModifier*, FvRefNode1> List;
	FvValueModifierList();
	~FvValueModifierList();

	void Attach(FvValueModifier& mod);
	void Detach(FvValueModifier& mod);

	void SetBaseValue(const FvInt32 iValue);
	FvInt32 GetBaseValue()const;
	FvInt32 GetTotalValue()const;

	void SetMaxValue(const FvInt32 iValue);
	void SetMinValue(const FvInt32 iValue);

private:
	void _Update();

	FvInt32 m_iBaseValue;
	FvInt32 m_iTotalValue;

	FvInt32 m_iMaxValue;
	FvInt32 m_iMinValue;

	List m_Modifiers;
};

namespace Modifier_Demo
{
	static void Test()
	{
		FvValueModifierList modifierList;
		FvValueModifier modifier_1;
		modifier_1.Init(50);
		FvValueModifier modifier_2;
		modifier_2.Init(-30);

		modifierList.SetBaseValue(100);
		modifierList.SetMaxValue(200);
		modifierList.Attach(modifier_1);
		FV_ASSERT(modifierList.GetTotalValue() == 150);
		modifierList.SetMaxValue(140);
		FV_ASSERT(modifierList.GetTotalValue() == 140);
		modifierList.Attach(modifier_2);
		FV_ASSERT(modifierList.GetTotalValue() == 120);
		modifierList.Detach(modifier_1);
		FV_ASSERT(modifierList.GetTotalValue() == 70);
		modifierList.Detach(modifier_2);
		FV_ASSERT(modifierList.GetTotalValue() == 100);

	}
}

#endif// __FvValueModifier_H__
