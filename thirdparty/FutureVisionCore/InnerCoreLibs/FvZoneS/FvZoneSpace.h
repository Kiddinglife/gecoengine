//{future header message}
#ifndef __FvZoneSpace_H__
#define __FvZoneSpace_H__

#ifdef FV_SERVER
#include "FvServerZoneSpace.h"
typedef FvServerZoneSpace FvConfigZoneSpace;
#else // FV_SERVER
#include "FvClientZoneSpace.h"
class FvClientZoneSpace;
typedef FvClientZoneSpace FvConfigZoneSpace;
#endif // !FV_SERVER

#include <FvBaseTypes.h>
#include <FvXMLSection.h>
#include <FvBGTaskManager.h>
#include <FvConcurrency.h>
#include <FvTerrainSettings.h>

#include "FvZoneItem.h"
#include "FvZoneObstacle.h"
#include "FvZoneDefine.h"

#include <map>

class FvCollisionCallback;
FV_ZONE_API extern FvCollisionCallback & s_kCollisionCallbackDefault;
class FvZoneObstacle;

typedef FvUInt32 FvZoneSpaceID;
typedef FvBaseZoneSpace::ZoneSpaceEntryID FvZoneSpaceEntryID;

class FvZoneSpace;
class FvZoneDirMapping;
typedef FvSmartPointer<FvZoneSpace> FvZoneSpacePtr;

const float FV_EXTRA_GRID_RESOLUTION = 5.f;
const float FV_EXTRA_HALF_GRID_RESOLUTION = 2.5f;

class FV_ZONE_API FvZoneSpace : public FvConfigZoneSpace
{
public:
	FvZoneSpace( FvZoneSpaceID id );
	~FvZoneSpace();

	typedef std::map<FvZoneSpaceEntryID,FvZoneDirMapping*> ZoneDirMappings;

	FvZoneDirMapping * AddMapping( FvZoneSpaceEntryID kMappingID, float *pfMatrix,
		const FvString &kGroup, const FvString &kPath, FvXMLSectionPtr spSettings = NULL );
	void AddMappingAsync( FvZoneSpaceEntryID kMappingID, float *pfMatrix,
		const FvString &kGroup, const FvString &kPath );

	FvZoneDirMapping * GetMapping( FvZoneSpaceEntryID mappingID );
	void DelMapping( FvZoneSpaceEntryID mappingID );
	const ZoneDirMappings & GetMappings()
	{ return m_kMappings; }

	void Clear();

	FvZone *FindZoneFromPoint( const FvVector3 &kPoint );

	Column *pColumn( const FvVector3 &kPoint, bool bCanCreate = true );

	Column *pColumn(int iX, int iY);

	float Collide( const FvVector3 &kStart, const FvVector3 &kEnd,
		FvCollisionCallback &cc = s_kCollisionCallbackDefault ) const;

	float Collide( const FvWorldTriangle &kStart, const FvVector3 &kEnd,
		FvCollisionCallback &cc = s_kCollisionCallbackDefault ) const;

	void DumpDebug() const;

	FvBoundingBox GridBounds() const;

	FvZone *GuessZone( const FvVector3 &kPoint, bool bLookInside = false );

	void Emulate( FvZoneSpacePtr pRightfulSpace );

	void EjectLoadedZoneBeforeBinding( FvZone * pChunk );

	void IgnoreZone( FvZone * pChunk );
	void NoticeZone( FvZone * pChunk );

	void ClosestUnloadedZone( float fClosest ) { m_fClosestUnloadedZone = fClosest; }
	float ClosestUnloadedZone()			   { return m_fClosestUnloadedZone;	}

	bool ValidatePendingTask( FvBackgroundTask* pkTask );

	FvTerrainSettingsPtr TerrainSettings() const { return m_spTerrainSettings; }

	template <class T>
	T const* GetExtraData(int iX, int iY) const
	{
		if(m_pkExtraData == NULL)
			return NULL;

		FV_ASSERT(iX >= m_iExtraMinX && iX <= m_iExtraMaxX);
		FV_ASSERT(iY >= m_iExtraMinY && iY <= m_iExtraMaxY);
		return ((T*)m_pkExtraData + (m_iExtraMaxY - iY) * m_stExtraPixelWidth + (iX - m_iExtraMinX));
	}

