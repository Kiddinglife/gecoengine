//{future header message}
#ifndef __FvSimpleVector_H__
#define __FvSimpleVector_H__

#include <FvKernel.h>
#include <FvDebug.h>

template<class T>
class FvSimpleVector
{
public:
	FvSimpleVector(void)
		:m_opArray(0), m_uiSize(0)
	{}
	~FvSimpleVector(void)
	{
		Clear();
	}

	void Resize(const FvUInt32 uiSize)
	{
		if(uiSize == 0) return;
		if(m_uiSize != 0) return;//! 不能被初始化两次,否则就去用Vector
		FV_ASSERT(m_opArray == NULL);
		m_opArray = new T[uiSize];
		m_uiSize = uiSize;
	}
	T* Get(const FvUInt32 uiSlot)const
	{
		if(uiSlot < m_uiSize)
		{
			return m_opArray[uiSlot];
		}
		else
			return NULL;
	}

	const T& operator[](const FvUInt32 uiSlot) const
	{
		FV_ASSERT(uiSlot < m_uiSize);
		return m_opArray[uiSlot];
	}
	T& operator[](const FvUInt32 uiSlot)
	{
		FV_ASSERT(uiSlot < m_uiSize);
		return m_opArray[uiSlot];
	}

	FvUInt32 Size()const{return m_uiSize;}

	void Clear()
	{
		if(m_uiSize)
		{
			m_uiSize = 0;
			if(m_opArray)
			{
				delete [](m_opArray);
				m_opArray = NULL;
			}
		}
	}

private:
	
	T* m_opArray;
	FvUInt32 m_uiSize;
};


#endif //__FvSimpleVector_H__
