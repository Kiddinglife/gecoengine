#pragma once

#include "FvLogicCommonDefine.h"

#include "FvCallback.h"
#include "FvMemoryNode.h"
#include "FvTuple.h"

#include <FvSmartPointer.h>
#include <FvRefList.h>

//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From>
class FvTupleCallbackInterface: private FvRefNode1<FvTupleCallbackInterface<From>*>
{
	FV_MEM_TRACKER(FvTupleCallbackInterface);
	FV_NOT_COPY_COMPARE(FvTupleCallbackInterface);
public:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData) = 0;
	bool IsAttach()const{return FvRefNode1::IsAttach();}
	void Detach(){FvRefNode1::Detach();}
protected:
	FvTupleCallbackInterface(){FV_MEM_TRACK;}
	~FvTupleCallbackInterface(){}
	template<class From>friend class FvTupleAsynCallbackList;
	template<class From>friend class FvTupleAsynCaller;
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From>
class FvTupleAsynCallbackInterface: public FvAsynCallbackInterface, public FvTupleCallbackInterface<From> 
{
	FV_NOT_COPY_COMPARE(FvTupleAsynCallbackInterface);
public:
	bool IsAttach()const{return FvTupleCallbackInterface::IsAttach();}
	void Detach(){FvAsynCallbackInterface::Detach(); FvTupleCallbackInterface::Detach();}
protected:
	FvTupleAsynCallbackInterface(){}
	~FvTupleAsynCallbackInterface(){}
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const FvBaseTuple&)>
class FvTupleCallback: public FvTupleCallbackInterface<From>
{
	FV_NOT_COPY_COMPARE(FvTupleCallback);
public:
	FvTupleCallback(To& kTo):m_kTo(kTo){}
	~FvTupleCallback(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData);
	To& m_kTo;
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From, class To, void (To::*CallbackFunc)(const FvUInt32, From&)>
class FvTupleCallback0: public FvTupleCallbackInterface<From>
{
	FV_NOT_COPY_COMPARE(FvTupleCallback0);
public:
	FvTupleCallback0(To& kTo):m_kTo(kTo){}
	~FvTupleCallback0(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData);
	To& m_kTo;
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From, class To>
class FvTupleDynamicCallback0: public FvTupleCallbackInterface<From>
{
	FV_NOT_COPY_COMPARE(FvTupleDynamicCallback0);
public:
	typedef void (To::*CallbackFunc)(const FvUInt32);
	FvTupleDynamicCallback0(To& kTo):m_kTo(kTo), m_pCallbackFunc(NULL){}
	~FvTupleDynamicCallback0(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData);
	To& m_kTo;
	CallbackFunc m_pCallbackFunc;
	template<class>friend class FvTupleAsynCallbackList;
	template <class>friend class FvTupleAsynCaller;
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From, class To, void (To::*CallbackFunc)(const FvUInt32)>
class FvTupleAsynCallback0: public FvTupleAsynCallbackInterface<From>
{
	FV_NOT_COPY_COMPARE(FvTupleAsynCallback0);
public:
	FvTupleAsynCallback0(To& kTo):m_kTo(kTo), m_uiEvent(0){}
	~FvTupleAsynCallback0(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData);
	virtual void _AsynExec(){(m_kTo.*CallbackFunc)(m_uiEvent);}
	To& m_kTo;
	FvUInt32 m_uiEvent;
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From, class To>
class FvTupleDynamicAsynCallback0: public FvTupleAsynCallbackInterface<From>
{
	FV_NOT_COPY_COMPARE(FvTupleDynamicAsynCallback0);
public:
	typedef void (To::*CallbackFunc)(const FvUInt32);
	FvTupleDynamicAsynCallback0(To& kTo):m_kTo(kTo), m_uiEvent(0), m_pCallbackFunc(NULL){}
	~FvTupleDynamicAsynCallback0(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData);
	virtual void _AsynExec(){FV_ASSERT_ERROR(m_pCallbackFunc);(m_kTo.*m_pCallbackFunc)(m_uiEvent);}
	To& m_kTo;
	FvUInt32 m_uiEvent;
	CallbackFunc m_pCallbackFunc;
	template<class>friend class FvTupleAsynCallbackList;
	template <class>friend class FvTupleAsynCaller;
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From, class Param0, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&)>
class FvTupleCallback1: public FvTupleCallbackInterface<From>
{
	FV_NOT_COPY_COMPARE(FvTupleCallback1);
public:
	FvTupleCallback1(To& kTo):m_kTo(kTo){}
	~FvTupleCallback1(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData);
	To& m_kTo;
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From, class Param0, class To>
class FvTupleDynamicCallback1: public FvTupleCallbackInterface<From>
{
	FV_NOT_COPY_COMPARE(FvTupleDynamicCallback1);
public:
	typedef void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&);
	FvTupleDynamicCallback1(To& kTo):m_kTo(kTo), m_pCallbackFunc(NULL){}
	~FvTupleDynamicCallback1(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData);
	To& m_kTo;
	CallbackFunc m_pCallbackFunc;
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From, class Param0, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&)>
class FvTupleAsynCallback1: public FvTupleAsynCallbackInterface<From>
{
	FV_NOT_COPY_COMPARE(FvTupleAsynCallback1);
public:
	FvTupleAsynCallback1(To& kTo):m_kTo(kTo), m_uiEvent(0){}
	~FvTupleAsynCallback1(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData);
	virtual void _AsynExec(){(m_kTo.*CallbackFunc)(m_uiEvent, m_kParam0);}
	To& m_kTo;
	FvUInt32 m_uiEvent;
	Param0 m_kParam0;
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From, class Param0, class To>
class FvTupleDynamicAsynCallback1: public FvTupleAsynCallbackInterface<From>
{
	FV_NOT_COPY_COMPARE(FvTupleDynamicAsynCallback1);
public:
	typedef void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&);
	FvTupleDynamicAsynCallback1(To& kTo):m_kTo(kTo), m_uiEvent(0){}
	~FvTupleDynamicAsynCallback1(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData);
	virtual void _AsynExec(){FV_ASSERT_ERROR(m_pCallbackFunc);(m_kTo.*m_pCallbackFunc)(m_uiEvent, m_kParam0);}
	To& m_kTo;
	FvUInt32 m_uiEvent;
	Param0 m_kParam0;
	CallbackFunc m_pCallbackFunc;
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From, class Param0, class Param1, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&, const Param1&)>
class FvTupleCallback2: public FvTupleCallbackInterface<From>
{
	FV_NOT_COPY_COMPARE(FvTupleCallback2);
public:
	FvTupleCallback2(To& kTo):m_kTo(kTo){}
	~FvTupleCallback2(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData);
	To& m_kTo;
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From, class Param0, class Param1, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&, const Param1&)>
class FvTupleAsynCallback2: public FvTupleAsynCallbackInterface<From>
{
	FV_NOT_COPY_COMPARE(FvTupleAsynCallback2);
public:
	FvTupleAsynCallback2(To& kTo):m_kTo(kTo){}
	~FvTupleAsynCallback2(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData);
	virtual void _AsynExec(){(m_kTo.*CallbackFunc)(m_uiEvent, m_kParam0, m_kParam1);}
	To& m_kTo;
	FvUInt32 m_uiEvent;
	Param0 m_kParam0;
	Param1 m_kParam1;
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From, class Param0, class Param1, class Param2, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&, const Param1&, const Param2&)>
class FvTupleCallback3: public FvTupleCallbackInterface<From>
{
	FV_NOT_COPY_COMPARE(FvTupleCallback3);
public:
	FvTupleCallback3(To& kTo):m_kTo(kTo){}
	~FvTupleCallback3(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData);
	To& m_kTo;
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From, class Param0, class Param1, class Param2, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&, const Param1&, const Param2&)>
class FvTupleAsynCallback3: public FvTupleAsynCallbackInterface<From>
{
	FV_NOT_COPY_COMPARE(FvTupleAsynCallback3);
public:
	FvTupleAsynCallback3(To& kTo):m_kTo(kTo){}
	~FvTupleAsynCallback3(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData);
	virtual void _AsynExec(){(m_kTo.*CallbackFunc)(m_uiEvent, m_kParam0, m_kParam1, m_kParam2);}
	To& m_kTo;
	FvUInt32 m_uiEvent;
	Param0 m_kParam0;
	Param1 m_kParam1;
	Param2 m_kParam2;
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From>
class FvTupleAsynCallbackList
{
	FV_MEM_TRACKER(FvTupleAsynCallbackList);
	FV_NOT_COPY_COMPARE(FvTupleAsynCallbackList);
	typedef FvTupleCallbackInterface<From> Callback;
	typedef FvRefList<Callback*, FvRefNode1> List;
public:
	FvTupleAsynCallbackList(){FV_MEM_TRACK;}
	~FvTupleAsynCallbackList(){Clear();}
	//
	void Clear(){m_kList.Clear();}
	//
	void Invoke(const FvUInt32 uiEvent, From& kFrom);
	void Invoke(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData);
	template<class Param0>
	void Invoke(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0);
	template<class Param0, class Param1>
	void Invoke(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0, const Param1& kParam1);
	template<class Param0, class Param1, class Param2>
	void Invoke(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0, const Param1& kParam1, const Param2& kParam2);
	//
	void Attach(FvTupleAsynCallbackInterface<From>& kNode){_Attach(kNode);}
	//
	template<class To>
	void Attach(FvTupleDynamicAsynCallback0<From, To>& kNode, void (To::*CallbackFunc)(const FvUInt32))
	{kNode.m_pCallbackFunc = CallbackFunc; _Attach(kNode);}
	template<class To, class Param0>
	void Attach(FvTupleDynamicAsynCallback1<From, Param0, To>& kNode, void (To::*CallbackFunc)(const FvUInt32, const Param0&))
	{kNode.m_pCallbackFunc = CallbackFunc; _Attach(kNode);}

protected:
	void _Attach(Callback& kNode)
	{
		m_kList.AttachBack(kNode);
	}
	void _Invoke(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData);
	List m_kList;
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From>
class FvTupleCallbackList: public FvTupleAsynCallbackList<From>
{
	FV_NOT_COPY_COMPARE(FvTupleCallbackList);
public:
	FvTupleCallbackList(){}
	~FvTupleCallbackList(){}
	//
	using FvTupleAsynCallbackList::Attach;
	//
	void Attach(FvTupleCallbackInterface<From>& kNode){_Attach(kNode);}
	template<class To>
	void Attach(FvTupleDynamicCallback0<From, To>& kNode, void (To::*CallbackFunc)(const FvUInt32))
	{kNode.m_pCallbackFunc = CallbackFunc; _Attach(kNode);}
	template<class To, class Param0>
	void Attach(FvTupleDynamicCallback1<From, Param0, To>& kNode, void (To::*CallbackFunc)(const FvUInt32, const Param0&))
	{kNode.m_pCallbackFunc = CallbackFunc; _Attach(kNode);}
	//
	template <class>friend class FvTupleAsynCaller;
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template <class From>
class FvTupleAsynCaller
{
	FV_MEM_TRACKER(FvTupleAsynCaller);
	FV_NOT_COPY_COMPARE(FvTupleAsynCaller);
public:
	typedef std::vector<FvTupleCallbackList<From>*> _Map;
	typedef FvTupleCallbackInterface<From> Callback;

