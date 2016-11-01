//{future header message}
#ifndef __FvFuncVector_H__
#define __FvFuncVector_H__


#include <FvKernel.h>
#include <vector>

template<class ReturnType, class Param0 = void, class Param1 = void, class Param2 = void>
class FvFuncVector
{
	FV_NOT_COPY_COMPARE(FvFuncVector);
public:
	typedef ReturnType (*Func)(Param0 kParam0, Param1 kParam1, Param2 kParam2);
	typedef std::vector<Func> FuncList;

	FvFuncVector(void){}
	~FvFuncVector(void){}

	void SetSize(const FvUInt32 uiSize);
	FvUInt32 Size()const{return m_kFuncList.size();}
	void Add(const FvUInt32 uiIdx, Func pFunc);
	Func Get(const FvUInt32 uiIdx)const
	{
		if(uiIdx >= m_kFuncList.size())
		{FvLogBus::Error("FvFuncVector::Add 越界"); return NULL;}
		return m_kFuncList[uiIdx];
	}

private:

	FuncList m_kFuncList;

};

template<class ReturnType, class Param0, class Param1>
class FvFuncVector<ReturnType, Param0, Param1>
{
	FV_NOT_COPY_COMPARE(FvFuncVector);
public:
	typedef ReturnType (*Func)(Param0 kParam0, Param1 kParam1);
	typedef std::vector<Func> FuncList;

	FvFuncVector(void){}
	~FvFuncVector(void){}

	void SetSize(const FvUInt32 uiSize);
	FvUInt32 Size()const{return m_kFuncList.size();}
	void Add(const FvUInt32 uiIdx, Func pFunc);
	Func Get(const FvUInt32 uiIdx)const
	{
		if(uiIdx >= m_kFuncList.size())
		{FvLogBus::Error("FvFuncVector::Add 越界"); return NULL;}
		return m_kFuncList[uiIdx];
	}

private:

	FuncList m_kFuncList;

};

template<class ReturnType, class Param0>
class FvFuncVector<ReturnType, Param0>
{
	FV_NOT_COPY_COMPARE(FvFuncVector);
public:
	typedef ReturnType (*Func)(Param0 kParam0);
	typedef std::vector<Func> FuncList;

	FvFuncVector(void){}
	~FvFuncVector(void){}

	void SetSize(const FvUInt32 uiSize);
	FvUInt32 Size()const{return m_kFuncList.size();}
	void Add(const FvUInt32 uiIdx, Func pFunc);
	Func Get(const FvUInt32 uiIdx)const
	{
		if(uiIdx >= m_kFuncList.size())
		{FvLogBus::Error("FvFuncVector::Add 越界"); return NULL;}
		return m_kFuncList[uiIdx];
	}

private:

	FuncList m_kFuncList;

};

template<class ReturnType>
class FvFuncVector<ReturnType>
{
	FV_NOT_COPY_COMPARE(FvFuncVector);
public:
	typedef ReturnType (*Func)();
	typedef std::vector<Func> FuncList;

	FvFuncVector(void){}
	~FvFuncVector(void){}

	void SetSize(const FvUInt32 uiSize);
	FvUInt32 Size()const{return m_kFuncList.size();}
	void Add(const FvUInt32 uiIdx, Func pFunc);
	Func Get(const FvUInt32 uiIdx)const
	{
		if(uiIdx >= m_kFuncList.size())
		{FvLogBus::Error("FvFuncVector::Add 越界"); return NULL;}
		return m_kFuncList[uiIdx];
	}

private:

	FuncList m_kFuncList;

};

#include "FvFuncSet.inl"


#endif //__FvFuncVector_H__