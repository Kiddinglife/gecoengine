#ifndef __FvNavigator_H__
#define __FvNavigator_H__

#include <FvVector3.h>
#include <FvSmartPointer.h>
#include <FvPowerDefines.h>

#include "FvNavigatorDefine.h"

class ChunkSpace;
class FvZone;
class FvZoneWaypointSet;
typedef FvSmartPointer<FvZoneWaypointSet> FvZoneWaypointSetPtr;

#include "FvZoneWaypointSet.h"

class FV_NAVIGATOR_API FvNavLoc
{
public:
	FvNavLoc();
	FvNavLoc( FvZoneSpace *pkSpace, const FvVector3 &kPoint, float fGirth );
	FvNavLoc( FvZone *pkZone, const FvVector3 &kPoint, float fGirth );
	FvNavLoc( const FvNavLoc &kGuess, const FvVector3 &kPoint );
	~FvNavLoc();

	bool Valid() const { return m_spSet && m_spSet->Zone(); }

	FvZoneWaypointSetPtr Set() const { return m_spSet; }
	int Waypoint() const { return m_iWaypoint; }
	FvVector3 Point() const	{ return m_kPoint; }

	bool IsWithinWP() const;
	void Clip();
	void Clip( FvVector3 &kPoint ) const;
	FvString Desc() const;

private:
	FvZoneWaypointSetPtr m_spSet;
	int	m_iWaypoint;
	FvVector3 m_kPoint;

	friend class FvZoneWaypointState;
	friend class FvNavigator;
};


class FvNavigatorCache;
typedef FvSmartPointer<FvNavigatorCache> FvNavigatorCachePtr;

class FV_NAVIGATOR_API FvNavigator
{
public:
	FvNavigator();
	~FvNavigator();

	bool FindPath( const FvNavLoc &kSrc, const FvNavLoc &kDst, float fMaxDistance,
		FvNavLoc &kWay, bool bBlockNonPermissive, bool &bPassedActivatedPortal );

	bool FindSituationAhead( FvUInt32 uiSituation, const FvNavLoc &kSrc,
		float fRadius, const FvVector3 &kTGT, FvVector3 &kDst );

	int GetWaySetPathSize() const;

	void ClearWPSetCache();
	void ClearWPCache();

	bool m_bInfiniteLoopProblem;

	void GetWaypointPath( const FvNavLoc &kSrcLoc, std::vector<FvVector3> &kWPPath );

	static void AStarSearchTimeLimit( float fSeconds );
	static float AStarSearchTimeLimit();

private:
	FvNavigator( const FvNavigator &kOther );
	FvNavigator & operator = ( const FvNavigator &kOther );

	FvNavigatorCachePtr m_spCache;
};

#endif // __FvNavigator_H__
