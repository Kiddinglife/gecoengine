#include "FvDBEntityDefs.h"

//#include <FvEntityDescriptionDebug.h>
#include <FvEntityDefConstants.h>

static const FvString EMPTY_STR;

bool FvEntityDefs::Init(FvXMLSectionPtr pFatherEntitiesSection,
						FvXMLSectionPtr pEntitiesSection,
						const FvString& defaultTypeName, const FvString& defaultNameProperty )
{
	struct LocalHelperFunctions
	{
		void setNameProperty( FvString& output,
				const FvDataDescription& dataDesc,
				const FvEntityDescription& entityDesc )
		{
			const FvDataType* pDataType = dataDesc.DataType();
			if ((strcmp( pDataType->pMetaDataType()->Name(), "BLOB" ) == 0) ||
				(strcmp( pDataType->pMetaDataType()->Name(), "STRING" ) == 0))
			{
				output = dataDesc.Name();
			}
			else
			{
				FV_ERROR_MSG( "FvEntityDefs::Init: Identifier must be of "
						"STRING or BLOB type. Identifier '%s' "
						"for entity type '%s' is ignored\n",
						dataDesc.Name().c_str(),
						entityDesc.Name().c_str() );
			}
		}
	} helper;

	if (!m_kEntityDescriptionMap.Parse( pFatherEntitiesSection, pEntitiesSection ))
	{
		FV_ERROR_MSG( "FvEntityDefs::Init: Could not parse '%s'\n",
			FvEntityDef::Constants::EntitiesFile() );
		return false;
	}

	m_kDefaultNameProperty = defaultNameProperty;

	m_kNameProperties.resize( m_kEntityDescriptionMap.Size() );
	m_kEntityPasswordBits.resize( m_kEntityDescriptionMap.Size() );
	for ( FvEntityTypeID i = 0; i < m_kEntityDescriptionMap.Size(); ++i )
	{
		const FvEntityDescription& entityDesc =
				m_kEntityDescriptionMap.EntityDescription(i);
		m_kEntityPasswordBits[i] = (entityDesc.FindProperty( "password" ) != 0);

		const FvDataDescription *pDefaultNameDataDesc = NULL;
		FvString&			nameProperty = m_kNameProperties[i];
		for ( unsigned int i = 0; i < entityDesc.PropertyCount(); ++i)
		{
			const FvDataDescription* pDataDesc = entityDesc.Property( i );
			if (pDataDesc->IsIdentifier())
			{
				if (nameProperty.empty())
				{
					helper.setNameProperty( nameProperty, *pDataDesc,
							entityDesc );
				}
				else 
				{
					FV_ERROR_MSG( "FvEntityDefs::Init: Multiple identifiers for "
							"one entity type not supported. Identifier '%s' "
							"for entity type '%s' is ignored\n",
							pDataDesc->Name().c_str(),
							entityDesc.Name().c_str() );
				}
			}
			else if ( pDataDesc->Name() == defaultNameProperty )
			{	
				pDefaultNameDataDesc = pDataDesc;
			}
		}
		if (nameProperty.empty() && pDefaultNameDataDesc)
		{
			helper.setNameProperty( nameProperty, *pDefaultNameDataDesc,
					entityDesc );
		}
	}

	m_kEntityDescriptionMap.NameToIndex( defaultTypeName, m_uiDefaultTypeID );

	return true;
}

bool FvEntityDefs::XMLToMD5()
{
	//FvMD5 md5;
	//m_kEntityDescriptionMap.AddToMD5( md5 );
	//md5.GetDigest( m_kMD5Digest );
	//FvMD5 persistentPropertiesMD5;
	//m_kEntityDescriptionMap.AddPersistentPropertiesToMD5(
	//	persistentPropertiesMD5 );
	//persistentPropertiesMD5.GetDigest( m_kPersistentPropertiesMD5Digest );
	//! modify by Uman, 20101110, 从dll里生成xml
	{
		char acPath[256];
		GetModuleFileName(0, acPath, sizeof(acPath));
		//! \换/
		int sLen = strlen(acPath);
		int lastPos = -1;
		int lastlastPos = -1;
		int lastlastlastPos = -1;
		for(int i=0; i<sLen; ++i)
		{
			if(acPath[i] == '\\')
			{
				acPath[i] = '/';
				lastlastlastPos = lastlastPos;
				lastlastPos = lastPos;
				lastPos = i;
			}
		}
#ifdef FV_SHIPPING
		if(lastlastPos == -1)
		{
			FV_ERROR_MSG("Path[%s] Err\n", acPath);
			return false;
		}
		sprintf_s(acPath+lastlastPos+1, sizeof(acPath)-lastlastPos-1, "ServerData/EntityDef");
#else // FV_SHIPPING
		if(lastlastlastPos == -1)
		{
			FV_ERROR_MSG("Path[%s] Err\n", acPath);
			return false;
		}
		sprintf_s(acPath+lastlastlastPos+1, sizeof(acPath)-lastlastlastPos-1, "ServerData/EntityDef");
#endif // !FV_SHIPPING

		HINSTANCE hDll = LoadLibraryEx( "XMLToMD5.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
		if(!hDll)
		{
			LPVOID lpMsgBuf; 
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS, 
				NULL, 
				GetLastError(), 
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
				(LPTSTR) &lpMsgBuf, 
				0, 
				NULL 
				); 
			::MessageBox( NULL, (char*)lpMsgBuf, "", MB_OK | MB_ICONERROR | MB_TASKMODAL);
			LocalFree( lpMsgBuf );
			return false;
		}

		typedef bool(*PFN_XMLTOMD5)(char*, FvMD5::Digest&);
		PFN_XMLTOMD5 pFnFun = (PFN_XMLTOMD5)GetProcAddress(hDll, "XMLToMD5");
		if(pFnFun)
		{
			if(!pFnFun(acPath, m_kMD5Digest))
			{
				FV_ERROR_MSG("XMLToMD5 return false\n");
				FreeLibrary(hDll);
				return false;
			}
			else
			{
				FreeLibrary(hDll);
			}
		}
		else
		{
			FV_ERROR_MSG("Can't Find XMLToMD5 func in XMLToMD5.dll\n");
			FreeLibrary(hDll);
			return false;
		}
	}

	return true;
}

const FvString& FvEntityDefs::GetDefaultTypeName() const
{
	return (this->IsValidEntityType( this->GetDefaultType() )) ?
				this->GetEntityDescription( this->GetDefaultType() ).Name() :
				EMPTY_STR;
}

FvString FvEntityDefs::GetPropertyType( FvEntityTypeID typeID,
	const FvString& propName ) const
{
	const FvEntityDescription& entityDesc = this->GetEntityDescription(typeID);
	FvDataDescription* pDataDesc = entityDesc.FindProperty( propName );
	return ( pDataDesc ) ? pDataDesc->DataType()->TypeName() : FvString();
}

void FvEntityDefs::DebugDump( int detailLevel ) const
{
	//FvEntityDescriptionDebug::Dump( m_kEntityDescriptionMap, detailLevel );
}
