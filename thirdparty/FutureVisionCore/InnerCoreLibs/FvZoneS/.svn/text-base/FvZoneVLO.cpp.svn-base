#include "FvZoneVLO.h"
#include "FvZone.h"
#include "FvZoneSpace.h"

#include <FvDebug.h>
#include <FvConcurrency.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

FvVeryLargeObject::UniqueObjectList FvVeryLargeObject::ms_kUniqueObjects;
#ifdef FV_EDITOR_ENABLED
FvUInt32 FvVeryLargeObject::ms_uiCurrentSelectionMark=0;
#endif

FvZoneVLO::Factories *FvZoneVLO::ms_pkFactories = NULL;


namespace
{
	FvSimpleMutex ms_kItemListMutex;
}

FvVLOFactory::FvVLOFactory(
		const FvString &kSection,
		int iPriority,
		Creator pfCreator ) :
	m_iPriority( iPriority ),
	m_pfCreator( pfCreator )
{
	// FV_GUARD;
	FvZoneVLO::RegisterFactory( kSection, *this );
}

bool FvVLOFactory::Create( FvZone *pkZone, FvXMLSectionPtr spSection, 
						FvString kUID ) const
{
	// FV_GUARD;
	if (m_pfCreator == NULL) return false;
	return (*m_pfCreator)( pkZone, spSection, kUID );
}

void FvZoneVLO::RegisterFactory( const FvString &kSection,
	const FvVLOFactory &kFactory )
{
	// FV_GUARD;
	if (ms_pkFactories == NULL)
	{
		ms_pkFactories = new Factories();
	}

	const FvVLOFactory *& pEntry = (*ms_pkFactories)[ kSection ];

	if (pEntry == NULL || pEntry->Priority() < kFactory.Priority())
	{
		pEntry = &kFactory;
	}
}

/*static*/ void FvZoneVLO::Fini()
{
	// FV_GUARD;
	delete ms_pkFactories;
	ms_pkFactories = NULL;
}

FvZoneVLO::FvZoneVLO( WantFlags eWantFlags ) :
	FvZoneItem( eWantFlags ),
	m_spObject( NULL ),
	m_bDirty( false ),
	m_bCreationRoot( false )
{
}

FvZoneVLO::~FvZoneVLO()
{
	// FV_GUARD;
	if (m_spObject)
	{
		m_spObject->RemoveItem( this );
		m_spObject = NULL;
	}
}

bool FvZoneVLO::LoadItem( FvZone *pkZone, FvXMLSectionPtr spSection )
{
	// FV_GUARD;
	FV_IF_NOT_ASSERT_DEV( ms_pkFactories != NULL )
	{
		return false;
	}

	FvString kType = spSection->ReadString( "type", "" );
	FvString kUID = spSection->ReadString( "uid", "" );
	Factories::iterator kFound = ms_pkFactories->find( kType );	
	if (kFound != ms_pkFactories->end() && (FvVeryLargeObject::GetObject(kUID) == NULL))
	{
        FvString kFile = pkZone->Mapping()->Path() + kUID + ".vlo";
		FvXMLSectionPtr spDS;
		try
		{
			Ogre::DataStreamPtr spDataStream = Ogre::ResourceGroupManager::getSingleton().
				openResource( kFile ,
				pkZone->Mapping()->Group(),true);
			if(!spDataStream.isNull())
			{
				spDS = FvXMLSection::OpenSection(spDataStream);
				spDS = spDS->OpenSection(kType);
			}
		}catch (Ogre::Exception &){}

		if (spDS && kFound->second->Create( pkZone, spDS, kUID ))
		{
#ifdef FV_EDITOR_ENABLED
			FvVeryLargeObjectPtr pObj = FvVeryLargeObject::GetObject( kUID );
			FvXMLSectionPtr pVLOSection = BWResource::openSection( kFile );
			if (pVLOSection)
				pObj->Section(pVLOSection);
#endif // FV_EDITOR_ENABLED
			return true;
		}
		else
        {
            FV_ERROR_MSG( "FvZoneVLO::LoadItem invalid or missing file %s", kFile.c_str());
			return false;
        }
	}
	return true;
}

#ifdef FV_EDITOR_ENABLED

bool FvZoneVLO::BuildVLOSection( FvXMLSectionPtr spObjectSection, FvZone *pkZone, FvString &kType, FvString &kUID )
{
	// FV_GUARD;
	if (spObjectSection)
	{
		Factories::iterator kFound = ms_pkFactories->find( kType );
		if (kFound != ms_pkFactories->end())
		{
			if (kFound->second->Create( pkZone, spObjectSection, kUID ))
			{
				m_spObject = FvVeryLargeObject::GetObject( kUID );
				m_spObject->AddItem( this );
			}
		}
	}
	else
		return false;

	if ( m_spObject )
	{
		FvString kFileName( kUID + ".vlo" );

		FvXMLSectionPtr spDS = pkZone->Mapping()->pDirSection();
		FvXMLSectionPtr spVLOSection = spDS->NewSection( kFileName );

		if (!spVLOSection)
			return false;

		FvXMLSectionPtr spWaterSection = spVLOSection->NewSection( kType );
		spWaterSection->SetParent( spVLOSection );
		m_spObject->Section( spVLOSection );
		m_spObject->Save();
		spWaterSection->SetParent( NULL );

		return true;
	}
	else
		return false;
}

