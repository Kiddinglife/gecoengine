#include "FvWeightChannel.h"

#include "FvLogbus.h"





FvWeightChannelNode::FvWeightChannelNode()
:m_uiWeight(0)
,m_rpChannel(NULL)
{
}

FvWeightChannelNode::~FvWeightChannelNode()
{
	FV_ASSERT(m_rpChannel == NULL);
}

FvUInt32 FvWeightChannelNode::Weight()const
{
	return m_uiWeight;
}
void FvWeightChannelNode::SetWeight(const FvUInt32 uiWeight)
{
	m_uiWeight = uiWeight;
	if(m_rpChannel && m_rpChannel->ActiveNode() != this)
	{
		m_rpChannel->_UpdateWeight(*this);
	}
}

void FvWeightChannelNode::_OnActive()
{
	//ViLogBus::CritOk("FvWeightChannelNode::_OnActive WEIGHT[%d]", Weight());
}
void FvWeightChannelNode::_OnDeactive()
{
	//ViLogBus::CritOk("FvWeightChannelNode::_OnDeactive WEIGHT[%d]", Weight());
}

FvWeightChannel::FvWeightChannel()
:m_rpActiveNode(NULL)
{
}
FvWeightChannel::~FvWeightChannel()
{
	if(m_rpActiveNode)
	{
		m_rpActiveNode->_OnActive();
	}
}

void FvWeightChannel::_UpdateWeight(FvWeightChannelNode& kNode)
{
	FV_ASSERT(kNode.m_rpChannel == this);
	if(kNode.m_rpChannel == this)
	{
		FV_ASSERT(m_rpActiveNode);
		FV_ASSERT(m_rpActiveNode != &kNode);
		if( kNode.Weight() > m_rpActiveNode->Weight())
		{
			m_rpActiveNode->_OnDeactive();
			m_rpActiveNode = &kNode;
			m_rpActiveNode->_OnActive();
		}
	}	
}

void FvWeightChannel::Attach(FvWeightChannelNode& kNode)
{
	if(kNode.m_rpChannel)
	{
		FV_ASSERT(kNode.m_rpChannel == this);
		_UpdateWeight(kNode);
		return;
	}
	kNode.m_rpChannel = this;
	m_Nodes.AttachBack(kNode);
	if(m_rpActiveNode == NULL)
	{
		m_rpActiveNode = &kNode;
		m_rpActiveNode->_OnActive();
	}
	else
	{
		if(kNode.Weight() > m_rpActiveNode->Weight())
		{
			m_rpActiveNode->_OnDeactive();
			m_rpActiveNode = &kNode;
			kNode._OnActive();
		}
	}
}
void FvWeightChannel::Detach(FvWeightChannelNode& kNode)
{
	FV_ASSERT(kNode.m_rpChannel == this);
	kNode.Detach();
	kNode.m_rpChannel = NULL;
	if(m_rpActiveNode == &kNode)
	{
		m_rpActiveNode->_OnDeactive();
		m_rpActiveNode = _MaxWeightNode();
		if(m_rpActiveNode)
		{
			m_rpActiveNode->_OnActive();
		}
	}
}

FvWeightChannelNode* FvWeightChannel::_MaxWeightNode()const
{
	if(m_Nodes.Size() == 0)
	{
		return NULL;
	}
	_List::iterator iter = m_Nodes.GetHead();
	FvWeightChannelNode* pkTopNode = static_cast<FvWeightChannelNode*>(iter);
	_List::Next(iter);
	FV_ASSERT(pkTopNode);
	while (!m_Nodes.IsEnd(iter))
	{
		FvWeightChannelNode* pkNode = static_cast<FvWeightChannelNode*>(iter);
		_List::Next(iter);
		FV_ASSERT(pkNode);
		if( pkNode->Weight() > pkTopNode->Weight() )
		{
			pkTopNode = pkNode;
		}
	}
	return pkTopNode;
}

FvWeightChannelNode* FvWeightChannel::ActiveNode()const
{
	return m_rpActiveNode;
}
FvUInt32 FvWeightChannel::Size()const
{
	return m_Nodes.Size();
}