#include "FvZoneDynamicObstacle.h"
#include "FvZoneModelObstacle.h"
#include "FvRenderManager.h"
#include "FvZoneVisitor.h"
#include "FvCamera.h"

#include <FvConcurrency.h>
#include <FvBGTaskManager.h>

class FvZoneDynamicObstacles : public FvZoneCache
{
public:
	FvZoneDynamicObstacles( FvZone &kZone ) : m_pkZone( &kZone ) { }
	~FvZoneDynamicObstacles() { FV_ASSERT_DEV( m_kCDOs.empty() ); }

	virtual int Focus();

	void Add( FvZoneDynamicObstacle *pkCDO );
	void Mod( FvZoneDynamicObstacle *pkCDO, const FvMatrix &kOldWorld );
	void Del( FvZoneDynamicObstaclePtr spCDO );

	static Instance<FvZoneDynamicObstacles> ms_kInstance;

private:
	void AddOne( const FvZoneObstacle &kCSO );
	void ModOne( const FvZoneObstacle &kCSO, const FvMatrix &kOldWorld );
	void DelOne( const FvZoneObstacle &kCSO );

	FvZone *m_pkZone;
	std::vector<FvZoneDynamicObstaclePtr> m_kCDOs;
};

FvZoneCache::Instance<FvZoneDynamicObstacles> FvZoneDynamicObstacles::ms_kInstance;

int FvZoneDynamicObstacles::Focus()
{
	//FV_GUARD;
	for (uint i = 0; i < m_kCDOs.size(); i++)
	{
		FvZoneObstacle *pkObstacle = m_kCDOs[i]->pObstacle()->Obstacle();
		this->AddOne( *pkObstacle );
	}
	return m_kCDOs.size();
}

void FvZoneDynamicObstacles::Add( FvZoneDynamicObstacle *pkCDO )
{
	//FV_GUARD;
	FV_IF_NOT_ASSERT_DEV( m_pkZone->Online() )
	{
		return;
	}

	m_kCDOs.push_back( pkCDO );

	if (m_pkZone->Focussed())
	{
		FvZoneObstacle *pkObstacle = pkCDO->pObstacle()->Obstacle();
		this->AddOne( *pkObstacle );
	}
}

void FvZoneDynamicObstacles::Mod( FvZoneDynamicObstacle *pkCDO,
								const FvMatrix &kOldWorld )
{
	//FV_GUARD;
	if (m_pkZone->Focussed())
	{
		FvZoneObstacle *pkObstacle = pkCDO->pObstacle()->Obstacle();
		this->ModOne( *pkObstacle, kOldWorld );
	}
}

void FvZoneDynamicObstacles::Del( FvZoneDynamicObstaclePtr spCDO )
{
	//FV_GUARD;
	unsigned int i;
	for (i = 0; i < m_kCDOs.size(); i++)
	{
		if (m_kCDOs[i] == spCDO) break;
	}
	FV_IF_NOT_ASSERT_DEV( i < m_kCDOs.size() )
	{
		return;
	}
	m_kCDOs[i] = m_kCDOs.back();
	m_kCDOs.pop_back();

	if (m_pkZone->Focussed())
	{
		FvZoneObstacle *pkObstacle = spCDO->pObstacle()->Obstacle();
		this->DelOne( *pkObstacle );
	}
}


void FvZoneDynamicObstacles::AddOne( const FvZoneObstacle &kCSO )
{
	//FV_GUARD;
	FvBoundingBox kObstacleBB( kCSO.m_kBB );
	kObstacleBB.TransformBy( kCSO.m_kTransform );

	for( int x = FvBaseZoneSpace::PointToGrid( kObstacleBB.MinBounds().x );
		x <= FvBaseZoneSpace::PointToGrid( kObstacleBB.MaxBounds().x );
		x++ )
	{
		for( int y = FvBaseZoneSpace::PointToGrid( kObstacleBB.MinBounds().y );
			y <= FvBaseZoneSpace::PointToGrid( kObstacleBB.MaxBounds().y );
			y++ )
		{
			FvVector3 kColumnPoint(	( x + 0.5f ) * FV_GRID_RESOLUTION,
				( y + 0.5f ) * FV_GRID_RESOLUTION,
				0.0f );

			FvZoneSpace::Column *pkColumn = 
				m_pkZone->Space()->pColumn( kColumnPoint, false );
			if( pkColumn )
			{
				FvBoundingBox kColumnBB( FvVector3((x + 0) * FV_GRID_RESOLUTION,
					(y + 0) * FV_GRID_RESOLUTION,
					0.0f ),
					FvVector3((x + 1) * FV_GRID_RESOLUTION,
					(y + 1) * FV_GRID_RESOLUTION,
					0.0f));
				kColumnBB.AddZBounds( FV_MIN_ZONE_HEIGHT );
				kColumnBB.AddZBounds( FV_MAX_ZONE_HEIGHT );

				if ( kColumnBB.Intersects( kObstacleBB ) )
					pkColumn->AddDynamicObstacle( kCSO );
			}
		}
	}
}

