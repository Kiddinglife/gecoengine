#include "FvBaseZoneSpace.h"
#include "FvZone.h"
#include "FvZoneSpace.h"
#include "FvZoneObstacle.h"

#include <FvBsp.h>
#include <FvHullTree.h>
#include <FvQuadTree.h>
#include <FvDebug.h>
#include <FvWatcher.h>

#include <queue>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

class FvHullZone : public FvHullContents
{
public:
	FvHullZone( FvZone *pkZone ) : m_pkZone(pkZone ) {}

	~FvHullZone() { m_pkZone->Smudge(); }

	FvZone *m_pkZone;
};

int FvBaseZoneSpace::ms_iObstacleTreeDepth = 5;

FvBaseZoneSpace::FvBaseZoneSpace( FvZoneSpaceID id ) :
m_uiID( id ),
m_iMinGridX( 0 ),
m_iMaxGridX( 0 ),
m_iMinGridY( 0 ),
m_iMaxGridY( 0 )
{
}

FvBaseZoneSpace::~FvBaseZoneSpace()
{
	//FV_GUARD;
	while (!m_kCurrentZones.empty())
	{
		FvZoneMap::iterator it = m_kCurrentZones.begin();
		delete it->second.back();
	}
	m_kCurrentZones.clear();
	FV_ASSERT_DEV( m_kCurrentZones.empty() );
}

void FvBaseZoneSpace::AddZone( FvZone *pkZone )
{
	//FV_GUARD;
	m_kCurrentZones[pkZone->Identifier()].push_back(pkZone);
	if (pkZone->IsOutsideZone() )
	{
		std::pair< int, int > kCoord(pkZone->X(),pkZone->Y());
		m_kGridZones[kCoord].push_back(pkZone);
	}
	pkZone->Ratify();
}

FvZone * FvBaseZoneSpace::FindOrAddZone( FvZone *pkZone )
{
	//FV_GUARD;
	FvZoneMap::iterator kFound =
		m_kCurrentZones.find(pkZone->Identifier());
	if (kFound != m_kCurrentZones.end())
	{
		for (unsigned int i = 0; i < kFound->second.size(); i++)
		{
			if (pkZone->Mapping() == kFound->second[i]->Mapping())
			{
				if (pkZone != kFound->second[i])
					delete pkZone;
				return kFound->second[i];
			}
		}
	}

	this->AddZone(pkZone );
	return pkZone;
}

void FvBaseZoneSpace::DelZone( FvZone *pkZone )
{
	//FV_GUARD;
	if (pkZone->IsOutsideZone() )
	{
		std::pair< int, int > kCoord(pkZone->X(),pkZone->Y() );
		std::vector< FvZone* > & kMappings = m_kGridZones[kCoord];

		for (unsigned int i = 0; i < kMappings.size(); i++)
		{
			if ( kMappings[i] == pkZone )
			{
				kMappings.erase( kMappings.begin() + i );
				break;
			}
		}
		if (kMappings.empty())
			m_kGridZones.erase( kCoord );
	}

	FvZoneMap::iterator kFound = m_kCurrentZones.find(pkZone->Identifier() );
	if (kFound != m_kCurrentZones.end())
	{
		for (unsigned int i = 0; i < kFound->second.size(); i++)
		{
			if (kFound->second[i] ==pkZone)
			{
				kFound->second.erase( kFound->second.begin() + i );
				break;
			}
		}
		if (kFound->second.empty())
			m_kCurrentZones.erase( kFound );
	}
}

void FvBaseZoneSpace::Clear()
{
#if 0
	for (FvZoneMap::iterator it = m_kCurrentZones.begin();
		it != m_kCurrentZones.end();
		it++)
	{
		for (unsigned int i = 0; i < it->second.size(); i++)
		{
			FV_ASSERT_DEV( it->second[i]->loading() );
		}
	}
#endif

	m_kDataEntries.clear();
}

bool FvBaseZoneSpace::InBounds( int iGridX, int iGridY ) const
{
	return m_iMinGridX <= iGridX && iGridX <= m_iMaxGridX &&
		m_iMinGridY <= iGridY && iGridY <= m_iMaxGridY;
}


float g_fGridResolution = FV_GRID_RESOLUTION;

