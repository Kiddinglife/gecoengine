#include "FvZoneManager.h"
#include "FvZoneSpace.h"
#include "FvZone.h"
#include "FvZoneLoader.h"
#include "FvZoneOverlapper.h"
#include "FvCamera.h"

#include <FvWatcher.h>
#include <FvDebug.h>

#include <vector>
#include <queue>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

//
//extern int g_iZoneTreeToken;
//const int s_iZoneTokenSet = g_iZoneTreeToken;

//PROFILER_DECLARE( FvZoneManager_Tick, "FvZoneManager Tick" );
//PROFILER_DECLARE( FvZoneManager_Draw, "FvZoneManager Draw" );
//
//PROFILER_DECLARE( UMBRA_tick, "UMBRA Tick" );
//PROFILER_DECLARE( UMBRA_resolveVisibility, "UMBRA ResolveVisibility" );
//
//PROFILER_DECLARE( SpeedTree_tick, "SpeedTree Tick" );
//PROFILER_DECLARE( SpeedTree_beginFrame, "SpeedTree BeginFrame" );
//PROFILER_DECLARE( SpeedTree_endFrame, "SpeedTree EndFrame" );
//PROFILER_DECLARE( SpeedTree_drawTrees, "SpeedTree DrawTrees" );

FvZoneManager *FvZoneManager::ms_pkZoneManager = NULL;
FvString FvZoneManager::ms_kSpecialConsoleString;

#ifdef FV_DEBUG

namespace 
{
	const FvUInt32 FV_MAX_LOADING_ZONES	= 1;

	const FvUInt32 FV_MAX_WORTHY_ZONES	= 1;
}

#else

namespace 
{
	const FvUInt32 FV_MAX_LOADING_ZONES	= 20;

	const FvUInt32 FV_MAX_WORTHY_ZONES	= 10;
}

#endif // FV_DEBUG

FvZoneManager::FvZoneManager() : 
m_bInitted( false ),
m_uiWorkingInSyncMode( 0 ),
m_uiWaitingForTerrainLoad( 0 ),
m_kCameraTrans( FvMatrix::IDENTITY ),
m_spCameraSpace( NULL ),
m_pkCameraZone( NULL ),
m_pkFoundSeed( NULL ),
m_pkFringeHead( NULL ),
m_fMaxLoadPath( 450.f ),
m_fMinEjectPath( 600.f ),
m_fScanSkippedFor( 60.f ),
m_kCameraAtLastScan( 0.f, 0.f, -1000000.f ),
m_bNoneLoadedAtLastScan( false ),
#ifdef FV_EDITOR_ENABLED
m_uiMaxEjectZones( 4 )
#else // FV_EDITOR_ENABLED
m_uiMaxEjectZones( 1 )
#endif // FV_EDITOR_ENABLED

#if FV_UMBRA_ENABLE
,m_pkUmbraCamera( NULL )
#endif
{
}

FvZoneManager::~FvZoneManager()
{
	if (m_bInitted) Fini();
}

void FvZoneManager::VisitZones(FvCamera *pkCamera, FvZoneVisitor *pkVisitor, bool bOnlyShadowCasters)
{
	if (m_pkCameraZone != NULL && !m_pkCameraZone->Online())
		m_pkCameraZone = NULL;

	if(bOnlyShadowCasters)
	{
		FvZone::ms_iRenderZone = -1;

		if (m_pkCameraZone)
		{
			FvClientZoneSpace::Column* pkColumn = m_spCameraSpace->pColumn(pkCamera->_GetMainPosition(), false);

			if(pkColumn)
			{
				FvZone* pkZone = pkColumn->OutsideZone();
				if(pkZone)
					pkZone->Visit(pkCamera,pkVisitor);

				FvZone::ms_kLentZoneList.BeginIterator();
				while (!FvZone::ms_kLentZoneList.IsEnd())
				{
					FvRefListNameSpace<FvZone>::List1::iterator iter = FvZone::ms_kLentZoneList.GetIterator();
					FvZone* obj = static_cast<FvZone*>(iter);
					FvZone::ms_kLentZoneList.Next();
					obj->VisitLents(pkCamera, pkVisitor);
				}
			}
		}
	}
	else
	{
		FvZone::ms_iRenderZone = 0;

		if (m_pkCameraZone)
			m_pkCameraZone->Visit(pkCamera,pkVisitor);

		FvZone::ms_kLentZoneList.BeginIterator();
		while (!FvZone::ms_kLentZoneList.IsEnd())
		{
			FvRefListNameSpace<FvZone>::List1::iterator iter = FvZone::ms_kLentZoneList.GetIterator();
			FvZone* obj = static_cast<FvZone*>(iter);
			FvZone::ms_kLentZoneList.Next();
			obj->VisitLents(pkCamera, pkVisitor);
		}		
	}

	FV_ASSERT(!FvZone::ms_kLentZoneList.Size());

	FvZone::NextMark();
	FvZone::NextRenderMark();
	FvZone::ms_uiCurrentCamera = 0;
	FvZone::ms_uiCurrentCameraID = 0;
}

void FvZoneManager::VisitTerrains(FvCamera *pkCamera, FvZoneVisitor *pkVisitor)
{
	if (m_pkCameraZone != NULL && !m_pkCameraZone->Online())
		m_pkCameraZone = NULL;

	if(m_pkCameraZone)
	{
		FvZone::ms_iRenderZone = -1;

		FvClientZoneSpace::Column* pkColumn = m_spCameraSpace->pColumn(*(FvVector3*)&(pkCamera->getLodCamera()->getRealPosition()), false);

		if(pkColumn)
		{
			FvZone* pkZone = pkColumn->OutsideZone();
			if(pkZone)
				pkZone->VisitTerrain(pkCamera,pkVisitor);
		}
	}

	FvZone::NextMark();
	FvZone::NextRenderMark();
	FvZone::ms_uiCurrentCamera = 0;
	FvZone::ms_uiCurrentCameraID = 0;
}