void FvZoneDynamicObstacles::ModOne( const FvZoneObstacle &kCSO,
								   const FvMatrix &kOldWorld )
{
	//FV_GUARD;
	FvBoundingBox kOldBB( kCSO.m_kBB );
	FvBoundingBox kNewBB( kCSO.m_kBB );

	kOldBB.TransformBy( kOldWorld );
	kNewBB.TransformBy( kCSO.m_kTransform );

	FvBoundingBox kSumBB( kOldBB );
	kSumBB.AddBounds( kNewBB );

	for( int x = FvBaseZoneSpace::PointToGrid( kSumBB.MinBounds().x );
		x <= FvBaseZoneSpace::PointToGrid( kSumBB.MaxBounds().x );
		x++ )
	{
		for( int y = FvBaseZoneSpace::PointToGrid( kSumBB.MinBounds().y );
			y <= FvBaseZoneSpace::PointToGrid( kSumBB.MaxBounds().y );
			y++ )
		{
			FvVector3 kColumnPoint(	( x + 0.5f ) * FV_GRID_RESOLUTION,
				( y + 0.5f ) * FV_GRID_RESOLUTION,
				0.0f );

			FvZoneSpace::Column *pkColumn =
				m_pkZone->Space()->pColumn( kColumnPoint, false );
			if( pkColumn )
			{
				FvBoundingBox kColumnBB(	FvVector3((x + 0) * FV_GRID_RESOLUTION,
					(y + 0) * FV_GRID_RESOLUTION,
					0.0f ),
					FvVector3((x + 1) * FV_GRID_RESOLUTION,
					(y + 1) * FV_GRID_RESOLUTION,
					0.0f));
				kColumnBB.AddZBounds( FV_MIN_ZONE_HEIGHT );
				kColumnBB.AddZBounds( FV_MAX_ZONE_HEIGHT );

				bool bInOld = kColumnBB.Intersects( kOldBB );
				bool bInNew = kColumnBB.Intersects( kNewBB );

				if (bInNew && !bInOld)
					pkColumn->AddDynamicObstacle( kCSO );

				if (!bInNew && bInOld)
					pkColumn->DelDynamicObstacle( kCSO );
			}
		}
	}
}

void FvZoneDynamicObstacles::DelOne( const FvZoneObstacle &kCSO )
{
	//FV_GUARD;
	FvBoundingBox kObstacleBB( kCSO.m_kBB );
	kObstacleBB.TransformBy( kCSO.m_kTransform );

	for( int x = FvBaseZoneSpace::PointToGrid( kObstacleBB.MinBounds().x );
		x <= FvBaseZoneSpace::PointToGrid( kObstacleBB.MaxBounds().x );
		x++ )
	{
		for( int y = FvBaseZoneSpace::PointToGrid( kObstacleBB.MinBounds().y );
			y <= FvBaseZoneSpace::PointToGrid( kObstacleBB.MaxBounds().y );
			y++ )
		{
			FvVector3 kColumnPoint( x * FV_GRID_RESOLUTION,
				y * FV_GRID_RESOLUTION,
				0.0f );

			FvZoneSpace::Column *pkColumn =
				m_pkZone->Space()->pColumn( kColumnPoint, false );
			if( pkColumn )
			{
				FvBoundingBox kColumnBB( kColumnPoint,
					FvVector3( (x+1) * FV_GRID_RESOLUTION,
					(y+1) * FV_GRID_RESOLUTION,
					0.0f));
				kColumnBB.AddZBounds( FV_MIN_ZONE_HEIGHT );
				kColumnBB.AddZBounds( FV_MAX_ZONE_HEIGHT );

				if ( kColumnBB.Intersects( kObstacleBB ) )
					pkColumn->DelDynamicObstacle( kCSO );
			}
		}
	}
}

