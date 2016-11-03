


template<class TValueNotifier, class TListener, void (TListener::*onMatch)(), void (TListener::*onNoMatch)() >
void FvTValueNotifier<TValueNotifier, TListener, onMatch, onNoMatch>::_OnMatch()
{
	FV_ASSERT(m_rpListener);
	if(m_rpListener)
	{
		((*m_rpListener).*onMatch)();
	}
}
template<class TValueNotifier, class TListener, void (TListener::*onMatch)(), void (TListener::*onNoMatch)() >
void FvTValueNotifier<TValueNotifier, TListener, onMatch, onNoMatch>::_OnNoMatch()
{
	FV_ASSERT(m_rpListener);
	if(m_rpListener)
	{
		((*m_rpListener).*onNoMatch)();
	}
}
//+---------------------------------------------------------------------------------------------------------------------------------------------
template<class TValue>
void FvValueCallbackInterface<TValue>::OnValueUpdated(const TValue& oldValue, const TValue& newValue, List& kList)
{
	kList.EndIterator();
	kList.BeginIterator();
	while (!kList.IsEnd())
	{
		List::iterator iter = kList.GetIterator();
		FvValueCallbackInterface* pkCallback = static_cast<FvValueCallbackInterface*>(iter);
		kList.Next();
		FV_ASSERT(pkCallback);
		pkCallback->OnValueUpdated(oldValue, newValue);
	}
}