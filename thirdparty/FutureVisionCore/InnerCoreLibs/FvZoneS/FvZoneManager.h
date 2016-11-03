//{future header message}
#ifndef __FvZoneManager_H__
#define __FvZoneManager_H__

#include <FvBaseTypes.h>
#include <FvStringmap.h>
#include <FvSmartPointer.h>
#include <FvVector3.h>
#include <FvMatrix.h>
#include <FvBGTaskManager.h>

#include <map>
#include <set>

#ifndef FV_SERVER
#include <OgreSceneManager.h>
#endif // !FV_SERVER

#include "FvZoneDefine.h"
#include "FvZoneSpace.h"
#include "FvZoneVisitor.h"

class FvZone;
class FvZoneSpace;
class FvZoneItem;
typedef FvUInt32 FvZoneSpaceID;
typedef FvSmartPointer<FvZoneSpace> FvZoneSpacePtr;
class FvZoneDirMapping;
class FvCamera;

#if FV_UMBRA_ENABLE
namespace Umbra
{
	class Camera;
	class Cell;
}
#endif

class FV_ZONE_API FvZoneManager
{
public:
	FvZoneManager();
	~FvZoneManager();

	bool Init();
	bool Fini();

	void VisitZones(FvCamera *pkCamera, FvZoneVisitor *pkVisitor, bool bOnlyShadowCasters = false);
	void VisitTerrains(FvCamera *pkCamera, FvZoneVisitor *pkVisitor);

	void Camera( const FvMatrix &kCameraTransform, FvZoneSpacePtr spSpace, FvZone* pkOverride = NULL );
	const FvMatrix &CameraTrans() const { return m_kCameraTrans; }

	void Tick( float dTime );

	void LoadZoneExplicitly( const FvString &kIdentifier,
		FvZoneDirMapping *pkMapping, bool bIsOverlapper = false );

	FvZone* FindZoneByName( const FvString &kIdentifier, 
		FvZoneDirMapping * pkMapping, bool bCreateIfNotFound = true );
	FvZone* FindZoneByGrid( FvInt16 uiX, FvInt16 uiY, FvZoneDirMapping *pkMapping );

	void LoadZoneNow( FvZone *pkZone );
	void LoadZoneNow( const FvString &kIdentifier, FvZoneDirMapping *pkMapping );

	FvZoneSpacePtr Space( FvZoneSpaceID uiSpaceID, bool bCreateIfMissing = true );

	FvZoneSpacePtr CameraSpace() const;
	FvZone * CameraZone() const			{ return m_pkCameraZone; }

	void ClearAllSpaces( bool bKeepClientOnlySpaces = false );

	bool Busy() const			{ return !m_kLoadingZones.empty(); }

	float MaxLoadPath() const	{ return m_fMaxLoadPath; }
	float MinEjectPath() const	{ return m_fMinEjectPath; }
	void MaxLoadPath( float v )		{ m_fMaxLoadPath = v; }
	void MinEjectPath( float v )	{ m_fMinEjectPath = v; }
	void MaxEjectZones( unsigned int uiMaxEjectZones)	{ m_uiMaxEjectZones = uiMaxEjectZones; }
	void AutoSetPathConstraints( float fFarPlane );
	float ClosestUnloadedZone( FvZoneSpacePtr spSpace ) const;

	void AddSpace( FvZoneSpace * spSpace );
	void DelSpace( FvZoneSpace * spSpace );

	static FvZoneManager *Create();
	static void Destory();
	static FvZoneManager & Instance();

	static int ms_iZonesTraversed;
	static int ms_iZonesVisible;
	static int ms_ZonesReflected;
	static int ms_iVisibleCount;

	static bool ms_bDrawVisibilityBBoxes;
	static bool ms_bEnableZoneCulling;

	bool CheckLoadingZones();

	void SwitchToSyncMode( bool bSync );
	void SwitchToSyncTerrainLoad( bool bSync );

#if FV_UMBRA_ENABLE
	Umbra::Camera *GetUmbraCamera() { return m_pkUmbraCamera; }
	void SetUmbraCamera( Umbra::Camera* pCamera ) { m_pkUmbraCamera = pCamera; }
#endif	
	void AddToCache(FvZone* pkZone, bool bFringeOnly = false);	
	void RemoveFromCache(FvZone* pkZone);
	void ClearCache();

	FvVector3 CameraNearPoint() const;
	FvVector3 CameraAxis(int axis) const;

	static FvString ms_kSpecialConsoleString;

private:

	bool m_bInitted;

	typedef std::map<FvZoneSpaceID,FvZoneSpace*> ZoneSpaces;
	ZoneSpaces m_kSpaces;

	FvMatrix m_kCameraTrans;
	FvZoneSpacePtr m_spCameraSpace;
	FvZone *m_pkCameraZone;

	typedef std::vector<FvZone*> ZoneVector;
	ZoneVector m_kLoadingZones;
	FvZone *m_pkFoundSeed;
	FvZone *m_pkFringeHead;

	std::set<std::pair<FvString, FvZoneDirMapping*> > m_kPendingZones;
	FvSimpleMutex m_kPendingZonesMutex;

	bool Scan();
	bool Blindpanic();
	bool AutoBootstrapSeedZone();

	void LoadZone( FvZone * pkZone, bool bHighPriority );

#if FV_UMBRA_ENABLE
	void CheckCameraBoundaries();
#endif

	float m_fMaxLoadPath;	 
	float m_fMinEjectPath;

	float m_fScanSkippedFor;
	FvVector3 m_kCameraAtLastScan;
	bool m_bNoneLoadedAtLastScan;

	unsigned int m_uiMaxEjectZones;

	unsigned int m_uiWorkingInSyncMode;
	unsigned int m_uiWaitingForTerrainLoad;
#if FV_UMBRA_ENABLE
	// umbra
	Umbra::Camera *m_pkUmbraCamera;
#endif

	static FvZoneManager *ms_pkZoneManager;
};

class FvScopedSyncMode
{
public:
	FvScopedSyncMode();
	~FvScopedSyncMode();

private:
	FvScopedSyncMode(const FvScopedSyncMode &);            
	FvScopedSyncMode &operator=(const FvScopedSyncMode &); 
};

#endif // __FvZoneManager_H__