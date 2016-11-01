//{future header message}
#ifndef __FvDistNotifier_H__
#define __FvDistNotifier_H__

#include "FvMemoryNode.h"

#include <FvVector3.h>
#include <FvProvider.h>
#include <FvRefList.h>

//+-------------------------------------------------------------------------------------------------------------------------------------------------------------

class FV_LOGIC_COMMON_API FvRootRanger
{
	FV_MEM_TRACKER(FvRootRanger);
public:
	FvRootRanger();
	void SetRoot(const FvVector3& kPos);
	void SetRange(const float fRange);
	bool IsIn(const FvVector3& kPos)const;

	const FvVector3& GetRoot()const;

private:
	FvVector3 m_kRootPos;
	float m_fRange2;

};

//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
class FV_LOGIC_COMMON_API FvDistNotifierInterface 
{
	FV_MEM_TRACKER(FvDistNotifierInterface);
public:

	void SetTarget(const FvSmartPointer<FvProvider1<FvVector3>>& kTarget0, const FvSmartPointer<FvProvider1<FvVector3>>& kTarget1);
	void SetRange(const float fRange);

	float GetDistance()const;
	float GetDistance2()const;
	bool IsIn()const;
	bool IsOut()const;

protected:

	FvDistNotifierInterface(void);
	~FvDistNotifierInterface(void);

	FvSmartPointer<FvProvider1<FvVector3>> m_spTarget0;
	FvSmartPointer<FvProvider1<FvVector3>> m_spTarget1;
	float m_fRange2;

};

//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
///<>
class FV_LOGIC_COMMON_API FvDistNotifier: public FvDistNotifierInterface, private FvRefNode1<FvDistNotifier*>
{ 
public:
	static void UpdateTrackDetectors();

	enum _State
	{
		NONE,
		INSIDE,
		OUTSIDE,
	};
	static _State GetState(const FvDistNotifier& kThis);

	bool IsAttach()const;

	void AttachUpdate();
	void DetachUpdate();

	void Notify();

protected:

	FvDistNotifier(void);
	~FvDistNotifier(void);

private:

	virtual void _OnIn() = 0;
	virtual void _OnOut() = 0;

	void _UpdateOutside();
	void _UpdateInside();

	static FvRefListNameSpace<FvDistNotifier>::List1 s_InList;
	static FvRefListNameSpace<FvDistNotifier>::List1 s_OutList;
};

template <class TListener, void (TListener::*funcIn)(), void (TListener::*funcOut)()>
class FvTDistNotifier: public FvDistNotifier
{
public:
	FvTDistNotifier(TListener& kListener):m_kListener(kListener){}
	~FvTDistNotifier(){}

	virtual void _OnIn()
	{
		(m_kListener.*funcIn)();
	}
	virtual void _OnOut()
	{
		(m_kListener.*funcOut)();
	}
private:
	TListener& m_kListener;
};



//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
///<>_OnOut()之前, 自动DetachUpdate();

class FV_LOGIC_COMMON_API FvDistOutNotifier: public FvDistNotifierInterface, private FvRefNode1<FvDistOutNotifier*>
{ 
public:
	static void UpdateTrackDetectors();

	bool IsAttach()const;
	void AttachUpdate();
	void DetachUpdate();

protected:

	FvDistOutNotifier(void);
	~FvDistOutNotifier(void);

private:

	virtual void _OnOut() = 0;

	void _Update();

	static FvRefListNameSpace<FvDistOutNotifier>::List1 s_List;
};

template <class TListener, void (TListener::*funcOut)()>
class FvTDistOutNotifier: public FvDistOutNotifier
{
public:
	FvTDistOutNotifier(TListener& kListener):m_kListener(kListener){}
	~FvTDistOutNotifier(){}

	virtual void _OnOut()
	{
		(m_kListener.*funcOut)();
	}
private:
	TListener& m_kListener;
};



//+-------------------------------------------------------------------------------------------------------------------------------------------------------------
///<>_OnIn()之前, 自动DetachUpdate();

class FV_LOGIC_COMMON_API FvDistInNotifier: public FvDistNotifierInterface, private FvRefNode1<FvDistInNotifier*>
{ 
public:
	static void UpdateTrackDetectors();

	bool IsAttach()const;
	void AttachUpdate();
	void DetachUpdate();

protected:

	FvDistInNotifier(void);
	~FvDistInNotifier(void);

private:

	virtual void _OnIn() = 0;

	void _Update();

	static FvRefListNameSpace<FvDistInNotifier>::List1 s_List;
};

template <class TListener, void (TListener::*funcIn)()>
class FvTDistInNotifier: public FvDistInNotifier
{
public:
	FvTDistInNotifier(TListener& kListener):m_kListener(kListener){}
	~FvTDistInNotifier(){}

	virtual void _OnIn()
	{
		(m_kListener.*funcIn)();
	}
private:
	TListener& m_kListener;
};


namespace DistNotifier_Demo
{
	class TestClass
	{
	public:

		TestClass()
			:
#pragma warning(push)
#pragma warning(disable: 4355)
			m_kDistNotifier(*this)
			,m_kDistInNotifier(*this)
			,m_kDistOutNotifier(*this)
#pragma warning(pop)
			{}

		void In(){}
		void Out(){}

		FvTDistNotifier<TestClass, &TestClass::In, &TestClass::Out> m_kDistNotifier;
		FvTDistInNotifier<TestClass, &TestClass::In> m_kDistInNotifier;
		FvTDistInNotifier<TestClass, &TestClass::Out> m_kDistOutNotifier;
	};


	static void Test()
	{
		TestClass kObj;
		kObj.m_kDistNotifier.AttachUpdate();
		kObj.m_kDistNotifier.DetachUpdate();

		kObj.m_kDistInNotifier.AttachUpdate();
		kObj.m_kDistOutNotifier.AttachUpdate();
	}
}

#endif //__FvDistNotifier_H__
