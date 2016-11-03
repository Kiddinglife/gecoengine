#include "FvClientZoneSpace.h"
#include "FvZoneSpace.h"
#include "FvZoneManager.h"
#include "FvZone.h"

#include <FvHullTree.h>
#include <FvDebug.h>
#include <FvWatcher.h>
#include <FvTimeStamp.h>

#include <queue>
#include <algorithm>
#include "FvRenderManager.h"

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

FvClientZoneSpace::FvClientZoneSpace( FvZoneSpaceID uiID ) :
FvBaseZoneSpace( uiID ),
m_bTicking( false ),
//pOutLight_( NULL ),
//sunLight_( NULL ),
//ambientLight_( 0.1f, 0.1f, 0.1f, 1.f ),
//lights_( new Moo::LightContainer() ),
//enviro_(id),
m_kCommon( FvMatrix::IDENTITY ),
m_kCommonInverse( FvMatrix::IDENTITY),
m_bIsMapped(false)
#if FV_UMBRA_ENABLE
,m_pkUmbraCell( NULL ),
m_pkUmbraInsideCell( NULL )
#endif
{
}


/**
*	Destructor.
*/
FvClientZoneSpace::~FvClientZoneSpace()
{
	//FV_GUARD;
	this->BlurSpace();
#if FV_UMBRA_ENABLE
	if (m_pkUmbraCell)
	{
		m_pkUmbraCell->release();
	}
	if (m_pkUmbraInsideCell)
	{
		m_pkUmbraInsideCell->release();
	}
#endif
}

void FvClientZoneSpace::BlurSpace()
{
	m_kCurrentFocus.Origin( m_kCurrentFocus.OriginX() + 10000, 0 );
}

void FvClientZoneSpace::Clear()
{
	//FV_GUARD;
	this->BlurSpace();

	this->m_kHomeless.clear();

	this->FvBaseZoneSpace::Clear();
#if FV_UMBRA_ENABLE
	if (m_pkUmbraCell)
	{
		m_pkUmbraCell->release();
		m_pkUmbraCell = NULL;
	}
	if (m_pkUmbraInsideCell)
	{
		m_pkUmbraInsideCell->release();
		m_pkUmbraInsideCell = NULL;
	}
#endif

	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton()._getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);

	((FvRenderManager*)pkSceneManager)->ResetOC();
}


extern FvString g_specialConsoleString;
#define HULL_DEBUG(x) (void)0

FvUInt64 g_CSCTimeLimit;

#ifndef FV_SERVER

const int FV_FOCUS_RANGE = int(3001.f / FV_GRID_RESOLUTION);

