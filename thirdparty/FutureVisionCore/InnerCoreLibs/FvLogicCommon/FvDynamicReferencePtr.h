//{future header message}
#ifndef __FvMultiReferencePtr_H__
#define __FvMultiReferencePtr_H__

#include "FvReferencePtr.h"
#include <FvKernel.h>

 class FV_LOGIC_COMMON_API FvDynamicReferencePtr
{
public:
	//+--------------------------------------------------
	FvDynamicReferencePtr():m_uiType(NULL_TYPE){}
	template<class T>
	FvDynamicReferencePtr(T* pObject);
	template<class T>
	FvDynamicReferencePtr(const FvReferencePtr<T>& ptr);
	FvDynamicReferencePtr(const FvDynamicReferencePtr& ptr);

	~FvDynamicReferencePtr(){}

	//+--------------------------------------------------
	FvDynamicReferencePtr& operator=(const FvDynamicReferencePtr& ptr);
	template<class T> FvDynamicReferencePtr& operator=(T* pObject);
	template<class T> FvDynamicReferencePtr& operator=(const FvReferencePtr<T>& ptr);

	//+--------------------------------------------------
	bool operator==(const FvDynamicReferencePtr& ptr)const;
	template<class T> bool operator==(T* pObject) const;
	template<class T> bool operator==(const FvReferencePtr<T>& ptr) const;

	bool operator!=(const FvDynamicReferencePtr& ptr)const;
	template<class T> bool operator!=(T* pObject) const;
	template<class T> bool operator!=(const FvReferencePtr<T>& ptr) const;

	template<class T> bool IsType()const;
	template<class T> T* GetObj()const;
	void Clear();
	bool IsClear()const;

protected:

	template<class T> void _SetObj(T* pkObj);
	template<class T> static FvUInt32 TYPE();
	static const FvUInt32 NULL_TYPE = 0XFFFFFFFF;
	//+--------------------------------------------------

	FvDoubleLinkNode2<void*> m_kNode;
	FvUInt32 m_uiType;


};
//+-----------------------------------------------------------------------------------------------------------------

template <class T>
FvDynamicReferencePtr::FvDynamicReferencePtr(T* pkObj)
{
	m_uiType = TYPE<T>();
	if(pkObj)
	{
		m_kNode.m_Content = (void*)pkObj;
		pkObj->REFERENCE_PTR_LIST._AddReference(m_kNode);
	}
}
//+-----------------------------------------------------------------------------------------------------------------

template <class T>
FvDynamicReferencePtr::FvDynamicReferencePtr(const FvReferencePtr<T>& ptr)
{
	m_uiType = TYPE<T>();
	if(ptr._GetObj())
	{
		m_kNode.m_Content = ptr._GetObj();
		ptr._GetObj()->REFERENCE_PTR_LIST._AddReference(m_kNode);
	}
}
//+-----------------------------------------------------------------------------------------------------------------
template <class T>
FvDynamicReferencePtr& FvDynamicReferencePtr::operator=(const FvReferencePtr<T>& ptr)
{
	if (GetObj<T>() != ptr._GetObj())
	{
		_SetObj(ptr._GetObj());
	}
	return *this;
}
//+-----------------------------------------------------------------------------------------------------------------
template <class T>
FvDynamicReferencePtr& FvDynamicReferencePtr::operator=(T* pObject)
{
	_SetObj(pObject);
	return *this;
}
//+-----------------------------------------------------------------------------------------------------------------
template <class T>
bool FvDynamicReferencePtr::operator==(T* pObject) const
{
	return (GetObj<T>() == pObject);
}
//+-----------------------------------------------------------------------------------------------------------------
template <class T>
bool FvDynamicReferencePtr::operator!=(T* pObject) const
{
	return (GetObj()<T> != pObject);
}
//+-----------------------------------------------------------------------------------------------------------------
template <class T>
bool FvDynamicReferencePtr::operator==(const FvReferencePtr<T>& ptr) const
{
	return (GetObj<T>() == ptr._GetObj());
}
//+-----------------------------------------------------------------------------------------------------------------
template <class T>
bool FvDynamicReferencePtr::operator!=(const FvReferencePtr<T>& ptr) const
{
	return (GetObj<T>() != ptr._GetObj());
}

template<class T> 
bool FvDynamicReferencePtr::IsType()const
{
	return m_uiType == TYPE<T>();
}

template<class T>
T* FvDynamicReferencePtr::GetObj()const
{
	if(m_uiType == TYPE<T>())
	{
		return (T*)(m_kNode.m_Content);
	}
	else
	{
		return NULL;
	}
}
template<class T>
void FvDynamicReferencePtr::_SetObj(T* pkObj)
{
	m_kNode.Detach();
	if(pkObj)
	{
		m_kNode.m_Content = pkObj;
		pkObj->REFERENCE_PTR_LIST._AddReference(m_kNode);
	}
	else
	{
		m_kNode.m_Content = NULL;
	}
	m_uiType = TYPE<T>();
}

//+-----------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------
///<²âÊÔÓÃÀý>
namespace MultiReference_Demo
{
	class FvRefTest1
	{
	public:
		FV_REFERENCE_HEAD;
	};


	static void Test()
	{
		FvRefTest1* pNew = new FvRefTest1();
		FvRefTest1* pNewErr = pNew;
		FvDynamicReferencePtr rpRefTest_1 = pNew;
		FvDynamicReferencePtr rpRefTest_2 = rpRefTest_1;
		FvDynamicReferencePtr rpRefTest_3 = pNew;
		FV_ASSERT(rpRefTest_3.IsType<FvRefTest1>());
		FV_ASSERT(rpRefTest_3 == pNew);
		rpRefTest_3.Clear();
		delete pNew;
		FV_ASSERT(rpRefTest_1.IsClear());
		FV_ASSERT(rpRefTest_2.IsClear());
		FV_ASSERT(pNewErr != NULL);
	};
}

#endif //__FvMultiReferencePtr_H__