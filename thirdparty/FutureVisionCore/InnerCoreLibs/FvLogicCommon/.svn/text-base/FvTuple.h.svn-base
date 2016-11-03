//{future header message}
#ifndef __FvTuple_H__
#define  __FvTuple_H__


#include "FvLogicCommonDefine.h"
#include "FvLogicDebug.h"
#include "FvMemoryNode.h"
#include <FvKernel.h>
#include <FvMemoryStream.h>
#include <vector>
#include <map>
#include <FvDebug.h> 

class FV_LOGIC_COMMON_API FvTupleBaseNode
{
public:
	virtual FvUInt32 Type() const = 0;
	virtual bool CopyFrom(const FvTupleBaseNode& kOther){return true;}
	virtual void SerializeTo(FvBinaryOStream& kOS)const{}
	virtual bool SerializeFrom(FvBinaryIStream& kIS){return true;}
	virtual void PrintTo(std::string& kStr)const{}

protected:

	FvTupleBaseNode(const FvTupleBaseNode&){}
	FvTupleBaseNode(){}
	~FvTupleBaseNode(){}

	void operator=(const FvTupleBaseNode& kOther){}
	bool operator==(const FvTupleBaseNode& kOther){return false;}

	friend class FvDynamicTuple;
};

class FV_LOGIC_COMMON_API FvBaseTuple
{
public:

	template <class T> bool GetValue(const FvUInt32 uiIdx, T& kValue)const;
	template <class T> const T& Value(const FvUInt32 uiIdx, const T& kDefaultValue)const;
	template <class T> bool SetValue(const FvUInt32 uiIdx, const T& kValue);
	virtual FvUInt32 Size()const = 0;

	virtual const FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx)const = 0;
	virtual FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx) = 0;

protected:
	FvBaseTuple(void){}
	~FvBaseTuple(void){}
};


//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class T>
class FvTupleNode: public FvTupleBaseNode
{
public:
	static FvUInt32 TYPE();
	virtual FvUInt32 Type()const {return TYPE();}

	FvTupleNode(const T& kValue = T()):m_kValue(kValue){}
	T m_kValue;

	virtual void SerializeTo(FvBinaryOStream& kOS)const{kOS << m_kValue;}
	virtual bool SerializeFrom(FvBinaryIStream& kIS){kIS >> m_kValue;	return true;}
	virtual void PrintTo(std::string& kStr)const;//! 外部定义
	virtual bool CopyFrom(const FvTupleBaseNode& kOther);
};

//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class Param0, class Param1 = void, class Param2 = void, class Param3 = void, class Param4 = void, class Param5 = void>
class FvStaticTuple: public FvBaseTuple
{
public:
	FvStaticTuple(){}
	FvUInt32 Size()const{return 6;}
	void PrintTo(std::string& kStr)const
	{
		m_kNode0.PrintTo(kStr);
		m_kNode1.PrintTo(kStr);
		m_kNode2.PrintTo(kStr);
		m_kNode3.PrintTo(kStr);
		m_kNode4.PrintTo(kStr);
		m_kNode5.PrintTo(kStr);
	}

	virtual const FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx)const
	{
		switch(uiIdx)
		{
		case 0:	return &m_kNode0;
		case 1:	return &m_kNode1;
		case 2:	return &m_kNode2;
		case 3:	return &m_kNode3;
		case 4:	return &m_kNode4;
		case 5:	return &m_kNode5;
		default:	return NULL;
		}
	}
	virtual FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx)
	{
		switch(uiIdx)
		{
		case 0:	return &m_kNode0;
		case 1:	return &m_kNode1;
		case 2:	return &m_kNode2;
		case 3:	return &m_kNode3;
		case 4:	return &m_kNode4;
		case 5:	return &m_kNode5;
		default:	return NULL;
		}
	}
private:
	FvTupleNode<Param0> m_kNode0;
	FvTupleNode<Param1> m_kNode1;
	FvTupleNode<Param2> m_kNode2;
	FvTupleNode<Param3> m_kNode3;
	FvTupleNode<Param4> m_kNode4;
	FvTupleNode<Param5> m_kNode5;
};

