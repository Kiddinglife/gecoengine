#include "FvWaypointSetState.h"
#include "FvWaypointZone.h"

#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT(0);

FvWaypointSetState::FvWaypointSetState() :
	m_spWPSet( NULL ),
	m_fDistanceFromParent( 0.0f )
{
}

void FvWaypointSetState::SetWaypointSet(const FvWaypointSet *pkWPSet)
{
	m_spWPSet = pkWPSet;
}

void FvWaypointSetState::SetPosition(const FvVector3 &kPosition)
{
	m_kPosition = kPosition;
}

int FvWaypointSetState::Compare(const FvWaypointSetState &kOther) const
{
	return int(FvUIntPtr(&*m_spWPSet) > FvUIntPtr(&*kOther.m_spWPSet)) -
		int(FvUIntPtr(&*m_spWPSet) < FvUIntPtr(&*kOther.m_spWPSet));
}

bool FvWaypointSetState::IsGoal(const FvWaypointSetState &kGoal) const
{
	return m_spWPSet == kGoal.m_spWPSet;
}

int FvWaypointSetState::GetAdjacencyCount() const
{
	return m_spWPSet->GetAdjacentSetCount();
}

bool FvWaypointSetState::GetAdjacency(int iIndex, FvWaypointSetState &kNewState,
		const FvWaypointSetState&) const
{
	FvWaypointSet *pkWPSet = m_spWPSet->GetAdjacentSet( iIndex );
	if (pkWPSet == NULL) return false;

	FV_ASSERT( &pkWPSet->Zone() != &m_spWPSet->Zone() );

	kNewState.m_spWPSet = pkWPSet;
	kNewState.m_kPosition = pkWPSet->Zone().Centre();
	kNewState.m_fDistanceFromParent = (kNewState.m_kPosition - m_kPosition).Length();

	return true;
}

float FvWaypointSetState::DistanceFromParent() const
{
	return m_fDistanceFromParent;
}

float FvWaypointSetState::DistanceToGoal(const FvWaypointSetState &kGoal) const
{
	return (kGoal.m_kPosition - m_kPosition).Length();
}
