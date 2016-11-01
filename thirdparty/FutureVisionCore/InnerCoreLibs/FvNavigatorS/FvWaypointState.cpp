#include "FvWaypointState.h"

#include <FvLine.h>

FvWaypointState::FvWaypointState() :
	m_pkWaypoint(NULL),
	m_fDistanceFromParent(0.0f),
	m_spWPSet(NULL),
	m_uiWaypointID(0)
{
}

void FvWaypointState::SetWaypoint(const FvWaypoint *pkWaypoint)
{
	m_pkWaypoint = pkWaypoint;
}

void FvWaypointState::SetPosition(const FvVector3 &kPosition)
{
	m_kPosition = kPosition;
}

void FvWaypointState::SetWPSet(const FvWaypointSet *pkWPSet)
{
	m_spWPSet = pkWPSet;
}

void FvWaypointState::SetWaypointID(FvWaypointID uiWaypointID)
{
	m_uiWaypointID = uiWaypointID;
}

int FvWaypointState::Compare(const FvWaypointState &kOther) const
{
	if (m_spWPSet == kOther.m_spWPSet)
		return m_uiWaypointID - kOther.m_uiWaypointID;
	else
		return FvIntPtr(&*m_spWPSet) - FvIntPtr(&*kOther.m_spWPSet);
}

bool FvWaypointState::IsGoal(const FvWaypointGoalState &kGoal) const
{
	if (kGoal.m_spWPSet && kGoal.m_spWPSet != m_spWPSet)
		return false;

	if (kGoal.m_uiWaypointID && kGoal.m_uiWaypointID != m_uiWaypointID)
		return false;

	return true;
}

int FvWaypointState::GetAdjacencyCount() const
{
	if(m_pkWaypoint)
		return m_pkWaypoint->VertexCount();
	else
		return 0;
}

bool FvWaypointState::GetAdjacency(int iIndex,
		FvWaypointState &kAdjacency, const FvWaypointGoalState &kGoal) const
{
	FvVector2 src, dst, p1, p2, movementVector, next;
	float p, cp1, cp2;

	const FvWaypointSet *pAdjSet = m_pkWaypoint->AdjacentWaypointSet(iIndex);
	if (m_pkWaypoint->AdjacentWaypoint(iIndex) == NULL && pAdjSet == NULL)
		return false;

	src.x = m_kPosition.x;
	src.y = m_kPosition.y;
	dst.x = kGoal.m_kPosition.x;
	dst.y = kGoal.m_kPosition.y;
	movementVector = dst - src;

	p1 = m_pkWaypoint->VertexPosition(iIndex);
	p2 = m_pkWaypoint->VertexPosition((iIndex + 1) % m_pkWaypoint->VertexCount());

	if (kGoal.ExtraRadius() > 0.f)
	{
		const float ger = kGoal.ExtraRadius();
		FvVector2 edir( p2 - p1 );
		const float elen = edir.Length();

		if (elen < ger * 2.f) return false;

		edir *= ger / elen;
		p1 += edir;
		p2 -= edir;
	}

	cp1 = movementVector.CrossProduct(p1 - src);
	cp2 = movementVector.CrossProduct(p2 - src);

	if(cp1 > 0.0f && cp2 < 0.0f)
	{
		FvLine moveLine(src, dst, true);
		FvLine edgeLine(p1, p2, true);
		p = moveLine.Intersect(edgeLine);
		next = moveLine.Param(p);
	}
	else if(fabs(cp1) < fabs(cp2))
	{
		next = p1;
	}
	else
	{
		next = p2;
	}

	kAdjacency.m_spWPSet = (pAdjSet == NULL) ? m_spWPSet : pAdjSet;
	kAdjacency.m_uiWaypointID = m_pkWaypoint->AdjacentID(iIndex);
	kAdjacency.m_pkWaypoint = m_pkWaypoint->AdjacentWaypoint(iIndex);
	kAdjacency.m_kPosition.x = next.x;
	kAdjacency.m_kPosition.y = next.y;
	kAdjacency.m_kPosition.z = m_kPosition.z;
	kAdjacency.m_fDistanceFromParent = (m_kPosition - kAdjacency.m_kPosition).Length();
	return true;
}

float FvWaypointState::DistanceFromParent() const
{
	return m_fDistanceFromParent;
}

float FvWaypointState::DistanceToGoal(const FvWaypointGoalState &kGoal) const
{
	return (kGoal.m_kPosition - m_kPosition).Length();
}