bool bCachingEnabled = true;

bool FvZoneManager::Init()
{
	m_kCameraTrans.SetIdentity();
	m_spCameraSpace = NULL;
	m_pkCameraZone = NULL;

#if !FV_UMBRA_ENABLE
	//FV_WATCH( "Chunks/Counters/Traversed Chunks", FvZoneManager::ms_iZonesTraversed, 
	//	Watcher::WT_READ_ONLY, "Number of chunks traversed to Draw the scene" );
	//FV_WATCH( "Chunks/Counters/Visible Chunks", FvZoneManager::ms_iZonesVisible, 
	//	Watcher::WT_READ_ONLY, "Number of chunks actually drawn after culling" );
	//FV_WATCH( "Chunks/Counters/Reflected Chunks", FvZoneManager::ms_ZonesReflected, 
	//	Watcher::WT_READ_ONLY, "Number of chunks drawn for the reflection passes" );
	//FV_WATCH( "Chunks/Counters/Visible Items", FvZoneManager::ms_iVisibleCount, 
	//	Watcher::WT_READ_ONLY, "Number of pkZone items drawn to compose the whole scene" );
	//FV_WATCH( "Chunks/Counters/Draw passes", FvZoneManager::ms_iDrawPass, 
	//	Watcher::WT_READ_ONLY, "Number of Draw passes (1-main scene + n-reflections)" );
	//FV_WATCH( "Chunks/Visibility Bounding Boxes", FvZoneManager::ms_bDrawVisibilityBBoxes, 
	//	Watcher::WT_READ_WRITE, "Toggles chunks visibility bounding boxes" );
	//FV_WATCH( "Chunks/Enable Visibility Culling", FvZoneManager::ms_bEnableZoneCulling, 
	//	Watcher::WT_READ_WRITE, "Toggles pkZone culling based on their visibility bounding boxes" );
#endif // !FV_UMBRA_ENABLE

	//FV_WATCH( "Chunks/caching", bCachingEnabled, Watcher::WT_READ_WRITE, "Enable caching" );

#if FV_UMBRA_ENABLE

	FvZoneUmbra::Init();

	m_pkUmbraCamera = Umbra::Camera::Create();
#endif // FV_UMBRA_ENABLE

	//Speedtree::SpeedTreeRenderer::Init();

	m_bInitted = true;

	FvZone::Init();

	return true;
}

bool FvZoneManager::Fini()
{
	//FV_GUARD;
	if (!m_bInitted) return false;

	while (!m_kLoadingZones.empty())
	{
		m_kLoadingZones.back()->Mapping()->DecRef();
		m_kLoadingZones.pop_back();
	}

	if (FvUIntPtr(m_pkFoundSeed) > 1)
	{
		m_pkFoundSeed->Mapping()->DecRef();
		delete m_pkFoundSeed;
	}
	m_pkFoundSeed = NULL;

	m_spCameraSpace = NULL;

	while (!m_kSpaces.empty())
	{
		FvZoneSpace * pCS = m_kSpaces.begin()->second;
		pCS->Clear();
		if (!m_kSpaces.empty() && m_kSpaces.begin()->second == pCS)
			m_kSpaces.erase( m_kSpaces.begin() );
	}

	//speedtree::SpeedTreeRenderer::Fini();

	//Moo::VisualCompound::Fini();

#if FV_UMBRA_ENABLE

	m_pkUmbraCamera->Release();

	FvZoneUmbra::Fini();
#endif

	FvZone::Fini();

	FvZoneBoundary::Fini();	

	m_bInitted = false;

	return true;
}

void FvZoneManager::Camera( const FvMatrix &CameraTrans, FvZoneSpacePtr spSpace, FvZone* pkOverride )
{
	//FV_GUARD;
	m_kCameraTrans = CameraTrans;
	m_spCameraSpace = spSpace;
	if (m_spCameraSpace)
	{
		m_pkCameraZone = NULL;	

		if (pkOverride)
			m_pkCameraZone = pkOverride;
		else
		{
			m_spCameraSpace->Focus( m_kCameraTrans.ApplyToOrigin() );

			m_pkCameraZone =
				m_spCameraSpace->FindZoneFromPoint( this->CameraNearPoint() );
		}
		//FV_ASSERT_DEV( !m_pkCameraZone || m_pkCameraZone->Online() );

		//if (m_spCameraSpace && m_spCameraSpace->terrainSettings())
		//{
		//	m_spCameraSpace->terrainSettings()->SetActiveRenderer();
		//}

#if FV_UMBRA_ENABLE
		// Do umbra related work
		if (m_pkCameraZone)
		{
			CheckCameraBoundaries();
		}

		FvMatrix cameraToCell = FvMatrix::IDENTITY;

		if (m_pkCameraZone)
		{
			// Set the cell of the pkZone the Camera is in
			m_pkUmbraCamera->setCell(m_pkCameraZone->getUmbraCell());

			// all cells are in world Space now
			cameraToCell = FvMatrix::IDENTITY;
		}
		else
		{
			// If there is no Camera pkZone set the Camera cell to NULL
			m_pkUmbraCamera->setCell( NULL );
		}

		// Multiply with the Camera transform
		cameraToCell.preMultiply( CameraTrans );

		// Make sure the last column of the matrix uses good
		// values as umbra is quite picky about its matrices
		FvMatrix m = FvMatrix::IDENTITY;
		m[0] = cameraToCell[0];
		m[1] = cameraToCell[1];
		m[2] = cameraToCell[2];
		m[3] = cameraToCell[3];

		// Set the matrix
		m_pkUmbraCamera->setCameraToCellMatrix((Umbra::Matrix4x4&)m);

		// Set up the umbra Camera and frustum
		Umbra::Frustum f;

		const Moo::Camera& Camera = Moo::rc().Camera();
		float hh = (float)tan(Camera.fov() / 2.0) * Camera.nearPlane();
		f.zNear  = Camera.nearPlane();
		f.zFar   = Camera.fFarPlane();
		f.right  = hh*Camera.aspectRatio();
		f.left   = -f.right;
		f.top    = hh;
		f.bottom = -f.top;
		m_pkUmbraCamera->setFrustum(f);

		// Set up the type of culling, this is determined by the
		// umbra helper class
		FvUInt32 properties = Umbra::Camera::VIEWFRUSTUM_CULLING;
		if (UmbraHelper::Instance().occlusionCulling())
			properties |= Umbra::Camera::OCCLUSION_CULLING;
		else
			properties |= Umbra::Camera::DEPTH_PASS;

		m_pkUmbraCamera->setParameters(	(int)Moo::rc().screenWidth(),
			(int)Moo::rc().screenHeight(), 
			properties, 0.25, 0.25 );
#endif
	}
	else
	{
		m_pkCameraZone = NULL;
	}
}