FvQuadTreeRange FvCalculateQuadTreeRange( const FvZoneObstacle &kInput, const FvVector2 &kOrigin,
						 float fRange, int iDepth )
{
	const float WIDTH = float(1 << iDepth);

	FvBoundingBox kBB = kInput.m_kBB;
	kBB.TransformBy( kInput.m_kTransform );

	FvQuadTreeRange kRV;

	kRV.m_kLeft		= int(WIDTH * (kBB.MinBounds().x - kOrigin.x)/fRange);
	kRV.m_kRight	= int(WIDTH * (kBB.MaxBounds().x - kOrigin.x)/fRange);
	kRV.m_kBottom	= int(WIDTH * (kBB.MinBounds().y - kOrigin.y)/fRange);
	kRV.m_kTop		= int(WIDTH * (kBB.MaxBounds().y - kOrigin.y)/fRange);

#if 0
	// TODO: For large objects, we could scale them up a bit so that they don't
	// create as many leaf nodes. This saves memory.
	int width = (rv.right_ - rv.left_);
	if (width > 8)
	{
		width >>= 3;

		FvUInt32 mask = ~1;
		while (width)
		{
			mask <<= 1;
			width >>= 1;
		}

		int oldLeft = rv.left_;
		int oldRight = rv.right_;

		rv.left_ &= mask;
		rv.right_ |= ~mask;
	}

	width = (rv.top_ - rv.bottom_);
	if (width > 8)
	{
		width >>= 3;

		FvUInt32 mask = ~1;
		while (width)
		{
			mask <<= 1;
			width >>= 1;
		}

		int oldBottom = rv.bottom_;
		int oldTop = rv.top_;

		rv.bottom_ &= mask;
		rv.top_ |= ~mask;
	}
#endif

	return kRV;
}

bool ContainsTestPoint( const FvZoneObstacle & /*kInput*/,
					   const FvVector3 & /*kPoint*/ )
{
	return true;
}

FvBaseZoneSpace::Column::Column( int x, int z ) :
m_pkZoneTree( NULL ),
m_pkObstacleTree( new FvObstacleTree( GridToPoint( x ), GridToPoint( z ),
			   FvBaseZoneSpace::ObstacleTreeDepth(),
			   FV_GRID_RESOLUTION ) ),
m_kHeldObstacles(),
m_pkOutsideZone( NULL ),
m_bStale( false ),
m_pkShutTo( NULL )
{
}

FvBaseZoneSpace::Column::~Column()
{
	//FV_GUARD;
	for (HeldObstacles::iterator iter = m_kHeldObstacles.begin();
		iter != m_kHeldObstacles.end();
		iter++)
	{
		const FvZoneObstacle * pObstacle = *iter;

		if (pObstacle->Zone() != NULL)
		{
			pObstacle->Zone()->Smudge();
		}
		pObstacle->DecRef();
	}

	delete m_pkObstacleTree;
	delete m_pkZoneTree;

	for (unsigned int i = 0; i < m_kHoldings.size(); i++)
	{
		delete m_kHoldings[i];
	}
}

void FvBaseZoneSpace::Column::AddZone( FvHullBorder & kBorder, FvZone *pkZone )
{
	//FV_GUARD;
	if (m_pkShutTo ==pkZone) return;

	FvHullContents *pkStuff = new FvHullZone(pkZone);

	if (pkZone->IsOutsideZone())
	{
		if (m_pkOutsideZone != NULL)
		{
			FV_ERROR_MSG( "Column::AddZone: Replacing outside chunk %s with %s!\n",
				m_pkOutsideZone->Identifier().c_str(),pkZone->Identifier().c_str() );

			if (pkZone->SizeStaticItems() <= m_pkOutsideZone->SizeStaticItems())
				pkZone = m_pkOutsideZone;
		}
		m_pkOutsideZone = pkZone;
	}
	else
	{
		if (m_pkZoneTree == NULL) m_pkZoneTree = new FvHullTree();
		m_pkZoneTree->Add( kBorder, pkStuff );

		if (m_pkOutsideZone != NULL)
		{
			m_pkOutsideZone->JogForeignItems();
		}
	}

	m_kHoldings.push_back( pkStuff );
}

void FvBaseZoneSpace::Column::AddObstacle( const FvZoneObstacle & kObstacle )
{
	//FV_GUARD;
	if (m_pkShutTo == kObstacle.Zone()) return;

	kObstacle.IncRef();
	m_kHeldObstacles.push_back( &kObstacle );
	m_pkObstacleTree->Add( kObstacle );
}