FvSimpleMutex FvZoneDynamicObstacle::ms_kDeletionLock;
FvZoneDynamicObstacle::Models FvZoneDynamicObstacle::ms_kModels;

FvZoneDynamicObstacle::FvZoneDynamicObstacle( const FvString& kName,
											 const FvMatrix& kMatrix,
											 FvEmbodimentListener* pkListener ) :
FvZoneDynamicEmbodiment( (WantFlags)(WANTS_VISIT | WANTS_TICK | (4 << USER_FLAG_SHIFT)) ),
m_bEnterSpace(false),
m_pkListener(pkListener)
{
	//FV_GUARD;
	ms_kDeletionLock.Grab();
	ms_kModels.push_back(this);
	ms_kDeletionLock.Give();

	m_pkModelObstacle = new FvModelObstacle(kName,kMatrix);
	FvBGTaskManager::Instance().AddBackgroundTask(
		new FvCStyleBackgroundTask( 
		&FvZoneDynamicObstacle::DoLoadResources, this,
		&FvZoneDynamicObstacle::OnLoadResourcesComplete, this ) );
}

FvZoneDynamicObstacle::~FvZoneDynamicObstacle()
{
	//FV_GUARD;
	DeleteModel(this);

	delete m_pkModelObstacle;
}

void FvZoneDynamicObstacle::Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor )
{
	if(!m_pkModelObstacle->Loaded())
		return;

	if((VisitMark() == FvZone::ms_uiNextMark) && m_pkModelObstacle->m_bIsRender)
	{
		return;
	}
	else
	{
		m_pkModelObstacle->m_bIsRender = false;
	}

	if(!pkVisitor->Visit(this))
		return;

	m_pkModelObstacle->Visit(pkCamera,pkVisitor);
}

void FvZoneDynamicObstacle::VisitPick(Ogre::RenderQueue *pkQueue, const Ogre::Vector4& kColor)
{	
	if(m_pkModelObstacle->Loaded())
		return;

	m_pkModelObstacle->VisitPick(pkQueue,kColor);
}

void FvZoneDynamicObstacle::Toss( FvZone *pkZone )
{
	//FV_GUARD;
	if (m_pkZone != NULL)
		FvZoneDynamicObstacles::ms_kInstance( *m_pkZone ).Del( this );

	this->FvZoneDynamicEmbodiment::Toss( pkZone );

	if (m_pkZone != NULL)
		FvZoneDynamicObstacles::ms_kInstance( *m_pkZone ).Add( this );
}

void FvZoneDynamicObstacle::EnterSpace( FvZoneSpacePtr spSpace, bool bTransient )
{
	//FV_GUARD;
	if (!bTransient && 
		this->pObstacle()->Loaded()) 
	{
		this->pObstacle()->EnterWorld( this );
		this->FvZoneDynamicEmbodiment::EnterSpace( spSpace, bTransient );
	}
	else
	{
		m_bEnterSpace = true;
		m_spLastSpace = spSpace;
		m_bLastTransient = bTransient;
	}
}

void FvZoneDynamicObstacle::LeaveSpace( bool bTransient )
{
	//FV_GUARD;
	if (!bTransient &&
		this->pObstacle()->Loaded())
	{
		this->FvZoneDynamicEmbodiment::LeaveSpace( bTransient );
		this->pObstacle()->LeaveWorld();
	}
	else
	{
		m_bEnterSpace = false;
		m_spLastSpace = NULL;
		m_bLastTransient = bTransient;
	}
}

void FvZoneDynamicObstacle::Move( float fTime )
{
	//FV_GUARD;
	if(!m_pkModelObstacle->Loaded() || 
		!m_spSpace)
		return;

	this->Sync();

	FvModelObstacle *pkModelObstacle = this->pObstacle();
	FvZoneObstacle *pkObstacle = this->pObstacle()->Obstacle();
	if (pkObstacle)
	{
		FvMatrix kOldWorld = pkObstacle->m_kTransform;

		FvMatrix kWorld = pkModelObstacle->WorldTransform();
		FvMatrix kWorldInverse; kWorldInverse.Invert( kWorld );
		pkObstacle->m_kTransform = kWorld;
		pkObstacle->m_kTransformInverse = kWorldInverse;

		if (m_pkZone != NULL)
			FvZoneDynamicObstacles::ms_kInstance( *m_pkZone ).Mod( this, kOldWorld );
	}

	this->FvZoneDynamicEmbodiment::Move( fTime );
}

