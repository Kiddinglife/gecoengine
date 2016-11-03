#include "FvZoneOverlapper.h"
#include "FvZoneManager.h"
#include "FvZoneSpace.h"
#include "FvZoneLoader.h"

#include <FvWatcher.h>
#include <FvDebug.h>

#include <OgreResourceGroupManager.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

FvZoneOverlapper::FvZoneOverlapper() :
m_pkOverlapper( NULL )
{
}

FvZoneOverlapper::~FvZoneOverlapper()
{
}

bool FvZoneOverlapper::Load( FvXMLSectionPtr spSection, FvZone *pkZone, FvString *kErrorString )
{
	//FV_GUARD;
	FV_IF_NOT_ASSERT_DEV( m_pkOverlapper == NULL )
	{
		if( kErrorString )
			*kErrorString = "Overlapper chunk item is already loaded";

		return false;
	}

	pkZone->HasInternalZones( true );

	m_kOverlapperID = spSection->AsString();
	FvString kError;
	if (!m_kOverlapperID.empty())
	{
		FvXMLSectionPtr spBBSect = spSection->OpenSection( "boundingBox" );
		if (!spBBSect)
		{
			Ogre::DataStreamPtr spDataStream;
			try
			{
				spDataStream = Ogre::ResourceGroupManager::getSingleton().
					openResource( pkZone->Mapping()->Path() + m_kOverlapperID + ".zone",
					pkZone->Mapping()->Group(),true);
			}
			catch (...){}
			if(!spDataStream.isNull())
			{
				spBBSect = FvXMLSection::OpenSection(spDataStream);
				spBBSect = spBBSect->OpenSection("boundingBox");
			}
		}
		if (spBBSect)
		{
			m_pkOverlapper = new FvZone( m_kOverlapperID, pkZone->Mapping() );
			//! modify by Uman, 20101216
#ifndef FV_SERVER
			FvZoneManager::Instance().LoadZoneExplicitly( m_kOverlapperID, pkZone->Mapping(), true );
#else
			FvZoneLoader::LoadNow( m_pkOverlapper );
#endif // FV_SERVER
//#ifndef FV_SERVER
//			FvZoneManager::Instance().LoadZoneExplicitly( m_kOverlapperID, pkZone->Mapping(), true );
//#endif // FV_SERVER
			return true;
		}
		else
		{
			kError = "Cannot find bounding box for overlapper for " + m_kOverlapperID;
		}
	}	
	else
	{
		kError = "Overlapper zone item is empty string";
	}

	if ( kErrorString )
	{
		*kErrorString = kError;
	}
	else
	{
		FV_ERROR_MSG( "%s\n", kError.c_str() );
	}

	return false;
}

void FvZoneOverlapper::Toss( FvZone *pkZone )
{
	//FV_GUARD;
	if (m_pkZone != NULL)
		FvZoneOverlappers::ms_kInstance( *m_pkZone ).Del( this );
	for (unsigned int i = 0; i < m_kAlsoIn.size(); i++)
		m_kAlsoIn[i]->Del( this, true );
	m_kAlsoIn.clear();
	this->FvZoneItem::Toss( pkZone );
	if (m_pkZone != NULL && m_pkOverlapper != NULL)
		FvZoneOverlappers::ms_kInstance( *m_pkZone ).Add( this );
}

void FvZoneOverlapper::Bind( bool bLooseNotBind )
{
	//FV_GUARD;
	if (!m_pkOverlapper->Ratified())
		m_pkOverlapper = m_pkZone->Space()->FindOrAddZone( m_pkOverlapper );
	if( !bLooseNotBind && m_pkOverlapper->Online() )
		m_pkOverlapper->BindPortals( true );
}

void FvZoneOverlapper::AlsoInAdd( FvZoneOverlappers *pkAlsoIn )
{
	//FV_GUARD;
	m_kAlsoIn.push_back( pkAlsoIn );
}

void FvZoneOverlapper::AlsoInDel( FvZoneOverlappers *pkAlsoIn )
{
	//FV_GUARD;
	for (unsigned int i = 0; i < m_kAlsoIn.size(); i++)
	{
		if (m_kAlsoIn[i] == pkAlsoIn)
		{
			m_kAlsoIn.erase( m_kAlsoIn.begin() + i );
			break;
		}
	}
}

#undef FV_IMPLEMENT_ZONE_ITEM_XML_ARGS
#define FV_IMPLEMENT_ZONE_ITEM_XML_ARGS (spSection, pkZone, &kErrorString)
FV_IMPLEMENT_ZONE_ITEM_XML( FvZoneOverlapper, overlapper, 0 )

FvZoneOverlappers::FvZoneOverlappers( FvZone &kZone ) :
m_kZone( kZone ),
m_bBound( false ),
m_bHalfBound( false ),
m_bComplete( true )
{
}