void FvBaseZoneSpace::Column::AddDynamicObstacle( const FvZoneObstacle & kObstacle)
{
	//FV_GUARD;
	if (m_pkShutTo == kObstacle.Zone()) return;

	kObstacle.IncRef();
	m_kHeldObstacles.push_back( &kObstacle );

	FvObstacleTree::Node::Elements &kElements = const_cast<
		FvObstacleTree::Node::Elements&>( m_pkObstacleTree->GetRoot()->GetElements() );
	kElements.push_back( &kObstacle );
}

void FvBaseZoneSpace::Column::DelDynamicObstacle( const FvZoneObstacle & kObstacle)
{
	//FV_GUARD;
	HeldObstacles::reverse_iterator iter = std::find(
		m_kHeldObstacles.rbegin(), m_kHeldObstacles.rend(), &kObstacle );
	if (iter == m_kHeldObstacles.rend()) return;

	*iter = m_kHeldObstacles.back();
	m_kHeldObstacles.pop_back();

	FvObstacleTree::Node::Elements & elements = const_cast<
		FvObstacleTree::Node::Elements&>( m_pkObstacleTree->GetRoot()->GetElements() );
	FvObstacleTree::Node::Elements::reverse_iterator found2 = std::find(
		elements.rbegin(), elements.rend(), &kObstacle );

	FV_IF_NOT_ASSERT_DEV( found2 != elements.rend() )
	{
		return;
	}

	*found2 = elements.back();
	elements.pop_back();

	kObstacle.DecRef();
}

FvZone * FvBaseZoneSpace::Column::FindZoneExcluding( const FvVector3 &kPoint,
												   FvZone *pkNot )
{
	//FV_GUARD;
	FvZone *pkZone = (m_pkOutsideZone != pkNot) ? m_pkOutsideZone : NULL;

	if (m_pkZoneTree != NULL)
	{
		const FvHullZone * result = static_cast< const FvHullZone * >(
			m_pkZoneTree->TestPoint( kPoint ) );

		while (result != NULL)
		{
			if( (pkZone == NULL ||pkZone == m_pkOutsideZone ||
				pkZone->Volume() > result->m_pkZone->Volume() ) &&
				result->m_pkZone->BoundingBox().Intersects( kPoint ) )
			{
				if (result->m_pkZone != pkNot)pkZone = result->m_pkZone;
			}

			result = static_cast<const FvHullZone*>( result->m_pkNext );
		}
	}

	return pkZone;
}

FvZone * FvBaseZoneSpace::Column::FindZone( const FvVector3 &kPoint )
{
	//FV_GUARD;
	FvZone *pkZone = m_pkOutsideZone;

	if (m_pkZoneTree != NULL)
	{
		const FvHullZone * result = static_cast< const FvHullZone * >(
			m_pkZoneTree->TestPoint( kPoint ) );

		while (result != NULL)
		{
			if ( (pkZone == m_pkOutsideZone ||pkZone->Volume() > result->m_pkZone->Volume() )
				&& result->m_pkZone->BoundingBox().Intersects( kPoint ) )
				pkZone = result->m_pkZone;

			result = static_cast<const FvHullZone*>( result->m_pkNext );
		}
	}

	return pkZone;
}

long FvBaseZoneSpace::Column::Size() const
{
	long sz = sizeof( Column );
	if (m_pkZoneTree != NULL)
	{
		sz += m_pkZoneTree->Size( 0, false );
	}

	sz += this->Obstacles().Size();

	sz += m_kHoldings.capacity() * sizeof( FvHullContents * );

	sz += m_kHoldings.size() * sizeof( FvHullZone );

	sz += m_kHeldObstacles.capacity() * sizeof( FvZoneObstacle * );

	return sz;
}

void FvBaseZoneSpace::Column::OpenAndSee( FvZone *pkZone )
{
	if (m_pkShutTo !=pkZone)
	{
		m_kSeen.push_back(pkZone );
	}
	m_pkShutTo = NULL;
}

void FvBaseZoneSpace::Column::ShutIfSeen( FvZone *pkZone )
{
	//FV_GUARD;
	if (std::find( m_kSeen.begin(), m_kSeen.end(),pkZone ) != m_kSeen.end())
	{
		m_pkShutTo =pkZone;
	}
	else
	{
		m_pkShutTo = NULL;
	}
}