const FvVector3 &FvZoneDynamicObstacle::GetPosition() const
{
	//FV_GUARD;
	return this->pObstacle()->GetPosition();
}

void FvZoneDynamicObstacle::BoundingBox( FvBoundingBox &kBB ) const
{
	//FV_GUARD;
	kBB = pObstacle()->BoundingBox();
}

void FvZoneDynamicObstacle::DeleteModel(FvZoneDynamicObstacle *pkModel)
{
	ms_kDeletionLock.Grab();
	Models::iterator kFind = 
		std::find( ms_kModels.begin(),ms_kModels.end(), pkModel );
	ms_kModels.erase(kFind);
	ms_kDeletionLock.Give();
}

bool FvZoneDynamicObstacle::StillValid(FvZoneDynamicObstacle *pkModel)
{
	if(!pkModel)
		return false;

	ms_kDeletionLock.Grab();
	Models::iterator kFind =
		std::find(	ms_kModels.begin(),ms_kModels.end(), pkModel );	
	if (kFind == ms_kModels.end())
	{
		ms_kDeletionLock.Give();
		return false;
	}

	return true;
}

void FvZoneDynamicObstacle::DoLoadResources( void *pkSelf )
{
	FvZoneDynamicObstacle *pkModel = 
		static_cast< FvZoneDynamicObstacle * >( pkSelf );
	if(StillValid(pkModel))
	{
		pkModel->pObstacle()->Load();
		ms_kDeletionLock.Give();
	}
}

void FvZoneDynamicObstacle::OnLoadResourcesComplete( void *pkSelf )
{
	FvZoneDynamicObstacle *pkModel = 
		static_cast< FvZoneDynamicObstacle * >( pkSelf );
	if(StillValid(pkModel))
	{
		if(pkModel->m_bEnterSpace)
			pkModel->EnterSpace( pkModel->m_spLastSpace, pkModel->m_bLastTransient );
		else
			pkModel->LeaveSpace( pkModel->m_bLastTransient );

		if(pkModel->m_pkListener)
			pkModel->m_pkListener->OnLoadComplete(pkModel);

		ms_kDeletionLock.Give();
	}
}

FvSimpleMutex FvZoneStaticObstacle::ms_kDeletionLock;
FvZoneStaticObstacle::Models FvZoneStaticObstacle::ms_kModels;

FvZoneStaticObstacle::FvZoneStaticObstacle( const FvString& kName,
										   const FvMatrix& kMatrix,
										   FvEmbodimentListener* pkListener ) :
FvZoneStaticEmbodiment( (WantFlags)(WANTS_VISIT | WANTS_TICK) ),
m_bEnterSpace(false),
m_pkListener(pkListener)
{
	//FV_GUARD;
	ms_kDeletionLock.Grab();
	ms_kModels.push_back(this);
	ms_kDeletionLock.Give();

	m_pkModelObstacle = new FvModelObstacle(kName,kMatrix);
	FvBGTaskManager::Instance().AddBackgroundTask(
		new FvCStyleBackgroundTask( 
		&FvZoneStaticObstacle::DoLoadResources, this,
		&FvZoneStaticObstacle::OnLoadResourcesComplete, this ) );
}

FvZoneStaticObstacle::~FvZoneStaticObstacle()
{
	//FV_GUARD;
	DeleteModel(this);

	delete m_pkModelObstacle;
}

void FvZoneStaticObstacle::Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor )
{
	if(!m_pkModelObstacle->Loaded() || 
		!m_spSpace)
		return;

	if((VisitMark() == FvZone::ms_uiNextMark) && m_pkModelObstacle->m_bIsRender)
	{
		return;
	}
	else
	{
		m_pkModelObstacle->m_bIsRender = false;
	}

	if(!pkVisitor->Visit(this))
		return;

	m_pkModelObstacle->Visit(pkCamera,pkVisitor);
}