#if FV_UMBRA_ENABLE
namespace
{
	bool lineOnPlaneIntersectsPortal( const FvZoneBoundary::Portal* p, 
		const FvVector3& start, const FvVector3& end )
	{
		//FV_GUARD;
		FvUInt32 oc1 = p->outcode( start );
		FvUInt32 oc2 = p->outcode( end );

		if (oc1 == 0 || oc2 == 0)
			return true;

		if (!(oc1 & oc2))
		{
			FvVector3 diff = start - end;
			FvVector3 op1 = start - p->origin;
			FvVector2 p2d( op1.dotProduct( p->uAxis ), op1.dotProduct( p->vAxis ) );
			FvVector2 diff2d( -diff.dotProduct( p->vAxis ), diff.dotProduct( p->uAxis ) );

			float d = diff2d.dotProduct( p2d );

			bool allLess = true;
			bool allGreater = true;

			for (FvUInt32 i = 0; i < p->points.size();i++)
			{
				if (d < diff2d.dotProduct( p->points[i] ))
				{
					allGreater = false;
				}
				else
				{
					allLess = false;
				}
			}

			// If we have points on both sides of the line, the portal
			// and line are intersecting.
			return allLess == false && allGreater == false;
		}
		return false;
	}

};

void FvZoneManager::CheckCameraBoundaries()
{
	//FV_GUARD;

	FvMatrix localCamera = m_pkCameraZone->transformInverse();
	localCamera.preMultiply( m_kCameraTrans );

	ChunkBoundaries& cb = m_pkCameraZone->bounds();
	for (FvUInt32 i = 0; i < cb.size(); i++)
	{
		for (FvUInt32 j = 0; j < cb[i]->boundPortals_.size(); j++)
		{
			FvZoneBoundary::Portal* p = cb[i]->boundPortals_[j];

			if (p->hasChunk() &&
				!(p->pkZone->IsOutsideZone() && 
				m_pkCameraZone->IsOutsideZone()))
			{
				if (!p->plane.isInFrontOf( localCamera.ApplyToOrigin() ))
				{
					const Moo::Camera& Camera = Moo::rc().Camera();
					float nearUp = (float)tan(Camera.fov() / 2.0) * 
						Camera.nearPlane();
					float nearRight = nearUp * Camera.aspectRatio();
					float nearZ = Camera.nearPlane();

					bool inFront[4];
					FvVector3 points[4];
					points[0] = localCamera.applyPoint( 
						FvVector3( -nearRight, nearUp, nearZ ));
					points[1] = localCamera.applyPoint( 
						FvVector3( nearRight, nearUp, nearZ ));
					points[2] = localCamera.applyPoint( 
						FvVector3( nearRight, -nearUp, nearZ ));
					points[3] = localCamera.applyPoint( 
						FvVector3( -nearRight, -nearUp, nearZ ));

					FvUInt32 nInFront = 0;
					for (FvUInt32 k = 0; k < 4; k++)
					{
						inFront[k] =
							p->plane.isInFrontOf( points[k] );
						nInFront = inFront[k] ? nInFront + 1 : nInFront;
					}

					if (nInFront != 4)
					{
						FvVector3 pointsOnPlane[2];
						FvUInt32 pointIndex = 0;
						for (FvUInt32 k = 0; k < 4; k++)
						{
							if (inFront[k] != inFront[(k + 1) % 4])
							{
								FvVector3 dir = points[(k + 1) % 4] - points[k];
								dir.normalise();
								pointsOnPlane[pointIndex++] =
									p->plane.intersectRay( points[k], dir );
							}
						}
						if (lineOnPlaneIntersectsPortal( p, 
							pointsOnPlane[0], pointsOnPlane[1] ))
						{
							m_pkCameraZone = p->pkZone;
							i = cb.size();
							break;
						}
					}
				}
			}
		}
	}
}
#endif

FvVector3 FvZoneManager::CameraNearPoint() const
{
	return m_kCameraTrans.ApplyToOrigin() /*+
		Moo::rc().Camera().nearPlane() *
		m_kCameraTrans.ApplyToUnitAxisVector( Z_AXIS )*/;
}


FvVector3 FvZoneManager::CameraAxis(int axis) const
{
	return m_kCameraTrans.ApplyToUnitAxisVector( axis );
}

