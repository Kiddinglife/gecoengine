//{future header message}
#ifndef __FvTimeNodeEx_H__
#define __FvTimeNodeEx_H__

#include "FvFastTimer.h"
#include "FvLogicDebug.h"
#include "FvParamChecker.h"


//+--------------------------------------------------------------------------------------------------------------------------------------------
template <class To>
class FvTimeEventNode1: public FvTimeEventNodeInterface
{
	friend class FvTimeAssisstant;
public:
	typedef void (To::*CallbackFunc)(FvTimeEventNodeInterface& qNode);
	FvTimeEventNode1(To& qTo):m_To(qTo), m_CallbackFunc(NULL)
	{}
	~FvTimeEventNode1(){}

	CallbackFunc GetFunc()const{return m_CallbackFunc;}
	void SetFunc(CallbackFunc pCallbackFunc){m_CallbackFunc = pCallbackFunc;}
	void ClearFunc(){m_CallbackFunc = NULL;}

	virtual void Exce(FvFastTimer& kTimer)
	{
		if(m_CallbackFunc)
		{
			(m_To.*m_CallbackFunc)(*this);
		}
	}
private:
	To& m_To;
	CallbackFunc m_CallbackFunc;
};
template <class To, class Param>
class FvTimeEventNodeEx1: public FvTimeEventNodeInterface
{
	friend class FvTimeAssisstant;
public:
	typedef void (To::*CallbackFunc)(const Param&, FvTimeEventNodeInterface&);
	FvTimeEventNodeEx1(To& qTo):m_To(qTo), m_CallbackFunc(NULL)
	{}
	~FvTimeEventNodeEx1(){}

	CallbackFunc GetFunc()const{return m_CallbackFunc;}
	void SetFunc(CallbackFunc pCallbackFunc){m_CallbackFunc = pCallbackFunc;}
	void ClearFunc(){m_CallbackFunc = NULL;}

	virtual void Exce(FvFastTimer& kTimer)
	{
		if(m_CallbackFunc)
		{
			FvParamChecker::AsynSafeParam<Param>();
			(m_To.*m_CallbackFunc)(m_kParam, *this);
		}
	}
	Param m_kParam;
private:
	To& m_To;
	CallbackFunc m_CallbackFunc;
};
//+--------------------------------------------------------------------------------------------------------------------------------------------
class FvRelocatableTimeNode: public FvTimeEventNodeInterface
{
protected:
	FvRelocatableTimeNode(){}
	~FvRelocatableTimeNode(){}
};

//+--------------------------------------------------------------------------------------------------------------------------------------------
template <class To, void (To::*CallbackFunc)(FvTimeEventNodeInterface&)>
class FvTimeEventNode2: public FvRelocatableTimeNode
{
	friend class FvTimeAssisstant;
public:
	FvTimeEventNode2(To& qTo):m_To(qTo){}
	~FvTimeEventNode2(){}

	virtual void Exce(FvFastTimer& kTimer)
	{
		(m_To.*CallbackFunc)(*this);
	}
private:
	To& m_To;
};

//+--------------------------------------------------------------------------------------------------------------------------------------------
template <class To, class Param, void (To::*CallbackFunc)(const Param&, FvTimeEventNodeInterface&)>
class FvTimeEventNodeEx2: public FvRelocatableTimeNode
{
	friend class FvTimeAssisstant;
public:
	FvTimeEventNodeEx2(To& qTo):m_To(qTo){}
	~FvTimeEventNodeEx2(){}

	virtual void Exce(FvFastTimer& kTimer)
	{
		FvParamChecker::AsynSafeParam<Param>();
		(m_To.*CallbackFunc)(m_kParam, *this);
	}
	Param m_kParam;
private:
	To& m_To;
};

//+--------------------------------------------------------------------------------------------------------------------------------------------
template <class To, void (To::*CallbackFunc)(FvTimeEventNodeInterface&)>
class FvTimeEventNode3: public FvRelocatableTimeNode
{
	friend class FvTimeAssisstant;
public:
	FvTimeEventNode3(To& qTo):m_To(qTo){}
	~FvTimeEventNode3(){}

	void Start(FvFastTimer& kTimer, const FvUInt32 uiCnt, const FvUInt32 uiSpan)
	{
		if(uiCnt > 0)
		{
			m_uiReserveCnt = uiCnt - 1;
			m_uiSpan = uiSpan;
			SetTime(FvUInt64(kTimer.GetTime() + m_uiSpan));
			kTimer.Add(*this);
		}
	}
	FvUInt32 GetReserveCnt()const{return m_uiReserveCnt;}
	FvUInt32 GetSpan()const{return m_uiSpan;}

