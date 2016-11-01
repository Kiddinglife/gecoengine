#ifndef __FvWaypointState_H__
#define __FvWaypointState_H__

#include "FvWaypoint.h"
#include "FvWaypointSet.h"
#include "FvWaypointZone.h"
#include "FvNavigatorDefine.h"

class FvWaypointGoalState;

class FV_NAVIGATOR_API FvWaypointState
{
public:
	FvWaypointState();

	void SetWaypoint(const FvWaypoint *pkWaypoint);
	void SetWPSet(const FvWaypointSet *pkWPSet);
	void SetWaypointID(FvWaypointID uiWaypointID);
	void SetPosition(const FvVector3 &kPosition);
	
	const FvZoneID &GetZoneID() const { return m_spWPSet->Zone().GetZoneID(); }
	unsigned int SetNum() const { return m_spWPSet->SetNum(); }
	FvWaypointID WaypointID() const	{ return m_uiWaypointID; }
	const FvVector3 &Position() const { return m_kPosition; }

	int Compare(const FvWaypointState &kOther) const;
	bool IsGoal(const FvWaypointGoalState &kGoal) const;
	int GetAdjacencyCount() const;
	bool GetAdjacency(int iIndex, FvWaypointState&, const FvWaypointGoalState&) const;
	float DistanceFromParent() const;
	float DistanceToGoal(const FvWaypointGoalState &kGoal) const;
	
protected:
	const FvWaypoint *m_pkWaypoint;
	float m_fDistanceFromParent;
	FvConstSmartPointer<FvWaypointSet> m_spWPSet;
	FvWaypointID m_uiWaypointID;
	FvVector3 m_kPosition;
};

class FvWaypointGoalState : public FvWaypointState
{
public:
	FvWaypointGoalState() : FvWaypointState(), m_fExtraRadius( 0.f ) { }

	float ExtraRadius() const { return m_fExtraRadius; }
	void ExtraRadius( float fValue ) { m_fExtraRadius = fValue; }

private:
	float m_fExtraRadius;
};


#endif // __FvWaypointState_H__
