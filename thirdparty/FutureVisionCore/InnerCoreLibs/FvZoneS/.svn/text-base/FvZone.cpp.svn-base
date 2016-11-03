#include "FvZone.h"
#ifdef FV_SERVER
#include "FvServerZoneModel.h"
#else // FV_SERVER
#include "FvZoneTerrain.h"
#include "FvZoneModel.h"
#endif // !FV_SERVER
#include "FvZoneSpace.h"
#include "FvZoneSerializer.h"
#ifndef FV_SERVER
#include "FvCamera.h"
#include "FvZoneManager.h"
#include "FvRenderManager.h"
#include <FvOcclusionQueryRenderable.h>
#endif // !FV_SERVER

#include <FvHullTree.h>
#include <FvWatcher.h>
#include <FvDebug.h>
#include <FvConcurrency.h>
#include <FvPowerDefines.h>

#include <OgreResourceGroupManager.h>

#include <set>
#include <queue>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

#ifndef FV_SERVER

//PROFILER_DECLARE( Chunk_tick, "FvZone Tick" );
//PROFILER_DECLARE( Chunk_tick2, "FvZone Tick 2" );

namespace 
{
	bool s_bCullDebugEnable = false;

#if FV_ENABLE_CULLING_HUD
	float s_bCullHUDDist = 2500;

	typedef std::avector< std::pair<FvMatrix, FvBoundingBox> > BBoxVector;
	BBoxVector s_kTraversedZones;
	BBoxVector s_kVisibleZones;
	BBoxVector s_kFringeZones;
	BBoxVector s_kReflectedZones;

	typedef std::map< FvZone *, FvBoundingBox > BBoxMap;
	BBoxMap s_kDebugBoxes;

	void ZonesDrawCullingHUDPriv();
#endif // FV_ENABLE_CULLING_HUD

	//class FvCullDebugTask : public MainLoopTask
	//{
	//	virtual void Draw()
	//	{
	//		ZonesDrawCullingHUD();
	//	}
	//};
	//std::auto_ptr<FvCullDebugTask> s_kCullDebugInstance;

} 
#endif // FV_SERVER

FvUInt32	FvZone::ms_uiNextMark           = 0;
FvUInt32	FvZone::ms_uiRenderMark			= 0;
FvUInt32	FvZone::ms_uiNextVisibilityMark = 0;
FvUInt32	FvZone::ms_uiCurrentCamera		= 0;
FvUInt32	FvZone::ms_uiCurrentCameraID	= 0;
FvInt32 	FvZone::ms_iRenderZone			= 0;
FvZone::OverlapperFinderFun FvZone::ms_kOverlapperFinder = NULL;
int FvZone::ms_iNextCacheID = 0;
FvUInt32	FvZone::ms_uiInstanceCount = 0;
FvUInt32	FvZone::ms_uiInstanceCountPeak = 0;
FvRefListNameSpace<FvZone>::List1 FvZone::ms_kLentZoneList;

FvZone::FvZone( const FvString &kIdentifier, FvZoneDirMapping *pkMapping ) :
m_kIdentifier( kIdentifier ),
m_pkMapping( pkMapping ),
m_pkSpace( &*pkMapping->pSpace() ),
m_IsOutsideZone( *(kIdentifier.end()-1) == 'o' ),
m_bHasInternalZones( false ),
m_bRatified( false ),
m_bLoading( false ),
m_bLoaded( false ),
m_bOnline( false ),
m_iFocusCount( 0 ),
m_kTransform( FvMatrix::IDENTITY ),
m_kTransformInverse( FvMatrix::IDENTITY ),
#ifndef FV_SERVER
m_kVisibilityBox( FvBoundingBox::ms_kInsideOut ),
m_kVisibilityBoxCache( FvBoundingBox::ms_kInsideOut ),
m_uiVisibilityBoxMark( ms_uiNextMark - 128 ), 
m_uiVisitIndex(0),
#endif // FV_SERVER
m_uiVisitMark( ms_uiNextMark - 128 ),
m_uiTraverseMark( ms_uiNextMark - 128 ),
m_uiReflectionMark( ms_uiNextMark - 128 ),
m_ppCaches( new FvZoneCache *[ FvZone::ms_iNextCacheID ] ),
m_pkFringeNext( NULL ),
m_pkFringePrev( NULL ),
m_bInTick( false ),
m_bRemovable( true ),
m_i32MainBoundaryID(-1),
m_uiCurrentGroupID(0),
m_bUseOutsideLight(true),
m_u32LentVisitItemPointer(0)
#if FV_UMBRA_ENABLE
,
m_pkUmbraCell( NULL )
#endif
{
	//FV_GUARD;
	for (int i = 0; i < FvZone::ms_iNextCacheID; i++) m_ppCaches[i] = NULL;

	if( IsOutsideZone() )
	{
		pkMapping->GridFromZoneName( this->Identifier(), m_iX, m_iY );

		float fXF = float(m_iX) * FV_GRID_RESOLUTION;
		float fYF = float(m_iY) * FV_GRID_RESOLUTION;

		m_kLocalBB = FvBoundingBox( FvVector3( 0.f, 0.f, 0.f ),
			FvVector3( FV_GRID_RESOLUTION, FV_GRID_RESOLUTION, 0.f ) );

		m_kBoundingBox = FvBoundingBox( FvVector3( fXF, fYF, 0.f ),
			FvVector3( fXF + FV_GRID_RESOLUTION, fYF + FV_GRID_RESOLUTION, 0.f ) );

		m_kTransform.SetTranslate( fXF, fYF, 0.f );
		m_kTransform.PostMultiply( pkMapping->Mapper() );
		m_kTransformInverse.Invert( m_kTransform );

		FvVector3 kMin = this->m_kLocalBB.MinBounds();
		FvVector3 kMax = this->m_kLocalBB.MaxBounds();
		kMin.y = +std::numeric_limits<float>::max();
		kMax.y = -std::numeric_limits<float>::max();

		m_kCentre = m_kBoundingBox.Centre();

#ifndef FV_SERVER
		this->m_kVisibilityBox.SetBounds(kMin,kMax);
#endif // FV_SERVER
	}

	ms_uiInstanceCount++;
	if ( ms_uiInstanceCount > ms_uiInstanceCountPeak )
		ms_uiInstanceCountPeak = ms_uiInstanceCount;

#ifndef FV_SERVER

	FvRenderManager* pkRenderManager = (FvRenderManager*)(Ogre::Root::getSingleton()._getCurrentSceneManager());
	FV_ASSERT(pkRenderManager);

	const Ogre::MeshPtr& spMesh = pkRenderManager->GetOcclusionMesh();
	const Ogre::MaterialPtr& spMaterial = pkRenderManager->GetOcclusionMaterial();

	if((!spMesh.isNull()) && (!spMaterial.isNull()))
	{
		m_pkOcclusion = new FvOcclusionQueryAABB(spMesh, spMaterial);
	}
	else
	{
		m_pkOcclusion = NULL;
	}
#endif
	
}

FvZone::~FvZone()
{
	//FV_GUARD;
	if (this->Online()) this->Loose( false );

	if (this->Loaded()) this->Eject();

	for (int i = 0; i < FvZone::ms_iNextCacheID; i++)
	{
		if (m_ppCaches[i] != NULL)
		{
			delete m_ppCaches[i];
			m_ppCaches[i] = NULL;
		}
	}
	delete [] m_ppCaches;

	if (this->Ratified()) m_pkSpace->DelZone( this );

	ms_uiInstanceCount--;

#ifndef FV_SERVER
	FV_SAFE_DELETE(m_pkOcclusion);
#endif
}

void FvZone::Ratify()
{
	m_bRatified = true;
}

void FvZone::Init()
{
	//FV_GUARD;	
#ifndef FV_SERVER
#if FV_ENABLE_CULLING_HUD
#if !FV_UMBRA_ENABLE
	FV_WATCH( "Chunks/FvZone Culling HUD", s_bCullDebugEnable,
		FvWatcher::WT_READ_WRITE, "Toggles the chunks culling debug HUD" );

	FV_WATCH( "Chunks/Culling HUD Far Distance", s_bCullHUDDist,
		FvWatcher::WT_READ_WRITE, "Sets the scale of the chunks culling debug HUD" );

	s_kCullDebugInstance.reset(new FvCullDebugTask);
	///<yu_kai>
	//MainLoopTasks::root().add(
	//	s_kCullDebugInstance.get(),
	//	"World/Debug FvZone Culling", ">App", NULL );
	MainLoopTasks::root().add(
		s_kCullDebugInstance.get(), false,
		"World/Debug FvZone Culling", ">App", NULL );
	///</yu_kai>
#endif // !FV_UMBRA_ENABLE
#endif // FV_ENABLE_CULLING_HUD

	//FV_WATCH( "Chunks/Loaded Chunks", ms_uiInstanceCount, Watcher::WT_READ_ONLY, "Number of loaded chunks" );
#endif // FV_SERVER
}


void FvZone::Fini()
{
	//FV_GUARD;
}

