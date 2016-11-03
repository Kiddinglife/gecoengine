#ifndef __FvWaypointSet_H__
#define __FvWaypointSet_H__

#include "FvWaypoint.h"
#include "FvWaypointZone.h"
#include "FvNavigatorDefine.h"

class FV_NAVIGATOR_API FvWaypointSet
{
public:
	FvWaypointSet( FvWaypointZone &kZone, int iSetNum );
	~FvWaypointSet();

	void Clear();

	FvWaypointZone &Zone() { return m_kZone; }
	const FvWaypointZone &Zone() const { return m_kZone; }
	int SetNum() const { return m_iSetNum; }

	void AddWaypoint( FvWaypoint *pkWaypoint );
	void LinkWaypoints();

	void Bind( FvWaypointZone &kAZone );
	void Loose( FvWaypointZone &kAZone );

	FvWaypoint *FindWaypoint( FvWaypointID uiWaypointID );
	FvWaypoint *FindEnclosingWaypoint( const FvVector3 &kPosition );
	FvWaypoint *FindClosestWaypoint( const FvVector3 &kPosition,
		float &foundDistSquared );

	FvWaypointID EndWaypointID() const;

	unsigned int GetAdjacentSetCount() const;
	FvWaypointSet *GetAdjacentSet(unsigned int iIndex) const;

	void IncRef() const { m_kZone.IncRef(); }
	void DecRef() const { m_kZone.DecRef(); }
	int RefCount() const { return m_kZone.RefCount(); }

private:
	void CacheAdjacentSets() const;

	FvWaypointZone &m_kZone;
	int m_iSetNum;

	typedef std::vector<FvWaypoint*> Waypoints;
	typedef std::vector<FvWaypointSet*> WaypointSets;

	Waypoints m_kWaypoints;

	mutable WaypointSets m_kAdjacentSets;
	mutable bool m_bAdjacentSetsCurrent;

	bool m_bVisited;
};


#endif // __FvWaypointSet_H__
