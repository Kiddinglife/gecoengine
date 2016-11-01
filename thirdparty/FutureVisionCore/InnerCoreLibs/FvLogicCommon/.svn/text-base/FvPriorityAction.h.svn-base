#pragma once

#include <FvKernel.h>
#include <FvDebug.h>

#include <FvDoubleLink.h>


template<class TData, class TAction>
class FvPriorityActionList
{
public:
	typedef FvDoubleLink<TAction*, FvDoubleLinkNode1> List;
	FvPriorityActionList()
	{}
	~FvPriorityActionList()
	{
		Clear();
	}

	TAction* GetActive()const;
	void Attach(TAction& kAction);
	void Clear(){m_kList.Clear();}
	bool IsEmpty()const{return m_kList.IsEmpty();}

	void Reset(TData& kData);

private:
	List m_kList;
};


//+----------------------------------------------------------------------------------------------------
namespace PriorityAction_Demo
{
	class PriorityAction: public FvDoubleLinkNode1<PriorityAction*>
	{
	public:
		PriorityAction():m_uiWeight(0), m_bReady(false), m_bActive(false)
		{}
		FvInt32 Weight()const{return m_uiWeight;}
		void SetWeight(const FvInt32& uiWeight){m_uiWeight = uiWeight;}

		bool IsReady(const FvUInt32& kData)const{return m_bReady;}
		bool m_bReady;

		bool IsActive()const{return m_bActive;}
		void OnStart(){m_bActive = true;}
		void OnEnd(){m_bActive = false;}

	private:

		bool m_bActive;
		FvInt32 m_uiWeight;
	};

	
	static void Test()
	{
		FvUInt32 uiData = 0;
		FvPriorityActionList<FvUInt32, PriorityAction> kList;
		PriorityAction kNode_1;kNode_1.SetWeight(1); kList.Attach(kNode_1);
		PriorityAction kNode_2;kNode_2.SetWeight(2); kList.Attach(kNode_2);
		PriorityAction kNode_3;kNode_3.SetWeight(3); kList.Attach(kNode_3);
		PriorityAction kNode_4;kNode_4.SetWeight(4); kList.Attach(kNode_4);
		PriorityAction kNode_5;kNode_5.SetWeight(5); kList.Attach(kNode_5);
		kNode_5.m_bReady = true;
		kList.Reset(uiData);
		kNode_5.m_bReady = false;
		kNode_4.m_bReady = true;
		kList.Reset(uiData);
		kNode_5.m_bReady = true;
		kList.Reset(uiData);
	}

}


#include "FvPriorityAction.inl"