bool FvZone::Load()
{
	//FV_GUARD;	
#ifndef FV_EDITOR_ENABLED
	FV_ASSERT_DEV( !m_bLoaded );
#endif

	m_bHasInternalZones = false;

	Ogre::DataStreamPtr spDataStream = Ogre::ResourceGroupManager::getSingleton().
		openResource(this->ResourceID(),m_pkMapping->Group(),true);

	if(spDataStream.isNull())
	{
		FV_WARNING_MSG( "FvZone::Load: %s don't exist (FNF)\n",
			m_kIdentifier.c_str() );
		m_bLoaded = true;
		return false;
	}

	FvZoneSerializer kSerializer;

	bool bGood = true;

	bGood = kSerializer.ImportZone(spDataStream,this);

	this->Transform( m_kTransform );

	for (int i = 0; i < FvZone::ms_iNextCacheID; i++)
	{
		(*FvZone::TouchType()[i])( *this );

		FvZoneCache *pkCC = m_ppCaches[i];
		if (pkCC != NULL) {
			if ( !pkCC->Load( kSerializer ) )
			{
				bGood = false;
				FV_ERROR_MSG( "FvZone::Load: Failed to load Cache %d for kZone %s\n", i, m_kIdentifier.c_str() );
			}
		}
	}

	m_bLoaded = true;
	return bGood;
}

void FvZone::Eject()
{
	//FV_GUARD;
	if (this->Online())
	{
		FV_ERROR_MSG( "FvZone::Eject: "
			"Tried to Eject a kZone while still online\n" );
		return;
	}

	if (!this->Loaded()) return;

	for (int i = m_kDynoItems.size()-1; i >= 0; i--)
	{
		FvZoneItemPtr spItem = m_kDynoItems[i];
		this->DelDynamicItem( spItem );
		m_pkSpace->AddHomelessItem( spItem.GetObject() );
	}
	{
		FvMatrixMutexHolder kLock( this );
		for (int i = m_kSelfItems.size()-1; i >= 0; i--)
		{
			FvZoneItemPtr spItem = m_kSelfItems[i];

#ifdef FV_EDITOR_ENABLED
			AmortiseZoneItemDelete::Instance().Add( spItem );
#endif // FV_EDITOR_ENABLED

			this->DelStaticItem( spItem );
			if (spItem->WantsNest())
			{
				m_pkSpace->AddHomelessItem( spItem.GetObject() );
			}
		}

		m_kSelfItems.clear();
	}
	m_kDynoItems.clear();
	m_kSwayItems.clear();

	m_kLenders.clear();
	m_kBorrowers.clear();

	m_kBounds.clear();
	m_kJoints.clear();

	for (int i = 0; i < FvZone::ms_iNextCacheID; i++)
	{
		if (m_ppCaches[i] != NULL)
		{
			delete m_ppCaches[i];
			m_ppCaches[i] = NULL;
		}
	}		

#if FV_UMBRA_ENABLE
	if (m_pkUmbraCell)
	{
		m_pkUmbraCell->Release();
		m_pkUmbraCell = NULL;
	}
#endif

	m_bLoaded = false;
}

void FvZone::Bind( bool bForm )
{
	//FV_GUARD;
	this->SyncInit();

	BindPortals( bForm );

	this->NotifyCachesOfBind( false );

	m_bOnline = true;

	m_pkSpace->NoticeZone( this );
}

void FvZone::BindPortals( bool bForm )
{
	//FV_GUARD;
	for (FvZoneBoundaries::iterator bit = m_kJoints.begin();
		bit != m_kJoints.end();
		bit++)
	{
		for (unsigned int i=0; i < (*bit)->m_kUnboundPortals.size(); i++)
		{
			FvZoneBoundary::Portal *& pPortal = (*bit)->m_kUnboundPortals[i];
			FvZoneBoundary::Portal & p = *pPortal;

			if (p.HasZone() && p.m_pkZone->Mapping()->Condemned())
			{
				FvZoneDirMapping *pkOthMapping = p.m_pkZone->Mapping();
				FV_ASSERT_DEV( pkOthMapping != m_pkMapping );
				FV_ASSERT_DEV( !p.m_pkZone->Ratified() );	

				delete p.m_pkZone;
				pkOthMapping->DecRef();

				p.m_pkZone = (FvZone*)FvZoneBoundary::Portal::EXTERN;
			}
			if (p.IsExtern())
			{
				p.ResolveExtern( this );
			}

			if (!p.HasZone())
			{
				if (!bForm) continue;
				if (p.m_pkZone != NULL && !p.IsInvasive()) continue;

				FvVector3 conPt = m_kTransform.ApplyPoint(
					p.m_kLCentre + p.m_kPlane.Normal() * -0.1f );

				FvZone * pFound = NULL;
				FvZoneSpace::Column *pCol = m_pkSpace->pColumn( conPt, false );
				if (pCol != NULL)
					pFound = pCol->FindZoneExcluding( conPt, this );

				if (pFound == NULL)
					continue;

				if (!pFound->FormPortal( this, p ))
					continue;

				p.m_pkZone = pFound;

				//(*bit)->SplitInvasivePortal( this, i );
			}
			else
			{
				bool holdingMappingRef =	
					(p.m_pkZone->Mapping() != m_pkMapping) && !p.m_pkZone->Ratified();

				p.m_pkZone = p.m_pkZone->Space()->FindOrAddZone( p.m_pkZone );

				if (holdingMappingRef) p.m_pkZone->Mapping()->DecRef();
			}

			if (!this->m_IsOutsideZone && p.m_pkZone->IsOutsideZone())
			{
				//this->AddStaticItem( new FvZoneExitPortal(p) );
			}

			if (p.m_pkZone->Online())
			{
				FvZone *pkOnlineZone = p.m_pkZone;

				(*bit)->BindPortal( i-- );

				pkOnlineZone->Bind( this );

#if FV_UMBRA_ENABLE
				p.CreateUmbraPortal( this );
#endif
			}
		}
	}
}

void FvZone::Loose( bool bCut )
{
	m_pkSpace->IgnoreZone( this );
	m_iFocusCount = 0;

	Borrowers::iterator brit;
	for (brit = m_kBorrowers.begin(); brit != m_kBorrowers.end(); brit++)
	{
		bool foundSelfAsLender = false;

		for (Lenders::iterator lit = (*brit)->m_kLenders.begin();
			lit != (*brit)->m_kLenders.end();
			lit++)
		{
			if ((*lit)->m_pkLender == this)
			{
				(*brit)->m_kLenders.erase( lit );
				foundSelfAsLender = true;
				break;
			}
		}

		if (!foundSelfAsLender)
		{
			FV_CRITICAL_MSG( "FvZone::Loose: "
				"%s could not find itself as a lender in %s\n",
				m_kIdentifier.c_str(), (*brit)->m_kIdentifier.c_str() );
		}
		else
		{
			/*FV_TRACE_MSG( "FvZone::Loose: %s bCut ties with borrower %s\n",
			m_kIdentifier.c_str(), (*brit)->m_kIdentifier.c_str() );*/
		}
	}
	m_kBorrowers.clear();

	Lenders::iterator lit;
	for (lit = m_kLenders.begin(); lit != m_kLenders.end(); lit++)
	{
		FvZone * pLender = (*lit)->m_pkLender;
		Borrowers::iterator brit = std::find(
			pLender->m_kBorrowers.begin(), pLender->m_kBorrowers.end(), this );

		bool foundSelfAsBorrower = (brit != pLender->m_kBorrowers.end());
		if (foundSelfAsBorrower)
			pLender->m_kBorrowers.erase( brit );

		if (!foundSelfAsBorrower)
		{
			FV_CRITICAL_MSG( "FvZone::Loose: "
				"%s could not find itself as a borrower in %s\n",
				m_kIdentifier.c_str(), pLender->m_kIdentifier.c_str() );
		}
		else
		{
			/*FV_TRACE_MSG( "FvZone::Loose: %s bCut ties with lender %s\n",
			m_kIdentifier.c_str(), pLender->m_kIdentifier.c_str() );*/
		}
	}
	m_kLenders.clear();

	for (FvZoneBoundaries::iterator bit = m_kJoints.begin();
		bit != m_kJoints.end();
		bit++)
	{
		for (unsigned int i=0; i < (*bit)->m_kBoundPortals.size(); i++)
		{
			FvZoneBoundary::Portal *& pPortal = (*bit)->m_kBoundPortals[i];
			FvZoneBoundary::Portal & p = *pPortal;

			if (!p.HasZone()) continue;

			FvZone *pkOnlineZone = p.m_pkZone;

			if (bCut)
			{
				if (!this->IsOutsideZone() && p.m_pkZone->IsOutsideZone())
					p.m_pkZone = (FvZone*)FvZoneBoundary::Portal::INVASIVE;
				else
					p.m_pkZone = NULL;
			}

			(*bit)->LoosePortal( i-- );

			if (this->IsOutsideZone() && !pkOnlineZone->IsOutsideZone())
				pkOnlineZone->Loose( this, true );
			else
				pkOnlineZone->Loose( this, bCut );
		}
	}

	this->NotifyCachesOfBind( true );

	m_bOnline = false;
}

typedef std::set< FvZoneSpace::Column * > ColumnSet;