	template <class T>
	T const* GetExtraData(float fX, float fY) const
	{
		if(m_pkExtraData == NULL)
			return NULL;
		
		FV_ASSERT(sizeof(T) == m_stExtraPixelSize);
		int iX = int(fX/FV_EXTRA_GRID_RESOLUTION);
		int iY = int(fY/FV_EXTRA_GRID_RESOLUTION);
		FV_ASSERT(iX >= m_iExtraMinX && iX <= m_iExtraMaxX);
		FV_ASSERT(iY >= m_iExtraMinY && iY <= m_iExtraMaxY);
		return ((T*)m_pkExtraData + (m_iExtraMaxY - iY) * m_stExtraPixelWidth + (iX - m_iExtraMinX));
	}

private:

	void RecalcGridBounds();

	class LoadMappingTask : public FvBackgroundTask
	{
	public:
		LoadMappingTask( FvZoneSpacePtr spZoneSpace, FvZoneSpaceEntryID kMappingID,
			float *pfMatrix, const FvString& kGroup, const FvString& kPath );

		virtual void DoBackgroundTask( FvBGTaskManager& kMGR );
		virtual void DoMainThreadTask( FvBGTaskManager& kMGR );

	private:
		FvZoneSpacePtr m_spZoneSpace;

		FvZoneSpaceEntryID m_kMappingID;
		FvMatrix m_kMatrix;
		FvString m_kPath;
		FvString m_kGroup;

		FvXMLSectionPtr m_spSettings;
	};

	FvTerrainSettingsPtr m_spTerrainSettings;

	int m_iExtraMinX;
	int m_iExtraMaxX;
	int m_iExtraMinY;
	int m_iExtraMaxY;
	size_t m_stExtraPixelWidth;
	size_t m_stExtraPixelHeight;
	size_t m_stExtraPixelSize;
	void* m_pkExtraData;

	ZoneDirMappings m_kMappings;
	float m_fClosestUnloadedZone;
	std::set< FvBackgroundTask* > m_kBackgroundTasks;

	FvSimpleMutex m_kMappingsLock;
};

class FV_ZONE_API FvZoneDirMapping : public FvSafeReferenceCount
{
public:
	FvZoneDirMapping( FvZoneSpacePtr spSpace, FvMatrix &kMatrix,
		const FvString &kGroup, const FvString &kPath, FvXMLSectionPtr spSettings );
	~FvZoneDirMapping();

	FvZoneSpacePtr pSpace() const { return m_spSpace; }

	const FvMatrix &Mapper() const { return m_kMapper; }
	const FvMatrix &InvMapper() const { return m_kInvMapper; }

	const FvString &Group() const { return m_kGroup; }
	const FvString &Path() const { return m_kPath; }
	const FvString &Name() const { return m_kName; }

	static FvXMLSectionPtr OpenSettings( const FvString &kGroup, const FvString &kPath );

	int MinGridX() const { return m_iMinGridX; }
	int MaxGridX() const { return m_iMaxGridX; }
	int MinGridY() const { return m_iMinGridY; }
	int MaxGridY() const { return m_iMaxGridY; }

	int MinLGridX() const { return m_iMinLGridX; }
	int MaxLGridX() const { return m_iMaxLGridX; }
	int MinLGridY() const { return m_iMinLGridY; }
	int MaxLGridY() const { return m_iMaxLGridY; }

	bool Condemned() { return m_bCondemned; }
	void Condemn() { m_bCondemned = true; }

	FvString OutsideZoneIdentifier( const FvVector3 &kLocalPoint,
		bool bCheckBounds = true ) const;
	FvString OutsideZoneIdentifier( int iX, int iY, bool bCheckBounds = true ) const;
	static bool GridFromZoneName( const FvString &kZoneName, FvInt16 &iX, FvInt16 &iY );

	void Add( FvZone *pkZone );
	void Remove( FvZone *pkZone );
	FvZone *ZoneFromGrid( FvInt16 iX, FvInt16 iY );

private:
	FvZoneSpacePtr m_spSpace;

	FvMatrix m_kMapper;
	FvMatrix m_kInvMapper;

	FvString m_kGroup;
	FvString m_kPath;
	FvString m_kName;

	std::map< std::pair<int, int>, FvZone* > m_kZones;

	int m_iMinGridX;
	int m_iMaxGridX;
	int	m_iMinGridY;
	int m_iMaxGridY;

	int m_iMinLGridX;
	int m_iMaxLGridX;
	int m_iMinLGridY;
	int m_iMaxLGridY;

	bool m_bCondemned;
	bool m_bSingleDir;
};

#endif // __FvZoneSpace_H__