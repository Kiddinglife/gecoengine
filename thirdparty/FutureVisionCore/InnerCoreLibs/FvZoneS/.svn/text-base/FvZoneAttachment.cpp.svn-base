#include "FvZoneAttachment.h"
#include "FvZoneSpace.h"
#include "FvAttachment.h"

FvZoneAttachment::FvZoneAttachment() :
	FvZoneDynamicEmbodiment( (WantFlags)(WANTS_VISIT | WANTS_TICK) ),
	m_bNeedsSync( false ),
	m_bInited( false ),
	m_bIsRender(false)
{
}

FvZoneAttachment::FvZoneAttachment( FvAttachmentPtr spAttachment ) :
	FvZoneDynamicEmbodiment( (WantFlags)(WANTS_VISIT | WANTS_TICK) ),
	m_bNeedsSync( false ),
	m_bInited( true ),
	m_spAttachment( spAttachment )
{
	// FV_GUARD
	// FV_IF_NOT_MF_ASSERT_DEV( m_spAttachment )
	// {
	//	FV_EXIT( "attach to NULL object" );
	// }
	this->GetAttachment()->Attach();
}

FvZoneAttachment::~FvZoneAttachment()
{
	if(m_bInited)
		this->GetAttachment()->Detach();
}

void FvZoneAttachment::Tick( float fTime )
{
	if ( !m_bInited )
	{
		m_bInited = true;
		this->GetAttachment()->Attach();
	}

	this->GetAttachment()->Tick( fTime );
}

void FvZoneAttachment::Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor )
{
	if((VisitMark() == FvZone::ms_uiNextMark) && m_bIsRender)
	{
		return;
	}
	else
	{
		m_bIsRender = GetAttachment()->Visit(pkCamera,pkVisitor);
	}
}

void FvZoneAttachment::Toss( FvZone *pkZone )
{
	// FV_GUARD;
	this->FvZoneDynamicEmbodiment::Toss( pkZone );

	this->GetAttachment()->Tossed( m_pkZone );
}

void FvZoneAttachment::EnterSpace( FvZoneSpacePtr spSpace, bool bTransient )
{
	// FV_GUARD;
	this->FvZoneDynamicEmbodiment::EnterSpace( spSpace, bTransient );
	m_bNeedsSync = false;

	if (!bTransient)
		this->GetAttachment()->EnterWorld();

	FvBoundingBox kBB = FvBoundingBox::ms_kInsideOut;
	this->BoundingBox(kBB);
}

void FvZoneAttachment::LeaveSpace( bool bTransient )
{
	// FV_GUARD;
	if (!bTransient)
		this->GetAttachment()->LeaveWorld();

	this->FvZoneDynamicEmbodiment::LeaveSpace( bTransient );
}

void FvZoneAttachment::Move( float fTime )
{
	// FV_GUARD;
	if (m_bNeedsSync)
	{
		m_bNeedsSync = false;
		this->Sync();
	}

	this->GetAttachment()->Move( fTime );

	this->FvZoneDynamicEmbodiment::Move( fTime );
}

void FvZoneAttachment::BoundingBox( FvBoundingBox &kBB ) const
{
	// FV_GUARD;
	kBB = FvBoundingBox::ms_kInsideOut;
	this->GetAttachment()->BoundingBoxAcc( kBB, true );
}

void FvZoneAttachment::SetPosition( const FvVector3 &kPosition )
{
	this->GetAttachment()->SetPosition(kPosition);

	if (!m_spSpace) return;

	if (!m_spSpace->Ticking())
		this->Sync();
	else
		m_bNeedsSync = true;
}

bool FvZoneAttachment::AddZBounds( FvBoundingBox &kBB ) const
{
	// FV_GUARD;

	FvBoundingBox kVBB = FvBoundingBox::ms_kInsideOut;
	this->GetAttachment()->BoundingBoxAcc(kVBB);
	if (!kVBB.InsideOut())
	{
		kVBB.TransformBy(m_spSpace->CommonInverse());
		kBB.AddZBounds(kVBB.MinBounds().z);
		kBB.AddZBounds(kVBB.MaxBounds().z);
	}
	
	return true;
}

void FvZoneAttachment::VisitPick(Ogre::RenderQueue *pkQueue, const Ogre::Vector4& kColor)
{
	FV_ASSERT(this->GetAttachment());
	this->GetAttachment()->VisitPick(pkQueue,kColor);
}