void FvZone::Focus()
{
	FvHullBorder kBorder;

	for (unsigned int i = 0; i < m_kBounds.size(); i++)
	{
		const FvPlane &kPEQ = m_kBounds[i]->Plane();
		FvVector3 kNDTR = m_kTransform.ApplyPoint( kPEQ.Normal() * kPEQ.Distance() );
		FvVector3 kNTR = m_kTransform.ApplyVector( kPEQ.Normal() );
		kBorder.push_back( FvPlane( kNTR, kNTR.DotProduct( kNDTR ) ) );
	}

	ColumnSet kColumns;
	if (*(this->Identifier().end()-1) == 'o')
	{
		kColumns.insert( m_pkSpace->pColumn( m_kCentre ) );
	}
	else
	{
		const FvVector3 &kMinBB = m_kBoundingBox.MinBounds();
		const FvVector3 &kMaxBB = m_kBoundingBox.MaxBounds();
		for (int i = 0; i < 8; i++)
		{
			FvVector3 kPT(	(i&1) ? kMaxBB.x : kMinBB.x,
				(i&2) ? kMaxBB.y : kMinBB.y,
				(i&4) ? kMaxBB.z : kMinBB.z );

			FvZoneSpace::Column *pkColumn = m_pkSpace->pColumn( kPT );
			if (pkColumn)
			{
				kColumns.insert( pkColumn );
			}
		}

		FvZoneSpace::Column *pkColumn = m_pkSpace->pColumn( m_kBoundingBox.Centre() );
		if(pkColumn)
			kColumns.insert( pkColumn );

	}

	for (ColumnSet::iterator it = kColumns.begin(); it != kColumns.end(); it++)
	{
		FV_ASSERT_DEV( &**it );

		if( &**it )
			(*it)->AddZone( kBorder, this );
	}

	for (int i = 0; i < FvZone::ms_iNextCacheID; i++)
	{
		FvZoneCache * cc = m_ppCaches[i];
		if (cc != NULL) m_iFocusCount += cc->Focus();
	}

	m_iFocusCount = 1;
}

void FvZone::Smudge()
{
	//FV_GUARD;
	if (m_iFocusCount != 0)
	{
		//FV_TRACE_MSG( "FvZone::Smudge: %s is now blurred (fc %d)\n",
		//	m_kIdentifier.c_str(), m_iFocusCount );
		m_iFocusCount = 0;
		m_pkSpace->BlurredZone( this );
	}
}

void FvZone::ResolveExterns( FvZoneDirMapping *pkDeadMapping )
{
	//FV_GUARD;
	FV_IF_NOT_ASSERT_DEV( m_bOnline )
	{
		return;
	}

	FvZoneBoundaries::iterator bit;
	for (bit = m_kJoints.begin(); bit != m_kJoints.end(); bit++)
	{
		for (unsigned int i=0; i < (*bit)->m_kUnboundPortals.size(); i++)
		{
			FvZoneBoundary::Portal & p = *(*bit)->m_kUnboundPortals[i];

			if (pkDeadMapping != NULL)
			{
				if (!p.HasZone() || p.m_pkZone->Mapping() != pkDeadMapping)
					continue;

				p.m_pkZone = (FvZone*)FvZoneBoundary::Portal::EXTERN;
			}
			else
			{
				if (!p.IsExtern()) continue;
			}

			if (p.ResolveExtern( this ))
			{
				p.m_pkZone = m_pkSpace->FindOrAddZone( p.m_pkZone );
				p.m_pkZone->Mapping()->DecRef();
				if (p.m_pkZone->Online())
				{
					FvZone *pkOnlineZone = p.m_pkZone;

					(*bit)->BindPortal( i-- );

					pkOnlineZone->Bind( this );
				}
			}
		}
	}
}

void FvZone::Bind( FvZone * pkZone )
{
	//FV_GUARD;
	for (FvZoneBoundaries::iterator bit = m_kJoints.begin();
		bit != m_kJoints.end();
		bit++)
	{
		for (FvZoneBoundary::Portals::iterator pit =
			(*bit)->m_kUnboundPortals.begin();
			pit != (*bit)->m_kUnboundPortals.end(); pit++)
		{
			if ((*pit)->m_pkZone == pkZone)
			{
#if FV_UMBRA_ENABLE
				(*pit)->CreateUmbraPortal( this );
#endif
				(*bit)->BindPortal( pit - (*bit)->m_kUnboundPortals.begin() );

				this->NotifyCachesOfBind( false );

				return;
			}
		}
	}

	FV_ERROR_MSG( "FvZone::Bind: FvZone %s didn't find reverse kPortal to %s!\n",
		m_kIdentifier.c_str(), pkZone->Identifier().c_str() );
}

namespace 
{
	bool CanBind( FvZoneBoundary::Portal &kPortalA, FvZoneBoundary::Portal &kPortalB,
		FvZone *pkZoneA, FvZone *pkZoneB )
	{
		//FV_GUARD;
		FV_IF_NOT_ASSERT_DEV( pkZoneA != pkZoneB )
		{
			return false;
		}

		if ((kPortalA.m_pkZone != NULL && !kPortalA.HasZone()) ||
			(kPortalB.m_pkZone != NULL && !kPortalB.HasZone()) )
		{
			return false;
		}

		if (kPortalA.m_kPoints.size() != kPortalB.m_kPoints.size())
		{
			return false;
		}

		if (! FvAlmostEqual( ( kPortalA.m_kCentre - kPortalB.m_kCentre ).LengthSquared(), 0.f ))
		{
			return false;
		}

		FvVector3 n1 = pkZoneA->Transform().ApplyVector(kPortalA.m_kPlane.Normal());
		FvVector3 n2 = pkZoneB->Transform().ApplyVector(kPortalB.m_kPlane.Normal());

		if (! FvAlmostEqual( ( n1 + n2 ).Length(), 0.f ))
		{
			return false;
		}

		std::vector< FvVector3 > kPoints;

		for (unsigned int i = 0; i < kPortalA.m_kPoints.size(); ++i)
		{
			FvVector3 v = pkZoneA->Transform().ApplyPoint( kPortalA.ObjectSpacePoint( i ) );
			kPoints.push_back( v );
		}

		for (unsigned int i = 0; i < kPortalA.m_kPoints.size(); ++i)
		{
			FvVector3 v = pkZoneB->Transform().ApplyPoint( kPortalB.ObjectSpacePoint( i ) );
			std::vector< FvVector3 >::iterator iter = kPoints.begin();

			for (; iter != kPoints.end(); ++iter)
			{
				if (FvAlmostEqual( v, *iter ))
				{
					break;
				}
			}

			if (iter == kPoints.end())
			{
				return false;
			}
		}

		return true;
	}
}

bool FvZone::FormPortal( FvZone * pkZone, FvZoneBoundary::Portal &kPortal )
{
	//FV_GUARD;
	if (kPortal.IsInvasive() || ( !kPortal.IsInvasive() && !this->IsOutsideZone()))
	{
		for (FvZoneBoundaries::iterator kBIt = m_kJoints.begin();
			kBIt != m_kJoints.end();
			kBIt++)
		{
			for (FvZoneBoundary::Portals::iterator kPIt =
				(*kBIt)->m_kUnboundPortals.begin();
				kPIt != (*kBIt)->m_kUnboundPortals.end(); kPIt++)
			{
				if ( CanBind( kPortal, **kPIt, pkZone, this ) )
				{
					(*kPIt)->m_pkZone = pkZone;
					return true;
				}
			}
		}
	}

	if (!kPortal.IsInvasive() || !this->IsOutsideZone()) return false;

	const FvPlane &kFPlane = kPortal.m_kPlane;
	const FvVector3 &kFNormal = kFPlane.Normal();
	FvVector3 kWNormal = pkZone->m_kTransform.ApplyVector( kFNormal ) * -1.f;
	FvVector3 kWCentre = kPortal.m_kCentre;				
	FvVector3 kLNormal = m_kTransformInverse.ApplyVector( kWNormal );
	FvVector3 kLCentre = m_kTransformInverse.ApplyPoint( kWCentre );
	FvPlane kLPlane( kLCentre, kLNormal );

	bool bIsInternal = false;
	FvZoneBoundaries::iterator kBIt;
	
	kBIt = m_kJoints.end();

	if (kBIt == m_kJoints.end())
	{
		bIsInternal = true;

		FvZoneBoundary * pCB = new FvZoneBoundary;
		pCB->m_kPlane = kLPlane;
		m_kJoints.push_back( pCB );
		kBIt = m_kJoints.end() - 1;
	}

	FvZoneBoundary::Portal *pkPortal =
		new FvZoneBoundary::Portal((*kBIt).Get());
	pkPortal->m_kPlane = (*kBIt)->m_kPlane;
	pkPortal->m_bInternal = bIsInternal;
	pkPortal->m_pkZone = pkZone;

	float NdotX = kLNormal.DotProduct( FvVector3(1.0f, 0.0f, 0.0f) );
	float NdotY = kLNormal.DotProduct( FvVector3(0.0f, 1.0f, 0.0f) );
	float NdotZ = kLNormal.DotProduct( FvVector3(0.0f, 0.0f, 1.0f) );

	FvVector3 kCartesianAxis;

	if ( fabsf(NdotX) < fabsf(NdotY) )
		if ( fabsf(NdotX) < fabsf(NdotZ) )
			kCartesianAxis = FvVector3(1.0f, 0.0f, 0.0f);
		else
			kCartesianAxis = FvVector3(0.0f, 0.0f, 1.0f);
	else
		if ( fabsf(NdotY) < fabsf(NdotZ) )
			kCartesianAxis = FvVector3(0.0f, 1.0f, 0.0f);
		else
			kCartesianAxis = FvVector3(0.0f, 0.0f, 1.0f);

	FvVector3 kLUAxis = kLNormal.CrossProduct( kCartesianAxis );

	FvVector3 kLVAxis = kLNormal.CrossProduct( kLUAxis );

	FvMatrix kBasis = FvMatrix::IDENTITY;
	memcpy( kBasis[0], kLUAxis, sizeof(FvVector3) );
	memcpy( kBasis[1], kLVAxis, sizeof(FvVector3) );
	memcpy( kBasis[2], kLNormal, sizeof(FvVector3) );

	kBasis[3] = ( kLNormal * kLPlane.Distance() / kLNormal.LengthSquared() );
	FvMatrix kInvBasis;
	kInvBasis.Invert( kBasis );

	for (unsigned int i = 0; i < kPortal.m_kPoints.size(); i++)
	{
		FvVector3 kFPT =
			kPortal.m_kUAxis * kPortal.m_kPoints[i][0] +
			kPortal.m_kVAxis * kPortal.m_kPoints[i][1] +
			kPortal.m_kOrigin;
		FvVector3 kWPT = pkZone->m_kTransform.ApplyPoint( kFPT );
		FvVector3 kLPT = m_kTransformInverse.ApplyPoint( kWPT );
		FvVector3 kPPT = kInvBasis.ApplyPoint( kLPT );
		pkPortal->m_kPoints.push_back( FvVector2( kPPT.x, kPPT.y ) );
		pkPortal->m_kWPoints.push_back(kWPT);
	}
	pkPortal->m_kWNormal = kWNormal;
	pkPortal->m_kUAxis = kBasis.ApplyToUnitAxisVector(0); //luAxis;
	pkPortal->m_kVAxis = kBasis.ApplyToUnitAxisVector(1); //lvAxis;
	pkPortal->m_kOrigin = kBasis.ApplyToOrigin();
	pkPortal->m_kLCentre = m_kTransformInverse.ApplyPoint( kWCentre );
	pkPortal->m_kCentre = kWCentre;

	if (pkPortal->m_kPoints.size() == 4)
	{
		FvPlane kTestPlane(
			pkPortal->m_kPoints[0][0] * pkPortal->m_kUAxis + pkPortal->m_kPoints[0][1] * pkPortal->m_kVAxis + pkPortal->m_kOrigin,
			pkPortal->m_kPoints[1][0] * pkPortal->m_kUAxis + pkPortal->m_kPoints[1][1] * pkPortal->m_kVAxis + pkPortal->m_kOrigin,
			pkPortal->m_kPoints[2][0] * pkPortal->m_kUAxis + pkPortal->m_kPoints[2][1] * pkPortal->m_kVAxis + pkPortal->m_kOrigin );
		FvVector3 kN1 = (*kBIt)->m_kPlane.Normal();
		FvVector3 kN2 = kTestPlane.Normal();
		kN1.Normalise();
		kN2.Normalise();
		if ((kN1 + kN2).Length() < 1.f)	
		{
			FvVector2 kTPT = pkPortal->m_kPoints[1];
			pkPortal->m_kPoints[1] = pkPortal->m_kPoints[3];
			pkPortal->m_kPoints[3] = kTPT;
			FvVector3 kTWPT = pkPortal->m_kWPoints[1];
			pkPortal->m_kWPoints[1] = pkPortal->m_kWPoints[3];
			pkPortal->m_kWPoints[3] = kTWPT;
		}
	}

	(*kBIt)->AddInvasivePortal( pkPortal );

	this->NotifyCachesOfBind( false );

	m_bHasInternalZones |= bIsInternal;

	return true;
}

