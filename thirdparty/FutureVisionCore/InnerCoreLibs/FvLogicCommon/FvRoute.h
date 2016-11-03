//{future header message}
#ifndef __FvRoute_H__
#define __FvRoute_H__

#include "FvLogicCommonDefine.h"
#include "FvMemoryNode.h"
#include "FvCallback.h"
#include <FvVector3.h>
#include <list>
#include <vector>

class FV_LOGIC_COMMON_API FvRouteNode
{
public:
	static const FvUInt32 NULL_EVENT = 0XFFFFFFFF;
	static bool HasEvent(const FvUInt32 uiEvent){return (uiEvent != NULL_EVENT);}

	FvRouteNode(const FvVector3& kPos, const FvUInt32 uiEvent):m_kPos(kPos), m_uiEvent(uiEvent){}
	~FvRouteNode(){}

	const FvVector3 m_kPos;
	const FvUInt32 m_uiEvent;
};

//+-------------------------------------------------------------------------------------------------

class FV_LOGIC_COMMON_API FvRoute
{
	FV_MEM_TRACKER(FvRoute);
	FV_NOT_COPY_COMPARE(FvRoute);
public:
	static void FomatIntervalDest(const FvVector3& kSrc, std::vector<FvVector3>& kPath, const FvVector3& kDest, const float fDiff);

	//+-------------------------------------------------------------------
	FvRoute(void);
	~FvRoute(void);

	//+-------------------------------------------------------------------
	void Append(const FvVector3& kPos);//! 添加路过的点
	void Append(const FvVector3& kPos, const FvUInt32 uiEvent);//! 添加路过的点和事件

	bool OnTick(const float fDeltaTime, const float fSpd);
	void Start(const FvVector3& kPos);
	void Reset();
	bool End(const FvVector3& kPos);//! 把路径修正到离pos最近的地方 true 需要修正 false 不需要修正

	const FvVector3& GetPosition()const;
	const FvVector3* GetNextPosition()const;
	const FvVector3& GetDestPos()const;
	const FvVector3& GetDir()const;
	float Length()const;
	bool IsEnd()const;

	FvAsynCallbackList0<FvRoute>& EventCallbackList(){return m_kEventCallbackList;}

private:

	enum _State
	{
		IDLE,
		MOVING,
	};

	//+-------------------------------------------------------------------
	bool _UpdateDistance(const float fDistance);
	FvRouteNode* _LerpToNextNode();
	void _ClearNodeList();
	void _ClearState();

	typedef std::list<FvRouteNode> _RouteNodeList;
	_RouteNodeList m_Nodes;
	FvRouteNode* m_rpNextNode;
	FvVector3 m_PrePos;
	//! 
	FvVector3 m_PosCurrent;
	FvVector3 m_Dir;

	_State m_State;

	FvAsynCallbackList0<FvRoute> m_kEventCallbackList;
};


//+-------------------------------------------------------------------------------------------------
///<实例>

namespace Route_Demo
{
	static void Test()
	{
		FvVector3 pos_1(1.0f, 1.f, 2.0f);
		FvVector3 pos_2(1.0f, 2.f, 2.0f);
		FvRoute route;
		// 设置路径点
		route.Append(pos_1, 1);
		route.Append(pos_2, 2);

		//! 启动
		route.Start(FvVector3(0,0,0));
		while (route.GetNextPosition())
		{
			route.OnTick(1.0f, 1.0f);
		}
		//! 结束
	}
}




#endif//__FvRoute_H__