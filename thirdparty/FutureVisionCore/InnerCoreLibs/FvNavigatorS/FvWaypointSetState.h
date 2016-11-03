#ifndef __FvWaypointSetState_H__
#define __FvWaypointSetState_H__

#include "FvWaypointSet.h"
#include "FvNavigatorDefine.h"

class FV_NAVIGATOR_API FvWaypointSetState
{
public:
	FvWaypointSetState();

	void SetWaypointSet(const FvWaypointSet *pkWPSet);
	void SetPosition(const FvVector3 &kPosition);

	const FvWaypointSet *pWPSet() const	{ return &*m_spWPSet; }
	
	int Compare(const FvWaypointSetState &kOther) const;
	bool IsGoal(const FvWaypointSetState &kGoal) const;
	int	GetAdjacencyCount() const;
	bool GetAdjacency(int iIndex, FvWaypointSetState &kNewState,
				const FvWaypointSetState &kGoal) const;
	float DistanceFromParent() const;
	float DistanceToGoal(const FvWaypointSetState &kGoal) const;
	
private:
	FvConstSmartPointer<FvWaypointSet> m_spWPSet;
	float m_fDistanceFromParent;
	FvVector3 m_kPosition;
};

#endif // __FvWaypointSetState_H__
