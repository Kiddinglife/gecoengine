//{future header message}
#ifndef __FvRobotMovementController_H__
#define __FvRobotMovementController_H__

#include "FvRobotDefines.h"
#include "FvRobotPatrolGraph.h"
#include <FvVector3.h>
#include <FvNetTypes.h>
#include <FvXMLSection.h>


class FvRobotMovementController
{
public:
	virtual bool NextStep(float& fSpeed, float dTime,
		FvVector3& kPos, FvDirection3& kDir) = 0;
};

class FvRobotMovementFactory
{
public:
	FvRobotMovementFactory(const char* pcName);
	virtual FvRobotMovementController* Create(const FvString& kData,
		float& fSpeed, FvVector3& kStartPos) = 0;
};


class FvGraphTraverser : public FvRobotMovementController
{
public:
	FvGraphTraverser(const Graph& kGraph, float& fSpeed,
		FvVector3& kStartPosition, bool bRandomise, bool bSnap);
	virtual bool NextStep(float& fSpeed, float dTime,
		FvVector3& kPos, FvDirection3& kDir);

protected:
	int m_iDestinationNode;
	FvVector3 m_kDestinationPos;
	float m_fStayTime;
	const Graph& m_kGraph;
};


class FvZigzagGraphTraverser : public FvGraphTraverser
{
public:
	static const int DEFAULT_CORRIDOR_WIDTH;

	FvZigzagGraphTraverser(const Graph& kGraph, float& fSpeed,
		FvVector3& kStartPosition,
		float fCorridorWidth = DEFAULT_CORRIDOR_WIDTH);
	virtual bool NextStep(float& fSpeed, float dTime,
		FvVector3& kPos, FvDirection3& kDir);

protected:
	void SetZigzagPos(const FvVector3& kCurrPos);

	float m_fCorridorWidth;
	FvVector3 m_kSourcePos;
	FvVector3 m_kZigzagPos;
};


#endif//! __FvRobotMovementController_H__