	bool GetReserveTime(const FvFastTimer& kTimer, FvUInt64& uiReserveTime)const
	{
		if(FvTimeAssisstant::GetReserve(*this, uiReserveTime, kTimer))
		{
			FV_ASSERT(uiReserveTime > 0);
			uiReserveTime += m_uiSpan*m_uiReserveCnt;
			return true;
		}
		else
			return false;
	}
	void SetReserveTime(FvFastTimer& kTimer, const FvUInt32 uiSpan, const FvUInt64 uiReserveTime)
	{
		if(uiSpan)
		{
			m_uiSpan = uiSpan;
			const FvUInt32 uiReserveTimeMod = FvUInt32((uiReserveTime > 0)?(uiReserveTime - 1): 0);
			m_uiReserveCnt = FvUInt32(uiReserveTimeMod/m_uiSpan);
			const FvUInt32 uiReserveSpan = FvUInt32(uiReserveTimeMod%m_uiSpan + 1);
			SetTime(FvUInt64(kTimer.GetTime() + uiReserveSpan));
			kTimer.Add(*this);
		}
	}

private:
	virtual void Exce(FvFastTimer& kTimer)
	{
		(m_To.*CallbackFunc)(*this);
		if(m_uiReserveCnt > 0)
		{
			--m_uiReserveCnt;
			SetTime(FvUInt64(kTimer.GetTime() + m_uiSpan));
			kTimer.Add(*this);
		}
	}
	To& m_To;
	FvUInt32 m_uiSpan;
	FvUInt32 m_uiReserveCnt;
};

//+--------------------------------------------------------------------------------------------------------------------------------------------

template <class To, void (To::*TickCallbackFunc)(FvTimeEventNodeInterface&), void (To::*EndCallbackFunc)(FvTimeEventNodeInterface&)>
class FvTimeEventNode4: public FvRelocatableTimeNode
{
	friend class FvTimeAssisstant;
public:
	FvTimeEventNode4(To& qTo):m_To(qTo){}
	~FvTimeEventNode4(){}

	void Start(FvFastTimer& kTimer, const FvUInt32 uiCnt, const FvUInt32 uiSpan)
	{
		if(uiCnt > 0)
		{
			m_uiReserveCnt = uiCnt - 1;
			m_uiSpan = uiSpan;
			SetTime(FvUInt64(kTimer.GetTime() + m_uiSpan));
			kTimer.Add(*this);
		}
	}
	FvUInt32 GetReserveCnt()const{return m_uiReserveCnt;}
	FvUInt32 GetSpan()const{return m_uiSpan;}

	bool GetReserveTime(const FvFastTimer& kTimer, FvUInt64& uiReserveTime)const
	{
		if(FvTimeAssisstant::GetReserve(*this, uiReserveTime, kTimer))
		{
			FV_ASSERT(uiReserveTime > 0);
			uiReserveTime += m_uiSpan*m_uiReserveCnt;
			return true;
		}
		else
			return false;
	}
	void SetReserveTime(FvFastTimer& kTimer, const FvUInt32 uiSpan, const FvUInt64 uiReserveTime)
	{
		if(uiSpan)
		{
			m_uiSpan = uiSpan;
			const FvUInt32 uiReserveTimeMod = FvUInt32((uiReserveTime > 0)?(uiReserveTime - 1): 0);
			m_uiReserveCnt = FvUInt32(uiReserveTimeMod/m_uiSpan);
			const FvUInt32 uiReserveSpan = FvUInt32(uiReserveTimeMod%m_uiSpan + 1);
			SetTime(FvUInt64(kTimer.GetTime() + uiReserveSpan));
			kTimer.Add(*this);
		}
	}
private:
	virtual void Exce(FvFastTimer& kTimer)
	{
		if(m_uiReserveCnt == 0)
		{
			(m_To.*EndCallbackFunc)(*this);
		}
		else
		{
			(m_To.*TickCallbackFunc)(*this);
			--m_uiReserveCnt;
			SetTime(FvUInt64(kTimer.GetTime() + m_uiSpan));
			kTimer.Add(*this);
		}
	}
	To& m_To;
	FvUInt32 m_uiSpan;
	FvUInt32 m_uiReserveCnt;
};

#define _FRIEND_CLASS_TimeNodeEvent_ \
	template <class To> friend class FvTimeEventNode1;\
	template <class To, class Param> friend class FvTimeEventNodeEx1;\
	template <class To, void (To::*CallbackFunc)(FvTimeEventNodeInterface&)> friend class FvTimeEventNode2;\
	template <class To, class Param, void (To::*CallbackFunc)(const Param&, FvTimeEventNodeInterface&)> friend class FvTimeEventNodeEx2;\
	template <class To, void (To::*CallbackFunc)(FvTimeEventNodeInterface&)> friend class FvTimeEventNode3;\
	template <class To, void (To::*TickCallbackFunc)(FvTimeEventNodeInterface&), void (To::*EndCallbackFunc)(FvTimeEventNodeInterface&)> friend class FvTimeEventNode4;