void FvZoneStaticObstacle::VisitPick(Ogre::RenderQueue *pkQueue, const Ogre::Vector4& kColor)
{	
	if(!m_pkModelObstacle->Loaded() || 
		!m_spSpace)
		return;

	m_pkModelObstacle->VisitPick(pkQueue,kColor);
}

void FvZoneStaticObstacle::Toss( FvZone *pkZone )
{
	//FV_GUARD;
	FV_ASSERT(pObstacle()->Loaded());

	if (m_pkZone != NULL)
		FvZoneModelObstacle::ms_kInstance( *m_pkZone ).DelObstacles( this );

	this->FvZoneStaticEmbodiment::Toss( pkZone );

	if (m_pkZone != NULL)
	{
		FvModelObstacle *pkModelObstacle = this->pObstacle();
		FvZoneObstacle *pkObstacle = pkModelObstacle->Obstacle();
		if (pkObstacle)
		{
			FvZoneModelObstacle &kZoneModelObstacle =
				FvZoneModelObstacle::ms_kInstance( *m_pkZone );

			FvMatrix kWorldInverse; kWorldInverse.Invert( this->pObstacle()->WorldTransform() );

			pkObstacle->m_kTransform = this->pObstacle()->WorldTransform();
			pkObstacle->m_kTransformInverse = kWorldInverse;
			kZoneModelObstacle.AddObstacle( pkObstacle );
		}
	}
}

void FvZoneStaticObstacle::Lend( FvZone *pkLender )
{
	//FV_GUARD;
	FV_ASSERT(pObstacle()->Loaded());

	if (m_pkZone)
	{
		FvBoundingBox kBB = pObstacle()->BoundingBox();
		kBB.TransformBy( pObstacle()->WorldTransform() );
		this->LendByBoundingBox( pkLender, kBB );
	}
}

void FvZoneStaticObstacle::EnterSpace( FvZoneSpacePtr spSpace, bool bTransient )
{
	//FV_GUARD;
	if (!bTransient && 
		this->pObstacle()->Loaded()) 
	{
		this->pObstacle()->EnterWorld( this );
		this->FvZoneStaticEmbodiment::EnterSpace( spSpace, bTransient );
	}
	else
	{
		m_bEnterSpace = true;
		m_spLastSpace = spSpace;
		m_bLastTransient = bTransient;
	}
}

void FvZoneStaticObstacle::LeaveSpace( bool bTransient )
{
	//FV_GUARD;
	if (!bTransient &&
		this->pObstacle()->Loaded())
	{
		this->FvZoneStaticEmbodiment::LeaveSpace( bTransient );
		this->pObstacle()->LeaveWorld();
	}
	else
	{
		m_bEnterSpace = false;
		m_spLastSpace = NULL;
		m_bLastTransient = bTransient;
	}
}

const FvVector3 &FvZoneStaticObstacle::GetPosition() const
{
	//FV_GUARD;
	return this->pObstacle()->GetPosition();
}

void FvZoneStaticObstacle::BoundingBox( FvBoundingBox &kBB ) const
{
	//FV_GUARD;
	kBB = pObstacle()->BoundingBox();
}

void FvZoneStaticObstacle::DeleteModel(FvZoneStaticObstacle *pkModel)
{
	ms_kDeletionLock.Grab();
	Models::iterator kFind = 
		std::find( ms_kModels.begin(),ms_kModels.end(), pkModel );
	ms_kModels.erase(kFind);
	ms_kDeletionLock.Give();
}

bool FvZoneStaticObstacle::StillValid(FvZoneStaticObstacle *pkModel)
{
	if(!pkModel)
		return false;

	ms_kDeletionLock.Grab();
	Models::iterator kFind =
		std::find(	ms_kModels.begin(),ms_kModels.end(), pkModel );	
	if (kFind == ms_kModels.end())
	{
		ms_kDeletionLock.Give();
		return false;
	}

	return true;
}

void FvZoneStaticObstacle::DoLoadResources( void *pkSelf )
{
	FvZoneStaticObstacle *pkModel = 
		static_cast< FvZoneStaticObstacle * >( pkSelf );
	if(StillValid(pkModel))
	{
		pkModel->pObstacle()->Load();
		ms_kDeletionLock.Give();
	}
}

