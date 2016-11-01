//{future header message}
#ifndef __FvCallback_H__
#define __FvCallback_H__

#include "FvLogicCommonDefine.h"

#include "FvMemoryNode.h"
#include "FvLogicDebug.h"
#include "FvParamChecker.h"

#include <FvSmartPointer.h>
#include <FvRefList.h>

//+-----------------------------------------------------------------------------------------------------------------
///<AsynCallback>
class FV_LOGIC_COMMON_API FvAsynCallbackInterface: protected FvRefNode1<FvAsynCallbackInterface*>
{
public:
	typedef FvRefListNameSpace<FvAsynCallbackInterface>::List1 List;
	static void UpdateAsynCallbacks();
	bool IsAsynAttach()const{return FvRefNode1::IsAttach();}
protected:
	FvAsynCallbackInterface();
	virtual void _AsynExec() = 0;
	static List& S_List(){ return S_LIST; }
	static List S_LIST;
};

//+-----------------------------------------------------------------------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<>
template<class From>
class FvCallbackIterface0: private FvRefNode1<FvCallbackIterface0<From>*>
{
	FV_MEM_TRACKER(FvCallbackIterface0);
	FV_NOT_COPY_COMPARE(FvCallbackIterface0);
public:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom) = 0;
	bool IsAttach()const{return FvRefNode1::IsAttach();}
	void Detach(){FvRefNode1::Detach();}
protected:
	FvCallbackIterface0(){FV_MEM_TRACK;}
	~FvCallbackIterface0(){}
	template<class>friend class FvAsynCallbackList0;
	template<class>friend class FvCallbackList0;
};
///<>
template<class From, class To, void (To::*CallbackFunc)(const FvUInt32)>
class FvAsynCallback0: public FvAsynCallbackInterface, public FvCallbackIterface0<From>
{
public:
	FvAsynCallback0(To& kTo):m_kTo(kTo),m_uiEvent(0){}
	~FvAsynCallback0(){}

	bool IsAttach()const{return FvCallbackIterface0::IsAttach();}
	void Detach(){FvAsynCallbackInterface::Detach(); FvCallbackIterface0::Detach();}

private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom);
	virtual void _AsynExec(){(m_kTo.*CallbackFunc)(m_uiEvent);}

	To& m_kTo;
	FvUInt32 m_uiEvent;
	template<class>friend class FvAsynCallbackList0;
};
///<>
template<class From, class To>
class FvDynamicAsynCallback0: public FvAsynCallbackInterface, public FvCallbackIterface0<From>
{
public:
	typedef void (To::*CallbackFunc)(const FvUInt32);
	FvDynamicAsynCallback0(To& kTo):m_kTo(kTo),m_pCallbackFunc(NULL),m_uiEvent(0){}
	~FvDynamicAsynCallback0(){}