bool FvZoneVLO::CreateVLO( FvXMLSectionPtr spSection, FvZone *pkZone )
{
	// FV_GUARD;
	FvString kType = spSection->ReadString( "type", "" );
	FvString kUID = spSection->ReadString( "uid", "" );
	if (kUID == "" && kType != "")
	{
		kUID = FvVeryLargeObject::GenerateUID();
		return BuildVLOSection( spSection->OpenSection( kType ), pkZone, kType, kUID );
	}
	return false;
}

bool FvZoneVLO::CreateLegacyVLO( FvXMLSectionPtr spSection, FvZone *pkZone, FvString &kType )
{	
	// FV_GUARD;
	if (kType != "")
	{
		FvString kUID = FvVeryLargeObject::GenerateUID();
		return BuildVLOSection( spSection, pkZone, kType, kUID );
	}
	return false;
}

bool FvZoneVLO::CloneVLO( FvXMLSectionPtr spSection, FvZone *pkZone, FvVeryLargeObjectPtr spSource )
{
	// FV_GUARD;
	FvString kType = spSection->ReadString( "type", "" );
	if (kType != "")
	{
		FvXMLSectionPtr spObjectSection = spSource->section()->OpenSection(type);
		return BuildVLOSection( spObjectSection, pkZone, kType, FvVeryLargeObject::GenerateUID() );
	}
	return false;
}
#endif // FV_EDITOR_ENABLED

bool FvZoneVLO::Load( FvXMLSectionPtr spSection, FvZone *pkZone )
{
	// FV_GUARD;
	FvString kUID = spSection->ReadString( "uid", "" );
	FvString kType = spSection->ReadString( "type", "" );
	if (kUID != "" && kType != "")
	{
		bool bRet = FvZoneVLO::LoadItem( pkZone, spSection );
		if (bRet)
		{
			m_spObject = FvVeryLargeObject::GetObject( kUID );
			m_spObject->AddItem( this );
		}

		return bRet;
	}
	return false;
}

void FvZoneVLO::SyncInit()
{
	// FV_GUARD;	
#if FV_UMBRA_ENABLE
	if (m_pkZone)
	{
		FvBoundingBox bb = m_spObject->ZoneBB( this->m_pkZone );
		if (!bb.insideOut())
		{
			pUmbraModel_ = UmbraModelProxy::getObbModel( bb.minBounds(), bb.maxBounds() );
			pUmbraObject_ = UmbraObjectProxy::get( pUmbraModel_ );
			FvMatrix m = FvMatrix::identity;
			Umbra::Cell* umbraCell = this->m_pkZone->getUmbraCell();
			m = this->m_pkZone->Transform();
			pUmbraObject_->object()->setUserPointer( (void*)this );
			pUmbraObject_->object()->setObjectToCellMatrix( (Umbra::Matrix4x4&)m );
			pUmbraObject_->object()->setCell( umbraCell );
		}
	}
	else
	{
		pUmbraModel_ = NULL;
		pUmbraObject_ = NULL;
	}
#endif // FV_UMBRA_ENABLE

	m_spObject->SyncInit( this );
}

void FvZoneVLO::Lend( FvZone *pkZone )
{
	// FV_GUARD;
	if (m_spObject)
		m_spObject->Lend( pkZone);
}

void FvZoneVLO::Toss( FvZone *pkZone )
{
	// FV_GUARD;
	if (m_pkZone)
	{
		RemoveCollisionScene( );
#if FV_UMBRA_ENABLE
		if (pkZone == NULL && m_spObject)
		{
			m_spObject->Unlend( m_pkZone );
		}
#endif // FV_UMBRA_ENABLE
	}

	this->FvZoneItem::Toss( pkZone );
}

void FvZoneVLO::Sway( const FvVector3 &kSrc, const FvVector3 &kDest, const float fDiameter )
{
	// FV_GUARD;
	if (m_spObject)
		m_spObject->Sway( kSrc, kDest, fDiameter );
}

void FvZoneVLO::Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor )
{
	// FV_GUARD;
	if (m_spObject && m_pkZone)
		m_spObject->Visit( m_pkZone->Space(), pkCamera, pkVisitor );
}

FvZoneVLO* FvVeryLargeObject::ContainsZone( const FvZone *pkZone ) const
{
	// FV_GUARD;
	FvSimpleMutexHolder kHolder(ms_kItemListMutex);

	ZoneItemList::const_iterator it;
	for (it = m_kItemList.begin(); it != m_kItemList.end(); it++)
	{
		if ((*it)->Zone() == pkZone)
			return (*it);
	}
	return NULL;
}