FvZoneOverlappers::~FvZoneOverlappers()
{
	//FV_GUARD;
	for (unsigned int i = 0; i < m_kForeign.size(); i++)
		m_kForeign[i]->AlsoInDel( this );
}

void FvZoneOverlappers::Bind( bool bLooseNotBind )
{
	//FV_GUARD;
	this->CheckIfComplete( m_bBound );

	if (m_bBound) return;

	for (unsigned int i = 0; i < m_kOverlappers.size(); i++)
		m_kOverlappers[i]->Bind( bLooseNotBind );

	FvZoneBoundaries &kBounds = m_kZone.Bounds();	
	for (FvZoneBoundaries::iterator kBIt = kBounds.begin();
		kBIt != kBounds.end();
		kBIt++)
	{
		FvZoneBoundary::Portals::iterator kPIt;
		for (kPIt = (*kBIt)->m_kBoundPortals.begin();
			kPIt != (*kBIt)->m_kBoundPortals.end();
			kPIt++)
		{
			FvZoneBoundary::Portal &kPortal = **kPIt;
			if (!kPortal.HasZone()) continue;
			FvZone *pkZone = kPortal.m_pkZone;
			if (!pkZone->Online() || !pkZone->IsOutsideZone()) continue;

			if (FvZoneOverlappers::ms_kInstance.Exists( *pkZone ))
			{
				this->CopyFrom( FvZoneOverlappers::ms_kInstance( *pkZone ) );
			}
		}
	}

	m_bHalfBound = true;

	this->Share();

	if (m_kOverlappers.empty())
	{
		m_bBound = true;
	}
	else
	{
		m_bBound = true;
	}
}

void FvZoneOverlappers::Share()
{
	//FV_GUARD;
	FvZoneBoundaries &kBounds = m_kZone.Bounds();
	for (FvZoneBoundaries::iterator kBIt = kBounds.begin();
		kBIt != kBounds.end();
		kBIt++)
	{
		FvZoneBoundary::Portals::iterator kPIt;
		for (kPIt = (*kBIt)->m_kBoundPortals.begin();
			kPIt != (*kBIt)->m_kBoundPortals.end();
			kPIt++)
		{
			FvZoneBoundary::Portal &kPortal = **kPIt;
			if (!kPortal.HasZone()) continue;
			FvZone *pkZone = kPortal.m_pkZone;
			if (!pkZone->Online() || !pkZone->IsOutsideZone()) continue;

			FvZoneOverlappers &kOth = FvZoneOverlappers::ms_kInstance( *pkZone );
			kOth.CopyFrom( *this );
		}
	}
}

void FvZoneOverlappers::CopyFrom( FvZoneOverlappers &kOther )
{
	//FV_GUARD;
	bool bAnyNew = false;

	for (unsigned int i = 0; i < kOther.m_kOverlappers.size(); i++)
	{
		FvZoneOverlapperPtr spOverlapper = kOther.m_kOverlappers[i];

		if( !spOverlapper->Overlapper()->Online() )
			continue;

		if (!m_kZone.BoundingBox().Intersects( spOverlapper->BoundingBox() ))
			continue;

		ZoneOverlappers::iterator kOIt;
		for (kOIt = m_kOverlappers.begin(); kOIt != m_kOverlappers.end(); ++kOIt)
		{
			if (*kOIt == spOverlapper) break;
			if ((*kOIt)->Overlapper() == spOverlapper->Overlapper()) break;
		}
		if (kOIt != m_kOverlappers.end()) continue;

		m_kOverlappers.push_back( spOverlapper );
		m_kForeign.push_back( spOverlapper );
		spOverlapper->AlsoInAdd( this );
		bAnyNew = true;

		FV_DEBUG_MSG( "FvZoneOverlappers::CopyFrom: %s also intersects with %s, "
			"which hails from %s, tipoff by %s\n",
			m_kZone.Identifier().c_str(),
			spOverlapper->Overlapper()->Identifier().c_str(),
			spOverlapper->Zone()->Identifier().c_str(),
			kOther.m_kZone.Identifier().c_str() );
	}

	if (bAnyNew)
	{
		if (!m_bBound)
		{
			FV_ASSERT_DEV( !m_bHalfBound );
			return;
		}

		this->Share();
	}
}