	bool IsAttach()const{return FvCallbackIterface0::IsAttach();}
	void Detach(){FvAsynCallbackInterface::Detach(); FvCallbackIterface0::Detach();}

private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom);
	virtual void _AsynExec(){(m_kTo.*m_pCallbackFunc)(m_uiEvent);}

	To& m_kTo;
	CallbackFunc m_pCallbackFunc;
	FvUInt32 m_uiEvent;
	template<class>friend class FvAsynCallbackList0;
};
///<>
template<class From>
class FvAsynCallbackList0
{
	FV_MEM_TRACKER(FvAsynCallbackList0);
	FV_NOT_COPY_COMPARE(FvAsynCallbackList0);
	typedef FvCallbackIterface0<From> Callback;
	typedef FvRefList<Callback*, FvRefNode1> List;
public:
	FvAsynCallbackList0(){FV_MEM_TRACK;}
	void Clear(){m_kList.Clear();}
	//
	void Invoke(const FvUInt32 uiEvent, From& kFrom);
	template<class To, void (To::*CallbackFunc)(const FvUInt32)>
	void Attach(FvAsynCallback0<From, To, CallbackFunc>& kCallback)
	{
		m_kList.AttachBack(kCallback);
	}
	template<class To>
	void Attach(void (To::*CallbackFunc)(const FvUInt32), FvDynamicAsynCallback0<From, To>& kCallback)
	{
		FV_ASSERT_ERROR(CallbackFunc);
		kCallback.m_pCallbackFunc = CallbackFunc;
		m_kList.AttachBack(kCallback);
	}
protected:
	List m_kList;
};
///<>
template<class From, class To, void (To::*CallbackFunc)(const FvUInt32, From&)>
class FvCallback0: public FvCallbackIterface0<From>
{
public:
	FvCallback0(To& kTo):m_kTo(kTo){}
	~FvCallback0(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom);
	To& m_kTo;
	template<class>friend class FvCallbackList0;
};
///<>
template<class From, class To>
class FvDynamicCallback0: public FvCallbackIterface0<From>
{
public:
	typedef void (To::*CallbackFunc)(const FvUInt32, From&);
	FvDynamicCallback0(To& kTo):m_kTo(kTo),m_pCallbackFunc(NULL){}
	~FvDynamicCallback0(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom);
	To& m_kTo;
	CallbackFunc m_pCallbackFunc;
	template<class>friend class FvCallbackList0;
};
///<>
template<class From>
class FvCallbackList0: public FvAsynCallbackList0<From>
{
public:
	using FvAsynCallbackList0::Attach;
	template<class To, void (To::*CallbackFunc)(const FvUInt32, From&)>
	void Attach(FvCallback0<From, To, CallbackFunc>& kCallback)
	{
		m_kList.AttachBack(kCallback);
	}
	template<class To>
	void Attach(void (To::*CallbackFunc)(const FvUInt32), FvDynamicCallback0<From, To>& kCallback)
	{
		FV_ASSERT_ERROR(CallbackFunc);
		kCallback.m_pCallbackFunc = CallbackFunc;
		m_kList.AttachBack(kCallback);
	}
};
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
template<class From, class Param0>
class FvCallbackInterface1: private FvRefNode1<FvCallbackInterface1<From, Param0>*>
{
	FV_MEM_TRACKER(FvCallbackInterface1);
	FV_NOT_COPY_COMPARE(FvCallbackInterface1);
public:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0) = 0;
	bool IsAttach()const{return FvRefNode1::IsAttach();}
	void Detach(){FvRefNode1::Detach();}
protected:
	FvCallbackInterface1(){FV_MEM_TRACK;}
	~FvCallbackInterface1(){}
	template<class, class>friend class FvAsynCallbackList1;
	template<class, class>friend class FvCallbackList1;
};
///<>
template<class From, class Param0, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&)>
class FvAsynCallback1: public FvAsynCallbackInterface, public FvCallbackInterface1<From, Param0>
{
public:
	FvAsynCallback1(To& kTo):m_kTo(kTo),m_uiEvent(0){}

	bool IsAttach()const{return FvCallbackInterface1::IsAttach();}
	void Detach(){FvAsynCallbackInterface::Detach(); FvCallbackInterface1::Detach();}

private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0);
	virtual void _AsynExec(){(m_kTo.*CallbackFunc)(m_uiEvent, m_kParam0);}

	To& m_kTo;
	FvUInt32 m_uiEvent;
	Param0 m_kParam0;
	template<class, class>friend class FvAsynCallbackList1;
};
///<>
template<class From, class Param0, class To>
class FvDynamicAsynCallback1: public FvAsynCallbackInterface, public FvCallbackInterface1<From, Param0>
{
public:
	typedef void (To::*CallbackFunc)(const FvUInt32, const Param0&);
	FvDynamicAsynCallback1(To& kTo):m_kTo(kTo),m_pCallbackFunc(NULL),m_uiEvent(0){}

	bool IsAttach()const{return FvCallbackInterface1::IsAttach();}
	void Detach(){FvAsynCallbackInterface::Detach(); FvCallbackInterface1::Detach();}

private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0);
	virtual void _AsynExec(){(m_kTo.*m_pCallbackFunc)(m_uiEvent, m_kParam0);}

	To& m_kTo;
	CallbackFunc m_pCallbackFunc;
	FvUInt32 m_uiEvent;
	Param0 m_kParam0;
	template<class, class>friend class FvAsynCallbackList1;
};
///<>
template<class From, class Param0>
class FvAsynCallbackList1
{
	FV_MEM_TRACKER(FvAsynCallbackList1);
	FV_NOT_COPY_COMPARE(FvAsynCallbackList1);
	typedef FvCallbackInterface1<From, Param0> Callback;
	typedef FvRefList<Callback*, FvRefNode1> List;
public:
	FvAsynCallbackList1(){FV_MEM_TRACK;}
	void Clear(){m_kList.Clear();}
	//
	void Invoke(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0);
	template<class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&)>
	void Attach(FvAsynCallback1<From, Param0, To, CallbackFunc>& kCallback)
	{
		m_kList.AttachBack(kCallback);
	}
	template<class To>
	void Attach(void (To::*CallbackFunc)(const FvUInt32, const Param0&), FvDynamicAsynCallback1<From, Param0, To>& kCallback)
	{
		FV_ASSERT_ERROR(CallbackFunc);
		kCallback.m_pCallbackFunc = CallbackFunc;
		m_kList.AttachBack(kCallback);
	}
