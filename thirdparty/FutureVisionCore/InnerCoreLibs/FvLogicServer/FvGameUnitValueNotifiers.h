//{future header message}
#ifndef __FvGameUnitValueNotifiers_H__
#define __FvGameUnitValueNotifiers_H__


#include <FvValueNotifier_Ex.h>
#include <FvStaticArray.h>
#include "FvGameUnitInfo.h"


class FvGameUnitAppearance;


template<FvUInt32 CNT, class TListener, void (TListener::*funcOK)(), void (TListener::*funcErr)()>
class FvGameUnitValueNotifiers
{
public:
	FvGameUnitValueNotifiers(TListener& kListener);
	~FvGameUnitValueNotifiers(){Detach();}

	void Detach();
	bool IsMatch()const{return (m_eState == FvValueNotifierInterface::MATCH);}

	void Init(const FvStaticArray<FvIdxValueRange, CNT>& kCondition, FvGameUnitAppearance& kObj);

private:

	bool _IsMatch()const;
	_VALUE_NOTIFIER_FRIEND_CLASS_;
	void _OnMatch();
	void _OnUnMatch();

	TListener& m_kListener;
	FvValueNotifierInterface::State m_eState;

	FvTValueNotifier<FvValueRangeNotifier, FvGameUnitValueNotifiers, &_OnMatch, &_OnUnMatch> m_kNotifiers[CNT];
};


template<FvUInt32 CNT, class TListener, void (TListener::*funcOK)(), void (TListener::*funcErr)()>
FvGameUnitValueNotifiers<CNT, TListener, funcOK, funcErr>::FvGameUnitValueNotifiers(TListener& kListener)
:m_kListener(kListener)
,m_eState(FvValueNotifierInterface::NO_MATCH)
{
	for (FvUInt32 uiIdx = 0 ; uiIdx < CNT; ++uiIdx)
	{
		m_kNotifiers[uiIdx].SetListener(*this);
	}
}
template<FvUInt32 CNT, class TListener, void (TListener::*funcOK)(), void (TListener::*funcErr)()>
void FvGameUnitValueNotifiers<CNT, TListener, funcOK, funcErr>::Detach()
{
	for (FvUInt32 uiIdx = 0 ; uiIdx < CNT; ++uiIdx)
	{
		m_kNotifiers[uiIdx].Detach();
	}
}
template<FvUInt32 CNT, class TListener, void (TListener::*funcOK)(), void (TListener::*funcErr)()>
bool FvGameUnitValueNotifiers<CNT, TListener, funcOK, funcErr>::_IsMatch()const
{
	for (FvUInt32 uiIdx = 0 ; uiIdx < CNT; ++uiIdx)
	{
		if(m_kNotifiers[uiIdx].GetState() == FvValueNotifierInterface::NO_MATCH)
		{
			return false;
		}
	}
	return true;
}

template<FvUInt32 CNT, class TListener, void (TListener::*funcOK)(), void (TListener::*funcErr)()>
void FvGameUnitValueNotifiers<CNT, TListener, funcOK, funcErr>::Init(const FvStaticArray<FvIdxValueRange, CNT>& kCondition, FvGameUnitAppearance& kObj)
{
	for (FvUInt32 uiIdx = 0 ; uiIdx < CNT; ++uiIdx)
	{
		const FvIdxValueRange& kRange = kCondition.m_kDatas[uiIdx];
		FvValueRangeNotifier& kNotifier = m_kNotifiers[uiIdx];
		if(kRange.m_iType)
		{
			kNotifier.Init(kObj.GetValue32(kRange.m_iType), kRange.m_iValueInf, kRange.m_iValueSup);
			kObj.AttachValue32Listen(kRange.m_iType, kNotifier);
		}
	}
	if(_IsMatch())
	{
		m_eState = FvValueNotifierInterface::MATCH;
	}
	else
	{
		m_eState = FvValueNotifierInterface::NO_MATCH;
	}
}

template<FvUInt32 CNT, class TListener, void (TListener::*funcOK)(), void (TListener::*funcErr)()>
void FvGameUnitValueNotifiers<CNT, TListener, funcOK, funcErr>::_OnMatch()
{
	if(_IsMatch())
	{
		FV_ASSERT(m_eState == FvValueNotifierInterface::NO_MATCH);
		m_eState = FvValueNotifierInterface::MATCH;
		(m_kListener.*funcOK)();
	}
}

template<FvUInt32 CNT, class TListener, void (TListener::*funcOK)(), void (TListener::*funcErr)()>
void FvGameUnitValueNotifiers<CNT, TListener, funcOK, funcErr>::_OnUnMatch()
{
	if(m_eState == FvValueNotifierInterface::MATCH)
	{
		m_eState = FvValueNotifierInterface::NO_MATCH;
		(m_kListener.*funcErr)();
	}
}

//+-------------------------------------------------------------------------------------------------------------------------------------------------------------

#define _GAME_UNIT_VALUE_NOTIFIER_FRIEND_CLASS_ \
	template<FvUInt32 CNT, class TListener, void (TListener::*funcOK)(), void (TListener::*funcErr)()> friend class FvGameUnitValueNotifiers;\
	_VALUE_NOTIFIER_FRIEND_CLASS_





#endif //__FvGameUnitValueNotifiers_H__