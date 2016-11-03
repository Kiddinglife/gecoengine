//{future header message}
#ifndef __FvCreator_H__
#define __FvCreator_H__


#include <map>
#include "FvLogicDebug.h"

template<class TObj, class TKey, class TParam = void>
class FvCreator
{
public:
	FvCreator(void){}
	~FvCreator(void){}

	typedef TObj* (*CreateFunc)(TParam);
	typedef std::map<TKey, CreateFunc> FuncList;

	void Add(const TKey& key, CreateFunc func)
	{
		FuncList::iterator iter = m_FuncList.find(key);
		if(iter == m_FuncList.end())
		{
			m_FuncList[key] = func;
		}
		else
		{
			FV_ASSERT_WARNING(0 && "重复插入函数");
		}
	}

	TObj* Create(const TKey& key, TParam kParam)
	{
		FuncList::iterator iter = m_FuncList.find(key);
		if(iter != m_FuncList.end())
		{
			CreateFunc pFunc = (*iter).second;
			FV_ASSERT_ERROR(pFunc);
			return (*pFunc)(kParam);
		}
		else
			return NULL;
	}

private:
	FuncList m_FuncList;

};

template<class TObj, class TKey>
class FvCreator<TObj, TKey>
{
public:
	FvCreator(void){}
	~FvCreator(void){}

	typedef TObj* (*CreateFunc)();
	typedef std::map<TKey, CreateFunc> FuncList;

	void Add(const TKey& key, CreateFunc func)
	{
		FuncList::iterator iter = m_FuncList.find(key);
		if(iter == m_FuncList.end())
		{
			m_FuncList[key] = func;
		}
		else
		{
			FV_ASSERT_WARNING(0 && "重复插入函数");
		}
	}
	TObj* Create(const TKey& key)
	{
		FuncList::iterator iter = m_FuncList.find(key);
		if(iter != m_FuncList.end())
		{
			CreateFunc pFunc = (*iter).second;
			FV_ASSERT_ERROR(pFunc);
			return (*pFunc)();
		}
		else
			return NULL;
	}
private:
	FuncList m_FuncList;
};

#endif //__FvCreator_H__