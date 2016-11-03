//{future header message}
#ifndef __FvSlot_H__
#define  __FvSlot_H__

#include "FvLogicCommonDefine.h"


#include "FvReferencePtr.h"


#include <vector>
#include <list>
#include "FvLogBus.h"

//class SlotObj
//{
//public:
//	virtual void SetSlot(const FvUInt16 uiSlot) = 0;
//};


template <class TObj>
class FvTSlotManager
{
public:
	typedef std::vector<FvReferencePtr<TObj>> _List;

	bool Attach(const FvUInt16 uiSlot, TObj& kObj);
	bool Attach(TObj& kObj);
	TObj* Detach(const FvUInt16 uiSlot);

	bool IsEmpty()const;
	bool IsFull()const;

	bool IsEmptySlot(const FvUInt16 uiSlot)const;
	TObj* GetInSlot(const FvUInt16 uiSlot)const;	

	void SetSlotSize(const FvUInt16 uiSize);
	FvUInt16 SlotSize()const;

	bool GetEmptySlot(FvUInt16& uiSlot)const;
	FvUInt16 GetEmptySlotCnt()const;
	FvUInt16 GetFillSlotCnt()const;

	template<class TKey, TKey (TObj::*func)()const>
	TObj* Get(const TKey uiKey)const
	{
		for(_List::const_iterator iter = m_ObjList.begin(); iter != m_ObjList.end(); ++iter)
		{
			TObj* pkObj = (*iter);
			if(pkObj == NULL) 
				continue;
			const TKey uiObjKey = ((*pkObj).*func)();
			if(uiObjKey == uiKey)
			{
				return pkObj;
			}
		}
		return NULL;
	}

	template<void (*func)(TObj*&)>
	void ForEach()
	{
		for (_List::iterator iter = m_ObjList.begin(); iter != m_ObjList.end(); ++iter)
		{
			TObj* pkObj = (*iter);
			if(pkObj)
			{
				func(pkObj);
			}
		}	
	}

private:

	_List m_ObjList;
};

