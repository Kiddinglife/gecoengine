#include "FvDataDescription.h"
#include "FvEntityDefConstants.h"
#include "FvEntityDescription.h"

#include <FvMD5.h>
#include <FvDebug.h>
#include <FvBase64.h>
#include <FvMemoryStream.h>
#include <FvNetTypes.h>
#include <FvXMLSection.h>
#include <OgreResourceGroupManager.h>


FV_DECLARE_DEBUG_COMPONENT2( "FvDataDescription", 0 )


FvDataType::SingletonMap * FvDataType::ms_pkSingletonMap;
FvDataType::Aliases FvDataType::ms_kAliases;
#ifdef FV_EDITOR_ENABLED
FvDataType::AliasWidgets FvDataType::ms_AliasWidgets;
#endif // FV_EDITOR_ENABLED
static bool s_bAliasesDone = false;

bool FvDataType::IsSameType( FvObjPtr pValue )
{
	FV_ASSERT(pValue);
	return pValue->DataType()==this;
}

FvXMLSectionPtr FvDataType::pDefaultSection() const
{
	FvXMLSectionPtr spDefaultSection = new FvXMLSection( "Default" );
	this->AddToSection( this->pDefaultValue(), spDefaultSection );
	return spDefaultSection;
}

FvDataTypePtr FvDataType::BuildDataType( FvXMLSectionPtr pSection )
{
	if (!pSection)
	{
		FV_WARNING_MSG( "FvDataType::BuildDataType: No <Type> section\n" );
		return NULL;
	}

	if (!s_bAliasesDone)
	{
	   	s_bAliasesDone = true;
		FvDataType::InitAliases();
	}

	FvString typeName = pSection->AsString();

	Aliases::iterator found = ms_kAliases.find( typeName );
	if (found != ms_kAliases.end())
	{
/**
		if ( pSection->OpenChild( "Default" ) )
		{
			FV_WARNING_MSG( "FvDataType::BuildDataType: New default value for "
					"aliased data type '%s' is ignored. The default value of an"
					" aliased data type can only be overridden by the "
					"default value of an entity property.\n",
					typeName.c_str() );
		}
**/
		return found->second;
	}

	FvMetaDataType * pMetaType = FvMetaDataType::Find( typeName );
	if (pMetaType == NULL)
	{
		FV_ERROR_MSG( "FvDataType::BuildDataType: "
			"Could not find FvMetaDataType '%s'\n", typeName.c_str() );
		return NULL;
	}

	FvDataTypePtr pDT = pMetaType->GetType( pSection );

	if (!pDT)
	{
		FV_ERROR_MSG( "FvDataType::BuildDataType: "
			"Could not build %s from spec given\n", typeName.c_str() );
		return NULL;
	}

	pDT->SetDefaultValue( pSection->OpenChild( "Default" ) );

	return FvDataType::FindOrAddType( pDT.GetObject() );
}

void FvDataType::DumpDataType( FvXMLSectionPtr pSection )
{
	if(ms_pkSingletonMap != NULL)
	{
		for(SingletonMap::iterator it = ms_pkSingletonMap->begin(); it != ms_pkSingletonMap->end(); ++it)
		{
			FvDataType* pType = it->m_pkInst;
			FvXMLSectionPtr pTypeSec = pSection->NewSection("Type");
			FvXMLSectionPtr pName = pTypeSec->NewSection("Name");
			pName->SetString(pType->TypeName());
			FvXMLSectionPtr pDef = pTypeSec->NewSection("Default");
			pType->AddToSection(pType->pDefaultValue(), pDef);
		}
	}
}

void FvDataType::DumpAliases( FvXMLSectionPtr pSection )
{
	if(!pSection) return;;

	Aliases::iterator itrB = ms_kAliases.begin();
	Aliases::iterator itrE = ms_kAliases.end();
	while(itrB != itrE)
	{
		FvXMLSectionPtr pTypeSec = pSection->NewSection(itrB->first);
		itrB->second->AddToSection(itrB->second->pDefaultValue(), pTypeSec);
		++itrB;
	}
}

FvDataTypePtr FvDataType::FindOrAddType( FvDataTypePtr pDT )
{
	if (ms_pkSingletonMap == NULL) ms_pkSingletonMap = new SingletonMap();

	SingletonMap::iterator found = ms_pkSingletonMap->find(
										SingletonPtr( pDT.GetObject() ) );

	if (found != ms_pkSingletonMap->end())
		return found->m_pkInst;

	ms_pkSingletonMap->insert( SingletonPtr( pDT.GetObject() ) );
	return pDT;
}

