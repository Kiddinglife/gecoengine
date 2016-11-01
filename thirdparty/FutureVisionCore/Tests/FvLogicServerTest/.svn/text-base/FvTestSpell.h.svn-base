#pragma once

namespace TestSpell_Demo
{
	static void Test();
}

//class Caser
//{
//public:
//
//	template<class TCallee>
//	static void Invoke(TCallee& kCallee)
//	{
//		
//	}
//
//	template<class TCallee>
//	static void _Invoke(TCallee& kCallee)
//	{
//
//	}
//
//
//private:
//
//
//};
//
//
//template<class Param>
//class Caser1
//{
//public:
//	void Encode(const Param kValue, FvMemoryOStream& kOS)
//	{
//		kOS << kValue;
//	}
//
//	void Invoke(FvMemoryIStream& kIS)
//	{
//		Param kValue;
//		kIS >> kValue;
//	}
//
//private:
//
//	
//};
//
//template<class Param1, class Param2>
//class Caser2
//{
//public:
//	void Encode(const Param1 kValue1, const Param2 kValue2);
//
//	void Decode(){}
//
//	void Invoke(){}
//
//private:
//
//
//};

#include <FvKernel.h>

#include <vector>


template<class T>
class GhostInvoker
{
public:
	typedef void (*pFuncDecode)(T& kT);
	typedef std::vector<pFuncDecode> FuncList;

	//+----------------------------------------------------------------------------------------------------
	template <void (T::*func)()>
	static void Invoke(T& kT)
	{
		(kT.*func)();
		FvUInt32 uiFuncId = GetFuncIdx<func>();
	}
	//!
	template <void (T::*func)()>
	static FvUInt32 GetFuncIdx();
	//! 
	template <void (T::*func)()>
	static void Decode(T& kT)
	{
		(kT.*func)();
	}

	//+----------------------------------------------------------------------------------------------------
	template <class Param, void (T::*func)(const Param)>
	static void Invoke(T& kT, const Param kValue)
	{
		(kT.*func)(kValue);
		FvUInt32 uiFuncId = GetFuncIdx<Param, func>();
	}
	//!
	template <class Param, void (T::*func)(const Param)>
	static FvUInt32 GetFuncIdx();
	//!
	template <class Param, void (T::*func)(const Param)>
	static void Decode(T& kT)
	{
		Param kParam;
		(kT.*func)(kParam);
	}

	//+----------------------------------------------------------------------------------------------------
	static void Exec(T& kT, const FvUInt32 uiIdx)
	{
		if(uiIdx < S_FuncList.size())
		{
			(*S_FuncList[uiIdx])(kT);
		}
	}	
	static void SetFuncSize(const FvUInt32 uiSize)
	{
		S_FuncList.resize(uiSize, DecodeNULL);
	}
	static void SetFunc(const FvUInt32 uiIdx, pFuncDecode pFunc)
	{
		if(uiIdx < S_FuncList.size())
		{
			S_FuncList[uiIdx] = pFunc;
		}
	}

private:
	static void DecodeNULL(T& kT){}

	static FuncList S_FuncList;
};


#define _FUNC_SET_(OBJ) \
	template <> GhostInvoker<OBJ>::FuncList GhostInvoker<OBJ>::S_FuncList;

//+----------------------------------------------------------------------------------------------------

#define _FUNC_IDX_1_(OBJ, FUNC) \
	GhostInvoker<OBJ>::GetFuncIdx<&OBJ::FUNC>()
#define _FUNC_IDX_2_(OBJ, FUNC, PARAM) \
	GhostInvoker<OBJ>::GetFuncIdx<PARAM, &OBJ::FUNC>()

//+----------------------------------------------------------------------------------------------------
#define _ADD_GHOST_FUNC_1_(OBJ, FUNC) \
	GhostInvoker<OBJ>::SetFunc(GhostInvoker<OBJ>::GetFuncIdx<&OBJ::FUNC>(), &GhostInvoker<OBJ>::Decode<&OBJ::FUNC>)
#define _ADD_GHOST_FUNC_2_(OBJ, FUNC, PARAM) \
	GhostInvoker<OBJ>::SetFunc(GhostInvoker<OBJ>::GetFuncIdx<PARAM, &OBJ::FUNC>(), &GhostInvoker<OBJ>::Decode<PARAM, &OBJ::FUNC>);

//+----------------------------------------------------------------------------------------------------
#define _SET_FUNC_IDX_1_(OBJ, FUNC, FUNC_IDX)\
	template<> template<>\
	FvUInt32 GhostInvoker<OBJ>::GetFuncIdx<&OBJ::FUNC>()	{return FUNC_IDX;}
#define _SET_FUNC_IDX_2_(OBJ, FUNC, PARAM, FUNC_IDX)\
	template<> template<>\
	FvUInt32 GhostInvoker<OBJ>::GetFuncIdx<PARAM, &OBJ::FUNC>()	{return FUNC_IDX;}