	//+-------------------------------------------------------------------------------------------------
	FvTupleAsynCaller(){FV_MEM_TRACK;}
	~FvTupleAsynCaller(){Clear();}
	//
	template<FvUInt32 EVENT_ID, class Param0> static void ParamCheck();
	template<FvUInt32 EVENT_ID, class Param0, class Param1> static void ParamCheck();
	template<FvUInt32 EVENT_ID, class Param0, class Param1, class Param2> static void ParamCheck();
	//
	void SetEventSize(const FvUInt32 uiEventTotal);
	void Clear();
	//
	void Invoke(const FvUInt32 uiEvent, From& kFrom);
	template<FvUInt32 EVENT_ID, class Param0, class Param1, class Param2>
	void Invoke(From& kFrom, const Param0& kParam0, const Param1& kParam1, const Param2& kParam2);
	template<FvUInt32 EVENT_ID, class Param0, class Param1>
	void Invoke(From& kFrom, const Param0& kParam0, const Param1& kParam1);
	template<FvUInt32 EVENT_ID, class Param0>
	void Invoke(From& kFrom, const Param0& kParam0);
	//
	template<class To, void (To::*CallbackFunc)(const FvUInt32)>
	void Attach(const FvUInt32 uiEvent, FvTupleAsynCallback0<From, To, CallbackFunc>& kCallback){_Attach(uiEvent, kCallback);}
	template<FvUInt32 EVENT_ID, class Param0, class Param1, class Param2, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&, const Param1&, const Param2&)>
	void Attach(FvTupleAsynCallback3<From, Param0, Param1, Param2, To, CallbackFunc>& kCallback){ParamCheck<EVENT_ID, Param0, Param1, Param2>(); _Attach(EVENT_ID, kCallback);}
	template<FvUInt32 EVENT_ID, class Param0, class Param1, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&, const Param1&)>
	void Attach(FvTupleAsynCallback2<From, Param0, Param1, To, CallbackFunc>& kCallback){ParamCheck<EVENT_ID, Param0, Param1>(); _Attach(EVENT_ID, kCallback);}
	template<FvUInt32 EVENT_ID, class Param0 , class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&)>
	void Attach(FvTupleAsynCallback1<From, Param0, To, CallbackFunc>& kCallback){ParamCheck<EVENT_ID, Param0>(); _Attach(EVENT_ID, kCallback);}
	template<class To>
	void Attach(const FvUInt32 uiEvent, FvTupleDynamicAsynCallback0<From, To>& kNode, void (To::*CallbackFunc)(const FvUInt32)){kNode.m_pCallbackFunc = CallbackFunc; _Attach(uiEvent, kNode);}
	template<FvUInt32 EVENT_ID, class To, class Param0>
	void Attach(FvTupleDynamicAsynCallback1<From, Param0, To>& kNode, void (To::*CallbackFunc)(const FvUInt32, const Param0&)){ParamCheck<EVENT_ID, Param0>(); kNode.m_pCallbackFunc = CallbackFunc; _Attach(EVENT_ID, kNode);}

protected:
	//
	void _Attach(const FvUInt32 uiEvent, Callback& kNode);
	void _Invoke(const FvUInt32 uiEvent, From& kFrom, const FvBaseTuple& kData);
	//
	_Map m_CallbackRefMap;
};

template <class From>
class FvTupleCaller: public FvTupleAsynCaller<From>
{
	FV_NOT_COPY_COMPARE(FvTupleCaller);
public:
	FvTupleCaller(){}
	~FvTupleCaller(){}
	//
	using FvTupleAsynCaller::Attach;
	template<class To, void (To::*CallbackFunc)(const FvUInt32, From&)>
	void Attach(const FvUInt32 uiEvent, FvTupleCallback0<From, To, CallbackFunc>& kCallback){_Attach(uiEvent, kCallback);}
	template<FvUInt32 EVENT_ID, class Param0, class Param1, class Param2, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&, const Param1&, const Param2&)>
	void Attach(FvTupleCallback3<From, Param0, Param1, Param2, To, CallbackFunc>& kCallback){ParamCheck<EVENT_ID, Param0, Param1, Param2>(); _Attach(EVENT_ID, kCallback);}
	template<FvUInt32 EVENT_ID, class Param0, class Param1, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&, const Param1&)>
	void Attach(FvTupleCallback2<From, Param0, Param1, To, CallbackFunc>& kCallback){ParamCheck<EVENT_ID, Param0, Param1>(); _Attach(EVENT_ID, kCallback);}
	template<FvUInt32 EVENT_ID, class Param0 , class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&)>
	void Attach(FvTupleCallback1<From, Param0, To, CallbackFunc>& kCallback){ParamCheck<EVENT_ID, Param0>(); _Attach(EVENT_ID, kCallback);}
	template<class To>
	void Attach(const FvUInt32 uiEvent, FvTupleDynamicCallback0<From, To>& kNode, void (To::*CallbackFunc)(const FvUInt32)){kNode.m_pCallbackFunc = CallbackFunc; _Attach(uiEvent, kNode);}
	template<FvUInt32 EVENT_ID, class To, class Param0>
	void Attach(FvTupleDynamicCallback1<From, Param0, To>& kNode, void (To::*CallbackFunc)(const FvUInt32, const Param0&)){ParamCheck<EVENT_ID, Param0>(); kNode.m_pCallbackFunc = CallbackFunc; _Attach(EVENT_ID, kNode);}
};

//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
#define _TUPLE_CALLBACK_FRIEND_CLASS_ \
	template<class From, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const FvBaseTuple&)>friend class FvTupleCallback;\
	template<class From, class To, void (To::*CallbackFunc)(const FvUInt32, From&)>friend class FvTupleCallback0;\
	template<class From, class Param0, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&)>friend class FvTupleCallback1;\
	template<class From, class Param0, class Param1, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&, const Param1&)>friend class FvTupleCallback2;\
	template<class From, class Param0, class Param1, class Param2, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&, const Param1&, const Param2&)> friend class FvTupleCallback3;\
	template<class From, class To, void (To::*CallbackFunc)(const FvUInt32)>friend class FvTupleAsynCallback0;\
	template<class From, class Param0, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&)>friend class FvTupleAsynCallback1;\
	template<class From, class Param0, class Param1, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&, const Param1&)>friend class FvTupleAsynCallback2;\
	template<class From, class Param0, class Param1, class Param2, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&, const Param1&, const Param2&)> friend class FvTupleAsynCallback3;\
	template<class From, class To>friend class FvTupleDynamicCallback0;\
	template<class From, class To>friend class FvTupleDynamicAsynCallback0;\
	_CALLBACK_FRIEND_CLASS_;