void FvZone::Loose( FvZone * pkZone, bool bCut )
{
	//FV_GUARD;
	for (FvZoneBoundaries::iterator bit = m_kJoints.begin();
		bit != m_kJoints.end();
		bit++)
	{
		for (FvZoneBoundary::Portals::iterator ppit =
			(*bit)->m_kBoundPortals.begin();
			ppit != (*bit)->m_kBoundPortals.end(); ppit++)
		{
			FvZoneBoundary::Portal * pit = *ppit;
			if (pit->m_pkZone == pkZone)
			{
				if (bCut)
				{
					if( !IsOutsideZone() && pkZone->IsOutsideZone() )
						pit->m_pkZone = (FvZone*)FvZoneBoundary::Portal::INVASIVE;
					else
						pit->m_pkZone = NULL;	

					if (pit->m_bInternal)
					{
						m_kJoints.erase( bit );

						this->NotifyCachesOfBind( true );

						return;
					}
				}

				(*bit)->LoosePortal( ppit - (*bit)->m_kBoundPortals.begin() );

				this->NotifyCachesOfBind( true );

				return;
			}
		}
	}

	FV_ERROR_MSG( "FvZone::Loose: FvZone %s didn't find reverse kPortal to %s!\n",
		m_kIdentifier.c_str(), pkZone->Identifier().c_str() );
}

void FvZone::SyncInit()
{
	//FV_GUARD;
#if FV_UMBRA_ENABLE
	if (FvZoneUmbra::SoftwareMode() && !m_IsOutsideZone)
	{
		m_pkUmbraCell = Umbra::Cell::Create();
	}
#endif

	FvMatrixMutexHolder kLock( this );
	Items::iterator it;
	for (it = m_kSelfItems.begin(); it != m_kSelfItems.end(); it++)
	{
		(*it)->SyncInit();
	}
}

void FvZone::NotifyCachesOfBind( bool bLooseNotBind )
{
	//FV_GUARD;
	for (int i = 0; i < FvZone::ms_iNextCacheID; i++)
	{
		FvZoneCache * cc = m_ppCaches[i];
		if (cc != NULL) cc->Bind( bLooseNotBind );
	}

	if (!bLooseNotBind)
	{
		Items::iterator it;
		{
			FvMatrixMutexHolder kLock( this );
			for (it = m_kSelfItems.begin(); it != m_kSelfItems.end(); it++)
			{
				(*it)->Lend( this );
			}
		}
		Lenders::iterator lit;
		for (lit = m_kLenders.begin(); lit != m_kLenders.end(); lit++)
		{
			FvMatrixMutexHolder kLock( (*lit).GetObject() );
			for (it = (*lit)->m_kItems.begin(); it != (*lit)->m_kItems.end(); it++)
				(*it)->Lend( this );
		}
	}
}

void FvZone::UpdateBoundingBoxes( FvZoneItemPtr spItem )
{
	//FV_GUARD;
	if( spItem->AddZBounds( m_kLocalBB ) )
	{
		m_kBoundingBox = m_kLocalBB;
		m_kBoundingBox.TransformBy( Transform() );
	}

#ifndef FV_SERVER
	spItem->AddZBounds(m_kVisibilityBox);
#endif // FV_SERVER
}

void FvZone::AddStaticItem( FvZoneItemPtr spItem )
{
	{
		FvMatrixMutexHolder kLock( this );

		if( !IsOutsideZone() && m_kLocalBB.InsideOut() )
		{
#ifdef FV_SERVER
			m_kLocalBB = ((FvServerZoneModel*)spItem.GetObject())->LocalBB();//! open by Uman, 20101216
			//m_kLocalBB = ( (ServerChunkModel*)spItem.GetObject() )->LocalBB();
			m_kBoundingBox = m_kLocalBB;
#else // FV_SERVER
			m_kLocalBB = ((FvZoneModel*)spItem.GetObject())->LocalBB();
			m_kVisibilityBox = m_kLocalBB;
			m_kBoundingBox = m_kLocalBB;
#endif // !FV_SERVER
			m_kBoundingBox.TransformBy( m_kTransform );
		}

		UpdateBoundingBoxes( spItem );

		m_kSelfItems.push_back( spItem );
	}
	if (spItem->WantsSway()) m_kSwayItems.push_back( spItem );

	spItem->Toss( this );

	if (this->Online())
	{
		spItem->Lend( this );
	}
}

void FvZone::DelStaticItem( FvZoneItemPtr spItem )
{
	//FV_GUARD;
	FvMatrixMutexHolder kLock( this );
	Items::iterator found = std::find(
		m_kSelfItems.begin(), m_kSelfItems.end(), spItem );
	if (found == m_kSelfItems.end()) return;

	if (this->Online())
	{
		unsigned int bris = m_kBorrowers.size();
		for (unsigned int bri = 0; bri < bris; bri++)
		{
			m_kBorrowers[bri]->DelLoanItem( spItem );

			unsigned int newBris = m_kBorrowers.size();
			if (bris != newBris)
			{
				bri--;
				bris = newBris;
			}
		}
	}

	m_kSelfItems.erase( found );

	if (spItem->WantsSway())
	{
		found = std::find( m_kSwayItems.begin(), m_kSwayItems.end(), spItem );
		if (found != m_kSwayItems.end()) m_kSwayItems.erase( found );
	}

	spItem->Toss( NULL );
}

void FvZone::AddDynamicItem( FvZoneItemPtr spItem )
{
	//FV_GUARD;
	m_kDynoItems.push_back( spItem );
	spItem->Toss( this );
}