bool FvDataType::InitAliases()
{
	// Add internal aliases
	FvMetaDataType::AddAlias( "FLOAT32", "FLOAT" );
	FvMetaDataType::AddAlias( "FLOAT64", "DOUBLE" );

	FvXMLSectionPtr spFatherAliases = OpenFatherAliasesFile();
	FvXMLSectionPtr spChildAliases = OpenAliasesFile();

	FvXMLSectionPtr pAliases = spFatherAliases;
	if(pAliases)
	{
		FvXMLSectionIterator iter;
		for (iter = pAliases->Begin(); iter != pAliases->End(); ++iter)
		{
			if((*iter)->GetType() != TiXmlNode::ELEMENT)
				continue;
			if((*iter)->SectionName() == "ExtType")
				continue;

			FvDataTypePtr pAliasedType = FvDataType::BuildDataType( *iter );

			if (pAliasedType)
			{
				ms_kAliases.insert( std::make_pair(
					(*iter)->SectionName().c_str(), pAliasedType ) );

#ifdef FV_EDITOR_ENABLED
				ms_AliasWidgets.insert( std::make_pair(
					(*iter)->SectionName().c_str(),
					(*iter)->OpenChild( "Widget" ) ) );
#endif // FV_EDITOR_ENABLED
			}
			else
			{
				FV_ERROR_MSG( "FvDataType::InitAliases: Failed to Add %s\n",
					(*iter)->SectionName().c_str() );
			}
		}
	}

	pAliases = spChildAliases;
	if(pAliases)
	{
		FvXMLSectionIterator iter;
		for (iter = pAliases->Begin(); iter != pAliases->End(); ++iter)
		{
			if((*iter)->GetType() != TiXmlNode::ELEMENT)
				continue;
			if((*iter)->SectionName() == "ExtType")
				continue;

			FvDataTypePtr pAliasedType = FvDataType::BuildDataType( *iter );

			if (pAliasedType)
			{
				ms_kAliases.insert( std::make_pair(
					(*iter)->SectionName().c_str(), pAliasedType ) );

#ifdef FV_EDITOR_ENABLED
				ms_AliasWidgets.insert( std::make_pair(
					(*iter)->SectionName().c_str(),
					(*iter)->OpenChild( "Widget" ) ) );
#endif // FV_EDITOR_ENABLED
			}
			else
			{
				FV_ERROR_MSG( "FvDataType::InitAliases: Failed to Add %s\n",
					(*iter)->SectionName().c_str() );
			}
		}
	}
	else
	{
		FV_WARNING_MSG( "Couldn't open aliases file '%s'\n",
			FvEntityDef::Constants::AliasesFile() );
	}

	return true;
}

void FvDataType::ClearStaticsForReload()
{
	// only need to clear the singleton map for UserDataTypes
	FvDataType::SingletonMap * oldMap = ms_pkSingletonMap;
	ms_pkSingletonMap = NULL;	// set to NULL first for safety
	if(oldMap) delete oldMap;

	ms_kAliases.clear();
	s_bAliasesDone = false;

	FV_IF_NOT_ASSERT_DEV( ms_pkSingletonMap == NULL )
	{
		FV_EXIT( "something is really wrong (NULL is no longer NULL)" );
	}
}

////! ÒÆ³ý´ý¶¨
//void FvDataType::CallOnEach( void (FvDataType::*fn)() )
//{
//	if (ms_pkSingletonMap)
//	{
//		FvDataType::SingletonMap::iterator iter = ms_pkSingletonMap->begin();
//		const FvDataType::SingletonMap::iterator endIter = ms_pkSingletonMap->end();
//
//		while (iter != endIter)
//		{
//			(iter->m_pkInst->*fn)();
//
//			++iter;
//		}
//	}
//}
////! end

FvDataType::~FvDataType()
{
	if(ms_pkSingletonMap != NULL)
	{
		for(SingletonMap::iterator it = ms_pkSingletonMap->begin(); it != ms_pkSingletonMap->end(); ++it)
		{
			if(it->m_pkInst == this)
			{
				ms_pkSingletonMap->erase( it );
				break;
			}
		}
	}
}

////! ÒÆ³ý´ý¶¨
//FvObjPtr FvDataType::Attach( FvObjPtr pObject,
//	FvPropertyOwnerBase * pOwner, FvInt32 ownerRef )
//{
//	if (this->IsSameType( pObject ))
//		return pObject;
//
//	return NULL;
//}
//
//void FvDataType::Detach( FvObjPtr pObject )
//{
//}
//
//FvPropertyOwnerBase * FvDataType::AsOwner( FvObjPtr pObject )
//{
//	return NULL;
//}
////! end


