//{future header message}
#ifndef __FvBaseZoneSpace_H__
#define __FvBaseZoneSpace_H__

#include <FvSmartPointer.h>
#include <FvStringMap.h>
#include <FvVector2.h>
#include <FvVector3.h>
#include <FvMatrix.h>
#include <FvQuadTree.h>

#include <map>

#include "FvZoneDefine.h"

class FvZone;
class FvZoneItem;
class FvBoundingBox;
class FvHullTree;
class FvHullBorder;
class FvHullContents;
class FvBSP;
class FvWorldTriangle;
class FvOutsideLighting;
class FvDataSection;

class FvZoneObstacle;

typedef FvQuadTree< FvZoneObstacle> FvObstacleTree;
typedef FvQuadTreeTraversal< FvZoneObstacle > FvObstacleTreeTraversal;

const float FV_GRID_RESOLUTION = 100.f;
const float FV_MAX_ZONE_HEIGHT = 10000.f;
const float FV_MAX_SPACE_WIDTH = 50000.f;
const float FV_MIN_ZONE_HEIGHT = -10000.f;
extern float g_fGridResolution;	

typedef FvUInt32 FvZoneSpaceID;

const FvZoneSpaceID FV_NULL_ZONE_SPACE = ((FvZoneSpaceID)0);

class FvCollisionCallback;
extern FvCollisionCallback &CollisionCallback_s_default;

typedef FvStringHashMap< std::vector<FvZone*> > FvZoneMap;

typedef std::map< std::pair<int, int>, std::vector<FvZone*> > FvGridZoneMap;


static const FvString FV_SPACE_SETTING_FILE_NAME = "space.config";

class FV_ZONE_API FvBaseZoneSpace : public FvSafeReferenceCount
{
public:
	FvBaseZoneSpace( FvZoneSpaceID id );
	virtual ~FvBaseZoneSpace();

	FvZoneSpaceID ID() const { return m_uiID; }

	void AddZone( FvZone *pkZone );
	FvZone *FindOrAddZone( FvZone *pkZone );
	FvZone *FindZone( const FvString &kIdentifier,
		const FvString &kMappingName );
	void DelZone( FvZone *pkZone );
	void Clear();

	bool InBounds( int iGridX, int iGridY ) const;

	int MinGridX() const { return m_iMinGridX; }
	int MaxGridX() const { return m_iMaxGridX; }
	int MinGridY() const { return m_iMinGridY; }
	int MaxGridY() const { return m_iMaxGridY; }

	float MinCoordX() const { return FV_GRID_RESOLUTION * m_iMinGridX; }
	float MaxCoordX() const { return FV_GRID_RESOLUTION * (m_iMaxGridX + 1); }
	float MinCoordY() const { return FV_GRID_RESOLUTION * m_iMinGridY; }
	float MaxCoordY() const { return FV_GRID_RESOLUTION * (m_iMaxGridY + 1); }

	static int ObstacleTreeDepth() { return ms_iObstacleTreeDepth; }
	static void ObstacleTreeDepth( int iDepth ) { ms_iObstacleTreeDepth = iDepth; }

	class Column
	{
	public:
		Column( int x, int y );
		~Column();

		void AddZone( FvHullBorder &kBorder, FvZone *pkZone );
		bool HasInsideZones() const { return m_pkZoneTree != NULL; }

		FvZone * FindZone( const FvVector3 &kPoint );
		FvZone * FindZoneExcluding( const FvVector3 &kPoint, FvZone *pkNot );

		void AddObstacle( const FvZoneObstacle &kObstacle );
		const FvObstacleTree &Obstacles() const	{ return *m_pkObstacleTree; }
		FvObstacleTree &Obstacles()	{ return *m_pkObstacleTree; }

		void AddDynamicObstacle( const FvZoneObstacle &kObstacle );
		void DelDynamicObstacle( const FvZoneObstacle &kObstacle );

		FvZone *OutsideZone() const { return m_pkOutsideZone; }

