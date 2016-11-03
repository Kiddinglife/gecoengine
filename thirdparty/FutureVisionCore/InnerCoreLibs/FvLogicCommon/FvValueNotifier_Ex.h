#pragma once

#include "FvValueNotifier.h"


//+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
///<»ù±¾>
template<class TValueNotifier, class TListener, void (TListener::*onMatch)(), void (TListener::*onNoMatch)() >
class FvTValueNotifier: public TValueNotifier
{
public:
	FvTValueNotifier():m_rpListener(NULL){}
	void SetListener(TListener& kListener){m_rpListener = &kListener;}

private:
	virtual void _OnMatch();
	virtual void _OnNoMatch();

	TListener* m_rpListener;
};

//+---------------------------------------------------------------------------------------------------------------------------------------------

template<class TValue>
class FvValueCallbackInterface: private FvRefNode1<FvValueCallbackInterface<TValue>*>
{
public:
	typedef typename FvRefListNameSpace<FvValueCallbackInterface>::List1 List;
	static void OnValueUpdated(const TValue& oldValue, const TValue& newValue, List& kList);

	FvValueCallbackInterface(){}
	virtual void OnValueUpdated(const TValue& oldValue, const TValue& newValue) = 0;
	void AttachTo(List& kList){kList.AttachBack(*this);}
	void Detach(){FvRefNode1::Detach();}
	bool IsAttach()const{return FvRefNode1::IsAttach();}
protected:
	~FvValueCallbackInterface(){}

};


template <class TListener, class TValue, void (TListener::*func)(const TValue&, const TValue&)>
class FvValueCallback: public FvValueCallbackInterface<TValue>
{
public:
	typedef FvValueCallbackInterface<TValue> Base;
	FvValueCallback(TListener& kListener):m_kListener(kListener)	{}
	virtual void OnValueUpdated(const TValue& oldValue, const TValue& newValue){(m_kListener.*func)(oldValue, newValue);}
private:
	TListener& m_kListener;
};

#define _VALUE_NOTIFIER_FRIEND_CLASS_\
	template<class TValueNotifier, class TListener, void (TListener::*onMatch)(), void (TListener::*onNoMatch)()> friend class FvTValueNotifier;\
	template<class TListener, class TValue, void (TListener::*func)(const TValue&, const TValue&)> friend class FvValueCallback;

#include "FvValueNotifier_Ex.inl"

//+----------------------------------------------------------------------------------------------------
///<ÊµÀý>
namespace ValueNotifier_Demo
{
	class TestListener
	{
	public:
		void OnMatch()
		{
		}
		void OnNoMatch()
		{
		}
	};

	static void Test()
	{
		TestListener kListener;
		FvTValueNotifier<FvValueRangeNotifier, TestListener, &TestListener::OnMatch, &TestListener::OnNoMatch> kValueRangeNotifier;
		FvValueRangeNotifier::List kValueRangeNotifierList;
		FvInt32 rangeValue = 100;
		kValueRangeNotifier.AttachTo(kValueRangeNotifierList);
		kValueRangeNotifier.SetListener(kListener);
		kValueRangeNotifier.Init(rangeValue, 100, 200);
		FV_ASSERT(kValueRangeNotifier.GetState() == FvValueNotifierInterface::MATCH);

		rangeValue = 150;
		kValueRangeNotifierList.BeginIterator();
		while (!kValueRangeNotifierList.IsEnd())
		{
			FvValueRangeNotifier::List::iterator iter = kValueRangeNotifierList.GetIterator();
			FvValueRangeNotifier* notifier = static_cast<FvValueRangeNotifier*>(iter);
			kValueRangeNotifierList.Next();
			notifier->OnModified(0, rangeValue);
		}
		FV_ASSERT(kValueRangeNotifier.GetState() == FvValueNotifierInterface::MATCH);

		rangeValue = 201;
		kValueRangeNotifierList.BeginIterator();
		while (!kValueRangeNotifierList.IsEnd())
		{
			FvValueRangeNotifier::List::iterator iter = kValueRangeNotifierList.GetIterator();
			FvValueRangeNotifier* notifier = static_cast<FvValueRangeNotifier*>(iter);
			kValueRangeNotifierList.Next();
			notifier->OnModified(0, rangeValue);
		}
		FV_ASSERT(kValueRangeNotifier.GetState() == FvValueNotifierInterface::NO_MATCH);

		rangeValue = 200;
		kValueRangeNotifierList.BeginIterator();
		while (!kValueRangeNotifierList.IsEnd())
		{
			FvValueRangeNotifier::List::iterator iter = kValueRangeNotifierList.GetIterator();
			FvValueRangeNotifier* notifier = static_cast<FvValueRangeNotifier*>(iter);
			kValueRangeNotifierList.Next();
			notifier->OnModified(0, rangeValue);
		}
		FV_ASSERT(kValueRangeNotifier.GetState() == FvValueNotifierInterface::MATCH);

		rangeValue = 199;
		kValueRangeNotifierList.BeginIterator();
		while (!kValueRangeNotifierList.IsEnd())
		{
			FvValueRangeNotifier::List::iterator iter = kValueRangeNotifierList.GetIterator();
			FvValueRangeNotifier* notifier = static_cast<FvValueRangeNotifier*>(iter);
			kValueRangeNotifierList.Next();
			notifier->OnModified(0, rangeValue);
		}
		FV_ASSERT(kValueRangeNotifier.GetState() == FvValueNotifierInterface::MATCH);

	}
}

