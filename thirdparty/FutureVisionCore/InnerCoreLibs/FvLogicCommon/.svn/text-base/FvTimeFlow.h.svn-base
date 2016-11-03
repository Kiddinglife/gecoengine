//{future header message}
#ifndef __FvTimeFlow_H__
#define __FvTimeFlow_H__



#include "FvTimeNodeEx.h"

template <class T>
class FvTimeFlow
{
	typedef FvTimeEventNodeEx1<T, FvUInt32> _Node;
	typedef void (T::*CallbackFunc)(const FvUInt32&, FvTimeEventNodeInterface&);

public:
	//+------------------------------------------------------------------------------------------------------------
	FvTimeFlow();
	~FvTimeFlow();

	//+------------------------------------------------------------------------------------------------------------
	void Stop();
	void Init(FvFastTimer& qTimer, const FvUInt32 uiSize, T& qT);
	void SetFrequence(const float frq); 
	void SetTimeLapse(const FvUInt32 uiLapseIdx, const FvUInt32 uiLapseTime, CallbackFunc call);
	bool IsAttach(const FvUInt32 uiLapseIdx)const;
	bool IsAttachAll()const;
	bool IsAttachAny()const;
	float GetFrequence()const{return m_fFrequence;}
	FvUInt32 GetReserveTime(const FvUInt32 uiLapseIdx)const;
	void DelayAll(const FvUInt32 uiDeltaTime);

private:

	FV_DEFINE_COPY_CONSTRUCTOR(FvTimeFlow);
	FV_DEFINE_COPY(FvTimeFlow);
	FV_DEFINE_COMPARE(FvTimeFlow);

	//+------------------------------------------------------------------------------------------------------------
	void _End();
	void _SetFreq(const float fOldFrq, const float fNewFrq);
	void _SetTime(const FvUInt64 uiLapseTimeMod, _Node& kNode, CallbackFunc call);
	void _SetTime(const FvUInt64 uiLapseTimeMod, _Node& kNode);

	//+------------------------------------------------------------------------------------------------------------
	FvFastTimer* m_rpTimer;
	float m_fFrequence;
	FvUInt32 m_uiSize;
	_Node** m_opTimeNode;

	FV_MEM_TRACKER(FvTimeFlow);

private:
	//! 只声明不定义
	void SetTimeLapse(const FvUInt32 uiLapseIdx, const float fLapseTime, CallbackFunc call);//! 
};


//+------------------------------------------------------------------------------------------------------------
template <class T>
FvTimeFlow<T>::FvTimeFlow()
:m_fFrequence(1.0f)
,m_rpTimer(NULL)
,m_uiSize(0)
,m_opTimeNode(NULL)
{
}

//+------------------------------------------------------------------------------------------------------------
template <class T>
FvTimeFlow<T>::~FvTimeFlow()
{
	if(m_opTimeNode)
	{
		for(FvUInt32 uiIdx = 0; uiIdx < m_uiSize; ++uiIdx)
		{
			FV_SAFE_DELETE(m_opTimeNode[uiIdx]);
		}
		FV_SAFE_DELETE_ARRAY(m_opTimeNode);
	}
}

//+------------------------------------------------------------------------------------------------------------
template <class T>
void FvTimeFlow<T>::Init(FvFastTimer& qTimer, const FvUInt32 uiSize, T& qT)
{
	m_rpTimer = &qTimer;
	m_uiSize = uiSize;
	m_opTimeNode = new _Node*[m_uiSize];
	FV_ASSERT(m_opTimeNode);
	for(FvUInt32 uiIdx = 0; uiIdx < m_uiSize; ++uiIdx)
	{
		m_opTimeNode[uiIdx] = new _Node(qT);
		FV_ASSERT(m_opTimeNode[uiIdx]);
	}
}

//+------------------------------------------------------------------------------------------------------------
template<class T>
void FvTimeFlow<T>::Stop()
{
	_End();
}