		void Stale() { m_bStale = true; }
		bool IsStale() const { return m_bStale; }

		long Size() const;

		void OpenAndSee( FvZone *pkZone );
		void ShutIfSeen( FvZone *pkZone );

	protected:

		typedef std::vector< const FvZoneObstacle * > HeldObstacles;
		typedef std::vector< FvHullContents * >	Holdings;

		FvHullTree *m_pkZoneTree;
		FvObstacleTree *m_pkObstacleTree;
		HeldObstacles m_kHeldObstacles;
		Holdings m_kHoldings;

		FvZone *m_pkOutsideZone;

		bool m_bStale;
		FvZone *m_pkShutTo;

		std::vector< FvZone * > m_kSeen;

	};

	static inline float GridToPoint( int iGrid )
	{
		return iGrid * FV_GRID_RESOLUTION;
	}

	static inline int PointToGrid( float fPoint )
	{
		return static_cast<int>( floorf( fPoint / FV_GRID_RESOLUTION ) );
	}

	static inline float AlignToGrid( float fPoint )
	{
		return GridToPoint( PointToGrid( fPoint ) );
	}

	//! add by Uman, 20100605
	struct ZoneSpaceEntryID
	{
		FvUInt32	m_uiIP;
		FvUInt16	m_uiPort;
		FvUInt16	m_uiSalt;

		ZoneSpaceEntryID():m_uiIP(0), m_uiPort(0), m_uiSalt(0) {}
		ZoneSpaceEntryID(FvUInt32 uiIP, FvUInt16 uiPort) :
		m_uiSalt(0)
		{
			m_uiIP = uiIP;
			m_uiPort = uiPort;
		}
		bool operator<(const ZoneSpaceEntryID& kOther) const
		{
			return (m_uiIP < kOther.m_uiIP) ||
				(m_uiIP == kOther.m_uiIP && (m_uiPort < kOther.m_uiPort));
		}
		bool operator==(const ZoneSpaceEntryID& kOther) const
		{
			return (m_uiIP == kOther.m_uiIP) && (m_uiPort == kOther.m_uiPort);
		}
	};
	struct DataEntryMapKey
	{
		ZoneSpaceEntryID	kEntryID;
		FvUInt16			uiKey;

		bool operator<(const DataEntryMapKey& kOther) const
		{
			return uiKey < kOther.uiKey ||
				(uiKey == kOther.uiKey && kEntryID < kOther.kEntryID);
		}
	};
	typedef std::map<DataEntryMapKey,FvString> DataEntryMap;
	typedef std::pair<FvUInt16,const FvString*> DataValueReturn;

	FvUInt16 DataEntry(const ZoneSpaceEntryID& kEntryID, FvUInt16 uiKey, const FvString& kData);
	DataValueReturn DataRetrieveSpecific(const ZoneSpaceEntryID& kEntryID, FvUInt16 uiKey = FvUInt16(-1));
	const FvString* DataRetrieveFirst(FvUInt16 uiKey);
	DataEntryMap::const_iterator DataRetrieve(FvUInt16 uiKey);
	const DataEntryMap& DataEntries() { return m_kDataEntries; }
	//! add end

	void Emulate( FvSmartPointer<FvBaseZoneSpace> kRightfulSpace );

	void BlurredZone( FvZone *pkZone );
	bool RemoveFromBlurred( FvZone *pkZone );

private:
	FvZoneSpaceID	m_uiID;

	DataEntryMap	m_kDataEntries;	//! add by Uman, 20100605

protected:
	void RecalcGridBounds()		{ }	

	int	m_iMinGridX;
	int	m_iMaxGridX;
	int	m_iMinGridY;
	int m_iMaxGridY;

	FvZoneMap m_kCurrentZones;
	FvGridZoneMap	m_kGridZones;

	std::vector<FvZone*> m_kBlurred;	

	static int ms_iObstacleTreeDepth;
};

#endif //__FvBaseZoneSpace_H__