//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class Param0, class Param1, class Param2, class Param3, class Param4>
class FvStaticTuple<Param0, Param1, Param2, Param3, Param4>: public FvBaseTuple
{
public:
	FvStaticTuple(){}
	FvUInt32 Size()const{return 5;}
	void PrintTo(std::string& kStr)const
	{
		m_kNode0.PrintTo(kStr);
		m_kNode1.PrintTo(kStr);
		m_kNode2.PrintTo(kStr);
		m_kNode3.PrintTo(kStr);
		m_kNode4.PrintTo(kStr);
	}

	virtual const FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx)const
	{
		switch(uiIdx)
		{
		case 0:	return &m_kNode0;
		case 1:	return &m_kNode1;
		case 2:	return &m_kNode2;
		case 3:	return &m_kNode3;
		case 4:	return &m_kNode4;
		default:	return NULL;
		}
	}
	virtual FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx)
	{
		switch(uiIdx)
		{
		case 0:	return &m_kNode0;
		case 1:	return &m_kNode1;
		case 2:	return &m_kNode2;
		case 3:	return &m_kNode3;
		case 4:	return &m_kNode4;
		default:	return NULL;
		}
	}
private:
	FvTupleNode<Param0> m_kNode0;
	FvTupleNode<Param1> m_kNode1;
	FvTupleNode<Param2> m_kNode2;
	FvTupleNode<Param3> m_kNode3;
	FvTupleNode<Param4> m_kNode4;
};

//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class Param0, class Param1, class Param2, class Param3>
class FvStaticTuple<Param0, Param1, Param2, Param3>: public FvBaseTuple
{
public:
	FvStaticTuple(){}
	FvUInt32 Size()const{return 4;}
	void PrintTo(std::string& kStr)const
	{
		m_kNode0.PrintTo(kStr);
		m_kNode1.PrintTo(kStr);
		m_kNode2.PrintTo(kStr);
		m_kNode3.PrintTo(kStr);
	}

	virtual const FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx)const
	{
		switch(uiIdx)
		{
		case 0:	return &m_kNode0;
		case 1:	return &m_kNode1;
		case 2:	return &m_kNode2;
		case 3:	return &m_kNode3;
		default:	return NULL;
		}
	}
	virtual FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx)
	{
		switch(uiIdx)
		{
		case 0:	return &m_kNode0;
		case 1:	return &m_kNode1;
		case 2:	return &m_kNode2;
		case 3:	return &m_kNode3;
		default:	return NULL;
		}
	}
private:
	FvTupleNode<Param0> m_kNode0;
	FvTupleNode<Param1> m_kNode1;
	FvTupleNode<Param2> m_kNode2;
	FvTupleNode<Param3> m_kNode3;
};

//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class Param0, class Param1, class Param2>
class FvStaticTuple<Param0, Param1, Param2>: public FvBaseTuple
{
public:
	FvStaticTuple(){}
	FvUInt32 Size()const{return 3;}
	void PrintTo(std::string& kStr)const
	{
		m_kNode0.PrintTo(kStr);
		m_kNode1.PrintTo(kStr);
		m_kNode2.PrintTo(kStr);
	}

	virtual const FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx)const
	{
		switch(uiIdx)
		{
		case 0:	return &m_kNode0;
		case 1:	return &m_kNode1;
		case 2:	return &m_kNode2;
		default:	return NULL;
		}
	}
	virtual FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx)
	{
		switch(uiIdx)
		{
		case 0:	return &m_kNode0;
		case 1:	return &m_kNode1;
		case 2:	return &m_kNode2;
		default:	return NULL;
		}
	}
private:
	FvTupleNode<Param0> m_kNode0;
	FvTupleNode<Param1> m_kNode1;
	FvTupleNode<Param2> m_kNode2;
};

//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class Param0, class Param1>
class FvStaticTuple<Param0, Param1>: public FvBaseTuple
{
public:
	FvStaticTuple(){}
	FvUInt32 Size()const{return 2;}
	void PrintTo(std::string& kStr)const
	{
		m_kNode0.PrintTo(kStr);
		m_kNode1.PrintTo(kStr);
	}

	virtual const FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx)const
	{
		switch(uiIdx)
		{
		case 0:	return &m_kNode0;
		case 1:	return &m_kNode1;
		default:	return NULL;
		}
	}
	virtual FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx)
	{
		switch(uiIdx)
		{
		case 0:	return &m_kNode0;
		case 1:	return &m_kNode1;
		default:	return NULL;
		}
	}