FvMetaDataType::FvMetaDataTypesByName* FvMetaDataType::ms_pkMetaDataTypesByName = NULL;
FvMetaDataType::FvMetaDataTypesByID* FvMetaDataType::ms_pkMetaDataTypesByID = NULL;

void FvMetaDataType::AddAlias( const FvString& orig, const FvString& alias )
{
	FvMetaDataType * pMetaDataType = FvMetaDataType::Find( orig );
	FV_IF_NOT_ASSERT_DEV( pMetaDataType )
	{
		return;
	}
	(*ms_pkMetaDataTypesByName)[ alias ] = pMetaDataType;
}

void FvMetaDataType::AddMetaType( FvMetaDataType * pMetaType )
{
	if (ms_pkMetaDataTypesByName == NULL)
		ms_pkMetaDataTypesByName = new FvMetaDataTypesByName();
	if (ms_pkMetaDataTypesByID == NULL)
		ms_pkMetaDataTypesByID = new FvMetaDataTypesByID();

	const char * name = pMetaType->Name();

	if (ms_pkMetaDataTypesByName->find( name ) != ms_pkMetaDataTypesByName->end())
	{
		FV_CRITICAL_MSG( "FvMetaDataType::addType: "
			"%s has already been registered.\n", name );
		return;
	}

	(*ms_pkMetaDataTypesByName)[ name ] = pMetaType;
	(*ms_pkMetaDataTypesByID)[ pMetaType->ID() ] = pMetaType;
}

void FvMetaDataType::DelMetaType( FvMetaDataType * pMetaType )
{
}

FvMetaDataType* FvMetaDataType::Find( const FvString & name )
{
	FvMetaDataTypesByName::iterator found = ms_pkMetaDataTypesByName->find( name );
	if (found != ms_pkMetaDataTypesByName->end()) return found->second;
	return NULL;
}

FvMetaDataType* FvMetaDataType::Find( FvUInt32 id )
{
	FvMetaDataTypesByID::iterator found = ms_pkMetaDataTypesByID->find( id );
	if (found != ms_pkMetaDataTypesByID->end()) return found->second;
	return NULL;
}

void FvMetaDataType::Fini()
{
	if(ms_pkMetaDataTypesByName)
	{
		delete ms_pkMetaDataTypesByName;
		ms_pkMetaDataTypesByName = NULL;
	}

	if(ms_pkMetaDataTypesByID)
	{
		delete ms_pkMetaDataTypesByID;
		ms_pkMetaDataTypesByID = NULL;
	}
}


FvDataDescription::FvDataDescription()
:FvMemberDescription()
,m_uiDataFlags( 0 )
,m_iIndex( -1 )
,m_iLocalIndex( -1 )
,m_iEventStampIndex( -1 )
,m_iClientServerFullIndex( -1 )
,m_iDetailLevel( FvDataLoDLevel::NO_LEVEL )
,m_iDatabaseLength( FV_DEFAULT_DATABASE_LENGTH )
#ifdef FV_EDITOR_ENABLED
,m_bEditable( false )
#endif
{
}

