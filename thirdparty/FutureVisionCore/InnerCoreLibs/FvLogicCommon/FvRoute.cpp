#include "FvRoute.h"
#include <FvMath.h>

FvVector2 GetCross(const FvVector2& kPos1, const FvVector2& kPos2, const FvVector2& kCenter, const float fRadius)
{
	const float distNear = (kPos1 - kCenter).Length();
	FV_ASSERT(distNear < fRadius + 0.01f);
	const float distFar = (kPos2 - kCenter).Length();
	FV_ASSERT(distFar > fRadius - 0.01f);
	FvVector2 v2_v1 = kPos2 - kPos1; 
	v2_v1.Normalise();
	const FvVector2 v1_center = kPos1 - kCenter;
	float prj_v1_center = v1_center.DotProduct(v2_v1);
	const FvVector2 crossHeight = v1_center - v2_v1*prj_v1_center;
	const float len2 = crossHeight.LengthSquared();
	const float d = sqrt(fRadius*fRadius - len2);
	const float diff = -prj_v1_center + d;
	const FvVector2 crossPoint = kPos1 + v2_v1*diff;
	const float R = (crossPoint - kCenter).Length();
	FV_ASSERT(fabs(fRadius - R) < 0.01f && "ViMathTool::GetCross 计算误差太大");
	return crossPoint;
}
void FvRoute::FomatIntervalDest(const FvVector3& kSrc, std::vector<FvVector3>& kPath, const FvVector3& kDest, const float fDiff)
{
	if(fDiff == 0.0f)
	{
		const FvVector3* pkPre = &kSrc;
		for (std::vector<FvVector3>::iterator iter = kPath.begin(); iter != kPath.end();)
		{
			const FvVector3& kPos = (*iter);
			if(kPos == *pkPre)
			{
				iter = kPath.erase(iter);
			}
			else
			{
				 ++iter;
			}
		}
		return;
	}
	else
	{
		const FvVector3* pkPre = &kSrc;
		const float fDiff2 = fDiff*fDiff;
		const float dist2 = FvMath2DTool::Distance2(pkPre->x, pkPre->y, kDest.x, kDest.y);
		if(dist2 < fDiff2)//! 进入圆
		{
			kPath.clear();
			return;
		}

		for (std::vector<FvVector3>::iterator iter = kPath.begin(); iter != kPath.end();)
		{
			const FvVector3& kPos = (*iter);
			if(kPos == *pkPre)
			{
				iter = kPath.erase(iter);
				continue;
			}
			const float dist2 = FvMath2DTool::Distance2(kPos.x, kPos.y, kDest.x, kDest.y);
			if(dist2 < fDiff2)//! 进入圆
			{
				const FvVector2 kCross = ::GetCross(FvVector2(kPos.x, kPos.y), FvVector2(pkPre->x, pkPre->y), FvVector2(kDest.x, kDest.y), fDiff);
				kPath.erase(iter, kPath.end());
				kPath.push_back(FvVector3(kCross.x, kCross.y, kDest.z));
				break;
			}
			pkPre = &kPos;
			++iter;
		}
	}
}


FvRoute::FvRoute(void)
:m_PrePos(0,0,0)
,m_PosCurrent(0,0,0)
,m_rpNextNode(NULL)
,m_Dir(0,0,0)
,m_State(IDLE)
{
	FV_MEM_TRACK;
}

FvRoute::~FvRoute(void)
{
}
//+-------------------------------------------------------------------
void FvRoute::Append(const FvVector3& kPos)
{
	m_Nodes.push_back(FvRouteNode(kPos, FvRouteNode::NULL_EVENT));
}
void FvRoute::Append(const FvVector3& kPos, const FvUInt32 uiEvent)
{
	FV_ASSERT_ERROR(uiEvent != FvRouteNode::NULL_EVENT)
	m_Nodes.push_back(FvRouteNode(kPos, uiEvent));
}
//+-------------------------------------------------------------------
bool FvRoute::OnTick(const float fDeltaTime, const float fSpd)
{
	return _UpdateDistance(fDeltaTime*fSpd);
}
const FvVector3& FvRoute::GetPosition()const
{
	return m_PosCurrent;
}
const FvVector3& FvRoute::GetDir()const
{
	return m_Dir;
}
const FvVector3& FvRoute::GetDestPos()const
{
	if(m_Nodes.size() == 0)//! 少于两个点的轨迹是无效的!
	{
		return m_PosCurrent;
	}
	else
	{
		_RouteNodeList::const_reference ref = m_Nodes.back();
		return (ref).m_kPos;
	}
}
const FvVector3* FvRoute::GetNextPosition()const
{
	if(m_rpNextNode)
	{
		return &(m_rpNextNode->m_kPos);
	}
	else
	{
		return NULL;
	}
}
void FvRoute::Start(const FvVector3& kPos)
{
	if(m_Nodes.size() == 0)//! 少于两个点的轨迹是无效的!
	{
		_ClearState();
		return;
	}
	m_State = MOVING;
	m_PrePos = kPos;
	m_PosCurrent = m_PrePos;
	m_rpNextNode = _LerpToNextNode();
	FV_ASSERT(m_rpNextNode);
}
bool FvRoute::_UpdateDistance(const float fDistance)
{
	if(m_rpNextNode == NULL)
	{
		return false;
	}
	const FvVector3& kNextPos = m_rpNextNode->m_kPos;
	//! 只考虑水平方向的速度
	const float fDisRight = FvMath2DTool::Distance(m_PosCurrent.x, m_PosCurrent.y, kNextPos.x, kNextPos.y);
	if(fDisRight > fDistance)
	{
		m_PosCurrent += m_Dir*fDistance;
	}
	else//! 越过当前节点
	{
		const float fNewDistance = fDistance - fDisRight;
		m_PosCurrent = kNextPos;
		m_PrePos = kNextPos;
		if(FvRouteNode::HasEvent(m_rpNextNode->m_uiEvent))
		{
			m_kEventCallbackList.Invoke(m_rpNextNode->m_uiEvent, *this);
		}
		m_Nodes.pop_front();
		m_rpNextNode = _LerpToNextNode();
		if(m_rpNextNode)
		{
			_UpdateDistance(fNewDistance);//! 迭代计算
		}
		else
		{
			//! 没有点可以移动了	
			_ClearState();
		}	
	}
	return true;
}
FvRouteNode* FvRoute::_LerpToNextNode()
{
	if(m_Nodes.size() != 0)//! 如果还有剩余
	{
		FvRouteNode* pNode = &(*m_Nodes.begin());
		m_Dir = pNode->m_kPos - m_PrePos;//! 新的朝向
		m_Dir.z = 0.0f;
		m_Dir.Normalise();
		return pNode;
	}
	else
	{
		return NULL;
	}	
}