void FvZoneManager::Tick( float dTime )
{
	//FV_GUARD_PROFILER( ChunkManager_tick );

	FvZoneManager::ms_iZonesTraversed = 0;
	FvZoneManager::ms_iZonesVisible   = 0;
	FvZoneManager::ms_ZonesReflected  = 0;
	FvZoneManager::ms_iVisibleCount   = 0;

	if (!m_bInitted) return;

	{
		FvSimpleMutexHolder kSMH( m_kPendingZonesMutex );
		///<YISA> 下面的代码太奇怪了 </YISA>
		while( !m_kPendingZones.empty() )
		{
			LoadZoneExplicitly( m_kPendingZones.begin()->first, m_kPendingZones.begin()->second );
			m_kPendingZones.erase( m_kPendingZones.begin() );
		}
	}

	if( m_uiWorkingInSyncMode )
		return;

	float fFarPlane = 1000/*Moo::rc().Camera().fFarPlane()*/;

	//PROFILER_BEGIN( SpeedTree_tick );
	//speedtree::SpeedTreeRenderer::Tick( dTime );
	//PROFILER_END();

	FvUInt32 uiPreLoadingZonesSize = m_kLoadingZones.size();
	bool anyChanges = this->CheckLoadingZones();

	if (m_pkCameraZone != NULL && !m_pkCameraZone->Online())
		m_pkCameraZone = NULL;
	if (m_pkCameraZone == NULL && m_spCameraSpace)
	{
		m_pkCameraZone =
			m_spCameraSpace->FindZoneFromPoint( this->CameraNearPoint() );
		//FV_ASSERT_DEV( !m_pkCameraZone || m_pkCameraZone->Online() );
	}

	//static DogWatch kZoneScan( "ChunkScan" );
	//kZoneScan.start();
	if (m_pkCameraZone != NULL)
	{
		m_fScanSkippedFor += dTime;
		bool noGo = false;
		if ((m_kCameraTrans.ApplyToOrigin()-m_kCameraAtLastScan).
			LengthSquared() < 100.f)
		{
			if (m_kLoadingZones.size() >= FV_MAX_LOADING_ZONES) noGo = true;
			if (uiPreLoadingZonesSize == 0 && m_bNoneLoadedAtLastScan &&
				m_fScanSkippedFor < 1.f) noGo = true;
		}
		if (!noGo)
		{
			anyChanges |= this->Scan();
			m_fScanSkippedFor = 0.f;
		}

		if (FvUIntPtr(m_pkFoundSeed) > 1)
		{
			FvZoneDirMapping * pkMapping = m_pkFoundSeed->Mapping();
			delete m_pkFoundSeed;	
			pkMapping->DecRef();
			m_pkFoundSeed = NULL;
		}
	}
	else if (m_spCameraSpace)
	{
		anyChanges |= this->AutoBootstrapSeedZone();
	}
	//kZoneScan.stop();

	if (m_spCameraSpace && anyChanges)
		this->Camera( m_kCameraTrans, m_spCameraSpace );

	//static DogWatch s_dwTickSpaces("Spaces");
	//s_dwTickSpaces.start();
	for (ZoneSpaces::iterator it = m_kSpaces.begin(); it != m_kSpaces.end(); it++)
	{
		it->second->Tick( dTime );
	}
	//s_dwTickSpaces.stop();

	ClearCache();

#if FV_UMBRA_ENABLE
	PROFILER_BEGIN( UMBRA_tick );
	FvZoneUmbra::Tick();
	PROFILER_END();
#endif

	FvZone::NextVisibilityMark();
}

typedef std::vector< std::pair< FvZone*,bool> > FvCacheVector;
FvCacheVector kZoneCache;

void FvZoneManager::AddToCache(FvZone* pkZone, bool bFringeOnly)
{
	//FV_GUARD;
	if (bCachingEnabled)
		kZoneCache.push_back( std::make_pair(pkZone,bFringeOnly) );
}

void FvZoneManager::ClearCache()
{
	//FV_GUARD;
	kZoneCache.clear();
}

void FvZoneManager::LoadZoneExplicitly( const FvString &kIdentifier, 
									   FvZoneDirMapping * pkMapping, bool bIsOverlapper /*= false*/ )
{
	//FV_GUARD;
	if( m_uiWorkingInSyncMode )
	{
		LoadZoneNow( kIdentifier, pkMapping );
	}
	else
	{
		if( /*MainThreadTracker::isCurrentThreadMain() &&*/ !bIsOverlapper )
		{
			FvZone *pkZone = FindZoneByName( kIdentifier, pkMapping );

			if (pkZone != NULL && !pkZone->Loaded() && std::find( m_kLoadingZones.begin(),
				m_kLoadingZones.end(), pkZone) == m_kLoadingZones.end())
			{
				this->LoadZone( pkZone, false );
			}
		}
		else
		{
			FvSimpleMutexHolder kSMH( m_kPendingZonesMutex );
			m_kPendingZones.insert( std::make_pair( kIdentifier, pkMapping ) );
		}
	}
}

FvZone* FvZoneManager::FindZoneByGrid( FvInt16 uiX, FvInt16 uiY, FvZoneDirMapping * pkMapping )
{
	//FV_GUARD;
	FV_IF_NOT_ASSERT_DEV( pkMapping )
	{
		return NULL;
	}

	FvZoneSpacePtr spSpace = pkMapping->pSpace();
	std::pair< int, int > coord( uiX, uiY );

	if ( !spSpace->GridZones().count( coord ) )
		return NULL;


	FvGridZoneMap &kGridZones = spSpace->GridZones();
	const std::vector< FvZone* > & mappings = kGridZones[coord];
	FvZone * pkZone = NULL;

	for (unsigned int i = 0; i < mappings.size(); i++)
	{
		if (mappings[i]->Mapping() == pkMapping)
		{
			pkZone = mappings[i];
			break;
		}
	}

	return pkZone;
}

FvZone* FvZoneManager::FindZoneByName( const FvString &kIdentifier, 
									 FvZoneDirMapping * pkMapping, bool bCreateIfNotFound /*= true*/ )
{
	//FV_GUARD;
	FV_IF_NOT_ASSERT_DEV( pkMapping )
	{
		return NULL;
	}

	FvZoneSpacePtr spSpace = pkMapping->pSpace();
	FvZoneMap::const_iterator found = spSpace->Zones().find( kIdentifier );
	FvZone *pkZone = NULL;

	if (found != spSpace->Zones().end())
	{
		for (unsigned int i = 0; i < found->second.size(); i++)
		{
			if (found->second[i]->Mapping() == pkMapping)
			{
				pkZone = found->second[i];
				break;
			}
		}
	}

	if (pkZone == NULL && bCreateIfNotFound)
	{
		pkZone = new FvZone( kIdentifier, pkMapping );

		spSpace->AddZone( pkZone );
	}
	return pkZone;
}

