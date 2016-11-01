
//+-----------------------------------------------------------------------------------------------------------------
template<class From, class To, void (To::*CallbackFunc)(const FvUInt32)>
void FvAsynCallback0<From, To, CallbackFunc>::Exec(const FvUInt32 uiEvent, From& kFrom)
{
	m_uiEvent = uiEvent;
	S_List().AttachBackSafeIterator(*this);
}
//+-----------------------------------------------------------------------------------------------------------------
template<class From, class To>
void FvDynamicAsynCallback0<From, To>::Exec(const FvUInt32 uiEvent, From& kFrom)
{
	m_uiEvent = uiEvent;
	S_List().AttachBackSafeIterator(*this);
}
//+-----------------------------------------------------------------------------------------------------------------
template<class From>
void FvAsynCallbackList0<From>::Invoke(const FvUInt32 uiEvent, From& kFrom)
{
	for (m_kList.BeginIterator(); !m_kList.IsEnd();)
	{
		Callback* pkCallback = m_kList.GetIterator()->Content();
		FV_ASSERT(pkCallback);
		m_kList.Next();
		pkCallback->Exec(uiEvent, kFrom);
	}
}
//+-----------------------------------------------------------------------------------------------------------------
template<class From, class To, void (To::*CallbackFunc)(const FvUInt32, From&)>
void FvCallback0<From, To, CallbackFunc>::Exec(const FvUInt32 uiEvent, From& kFrom)
{
	(m_kTo.*CallbackFunc)(uiEvent, kFrom);
}

//+-----------------------------------------------------------------------------------------------------------------
template<class From, class To>
void FvDynamicCallback0<From, To>::Exec(const FvUInt32 uiEvent, From& kFrom)
{
	(m_kTo.*m_pCallbackFunc)(uiEvent, kFrom);
}
//+-----------------------------------------------------------------------------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------------------------------------------------------------------------

template<class From, class Param0, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&)>
void FvAsynCallback1<From, Param0, To, CallbackFunc>::Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0)
{
	FvParamChecker::AsynSafeParam<Param0>();
	m_uiEvent = uiEvent;
	m_kParam0 = kParam0;
	S_List().AttachBackSafeIterator(*this);
}
//+-----------------------------------------------------------------------------------------------------------------
template<class From, class Param0, class To>
void FvDynamicAsynCallback1<From, Param0, To>::Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0)
{
	FvParamChecker::AsynSafeParam<Param0>();
	m_uiEvent = uiEvent;
	m_kParam0 = kParam0;
	S_List().AttachBackSafeIterator(*this);
}
//+-----------------------------------------------------------------------------------------------------------------
template<class From, class Param0>
void FvAsynCallbackList1<From, Param0>::Invoke(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0)
{
	for (m_kList.BeginIterator(); !m_kList.IsEnd();)
	{
		Callback* pkCallback = m_kList.GetIterator()->Content();
		FV_ASSERT(pkCallback);
		m_kList.Next();
		pkCallback->Exec(uiEvent, kFrom, kParam0);
	}
}
//+-----------------------------------------------------------------------------------------------------------------
template<class From, class Param0, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&)>
void FvCallback1<From, Param0, To, CallbackFunc>::Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0)
{
	(m_kTo.*CallbackFunc)(uiEvent, kFrom, kParam0);
}
//+-----------------------------------------------------------------------------------------------------------------
template<class From, class Param0, class To>
void FvDynamicCallback1<From, Param0, To>::Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0)
{
	(m_kTo.*m_pCallbackFunc)(uiEvent, kFrom, kParam0);
}
//+-----------------------------------------------------------------------------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------------------------------------------------------------------------

template<class From, class Param0, class Param1, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&, const Param1&)>
void FvAsynCallback2<From, Param0, Param1, To, CallbackFunc>::Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0, const Param1& kParam1)
{
	FvParamChecker::AsynSafeParam<Param0>();
	FvParamChecker::AsynSafeParam<Param1>();
	m_uiEvent = uiEvent;
	m_kParam0 = kParam0;
	m_kParam1 = kParam1;
	S_List().AttachBackSafeIterator(*this);
}
//+-----------------------------------------------------------------------------------------------------------------
template<class From, class Param0, class Param1, class To>
void FvDynamicAsynCallback2<From, Param0, Param1, To>::Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0, const Param1& kParam1)
{
	FvParamChecker::AsynSafeParam<Param0>();
	FvParamChecker::AsynSafeParam<Param1>();
	m_uiEvent = uiEvent;
	m_kParam0 = kParam0;
	m_kParam1 = kParam1;
	S_List().AttachBackSafeIterator(*this);
}
//+-----------------------------------------------------------------------------------------------------------------
template<class From, class Param0, class Param1>
void FvAsynCallbackList2<From, Param0, Param1>::Invoke(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0, const Param1& kParam1)
{
	for (m_kList.BeginIterator(); !m_kList.IsEnd();)
	{
		Callback* pkCallback = m_kList.GetIterator()->Content();
		FV_ASSERT(pkCallback);
		m_kList.Next();
		pkCallback->Exec(uiEvent, kFrom, kParam0, kParam1);
	}
}
//+-----------------------------------------------------------------------------------------------------------------
template<class From, class Param0, class Param1, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&, const Param1&)>
void FvCallback2<From, Param0, Param1, To, CallbackFunc>::Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0, const Param1& kParam1)
{
	(m_kTo.*CallbackFunc)(uiEvent, kFrom, kParam0, kParam1);
}
//+-----------------------------------------------------------------------------------------------------------------
template<class From, class Param0, class Param1, class To>
void FvDynamicCallback2<From, Param0, Param1, To>::Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0, const Param1& kParam1)
{
	(m_kTo.*m_pCallbackFunc)(uiEvent, kFrom, kParam0, kParam1);
}
//+-----------------------------------------------------------------------------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------------------------------------------------------------------------


template <class To, void (To::*CallbackFunc)()>
void FvFrameEndCallback0<To, CallbackFunc>::Exec(To& kTo)
{
	m_pCallee = &kTo;
	_Attach();
}
template <class To, void (To::*CallbackFunc)()>
void FvFrameEndCallback0<To, CallbackFunc>::_Exec()
{ 
	FV_ASSERT(m_pCallee);
	((*m_pCallee).*CallbackFunc)();//! Ö´ÐÐ
}
//+-----------------------------------------------------------------------------------------------------------------
template <class To, class Param0, void (To::*CallbackFunc)(const Param0&)>
void FvFrameEndCallback1<To, Param0, CallbackFunc>::Exec(To& kTo, const Param0& kParam)
{
	m_kParam = kParam;
	m_pCallee = &kTo;
	_Attach();
}
template <class To, class Param0, void (To::*CallbackFunc)(const Param0&)>
void FvFrameEndCallback1<To, Param0, CallbackFunc>::_Exec()
{ 
	FvParamChecker::AsynSafeParam<Param0>();
	FV_ASSERT(m_pCallee);
	((*m_pCallee).*CallbackFunc)(m_kParam);//! Ö´ÐÐ
}
//+-----------------------------------------------------------------------------------------------------------------

//+-----------------------------------------------------------------------------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------------------------------------------------------------------------