bool FvZone::ModDynamicItem( FvZoneItemPtr spItem,
						   const FvVector3 & kOldPos, const FvVector3 & kNewPos, const float fDiameter,
						   bool bUseDynamicLending )
{
	//FV_GUARD;
	for (Items::iterator it = m_kSwayItems.begin(); it != m_kSwayItems.end(); it++)
	{
		(*it)->Sway( kOldPos, kNewPos, fDiameter );
	}

	FvZoneSpace::Column *pkCol = m_pkSpace->pColumn( kNewPos, false );
	float fRadius = fDiameter > 1.f ? fDiameter*0.5f : 0.f;

	if (!m_bHasInternalZones &&
		(!m_IsOutsideZone || pkCol == NULL || !pkCol->HasInsideZones()) &&
		this->Contains( kNewPos, fRadius ))
	{
		return true;
	}

	FvZone *pkDest = pkCol != NULL ? pkCol->FindZone( kNewPos ) : NULL;

	if ( bUseDynamicLending && fRadius > 0.f )
	{
#ifndef FV_SERVER
		//static DogWatch dWatch("DynamicLending");
		//dWatch.start();
#endif

		static std::vector<FvZonePtr> kNearbyZones;
		FvZone::piterator kPEnd = this->PEnd();
		for (FvZone::piterator kPTt = this->PBegin(); kPTt != kPEnd; kPTt++)
		{
			if (!kPTt->HasZone()) continue;
			FvZone * pConsider = kPTt->m_pkZone;

			pConsider->DelLoanItem( spItem, true );

			if ( pConsider->BoundingBox().Distance(kNewPos) <= fRadius )
				kNearbyZones.push_back(pConsider);
		}

		if (pkDest != this)
		{
			kNearbyZones.clear();
			this->DelDynamicItem( spItem, false );
			if (pkDest != NULL)
			{
				pkDest->AddDynamicItem( spItem );
			}
			else
			{
				m_pkSpace->AddHomelessItem( spItem.GetObject() );
#ifndef FV_SERVER
				//dWatch.stop();
#endif
				return false;
			}
		}

		if (kNearbyZones.size())
		{
			std::vector<FvZonePtr>::iterator cit=kNearbyZones.begin();
			while(cit != kNearbyZones.end())
			{
				if (!(*cit)->AddLoanItem( spItem ))
					break;
				cit++;
			}
			kNearbyZones.clear();
		}
		else
		{
			FvZone::piterator kPEnd2 = pkDest->PEnd();
			for (FvZone::piterator kPTt = pkDest->PBegin(); kPTt != kPEnd2; kPTt++)
			{
				if (!kPTt->HasZone()) continue;

				FvZone * pConsider = kPTt->m_pkZone;
				if (pConsider == pkDest ||
					pConsider->BoundingBox().Distance(kNewPos) > (fRadius)) continue;

				pConsider->AddLoanItem( spItem );
			}
		}
#ifndef FV_SERVER
		//dWatch.stop();
#endif
	}
	else if (pkDest != this) 
	{
		this->DelDynamicItem( spItem, false );
		if (pkDest != NULL)
		{
			pkDest->AddDynamicItem( spItem );
		}
		else
		{
			m_pkSpace->AddHomelessItem( spItem.GetObject() );
			return false;
		}
	}

	return true;
}

void FvZone::DelDynamicItem( FvZoneItemPtr spItem, bool bUseDynamicLending /* =true */ )
{
	//FV_GUARD;
	if (bUseDynamicLending)
	{
		FvZone::piterator pend = this->PEnd();
		for (FvZone::piterator pit = this->PBegin(); pit != pend; pit++)
		{
			if (!pit->HasZone()) continue;

			FvZone * pConsider = pit->m_pkZone;
			pConsider->DelLoanItem( spItem, true );
		}
	}

	Items::iterator found = std::find(
		m_kDynoItems.begin(), m_kDynoItems.end(), spItem );

	if (found != m_kDynoItems.end())
	{
		m_kDynoItems.erase( found );
		spItem->Toss( NULL );
	}
}

void FvZone::JogForeignItems()
{
	//FV_GUARD;
	int diSize = m_kDynoItems.size();
	for (int i = 0; i < diSize; i++)
	{
		Items::iterator it = m_kDynoItems.begin() + i;

		FvZoneItemPtr cip = (*it);	
		cip->Nest( m_pkSpace );

		int niSize = m_kDynoItems.size();
		i -= diSize - niSize;
		diSize = niSize;
	}

	FvMatrixMutexHolder kLock( this );
	int siSize = m_kSelfItems.size();
	for (int i = 0; i < siSize; i++)
	{
		Items::iterator it = m_kSelfItems.begin() + i;
		if (!(*it)->WantsNest()) continue;

		FvZoneItemPtr cip = (*it);	
		cip->Nest( m_pkSpace );

		int niSize = m_kSelfItems.size();
		i -= siSize - niSize;
		siSize = niSize;
	}
}

bool FvZone::AddLoanItem( FvZoneItemPtr spItem )
{
	//FV_GUARD;
	FvZone * pSourceChunk = spItem->Zone();
	if (pSourceChunk == this) return false;

	Lenders::iterator lit;
	for (lit = m_kLenders.begin(); lit != m_kLenders.end(); lit++)
	{
		if ((*lit)->m_pkLender == pSourceChunk) break;
	}
	if (lit != m_kLenders.end())
	{
		Items::iterator found = std::find(
			(*lit)->m_kItems.begin(), (*lit)->m_kItems.end(), spItem );
		if (found != (*lit)->m_kItems.end()) return false;
	}
	else
	{
		m_kLenders.push_back( new Lender() );
		lit = m_kLenders.end() - 1;
		(*lit)->m_pkLender = pSourceChunk;
		pSourceChunk->m_kBorrowers.push_back( this );
	}

	(*lit)->m_kItems.push_back( spItem );

#if FV_UMBRA_ENABLE

	if (spItem->kZone()->GetUmbraCell() != this->GetUmbraCell())
	{
		FvMatrix lenderChunkTransform = FvMatrix::IDENTITY;
		FvMatrix invBorrowerChunkTransform = FvMatrix::IDENTITY;
		invBorrowerChunkTransform.Invert();

		FvMatrix itemTransform;
		UmbraObjectProxyPtr pZoneObject = spItem->pUmbraObject();
		if (pZoneObject && pZoneObject->object())
		{
			pZoneObject->object()->getObjectToCellMatrix((Umbra::Matrix4x4&)itemTransform);
			itemTransform.PostMultiply( lenderChunkTransform );
			itemTransform.PostMultiply( invBorrowerChunkTransform );

			UmbraObjectProxyPtr pLenderObject = UmbraObjectProxy::get( pZoneObject->pModelProxy() );
			pLenderObject->object()->setUserPointer( pZoneObject->object()->getUserPointer() );
			pLenderObject->object()->setCell( this->GetUmbraCell() );
			pLenderObject->object()->setObjectToCellMatrix( (Umbra::Matrix4x4&)itemTransform );

			(*lit)->m_kUmbraItems.insert(std::make_pair(spItem.GetObject(), pLenderObject));
		}
	}
#endif

	if (spItem->WantsSway()) m_kSwayItems.push_back( spItem );

	spItem->Lend( this );

	return true;
}

bool FvZone::DelLoanItem( FvZoneItemPtr spItem, bool bCanFail )
{
	//FV_GUARD;
	FvZone * pSourceChunk = spItem->Zone();

	Lenders::iterator lit;
	for (lit = m_kLenders.begin(); lit != m_kLenders.end(); lit++)
	{
		if ((*lit)->m_pkLender == pSourceChunk) break;
	}
	if (lit == m_kLenders.end())
	{
		if (!bCanFail)
			FV_ERROR_MSG( "FvZone::DelLoanItem: "
			"No lender entry in %s for borrower entry in %s!\n",
			m_kIdentifier.c_str(), pSourceChunk->m_kIdentifier.c_str() );
		return false;
	}
#if FV_UMBRA_ENABLE
	UmbraItems::iterator dit = (*lit)->m_kUmbraItems.find( spItem.GetObject() );
	if (dit != (*lit)->m_kUmbraItems.end())
	{
		(*lit)->m_kUmbraItems.erase(dit);
	}
#endif

	Items::iterator found = std::find(
		(*lit)->m_kItems.begin(), (*lit)->m_kItems.end(), spItem );
	if (found == (*lit)->m_kItems.end()) return false;

	(*lit)->m_kItems.erase( found );

	if ((*lit)->m_kItems.empty())
	{
		m_kLenders.erase( lit );

		Borrowers::iterator brit;
		brit = std::find( pSourceChunk->m_kBorrowers.begin(),
			pSourceChunk->m_kBorrowers.end(), this );
		if (brit == pSourceChunk->m_kBorrowers.end())
		{
			FV_CRITICAL_MSG( "FvZone::DelLoanItem: "
				"No borrower entry in %s for lender entry in %s!\n",
				pSourceChunk->m_kIdentifier.c_str(), m_kIdentifier.c_str() );
			return false;
		}
		pSourceChunk->m_kBorrowers.erase( brit );
	}

	return true;
}

bool FvZone::IsLoanItem( FvZoneItemPtr spItem ) const
{
	//FV_GUARD;
	FvZone *pSourceChunk = spItem->Zone();

	Lenders::const_iterator lit;
	for (lit = m_kLenders.begin(); lit != m_kLenders.end(); lit++)
	{
		if ((*lit)->m_pkLender == pSourceChunk) break;
	}
	if (lit == m_kLenders.end())
	{
		return false;
	}

	Items::iterator found = std::find(
		(*lit)->m_kItems.begin(), (*lit)->m_kItems.end(), spItem );
	return found != (*lit)->m_kItems.end();
}

#ifndef FV_SERVER