void FvZoneManager::LoadZoneNow( FvZone* pkZone )
{
	//FV_GUARD;
	if (!pkZone)
	{
		FV_CRITICAL_MSG( "Trying to load NULL pkZone at 0x%08X\n", FvUInt32(this) );
	}

	if (pkZone->Loaded())
	{
		FV_ERROR_MSG( "Trying to load Loaded pkZone %s at 0x%08X\n",
			pkZone->Identifier().c_str(), FvUInt32(this) );
		return;
	}

	m_kLoadingZones.push_back( pkZone );

	pkZone->Loading( true );

	pkZone->Mapping()->IncRef();

	FvZoneLoader::LoadNow( pkZone );
}

void FvZoneManager::LoadZoneNow( const FvString &kIdentifier, FvZoneDirMapping * pkMapping )
{
	//FV_GUARD;
	FvZone* pkZone = FindZoneByName( kIdentifier, pkMapping );

	if (pkZone != NULL && !pkZone->Loaded() && std::find( m_kLoadingZones.begin(),
		m_kLoadingZones.end(), pkZone) == m_kLoadingZones.end())
	{
		this->LoadZoneNow( pkZone );
	}
}

bool FvZoneManager::CheckLoadingZones()
{
	//FV_GUARD;
	bool anyChanges = false;

	for (unsigned int i=0; i<m_kLoadingZones.size(); i++)
	{
		FvZone * pkZone = m_kLoadingZones[i];
		if (pkZone->Loaded())
		{
			m_kLoadingZones.erase( m_kLoadingZones.begin() + (i--) );
			pkZone->Loading( false );

			FvZoneDirMapping * pkMapping = pkZone->Mapping();
			if (pkMapping->Condemned())
			{
				pkZone->Space()->EjectLoadedZoneBeforeBinding( pkZone );
				delete pkZone;
				pkMapping->DecRef();
				continue;
			}
			pkMapping->DecRef();

			if( !pkZone->IsOutsideZone() && m_spCameraSpace )
				m_spCameraSpace->Focus( m_kCameraTrans.ApplyToOrigin() );
			pkZone->Bind( true );

			if( pkZone->IsOutsideZone() && m_spCameraSpace && FvZone::OverlapperFinder() )
			{
				std::vector<FvZone*> shells = FvZone::OverlapperFinder()( pkZone );
				if( !shells.empty() )
				{
					m_spCameraSpace->Focus( m_kCameraTrans.ApplyToOrigin() );
					for( std::vector<FvZone*>::iterator iter = shells.begin(); iter != shells.end();
						++iter )
					{
						(*iter)->BindPortals( true );
					}
				}
			}

			if (m_uiWaitingForTerrainLoad)
			{
				//ChunkTerrain* terrain = ChunkTerrainCache::Instance( *pkZone ).pTerrain();

				//while (terrain && terrain->doingBackgroundTask())
				//{
				//	BgTaskManager::Instance().Tick();
				//}
			}

			anyChanges = true;
		}
	}

	return anyChanges;
}

void FvZoneManager::SwitchToSyncMode( bool bSync )
{
	//FV_GUARD;
	if( !bSync )
	{
		//FV_ASSERT_DEV( m_uiWorkingInSyncMode > 0 );
		--m_uiWorkingInSyncMode;
		return;
	}
	else if( bSync && m_uiWorkingInSyncMode > 0 )
	{
		++m_uiWorkingInSyncMode;
		return;
	}
	++m_uiWorkingInSyncMode;
	while( Busy() )
	{
		CheckLoadingZones();
		Sleep( 50 );
	}
}

void FvZoneManager::SwitchToSyncTerrainLoad( bool bSync )
{
	//FV_GUARD;
	if( !bSync )
	{
		//FV_ASSERT_DEV( m_uiWaitingForTerrainLoad > 0 );
		--m_uiWaitingForTerrainLoad;
	}
	else
	{
		++m_uiWaitingForTerrainLoad;
	}
}

static const float FV_CAMERA_INSIDE_OVERLAPPER_BIAS = 10.f;

static const float FV_CAMERA_NEARBY_WIRED_BIAS = 20.f;

static FvZone * FindOverlapper( FvZone * pInChunk, const FvVector3 & pos,
							  FvUInt32 mark )
{
	//FV_GUARD;	
#ifndef FV_EDITOR_ENABLED
	//if (FvZoneOverlappers::Instance.exists( *pInChunk ))
	//{
	//	const FvZoneOverlappers::Overlappers & overlappers =
	//		FvZoneOverlappers::Instance( *pInChunk ).overlappers();
	//	for (unsigned int i = 0; i < overlappers.size(); i++)
	//	{
	//		FvZoneOverlapper * pOverlapper = &*overlappers[i];
	//		if (pOverlapper->pOverlapper()->Online()) continue;		  // Online
	//		if (pOverlapper->pOverlapper()->TraverseMark() == mark) continue; // Loading
	//		if (pOverlapper->bb().Intersects( pos,
	//			FV_CAMERA_INSIDE_OVERLAPPER_BIAS ))
	//		{
	//			FvZone * pkZone = pOverlapper->pOverlapper();
	//			pkZone->TraverseMark( mark );
	//			pkZone->PathSum( 0.f );
	//			return pkZone;
	//		}
	//	}
	//}
#endif
	return NULL;
}

struct GridComp
{
	bool operator()( std::pair<int, int> grid1, std::pair<int, int> grid2 )
	{
		return grid1.first * grid1.first + grid1.second * grid1.second <
			grid2.first * grid2.first + grid2.second * grid2.second;
	}
};

//PROFILER_DECLARE( ChunkManager_scan, "FvZoneManager Scan" );
//PROFILER_DECLARE( ChunkManager_scan2, "FvZoneManager Scan2" );

