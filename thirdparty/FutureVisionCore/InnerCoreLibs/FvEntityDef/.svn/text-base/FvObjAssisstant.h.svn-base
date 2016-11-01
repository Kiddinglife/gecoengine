//{future header message}
#ifndef __FvObjAssisstant_H__
#define __FvObjAssisstant_H__

#include "FvEntityDefDefines.h"
#include "FvObj.h"
//
//class FV_ENTITYDEF_API FvObjAssisstant
//{
//public:
//		
//	template <class T>
//	static T GetValue(const FvObj* pkObj);
//
//	//+----------------------------------------------------------------------------------------------------
//	template <class TValue, class TObj>
//	static TValue GetValue(const TObj* pkObj);	
//	template <class TValue, class TObj>
//	static TValue GetValue(const FvObj* pkObj);
//
//#ifdef FV_SERVER
//	template <class TValue, class TObj>
//	static void SetValue(TObj* pkObj, const TValue& kValue);
//	template <class TValue, class TObj>
//	static void SetValue(FvObj* pkObj, const TValue& kValue);
//#endif //FV_SERVER
//
//	//+----------------------------------------------------------------------------------------------------
//	template <class T>
//	static T GetValue(const FvObjStructOld& kObj, const FvUInt32 idx);
//	template <class T>
//	static T GetValue(const FvObjStructOld& kObj, const FvString& name);
//	template <class T>
//	static T GetValue(const FvObjStructOld* pkObj, const FvUInt32 idx);
//	template <class T>
//	static T GetValue(const FvObjStructOld* pkObj, const FvString& name);
//
//
//#ifdef FV_SERVER
//	template <class T>
//	static void SetValue(const FvObjStructOld& kObj, const FvUInt32 idx, const T& kValue);
//	template <class T>
//	static void SetValue(const FvObjStructOld& kObj, const FvString& name, const T& kValue);
//#endif //FV_SERVER
//
//	//template <class T>
//	//static bool GetValue(FvObjStruct& kObj, const FvUInt32 idx, T& value);
//	//template <class T>
//	//static T GetValue(FvObjStruct& kObj, const FvString& name, T& value);
//
//
//private:	
//
//	FvObjAssisstant(void);
//	~FvObjAssisstant(void);
//
//};
//
//
////+----------------------------------------------------------------------------------------------------
//template<class T>
//class PropertyValue
//{
//public:
//	typedef FvSmartPointer<T> _Ptr;
//	void AttachValue(const _Ptr& spValue)
//	{
//		if(m_spValue == NULL)
//		{
//			m_spValue = spValue;
//		}
//	}
//	void DetachValue(){m_spValue = NULL;}
//	bool IsAttach(const _Ptr& spValue)const{return (m_spValue == spValue);}
//	const _Ptr& Value()const{return m_spValue;}
//
//#ifdef FV_SERVER
//	_Ptr& Value(){return m_spValue;}
//#endif //FV_SERVER
//
//protected:
//	PropertyValue(){}
//	~PropertyValue()
//	{
//#ifdef FV_SERVER
//		FV_ASSERT(m_spValue == NULL);
//#endif //FV_SERVER
//	}
//
//private:
//	_Ptr m_spValue;
//};
//
//template<class T>
//class PropertyValueNode
//{
//public:
//	typedef FvSmartPointer<T> _Ptr;
//	void AttachValue(const _Ptr& spValue, const FvObjArrayPtr& spArray)
//	{
//		if(spValue == NULL || spArray == NULL)
//		{
//			return;
//		}
//#ifdef FV_SERVER
//		if(m_spValue && m_spArray)
//		{
//			m_spArray->Erase(m_spValue);
//		}
//		if(spValue->IsOwner(&*spArray))
//		{
//
//		}
//		else
//		{
//			FV_ASSERT(spValue->HasOwner() == false && "²»ÄÜ¸´ÓÃ");
//			_Ptr spObj = spValue;
//			spArray->PushBack(spObj);
//		}
//#endif //FV_SERVER
//		m_spValue = spValue;
//		m_spArray = spArray;
//	}
//	void DetachValue(){m_spValue = NULL; m_spArray = NULL;}
//	bool IsAttach(_Ptr& spValue)const{return (m_spValue == spValue);}
//	const _Ptr& Value()const{return m_spValue;}
//
//#ifdef FV_SERVER
//	void Erase()
//	{
//		FV_ASSERT((m_spValue != NULL && m_spArray != NULL) || (m_spValue == NULL && m_spArray == NULL));
//		if(m_spValue && m_spArray)
//		{
//			m_spArray->Erase(m_spValue);
//			m_spValue = NULL;
//			m_spArray = NULL;
//		}
//	}
//	_Ptr& Value(){return m_spValue;}
//#endif //FV_SERVER
//
//protected:
//	PropertyValueNode(){}
//	~PropertyValueNode()
//	{
//#ifdef FV_SERVER
//		FV_ASSERT(m_spValue == NULL && m_spArray == NULL);
//#endif //FV_SERVER
//	}
//
//private:
//	_Ptr m_spValue;
//	FvObjArrayPtr m_spArray;
//};
//
//
////+----------------------------------------------------------------------------------------------------
//
//class FV_ENTITYDEF_API PropertyStructValue: public PropertyValue<FvObjStructOld>
//{
//public:
//	PropertyStructValue();
//	~PropertyStructValue();
//
//	template <class T>T GetValue(const FvUInt32 idx)const;
//	template <class T>T GetValue(const FvString& name)const;
//
//	bool IsOutModed()const;
//
//protected:
//
//#ifdef FV_SERVER
//	template <class T>void _SetValue(const FvUInt32 idx, const T& kValue);
//	template <class T>void _SetValue(const FvString& name, const T& kValue);
//
//	//virtual void SaveClear() = 0;
//	//virtual void DestroyClear() = 0;
//#endif //FV_SERVER
//};
//
//template <class T>
//T PropertyStructValue::GetValue(const FvUInt32 idx)const
//{
//	FV_ASSERT(Value());
//	if(Value())
//	{
//		return FvObjAssisstant::GetValue<T>(*Value(), idx);
//	}
//	else
//		return 0;
//}
//template <class T>
//T PropertyStructValue::GetValue(const FvString& name)const
//{
//	FV_ASSERT(Value());
//	if(Value())
//	{
//		return FvObjAssisstant::GetValue<T>(*Value(), name);
//	}
//	else
//		return 0;
//}
//
//#ifdef FV_SERVER
//template <class T>
//void PropertyStructValue::_SetValue(const FvUInt32 idx, const T& kValue)
//{
//	FV_ASSERT(Value());
//	if(Value())
//	{
//		FvObjAssisstant::SetValue<T>(*Value(), idx, kValue);
//	}
//}
//template <class T>
//void PropertyStructValue::_SetValue(const FvString& name, const T& kValue)
//{
//	FV_ASSERT(Value());
//	if(Value())
//	{
//		FvObjAssisstant::SetValue<T>(*Value(), name, kValue);
//	}
//}
//#endif //FV_SERVER
//
//
//
//class FV_ENTITYDEF_API PropertyStructValueNode: public PropertyValueNode<FvObjStructOld>
//{
//public:
//	PropertyStructValueNode();
//	~PropertyStructValueNode();
//
//	template <class T>T GetValue(const FvUInt32 idx)const;
//	template <class T>T GetValue(const FvString& name)const;
//
//	bool IsOutModed()const;
//
//protected:
//
//#ifdef FV_SERVER
//	template <class T>void _SetValue(const FvUInt32 idx, const T& kValue);
//	template <class T>void _SetValue(const FvString& name, const T& kValue);
//
//	//virtual void SaveClear() = 0;
//	//virtual void DestroyClear() = 0;
//#endif //FV_SERVER
//};
//
//template <class T>
//T PropertyStructValueNode::GetValue(const FvUInt32 idx)const
//{
//	FV_ASSERT(Value());
//	if(Value())
//	{
//		return FvObjAssisstant::GetValue<T>(*Value(), idx);
//	}
//	else
//		return 0;
//}
//template <class T>
//T PropertyStructValueNode::GetValue(const FvString& name)const
//{
//	FV_ASSERT(Value());
//	if(Value())
//	{
//		return FvObjAssisstant::GetValue<T>(*Value(), name);
//	}
//	else
//		return 0;
//}
//
//#ifdef FV_SERVER
//template <class T>
//void PropertyStructValueNode::_SetValue(const FvUInt32 idx, const T& kValue)
//{
//	FV_ASSERT(Value());
//	if(Value())
//	{
//		FvObjAssisstant::SetValue<T>(*Value(), idx, kValue);
//	}
//}
//template <class T>
//void PropertyStructValueNode::_SetValue(const FvString& name, const T& kValue)
//{
//	FV_ASSERT(Value());
//	if(Value())
//	{
//		FvObjAssisstant::SetValue<T>(*Value(), name, kValue);
//	}
//}
//#endif //FV_SERVER
//
//#ifndef _GET_FUNC_
//#define _GET_FUNC_(FuncName, FieldID, FieldType)\
//	FieldType FuncName()const{return GetValue<FieldType>(FieldID);}
//#endif
//
//#ifndef _SET_FUNC_
//#define _SET_FUNC_(FuncName, FieldID, FieldType)\
//	void FuncName(const FieldType& kValue){_SetValue<FieldType>(FieldID, kValue);}
//#endif

#endif //__FvObjAssisstant_H__