protected:
	List m_kList;
};
///<>
template<class From, class Param0, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&)>
class FvCallback1: public FvCallbackInterface1<From, Param0>
{
public:
	FvCallback1(To& kTo):m_kTo(kTo){}
	~FvCallback1(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0);
	To& m_kTo;
	template<class, class>friend class FvCallbackList1;
};
///<>
template<class From, class Param0, class To>
class FvDynamicCallback1: public FvCallbackInterface1<From, Param0>
{
public:
	typedef void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&);
	FvDynamicCallback1(To& kTo):m_kTo(kTo), m_pCallbackFunc(NULL){}
	~FvDynamicCallback1(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0);
	To& m_kTo;
	CallbackFunc m_pCallbackFunc;
	template<class, class>friend class FvCallbackList1;
};
///<>
template<class From, class Param0>
class FvCallbackList1: public FvAsynCallbackList1<From, Param0>
{
public:
	using FvAsynCallbackList1::Attach;
	template<class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&)>
	void Attach(FvCallback1<From, Param0, To, CallbackFunc>& kCallback)
	{
		m_kList.AttachBack(kCallback);
	}
	template<class To>
	void Attach(void (To::*CallbackFunc)(const FvUInt32, const Param0&), FvDynamicCallback1<From, Param0, To>& kCallback)
	{
		FV_ASSERT_ERROR(CallbackFunc);
		kCallback.m_pCallbackFunc = CallbackFunc;
		m_kList.AttachBack(kCallback);
	}
};

//+-----------------------------------------------------------------------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
template<class From, class Param0, class Param1>
class FvCallbackInterface2: private FvRefNode1<FvCallbackInterface2<From, Param0, Param1>*>
{
	FV_MEM_TRACKER(FvCallbackInterface2);
	FV_NOT_COPY_COMPARE(FvCallbackInterface2);
public:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0, const Param1& kParam1) = 0;
	bool IsAttach()const{return FvRefNode1::IsAttach();}
	void Detach(){FvRefNode1::Detach();}
protected:
	FvCallbackInterface2(){FV_MEM_TRACK;}
	~FvCallbackInterface2(){}
	template<class, class, class>friend class FvAsynCallbackList2;
	template<class, class, class>friend class FvCallbackList2;
};
///<>
template<class From, class Param0, class Param1, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&, const Param1&)>
class FvAsynCallback2: public FvAsynCallbackInterface, public FvCallbackInterface2<From, Param0, Param1>
{
public:
	FvAsynCallback2(To& kTo):m_kTo(kTo),m_uiEvent(0){}

	bool IsAttach()const{return FvCallbackInterface2::IsAttach();}
	void Detach(){FvAsynCallbackInterface::Detach(); FvCallbackInterface1::Detach();}

private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0, const Param1& kParam1);
	virtual void _AsynExec(){(m_kTo.*CallbackFunc)(m_uiEvent, m_kParam0, m_kParam1);}

	To& m_kTo;
	FvUInt32 m_uiEvent;
	Param0 m_kParam0;
	Param1 m_kParam1;
	template<class, class, class>friend class FvAsynCallbackList2;
};
///<>
template<class From, class Param0, class Param1, class To>
class FvDynamicAsynCallback2: public FvAsynCallbackInterface, public FvCallbackInterface2<From, Param0, Param1>
{
public:
	typedef void (To::*CallbackFunc)(const FvUInt32, const Param0&, const Param1&);
	FvDynamicAsynCallback2(To& kTo):m_kTo(kTo),m_pCallbackFunc(NULL),m_uiEvent(0){}