//+------------------------------------------------------------------------------------------------------------------------
template <class TObj>
bool FvTSlotManager<TObj>::Attach(const FvUInt16 uiSlot, TObj& kObj)
{
	if(uiSlot >= m_ObjList.size())
	{
		FvLogBus::CritOk("FvSlotVector::Attach uiSlot[%d]栏位越界", uiSlot);
		return false;
	}
	if(m_ObjList[uiSlot])
	{
		FvLogBus::CritOk("FvSlotVector::Attach uiSlot[%d]栏位非空", uiSlot);
		return false;
	}
	m_ObjList[uiSlot] = &kObj;
	kObj.SetSlot(uiSlot);
	return true;
}
//+------------------------------------------------------------------------------------------------------------------------
template <class TObj>
bool FvTSlotManager<TObj>::Attach(TObj& kObj)
{
	FvUInt16 uiSlot = 0;
	if(GetEmptySlot(uiSlot) == false)
	{
		FvLogBus::CritOk("FvSlotVector::Attach 没有空闲栏位");
		return false;
	}
	kObj.SetSlot(uiSlot);
	m_ObjList[uiSlot] = &kObj;
	return true;
}
//+------------------------------------------------------------------------------------------------------------------------
template <class TObj>
TObj* FvTSlotManager<TObj>::Detach(const FvUInt16 uiSlot)
{
	if(uiSlot >= m_ObjList.size())
	{
		return NULL;
	}
	TObj* pkObj = m_ObjList[uiSlot];
	if(pkObj == NULL)
	{
		FvLogBus::CritOk("FvSlotVector::Detach 指定物品不存在");
		return NULL;
	}
	m_ObjList[uiSlot] = NULL;
	return pkObj;
}
//+------------------------------------------------------------------------------------------------------------------------
template <class TObj>
bool FvTSlotManager<TObj>::IsEmptySlot(const FvUInt16 uiSlot)const
{
	if(uiSlot >= m_ObjList.size())
	{
		return false;
	}
	return (m_ObjList[uiSlot] == NULL);
}
//+------------------------------------------------------------------------------------------------------------------------
template <class TObj>
bool FvTSlotManager<TObj>::IsEmpty()const
{
	for (_List::const_iterator iter = m_ObjList.begin(); iter != m_ObjList.end(); ++iter)
	{
		const TObj* pkObj = (*iter);
		if(pkObj)
		{
			return false;
		}
	}
	return true;
}
//+------------------------------------------------------------------------------------------------------------------------
template <class TObj>
bool FvTSlotManager<TObj>::IsFull()const
{
	for (_List::const_iterator iter = m_ObjList.begin(); iter != m_ObjList.end(); ++iter)
	{
		const TObj* pkObj = (*iter);
		if(pkObj == NULL)
		{
			return false;
		}
	}
	return true;	
}
//+------------------------------------------------------------------------------------------------------------------------
template <class TObj>
TObj* FvTSlotManager<TObj>::GetInSlot(const FvUInt16 uiSlot)const
{
	if(uiSlot < m_ObjList.size())
	{
		return m_ObjList[uiSlot];
	}
	else
	{
		return NULL;
	}
}
//+------------------------------------------------------------------------------------------------------------------------
template <class TObj>
void FvTSlotManager<TObj>::SetSlotSize(const FvUInt16 uiSize)
{
	if(GetFillSlotCnt() != 0)
	{
		FvLogBus::Warning("FvSlotVector::SetSlotSize 有item的情况下不能被初始化");
		return;
	}
	m_ObjList.resize(uiSize, NULL);
}
//+------------------------------------------------------------------------------------------------------------------------
template <class TObj>
FvUInt16 FvTSlotManager<TObj>::SlotSize()const
{
	return m_ObjList.size();
}
//+------------------------------------------------------------------------------------------------------------------------
template <class TObj>
bool FvTSlotManager<TObj>::GetEmptySlot(FvUInt16& uiSlot)const
{
	FvUInt16 uiEmptySlot = 0;
	for (_List::const_iterator iter = m_ObjList.begin(); iter != m_ObjList.end(); ++iter, ++uiEmptySlot)
	{
		const TObj* pkObj = (*iter);
		if(pkObj == NULL)
		{
			uiSlot = uiEmptySlot;
			return true;
		}
	}
	return false;
}	
//+------------------------------------------------------------------------------------------------------------------------
template <class TObj>
FvUInt16 FvTSlotManager<TObj>::GetEmptySlotCnt()const
{
	FvUInt16 uiCnt = 0;
	for (_List::const_iterator iter = m_ObjList.begin(); iter != m_ObjList.end(); ++iter)
	{
		const TObj* pkObj = (*iter);
		if(pkObj == NULL)
		{
			++uiCnt;
		}
	}
	return uiCnt;
}
//+------------------------------------------------------------------------------------------------------------------------
template <class TObj>
FvUInt16 FvTSlotManager<TObj>::GetFillSlotCnt()const
{
	FvUInt16 uiCnt = 0;
	for (_List::const_iterator iter = m_ObjList.begin(); iter != m_ObjList.end(); ++iter)
	{
		const TObj* pkObj = (*iter);
		if(pkObj)
		{
			++uiCnt;
		}
	}
	return uiCnt;
}



namespace SLot_Demo
{
	class TestSlotObj
	{
	public:
		TestSlotObj():m_uiSlot(0){}
		~TestSlotObj(){}

		virtual FvUInt16 GetSlot()const{return m_uiSlot;}
		virtual void SetSlot(const FvUInt16 uiSlot){m_uiSlot = uiSlot;}

		FvUInt16 m_uiSlot;
	};

	static void Test()
	{
		TestSlotObj* pSlot = new TestSlotObj();
		//FvTSlotManager<TestSlotObj, FvSlotList> testFvSlotList;
		FvTSlotManager<TestSlotObj> testFvSlotVector;

		delete pSlot;
		//testFvSlotList.Attach(*pSlot);
		//testFvSlotList.SwapSlot(1,2);
		//testFvSlotList.DestroyAllSlot();
	}
}

#endif //__FvSlot_H__