void FvClientZoneSpace::Focus( const FvVector3 &kPoint )
{
	//FV_GUARD;
	//static DogWatch dwFocus( "Focus FvZone" );
	//dwFocus.start();

	int	iCX = int(kPoint.x / FV_GRID_RESOLUTION);		if (kPoint.x < 0.f) iCX--;
	int	iCY = int(kPoint.y / FV_GRID_RESOLUTION);		if (kPoint.y < 0.f) iCY--;

	m_kCurrentFocus.Origin( iCX, iCY );

	for (int x = iCX - ColumnGrid::SPANH; x <= iCX + ColumnGrid::SPANH; x++)
	{
		for (int z = iCY - ColumnGrid::SPANH; z <= iCY + ColumnGrid::SPANH; z++)
		{
			Column * & rpCol = m_kCurrentFocus( x, z );
			if (rpCol != NULL && rpCol->IsStale())
			{
				delete rpCol;
				rpCol = NULL;
			}
		}
	}

	int bs = m_kBlurred.size();
	g_CSCTimeLimit = Timestamp() + StampsPerSecond() * 2 / 1000;
	bool hitTimeLimit = false;

	for (unsigned int i = 0; i < m_kBlurred.size(); i++)
	{
		FvZone * pkZone = m_kBlurred[i];

		const FvVector3 & cen = pkZone->Centre();
		int nx = int(cen.x / FV_GRID_RESOLUTION);	if (cen.x < 0.f) nx--;
		int ny = int(cen.y / FV_GRID_RESOLUTION);	if (cen.y < 0.f) ny--;
		nx -= iCX;
		ny -= iCY;

		if (-FV_FOCUS_RANGE <= nx && nx <= FV_FOCUS_RANGE &&
			-FV_FOCUS_RANGE <= ny && ny <= FV_FOCUS_RANGE)
		{
			if (hitTimeLimit && (
				nx <= -(FV_FOCUS_RANGE-1) || nx >= (FV_FOCUS_RANGE-1) ||
				ny <= -(FV_FOCUS_RANGE-1) || ny >= (FV_FOCUS_RANGE-1))) continue;

			m_kBlurred.erase( m_kBlurred.begin() + i );
			i--;

			for (int x = nx - 1; x <= nx + 1; x++) for (int z = ny - 1; z <= ny + 1; z++)
			{
				Column * pCol = m_kCurrentFocus( iCX + x, iCY + z );
				if (pCol != NULL) pCol->ShutIfSeen( pkZone );
			}

			pkZone->Focus();

			for (int x = nx - 1; x <= nx + 1; x++) for (int z = ny - 1; z <= ny + 1; z++)
			{
				Column * pCol = m_kCurrentFocus( iCX + x, iCY + z );
				if (pCol != NULL) pCol->OpenAndSee( pkZone );
			}

			if (Timestamp() > g_CSCTimeLimit) hitTimeLimit = true;
#ifdef FV_EDITOR_ENABLED
			hitTimeLimit = false;
#endif // FV_EDITOR_ENABLED
		}

	}

	for (int x = iCX - ColumnGrid::SPANH; x <= iCX + ColumnGrid::SPANH; x++)
	{
		for (int y = iCY - ColumnGrid::SPANH; y <= iCY + ColumnGrid::SPANH; y++)
		{
			Column * pCol = m_kCurrentFocus( x, y );
			if (pCol != NULL) pCol->Soft(
				x <= iCX-(FV_FOCUS_RANGE-1) || x >= iCX+(FV_FOCUS_RANGE-1) ||
				y <= iCY-(FV_FOCUS_RANGE-1) || y >= iCY+(FV_FOCUS_RANGE-1) );
		}
	}

	//dwFocus.stop();

	if (bs != m_kBlurred.size())
	{
		for (int i = int(m_kHomeless.size()-1); i >= 0; i--)
		{
			i = FV_MIN( i, int(m_kHomeless.size()-1) );
			FvZoneItemPtr pHomelessItem = m_kHomeless[i];
			pHomelessItem->Nest( static_cast<FvZoneSpace*>( this ) );
		}
	}
}

#if FV_UMBRA_ENABLE
Umbra::Cell* FvClientZoneSpace::UmbraCell()
{
	if (m_pkUmbraCell == NULL)
		m_pkUmbraCell = Umbra::Cell::Create();
	return m_pkUmbraCell;
}

Umbra::Cell* FvClientZoneSpace::UmbraInsideCell()
{
	if (m_pkUmbraInsideCell == NULL)
		m_pkUmbraInsideCell = Umbra::Cell::Create();
	return m_pkUmbraInsideCell;
}
#endif

void FvClientZoneSpace::Tick( float fTime )
{
	//FV_GUARD;
	m_bTicking = true;

	//this->UpdateHeavenlyLighting();

	for (FvZoneMap::iterator it = m_kCurrentZones.begin();
		it != m_kCurrentZones.end();
		it++)
	{
		for (unsigned int i = 0; i < it->second.size(); i++)
		{
			if (!it->second[i]->Online()) continue;

			it->second[i]->Tick( fTime );
		}
	}

	//static DogWatch dwHomeless( "Homeless" );
	//dwHomeless.start();

	for (unsigned int i = 0; i < m_kHomeless.size(); i++)
	{		
		m_kHomeless[i]->Tick( fTime );
	}
	//dwHomeless.stop();

	m_bTicking = false;
}

//void FvClientZoneSpace::UpdateHeavenlyLighting()
//{
	//FV_GUARD;
	//if (pOutLight_ != NULL && sunLight_)
	//{
	//	FvVector3 localDir( 0, 0, -1 );
	//	sunLight_->direction( localDir );
	//	sunLight_->colour( Moo::Colour( pOutLight_->sunColour ) );

	//	the dawn/dusk sneaky swap changes moonlight for
	//	sunlight when the moon is brighter
	//	sunLight_->worldTransform( pOutLight_->mainLightTransform() );		

	//	ambientLight_ = Moo::Colour( pOutLight_->ambientColour );
	//	lights_->ambientColour( ambientLight_ );
	//}