void FvRoute::Reset()
{
	_ClearState();
	_ClearNodeList();
	m_kEventCallbackList.Clear();
}
//bool FvRoute::End(const ViPoint3& pos)
//{
	//VI_ASSERT(m_spNextNode);
	//if(m_spNextNode == NULL)
	//{
	//	//m_PosCurrent = pos;
	//	return false;
	//}
	////ViLogBus::OK(LOG_YISA, "FvRoute::End(%f, %f)", pos.x, pos.z);
	////ViLogBus::OK(LOG_YISA, "FvRoute::CurrentPos(%f, %f)", m_PosCurrent.x, m_PosCurrent.z);
	//FvVector3 project(0,0,0);
	//FvVector3 aplomb(0,0,0);
	//FvVector3 posEnd(0,0,0);
	//const FvVector3* fromPos = &m_PosCurrent;
	//const FvVector3* toPos = &m_rpNextNode->pos;
	//const FvVector3 nearPos = ViMathCore::ViMathTool::Near(*fromPos, *toPos, pos);
	//float diff2 = ViMathTool::Distance2(nearPos.x, nearPos.z, pos.x, pos.z);
	//fromPos = toPos;
	//posEnd = nearPos;
	//if(m_Nodes.size() == 0)
	//{
	//	//ViLogBus::OK(LOG_YISA, "(%f, %f)", m_spNextNode->pos.x, m_spNextNode->pos.z);
	//	m_rpNextNode->pos = posEnd;
	//	//ViLogBus::OK(LOG_YISA, "m_Nodes is None EndPosMod(%f, %f)", posEnd.x, posEnd.z);
	//	return true;
	//}
	//bool better = false;
	//_RouteNodeList::iterator iterEnd = m_Nodes.begin();
	//_RouteNodeList::iterator iter = m_Nodes.begin();
	//while(iter != m_Nodes.end())
	//{
	//	_RouteNode* pNode = (*iter);
	//	VI_ASSERT(pNode);
	//	toPos = &pNode->pos;
	//	//ViLogBus::OK(LOG_YISA, "(%f, %f)", toPos->x, toPos->z);
	//	const ViPoint3 nearPos_ = ViMathCore::ViMathTool::Near(*fromPos, *toPos, pos);
	//	const float diff2_ = ViMathTool::Distance2(nearPos_.x, nearPos_.z, pos.x, pos.z);
	//	if(diff2_ < diff2)
	//	{
	//		diff2 = diff2_;
	//		iterEnd = iter;
	//		pNode->pos = nearPos_;
	//		posEnd = nearPos_;
	//		better = true;
	//	}
	//	fromPos = toPos;	
	//	++iter;
	//}
	//if(better)//! 
	//{
	//	VI_ASSERT(iterEnd != m_Nodes.end());
	//	++iterEnd;
	//	if(iterEnd != m_Nodes.end())
	//	{
	//		m_Nodes.erase(iterEnd, m_Nodes.end());
	//	}
	//}
	//else
	//{
	//	m_spNextNode->pos = posEnd;
	//	m_Nodes.clear();
	//}
	////ViLogBus::OK(LOG_YISA, "修正后的路径");
	//for(_RouteNodeList::const_iterator iter = m_Nodes.begin(); iter != m_Nodes.end(); ++iter)
	//{
	//	const _RouteNode* pNode = (*iter);
	//	VI_ASSERT(pNode);
	//	toPos = &pNode->pos;
	//	//ViLogBus::OK(LOG_YISA, "(%f, %f)", toPos->x, toPos->z);
	//}
	//ViLogBus::OK(LOG_YISA, "m_Nodes is not None EndPosMod(%f, %f)", posEnd.x, posEnd.z);
//	return true;
//}

void FvRoute::_ClearNodeList()
{
	m_Nodes.clear();
}
void FvRoute::_ClearState()
{
	m_State = IDLE;
	m_rpNextNode = NULL;
	m_PrePos = m_PosCurrent;
	m_Dir = FvVector3::ZERO;
}
bool FvRoute::IsEnd()const{return m_State == IDLE;}
float FvRoute::Length()const
{
	float fLen = 0.0f;
	const FvVector3* pPreNode = &m_PosCurrent;
	_RouteNodeList::const_iterator iter = m_Nodes.begin();
	while(iter != m_Nodes.end())
	{
		const FvRouteNode& pNode = (*iter);
		fLen += FvDistance(pNode.m_kPos, *pPreNode);
		pPreNode = &(pNode.m_kPos);
		++iter;
	}
	return fLen;
}
