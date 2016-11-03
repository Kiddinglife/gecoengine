//{future header message}
#ifndef __FvScriptConstValue_H__
#define  __FvScriptConstValue_H__

#include "FvOwnPtr.h"
#include <FvDoubleLink.h>

#include <map>
#include <string>

namespace Private_ScriptConstValue
{
	template<class T>
	class Item 
	{
	public:
		Item(const T& kValue):m_kValue(kValue)
		{}
		T m_kValue;
		FvDoubleLink<T> m_kList;
	};
}

template<class T>
class FvScriptConstValue
{
public:
	FvScriptConstValue(const char* pcName, const T& kValue)
		:m_kValue(kValue)
	{
		FvScriptConstValueList<T>::Attach(pcName, m_kValue);
	}
	const T& Value()const{return m_kValue.m_Content;}
private:
	FvDoubleLinkNode2<T> m_kValue;
	template<class T>friend class FvScriptConstValueList;
};

class FV_LOGIC_COMMON_API ScriptConstValueExport
{
public:
	template<class T>
	static std::map<std::string, FvOwnPtr<Private_ScriptConstValue::Item<T>>>& S_LIST();
};


template<class T>
class FvScriptConstValueList
{
public:
	typedef Private_ScriptConstValue::Item<T> _Item;
	typedef std::map<std::string, FvOwnPtr<_Item>> Map;

	static void AddValue(const char* pcStr, const T& kValue)
	{
		Map& kList =ScriptConstValueExport:: S_LIST<T>();
		Map::const_iterator iter = kList.find(pcStr);
		if(iter != kList.end())
		{
			FvLogBus::CritOk("FvScriptConstValueList修改字段[%s]", pcStr);
			const FvOwnPtr<_Item>& pkItem = (*iter).second;
			FV_ASSERT(pkItem.IsNULL() == false);
			pkItem->m_kValue = kValue;
			_SetValue(pkItem->m_kList, kValue);
		}
		else
		{
			_Item* pkItem = new _Item(kValue);
			pkItem->m_kValue = kValue;
			_SetValue(pkItem->m_kList, kValue);
			kList[pcStr] = pkItem;
		}
	}
	static void Attach(const char* pcStr, FvDoubleLinkNode2<T>& kNode)
	{
		Map& kList =ScriptConstValueExport:: S_LIST<T>();
		Map::const_iterator iter = kList.find(pcStr);
		if(iter != kList.end())
		{
			const FvOwnPtr<_Item>& pkItem = (*iter).second;
			FV_ASSERT(pkItem.IsNULL() == false);
			pkItem->m_kList.PushBack(kNode);
			kNode.m_Content = pkItem->m_kValue;
		}
		else
		{
			FvLogBus::Warning("FvScriptConstValueList字段缺失[%s]", pcStr);
			_Item* pkItem = new _Item(kNode.m_Content);
			pkItem->m_kList.PushBack(kNode);
			kList[pcStr] = pkItem;
		}
	}

private:
	FvScriptConstValueList(void);
	~FvScriptConstValueList(void);

	static void _SetValue(FvDoubleLink<T>& kList, const T& kValue)
	{
		FvDoubleLink<T>::Iterator iter = kList.GetHead();
		while (!kList.IsEnd(iter))
		{
			(*iter).m_Content = kValue;
			FvDoubleLink<T>::Next(iter);
		}
	}
	friend class ScriptConstValueExport;
};



//+---------------------------------------------------------------------------------------------------------------------
namespace ScriptConstValue_Demo
{
	static void Func()
	{
		static FvScriptConstValue<FvUInt32> S_UI_VALUE("整数1", 1);
		FvUInt32 uiValue = S_UI_VALUE.Value();
		static FvScriptConstValue<FvUInt32> S_UI_VALUE_("整数1", 2);
		FvUInt32 uiValue2 = S_UI_VALUE_.Value();
		static FvScriptConstValue<float> S_F_VALUE("浮点数1", -1.0f);
		float fValue = S_F_VALUE.Value();
	}


	static void TEST()
	{
		Func();
		FvScriptConstValueList<FvUInt32>::AddValue("整数1", 100);
		FvScriptConstValueList<FvUInt32>::AddValue("整数2", 200);
		FvScriptConstValueList<float>::AddValue("浮点数1", 1.0f);
		FvScriptConstValueList<float>::AddValue("浮点数2", 2.0f);
		Func();
		FvScriptConstValueList<FvUInt32>::AddValue("整数1", 300);
		FvScriptConstValueList<FvUInt32>::AddValue("整数2", 400);
		FvScriptConstValueList<float>::AddValue("浮点数1", 3.0f);
		FvScriptConstValueList<float>::AddValue("浮点数2", 4.0f);
		Func();
	}


}










#endif //__FvScriptConstValue_H__