bool FvZoneManager::Scan()
{
	//FV_GUARD_PROFILER( ChunkManager_scan );

	static int savedMaxLoadGrid = 0;
	static std::vector<std::pair<int, int> > sortedGridBounds;

	FvZoneDirMapping* Mapping = m_pkCameraZone->Mapping();

	if( savedMaxLoadGrid < FvZoneSpace::PointToGrid( m_fMaxLoadPath ) )
	{
		savedMaxLoadGrid = FvZoneSpace::PointToGrid( m_fMaxLoadPath );

		sortedGridBounds.resize( 0 );
		sortedGridBounds.reserve( ( savedMaxLoadGrid * 2 + 1 ) * ( savedMaxLoadGrid * 2 + 1 ) );

		for( int i = -savedMaxLoadGrid; i < savedMaxLoadGrid + 1; ++i )
		{
			for( int j = -savedMaxLoadGrid; j < savedMaxLoadGrid + 1; ++j )
			{
				sortedGridBounds.push_back( std::make_pair( i, j ) );
			}
		}

		std::sort( sortedGridBounds.begin(), sortedGridBounds.end(), GridComp() );
	}

	m_kCameraAtLastScan = m_kCameraTrans.ApplyToOrigin();
	m_bNoneLoadedAtLastScan = true;	
	float fClosestUnloadedZone = 1000000.f;

	FvUInt32	mark = FvZone::NextMark();

	FvZone * pMostWorthy[FV_MAX_WORTHY_ZONES] = { NULL };
	size_t pMostWorthSize = 0;

	//PROFILER_BEGIN( ChunkManager_scan2 );

	int cameraGridX = FvZoneSpace::PointToGrid( m_kCameraAtLastScan.x );
	int cameraGridY = FvZoneSpace::PointToGrid( m_kCameraAtLastScan.y );

	for( std::vector<std::pair<int, int> >::iterator iter = sortedGridBounds.begin();
		iter != sortedGridBounds.end(); ++iter )
	{
		if( FvZoneSpace::GridToPoint( iter->first ) * FvZoneSpace::GridToPoint( iter->first ) +
			FvZoneSpace::GridToPoint( iter->second ) * FvZoneSpace::GridToPoint( iter->second ) >
			m_fMaxLoadPath * m_fMaxLoadPath )
			break;

		int x = cameraGridX + iter->first;
		int y = cameraGridY + iter->second;

		if ( x < Mapping->MinLGridX() || x > Mapping->MaxLGridX() ||
			y < Mapping->MinLGridY() || y > Mapping->MaxLGridY() )
			continue;


		FvZone* pkZone = FindZoneByGrid( x, y, Mapping );

		if ( !pkZone )
		{
			FvString kZoneName = Mapping->OutsideZoneIdentifier( x, y, false );

			pkZone = new FvZone( kZoneName, Mapping );

			Mapping->pSpace()->AddZone( pkZone );
		}

		if( pkZone && !pkZone->Loading() && !pkZone->Loaded() )
		{
			pMostWorthy[pMostWorthSize++] = pkZone;
			if (pMostWorthSize >= FV_MAX_WORTHY_ZONES)
				break;
		}
	}
	//PROFILER_END();

	FvVector3 cameraGridPos( 
		FvZoneSpace::GridToPoint( cameraGridX ),
		FvZoneSpace::GridToPoint( cameraGridY ), 0 );

	std::vector< FvZone * > kFurthestZones;
	kFurthestZones.reserve( m_uiMaxEjectZones );

	for (FvZoneMap::reverse_iterator it = m_spCameraSpace->Zones().rbegin();
		it != m_spCameraSpace->Zones().rend() && kFurthestZones.size() < m_uiMaxEjectZones;
		++it)
	{
		std::vector<FvZone*>::reverse_iterator i;
		for (i = it->second.rbegin();
			i != it->second.rend() && kFurthestZones.size() < m_uiMaxEjectZones; ++i)
		{
			if (!(*i)->Online()) continue;
			if (!(*i)->Removable()) continue;
			FvVector3 origin = (*i)->Transform().ApplyToOrigin();
			if( ( origin.x - cameraGridPos.x ) * ( origin.x - cameraGridPos.x ) + 
				( origin.y - cameraGridPos.y ) * ( origin.y - cameraGridPos.y ) <=
				m_fMinEjectPath * m_fMinEjectPath )
				continue;
			if (!(*i)->IsOutsideZone())
			{
				FvVector3 pos = (*i)->Transform().ApplyToOrigin();
				FvString identifier = (*i)->Mapping()->OutsideZoneIdentifier( pos );
				FvZone* pkZone = FindZoneByName( identifier, (*i)->Mapping(), false );
				if (pkZone != NULL && pkZone->Loaded())
					continue;
			}
			else
			{
				bool bAllShellsLoaded = true;
				const FvZoneOverlappers::ZoneOverlappers &kOverlappers =
					FvZoneOverlappers::ms_kInstance( **i ).Overlappers();
				for (unsigned int i = 0; i < kOverlappers.size(); i++)
				{
					FvZoneOverlapper *pkOverlapper = &*kOverlappers[i];
					if (pkOverlapper->Overlapper()->Loading())
					{
						bAllShellsLoaded = false;
						break;
					}
				}
				if( !bAllShellsLoaded )
					continue;
			}

			if ((*i)->BoundingBox().Intersects(
				cameraGridPos, FV_CAMERA_NEARBY_WIRED_BIAS )) continue;

			kFurthestZones.push_back( *i );
		}
		if (i != it->second.rend()) break;
	}

	for 
		(
		size_t pMostWorthIdx = 0;
	pMostWorthIdx < pMostWorthSize;
	++pMostWorthIdx
		)
	{
		if (m_kLoadingZones.size() < FV_MAX_LOADING_ZONES)
		{
			FvZone *pkZone = pMostWorthy[pMostWorthIdx];
			this->LoadZone( pkZone, pkZone->PathSum() == 0 );
			m_bNoneLoadedAtLastScan = false;

			FvVector3 origin = pkZone->Transform().ApplyToOrigin();
			float dist = ( ( origin.x - m_kCameraAtLastScan.x ) * ( origin.x - m_kCameraAtLastScan.x ) + 
				( origin.y - m_kCameraAtLastScan.y ) * ( origin.y - m_kCameraAtLastScan.y ) );

			fClosestUnloadedZone = FV_MIN( fClosestUnloadedZone, sqrtf(dist) );
		}
		else
		{
			break;
		}
	}

	if ( m_spCameraSpace )
	{
		m_spCameraSpace->ClosestUnloadedZone( fClosestUnloadedZone );
	}

	bool anyChanges = false;

	if (!kFurthestZones.empty())
	{
		m_bNoneLoadedAtLastScan = false;

		for (std::vector< FvZone * >::iterator it = kFurthestZones.begin();
			it != kFurthestZones.end(); ++it)
		{
			FV_IF_NOT_ASSERT_DEV( (*it)->Removable() )
			{
				continue;
			}

			(*it)->Loose( false );

			(*it)->Eject();
		}

		anyChanges = true;
	}

	return anyChanges;
}