//+------------------------------------------------------------------------------------------------------------
template<class T>
void FvTimeFlow<T>::SetFrequence(const float frq)
{
	_SetFreq(m_fFrequence, frq);
	m_fFrequence = frq;
}

//+------------------------------------------------------------------------------------------------------------
template<class T>
void FvTimeFlow<T>::SetTimeLapse(const FvUInt32 uiLapseIdx, const FvUInt32 uiLapseTime, CallbackFunc call)
{
	FV_ASSERT(m_opTimeNode && (uiLapseIdx < m_uiSize));
	FV_CHECK_RETURN(m_opTimeNode && (uiLapseIdx < m_uiSize));
	FV_ASSERT(m_opTimeNode[uiLapseIdx]);
	const FvUInt64 uiLapseTimeMod = FvUInt64(uiLapseTime/m_fFrequence);
	m_opTimeNode[uiLapseIdx]->m_kParam = uiLapseIdx;
	_SetTime(uiLapseTimeMod, *(m_opTimeNode[uiLapseIdx]), call);
}

template<class T>
bool FvTimeFlow<T>::IsAttach(const FvUInt32 uiLapseIdx)const
{
	FV_ASSERT(m_opTimeNode && (uiLapseIdx < m_uiSize));
	FV_CHECK_RETURN_FALSE(m_opTimeNode && (uiLapseIdx < m_uiSize));
	FV_ASSERT(m_opTimeNode[uiLapseIdx]);
	return m_opTimeNode[uiLapseIdx]->IsAttach();
}

template<class T>
bool FvTimeFlow<T>::IsAttachAll()const
{
	FV_ASSERT(m_opTimeNode);
	FV_CHECK_RETURN_FALSE(m_opTimeNode);
	for(FvUInt32 uiIdx = 0; uiIdx < m_uiSize; ++uiIdx)
	{
		FV_ASSERT(m_opTimeNode[uiIdx]);
		if(m_opTimeNode[uiIdx]->IsAttach() == false)//! 已经有回调了
		{
			return false;
		}
	}
	return true;
}

template<class T>
bool FvTimeFlow<T>::IsAttachAny()const
{
	FV_ASSERT(m_opTimeNode);
	FV_CHECK_RETURN_FALSE(m_opTimeNode);
	for(FvUInt32 uiIdx = 0; uiIdx < m_uiSize; ++uiIdx)
	{
		FV_ASSERT(m_opTimeNode[uiIdx]);
		if(m_opTimeNode[uiIdx]->IsAttach() == true)//! 已经有回调了
		{
			return true;
		}
	}
	return false;
}
template<class T>
FvUInt32 FvTimeFlow<T>::GetReserveTime(const FvUInt32 uiLapseIdx)const
{
	FV_ASSERT(m_rpTimer && m_opTimeNode);
	if(m_rpTimer == NULL || m_opTimeNode == NULL)
	{
		return 0;
	}
	if(uiLapseIdx < m_uiSize)
	{
		FV_ASSERT(m_opTimeNode[uiLapseIdx]);
		return FvUInt32(m_opTimeNode[uiLapseIdx]->GetReserveDuration(*m_rpTimer));
	}
	else
		return 0;
}
template<class T>
void FvTimeFlow<T>::DelayAll(const FvUInt32 uiDeltaTime)
{
	FV_ASSERT(m_rpTimer && m_opTimeNode);
	FV_CHECK_RETURN(m_rpTimer && m_opTimeNode);
	const FvUInt64 uiCurrentTime  = m_rpTimer->GetTime();
	for(FvUInt32 uiIdx = 0; uiIdx < m_uiSize; ++uiIdx)
	{
		FV_ASSERT(m_opTimeNode[uiIdx]);
		if(m_opTimeNode[uiIdx]->IsAttach())//! 已经没有被回调了
		{
			const FvInt64 uiDelta = FvInt64(m_opTimeNode[uiIdx]->GetTime()) - FvInt64(uiCurrentTime);
			if(uiDelta >= 0)
			{
				_SetTime(uiDelta + uiDeltaTime, *(m_opTimeNode[uiIdx]));
			}
			else
			{
				_SetTime(uiDeltaTime, *(m_opTimeNode[uiIdx]));
			}
		}
	}
}