namespace
{
struct EntityDataFlagMapping
{
	const char *	m_pName;
	FvUInt32		m_uiFlags;
	const char *	m_pNewName;
};

EntityDataFlagMapping s_EntityDataFlagMappings[] =
{
	{ "CELL_PRIVATE",		0,														NULL },
	{ "CELL_PUBLIC",		FV_DATA_GHOSTED_OLD,										NULL },
	{ "OTHER_CLIENTS",		FV_DATA_GHOSTED_OLD|FV_DATA_OTHER_CLIENT_OLD,					NULL },
	{ "OWN_CLIENT",			FV_DATA_OWN_CLIENT_OLD,										NULL },
	{ "BASE",				FV_DATA_BASE_OLD,											NULL },
	{ "BASE_AND_CLIENT",	FV_DATA_OWN_CLIENT_OLD|FV_DATA_BASE_OLD,						NULL },
	{ "CELL_PUBLIC_AND_OWN",FV_DATA_GHOSTED_OLD|FV_DATA_OWN_CLIENT_OLD,						NULL },
	{ "ALL_CLIENTS",		FV_DATA_GHOSTED_OLD|FV_DATA_OTHER_CLIENT_OLD|FV_DATA_OWN_CLIENT_OLD,NULL },
	{ "EDITOR_ONLY",		FV_DATA_EDITOR_ONLY_OLD,									NULL },

	{ "PRIVATE",			0,														"CELL_PRIVATE" },
	{ "CELL",				FV_DATA_GHOSTED_OLD,										"CELL_PUBLIC" },
	{ "GHOSTED",			FV_DATA_GHOSTED_OLD,										"CELL_PUBLIC" },
	{ "OTHER_CLIENT",		FV_DATA_GHOSTED_OLD|FV_DATA_OTHER_CLIENT_OLD,					"OTHER_CLIENTS" },
	{ "GHOSTED_AND_OWN",	FV_DATA_GHOSTED_OLD|FV_DATA_OWN_CLIENT_OLD,						"CELL_PUBLIC_AND_OWN" },
	{ "CELL_AND_OWN",		FV_DATA_GHOSTED_OLD|FV_DATA_OWN_CLIENT_OLD,						"CELL_PUBLIC_AND_OWN" },
	{ "ALL_CLIENT",			FV_DATA_GHOSTED_OLD|FV_DATA_OTHER_CLIENT_OLD|FV_DATA_OWN_CLIENT_OLD,"ALL_CLIENTS" },
};

bool SetEntityDataFlags( const FvString & name, FvUInt32 & flags,
		const FvString & parentName, const FvString & propName )
{
	const FvInt32 size = sizeof( s_EntityDataFlagMappings )/
		sizeof( s_EntityDataFlagMappings[0] );

	for (FvInt32 i = 0; i < size; ++i)
	{
		const EntityDataFlagMapping & mapping = s_EntityDataFlagMappings[i];

		if (name == mapping.m_pName)
		{
			flags = mapping.m_uiFlags;

			if (mapping.m_pNewName)
			{
				FV_WARNING_MSG( "FvDataDescription::Parse: "
						"Using old Flags option - %s instead of %s for %s.%s\n",
					mapping.m_pName, mapping.m_pNewName,
					parentName.c_str(), propName.c_str() );
			}

			return true;
		}
	}

	return false;
};

const char * GetEntityDataFlagStr( FvInt32 flags )
{
	const FvInt32 size = sizeof( s_EntityDataFlagMappings )/
		sizeof( s_EntityDataFlagMappings[0] );

	for (FvInt32 i = 0; i < size; ++i)
	{
		const EntityDataFlagMapping & mapping = s_EntityDataFlagMappings[i];

		if (flags == mapping.m_uiFlags)
			return mapping.m_pName;
	}

	return NULL;
}

}

const char* FvDataDescription::GetDataFlagsAsStr() const
{
	return GetEntityDataFlagStr( m_uiDataFlags & FV_DATA_DISTRIBUTION_FLAGS );
}


bool FvDataDescription::Parse( FvXMLSectionPtr pSection,
		const FvString & parentName,
		PARSE_OPTIONS options /*= PARSE_DEFAULT*/  )
{
	m_kName = pSection->SectionName();

	FvXMLSectionPtr typeSection = pSection->OpenSection( "Type" );

	m_spDataType = FvDataType::BuildDataType( typeSection );

	if (!m_spDataType)
	{
		FV_ERROR_MSG( "FvDataDescription::Parse: "
					"Unable to Find data type '%s' for %s.%s\n",
				pSection->ReadString( "Type" ).c_str(),
				parentName.c_str(),
				m_kName.c_str() );

		return false;
	}

#ifdef FV_EDITOR_ENABLED
	// try to Get the default Widget, if it's an alias and has one that is
	Widget( FvDataType::FindAliasWidget( typeSection->AsString() ) );
#endif // FV_EDITOR_ENABLED

	if ( options & PARSE_IGNORE_FLAGS )
	{
		m_uiDataFlags = 0;
	}
	else
	{
		if (!SetEntityDataFlags( pSection->ReadString( "Flags" ), m_uiDataFlags,
					parentName, m_kName ))
		{
			FV_ERROR_MSG( "FvDataDescription::Parse: Invalid Flags section '%s' for %s\n",
					pSection->ReadString( "Flags" ).c_str(), m_kName.c_str() );
			return false;
		}
	}

	if (pSection->OpenSection("Persistent"))
	{
		m_uiDataFlags |= FV_DATA_PERSISTENT_OLD;
	}

	if (pSection->OpenSection("Identifier"))
	{
		m_uiDataFlags |= FV_DATA_ID_OLD;
	}

	FV_ASSERT_DEV( !this->IsBaseData() ||
			(!this->IsGhostedData() && !this->IsOtherClientData()) );

	if (this->IsClientOnlyData())
	{
		FV_WARNING_MSG( "FvDataDescription::Parse(type %s): "
			"ClientOnlyData not yet supported.\n",
			pSection->AsString().c_str() );
	}

	FvXMLSectionPtr pSubSection = pSection->OpenChild( "Default" );

	if (pSubSection)
		m_spInitialValue = m_spDataType->CreateFromSection( pSubSection );
	else
		m_spInitialValue = m_spDataType->pDefaultValue();

#ifdef FV_EDITOR_ENABLED
	m_bEditable = pSection->ReadBool( "Editable", false );
	// The editor always pre loads the default value, so it won't try to make
	// it in the loading thread, which causes issues
	else if (Editable())
	{
		m_spInitialValue = m_spDataType->pDefaultValue();
//		Py_XDECREF( m_spkInitialValue.GetObject() );
	}

//	FV_ASSERT( m_spkInitialValue );
#endif

	m_iDatabaseLength = pSection->ReadInt( "DatabaseLength", m_iDatabaseLength );

	return true;
}