void FvZoneStaticObstacle::OnLoadResourcesComplete( void *pkSelf )
{
	FvZoneStaticObstacle *pkModel = 
		static_cast< FvZoneStaticObstacle * >( pkSelf );
	if(StillValid(pkModel))
	{
		if(pkModel->m_bEnterSpace)
			pkModel->EnterSpace( pkModel->m_spLastSpace, pkModel->m_bLastTransient );
		else
			pkModel->LeaveSpace( pkModel->m_bLastTransient );

		if(pkModel->m_pkListener)
			pkModel->m_pkListener->OnLoadComplete(pkModel);

		ms_kDeletionLock.Give();
	}
}

FvModelObstacle::FvModelObstacle( const FvString& kName,
								 const FvMatrix& kMatrix ):
m_bIsRender(false),
m_kResourceName(kName),
m_kWorldTransform(kMatrix),
m_kBoundingBox(FvVector3(0.f,0.f,0.f),
			   FvVector3(0.5f,0.5f,0.5f)),
m_bLoaded(false)
{
	
}

FvModelObstacle::~FvModelObstacle()
{
	m_spModel = NULL;
}

void FvModelObstacle::EnterWorld( FvZoneItem *pkItem )
{
	FV_ASSERT( m_bLoaded );

	FvMatrix kMat = this->WorldTransform();

	FV_ASSERT(m_spModel);
	if(m_spModel->GetBSPTree())
		m_spObstacle = new FvZoneBSPObstacle( 
			*m_spModel->GetBSPTree(), kMat, &m_spModel->BoundingBox(), pkItem );
}

void FvModelObstacle::LeaveWorld()
{
	FV_ASSERT( m_bLoaded );
	
	m_spObstacle = NULL;
}

const FvVector3 &FvModelObstacle::GetPosition() const
{
	return m_kWorldTransform.ApplyToOrigin();
}

void FvModelObstacle::WorldTransform(const FvMatrix& kMatrix)
{
	m_kWorldTransform = kMatrix;
	if(m_bLoaded)
		m_spModel->Transform(m_kWorldTransform);
}

const FvBoundingBox &FvModelObstacle::BoundingBox()
{
	if(!m_bLoaded)
		return m_kBoundingBox;

	m_kBoundingBox = m_spModel->BoundingBox();
	m_kBoundingBox.TransformBy( m_kWorldTransform );
	return m_kBoundingBox;
}

void FvModelObstacle::Load()
{
	m_spModel = new FvGameObjectModel();
	m_spModel->Load(m_kResourceName,"General","Dynamic",m_kWorldTransform,true);
	m_spModel->Transform(m_kWorldTransform);
	m_bLoaded = true;
}

void FvModelObstacle::Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor )
{
	FV_ASSERT(m_bLoaded);

	if(pkVisitor->HasFlag(FvZoneVisitor::VISIT_FLAG_WATER_REFLECTION))
		return;

	float fDistSq = pkCamera->getLodCamera()->getRealPosition().squaredDistance(m_spModel->GetNode()->_getDerivedPosition());

	float fInvVolume = m_spModel->GetVolume();

	if((fDistSq * fInvVolume) > FvRenderManager::GetLodDistance(3).y)
		return;

	FvInt32 u32LodLevel = m_spModel->GetCurrentLodLevel();

	const FvVector2 kParams = FvRenderManager::GetLodDistance(u32LodLevel);

	if(u32LodLevel && fDistSq < kParams.x)
	{
		--u32LodLevel;
	}
	else if(u32LodLevel < 3 && fDistSq > kParams.y)
	{
		++u32LodLevel;
	}

	m_spModel->SetCurrentLodLevel(u32LodLevel);

	const FvGameObjectModel::Node& kNode = m_spModel->GetModleNode();

	if(pkCamera->isVisible(kNode.m_pkNode->_getWorldAABB()))
	{
		kNode.m_pkNode->_findVisibleObjects(pkCamera,pkVisitor->GetRenderQueue(),
			pkVisitor->GetVisibleObjectsBoundsInfo());
		m_bIsRender = true;
	}
}

void FvModelObstacle::VisitPick(Ogre::RenderQueue *pkQueue, const Ogre::Vector4& kColor)
{
	FV_ASSERT(m_bLoaded);
	m_spModel->UpdateRenderQueue(pkQueue, kColor);
}