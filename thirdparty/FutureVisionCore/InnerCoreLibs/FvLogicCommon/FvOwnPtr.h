//{future header message}
#ifndef __FvOwnPtr_H__
#define __FvOwnPtr_H__

#include "FvLogicCommonDefine.h"
#include "FvMemoryNode.h"

template <class T> class FvOwnPtr
{
public:
	//+--------------------------------------------------
	FvOwnPtr(T* pObject = (T*) 0);
	~FvOwnPtr();

	//+--------------------------------------------------
	operator T*() const;
	T& operator*() const;
	T* operator->() const;

	FvOwnPtr& operator=(T* pObject);


	FvOwnPtr& operator=(const FvOwnPtr& ptr);
	FvOwnPtr(const FvOwnPtr& ptr);

	bool IsNULL()const{return m_pObject == NULL;}

private:

	//+--------------------------------------------------

	//+--------------------------------------------------
	bool operator==(T* pObject) const;
	bool operator!=(T* pObject) const;
	bool operator==(const FvOwnPtr& ptr) const;
	bool operator!=(const FvOwnPtr& ptr) const;

protected:
	//+--------------------------------------------------
	T* m_pObject;
};



//+-----------------------------------------------------------------------------------------------------------------

template <class T>
inline FvOwnPtr<T>::FvOwnPtr(T* pObject)
{
	m_pObject = pObject;
}
//+-----------------------------------------------------------------------------------------------------------------
template <class T>
inline FvOwnPtr<T>::FvOwnPtr(const FvOwnPtr& ptr)
{
	m_pObject = ptr.m_pObject;
	const_cast<FvOwnPtr&>(ptr).m_pObject = NULL;
}
//+-----------------------------------------------------------------------------------------------------------------
template <class T>
inline FvOwnPtr<T>::~FvOwnPtr()
{
	if (m_pObject)
	{
		delete m_pObject;
	}
}
//+-----------------------------------------------------------------------------------------------------------------
template <class T>
inline FvOwnPtr<T>::operator T*() const
{
	return m_pObject;
}
//+-----------------------------------------------------------------------------------------------------------------
template <class T>
inline T& FvOwnPtr<T>::operator*() const
{
	return *m_pObject;
}
//+-----------------------------------------------------------------------------------------------------------------
template <class T>
inline T* FvOwnPtr<T>::operator->() const
{
	return m_pObject;
}
//+-----------------------------------------------------------------------------------------------------------------

//+-----------------------------------------------------------------------------------------------------------------
template <class T>
inline FvOwnPtr<T>& FvOwnPtr<T>::operator=(T* pObject)
{
	if(m_pObject)
	{
		delete m_pObject;
	}
	m_pObject = pObject;
	return *this;
}
template <class T>
inline FvOwnPtr<T>& FvOwnPtr<T>::operator=(const FvOwnPtr& ptr)
{
	if(m_pObject)
	{
		delete m_pObject;
	}
	m_pObject = ptr.m_pObject;
	const_cast<FvOwnPtr&>(ptr).m_pObject = NULL;
	return *this;
}



#endif //__FvOwnPtr_H__