void FvZoneOverlappers::CheckIfComplete( bool bCheckNeighbours )
{
	//FV_GUARD;
	m_bComplete = true;
	FvZoneBoundaries &kBounds = m_kZone.Bounds();
	for (FvZoneBoundaries::iterator kBIt = kBounds.begin();
		kBIt != kBounds.end();
		kBIt++)
	{
		FvZoneBoundary::Portals::iterator kPIt;
		for (kPIt = (*kBIt)->m_kUnboundPortals.begin();
			kPIt != (*kBIt)->m_kUnboundPortals.end();
			kPIt++)
		{
			FvZoneBoundary::Portal &kPortal = **kPIt;
			if (!kPortal.HasZone()) continue;
			m_bComplete = false;
			break;
		}

		for (kPIt = (*kBIt)->m_kBoundPortals.begin();
			kPIt != (*kBIt)->m_kBoundPortals.end();
			kPIt++)
		{
			FvZoneBoundary::Portal &kPortal = **kPIt;
			if (!kPortal.HasZone()) continue;
			FvZone *pkZone = kPortal.m_pkZone;
			if (!pkZone->Online() || !pkZone->IsOutsideZone()) continue;

			if (bCheckNeighbours && FvZoneOverlappers::ms_kInstance.Exists( *pkZone ))
			{
				FvZoneOverlappers &kOther = FvZoneOverlappers::ms_kInstance( *pkZone );
				kOther.CheckIfComplete( false );
			}

			FvZoneBoundaries &kBounds2 = pkZone->Bounds();
			for (FvZoneBoundaries::iterator kBIt2 = kBounds2.begin();
				kBIt2 != kBounds2.end() && m_bComplete;
				kBIt2++)
			{
				FvZoneBoundary::Portals::iterator kPIt2;
				for (kPIt2 = (*kBIt2)->m_kUnboundPortals.begin();
					kPIt2 != (*kBIt2)->m_kUnboundPortals.end();
					kPIt2++)
				{
					FvZoneBoundary::Portal &kPortal2 = **kPIt2;
					if (!kPortal2.HasZone()) continue;

					FvVector3 kWPT;
					FvZoneBoundary::V2Vector &kPTS = kPortal2.m_kPoints;
					for (unsigned int i = 0; i < kPTS.size(); i++)
					{
						pkZone->Transform().ApplyPoint( kWPT, FvVector3( kPortal2.m_kOrigin+
							kPortal2.m_kUAxis * kPTS[i][0] + kPortal2.m_kVAxis * kPTS[i][1] ) );
						if (m_kZone.BoundingBox().Intersects( kWPT, 1.f ))
						{	
							m_bComplete = false;
							break;
						}
					}
				}
			}
		}
	}
}

void FvZoneOverlappers::Touch( FvZone &kZone )
{
	//FV_GUARD;
	if (kZone.IsOutsideZone())
		FvZoneOverlappers::ms_kInstance( kZone );
}

void FvZoneOverlappers::Add( FvZoneOverlapperPtr spOverlapper, bool bForeign )
{
#ifdef FV_EDITOR_ENABLED
	for (Overlappers::iterator it = m_kOverlappers.begin();
		it != m_kOverlappers.end();
		++it)
	{
		if ((*it)->Overlapper()->Identifier() ==
			spOverlapper->Overlapper()->Identifier())
		{
			FV_ERROR_MSG( "ChunkOverlappers::add(%s): "
				"Tried to add two overlappers for the same chunk %s\n",
				m_pkZone.Identifier().c_str(),
				spOverlapper->Overlapper()->Identifier().c_str() );
			return;
		}
	}
#endif

	m_kOverlappers.push_back( spOverlapper );

	if (bForeign)
		m_kForeign.push_back( spOverlapper );
}

void FvZoneOverlappers::Del( FvZoneOverlapperPtr spOverlapper, bool bForeign )
{
	//FV_GUARD;
	ZoneOverlappers::iterator kFound = std::find(
		m_kOverlappers.begin(), m_kOverlappers.end(), spOverlapper );
	if (kFound != m_kOverlappers.end())
		m_kOverlappers.erase( kFound );

	if (bForeign)
	{
		ZoneOverlappers::iterator kFound = std::find(
			m_kForeign.begin(), m_kForeign.end(), spOverlapper );
		if (kFound != m_kForeign.end())
			m_kForeign.erase( kFound );
	}
}

FvZoneCache::Instance<FvZoneOverlappers> FvZoneOverlappers::ms_kInstance;

static std::vector<FvZone*> OverlapperFinder( FvZone *pkZone )
{
	//FV_GUARD;
	std::vector<FvZone*> kResult;
	FvZoneOverlappers::ZoneOverlappers kOverlappers = FvZoneOverlappers::ms_kInstance( *pkZone ).Overlappers();
	for( FvZoneOverlappers::ZoneOverlappers::iterator kIter = kOverlappers.begin();
		kIter != kOverlappers.end(); ++kIter )
	{
		FvZone *pkZone = (*kIter)->Overlapper();
		if( pkZone && pkZone->Online() )
			kResult.push_back( pkZone );
	}
	return kResult;
}

static struct OverlapperFinderRegister
{
	OverlapperFinderRegister()
	{
		//FV_GUARD;
		if( !FvZone::OverlapperFinder() )
			FvZone::OverlapperFinder( OverlapperFinder );
	}
}
OverlapperFinderRegister;