//+--------------------------------------------------------------------------------------------------------------------------------------------
//+--------------------------------------------------------------------------------------------------------------------------------------------
///<²âÊÔÓÃÀý>
namespace FvTimeEvent_Demo
{
	class _Class
	{
	public:
		_Class()
			:
#pragma warning(push)
#pragma warning(disable: 4355)
		m_kNode1(*this)
			,m_kNodeEx1(*this)
			,m_kNode2(*this)
			,m_kNodeEx2(*this)
			,m_kNode3(*this)
			,m_kNode4(*this)
#pragma warning(pop)
		{}
		void OnTime1(FvTimeEventNodeInterface& node)
		{
			FvLogBus::CritOk("Time1[%d]", FvUInt32(node.GetTime()));
		}
		void OnTimeEx1(const float& kParam, FvTimeEventNodeInterface& node)
		{
			FvLogBus::CritOk("TimeEx1[%d]", FvUInt32(node.GetTime()));
		}
		void OnTime2(FvTimeEventNodeInterface& node)
		{
			FvLogBus::CritOk("Time2[%d]", FvUInt32(node.GetTime()));
		}
		void OnTimeEx2(const float& kParam, FvTimeEventNodeInterface& node)
		{
			FvLogBus::CritOk("TimeEx2[%d]", FvUInt32(node.GetTime()));
		}
		void OnTime3(FvTimeEventNodeInterface& node)
		{
			FvLogBus::CritOk("Time3[%d]", FvUInt32(node.GetTime()));
		}
		void OnTime41(FvTimeEventNodeInterface& node)
		{
			FvLogBus::CritOk("OnTime41[%d]", FvUInt32(node.GetTime()));
		}
		void OnTime42(FvTimeEventNodeInterface& node)
		{
			FvLogBus::CritOk("OnTime42[%d]", FvUInt32(node.GetTime()));
		}
		FvTimeEventNode1<_Class> m_kNode1;
		FvTimeEventNodeEx1<_Class, float> m_kNodeEx1;
		FvTimeEventNode2<_Class, &OnTime2> m_kNode2;
		FvTimeEventNodeEx2<_Class, float, &OnTimeEx2> m_kNodeEx2;
		FvTimeEventNode3<_Class, &OnTime3> m_kNode3;
		FvTimeEventNode4<_Class, &OnTime41, &OnTime42> m_kNode4;
	};

	static void Test()
	{
		FvFastTimer timer;
		FvTime64 uiTime = 0;
		timer.Start(0);

		uiTime = 10;
		timer.Update(uiTime);

		//
		_Class kLister;
		const FvUInt32 uiSpan = 10;
		kLister.m_kNode3.Start(timer, 3, uiSpan);

		FvTime64 uiReserveTime = 0;
		if(kLister.m_kNode3.GetReserveTime(timer, uiReserveTime))
		{
			kLister.m_kNode3.SetReserveTime(timer, uiSpan, uiReserveTime);
		}

		uiTime = 29;
		timer.Update(uiTime);

		uiReserveTime = 0;
		if(kLister.m_kNode3.GetReserveTime(timer, uiReserveTime))
		{
			kLister.m_kNode3.SetReserveTime(timer, uiSpan, uiReserveTime);
		}

		uiTime = 30;
		timer.Update(uiTime);

		uiReserveTime = 0;
		if(kLister.m_kNode3.GetReserveTime(timer, uiReserveTime))
		{
			kLister.m_kNode3.SetReserveTime(timer, uiSpan, uiReserveTime);
		}


		uiTime = 31;
		timer.Update(uiTime);

		uiReserveTime = 0;
		if(kLister.m_kNode3.GetReserveTime(timer, uiReserveTime))
		{
			kLister.m_kNode3.SetReserveTime(timer, uiSpan, uiReserveTime);
		}

		kLister.m_kNode4.Start(timer, 3, uiSpan);
		uiTime = 200;
		timer.Update(uiTime);

		//
		uiTime = 401;
		while(uiTime < 3101)
		{
			timer.Update(uiTime - 200);
			++uiTime;

			kLister.m_kNode1.SetTime(uiTime);	
			kLister.m_kNode1.SetFunc(&(_Class::OnTime1));
			timer.Add(kLister.m_kNode1);

			kLister.m_kNodeEx1.SetTime(uiTime);
			kLister.m_kNodeEx1.m_kParam = 1.0f;
			kLister.m_kNodeEx1.SetFunc(&(_Class::OnTimeEx1));
			timer.Add(kLister.m_kNodeEx1);

			kLister.m_kNode2.SetTime(uiTime);	
			timer.Add(kLister.m_kNode2);

			kLister.m_kNodeEx2.m_kParam = 2.0f;
			kLister.m_kNodeEx2.SetTime(uiTime);	
			timer.Add(kLister.m_kNodeEx2);
		}

	}
}
//+-----------------------------------------------------------------------------------------


#endif //__FvTimeNodeEx_H__