void FvZone::VisitTerrain(FvCamera *pkCamera, FvZoneVisitor *pkVisitor)
{
	if (!this->Online() ||
		!pkVisitor->Visit(this))
		return;

	if(this->VisitMark() == ms_uiNextMark)
	{
		if(FV_MASK_HAS_ANY(m_uiCameraVisitFlag, FV_MASK(ms_uiCurrentCameraID)))
			return;
	}
	else
	{
		m_uiCameraVisitFlag = 0;
	}

	FV_MASK_ADD(m_uiCameraVisitFlag, FV_MASK(ms_uiCurrentCameraID));

	++FvZoneManager::ms_iZonesTraversed;

	if(!BoundingBox().InsideOut())
	{
		FvZoneTerrain *pkTerrain = FvZoneTerrainCache::ms_kInstance(*this).ZoneTerrain();
		if(pkTerrain)
		{
			pkTerrain->VisitTerrain(pkCamera,pkVisitor);
			pkTerrain->VisitMark(ms_uiNextMark);
		}
		//
		FvZoneManager::ms_iVisibleCount += 1;
	}

	this->VisitMark( ms_uiNextMark );
	//this->TraverseMark( ms_uiNextMark );


	// 搜索连接区域
	for (FvZone::piterator kIt = this->PBegin(); kIt != this->PEnd(); kIt++)
	{
		switch (ulong( kIt->m_pkZone ))
		{
		case FvZoneBoundary::Portal::NOTHING:
			break;
		case FvZoneBoundary::Portal::HEAVEN:
			break;
		case FvZoneBoundary::Portal::EARTH:

			break;
		default:
			if (!kIt->m_pkZone->Online()) break;

			if(kIt->m_pkZone->IsOutsideZone())
			{
				FvBoundingBox kTemp = kIt->m_pkZone->BoundingBox();
				kTemp.AddZBounds(1000);
				kTemp.AddZBounds(-1000);

				if(pkCamera->IsVisible(kTemp))
					kIt->m_pkZone->VisitTerrain(pkCamera,pkVisitor);
				break;
			}
			break;
		}	
	}
}

void FvZone::VisitLents(FvCamera *pkCamera, FvZoneVisitor *pkVisitor)
{
	if(this->VisitMark() != ms_uiNextMark)
	{
		m_uiCurrentGroupID = 0;
		m_uiCameraVisitFlag = 0;
		
	}
	
	if(pkVisitor->Visit(this))
	{
		for(FvUInt32 i(0); i < m_u32LentVisitItemPointer; ++i)
		{
			m_apkLentVisitItems[i]->Visit(pkCamera,pkVisitor);
			m_apkLentVisitItems[i]->VisitMark(ms_uiNextMark);
		}
	}	

	ClearLentVisitItems();
	Detach();
}

void FvZone::Visit(FvCamera *pkCamera, FvZoneVisitor *pkVisitor)
{
	if (!this->Online())
		return;

	if(this->VisitMark() == ms_uiNextMark)
	{
		if(FV_MASK_HAS_ANY(m_uiCameraVisitFlag, FV_MASK(ms_uiCurrentCameraID)))
			return;
	}
	else
	{
		m_uiCurrentGroupID = 0;
		m_uiCameraVisitFlag = 0;
	}
	if(!pkVisitor->Visit(this)) return;

	FV_MASK_ADD(m_uiCameraVisitFlag, FV_MASK(ms_uiCurrentCameraID));

	++FvZoneManager::ms_iZonesTraversed;

	bool bOCVisable = true;

	if(FvRenderManager::IsOCStart() && m_pkOcclusion)
	{
		bOCVisable = m_pkOcclusion->IsVisible();

		if(!bOCVisable)
		{
			m_pkOcclusion->SetResult(1);
		}
	}

	if(!BoundingBox().InsideOut())
	{
		if(pkCamera->IsVisible(BoundingBox()))
		{
			if(FvRenderManager::IsOCStart() && m_pkOcclusion)
			{
				if(!(BoundingBox().Intersects(*(FvVector3*)&(pkCamera->getRealPosition()), pkCamera->getNearClipDistance() + 20)))
				{
					FvRenderManager* pkRenderManager = (FvRenderManager*)(Ogre::Root::getSingleton()._getCurrentSceneManager());
					FV_ASSERT(pkRenderManager);

					m_pkOcclusion->Update(m_kBoundingBox.MaxBounds() - m_kBoundingBox.MinBounds(), m_kCentre);

					pkRenderManager->AddOcclusionQueryAABB(m_pkOcclusion);
					pkVisitor->GetRenderQueue()->addRenderable(m_pkOcclusion, 79);
				}
			}

			if(bOCVisable)
			{
				int iVisiableCount = 0;

				for (int i = 0; i < ms_iNextCacheID; i++)
				{
					FvZoneCache *pkCC = m_ppCaches[i];
					if (pkCC != NULL) 
						pkCC->Visit(pkCamera,pkVisitor);
				}

				Items::iterator kIt;
				{
					FvMatrixMutexHolder kLock( this );
					for (kIt = m_kSelfItems.begin(); kIt != m_kSelfItems.end(); kIt++)
					{
						iVisiableCount++;
						FvZoneItem *pkItem = &**kIt;
						if(pkItem->WantsVisit())
						{
							pkItem->Visit(pkCamera,pkVisitor);
							pkItem->VisitMark(ms_uiNextMark);
						}
					}

					for (kIt = m_kDynoItems.begin(); kIt != m_kDynoItems.end(); kIt++)
					{
						iVisiableCount++; 
						FvZoneItem *pkItem = &**kIt;
						if(pkItem->WantsVisit() && (!pkItem->GetTrueDynamic()))
						{
							pkItem->Visit(pkCamera,pkVisitor);
							pkItem->VisitMark(ms_uiNextMark);
						}
					}
				}

				for (Lenders::iterator kLIt = m_kLenders.begin(); kLIt != m_kLenders.end(); kLIt++)
				{
					FvZone* pkLender = (*kLIt)->m_pkLender;
					if(pkLender->IsOutsideLight() && IsOutsideLight())
					{
						for (kIt = (*kLIt)->m_kItems.begin(); kIt != (*kLIt)->m_kItems.end(); kIt++)
						{
							iVisiableCount++; 
							FvZoneItem *pkItem = &**kIt;
							if(pkItem->WantsVisit())
							{
								pkItem->Visit(pkCamera,pkVisitor);
								pkItem->VisitMark(ms_uiNextMark);
							}
						}
					}
					else
					{
						for (kIt = (*kLIt)->m_kItems.begin(); kIt != (*kLIt)->m_kItems.end(); kIt++)
						{
							iVisiableCount++; 
							FvZoneItem *pkItem = &**kIt;
							if(pkItem->WantsVisit())
							{						
								pkItem->Zone()->AddLentVisitItem(pkItem);
								ms_kLentZoneList.AttachBack(*pkItem->Zone());								

							}
						}
					}
				}

				unsigned int uiLentCount = m_kLentItemLists.size();
				for (unsigned int i = 0; i < uiLentCount; i++)
				{
					for (kIt = m_kLentItemLists[i]->begin();
						kIt != m_kLentItemLists[i]->end();
						kIt++)
					{
						FvZoneItem *pkItem = &**kIt;

						if(pkItem->VisitMark() == ms_uiNextMark)
						{
							if(FV_MASK_HAS_ANY(pkItem->CameraMark(), FV_MASK(ms_uiCurrentCameraID)))
								return;
						}
						else
						{
							pkItem->CameraMark() = 0;
						}

						if(pkItem->WantsVisit())
						{
							iVisiableCount++;
							pkItem->Visit(pkCamera,pkVisitor);
							pkItem->VisitMark( ms_uiNextMark );
							FV_MASK_ADD(pkItem->CameraMark(), FV_MASK(ms_uiCurrentCameraID));
						}
					}
				}

				m_kLentItemLists.clear();

				FvZoneManager::ms_iVisibleCount += iVisiableCount;	
			}
		}
		else
		{
			if(m_pkOcclusion) m_pkOcclusion->SetResult(1);
		}
	}

	{
		FvMatrixMutexHolder kLock( this );
		for (Items::iterator kIt = m_kDynoItems.begin(); kIt != m_kDynoItems.end(); kIt++)
		{
			FvZoneItem *pkItem = &**kIt;
			if(pkItem->WantsVisit() && pkItem->GetTrueDynamic())
			{
				pkItem->Visit(pkCamera,pkVisitor);
				pkItem->VisitMark(ms_uiNextMark);
			}
		}
	}
	
	this->VisitMark( ms_uiNextMark );
	//this->TraverseMark( ms_uiNextMark );
	

	// 搜索连接区域
	for (FvZone::piterator kIt = this->PBegin(); kIt != this->PEnd(); kIt++)
	{
		switch (ulong( kIt->m_pkZone ))
		{
		case FvZoneBoundary::Portal::NOTHING:
			break;
		case FvZoneBoundary::Portal::HEAVEN:
			if (!this->IsOutsideZone() && 
				pkCamera->IsVisible(&(*kIt)) &&
				pkVisitor->Visit(&(*kIt)))
			{
#ifdef FV_ENABLE_DRAW_PORTALS
				if(FvZoneBoundary::Portal::ms_bDrawPortals)
					kIt->Visit(pkVisitor,m_kTransform);
#endif // FV_ENABLE_DRAW_PORTALS
				FvZoneSpace::Column *pkCol =
					this->Space()->pColumn( kIt->m_kCentre, false );
				if (pkCol == NULL) break;
				FvZone *pkOutZone = pkCol->OutsideZone();
				if (pkOutZone == NULL || pkOutZone->TraverseMark() == m_uiVisitMark)
					break;

				kIt->m_pkZone = pkOutZone;
				if(pkCamera->IsVisible(kIt->m_pkZone->BoundingBox()))
				{
					int iNum = pkCamera->AddPortalCullingPlanes(&(*kIt));
					kIt->m_pkZone->Visit(pkCamera,pkVisitor);
					if(iNum > 0)
						pkCamera->RemovePortalCullingPlanes(&(*kIt));
				}
				kIt->m_pkZone = (FvZone*)FvZoneBoundary::Portal::HEAVEN;
			}
			break;
		case FvZoneBoundary::Portal::EARTH:

			break;
		default:
			if (!kIt->m_pkZone->Online()) break;				

			//if (kIt->m_pkZone->VisitMark() == m_uiVisitMark) break;

			if(this->IsOutsideZone() && kIt->m_pkZone->IsOutsideZone())
			{
				FvBoundingBox kTemp = kIt->m_pkZone->BoundingBox();
				kTemp.AddZBounds(1000);
				kTemp.AddZBounds(-1000);

				if(pkCamera->IsVisible(kTemp))
					kIt->m_pkZone->Visit(pkCamera,pkVisitor);
				break;
			}

			if(pkCamera->IsVisible(&(*kIt)) &&
				pkVisitor->Visit(&(*kIt)))
			{
#if FV_ENABLE_DRAW_PORTALS
				if(FvZoneBoundary::Portal::ms_bDrawPortals)
					kIt->Visit(pkVisitor,m_kTransform);
#endif // FV_ENABLE_DRAW_PORTALS
				if(pkCamera->IsVisible(kIt->m_pkZone->BoundingBox()))
				{
					if(/*(!(this->IsOutsideZone())) && (!(kIt->m_pkZone->IsOutsideZone())) && */this->IsOutsideLight() && kIt->m_pkZone->IsOutsideLight())
					{
						kIt->m_pkZone->Visit(pkCamera,pkVisitor);
					}
					else
					{
						int iNum = pkCamera->AddPortalCullingPlanes(&(*kIt));
						FvUInt32 u32CurrentCameraID = FvZone::ms_uiCurrentCameraID;
						bool bVisit = true;
						if(iNum)
						{
							FvZone::ms_uiCurrentCameraID = ++FvZone::ms_uiCurrentCamera;
							if(FvZone::ms_uiCurrentCamera >= 32)
							{
								bVisit = false;
							}

						}

						if(bVisit)
						{
							kIt->m_pkZone->Visit(pkCamera,pkVisitor);
						}

						if(iNum > 0)
						{
							pkCamera->RemovePortalCullingPlanes(&(*kIt));
						}

						FvZone::ms_uiCurrentCameraID = u32CurrentCameraID;		
					}			
				}
			}

			break;
		}	
	}
}

