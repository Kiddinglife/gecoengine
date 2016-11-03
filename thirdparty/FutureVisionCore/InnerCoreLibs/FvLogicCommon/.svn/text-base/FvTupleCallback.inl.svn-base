#include "FvTupleCallback.h"



//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class From, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const FvBaseTuple&)>
void FvTupleCallback<From, To, CallbackFunc>::Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData)
{
	(m_kTo.*CallbackFunc)(uiEvent, kFrom, kData);
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class From, class To, void (To::*CallbackFunc)(const FvUInt32, From&)>
void FvTupleCallback0<From, To, CallbackFunc>::Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData)
{
	(m_kTo.*CallbackFunc)(uiEvent, kFrom);
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class From, class To>
void FvTupleDynamicCallback0<From, To>::Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData)
{
	FV_ASSERT_ERROR(m_pCallbackFunc);
	(m_kTo.*m_pCallbackFunc)(uiEvent, kFrom);
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class From, class To, void (To::*CallbackFunc)(const FvUInt32)>
void FvTupleAsynCallback0<From, To, CallbackFunc>::Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData)
{
	m_uiEvent = uiEvent;
	S_List().AttachBackSafeIterator(*this);
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class From, class To>
void FvTupleDynamicAsynCallback0<From, To>::Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData)
{
	m_uiEvent = uiEvent;
	S_List().AttachBackSafeIterator(*this);
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class From, class Param0, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&)>
void FvTupleCallback1<From, Param0, To, CallbackFunc>::Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData)
{
	Param0 kParam0;
	if(kData.GetValue(0, kParam0) == false)
	{
		FvLogBus::Warning("FvTupleCallback 参数[0]解析失败");
		return;
	}
	(m_kTo.*CallbackFunc)(uiEvent, kFrom, kParam0);
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class From, class Param0, class To>
void FvTupleDynamicCallback1<From, Param0, To>::Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData)
{
	Param0 kParam0;
	if(kData.GetValue(0, kParam0) == false)
	{
		FvLogBus::Warning("FvTupleCallback 参数[0]解析失败");
		return;
	}
	FV_ASSERT_ERROR(m_pCallbackFunc);
	(m_kTo.*m_pCallbackFunc)(uiEvent, kFrom, kParam0);
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class From, class Param0, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&)>
void FvTupleAsynCallback1<From, Param0, To, CallbackFunc>::Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData)
{
	FvParamChecker::AsynSafeParam<Param0>();
	Param0 kParam0;
	if(kData.GetValue(0, kParam0) == false)
	{
		FvLogBus::Warning("FvTupleCallback 参数[0]解析失败");
		return;
	}
	m_uiEvent = uiEvent;
	m_kParam0 = kParam0;
	S_List().AttachBackSafeIterator(*this);
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class From, class Param0, class To>
void FvTupleDynamicAsynCallback1<From, Param0, To>::Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData)
{
	FvParamChecker::AsynSafeParam<Param0>();
	Param0 kParam0;
	if(kData.GetValue(0, kParam0) == false)
	{
		FvLogBus::Warning("FvTupleCallback 参数[0]解析失败");
		return;
	}
	m_uiEvent = uiEvent;
	m_kParam0 = kParam0;
	S_List().AttachBackSafeIterator(*this);
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class From, class Param0, class Param1, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&, const Param1&)>
void FvTupleCallback2<From, Param0, Param1, To, CallbackFunc>::Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData)
{
	Param0 kParam0;
	if(kData.GetValue(0, kParam0) == false)
	{
		FvLogBus::Warning("FvTupleCallback 参数[0]解析失败");
		return;
	}
	Param1 kParam1;
	if(kData.GetValue(1, kParam1) == false)
	{
		FvLogBus::Warning("FvTupleCallback 参数[1]解析失败");
		return;
	}
	(m_kTo.*CallbackFunc)(uiEvent, kFrom, kParam0, kParam1);
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class From, class Param0, class Param1, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&, const Param1&)>
void FvTupleAsynCallback2<From, Param0, Param1, To, CallbackFunc>::Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData)
{
	FvParamChecker::AsynSafeParam<Param0>();
	FvParamChecker::AsynSafeParam<Param1>();
	Param0 kParam0;
	if(kData.GetValue(0, kParam0) == false)
	{
		FvLogBus::Warning("FvTupleCallback 参数[0]解析失败");
		return;
	}
	Param1 kParam1;
	if(kData.GetValue(1, kParam1) == false)
	{
		FvLogBus::Warning("FvTupleCallback 参数[1]解析失败");
		return;
	}
	m_uiEvent = uiEvent;
	m_kParam0 = kParam0;
	m_kParam1 = kParam1;
	S_List().AttachBackSafeIterator(*this);
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class From, class Param0, class Param1, class Param2, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&, const Param1&, const Param2&)>
void FvTupleCallback3<From, Param0, Param1, Param2, To, CallbackFunc>::Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData)
{
	Param0 kParam0;
	if(kData.GetValue(0, kParam0) == false)
	{
		FvLogBus::Warning("FvTupleCallback 参数[0]解析失败");
		return;
	}
	Param1 kParam1;
	if(kData.GetValue(1, kParam1) == false)
	{
		FvLogBus::Warning("FvTupleCallback 参数[1]解析失败");
		return;
	}
	Param2 kParam2;
	if(kData.GetValue(2, kParam2) == false)
	{
		FvLogBus::Warning("FvTupleCallback 参数[2]解析失败");
		return;
	}
	(m_kTo.*CallbackFunc)(uiEvent, kFrom, kParam0, kParam1, kParam2);
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class From, class Param0, class Param1, class Param2, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&, const Param1&, const Param2&)>
void FvTupleAsynCallback3<From, Param0, Param1, Param2, To, CallbackFunc>::Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData)
{
	FvParamChecker::AsynSafeParam<Param0>();
	FvParamChecker::AsynSafeParam<Param1>();
	FvParamChecker::AsynSafeParam<Param2>();
	Param0 kParam0;
	if(kData.GetValue(0, kParam0) == false)
	{
		FvLogBus::Warning("FvTupleCallback 参数[0]解析失败");
		return;
	}
	Param1 kParam1;
	if(kData.GetValue(1, kParam1) == false)
	{
		FvLogBus::Warning("FvTupleCallback 参数[1]解析失败");
		return;
	}
	Param2 kParam2;
	if(kData.GetValue(2, kParam2) == false)
	{
		FvLogBus::Warning("FvTupleCallback 参数[2]解析失败");
		return;
	}
	m_uiEvent = uiEvent;
	m_kParam0 = kParam0;
	m_kParam1 = kParam1;
	m_kParam2 = kParam2;
	S_List().AttachBackSafeIterator(*this);
}
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class From>
void FvTupleAsynCallbackList<From>::Invoke(const FvUInt32 uiEvent, From& kFrom)
{
	static FvStaticEmptyTuple S_Data;
	_Invoke(uiEvent, kFrom, S_Data);
}
template<class From>
void FvTupleAsynCallbackList<From>::Invoke(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData)
{
	_Invoke(uiEvent, kFrom, kData);
}
template<class From>
template<class Param0, class Param1, class Param2>
void FvTupleAsynCallbackList<From>::Invoke(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0, const Param1& kParam1, const Param2& kParam2)
{
	FvStaticTuple<Param0, Param1, Param2> kTuple;
	kTuple.SetValue(0, kParam0);
	kTuple.SetValue(1, kParam1);
	kTuple.SetValue(2, kParam2);
	_Invoke(uiEvent, kFrom, kTuple);
}
template<class From>
template<class Param0, class Param1>
void FvTupleAsynCallbackList<From>::Invoke(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0, const Param1& kParam1)
{
	FvStaticTuple<Param0, Param1> kTuple;
	kTuple.SetValue(0, kParam0);
	kTuple.SetValue(1, kParam1);
	_Invoke(uiEvent, kFrom, kTuple);
}
template<class From>
template<class Param0>
void FvTupleAsynCallbackList<From>::Invoke(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0)
{
	FvStaticTuple<Param0> kTuple;
	kTuple.SetValue(0, kParam0);
	_Invoke(uiEvent, kFrom, kTuple);
}
template<class From>
void FvTupleAsynCallbackList<From>::_Invoke(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData)
{
	m_kList.BeginIterator();
	while(!m_kList.IsEnd())
	{
		List::iterator iter = m_kList.GetIterator();
		Callback* pkCallback = static_cast<Callback*>(iter);
		FV_ASSERT(pkCallback);
		m_kList.Next();
		pkCallback->Exec(uiEvent, kFrom, kData);
	}
}
//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template <class From>
void FvTupleAsynCaller<From>::_Attach(const FvUInt32 uiEvent, Callback& kNode)
{
	if(uiEvent < m_CallbackRefMap.size())
	{
		FV_ASSERT(m_CallbackRefMap[uiEvent]);
		m_CallbackRefMap[uiEvent]->Attach(kNode);
	}
	else
	{
		FvLogBus::Warning("FvFastCaller::Attach越界");
	}
}
template <class From>
void FvTupleAsynCaller<From>::Invoke(const FvUInt32 uiEvent, From& kFrom)
{
	static FvStaticEmptyTuple S_Data;
	_Invoke(uiEvent, kFrom, S_Data);
}
template <class From>
template<FvUInt32 EVENT_ID, class Param0>
void FvTupleAsynCaller<From>::Invoke(From& kFrom, const Param0& kParam0)
{
	ParamCheck<EVENT_ID, Param0>();
	FvStaticTuple<Param0> kTuple;
	kTuple.SetValue(0, kParam0);
	_Invoke(EVENT_ID, kFrom, kTuple);
}
template <class From>
template<FvUInt32 EVENT_ID, class Param0, class Param1>
void FvTupleAsynCaller<From>::Invoke(From& kFrom, const Param0& kParam0, const Param1& kParam1)
{
	ParamCheck<EVENT_ID, Param0, Param1>();
	FvStaticTuple<Param0, Param1> kTuple;
	kTuple.SetValue(0, kParam0);
	kTuple.SetValue(1, kParam1);
	_Invoke(EVENT_ID, kFrom, kTuple);
}
template <class From>
template<FvUInt32 EVENT_ID, class Param0, class Param1, class Param2>
void FvTupleAsynCaller<From>::Invoke(From& kFrom, const Param0& kParam0, const Param1& kParam1, const Param2& kParam2)
{
	ParamCheck<EVENT_ID, Param0, Param1, Param2>();
	FvStaticTuple<Param0, Param1, Param2> kTuple;
	kTuple.SetValue(0, kParam0);
	kTuple.SetValue(1, kParam1);
	kTuple.SetValue(2, kParam2);
	_Invoke(EVENT_ID, kFrom, kTuple);
}
template <class From>
void FvTupleAsynCaller<From>::_Invoke(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData)
{
	if(uiEvent < m_CallbackRefMap.size())
	{
		FV_ASSERT(m_CallbackRefMap[uiEvent]);
		m_CallbackRefMap[uiEvent]->_Invoke(uiEvent, kFrom, kData);
	}
	else
	{
		FvLogBus::Warning("FvFastCaller::Attach越界");
	}
}
template <class From>
void FvTupleAsynCaller<From>::SetEventSize(const FvUInt32 uiEventTotal)
{
	Clear();
	m_CallbackRefMap.resize(uiEventTotal);
	for(FvUInt32 uiIdx = 0; uiIdx < uiEventTotal; ++uiIdx)
	{
		m_CallbackRefMap[uiIdx] = new FvTupleCallbackList<From>();
	}
}
template <class From>
void FvTupleAsynCaller<From>::Clear()
{
	for(FvUInt32 uiIdx = 0; uiIdx < m_CallbackRefMap.size(); ++uiIdx)
	{
		FV_ASSERT(m_CallbackRefMap[uiIdx]);
		if(m_CallbackRefMap[uiIdx])
		{
			delete m_CallbackRefMap[uiIdx];
		}
	}
	m_CallbackRefMap.clear();
}