//+------------------------------------------------------------------------------------------------------------
template<class T>
void FvTimeFlow<T>::_End()
{
	FV_CHECK_RETURN(m_opTimeNode);
	for(FvUInt32 uiIdx = 0; uiIdx < m_uiSize; ++uiIdx)
	{
		FV_ASSERT(m_opTimeNode[uiIdx]);
		m_opTimeNode[uiIdx]->Detach();
	}
}

//+------------------------------------------------------------------------------------------------------------
template<class T>
void FvTimeFlow<T>::_SetFreq(const float fOldFrq, const float fNewFrq)
{
	FV_CHECK_RETURN(m_rpTimer && m_opTimeNode);
	const FvUInt64 uiCurrentTime  = m_rpTimer->GetTime();
	for(FvUInt32 uiIdx = 0; uiIdx < m_uiSize; ++uiIdx)
	{
		FV_ASSERT(m_opTimeNode[uiIdx]);
		if(m_opTimeNode[uiIdx]->IsAttach())//! 已经没有被回调了
		{
			const FvInt64 uiDelta = m_opTimeNode[uiIdx]->GetTime() - uiCurrentTime;
			const FvUInt64 uiDeltaTimeOldMod = (uiDelta > 0)? uiDelta: 0;
			const FvUInt64 uiDeltaTime = FvUInt64(uiDeltaTimeOldMod*fOldFrq);
			const FvUInt64 uiDeltaTimeNewMod = FvUInt64(uiDeltaTime/fNewFrq);
			_SetTime(uiDeltaTimeNewMod, *(m_opTimeNode[uiIdx]));
		}
	}
}

//+------------------------------------------------------------------------------------------------------------
template<class T>
void FvTimeFlow<T>::_SetTime(const FvUInt64 uiLapseTimeMod, _Node& kNode, CallbackFunc call)
{
	FV_ASSERT(m_rpTimer);
	if(m_rpTimer)
	{
		kNode.SetTime(m_rpTimer->GetTime() + uiLapseTimeMod);
		kNode.SetFunc(call);
		m_rpTimer->Add(kNode);
	}
}
template<class T>
void FvTimeFlow<T>::_SetTime(const FvUInt64 uiLapseTimeMod, _Node& kNode)
{
	FV_ASSERT(m_rpTimer);
	if(m_rpTimer)
	{
		kNode.SetTime(m_rpTimer->GetTime() + uiLapseTimeMod);
		m_rpTimer->Add(kNode);
	}
}

//+-----------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------
///<测试用例>
namespace ViTimeFlow_Demo
{
	class _Class
	{
	public:
		void OnTime_1(const FvUInt32& idx, FvTimeEventNodeInterface&){}
		void OnTime_2(const FvUInt32& idx, FvTimeEventNodeInterface&){}
		void OnTime_3(const FvUInt32& idx, FvTimeEventNodeInterface&){}
	};

	static void Test()
	{
		FvFastTimer timer;
		_Class listener;
		FvTimeFlow<_Class> flow;
		flow.Init(timer, 3, listener);
		FvUInt32 time = 0;
		flow.SetTimeLapse(0, time, &_Class::OnTime_1);	
		time += 10;
		flow.SetTimeLapse(1, time, &_Class::OnTime_2);	
		time += 10;
		flow.SetTimeLapse(2, time, &_Class::OnTime_3);	

		flow.SetFrequence(2.0f);//! 设置频率
		const bool attached = flow.IsAttach(1);//! 1号回调是否发生过(true表示还没有发生)
	
		timer.Start(30);
		timer.Update(1);
		timer.Update(2);
		timer.Update(3);
	
	}
}
//+-----------------------------------------------------------------------------------------


#endif//__FvTimeFlow_H__