#include "FvTupleCallback.inl"

namespace TupleCallback_Demo
{
	class Param
	{
	};

	class From
	{
	public:

	};

	class To
	{
	public:
		To():
#pragma warning(push)
#pragma warning(disable: 4355)
		  m_kNode0(*this)
			  ,m_kNode1(*this)
			  ,m_kNode2(*this)
			  ,m_kNode3(*this)
			  ,m_kAsynNode0(*this)
			  ,m_kAsynNode1(*this)
			  ,m_kAsynNode2(*this)
			  ,m_kAsynNode3(*this)
#pragma warning(pop)
		  {}

	private:
		_TUPLE_CALLBACK_FRIEND_CLASS_;

		void Func0(const FvUInt32 uiEvent, From& kFrom){}
		void Func1(const FvUInt32 uiEvent, From& kFrom, const FvUInt32& uiValue){}
		void Func2(const FvUInt32 uiEvent, From& kFrom, const FvUInt32& uiValue, const float& fValue){}
		void Func3(const FvUInt32 uiEvent, From& kFrom, const FvUInt32& uiValue, const float& fValue, Param*const& bValue){}

		void AsynFunc0(const FvUInt32 uiEvent){}
		void AsynFunc1(const FvUInt32 uiEvent, const FvUInt32& uiValue){}
		void AsynFunc2(const FvUInt32 uiEvent, const FvUInt32& uiValue, const float& fValue){}
		void AsynFunc3(const FvUInt32 uiEvent, const FvUInt32& uiValue, const float& fValue, Param*const& bValue){}

