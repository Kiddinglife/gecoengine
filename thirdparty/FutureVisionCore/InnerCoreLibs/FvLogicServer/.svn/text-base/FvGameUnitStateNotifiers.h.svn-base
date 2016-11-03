//{future header message}
#ifndef __FvGameUnitStateNotifiers_H__
#define __FvGameUnitStateNotifiers_H__

#include "FvGameUnitInfo.h"
#include "FvLogicServerDefine.h"

#include <FvValueNotifier_Ex.h>
#include <FvRefList.h>

class FvGameUnitAppearance;

class FV_LOGIC_SERVER_API FvGameUnitStateNotifyInterface: public FvValueNotifierInterface, private FvRefNode1<FvGameUnitStateNotifyInterface*>
{
public:

	typedef FvRefListNameSpace<FvGameUnitStateNotifyInterface>::List1 List;
	static void OnValueUpdated(const FvGameUnitAppearance& kObj, List& kList);

	void Init(const FvStateCondition& kCondition, FvGameUnitAppearance& kObj);
	void Detach();

	void OnStateModified(const FvGameUnitAppearance& kObj);
	bool IsMatch()const{return (GetState() == MATCH);}
	bool IsMatch(const FvGameUnitAppearance& kObj)const;

protected:

	FvGameUnitStateNotifyInterface(){}
	~FvGameUnitStateNotifyInterface(){}

	FvStateCondition m_kCondition;
};

//+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class TListener, void (TListener::*onMatch)(), void (TListener::*onNoMatch)()>
class FvGameUnitStateNotifiers: public FvGameUnitStateNotifyInterface
{
public:
	FvGameUnitStateNotifiers(TListener& kListener):m_kListener(kListener){}
	~FvGameUnitStateNotifiers(){Detach();}

private:
	virtual void _OnMatch(){(m_kListener.*onMatch)();}
	virtual void _OnNoMatch(){(m_kListener.*onNoMatch)();}
	TListener& m_kListener;
};

//+---------------------------------------------------------------------------------------------------------------------------------------------
//+---------------------------------------------------------------------------------------------------------------------------------------------

#define _GAME_UNIT_STATE_NOTIFIER_FRIEND_CLASS_ \
	template<class TListener, void (TListener::*onMatch)(), void (TListener::*onNoMatch)()> friend class FvGameUnitStateNotifiers;\
	_VALUE_NOTIFIER_FRIEND_CLASS_



#endif //__FvGameUnitStateNotifiers_H__