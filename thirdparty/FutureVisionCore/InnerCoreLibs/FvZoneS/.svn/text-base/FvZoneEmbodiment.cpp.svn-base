#include "FvZoneEmbodiment.h"
#include "FvZone.h"
#include "FvZoneSpace.h"
#include "FvSceneNode.h"

FvZoneEmbodiment::FvZoneEmbodiment( WantFlags eWantFlags ) :
FvZoneItem( eWantFlags )
{
}

FvZoneEmbodiment::FvZoneEmbodiment( SceneNode *pkNode, 
								   WantFlags eWantFlags ) :
FvZoneItem( eWantFlags )
{
	
}

FvZoneEmbodiment::~FvZoneEmbodiment()
{
}

void FvZoneEmbodiment::Toss( FvZone *pkZone )
{
	this->FvZoneItem::Toss( pkZone );
}

FvZoneDynamicEmbodiment::FvZoneDynamicEmbodiment( WantFlags eWantFlags ) :
	FvZoneEmbodiment( eWantFlags ),
	m_spSpace( NULL )
{
}

FvZoneDynamicEmbodiment::~FvZoneDynamicEmbodiment()
{
	// FV_GUARD;
	 FV_ASSERT_DEV( !m_spSpace );
}

void FvZoneDynamicEmbodiment::Nest( FvZoneSpace *pkSpace )
{
	// FV_GUARD;
	const FvVector3 &kWPos = this->GetPosition();
	FvVector3 kTPos = m_spSpace->CommonInverse().ApplyPoint(
		FvVector3( kWPos[0], kWPos[1] + 0.1f, kWPos[2] ) );

	FvZone *pkDest = m_spSpace->FindZoneFromPoint( kTPos );
	if (pkDest != m_pkZone)
	{
		if (m_pkZone != NULL)
			m_pkZone->DelDynamicItem( this );
		else
			m_spSpace->DelHomelessItem( this );

		if (pkDest != NULL)
			pkDest->AddDynamicItem( this );
		else
		{
			FV_WARNING_MSG( "FvZoneDynamicEmbodiment::Nest: "
				"Went from being in a chunk to being homeless!\n" );
			m_spSpace->AddHomelessItem( this );
		}
	}
}

void FvZoneDynamicEmbodiment::EnterSpace( FvZoneSpacePtr spSpace, bool bTransient )
{
	// FV_GUARD;
	 FV_ASSERT_DEV( !m_spSpace );

	if( m_spSpace.Exists() )
		LeaveSpace(bTransient);

	const FvVector3 &kWPos = this->GetPosition();
	FvVector3 kTPos = spSpace->CommonInverse().ApplyPoint(
		FvVector3( kWPos[0], kWPos[1] + 0.1f, kWPos[2] ) );

	FvZone *pkDest = spSpace->FindZoneFromPoint( kTPos );
	if (pkDest != NULL)
		pkDest->AddDynamicItem( this );
	else
		spSpace->AddHomelessItem( this );

	m_spSpace = spSpace;
	m_kLastTranslation = kTPos;
}

void FvZoneDynamicEmbodiment::LeaveSpace( bool bTransient )
{
	// FV_GUARD;
	FV_ASSERT(m_spSpace);
	FV_IF_NOT_ASSERT_DEV( m_spSpace )
	{
		return;
	}

	if (m_pkZone != NULL)
		m_pkZone->DelDynamicItem( this );
	else
		m_spSpace->DelHomelessItem( this );
	m_spSpace = NULL;
}

void FvZoneDynamicEmbodiment::Move( float fTime )
{

}

void FvZoneDynamicEmbodiment::Sync()
{
	// FV_GUARD;
	 FV_ASSERT_DEV( m_spSpace && !m_spSpace->Ticking() );

	const FvVector3 &kWPos = this->GetPosition();
	FvVector3 kTPos;
	
	if( m_spSpace ) 
		kTPos = m_spSpace->CommonInverse().ApplyPoint(
			FvVector3( kWPos[0], kWPos[1], kWPos[2] + 0.1f ) );
	else
		kTPos = FvVector3( kWPos[0], kWPos[1], kWPos[2] + 0.1f );

	if (m_pkZone != NULL)
	{
		FvBoundingBox kBound;
		this->BoundingBox(kBound);
		FvVector2 kRadius = kBound.MaxBounds() - kBound.MinBounds();
		m_pkZone->ModDynamicItem( this, m_kLastTranslation, kTPos, kRadius.Length(), true );
	}
	else
	{
		FvZone *pkDest = m_spSpace->FindZoneFromPoint( kTPos );
		if (pkDest != NULL)
		{
			m_spSpace->DelHomelessItem( this );
			pkDest->AddDynamicItem( this );
		}
	}

	m_kLastTranslation = kTPos;
}

FvZoneStaticEmbodiment::FvZoneStaticEmbodiment( WantFlags eWantFlags ) :
	FvZoneEmbodiment( (WantFlags)(eWantFlags | WANTS_NEST) ),
	m_spSpace( NULL )
{
}

FvZoneStaticEmbodiment::~FvZoneStaticEmbodiment()
{
	 FV_ASSERT_DEV( !m_spSpace );
}

void FvZoneStaticEmbodiment::Nest( FvZoneSpace *pkSpace )
{
	// FV_GUARD;
	const FvVector3 &kWPos = this->GetPosition();
	FvVector3 kTPos = m_spSpace->CommonInverse().ApplyPoint(
		FvVector3( kWPos[0], kWPos[1] + 0.1f, kWPos[2] ) );

	FvZone *pkDest = m_spSpace->FindZoneFromPoint( kTPos );
	if (pkDest != m_pkZone)
	{
		if (m_pkZone != NULL)
			m_pkZone->DelStaticItem( this );
		else
			m_spSpace->DelHomelessItem( this );

		if (pkDest != NULL)
			pkDest->AddStaticItem( this );
		else
		{
			FV_WARNING_MSG( "FvZoneStaticEmbodiment::Nest: "
				"Went from being in a chunk to being homeless!\n" );
			m_spSpace->AddHomelessItem( this );
		}
	}
}

void FvZoneStaticEmbodiment::EnterSpace( FvZoneSpacePtr spSpace, bool bTransient )
{
	// FV_GUARD;
	 FV_ASSERT_DEV( !m_spSpace );

	if( m_spSpace )
		LeaveSpace(bTransient);

	const FvVector3 &kWPos = this->GetPosition();
	FvVector3 kTPos = spSpace->CommonInverse().ApplyPoint(
		FvVector3( kWPos[0], kWPos[1] + 0.1f, kWPos[2] ) );

	FvZone *pkDest = spSpace->FindZoneFromPoint( kTPos );
	if (pkDest != NULL)
		pkDest->AddStaticItem( this );
	else
		spSpace->AddHomelessItem( this );

	m_spSpace = spSpace;
}

void FvZoneStaticEmbodiment::LeaveSpace( bool bTransient )
{
	// FV_GUARD;
	FV_IF_NOT_ASSERT_DEV( m_spSpace )
	{
		return;
	}

	if (m_pkZone != NULL)
		m_pkZone->DelStaticItem( this );
	else
		m_spSpace->DelHomelessItem( this );
	m_spSpace = NULL;
}
