//{future header message}
#ifndef __FvCaller_H__
#define __FvCaller_H__

#include "FvLogicCommonDefine.h"

#include "FvCallback.h"
#include "FvMemoryNode.h"
#include "FvLogicDebug.h"

#include <vector>

//+--------------------------------------------------------------------------------------------------------------------------
template <class From>
class FvAsynCaller
{
	FV_NOT_COPY_COMPARE(FvAsynCaller);
	FV_MEM_TRACKER(FvAsynCaller);
public:
	typedef FvCallbackList0<From> _List;
	typedef std::vector<_List*> _Map;

	FvAsynCaller(){FV_MEM_TRACK;}
	~FvAsynCaller(){Clear();}

	template<class To, void (To::*pCallbackFunc)(const FvUInt32)>
	void Attach(const FvUInt32 uiEvent, FvAsynCallback0<From, To, pCallbackFunc>& kCallback)
	{
		if(uiEvent < m_CallbackRefMap.size())
		{
			_List* pkList = m_CallbackRefMap[uiEvent];
			FV_ASSERT_WARNING(pkList);
			if(pkList)
			{
				pkList->Attach(kCallback);
			}
		}
		else
		{
			FvLogBus::Warning("FvAsynCaller::Attach越界");
		}
	}
	template<class To>
	void Attach(const FvUInt32 uiEvent, void (To::*pCallbackFunc)(const FvUInt32), FvDynamicAsynCallback0<From, To>& kCallback)
	{
		if(uiEvent < m_CallbackRefMap.size())
		{
			_List* pkList = m_CallbackRefMap[uiEvent];
			FV_ASSERT_WARNING(pkList);
			if(pkList)
			{
				pkList->Attach(pCallbackFunc, kCallback);
			}
		}
		else
		{
			FvLogBus::Warning("FvAsynCaller::Attach越界");
		}
	}
	//! 发出回调
	void Invoke(const FvUInt32 uiEvent, From& kFrom);
	void SetEventSize(const FvUInt32 uiEventTotal);
	void Clear();
protected:
	_Map m_CallbackRefMap;
};

template <class From>
class FvFastCaller: public FvAsynCaller<From>
{
public:
	using FvAsynCaller::Attach;
	template<class To, void (To::*pCallbackFunc)(const FvUInt32, From&)>
	void Attach(const FvUInt32 uiEvent, FvCallback0<From, To, pCallbackFunc>& kCallback)
	{
		if(uiEvent < m_CallbackRefMap.size())
		{
			_List* pkList = m_CallbackRefMap[uiEvent];
			FV_ASSERT_WARNING(pkList);
			if(pkList)
			{
				pkList->Attach(kCallback);
			}
		}
		else
		{
			FvLogBus::Warning("FvFastCaller::Attach越界");
		}
	}
	template<class To>
	void Attach(const FvUInt32 uiEvent, void (To::*pCallbackFunc)(const FvUInt32, From&), FvDynamicCallback0<From, To>& kCallback)
	{
		if(uiEvent < m_CallbackRefMap.size())
		{
			_List* pkList = m_CallbackRefMap[uiEvent];
			FV_ASSERT_WARNING(pkList);
			if(pkList)
			{
				pkList->Attach(pCallbackFunc, kCallback);
			}
		}
		else
		{
			FvLogBus::Warning("FvFastCaller::Attach越界");
		}
	}
};

//+-----------------------------------------------------------------------------------------
#include "FvCaller.inl"
//+-----------------------------------------------------------------------------------------
///<测试用例>
#include "FvSmartPointer.h"

namespace FvCaller_Demo
{
	class Caller: public FvFastCaller<Caller>
	{
		
	};
	class Callee
	{
	public:

		Callee():
#pragma warning(push)
#pragma warning(disable: 4355)
		m_kSynCallback(*this)
#pragma warning(pop)
		{}

	private:
		_CALLBACK_FRIEND_CLASS_;
		void OnSyn(const FvUInt32 uiEvent, Caller& qCaller){}		

	public:
		FvCallback0<Caller, Callee, &Callee::OnSyn> m_kSynCallback;

	};

	static void Test()
	{
		Caller caller;
		Callee callee;
		caller.SetEventSize(1);//! 设置事件数量
		caller.Attach(0, callee.m_kSynCallback);
		caller.Invoke(0, caller);

	}
}
//+-----------------------------------------------------------------------------------------

#endif//__FvCaller_H__