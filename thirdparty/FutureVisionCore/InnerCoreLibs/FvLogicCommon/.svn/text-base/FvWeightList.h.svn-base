//{future header message}
#ifndef __FvWeightList_H__
#define __FvWeightList_H__


#include <vector>
#include <FvKernel.h>

template<class T>
class FvWeightList
{
protected:
	struct _Node
	{
		_Node(const T& kT, FvUInt32 uiWeight)
			:m_kT(kT), m_uiWeight(uiWeight)
		{}
		T m_kT;
		FvUInt32 m_uiWeight;
	};

public:
	void Add(const FvUInt32 uiWeight, const T& kT)
	{
		if(Has(kT))
		{
			return;
		}
		std::vector<_Node>::iterator iter = m_kList.begin();
		for (; iter != m_kList.end(); ++iter)
		{
			const _Node& kNode = (*iter);
			if(uiWeight > kNode.m_uiWeight)
			{
				break;
			}
		}
		m_kList.insert(iter, _Node(kT, uiWeight));
	}
	void Del(const T& kT)
	{
		for (std::vector<_Node>::iterator iter = m_kList.begin(); iter != m_kList.end(); ++iter)
		{
			const _Node& kNode = (*iter);
			if(kT == kNode.m_kT)
			{
				m_kList.erase(iter);
				break;
			}
		}
	}
	bool Has(const T& kT)const
	{
		for (std::vector<_Node>::const_iterator iter = m_kList.begin(); iter != m_kList.end(); ++iter)
		{
			const _Node& kNode = (*iter);
			if(kT == kNode.m_kT)
			{
				return true;
			}
		}
		return false;
	}
	bool GetTop(T& kT)
	{
		if(m_kList.size())
		{
			kT = m_kList[0].m_kT;
			return true;
		}
		else
			return false;
	}
	void Clear(){m_kList.clear();}
	const std::vector<_Node>& List()const{return m_kList;}

protected:

	std::vector<_Node> m_kList;

};

#include "FvRefList.h"

template<class TWeightType, class TObj, TWeightType DEFAULT_WEIGHT = 0, TObj DEFAULT_OBJ = 0>
class FvWeightList2
{
public:
	class Node
	{
	public:
		Node(): m_kWeight(DEFAULT_WEIGHT), m_kObj(DEFAULT_OBJ)
		{
			m_kAttachNode.m_Content = this;
		}
		TWeightType m_kWeight;
		TObj m_kObj;
		void Detach()
		{
			m_kAttachNode.Detach();
		}
	private:
		mutable FvRefNode2<Node*> m_kAttachNode;
		template<class TWeightType, class TObj, TWeightType DEFAULT_WEIGHT, TObj DEFAULT_OBJ >friend class FvWeightList2;
	};
	

	void Attach(const Node& kNode)
	{
		if(m_kList.Size() == 0)
		{
			m_kList.AttachBack(kNode.m_kAttachNode);
			return;
		}
		FvRefList<Node*>::iterator iter = m_kList.GetHead();
		while (!m_kList.IsEnd(iter))
		{
			Node* pkNode = (*iter).m_Content;
			FV_ASSERT(pkNode);
			if(kNode.m_kWeight > pkNode->m_kWeight)
			{
				break;
			}
			FvRefList<Node*>::Next(iter);
		}
		m_kList.AttachBefore(*iter, kNode.m_kAttachNode);
	}
	void UpdateNode(const Node& kNode)
	{
		if(kNode.m_kAttachNode.IsAttach(m_kList))
		{
			kNode.m_kAttachNode.Detach();
			Attach(kNode);
		}
	}

	bool GetTop(TObj& kObj)const
	{
		if(m_kList.Size() == 0)
		{
			return false;
		}
		FV_ASSERT(m_kList.GetHead()->m_Content);
		kObj = m_kList.GetHead()->m_Content->m_kObj;
		return true;
	}

	void Clear()
	{
		m_kList.Clear();
	}

	const FvRefList<Node*>& List()const{ return m_kList; }

private:

	FvRefList<Node*> m_kList;

};




namespace WeightList_Demo
{


	class TestObj
	{
	public:
		FvUInt32 m_kValue;
		FvWeightList2<FvUInt32, TestObj*>::Node m_kWeightNode;
	};

	static void TEST()
	{
		TestObj kTestObj_1; kTestObj_1.m_kValue = 1; kTestObj_1.m_kWeightNode.m_kWeight = 1; kTestObj_1.m_kWeightNode.m_kObj = &kTestObj_1; 
		TestObj kTestObj_2; kTestObj_2.m_kValue = 2; kTestObj_2.m_kWeightNode.m_kWeight = 2; kTestObj_2.m_kWeightNode.m_kObj = &kTestObj_2; 
		TestObj kTestObj_3; kTestObj_3.m_kValue = 3; kTestObj_3.m_kWeightNode.m_kWeight = 3; kTestObj_3.m_kWeightNode.m_kObj = &kTestObj_3; 

		FvWeightList2<FvUInt32, TestObj*> kWeightList;
		TestObj* pkObj = NULL;
		kWeightList.GetTop(pkObj);
		FV_ASSERT(pkObj == NULL);
		
		kWeightList.Attach(kTestObj_2.m_kWeightNode);
		kWeightList.GetTop(pkObj);
		FV_ASSERT(pkObj == &kTestObj_2);

		kWeightList.Attach(kTestObj_1.m_kWeightNode);
		kWeightList.GetTop(pkObj);
		FV_ASSERT(pkObj == &kTestObj_2);

		kWeightList.Attach(kTestObj_3.m_kWeightNode);
		kWeightList.GetTop(pkObj);
		FV_ASSERT(pkObj == &kTestObj_3);

	}

}
#endif //__FvWeightList_H__