void FvZone::Tick( float fTime )
{
	//FV_GUARD_PROFILER( Chunk_tick );
	FvMatrixMutexHolder kLock( this );

	//PROFILER_BEGIN( Chunk_tick2 );
	for (Items::iterator kIt = m_kSelfItems.begin(); kIt != m_kSelfItems.end(); kIt++)
	{
		FvZoneItem *pkItem = &**kIt;
		if(pkItem->WantsVisit())
			pkItem->Tick( fTime );
	}
	for (Items::iterator kIt = m_kDynoItems.begin(); kIt != m_kDynoItems.end(); kIt++)
	{
		FvZoneItem *pkItem = &**kIt;
		if(pkItem->WantsVisit())
			pkItem->Tick( fTime );
	}
	//PROFILER_END();
}
#endif // FV_SERVER



FvZone *FvZone::FindClosestUnloadedZoneTo( const FvVector3 & kPoint,
										  float * pDist )
{
	//FV_GUARD;
	FvZone * pClosest = NULL;
	float fDist = 0.f;

	for (FvZoneBoundaries::iterator kBIt = m_kJoints.begin();
		kBIt != m_kJoints.end();
		kBIt++)
	{
		for (FvZoneBoundary::Portals::iterator kPPIt =
			(*kBIt)->m_kUnboundPortals.begin();
			kPPIt != (*kBIt)->m_kUnboundPortals.end(); kPPIt++)
		{
			FvZoneBoundary::Portal *pkPortal = *kPPIt;
			if (!pkPortal->HasZone()) continue;

			float fTDist = (pkPortal->m_kCentre - kPoint).Length();
			if (!pClosest || fTDist < fDist)
			{
				pClosest = pkPortal->m_pkZone;
				fDist = fTDist;
			}
		}
	}

	*pDist = fDist;
	return pClosest;
}

void FvZone::Transform( const FvMatrix &kTransform )
{
	//FV_GUARD;
	FV_IF_NOT_ASSERT_DEV( !this->Online() )
	{
		return;
	}

	FvMatrix oldXFormInv = m_kTransformInverse;

	m_kTransform = kTransform;
	m_kTransformInverse.Invert( kTransform );

	m_kBoundingBox = m_kLocalBB;
	m_kBoundingBox.TransformBy( kTransform );

	m_kCentre = m_kBoundingBox.Centre();

	for (FvZoneBoundaries::iterator kBIt = m_kJoints.begin();
		kBIt != m_kJoints.end();
		kBIt++)
	{
		for (FvZoneBoundary::Portals::iterator kPIt =
			(*kBIt)->m_kBoundPortals.begin();
			kPIt != (*kBIt)->m_kBoundPortals.end(); kPIt++)
		{
			(*kPIt)->Transform( kTransform );
		}

		for (FvZoneBoundary::Portals::iterator kPIt =
			(*kBIt)->m_kUnboundPortals.begin();
			kPIt != (*kBIt)->m_kUnboundPortals.end(); kPIt++)
		{
			(*kPIt)->Transform( kTransform );

			if ((*kPIt)->IsExtern() && !this->Online())
				(*kPIt)->ResolveExtern( this );
		}
	}

	if (!this->Loaded()) return;

	FvMatrixMutexHolder kLock( this );
	for (Items::iterator it = m_kSelfItems.begin(); it != m_kSelfItems.end(); it++)
	{
		(*it)->Toss( this );
	}
}

void FvZone::TransformTransiently( const FvMatrix &kTransform )
{
	//FV_GUARD;
	FV_IF_NOT_ASSERT_DEV( this->Online() )
	{
		return;
	}

	m_kTransform = kTransform;
	m_kTransformInverse.Invert( kTransform );

	m_kBoundingBox = m_kLocalBB;
	m_kBoundingBox.TransformBy( kTransform );

	m_kCentre = m_kBoundingBox.Centre();

	for (FvZoneBoundaries::iterator kBIt = m_kJoints.begin();
		kBIt != m_kJoints.end();
		kBIt++)
	{
		for (FvZoneBoundary::Portals::iterator kPIt =
			(*kBIt)->m_kBoundPortals.begin();
			kPIt != (*kBIt)->m_kBoundPortals.end(); kPIt++)
		{
			(*kPIt)->Transform( kTransform );
		}

		for (FvZoneBoundary::Portals::iterator kPIt =
			(*kBIt)->m_kUnboundPortals.begin();
			kPIt != (*kBIt)->m_kUnboundPortals.end(); kPIt++)
		{
			(*kPIt)->Transform( kTransform );

			if ((*kPIt)->IsExtern() && !this->Online())
				(*kPIt)->ResolveExtern( this );
		}
	}
}

bool FvZone::Contains( const FvVector3 &kPoint, const float fRadius ) const
{
	//FV_GUARD;
	if (!m_kBoundingBox.Intersects( kPoint )) return false;

	FvVector3 kLocalPoint = m_kTransformInverse.ApplyPoint( kPoint );

	for (FvZoneBoundaries::const_iterator it = m_kBounds.begin();
		it != m_kBounds.end();
		it++)
	{
		if ((*it)->Plane().DistanceTo( kLocalPoint ) < fRadius) return false;
	}

	return true;
}

bool FvZone::Owns( const FvVector3 &kPoint )
{
	//FV_GUARD;
	if (IsOutsideZone())
	{
		if (!Contains( kPoint ))
		{
			return false;
		}

		std::vector<FvZone*> overlappers = OverlapperFinder()( this );

		for (std::vector<FvZone*>::iterator iter = overlappers.begin();
			iter != overlappers.end(); ++iter)
		{
			FvZone* overlapper = *iter;

			if (overlapper->Contains( kPoint ))
			{
				return false;
			}
		}
		return true;
	}
	return Contains( kPoint );
}

float FvZone::Volume() const
{
	FvVector3 kVec =
		m_kBoundingBox.MaxBounds() - m_kBoundingBox.MinBounds();
	return kVec[0] * kVec[1] * kVec[2];
}

FvString FvZone::BinFileName() const
{
	return Mapping()->Path() + Identifier() + ".ftd";
}


#ifndef FV_SERVER
const FvBoundingBox & FvZone::VisibilityBox()
{
	//FV_GUARD;
	if (this->m_uiVisibilityBoxMark != ms_uiNextVisibilityMark)
	{
		this->m_kVisibilityBoxCache = this->m_kVisibilityBox;
		Items::const_iterator itemsIt  = m_kDynoItems.begin();
		Items::const_iterator itemsEnd = m_kDynoItems.end();
		while (itemsIt != itemsEnd)
		{
			(*itemsIt)->AddZBounds(this->m_kVisibilityBoxCache);
			++itemsIt;
		}
		if (!this->m_kVisibilityBoxCache.InsideOut())
		{
			this->m_kVisibilityBoxCache.TransformBy(this->Transform());
		}
		this->m_uiVisibilityBoxMark = ms_uiNextVisibilityMark;
	}

	return this->m_kVisibilityBoxCache;
}


void FvZone::AddZBoundsToVisibilityBox(float fMinZ, float fMaxZ)
{
	this->m_kVisibilityBox.AddZBounds(fMinZ);
	this->m_kVisibilityBox.AddZBounds(fMaxZ);
}
#endif // FV_SERVER

FvString FvZone::ResourceID() const
{
	return m_pkMapping->Path() + Identifier() + ".zone";
}