FvZone * FvBaseZoneSpace::FindZone( const FvString &kIdentifier,
								  const FvString & kMappingName )
{
	//FV_GUARD;
	FvZoneMap::iterator found = m_kCurrentZones.find( kIdentifier );
	if (found == m_kCurrentZones.end()) return NULL;
	for (unsigned int i = 0; i < found->second.size(); i++)
	{
		if (kMappingName.empty() ||
			found->second[i]->Mapping()->Name() == kMappingName)
			return found->second[i];
	}
	return NULL;
}

FvUInt16 FvBaseZoneSpace::DataEntry(const ZoneSpaceEntryID& kEntryID, FvUInt16 uiKey, const FvString& kData)
{
	if(uiKey != FvUInt16(-1))
	{
		DataEntryMapKey kKey;
		kKey.kEntryID = kEntryID;
		kKey.uiKey = uiKey;
		if(m_kDataEntries.find(kKey) != m_kDataEntries.end())
			return FvUInt16(-1);

		m_kDataEntries.insert(std::make_pair(kKey, kData));
		return uiKey;
	}
	else
	{
		DataEntryMap::iterator itr;
		for(itr=m_kDataEntries.begin(); itr!=m_kDataEntries.end(); ++itr)
		{
			if(itr->first.kEntryID == kEntryID)
				break;
		}

		if(itr == m_kDataEntries.end())
			return FvUInt16(-1);

		uiKey = itr->first.uiKey;
		m_kDataEntries.erase(itr);
		return uiKey;
	}
}

FvBaseZoneSpace::DataValueReturn FvBaseZoneSpace::DataRetrieveSpecific(const ZoneSpaceEntryID& kEntryID, FvUInt16 uiKey)
{
	DataEntryMap::iterator itr;
	if(uiKey == FvUInt16(-1))
	{
		for(itr=m_kDataEntries.begin(); itr!=m_kDataEntries.end(); ++itr)
		{
			if(itr->first.kEntryID == kEntryID)
				break;
		}

		if(itr != m_kDataEntries.end())
			return DataValueReturn(itr->first.uiKey, &itr->second);
		else
			return DataValueReturn(FvUInt16(-1), NULL);
	}
	else
	{
		DataEntryMapKey kKey;
		kKey.kEntryID = kEntryID;
		kKey.uiKey = uiKey;
		itr = m_kDataEntries.find(kKey);
		if(itr != m_kDataEntries.end())
			return DataValueReturn(itr->first.uiKey, &itr->second);
		else
			return DataValueReturn(FvUInt16(-1), NULL);
	}
}

const FvString* FvBaseZoneSpace::DataRetrieveFirst(FvUInt16 uiKey)
{
	DataEntryMap::const_iterator found = this->DataRetrieve(uiKey);

	return (found != m_kDataEntries.end()) ? &found->second : NULL;
}

FvBaseZoneSpace::DataEntryMap::const_iterator FvBaseZoneSpace::DataRetrieve(FvUInt16 uiKey)
{
	DataEntryMapKey kKey;
	kKey.kEntryID.m_uiIP = 0;
	kKey.kEntryID.m_uiPort = 0;
	kKey.uiKey = uiKey;

	DataEntryMap::const_iterator found = m_kDataEntries.lower_bound(kKey);
	if(found != m_kDataEntries.end() && found->first.uiKey == uiKey)
		return found;
	else
		return m_kDataEntries.end();
}

void FvBaseZoneSpace::Emulate( FvSmartPointer<FvBaseZoneSpace> kRightfulSpace )
{
	//FV_GUARD;
	m_uiID = kRightfulSpace->m_uiID;
	m_kDataEntries = kRightfulSpace->m_kDataEntries;
	FV_ASSERT_DEV( kRightfulSpace->m_kCurrentZones.empty() );
}

void FvBaseZoneSpace::BlurredZone( FvZone *pkZone )
{
	//FV_GUARD;
	if (pkZone->Online())
	{
		m_kBlurred.push_back( pkZone );
	}
}

bool FvBaseZoneSpace::RemoveFromBlurred( FvZone *pkZone )
{
	//FV_GUARD;
	std::vector<FvZone*>::iterator found =
		std::find( m_kBlurred.begin(), m_kBlurred.end(),pkZone );

	if ( found != m_kBlurred.end() )
	{
		m_kBlurred.erase( found );
		return true;
	}

	return false;
}