	bool IsAttach()const{return FvCallbackInterface2::IsAttach();}
	void Detach(){FvAsynCallbackInterface::Detach(); FvCallbackInterface1::Detach();}

private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0, const Param1& kParam1);
	virtual void _AsynExec(){(m_kTo.*m_pCallbackFunc)(m_uiEvent, m_kParam0, m_kParam1);}

	To& m_kTo;
	CallbackFunc m_pCallbackFunc;
	FvUInt32 m_uiEvent;
	Param0 m_kParam0;
	Param1 m_kParam1;
	template<class, class, class>friend class FvAsynCallbackList2;
};
///<>
template<class From, class Param0, class Param1>
class FvAsynCallbackList2
{
	FV_MEM_TRACKER(FvAsynCallbackList2);
	FV_NOT_COPY_COMPARE(FvAsynCallbackList2);
	typedef FvCallbackInterface2<From, Param0, Param1> Callback;
	typedef FvRefList<Callback*, FvRefNode1> List;
public:
	FvAsynCallbackList2(){FV_MEM_TRACK;}
	void Clear(){m_kList.Clear();}
	//
	void Invoke(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0, const Param1& kParam1);
	template<class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&, const Param1&)>
	void Attach(FvAsynCallback2<From, Param0, Param1, To, CallbackFunc>& kCallback)
	{
		m_kList.AttachBack(kCallback);
	}
	template<class To>
	void Attach(void (To::*CallbackFunc)(const FvUInt32, const Param0&, const Param1&), FvDynamicAsynCallback2<From, Param0, Param1, To>& kCallback)
	{
		FV_ASSERT_ERROR(CallbackFunc);
		kCallback.m_pCallbackFunc = CallbackFunc;
		m_kList.AttachBack(kCallback);
	}
protected:
	List m_kList;
};
///<>
template<class From, class Param0, class Param1, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&, const Param1&)>
class FvCallback2: public FvCallbackInterface2<From, Param0, Param1>
{
public:
	FvCallback2(To& kTo):m_kTo(kTo){}
	~FvCallback2(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0, const Param1& kParam1);
	To& m_kTo;
	template<class, class, class>friend class FvCallbackList2;
};
///<>
template<class From, class Param0, class Param1, class To>
class FvDynamicCallback2: public FvCallbackInterface2<From, Param0, Param1>
{
public:
	typedef void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&, const Param1&);
	FvDynamicCallback2(To& kTo):m_kTo(kTo),m_pCallbackFunc(NULL){}
	~FvDynamicCallback2(){}
private:
	virtual void Exec(const FvUInt32 uiEvent, From& kFrom, const Param0& kParam0, const Param1& kParam1);
	To& m_kTo;
	CallbackFunc m_pCallbackFunc;
	template<class, class, class>friend class FvCallbackList2;
};
///<>
template<class From, class Param0, class Param1>
class FvCallbackList2: public FvAsynCallbackList2<From, Param0, Param1>
{
public:
	using FvAsynCallbackList2::Attach;
	template<class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&, const Param1&)>
	void Attach(FvCallback2<From, Param0, Param1, To, CallbackFunc>& kCallback)
	{
		m_kList.AttachBack(kCallback);
	}
	template<class To>
	void Attach(void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&, const Param1&), FvDynamicCallback2<From, Param0, Param1, To>& kCallback)
	{
		FV_ASSERT_ERROR(CallbackFunc);
		kCallback.m_pCallbackFunc = CallbackFunc;
		m_kList.AttachBack(kCallback);
	}
};

//+-----------------------------------------------------------------------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
///<为减少计算延时, 在帧末处理更加及时>
class FV_LOGIC_COMMON_API FvFrameEndCallbackInterface: private FvRefNode1<FvFrameEndCallbackInterface*>
{
	FV_MEM_TRACKER(FvFrameEndCallbackInterface);
	FV_NOT_COPY_COMPARE(FvFrameEndCallbackInterface);
public:
	typedef FvRefListNameSpace<FvFrameEndCallbackInterface>::List1 List;

	static void UpdateCallbacks();
	void Detach();
	bool IsAttach()const{return FvRefNode1::IsAttach();}

protected:
	FvFrameEndCallbackInterface();
	~FvFrameEndCallbackInterface();

