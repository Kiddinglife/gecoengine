//{future header message}
#ifndef __FvLogicPriority_H__
#define  __FvLogicPriority_H__


#include <FvKernel.h>
#include <FvDebug.h>

#include <FvDoubleLink.h>

//+----------------------------------------------------------------------------------------------------
template <class T, void (T::*OnActive)(), void (T::*OnDeactive)()>
class FvPriorityList
{
public:

	typedef FvDoubleLink<T*, FvDoubleLinkNode1> List;

	FvPriorityList()
	{}
	~FvPriorityList()
	{
		Clear();
	}
	T* GetTop()const;
	bool Attach(T& kNode);
	bool Remove(T& kNode);
	void Clear(){m_kList.Clear();}
	bool IsEmpty()const{return m_kList.IsEmpty();}
private:
	virtual void _OnUpdated(T* pkOldTop){}
	List m_kList;
};
#include "FvLogicPriority.inl"

//+----------------------------------------------------------------------------------------------------
namespace Priority_Demo
{
	class PriorityNode: public FvDoubleLinkNode1<PriorityNode*>
	{
	public:
		PriorityNode():m_uiWeight(0)
		{}
		FvInt32 Weight()const{return m_uiWeight;}
		void SetWeight(const FvInt32& uiWeight){m_uiWeight = uiWeight;}

		void OnActive(){}
		void OnDeactive(){}
		void OnClear(){}

	private:
		FvInt32 m_uiWeight;
	};

	class PriorityList: public FvPriorityList<PriorityNode, &PriorityNode::OnActive, &PriorityNode::OnDeactive>
	{
	public:
		virtual void _OnUpdated(PriorityNode* pkOldTop){}
		PriorityList(){}
		~PriorityList(){}

	};
	static void Test()
	{
		PriorityList kList;
		PriorityNode kNode_1;kNode_1.SetWeight(1);
		PriorityNode kNode_2;kNode_2.SetWeight(2);
		PriorityNode kNode_3;kNode_3.SetWeight(3);
		PriorityNode kNode_4;kNode_4.SetWeight(4);
		PriorityNode kNode_5;kNode_5.SetWeight(5);
		kList.Attach(kNode_1);
		kList.Attach(kNode_2);
		kList.Attach(kNode_4);
		kList.Attach(kNode_3);
		kList.Remove(kNode_4);
		kList.Remove(kNode_1);
		kList.Remove(kNode_3);
		kList.Remove(kNode_2);
	}

}

#endif //__FvLogicPriority_H__