private:
	FvTupleNode<Param0> m_kNode0;
	FvTupleNode<Param1> m_kNode1;
};

//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template<class Param0>
class FvStaticTuple<Param0>: public FvBaseTuple
{
public:
	FvStaticTuple(){}
	FvUInt32 Size()const{return 1;}
	void PrintTo(std::string& kStr)const
	{
		m_kNode0.PrintTo(kStr);
	}

	virtual const FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx)const
	{
		switch(uiIdx)
		{
		case 0:	return &m_kNode0;
		default:	return NULL;
		}
	}
	virtual FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx)
	{
		switch(uiIdx)
		{
		case 0:	return &m_kNode0;
		default:	return NULL;
		}
	}
private:
	FvTupleNode<Param0> m_kNode0;
};


class FV_LOGIC_COMMON_API FvStaticEmptyTuple: public FvBaseTuple
{
public:
	FvStaticEmptyTuple(){}
	FvUInt32 Size()const{return 0;}

private:
	virtual const FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx)const
	{
		return NULL;
	}
	virtual FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx)
	{
		return NULL;
	}
};


//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class FV_LOGIC_COMMON_API FvDynamicTuple: public FvBaseTuple
{
public:
	typedef FvTupleBaseNode* (*pCreateFunc)();
	typedef std::vector<pCreateFunc> _List;

	static FvTupleBaseNode* CreateNode(const FvUInt32 uiType);
	static void SetRegisterTypeCnt(const FvUInt32 uiSize);

	FvDynamicTuple(const FvBaseTuple& kOther);
	FvDynamicTuple(const FvDynamicTuple& kOther);

	FvDynamicTuple(){}
	~FvDynamicTuple();

	void Clear();
	void Reserve(const FvUInt32 uiSize){m_kList.reserve(uiSize);}
	const std::vector<FvTupleBaseNode*>& ValueList()const{return m_kList;}
	FvUInt32 Size()const;
	FvTupleBaseNode* AttachNode(const FvUInt32 uiType);

	void operator=(const FvDynamicTuple& kOther);
	void operator=(const FvBaseTuple& kOther);

	void Append(const FvDynamicTuple& kOther);
	void Append(const FvBaseTuple& kOther);

	template<class T> static void Register();
	template<class T> void PushBack(const T& kT);

	void PrintTo(std::vector<std::string>& kStrList)const;

private:
	virtual const FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx)const;
	virtual FvTupleBaseNode* _GetNode(const FvUInt32 uiIdx);

	std::vector<FvTupleBaseNode*> m_kList;

	template<class T> static FvTupleBaseNode* Create(){return new FvTupleNode<T>();}
	static FvTupleBaseNode* CreateNULL();
	static _List& S_List();
	
};

#include "FvTuple.inl"

namespace Tuple_Demo
{
	static void TEST()
	{
		float fValue = 0;
		int iValue = 0;
		bool bValue = false;
		char cValue = 0;
		float* pfValue = NULL;
		int* piValue = NULL;

		FvStaticTuple<float> kTuple1;
		FvBaseTuple* pkTuple = &kTuple1;
		pkTuple->SetValue<float>(0, 100.0f);
		pkTuple->GetValue<float>(0, fValue);
		pkTuple->SetValue<int>(0, 100);
		pkTuple->GetValue<int>(0, iValue);

		FvStaticTuple<float, int> kTuple2;
		pkTuple = &kTuple2;
		pkTuple->SetValue<int>(1, 1000);
		pkTuple->GetValue<int>(1, iValue);
		pkTuple->SetValue<float>(1, 100.0f);
		pkTuple->GetValue<float>(1, fValue);

		FvStaticTuple<float, int, bool> kTuple3;
		pkTuple = &kTuple3;
		pkTuple->SetValue<bool>(2, true);
		pkTuple->GetValue<bool>(2, bValue);

		FvStaticTuple<float, int, bool, char> kTuple4;
		pkTuple = &kTuple4;
		pkTuple->SetValue<char>(3, 2);
		pkTuple->GetValue<char>(3, cValue);

		FvDynamicTuple kDynamicTuple;
		kDynamicTuple = kTuple4;

	}
}







#endif //__FvTuple_H__