bool FvZoneManager::Blindpanic()
{
	//FV_GUARD;
	FvUInt32	mark = FvZone::NextMark();

	for (ZoneVector::iterator it = m_kLoadingZones.begin();
		it != m_kLoadingZones.end();
		it++)
	{
		(*it)->TraverseMark( mark );
		(*it)->PathSum( -1 );
	}

	const FvVector3 & cameraPoint = m_kCameraTrans.ApplyToOrigin();

	FvZone * pFurthest = NULL;
	FvZone * pClosest = NULL;

	for (FvZoneMap::iterator it = m_spCameraSpace->Zones().begin();
		it != m_spCameraSpace->Zones().end();
		it++)
	{
		for (unsigned int i = 0; i < it->second.size(); i++)
		{
			FvZone * pkZone = it->second[i];
			if (!pkZone->Online()) continue;
			if (pkZone->TraverseMark() == mark) continue;

			pkZone->TraverseMark( mark );
			pkZone->PathSum( (pkZone->Centre() - cameraPoint).LengthSquared() );

			if ((pFurthest == NULL || pkZone->PathSum() > pFurthest->PathSum())
				&& pkZone->Removable() && !pkZone->BoundingBox().Intersects(
				cameraPoint, FV_CAMERA_NEARBY_WIRED_BIAS ))
			{
				pFurthest = pkZone;
			}

			float	canDist = 0.f;
			FvZone * pCandidate = pkZone->FindClosestUnloadedZoneTo(
				cameraPoint, &canDist );

			if (pCandidate != NULL && pCandidate->TraverseMark() != mark &&
				(pClosest == NULL || canDist < pClosest->PathSum()))
			{
				//FV_ASSERT_DEV( !pCandidate->Online() );
				pClosest = pCandidate;
				pClosest->PathSum( canDist );
			}
		}
	}

	bool anyChanges = false;

	if (pFurthest != NULL && pFurthest->PathSum() > m_fMinEjectPath*m_fMinEjectPath)
	{
		//FV_ASSERT_DEV( pFurthest->Removable() );

		if( pFurthest->Removable() )
		{
			m_bNoneLoadedAtLastScan = false;

			pFurthest->Loose( false );

			pFurthest->Eject();

			anyChanges = true;
		}
	}

	if (pClosest == NULL)
	{
		return anyChanges;
	}

	if (m_kLoadingZones.size() < 3)
	{
		this->LoadZone( pClosest, true );
	}

	return anyChanges;
}

bool FvZoneManager::AutoBootstrapSeedZone()
{
	//FV_GUARD;
	if (m_pkFoundSeed == NULL)
	{
		FV_TRACE_MSG( "FvZoneManager::AutoBootstrapSeedZone: "
			"seed pkZone NULL so starting search\n" );
		(FvUInt32&)m_pkFoundSeed = 1;
		FvZoneLoader::FindSeed( &*m_spCameraSpace, this->CameraNearPoint(),
			m_pkFoundSeed );
	}
	else if (FvUInt32(m_pkFoundSeed) != 1)
	{
		FV_TRACE_MSG( "FvZoneManager::AutoBootstrapSeedZone: "
			"seed pkZone determined: %s\n", m_pkFoundSeed->Identifier().c_str() );
		FvZone * pkZone = m_pkFoundSeed;
		m_pkFoundSeed = NULL;

		FvZoneDirMapping * pkMapping = pkZone->Mapping();
		if (pkMapping->Condemned())
		{
			delete pkZone;
			pkMapping->DecRef();
		}
		else
		{
			pkMapping->DecRef();

			if (pkZone->Space() == m_spCameraSpace)
			{
				pkZone = m_spCameraSpace->FindOrAddZone( pkZone );
				if (!pkZone->Loading() && !pkZone->Loaded())
				{
					this->LoadZone( pkZone, true );
					FV_TRACE_MSG( "FvZoneManager::AutoBootstrapSeedZone: "
						"seed pkZone submitted for Loading\n" );
				}

				m_spCameraSpace->ClosestUnloadedZone( 0.f );
			}
			else
			{
				delete pkZone;
			}
		}
	}

	return false;
}


//PROFILER_DECLARE( FvZoneManager_LoadZone, "FvZoneManager LoadZone" );
void FvZoneManager::LoadZone( FvZone * pkZone, bool bHighPriority )
{
	//FV_GUARD_PROFILER( FvZoneManager_LoadZone );

	if( m_uiWorkingInSyncMode )
	{
		LoadZoneNow( pkZone );
		return;
	}
	if (!pkZone)
	{
		FV_CRITICAL_MSG( "Trying to load NULL pkZone at 0x%08X\n", FvUInt32(this) );
	}

	if (pkZone->Loaded())
	{
		FV_ERROR_MSG( "Trying to load Loaded pkZone %s at 0x%08X\n",
			pkZone->Identifier().c_str(), FvUInt32(this) );
		return;
	}
	//FV_ASSERT_DEV( pkZone && !pkZone->Loaded() );

	m_kLoadingZones.push_back( pkZone );
	pkZone->Loading( true );

	pkZone->Mapping()->IncRef();
	
	FvZoneLoader::Load(pkZone,bHighPriority ? FvBGTaskManager::HIGH : FvBGTaskManager::DEFAULT );
}