//}
#endif // FV_SERVER

void FvClientZoneSpace::AddHomelessItem( FvZoneItemPtr spItem )
{
	//FV_GUARD;
	m_kHomeless.push_back( spItem );
}

void FvClientZoneSpace::DelHomelessItem( FvZoneItemPtr spItem )
{
	//FV_GUARD;
	std::vector<FvZoneItemPtr>::iterator found =
		std::find( m_kHomeless.begin(), m_kHomeless.end(), spItem );
	if (found != m_kHomeless.end()) m_kHomeless.erase( found );
}

//void FvClientZoneSpace::ambientLight( Moo::Colour col )
//{
//	//FV_GUARD;
//	ambientLight_ = col;
//	lights_->ambientColour( ambientLight_ );
//}

void FvClientZoneSpace::TransformSpaceToCommon( FvVector3 &kPos, FvVector3 &kDir )
{
	//FV_GUARD;
	if (&*FvZoneManager::Instance().CameraSpace() == this) return;

	FvMatrix kMatrix;
	kMatrix.SetRotate( kDir[0], kDir[1], kDir[2] );
	kMatrix.SetTranslate( kPos );
	kMatrix.PostMultiply( m_kCommon );
	kPos = kMatrix.ApplyToOrigin();
	kDir[0] = kMatrix.Yaw();
	kDir[1] = kMatrix.Pitch();
	kDir[2] = kMatrix.Roll();
}

void FvClientZoneSpace::TransformCommonToSpace( FvVector3 &kPos, FvVector3 &kDir )
{
	//FV_GUARD;
	if (&*FvZoneManager::Instance().CameraSpace() == this) return;

	FvMatrix kMatrix;
	kMatrix.SetRotate( kDir[0], kDir[1], kDir[2] );
	kMatrix.SetTranslate( kPos );
	kMatrix.PostMultiply( m_kCommonInverse );
	kPos = kMatrix.ApplyToOrigin();
	kDir[0] = kMatrix.Yaw();
	kDir[1] = kMatrix.Pitch();
	kDir[2] = kMatrix.Roll();
}

bool FvClientZoneSpace::IsMapped() const
{
	return this->m_bIsMapped;
}

#include "FvHulltree.h"
#include "FvWorldTriangle.h"

FvClientZoneSpace::Column::Column( int iX, int iY ) :
FvBaseZoneSpace::Column( iX, iY ),
m_bSoft( false )
{
}

FvClientZoneSpace::Column::~Column()
{
}

template <class T, int ISPAN>
FvFocusGrid<T,ISPAN>::FvFocusGrid() :
m_iCX( 0 ),
m_iCY( 0 )
{
	//FV_GUARD;
	for (int y = 0; y < SPANX; y++) for (int x = 0; x < SPANX; x++)
	{
		m_pkGrid[y][x] = NULL;
	}
}

template <class T, int ISPAN>
FvFocusGrid<T,ISPAN>::~FvFocusGrid()
{
	//FV_GUARD;
	for (int y = 0; y < SPANX; y++) for (int x = 0; x < SPANX; x++)
	{
		delete m_pkGrid[y][x];
	}
}

template <class T, int ISPAN>
void FvFocusGrid<T,ISPAN>::Origin( int iCX, int iCY )
{
	//FV_GUARD;
	while (m_iCY < iCY)
	{
		for (int x = 0; x < SPANX; x++) this->EraseEntry( x, m_iCY - SPANH );
		m_iCY++;
	}
	while (m_iCY > iCY)
	{
		for (int x = 0; x < SPANX; x++) this->EraseEntry( x, m_iCY + SPANH );
		m_iCY--;
	}

	while (m_iCX < iCX)
	{
		for (int y = 0; y < SPANX; y++) this->EraseEntry( m_iCX - SPANH, y );
		m_iCX++;
	}
	while (m_iCX > iCX)
	{
		for (int y = 0; y < SPANX; y++) this->EraseEntry( m_iCX + SPANH, y );
		m_iCX--;
	}
}

template <class T, int ISPAN>
void FvFocusGrid<T,ISPAN>::EraseEntry( int iX, int iY )
{
	//FV_GUARD;
	T * & pT = this->operator()( iX, iY );
	if (pT != NULL)
	{
		delete pT;
		pT = NULL;
	}
}