	public:

		FvTupleCallback0<From, To, &Func0> m_kNode0;
		FvTupleCallback1<From, FvUInt32, To, &Func1> m_kNode1;
		FvTupleCallback2<From, FvUInt32, float, To, &Func2> m_kNode2;
		FvTupleCallback3<From, FvUInt32, float, Param*, To, &Func3> m_kNode3;
		FvTupleAsynCallback0<From, To, &AsynFunc0> m_kAsynNode0;
		FvTupleAsynCallback1<From, FvUInt32, To, &AsynFunc1> m_kAsynNode1;
		FvTupleAsynCallback2<From, FvUInt32, float, To, &AsynFunc2> m_kAsynNode2;
		FvTupleAsynCallback3<From, FvUInt32, float, Param*, To, &AsynFunc3> m_kAsynNode3;
	};

	static void Test()
	{
		From kFrom;
		To kTo;

		FvStaticTuple<FvUInt32, float, bool> kData;

		FvTupleCallbackList<From> kList;	

		kList.Attach(kTo.m_kNode0);
		kList.Attach(kTo.m_kNode1);
		kList.Attach(kTo.m_kNode2);
		kList.Attach(kTo.m_kNode3);
		kList.Attach(kTo.m_kAsynNode0);
		kList.Attach(kTo.m_kAsynNode1);
		kList.Attach(kTo.m_kAsynNode2);
		kList.Attach(kTo.m_kAsynNode3);
		kList.Invoke(0, kFrom, kData);
		kList.Invoke(0, kFrom);
		FvAsynCallbackInterface::UpdateAsynCallbacks();
	}
}