	void _Attach();

private:
	static List& S_List(){ return S_LIST; }
	static List S_LIST;
	virtual void _Exec() = 0;//! 执行函数 

};
template <class To, void (To::*CallbackFunc)()>
class FvFrameEndCallback0: public FvFrameEndCallbackInterface
{
public:
	FvFrameEndCallback0():m_pCallee(NULL){}
	void Exec(To& kTo);
private:
	virtual void _Exec();
	To* m_pCallee;
};
template <class To, class Param0, void (To::*CallbackFunc)(const Param0&)>
class FvFrameEndCallback1: public FvFrameEndCallbackInterface
{
public:
	FvFrameEndCallback1():m_pCallee(NULL){}
	void Exec(To& kTo, const Param0& kParam);
private:
	virtual void _Exec();
	Param0 m_kParam;
	To* m_pCallee;
};

#include "FvCallback.inl"

//+-----------------------------------------------------------------------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
#define _CALLBACK_FRIEND_CLASS_ \
	template<class From, class To, void (To::*CallbackFunc)(const FvUInt32, From&)> friend class FvCallback0;\
	template<class From, class Param0, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&)> friend class FvCallback1;\
	template<class From, class Param0, class Param1, class To, void (To::*CallbackFunc)(const FvUInt32, From&, const Param0&, const Param1&)> friend class FvCallback2;\
	template<class From, class To> friend class FvDynamicCallback0;\
	template<class From, class Param0, class To> friend class FvDynamicCallback1;\
	template<class From, class Param0, class Param1, class To> friend class FvDynamicCallback2;\
	template<class From, class To, void (To::*CallbackFunc)(const FvUInt32)> friend class FvAsynCallback0;\
	template<class From, class Param0, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0&)> friend class FvAsynCallback1;\
	template<class From, class Param0, class Param1, class To, void (To::*CallbackFunc)(const FvUInt32, const Param0& , const Param1&)> friend class FvAsynCallback2;\
	template<class From, class To> friend class FvDynamicAsynCallback0;\
	template<class From, class Param0, class To> friend class FvDynamicAsynCallback1;\
	template<class From, class Param0, class Param1, class To> friend class FvDynamicAsynCallback2;\
	template <class To, void (To::*CallbackFunc)()> friend class FvFrameEndCallback0;\
	template <class To, class Param0, void (To::*CallbackFunc)(const Param0&)> friend class FvFrameEndCallback1;

//+-----------------------------------------------------------------------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------------------------------------------------------------------
namespace Callback_Demo
{
	class Param: public FvReferenceCount
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
		,m_kNode4(*this)
#pragma warning(pop)
		  {}

	private:
		_CALLBACK_FRIEND_CLASS_;

		void Func0(const FvUInt32 uiEvent, From& kFrom){}
		void Func1(const FvUInt32 uiEvent, From& kFrom, const FvUInt32& uiValue){}
		void Func2(const FvUInt32 uiEvent, From& kFrom, const FvUInt32& uiValue, const float& fValue){}

		void OnFrameEndCallback0(){}
		void OnFrameEndCallback1(const FvSmartPointer<Param>& p){}

		void Func4(const FvUInt32 uiEvent){}

	public:
		FvAsynCallback0<From, To, &To::Func4> m_kNode4;

		FvCallback0<From, To, &To::Func0> m_kNode0;
		FvCallback1<From, FvUInt32, To, &To::Func1> m_kNode1;
		FvCallback2<From, FvUInt32, float, To, &To::Func2> m_kNode2;
		FvFrameEndCallback0<To, &OnFrameEndCallback0> m_kFrameEndNode0;
		FvFrameEndCallback1<To, FvSmartPointer<Param>, &OnFrameEndCallback1> m_kFrameEndNode1;
	};

	static void TestFunc()
	{
		From kFrom;
		To kTo;

		FvCallbackList0<From> kList0;		
		FvCallbackList1<From, FvUInt32> kList1;		
		FvCallbackList2<From, FvUInt32, float> kList2;		

		kList0.Attach(kTo.m_kNode0);
		kList0.Attach(kTo.m_kNode4);
		kList0.Invoke(0, kFrom);
		FvAsynCallbackInterface::UpdateAsynCallbacks();

		kList1.Attach(kTo.m_kNode1);
		kList1.Invoke(0, kFrom, 0);

		kList2.Attach(kTo.m_kNode2);
		kList2.Invoke(0, kFrom, 0, 0);

		kTo.m_kFrameEndNode0.Exec(kTo);
		FvSmartPointer<Param> kParam = new Param;
		kTo.m_kFrameEndNode1.Exec(kTo, kParam);
		FvFrameEndCallbackInterface::UpdateCallbacks();

	}
}

#endif//__FvCallback_H__