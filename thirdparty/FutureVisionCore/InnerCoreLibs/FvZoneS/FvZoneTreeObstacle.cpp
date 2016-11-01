#include "FvZoneTreeObstacle.h"

#include "FvZoneSpace.h"
#include "FvZoneObstacle.h"

#include <FvHullTree.h>
#include <FvBsp.h>

#include <FvTree.h>

#include <set>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

FvZoneCache::Instance<FvZoneTreeObstacle> FvZoneTreeObstacle::ms_kInstance;

//----------------------------------------------------------------------------
FvZoneTreeObstacle::FvZoneTreeObstacle( FvZone &kZone ) :
m_pkZone( &kZone )
{

}
//----------------------------------------------------------------------------
FvZoneTreeObstacle::~FvZoneTreeObstacle()
{
	//FV_GUARD;
	for (unsigned int o = 0; o < m_kObstacles.size(); o++)
	{
		this->DelFromSpace( *m_kObstacles[o] );
	}
}
//----------------------------------------------------------------------------
typedef std::set< FvZoneSpace::Column * > ColumnSet;
//----------------------------------------------------------------------------
int FvZoneTreeObstacle::Focus()
{
	//FV_GUARD;
	int iFoco = 0;

	for (unsigned int o = 0; o < m_kObstacles.size(); o++)
	{
		iFoco += this->AddToSpace( *m_kObstacles[o] );
	}

	return iFoco;
}
//----------------------------------------------------------------------------
void FvZoneTreeObstacle::AddObstacle( FvZoneObstacle *pkObstacle )
{
	//FV_GUARD;
	m_kObstacles.push_back( pkObstacle );

	if (pkObstacle->Item()->Zone()->Focussed())
	{
		this->AddToSpace( *m_kObstacles.back() );
	}
}
//----------------------------------------------------------------------------
void FvZoneTreeObstacle::DelObstacles( FvZoneItemPtr spItem )
{
	//FV_GUARD;
	for (unsigned int i = 0; i < m_kObstacles.size(); i++)
	{
		FvZoneObstacle &kCSO = *m_kObstacles[i];
		if (kCSO.Item() == spItem)
		{
			this->DelFromSpace( kCSO );

			m_kObstacles.erase( m_kObstacles.begin() + i );

			i--;
		}
	}
}
//----------------------------------------------------------------------------
void FvZoneTreeObstacle::AddTree(const FvTreePtr& spTree,
	const FvMatrix &kWorld, FvZoneItemPtr spItem, bool bEditorProxy)
{
	//FV_GUARD;
	if (spTree == NULL || spItem == NULL)
		return;

	const FvBSPTree *pkBSPTree = spTree->GetBSPTree();

	if (pkBSPTree != NULL)
	{
		if (bEditorProxy)
		{
			FvBSPFlagsMap kBspMap;
			kBspMap.push_back( FV_TRIANGLE_TRANSPARENT | FV_TRIANGLE_NOCOLLIDE | FV_TRIANGLE_DOUBLESIDED );
			const_cast<FvBSPTree*>(pkBSPTree)->RemapFlags( kBspMap );
		}

		this->AddObstacle( new FvZoneBSPObstacle(
			*pkBSPTree, kWorld, &spTree->BoundingBox(), spItem ) );
	}
}
//----------------------------------------------------------------------------
int FvZoneTreeObstacle::AddToSpace(FvZoneObstacle &kCSO)
{
	//FV_GUARD;
	int iFoco = 0;

	ColumnSet kColumns;

	FvBoundingBox m_kBBTr = kCSO.m_kBB;
	m_kBBTr.TransformBy( kCSO.m_kTransform );

	const FvVector3 &kMinBT = m_kBBTr.MinBounds();
	const FvVector3 &kMaxBT = m_kBBTr.MaxBounds();

	for (int i = 0; i < 8; i++)
	{
		FvVector3 kPT(	(i&1) ? kMaxBT.x : kMinBT.x,
			(i&2) ? kMaxBT.y : kMinBT.y,
			(i&4) ? kMaxBT.z : kMinBT.z );

		FvZoneSpace::Column *pkColumn = this->m_pkZone->Space()->pColumn( kPT );

		if (pkColumn)
		{
			kColumns.insert( pkColumn );
		}
	}

	FvZoneSpace::Column *pkColumn = this->m_pkZone->Space()->pColumn( m_kBBTr.Centre() );
	if(pkColumn)
		kColumns.insert( pkColumn );

	if (kColumns.empty())
	{
		FV_ERROR_MSG( "FvZoneModelObstacle::AddToSpace: "
			"Object is not inside the space -\n"
			"\tmin = (%.1f, %.1f, %.1f). max = (%.1f, %.1f, %.1f)\n",
			kMinBT.x, kMinBT.y, kMinBT.z,
			kMaxBT.x, kMaxBT.y, kMaxBT.z );
	}

	for (ColumnSet::iterator kIt = kColumns.begin(); kIt != kColumns.end(); kIt++)
	{
		//FV_ASSERT_DEV( &**kIt );	
		if (*kIt)
		{
			(*kIt)->AddObstacle( kCSO );
		}

		++iFoco;
	}

	return iFoco;
}
//----------------------------------------------------------------------------
void FvZoneTreeObstacle::DelFromSpace(FvZoneObstacle &kCSO)
{
	//FV_GUARD;
	ColumnSet kColumns;

	FvBoundingBox kBBTr = kCSO.m_kBB;
	kBBTr.TransformBy( kCSO.m_kTransform );

	const FvVector3 &kMinBT = kBBTr.MinBounds();
	const FvVector3 &kMaxBT = kBBTr.MaxBounds();

	for (int i = 0; i < 8; i++)
	{
		FvVector3 kPT(	(i&1) ? kMaxBT.x : kMinBT.x,
			(i&2) ? kMaxBT.y : kMinBT.y,
			(i&4) ? kMaxBT.z : kMinBT.z );
		kColumns.insert( this->m_pkZone->Space()->pColumn( kPT, false ) );
	}

	FvZoneSpace::Column *pkColumn = this->m_pkZone->Space()->pColumn( kBBTr.Centre() );
	if(pkColumn)
		kColumns.insert( pkColumn );

	for (ColumnSet::iterator kIt = kColumns.begin(); kIt != kColumns.end(); kIt++)
	{
		FvZoneSpace::Column *pkCol = &**kIt;
		if (pkCol == NULL) continue;

		pkCol->Stale();
	}
}
//----------------------------------------------------------------------------