FvZoneItemXMLFactory::Result FvZoneVLO::Create( FvZone *pkZone, FvXMLSectionPtr spSection )
{
	// FV_GUARD;
	FvZoneVLO *pkVLO = new FvZoneVLO( (WantFlags)(WANTS_VISIT | WANTS_SWAY | WANTS_NEST) );

	if (!pkVLO->Load( spSection, pkZone ))
	{
		delete pkVLO;
		return FvZoneItemXMLFactory::Result( NULL,
			"Failed to create " + spSection->ReadString( "type", "<unknown type>" ) +
			" VLO " + spSection->ReadString( "uid", "<unknown id>" ) );
	}
	else
	{
		pkZone->AddStaticItem( pkVLO );
		return FvZoneItemXMLFactory::Result( pkVLO );
	}
}

FvZoneItemXMLFactory FvZoneVLO::ms_kFactory( "vlo", 0, FvZoneVLO::Create );

FvVeryLargeObject::FvVeryLargeObject()
	: m_bRebuild( false )
#ifdef FV_EDITOR_ENABLED
	,m_spDataSection( NULL ),
	m_bListModified( false ),
	m_bObjectCreated( false ),
	m_uiSelectionMark( 0 ),
	m_kZonePath( "" )
#endif
{ 
}

FvVeryLargeObject::FvVeryLargeObject( FvString kUID, FvString kType ) :
	m_kType( kType ),
	m_bRebuild( false )
#ifdef FV_EDITOR_ENABLED
	,m_spDataSection( NULL ),
	m_bListModified( false ),
	m_bObjectCreated( false ),
	m_uiSelectionMark( 0 ),
	m_kZonePath( "" )
#endif
{
	SetUID( kUID );		 
}

FvVeryLargeObject::~FvVeryLargeObject()
{
}


#ifdef FV_EDITOR_ENABLED

void FvVeryLargeObject::DeleteUnused()
{
	// FV_GUARD;
	UniqueObjectList::iterator it = ms_kUniqueObjects.begin();
	for (;it != ms_kUniqueObjects.end(); it++)
	{
		FvVeryLargeObjectPtr spObj = (*it).second;
		if (spObj)
			spObj->Cleanup();
	}
}

void FvVeryLargeObject::SaveAll()
{
	// FV_GUARD;
	FvUniqueObjectList::iterator it = ms_kUniqueObjects.begin();
	for (;it != ms_kUniqueObjects.end(); it++)
	{
		FvVeryLargeObjectPtr spObj = (*it).second;
		if (spObj)
			spObj->SaveFile(NULL);
	}
}

bool FvVeryLargeObject::EDShouldDraw()
{
	// FV_GUARD;D;
	if( !FvEditorZoneItem::HideAllOutside() )
		return true;

	FvSimpleMutexHolder kHolder(ms_kItemListMutex);
	for( ZoneItemList::iterator iter = m_kItemList.begin(); iter != m_kItemList.end(); ++iter )
	{
		if( !(*iter)->Zone()->IsOutsideZone() )
			return true;
	}
	return false;
}

bool FvVeryLargeObject::IsObjectCreated() const
{
	return m_bObjectCreated;
}

FvString FvVeryLargeObject::GenerateUID()
{
	// FV_GUARD;
	FvString kUID( FvUniqueID::Generate() );
	std::transform( kUID.begin(), kUID.end(), kUID.begin(), tolower );
	return kUID;
}


#endif // FV_EDITOR_ENABLED

void FvVeryLargeObject::ObjectCreated()
{
	// FV_GUARD;	
#ifdef FV_EDITOR_ENABLED
	if ( m_bObjectCreated )
		return;
	m_bObjectCreated = true;
#endif // FV_EDITOR_ENABLED

	for (ZoneItemList::iterator it = m_kItemList.begin();
		it != m_kItemList.end(); it++)
	{
		if ((*it) && (*it)->Zone())
			(*it)->ObjectCreated();			
	}
}

void FvVeryLargeObject::SetUID( FvString kUID ) 
{
	// FV_GUARD;
	m_kUID = kUID;
#ifdef FV_EDITOR_ENABLED
	std::transform( m_kUID.begin(), m_kUID.end(), m_kUID.begin(), tolower );
#endif // FV_EDITOR_ENABLED
	ms_kUniqueObjects[m_kUID] = this;

}

void FvVeryLargeObject::AddItem( FvZoneVLO *pkItem )
{
	// FV_GUARD;
	FvSimpleMutexHolder kHolder(ms_kItemListMutex);

	m_kItemList.push_back( pkItem );
	m_kItemList.sort( );
	m_kItemList.unique( );
#ifdef FV_EDITOR_ENABLED
	m_bListModified = true;
#endif
}

void FvVeryLargeObject::RemoveItem( FvZoneVLO *pkItem)
{
	// FV_GUARD;
	FvSimpleMutexHolder kHolder(ms_kItemListMutex);

	m_kItemList.remove( pkItem );

	if (m_kItemList.size() == 0)
	{
		ms_kUniqueObjects[m_kUID] = NULL;
	}
#ifdef FV_EDITOR_ENABLED
	m_bListModified = true;
#endif // FV_EDITOR_ENABLED
}
