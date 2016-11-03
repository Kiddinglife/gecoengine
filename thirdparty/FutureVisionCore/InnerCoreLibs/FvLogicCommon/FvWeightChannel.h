//{future header message}
#ifndef __FvWeightChannel_H__
#define __FvWeightChannel_H__

#include "FvLogicCommonDefine.h"

#include "FvRefList.h"

#include <FvKernel.h>

class FV_LOGIC_COMMON_API FvWeightChannelNode: private FvRefNode1<FvWeightChannelNode*>
{
	friend class FvWeightChannel;
public:

	FvUInt32 Weight()const;
	void SetWeight(const FvUInt32 uiWeight);

	virtual void _OnActive();
	virtual void _OnDeactive();

	FvWeightChannelNode();
	~FvWeightChannelNode();

private:

	FvUInt32 m_uiWeight;
	FvWeightChannel* m_rpChannel;

};



//! 只允许一个Node是激活的
class FV_LOGIC_COMMON_API FvWeightChannel
{
public:
	typedef FvRefList<FvWeightChannelNode*, FvRefNode1> _List;

	void Attach(FvWeightChannelNode& kNode);
	void Detach(FvWeightChannelNode& kNode);
	void _UpdateWeight(FvWeightChannelNode& kNode);

	FvWeightChannelNode* ActiveNode()const;	
	FvUInt32 Size()const;

	FvWeightChannel();
	~FvWeightChannel();

private:

	FvWeightChannelNode* _MaxWeightNode()const;

	_List m_Nodes;
	FvWeightChannelNode* m_rpActiveNode;
};


//+-----------------------------------------------------------------------------------------------------------------------------------------------------------

//! 扩展ChannelNode

template <class T, void (T::*onActiveFunc)(), void (T::*onDeactiveFunc)()>
class FvTWeightChannelNode: public FvWeightChannelNode
{
public:

	FvTWeightChannelNode():m_rp(NULL){}
	void Init(T& qt)
	{
		m_rp = &qt;
	}

private:
	virtual void _OnActive()
	{
		FV_ASSERT(m_rp);
		if(m_rp)
		{
			((*m_rp).*onActiveFunc)();
		}
	}
	virtual void _OnDeactive()
	{
		FV_ASSERT(m_rp);
		if(m_rp)
		{
			((*m_rp).*onDeactiveFunc)();
		}
	}

	T* m_rp;
};


//+-----------------------------------------------------------------------------------------
//+-----------------------------------------------------------------------------------------
///<测试用例>
namespace ChannelNode_Demo
{
	static void Test_1()
	{
		FvWeightChannel _channel;
		FvWeightChannelNode _node_1;	
		_node_1.SetWeight(1);
		_channel.Attach(_node_1);
		_node_1.SetWeight(2);

		FvWeightChannelNode _node_2;	
		_node_2.SetWeight(1);
		_channel.Attach(_node_2);
		_node_2.SetWeight(3);

		FvWeightChannelNode _node_3;	
		_channel.Attach(_node_3);
		_node_3.SetWeight(3);

		_channel.Detach(_node_2);
		_channel.Detach(_node_1);
		_channel.Detach(_node_3);
	}

	class _Class
	{
	public:
		_Class()
		{
			_node.Init(*this);
		}
		void On(){}
		void Off(){}
		FvTWeightChannelNode<_Class, &_Class::On, &_Class::Off> _node;
	};
	static void Test_2()
	{
		FvWeightChannel _channel;
		_Class _node_1;	
		_node_1._node.SetWeight(1);
		_channel.Attach(_node_1._node);
		_node_1._node.SetWeight(2);

		_Class _node_2;	
		_node_2._node.SetWeight(1);
		_channel.Attach(_node_2._node);
		_node_2._node.SetWeight(3);

		_Class _node_3;	
		_channel.Attach(_node_3._node);
		_node_3._node.SetWeight(3);

		_channel.Detach(_node_2._node);
		_channel.Detach(_node_1._node);
		_channel.Detach(_node_3._node);
	}
}
//+-----------------------------------------------------------------------------------------


#endif //__FvWeightChannel_H__