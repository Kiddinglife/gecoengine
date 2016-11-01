#ifndef __FvWaypointZone_H__
#define __FvWaypointZone_H__

#include <map>

#include "FvWaypoint.h"
#include "FvNavigatorDefine.h"

#include <FvHullTree.h>
#include <FvBoundingBox.h>

class FvWaypointSet;

struct FvWPSpec
{
	FvWaypointSet *pWPSet;
	FvWaypoint *pWaypoint;
};

class FV_NAVIGATOR_API FvWaypointZone : public FvReferenceCount, public FvHullContents
{
public:
	FvWaypointZone(FvZoneID kZoneID);
	~FvWaypointZone();

	const FvZoneID &GetZoneID() const { return m_kZoneID; }
	const FvVector3 &Centre() const	{ return m_kCentre; }
	float Volume() const { return m_fVolume; }
	
	void AddWaypoint(int iSet, FvWaypoint *pkWaypoint);
	void AddAdjacency(FvWaypointZone *pkWaypointZone);
	void DelAdjacency(FvWaypointZone *pkWaypointZone);
	void LinkWaypoints();
	void Clear();

	const FvHullBorder &HullBorder() const;
	void AddPlane(const FvPlane &kPlane);
	void SetBoundingBox( const FvBoundingBox &kBB );
	bool IsOutsideZone() const;
	bool ContainsPoint(const FvVector3 &kPoint);

	FvWaypointSet *FindWaypointSet( unsigned int uiSet );

	bool FindWaypoint(FvWaypointID uiWaypointID, FvWPSpec &kWPSpec);
	bool FindEnclosingWaypoint(const FvVector3 &kPosition, FvWPSpec &kWPSpec);
	bool FindClosestWaypoint(const FvVector3 &kPosition, FvWPSpec &kWPSpec);
	
private:

	typedef std::vector<FvWaypointSet*> WaypointSets;
	typedef std::vector<FvWaypointZone*> WaypointZones;
	
	FvZoneID m_kZoneID;
	WaypointSets m_kWaypointSets;
	WaypointZones m_kAdjacentZones;
	FvHullBorder m_kHullBorder;
	FvBoundingBox m_kBB;
	FvVector3 m_kCentre;
	float m_fVolume;
};

#endif	// __FvWaypointZone_H__