FvDataDescription::FvDataDescription(const FvDataDescription & description)
{
	(*this) = description;
}

FvDataDescription::~FvDataDescription()
{
}

bool FvDataDescription::IsCorrectType( FvObjPtr pNewValue )
{
	return m_spDataType ? m_spDataType->IsSameType( pNewValue ) : false;
}

void FvDataDescription::AddToMD5( FvMD5 & md5 ) const
{
	md5.Append( m_kName.c_str(), m_kName.size() );
	FvInt32 md5DataFlags = m_uiDataFlags & FV_DATA_DISTRIBUTION_FLAGS;
	md5.Append( &md5DataFlags, sizeof(md5DataFlags) );
	m_spDataType->AddToMD5( md5 );
}

FvXMLSectionPtr FvDataDescription::pDefaultSection() const
{
	FvXMLSectionPtr pDefaultSection = new FvXMLSection( "Default" );
	m_spDataType->AddToSection( m_spInitialValue.GetObject(), pDefaultSection );
	return pDefaultSection;
}

void FvDataDescription::Dump(FvXMLSectionPtr pSection)
{
	if(!pSection) return;

	FvXMLSectionPtr pChild, pChild1;
	pChild = pSection->NewSection(m_kName);
	if(pChild)
	{
		pChild1 = pChild->NewSection("Type");
		pChild1->SetString(m_spDataType->TypeName());
		pChild1 = pChild->NewSection("Flags");
		pChild1->SetString(GetDataFlagsAsStr());
		pChild1 = pChild->NewSection("DetailLevel");
		pChild1->SetInt(m_iDetailLevel);
		pChild1 = pChild->NewSection("DatabaseLength");
		pChild1->SetInt(m_iDatabaseLength);
	}
}

#ifdef FV_EDITOR_ENABLED

void FvDataDescription::Widget( FvXMLSectionPtr pSection )
{
	m_spWidgetSection = pSection;
}

FvXMLSectionPtr FvDataDescription::Widget()
{
	return m_spWidgetSection;
}
#endif // FV_EDITOR_ENABLED


#if FV_ENABLE_WATCHERS

FvWatcherPtr FvDataDescription::pWatcher()
{
	static FvWatcherPtr watchMe = NULL;
	
	if (!watchMe)
	{
		watchMe = new FvDirectoryWatcher();
		FvDataDescription * pNull = NULL;

		watchMe->AddChild( "type", new FvSmartPointerDereferenceWatcher( 
				new FvMemberWatcher<FvString,FvDataType>(
				*static_cast< FvDataType *> ( NULL ),
				&FvDataType::TypeName,
				static_cast< void (FvDataType::*)( FvString ) >( NULL ) )), 
				&pNull->m_spDataType);

		watchMe->AddChild( "Name", 
						   MakeWatcher( pNull->m_kName ));
		watchMe->AddChild( "localIndex", 
						   MakeWatcher( pNull->m_iLocalIndex ));
		watchMe->AddChild( "clientServerFullIndex", 
						   MakeWatcher( pNull->m_iClientServerFullIndex ));
		watchMe->AddChild( "Index",
						   new FvMemberWatcher<FvInt32,FvDataDescription>
						   ( *static_cast< FvDataDescription *> ( NULL ),
							 &FvDataDescription::Index,
							 static_cast< void (FvDataDescription::*)
							 ( FvInt32 ) >( NULL ) ));
		watchMe->AddChild( "stats", FvMemberDescription::pWatcher());
	}
	return watchMe;
}

#endif


extern FvInt32 DATA_TYPES_TOKEN;
FvInt32* pDATA_TYPES_TOKEN = &DATA_TYPES_TOKEN;