FvZoneSpacePtr FvZoneManager::Space( FvZoneSpaceID id, bool bCreateIfMissing )
{
	ZoneSpaces::iterator it = m_kSpaces.find( id );
	if (it != m_kSpaces.end()) return it->second;

	if (bCreateIfMissing)
	{
		if ( id != FV_NULL_ZONE_SPACE )
		{
			return new FvZoneSpace( id );
		}
		else
		{			
			FV_ERROR_MSG("Somebody tried to create Space with NULL Space ID %d\n",
				FV_NULL_ZONE_SPACE );
		}
	}
	return NULL;
};

FvZoneSpacePtr FvZoneManager::CameraSpace() const
{
	return m_spCameraSpace;
}

void FvZoneManager::ClearAllSpaces( bool bKeepClientOnlySpaces )
{
	//FV_GUARD;
	if (m_kSpaces.empty())
		return;	

	{
		FvSimpleMutexHolder kSMH( m_kPendingZonesMutex );
		m_kPendingZones.clear();
	}

	
	int numSpaces = m_kSpaces.size();
	int i = 0;
	FvZoneSpaceID * spaceIDs = new FvZoneSpaceID[ numSpaces ];
	for (ZoneSpaces::iterator it = m_kSpaces.begin(); it != m_kSpaces.end(); it++)
	{
		spaceIDs[i++] = it->first;
	}

	for (i = 0; i < numSpaces; i++)
	{
		FvZoneSpacePtr spSpace = this->Space( spaceIDs[i], false );
		if (spSpace && (spSpace->ID() < (1<<30) || !bKeepClientOnlySpaces))
			spSpace->Clear();
	}

	delete [] spaceIDs;
}

FvZoneManager *FvZoneManager::Create()
{
	if(ms_pkZoneManager == NULL)
		ms_pkZoneManager = new FvZoneManager;
	return ms_pkZoneManager;
}

void FvZoneManager::Destory()
{
	delete ms_pkZoneManager;
}

FvZoneManager &FvZoneManager::Instance()
{
	FV_ASSERT(ms_pkZoneManager);
	return *ms_pkZoneManager;
}

void FvZoneManager::AutoSetPathConstraints( float fFarPlane )
{
	//FV_GUARD;
	const float diagonalGridResolution = 
		sqrtf(FV_GRID_RESOLUTION*FV_GRID_RESOLUTION +
		FV_GRID_RESOLUTION*FV_GRID_RESOLUTION);

	m_fMaxLoadPath = 
		sqrtf( fFarPlane*fFarPlane*2 ) + diagonalGridResolution;

	
	const float EJECT_PATH_EPSILON = 1.01f;

	m_fMinEjectPath = 
		m_fMaxLoadPath + diagonalGridResolution * EJECT_PATH_EPSILON;

	m_fMaxLoadPath = 400.f;
}

float FvZoneManager::ClosestUnloadedZone( FvZoneSpacePtr spSpace ) const
{
	return spSpace->ClosestUnloadedZone();
}

int FvZoneManager::ms_iZonesTraversed = 0;
int FvZoneManager::ms_iZonesVisible = 0;
int FvZoneManager::ms_ZonesReflected = 0;
int FvZoneManager::ms_iVisibleCount = 0;
bool FvZoneManager::ms_bDrawVisibilityBBoxes = false;
bool FvZoneManager::ms_bEnableZoneCulling   = true;

void FvZoneManager::AddSpace( FvZoneSpace * spSpace )
{
	//FV_GUARD;
	FV_IF_NOT_ASSERT_DEV( m_kSpaces.find( spSpace->ID() ) == m_kSpaces.end() )
	{
		return;
	}

	m_kSpaces.insert( std::make_pair( spSpace->ID(), spSpace ) );

	if (m_spCameraSpace == NULL)
	{
		this->Camera( FvMatrix::IDENTITY, spSpace );
	}
}

void FvZoneManager::DelSpace( FvZoneSpace * spSpace )
{
	//FV_GUARD;
	if (!m_bInitted) return;

	//m_kPendingZonesMutex.grab();

	const FvZoneSpace::ZoneDirMappings &kMappings = spSpace->GetMappings();
	bool bChanged = true;
	while( bChanged )
	{
		bChanged = false;
		///<YISA> 下面的代码比较奇怪 </YISA>
		for( std::set<std::pair<FvString, FvZoneDirMapping*> >::iterator kIter = m_kPendingZones.begin();
			kIter != m_kPendingZones.end() && !bChanged; ++kIter )
		{
			for( FvZoneSpace::ZoneDirMappings::const_iterator kMPIter = kMappings.begin();
				kMPIter != kMappings.end(); ++kMPIter )
			{
				if( kIter->second == kMPIter->second )
				{
					m_kPendingZones.erase( kIter );
					bChanged = true;
					break;
				}
			}
		}
	}

	//m_kPendingZonesMutex.give();

	ZoneSpaces::iterator found = m_kSpaces.find( spSpace->ID() );
	//FV_ASSERT_DEV( found != m_kSpaces.end() );
	//FV_ASSERT_DEV( spSpace->refCount() == 0 );
	if( found != m_kSpaces.end() )
		m_kSpaces.erase( found );
}

FvScopedSyncMode::FvScopedSyncMode()
{
	FvZoneManager::Instance().SwitchToSyncMode(true);
}

FvScopedSyncMode::~FvScopedSyncMode()
{
	FvZoneManager::Instance().SwitchToSyncMode(false);
}