bool FvZone::FindBetterPortal( FvZoneBoundary::Portal *pkCurr, float fWithinRange,
							 FvZoneBoundary::Portal *pkTest, const FvVector3 &kVec )
{
	//FV_GUARD;
	if (pkTest == NULL)
	{
		FV_WARNING_MSG( "FvZone::FindBetterPortal: testing kPortal is NULL\n" );
		return false;
	}

	float testArea = 0.f;
	bool inside = true;
	FvVector2 pt2D( pkTest->m_kUAxis.DotProduct( kVec ), pkTest->m_kVAxis.DotProduct( kVec ) );
	FvVector2 hpt = pkTest->m_kPoints.back();
	FvUInt32 npts = pkTest->m_kPoints.size();
	for (FvUInt32 i = 0; i < npts; i++)
	{
		FvVector2 tpt = pkTest->m_kPoints[i];
		testArea += hpt.x*tpt.y - tpt.x*hpt.y;

		inside &= ((tpt-hpt).CrossProduct(pt2D - hpt) > 0.f);
		hpt = tpt;
	}
	if (!inside) return false;

	if (fWithinRange > 0.f && fabs( pkTest->m_kPlane.DistanceTo( kVec ) ) > fWithinRange)
		return false;

	if (pkCurr == NULL) return true;

	if (pkTest->m_pkZone != pkCurr->m_pkZone)
		return pkTest->m_pkZone->Volume() < pkCurr->m_pkZone->Volume();

	return fabs(pkTest->m_kPlane.DistanceTo( kVec )) < fabs(pkCurr->m_kPlane.DistanceTo( kVec ));

	float currArea = 0.f;
	hpt = pkCurr->m_kPoints.back();
	npts = pkCurr->m_kPoints.size();
	for (FvUInt32 i = 0; i < npts; i++)
	{
		FvVector2 tpt = pkTest->m_kPoints[i];
		currArea += hpt.x*tpt.y - tpt.x*hpt.y;
		hpt = tpt;
	}
	return fabs( testArea ) < fabs( currArea );
}

int FvZone::RegisterCache( TouchFunction kTouchFun )
{
	TouchType().push_back( kTouchFun );

	return ms_iNextCacheID++;
}

bool FvZone::CanSeeHeaven()
{
	//FV_GUARD;
	for (piterator it = this->PBegin(); it != this->PEnd(); it++)
	{
		if (it->IsHeaven())
			return true;
	}
	return false;
}


#if FV_UMBRA_ENABLE

Umbra::Cell* FvZone::GetUmbraCell() const
{
	//FV_GUARD;
	if (m_pkUmbraCell)
		return m_pkUmbraCell;

	if (!m_IsOutsideZone)
		return m_pkSpace->UmbraInsideCell();

	return  m_pkSpace->UmbraCell();
}
#endif

int FvZone::SizeStaticItems() const
{
	FvMatrixMutexHolder kLock( this );
	return m_kSelfItems.size();
}

void FvZone::SetMainBoundaryID( FvInt32 i32MainBoundary )
{
	if(i32MainBoundary >= 0 && i32MainBoundary < (FvInt32)GetJointBoundaryNumber())
	{
		m_i32MainBoundaryID = i32MainBoundary;
	}
	else
	{
		m_i32MainBoundaryID = -1;
	}
}

FvInt32 FvZone::GetMainBoundaryID()
{
	return m_i32MainBoundaryID;
}

FvUInt32 FvZone::GetJointBoundaryNumber()
{
	return m_kJoints.size();
}

FvZoneBoundary* FvZone::GetMainBoundary()
{
	return m_kJoints[m_i32MainBoundaryID].Get();
}

void FvZone::AddLentVisitItem( FvZoneItem* pkItem )
{
	if(m_u32LentVisitItemPointer < 100)
	{
		m_apkLentVisitItems[m_u32LentVisitItemPointer++] = pkItem;
	}
}

void FvZone::ClearLentVisitItems()
{
	m_u32LentVisitItemPointer = 0;
}

//----------------------------------------------------------------------------
#ifndef FV_SERVER

void ZonesDrawCullingHUD()
{
	//FV_GUARD;
#if FV_ENABLE_CULLING_HUD
	if (s_bCullDebugEnable)
	{
		ZonesDrawCullingHUDPriv();
	}

	s_kTraversedZones.erase(s_kTraversedZones.begin(), s_kTraversedZones.end());
	s_kVisibleZones.erase(s_kVisibleZones.begin(), s_kVisibleZones.end());
	s_kFringeZones.erase(s_kFringeZones.begin(), s_kFringeZones.end());
	s_kReflectedZones.erase(s_kReflectedZones.begin(), s_kReflectedZones.end());
	s_kDebugBoxes.erase(s_kDebugBoxes.begin(), s_kDebugBoxes.end());
#endif // FV_ENABLE_CULLING_HUD
}

namespace 
{ // anonymous

#if FV_ENABLE_CULLING_HUD

	void ZonesDrawCullingHUDPriv()
	{
#define FV_DRAW_VBOXES(type, containter, colour)					\
		{															\
		type::const_iterator travIt  = containter.begin();			\
		type::const_iterator travEnd = containter.end();			\
		while (travIt != travEnd)									\
		{															\
		Geometrics::wireBox(travIt->second, colour, true);			\
		++travIt;													\
	}																\
	}

		//FV_GUARD;
		FvMatrix saveView = Moo::rc().view();
		FvMatrix saveProj = Moo::rc().projection();

		Moo::rc().push();
		Moo::rc().world(FvMatrix::IDENTITY);

		FvMatrix view = FvMatrix::IDENTITY;
		FvVector3 cameraPos = FvZoneManager::Instance().cameraNearPoint();
		cameraPos.y += s_bCullHUDDist;
		view.lookAt(cameraPos, FvVector3(0, -1, 0), FvVector3(0, 0, 1));
		Moo::rc().view(view);

		FvMatrix project = FvMatrix::IDENTITY;

		project.orthogonalProjection(
			s_bCullHUDDist * Moo::rc().screenWidth() / Moo::rc().screenHeight(),
			s_bCullHUDDist, 0, -s_bCullHUDDist * 2);
		project.row(0).z = 0;
		project.row(1).z = 0;
		project.row(2).z = 0;
		project.row(3).z = 0;
		Moo::rc().projection(project);

		Moo::rc().setRenderState( D3DRS_ZENABLE, FALSE );
		Moo::rc().setRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );
		FV_DRAW_VBOXES(BBoxVector, s_kTraversedZones, Moo::Colour(0.5, 0.5, 0.5, 1.0));
		FV_DRAW_VBOXES(BBoxVector, s_kVisibleZones, Moo::Colour(1.0, 0.0, 0.0, 1.0));
		FV_DRAW_VBOXES(BBoxVector, s_kFringeZones, Moo::Colour(1.0, 1.0, 0.0, 1.0));
		FV_DRAW_VBOXES(BBoxVector, s_kReflectedZones, Moo::Colour(0.0, 0.0, 1.0, 1.0));

		FvVector3 cameraX = FvZoneManager::Instance().cameraAxis(X_AXIS) * 50;
		FvVector3 cameraY = FvZoneManager::Instance().cameraAxis(Y_AXIS) * 50;
		FvVector3 cameraZ = FvZoneManager::Instance().cameraAxis(Z_AXIS) * 150;

		Moo::Material::setVertexColour();
		std::vector<FvVector3> cameraLines;
		cameraLines.push_back(cameraPos);
		cameraLines.push_back(cameraPos + cameraZ + cameraX + cameraY);
		cameraLines.push_back(cameraPos + cameraZ - cameraX + cameraY);
		cameraLines.push_back(cameraPos);
		cameraLines.push_back(cameraPos + cameraZ + cameraX - cameraY);
		cameraLines.push_back(cameraPos + cameraZ - cameraX - cameraY);
		cameraLines.push_back(cameraPos);
		cameraLines.push_back(cameraPos + cameraZ + cameraX + cameraY);
		cameraLines.push_back(cameraPos + cameraZ + cameraX - cameraY);
		cameraLines.push_back(cameraPos);
		cameraLines.push_back(cameraPos + cameraZ - cameraX + cameraY);
		cameraLines.push_back(cameraPos + cameraZ - cameraX - cameraY);
		cameraLines.push_back(cameraPos);
		Geometrics::drawLinesInWorld(&cameraLines[0],
			cameraLines.size(),
			cameraZ.y >= 0
			? Moo::Colour(1.0f, 1.0f, 1.0f, 1.0f)
			: Moo::Colour(0.7f, 0.7f, 0.7f, 1.0f));



		/** Experimental **/
		ChunkSpacePtr space = FvZoneManager::Instance().cameraSpace();
		if (space.exists())
		{
			for (ChunkMap::iterator it = space->chunks().begin();
				it != space->chunks().end();
				it++)
			{
				for (unsigned int i = 0; i < it->second.size(); i++)
				{
					if (it->second[i]->Online())
					{
						Geometrics::wireBox(
							it->second[i]->BoundingBox(),
							true // it->second[i]->Removable()
							? Moo::Colour(1.0f, 1.0f, 1.0f, 1.0f)
							: Moo::Colour(0.0f, 1.0f, 0.0f, 1.0f),
							true);
					}
				}
			}
		}






		Moo::rc().pop();
		Moo::rc().view(saveView);
		Moo::rc().projection(saveProj);

#undef FV_DRAW_VBOXES
	}

#endif // FV_ENABLE_CULLING_HUD

} 

#endif // FV_SERVER