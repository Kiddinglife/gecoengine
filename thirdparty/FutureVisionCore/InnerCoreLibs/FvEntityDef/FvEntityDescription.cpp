#include "FvEntityDescription.h"
#include "FvEntityDefConstants.h"

#include <FvMD5.h>
#include <FvDebug.h>
#include <FvBinaryStream.h>
#include <FvXMLSection.h>
#include <OgreResourceGroupManager.h>

#include <float.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvDataDescription", 0 )


FvVolatileInfo::FvVolatileInfo()
:m_fPositionPriority( -1.f )
,m_fYawPriority( -1.f )
,m_fPitchPriority( -1.f )
,m_fRollPriority( -1.f )
{
}

bool FvVolatileInfo::Parse( FvXMLSectionPtr pSection )
{
	if (!pSection)
	{
		return true;
	}

	this->m_fPositionPriority	= this->AsPriority( pSection->OpenSection( "position" ) );
	this->m_fYawPriority		= this->AsPriority( pSection->OpenSection( "yaw" ) );
	this->m_fPitchPriority		= this->AsPriority( pSection->OpenSection( "pitch" ) );
	this->m_fRollPriority		= this->AsPriority( pSection->OpenSection( "roll" ) );

	return this->IsValid();
}

float FvVolatileInfo::AsPriority( FvXMLSectionPtr pSection ) const
{
	if (pSection)
	{
		float value = pSection->AsFloat( -1.f );
		return (value == -1.f) ? VOLATILE_ALWAYS : value * value;
	}

	return -1.f;
}

bool FvVolatileInfo::IsLessVolatileThan( const FvVolatileInfo & info ) const
{
	return
		this->m_fPositionPriority < info.m_fPositionPriority ||
		this->m_fYawPriority < info.m_fYawPriority ||
		this->m_fPitchPriority < info.m_fPitchPriority ||
		this->m_fRollPriority < info.m_fRollPriority;
}

bool FvVolatileInfo::IsValid() const
{
	return m_fYawPriority >= m_fPitchPriority &&
		m_fPitchPriority >= m_fRollPriority;
}


FvEntityDescription::FvEntityDescription()
:FvBaseUserDataObjectDescription()
,m_uiIndex( FV_INVALID_ENTITY_TYPE_ID )
,m_uiClientIndex( FV_INVALID_ENTITY_TYPE_ID )
,m_bHasCellScript( false )
,m_bHasBaseScript( false )
,m_bHasClientScript( false )
,m_bHasGlobalScript( false )
,m_uiNumEventStampedProperties( 0 )
,m_bHasClientProp(false)
,m_bHasBaseProp(false)
,m_bHasCellProp(false)
{
}

FvEntityDescription::~FvEntityDescription()
{
}


bool FvEntityDescription::Parse( const FvString & name, FvXMLSectionPtr pSection, bool isFinal )
{
	if (!pSection)
	{
		FvString kFilename = this->GetDefsDir() + "/" + name + ".def";
		pSection = OpenXMLInDef(this->GetShortDefsDir(), name + ".def");

		if (!pSection)
		{
			FV_ERROR_MSG( "FvEntityDescription::Parse: Could not open %s\n",
					kFilename.c_str() );
			return false;
		}
	}

	FvString parentName = pSection->ReadString( "Parent" );

	if (!parentName.empty())
	{
		if (!this->Parse( parentName, NULL, false ))
		{
			FV_ERROR_MSG( "FvEntityDescription::Parse: "
						"Could not Parse %s, parent of %s\n",
					parentName.c_str(), name.c_str() );
			return false;
		}
	}

	m_kName = name;
/**
	m_kClientName = pSection->ReadString( "ClientName", m_kClientName );

	if (m_kClientName.empty() && isFinal)
		m_kClientName = m_kName;

	m_kVolatileInfo.Parse( pSection->OpenSection( "Volatile" ) );
**/

	bool result = this->ParseInterface( pSection, m_kName.c_str() );

	m_bHasCellScript = (m_kCell.Size() || m_bHasCellProp) ? true : false;
	m_bHasBaseScript = (m_kBase.Size() || m_bHasBaseProp) ? true : false;
	m_bHasGlobalScript = m_kGlobal.Size() ? true : false;
	m_bHasClientScript = (m_kClient.Size() || m_bHasClientProp) ? true : false;

/**
#if defined( FV_SERVER ) || defined( FV_EDITOR_ENABLED )

	m_bHasCellScript = m_kCell.Size() ? true : false;
	m_bHasBaseScript = m_kBase.Size() ? true : false;
	m_bHasGlobalScript = m_kGlobal.Size() ? true : false;

#else // neither the FV_SERVER nor the Tools

	m_bHasCellScript = m_kCell.Size() ? true : false;
	m_bHasBaseScript = m_kBase.Size() ? true : false;
	m_bHasGlobalScript = m_kGlobal.Size() ? true : false;

#endif // FV_SERVER || FV_EDITOR_ENABLED

	m_bHasClientScript = m_kClient.Size() ? true : false;

	if (!m_bHasClientScript && isFinal)
		m_kClientName = FvString();

#ifdef FV_SERVER
	if (isFinal)
	{
		FvInt32 levels[ FV_MAX_DATA_LOD_LEVELS + 1 ];

		for (FvInt32 i = 0; i < FV_MAX_DATA_LOD_LEVELS + 1; ++i)
		{
			levels[m_kLodLevels.GetLevel( i ).Index()] = i;
		}

		for (FvUInt32 i = 0; i < this->PropertyCount(); ++i)
		{
			FvDataDescription * pDD = this->Property( i );

			switch (pDD->DetailLevel())
			{
				case FvDataLoDLevel::NO_LEVEL:
					break;

				case FvDataLoDLevel::OUTER_LEVEL:
					pDD->DetailLevel( m_kLodLevels.Size() - 1 );
					break;

				default:
					pDD->DetailLevel( levels[ pDD->DetailLevel() ] );
					break;
			}
		}
	}

	if(!IsClientOnlyType())
	{
		if(HasGlobalScript())
		{
			if(PropertyCount() > 0)
			{
				FV_ERROR_MSG("Entity '%s' have global script but has Properties.\n", m_kName.c_str());
				result = false;
			}
			if(HasBaseScript())
			{
				FV_ERROR_MSG("Entity '%s' have global script but has base script.\n", m_kName.c_str());
				result = false;
			}
			if(HasCellScript())
			{
				FV_ERROR_MSG("Entity '%s' have global script but has cell script.\n", m_kName.c_str());
				result = false;
			}
			if(HasClientScript())
			{
				FV_ERROR_MSG("Entity '%s' have global script but has client script.\n", m_kName.c_str());
				result = false;
			}
		}
		else
		{
			if(!HasBaseScript())
			{
				for(Properties::const_iterator i = m_kProperties.begin();
					i != m_kProperties.end(); ++i)
				{
					if(i->IsBaseData())
					{
						FV_ERROR_MSG( "Entity '%s' does not have a base script but "
							"has base Property '%s'.\n", m_kName.c_str(),
							i->Name().c_str() );
						result = false;
						break;
					}
				}
			}

			if(!HasCellScript())
			{
				for(Properties::const_iterator i = m_kProperties.begin();
					i != m_kProperties.end(); ++i)
				{
					if(i->IsCellData())
					{
						FV_ERROR_MSG( "Entity '%s' does not have a cell script but "
							"has cell Property '%s'.\n", m_kName.c_str(),
							i->Name().c_str() );
						result = false;
						break;
					}
				}
			}
		}
	}
#endif
**/
	return result;
}

bool FvEntityDescription::ParseInterface( FvXMLSectionPtr pSection, const char * interfaceName )
{
	if (!pSection)
	{
		return false;
	}

	bool result =
/**
#ifdef FV_SERVER
		m_kLodLevels.AddLevels( pSection->OpenSection( "LoDLevels" ) ) &&
#endif
**/
		this->FvBaseUserDataObjectDescription::ParseInterface( pSection, 
			interfaceName ) &&

		this->ParseClientMethods( pSection->OpenSection( "ClientMethods" ),
			interfaceName ) &&
		this->ParseCellMethods( pSection->OpenSection( "CellMethods" ),
			interfaceName ) &&
		this->ParseBaseMethods( pSection->OpenSection( "BaseMethods" ),
			interfaceName ) &&
		this->ParseGlobalMethods( pSection->OpenSection( "GlobalMethods" ),
			interfaceName );

	return result;
}

bool FvEntityDescription::ParseImplements( FvXMLSectionPtr pInterfaces )
{
	bool result = true;

	if (pInterfaces)
	{
		FvXMLSectionIterator iter = pInterfaces->Begin();

		while (iter != pInterfaces->End())
		{
			if((*iter)->GetType() != TiXmlNode::ELEMENT)
			{
				++iter;
				continue;
			}

			FvString interfaceName = (*iter)->AsString();

			FvXMLSectionPtr pInterface = OpenXMLInDef(this->GetShortDefsDir() + "/interfaces", interfaceName + ".def");

			if (!this->ParseInterface( pInterface, interfaceName.c_str()  ))
			{
				FV_ERROR_MSG( "FvEntityDescription::ParseImplements: "
					"Failed parsing interface %s\n", interfaceName.c_str() );
				result = false;
			}

			++iter;
		}
	}

	return result;
}

bool FvEntityDescription::ParseProperties( FvXMLSectionPtr pProperties )
{
	if (pProperties)
	{
		for (FvXMLSectionIterator iter = pProperties->Begin();
				iter != pProperties->End();
				++iter)
		{
			if((*iter)->GetType() != TiXmlNode::ELEMENT)
				continue;

			FvDataDescription dataDescription;

			if (!dataDescription.Parse( *iter, m_kName ))
			{
				FV_WARNING_MSG( "Error parsing properties for %s\n",
						m_kName.c_str() );
				return false;
			}

#ifndef FV_EDITOR_ENABLED
			if (dataDescription.IsEditorOnly())
			{
				continue;
			}
#endif

			if(dataDescription.IsClientServerData())
				m_bHasClientProp = true;
			if(dataDescription.IsBaseData())
				m_bHasBaseProp = true;
			if(dataDescription.IsCellData())
				m_bHasCellProp = true;

			FvInt32 index = m_kProperties.size();
			FvInt32 clientServerIndex = -1;

			PropertyMap::const_iterator propIter =
					m_kPropertyMap.find( dataDescription.Name() );
			if (propIter != m_kPropertyMap.end())
			{
				FV_INFO_MSG( "FvEntityDescription::ParseProperties: "
						"Property %s.%s is being overridden.\n",
					m_kName.c_str(), dataDescription.Name().c_str() );
				index = propIter->second;
				if (dataDescription.IsClientServerData())
				{
					clientServerIndex = 
						m_kProperties[ index ].ClientServerFullIndex();
				}
			}

			dataDescription.Index( index );
			m_kPropertyMap[dataDescription.Name()] = dataDescription.Index();

#ifdef FV_EDITOR_ENABLED
			FvXMLSectionPtr widget = (*iter)->OpenSection( "Widget" );
			if ( !!widget )
			{
				dataDescription.Widget( widget );
			}
#endif

			if (dataDescription.IsClientServerData())
			{
				if (clientServerIndex != -1)
				{
					dataDescription.ClientServerFullIndex( clientServerIndex );
					m_kClientServerProperties[ clientServerIndex ] =
						dataDescription.Index();
				}
				else
				{
					dataDescription.ClientServerFullIndex(
									m_kClientServerProperties.size() );
					m_kClientServerProperties.push_back(
									dataDescription.Index() );
					FV_ASSERT(m_kClientServerProperties.size() < 0xFFFF);
				}
			}
/**
#ifdef FV_SERVER
			if (dataDescription.IsOtherClientData())
			{
				FvInt32 detailLevel;

				if (m_kLodLevels.FindLevel( detailLevel,
							(*iter)->OpenSection( "DetailLevel" ) ))
				{
					dataDescription.DetailLevel( detailLevel );
				}
				else
				{
					FV_ERROR_MSG( "FvEntityDescription::ParseProperties: "
											"Invalid detail level for %s.\n",
									dataDescription.Name().c_str() );

					return false;
				}

				dataDescription.EventStampIndex( m_uiNumEventStampedProperties );
				++m_uiNumEventStampedProperties;
			}
#endif
**/
			if (index == FvInt32(m_kProperties.size()))
			{
				m_kProperties.push_back( dataDescription );
			}
			else
			{
				m_kProperties[index] = dataDescription;
			}
		}
	}

	return true;
}


bool FvEntityDescription::ParseClientMethods( FvXMLSectionPtr pMethods,
		const char * interfaceName )
{
	if (!pMethods) return true;

	return m_kClient.Init( pMethods, FvMethodDescription::CLIENT, interfaceName );
}

bool FvEntityDescription::ParseCellMethods( FvXMLSectionPtr pMethods,
		const char * interfaceName )
{
	if (!pMethods) return true;

	return m_kCell.Init( pMethods, FvMethodDescription::CELL, interfaceName );
}

bool FvEntityDescription::ParseBaseMethods( FvXMLSectionPtr pMethods,
		const char * interfaceName )
{
	if (!pMethods) return true;

	return m_kBase.Init( pMethods, FvMethodDescription::BASE, interfaceName );
}

bool FvEntityDescription::ParseGlobalMethods( FvXMLSectionPtr pMethods,
										   const char * interfaceName )
{
	if (!pMethods) return true;

	return m_kGlobal.Init( pMethods, FvMethodDescription::GLOBAL, interfaceName );
}

void FvEntityDescription::Supersede( FvMethodDescription::eComponent component )
{
	if (component == FvMethodDescription::BASE)
	{
		m_kBase.Supersede();
	}
	else if (component == FvMethodDescription::CELL)
	{
		m_kCell.Supersede();
	}
	else
	{
		FV_WARNING_MSG("only baseApp and cellApp can call Supersede method. Ignored\n");
	}
}

const FvString FvEntityDescription::GetDefsDir() const
{
	return FvEntityDef::Constants::EntitiesDefsPath();
}

const FvString FvEntityDescription::GetShortDefsDir() const
{
	return FvEntityDef::Constants::EntitiesDefsShortPath();
}

const FvString FvEntityDescription::GetClientDir() const
{
	return FvEntityDef::Constants::EntitiesClientPath();
}

const FvString FvEntityDescription::GetCellDir() const
{
	return FvEntityDef::Constants::EntitiesCellPath();
}

const FvString FvEntityDescription::GetBaseDir() const
{
	return FvEntityDef::Constants::EntitiesBasePath();
}

inline bool FvEntityDescription::ShouldConsiderData( FvInt32 pass,
		const FvDataDescription * pDD, FvInt32 dataDomains )
{
	//! add by Uman, 20101120, 接口废弃, 因为属性的遍历顺序已改变
	FV_ASSERT(0 && "Code is abandoned!");

	const bool PASS_FILTER[4][2] =
	{
		{ true,  false },	// Base and not client
		{ true,  true },	// Base and client
		{ false, true },	// Cell and client
		{ false, false },	// Cell and not client
	};

	return !pDD->IsClientOnlyData() &&
			(PASS_FILTER[ pass ][0] == pDD->IsBaseData()) &&
			(PASS_FILTER[ pass ][1] == pDD->IsClientServerData()) &&
			(pDD->IsOtherClientData() || !(dataDomains & FV_ONLY_OTHER_CLIENT_DATA)) &&
			(pDD->IsPersistent() || !(dataDomains & FV_ONLY_PERSISTENT_DATA));
}

inline bool FvEntityDescription::ShouldSkipPass( FvInt32 pass, FvInt32 dataDomains )
{
	//! add by Uman, 20101120, 接口废弃, 因为属性的遍历顺序已改变
	FV_ASSERT(0 && "Code is abandoned!");

	const FvInt32 PASS_JUMPER[4] =
	{
		FV_EXACT_MATCH | FV_BASE_DATA,					// done in pass 0
		FV_EXACT_MATCH | FV_BASE_DATA | FV_CLIENT_DATA,	// done in pass 1
		FV_EXACT_MATCH | FV_CELL_DATA | FV_CLIENT_DATA,	// done in pass 2
		FV_EXACT_MATCH | FV_CELL_DATA					// done in pass 3
	};

	if (dataDomains & FV_EXACT_MATCH)
	{
		return (dataDomains != PASS_JUMPER[ pass ]);
	}
	else
	{
		return ((dataDomains & PASS_JUMPER[ pass ]) == 0);
	}
}


static FvInt32 NUM_PASSES = 4;

class FvAddToStreamVisitor
{
public:
	virtual ~FvAddToStreamVisitor() {};

	virtual FvObjPtr GetData( FvDataDescription & /*dataDesc*/ ) const
	{
		FV_EXIT( "GetData not implemented or invalid call to GetData" );
		return NULL;
	}

	virtual bool AddToStream( FvDataDescription & dataDesc,
			FvBinaryOStream & stream, bool isPersistentOnly ) const
	{
		bool result = true;
		FvObjPtr pValue = this->GetData( dataDesc );

		if (!pValue)
		{
			pValue = dataDesc.pInitialValue();
			result = !this->IsErrorOnNULL();
		}

		if (!dataDesc.IsCorrectType( pValue ))
		{
			FV_ERROR_MSG( "FvEntityDescription::AddToStream: "
				"data for %s is wrong type\n", dataDesc.Name().c_str() );
			pValue = dataDesc.pInitialValue();
			result = false;
		}

		dataDesc.AddToStream( pValue, stream, isPersistentOnly );

		return result;
	}

	virtual bool IsErrorOnNULL() const
	{
		return true;
	}
};


class FvAddToStreamSectionVisitor : public FvAddToStreamVisitor
{
public:
	FvAddToStreamSectionVisitor( FvXMLSectionPtr pSection ) : m_spSection( pSection )
	{}

protected:
	virtual bool AddToStream( FvDataDescription & dataDesc,
			FvBinaryOStream & stream, bool isPersistentOnly ) const
	{
		FvXMLSectionPtr pCurr = m_spSection->OpenSection( dataDesc.Name() );

		FvObjPtr pObj;
		if(pCurr) pObj = dataDesc.CreateFromSection(pCurr);
		if(!pObj) pObj = dataDesc.pInitialValue();
		dataDesc.AddToStream(pObj ,stream, isPersistentOnly);
		return true;
	}

private:
	FvXMLSectionPtr m_spSection;
};


class FvAddToStreamDictionaryVisitor : public FvAddToStreamVisitor
{
public:
	FvAddToStreamDictionaryVisitor( const FvEntityAttributes* pDict )
	:m_pkDict(pDict)
	{
	}

protected:
	FvObjPtr GetData( FvDataDescription & dataDesc ) const
	{
		return m_pkDict->GetAttribut(dataDesc.Name());
	}

	virtual bool IsErrorOnNULL() const	{ return false; }

private:
	const FvEntityAttributes*	m_pkDict;
};


class FvAddToStreamAttributeVisitor : public FvAddToStreamVisitor
{
public:
	FvAddToStreamAttributeVisitor( const FvEntityAttributes* pDict )
	:m_pkDict(pDict)
	{
	}

protected:
	FvObjPtr GetData( FvDataDescription & dataDesc ) const
	{
		return m_pkDict->GetAttribut(dataDesc.Name());
	}

private:
	const FvEntityAttributes* m_pkDict;
};


bool FvEntityDescription::AddSectionToStream( FvXMLSectionPtr pSection,
		FvBinaryOStream & stream,
		FvInt32 dataDomains ) const
{
	FvAddToStreamSectionVisitor visitor( pSection );

	return this->AddToStream( visitor, stream, dataDomains );
}


bool FvEntityDescription::AddSectionToDictionary( FvXMLSectionPtr pSection,
			FvEntityAttributes* pDict,
			FvInt32 dataDomains ) const
{
	class FvAddToDictSectionVisitor : public FvIDataDescriptionVisitor
	{
	public:
		FvAddToDictSectionVisitor( FvXMLSectionPtr pSection, FvEntityAttributes* pDict )
		:m_spSection( pSection )
		,m_pkDict(pDict)
		{
		}

		virtual bool Visit( const FvDataDescription& propDesc )
		{
			FvXMLSectionPtr pValueSection = m_spSection->OpenChild( propDesc.Name() );

			if (pValueSection)
			{
				FvObjPtr pValue = propDesc.CreateFromSection( pValueSection );

				if (pValue)
				{
					m_pkDict->SetAttribut(propDesc.Name(), pValue);
				}
				else
				{
					FV_WARNING_MSG( "FvEntityDescription::AddSectionToDictionary: "
							"Could not add %s\n", propDesc.Name().c_str() );
				}
			}

			return true;
		}

	private:
		FvXMLSectionPtr		m_spSection;
		FvEntityAttributes*	m_pkDict;
	};

	FvAddToDictSectionVisitor visitor( pSection, pDict );
	this->Visit( dataDomains, visitor );

	return true;
}


bool FvEntityDescription::AddDictionaryToStream( const FvEntityAttributes* pDict,
		FvBinaryOStream & stream,
		FvInt32 dataDomains ) const
{
	FvAddToStreamDictionaryVisitor visitor( pDict );

	return this->AddToStream( visitor, stream, dataDomains );
}

bool FvEntityDescription::AddAttributesToStream( const FvEntityAttributes* pDict,
		FvBinaryOStream & stream,
		FvInt32 dataDomains,
		FvInt32 * pDataSizes,
		FvInt32 numDataSizes ) const
{
	if (pDict == NULL)
	{
		FV_ERROR_MSG( "FvEntityDescription::AddAttributesToStream: "
				"pObject is NULL\n" );
		return false;
	}

	FvAddToStreamAttributeVisitor visitor( pDict );

	return this->AddToStream( visitor, stream, dataDomains,
			pDataSizes, numDataSizes );
}

bool FvEntityDescription::AddToStream( const FvAddToStreamVisitor & visitor,
		FvBinaryOStream & stream,
		FvInt32 dataDomains,
		FvInt32 * pDataSizes,
		FvInt32 numDataSizes ) const
{
	FvInt32 actualPass = 0;

	for (FvInt32 pass = 0; pass < NUM_PASSES; pass++)
	{
		if (!FvEntityDescription::ShouldSkipPass( pass, dataDomains ))
		{
			FvInt32 initialStreamSize = stream.Size();

			for (FvUInt32 i = 0; i < this->PropertyCount(); i++)
			{
				FvDataDescription * pDD = this->Property( i );

				if (FvEntityDescription::ShouldConsiderData( pass, pDD,
							dataDomains ))
				{
					if (!visitor.AddToStream( *pDD, stream,
						(dataDomains & FV_ONLY_PERSISTENT_DATA) != 0 ))
					{
						FV_ERROR_MSG( "FvEntityDescription::AddToStream: "
									"Failed to add to stream while adding %s. "
									"STREAM NOW INVALID!!\n",
							pDD->Name().c_str() );
						return false;
					}
				}
			}

			if ((pDataSizes != NULL) && (actualPass < numDataSizes))
			{
				pDataSizes[actualPass] = stream.Size() - initialStreamSize;
			}

			actualPass++;
		}
	}

	FV_ASSERT_DEV( (numDataSizes == 0) || (numDataSizes == actualPass) ||
			(numDataSizes == actualPass - 1) );

	return true;
}


bool FvEntityDescription::Visit( FvInt32 dataDomains,
		FvIDataDescriptionVisitor & visitor ) const
{
	for ( FvInt32 pass = 0; pass < NUM_PASSES; pass++ )
	{
		if (!FvEntityDescription::ShouldSkipPass( pass, dataDomains ))
		{
			for (FvUInt32 i = 0; i < this->PropertyCount(); i++)
			{
				FvDataDescription * pDD = this->Property( i );

				if (FvEntityDescription::ShouldConsiderData( pass, pDD,
					dataDomains ))
				{
					if (!visitor.Visit( *pDD ))
						return false;
				}
			}
		}
	}

	return true;
}


bool FvEntityDescription::ReadStreamToDict( FvBinaryIStream & stream,
	FvInt32 dataDomains, FvEntityAttributes* pDict ) const
{
	class Visitor : public FvIDataDescriptionVisitor
	{
		FvBinaryIStream & m_kStream;
		FvEntityAttributes* m_pkDict;
		bool m_bOnlyPersistent;

	public:
		Visitor( FvBinaryIStream & stream, FvEntityAttributes* pDict, bool onlyPersistent )
		:m_kStream( stream )
		,m_pkDict( pDict )
		,m_bOnlyPersistent( onlyPersistent )
		{
		}

		bool Visit( const FvDataDescription & dataDesc )
		{
			// TRACE_MSG( "FvEntityDescription::readStream: Reading Property=%s\n", pDD->Name().c_str() );

			FvObjPtr pValue = dataDesc.CreateFromStream( m_kStream,
									m_bOnlyPersistent );

			FV_ASSERT_DEV( pValue );

			if (pValue)
			{
				m_pkDict->SetAttribut(dataDesc.Name(), pValue);
			}
			else
			{
				FV_ERROR_MSG( "FvEntityDescription::readStream: "
							"Could not create %s from stream.\n",
						dataDesc.Name().c_str() );
				return false;
			}

			return !m_kStream.Error();
		}
	};

	Visitor visitor( stream, pDict,
			((dataDomains & FV_ONLY_PERSISTENT_DATA) != 0) );
	return this->Visit( dataDomains, visitor );
}

bool FvEntityDescription::ReadStreamToSection( FvBinaryIStream & stream,
	FvInt32 dataDomains, FvXMLSectionPtr pSection ) const
{
	class Visitor : public FvIDataDescriptionVisitor
	{
		FvBinaryIStream & m_kStream;
		FvXMLSectionPtr m_spSection;
		bool m_bOnlyPersistent;

	public:
		Visitor( FvBinaryIStream & stream, FvXMLSectionPtr pSection, bool onlyPersistent )
		:m_kStream( stream )
		,m_spSection( pSection )
		,m_bOnlyPersistent( onlyPersistent )
		{}

		bool Visit( const FvDataDescription & dataDesc )
		{
			FvXMLSectionPtr pCurr = m_spSection->OpenSection( dataDesc.Name(), true );
			FV_ASSERT_DEV( pCurr );

			FvObjPtr pObj = dataDesc.CreateFromStream( m_kStream, m_bOnlyPersistent );
			dataDesc.AddToSection(pObj, pCurr);

			return true;
		}
	};

	Visitor visitor( stream, pSection,
			((dataDomains & FV_ONLY_PERSISTENT_DATA) != 0) );
	return this->Visit( dataDomains, visitor );
}


void FvEntityDescription::AddToMD5( FvMD5 & md5 ) const
{
	md5.Append( m_kName.c_str(), m_kName.size() );

	Properties::const_iterator propertyIter = m_kProperties.begin();

	while (propertyIter != m_kProperties.end())
	{
		// Ignore the server side only ones.
		if (propertyIter->IsClientServerData())
		{
			FvInt32 csi = propertyIter->ClientServerFullIndex();
			md5.Append( &csi, sizeof( csi ) );
			propertyIter->AddToMD5( md5 );
		}

		propertyIter++;
	}

	FvInt32 count;
	FvMethodList::const_iterator methodIter;

	count = 0;
	methodIter = m_kClient.InternalMethods().begin();

	while (methodIter != m_kClient.InternalMethods().end())
	{
		methodIter->AddToMD5( md5, count );
		count++;

		methodIter++;
	}

	count = 0;
	const FvMethodList & baseMethods = m_kBase.InternalMethods();
	methodIter = baseMethods.begin();

	while (methodIter != baseMethods.end())
	{
		if (methodIter->IsExposed())
		{
			methodIter->AddToMD5( md5, count );
			count++;
		}

		methodIter++;
	}

	count = 0;
	const FvMethodList & cellMethods = m_kCell.InternalMethods();
	methodIter = cellMethods.begin();

	while (methodIter != cellMethods.end())
	{
		if (methodIter->IsExposed())
		{
			methodIter->AddToMD5( md5, count );
			count++;
		}

		methodIter++;
	}
}


void FvEntityDescription::AddPersistentPropertiesToMD5( FvMD5 & md5 ) const
{
	md5.Append( m_kName.c_str(), m_kName.size() );

	Properties::const_iterator propertyIter = m_kProperties.begin();

	while (propertyIter != m_kProperties.end())
	{
		if (propertyIter->IsPersistent())
		{
			propertyIter->AddToMD5( md5 );
		}

		propertyIter++;
	}
}


FvUInt32 FvEntityDescription::ClientServerPropertyCount() const
{
	return m_kClientServerProperties.size();
}


FvDataDescription* FvEntityDescription::ClientServerProperty( FvUInt32 n ) const
{
	FV_IF_NOT_ASSERT_DEV(n < m_kClientServerProperties.size())
	{
		FV_EXIT( "invalid Property requested" );
	}

	return this->Property( m_kClientServerProperties[n] );
}


FvUInt32 FvEntityDescription::ClientMethodCount() const
{
	return m_kClient.InternalMethods().size();
}


FvMethodDescription * FvEntityDescription::ClientMethod( FvUInt8 index, FvBinaryIStream & data ) const
{
	return m_kClient.ExposedMethod( index, data );
}


FvMethodDescription * FvEntityDescription::FindClientMethod( const FvString & name ) const
{
	return m_kClient.Find( name );
}

void FvEntityDescription::FvMethods::Dump(FvXMLSectionPtr pSection)
{
	for(int i=0; i<(int)m_kInternalMethods.size(); ++i)
	{
		m_kInternalMethods[i].Dump(pSection);
	}
}

void FvEntityDescription::Dump(FvXMLSectionPtr pSection)
{
	if(!pSection) return;

	FvXMLSectionPtr pChild, pChild1, pChild2;

	pChild = pSection->NewSection("EntityTypeID");
	pChild->SetInt(m_uiIndex);
	pChild = pSection->NewSection("ClientIndex");
	pChild->SetInt(m_uiClientIndex);
	pChild = pSection->NewSection("ClientName");
	pChild->SetString(m_kClientName);
	pChild = pSection->NewSection("VolatileInfo");
	if(pChild)
	{
		pChild1 = pChild->NewSection("PositionPriority");
		pChild1->SetFloat(m_kVolatileInfo.m_fPositionPriority);
		pChild1 = pChild->NewSection("YawPriority");
		pChild1->SetFloat(m_kVolatileInfo.m_fYawPriority);
		pChild1 = pChild->NewSection("PitchPriority");
		pChild1->SetFloat(m_kVolatileInfo.m_fPitchPriority);
		pChild1 = pChild->NewSection("RollPriority");
		pChild1->SetFloat(m_kVolatileInfo.m_fRollPriority);
	}
#ifdef FV_SERVER
	pChild = pSection->NewSection("LoDLevels");
	for(int i=0; i<m_kLodLevels.Size(); ++i)
	{
		pChild1 = pChild->NewSection("Level");
		const FvDataLoDLevel& lodLevel = m_kLodLevels.GetLevel(i);
		if(pChild1)
		{
			pChild2 = pChild1->NewSection("Label");
			pChild2->SetString(lodLevel.Label());
			pChild2 = pChild1->NewSection("Low");
			pChild2->SetFloat(lodLevel.Low());
			pChild2 = pChild1->NewSection("High");
			pChild2->SetFloat(lodLevel.High());
			pChild2 = pChild1->NewSection("Hyst");
			pChild2->SetFloat(lodLevel.Hyst());
		}
	}
#endif
	pChild = pSection->NewSection("NumEventStampedProperties");
	pChild->SetInt(m_uiNumEventStampedProperties);
	pChild = pSection->NewSection("bHasClientScript");
	pChild->SetString(m_bHasClientScript ? "true" : "false");
	pChild = pSection->NewSection("bHasBaseScript");
	pChild->SetString(m_bHasBaseScript ? "true" : "false");
	pChild = pSection->NewSection("bHasCellScript");
	pChild->SetString(m_bHasCellScript ? "true" : "false");
	pChild = pSection->NewSection("bHasGlobalScript");
	pChild->SetString(m_bHasGlobalScript ? "true" : "false");
	pChild = pSection->NewSection("Properties");
	for(int i=0; i<(int)m_kProperties.size(); ++i)
	{
		m_kProperties[i].Dump(pChild);
	}
	pChild = pSection->NewSection("ClientMethods");
	m_kClient.Dump(pChild);
	pChild = pSection->NewSection("BaseMethods");
	m_kBase.Dump(pChild);
	pChild = pSection->NewSection("CellMethods");
	m_kCell.Dump(pChild);
	pChild = pSection->NewSection("GlobalMethods");
	m_kGlobal.Dump(pChild);
}


#if FV_ENABLE_WATCHERS
FvWatcherPtr FvEntityDescription::pWatcher()
{
	static FvWatcherPtr watchMe = NULL;

	if (watchMe == NULL)
	{
		watchMe = new FvDirectoryWatcher();
		FvEntityDescription *pNull = NULL;
		watchMe->AddChild( "CellMethods", 
						   FvEntityDescription::FvMethods::pWatcher(), 
						   &pNull->m_kCell );
		watchMe->AddChild( "BaseMethods", 
						   FvEntityDescription::FvMethods::pWatcher(), 
						   &pNull->m_kBase );
		watchMe->AddChild( "ClientMethods", 
						   FvEntityDescription::FvMethods::pWatcher(), 
						   &pNull->m_kClient );
		watchMe->AddChild( "GlobalMethods", 
							FvEntityDescription::FvMethods::pWatcher(), 
							&pNull->m_kGlobal );
	}

	return watchMe;
}

#endif // FV_ENABLE_WATCHERS

bool FvEntityDescription::FvMethods::Init( FvXMLSectionPtr pMethods,
		FvMethodDescription::eComponent component, const char * interfaceName )
{
	if (!pMethods)
	{
		FV_WARNING_MSG( "FvEntityDescription::FvMethods::Init: pMethods is NULL\n" );

		return false;
	}

	FvXMLSectionIterator iter = pMethods->Begin();

	while (iter != pMethods->End())
	{
		if((*iter)->GetType() != TiXmlNode::ELEMENT)
		{
			++iter;
			continue;
		}

		FvMethodDescription methodDescription;

		if (!methodDescription.Parse( *iter, component ))
		{
			FV_WARNING_MSG( "Error parsing method %s\n",
				methodDescription.Name().c_str() );
			return false;
		}
		if (component == FvMethodDescription::CLIENT)	// all client methods are exposed
			methodDescription.SetExposed();

		methodDescription.InternalIndex( m_kInternalMethods.size() );
		m_kInternalMethods.push_back( methodDescription );

		if (methodDescription.IsExposed())
		{
			m_kInternalMethods.back().ExposedIndex( m_kExposedMethods.size() );
			m_kExposedMethods.push_back( methodDescription.InternalIndex() );
		}
/**
		if (m_kMap.find( methodDescription.Name() ) != m_kMap.end())
		{
			FV_ERROR_MSG( "FvEntityDescription::FvMethods::Init: "
					"method %s appears more than once\n",
				methodDescription.Name().c_str() );
		}

		m_kMap[ methodDescription.Name() ] = methodDescription.InternalIndex();
**/
		++iter;
	}
/**
	CheckExposedForSubSlots();
**/
	return true;
}


void FvEntityDescription::FvMethods::CheckExposedForSubSlots()
{
	FvInt32 numExposed = (FvInt32)m_kExposedMethods.size();
	FvInt32 numSubSlots = (numExposed-63 + 255) / 255;
	FvInt32 begSubSlot = 62 - numSubSlots;

	if (numSubSlots <= 0) return;
	// never have to reset FvMethodDescription...  once it's been sub-slotted
	// it will always remain so, even when derived from

	for (FvUInt32 eindex = 0; eindex < m_kExposedMethods.size(); eindex++)
	{
		FvUInt32 iindex = m_kExposedMethods[eindex];
		FvMethodDescription & mdesc = m_kInternalMethods[iindex];

		FvInt32 overBy = eindex - begSubSlot;
		if (overBy < 0)
			mdesc.ExposedIndex( eindex, -1 );
		else
			mdesc.ExposedIndex( begSubSlot + (overBy>>8), FvUInt8(overBy) );
	}
}


void FvEntityDescription::FvMethods::Supersede()
{
	m_kMap.clear();
	for (List::iterator it = m_kInternalMethods.begin();
		it != m_kInternalMethods.end(); it++)
	{
		FvString & str = const_cast<FvString&>( it->Name() );
		str = "old_" + str;

		m_kMap[ it->Name() ] = it - m_kInternalMethods.begin();
	}
}


FvUInt32 FvEntityDescription::FvMethods::Size() const
{
	return m_kInternalMethods.size();
}

FvMethodDescription * FvEntityDescription::FvMethods::InternalMethod(
		FvUInt32 index ) const
{
	if (index < m_kInternalMethods.size())
	{
		return const_cast<FvMethodDescription *>( &m_kInternalMethods[ index ] );
	}
	else
	{
		FV_ERROR_MSG( "FvEntityDescription::serverMethod: "
				"Do not have server method %d. There are only %d.\n"
				"	Check that entities.xml is up-to-date.\n",
				(FvInt32)index, (FvInt32)m_kInternalMethods.size() );
		return NULL;
	}
}


FvMethodDescription * FvEntityDescription::FvMethods::ExposedMethod(
	FvUInt8 topIndex, FvBinaryIStream & data ) const
{
	FvInt32 numExposed = (FvInt32)m_kExposedMethods.size();
	// see if topIndex implies a subIndex on the stream
	FvInt32 numSubSlots = (numExposed-63 + 255) / 255;
	FvInt32 begSubSlot = 62 - numSubSlots;
	FvInt32 curSubSlot = topIndex - begSubSlot;
	FvInt32 index = curSubSlot < 0 ? topIndex :
		(begSubSlot + (curSubSlot<<8) + *(FvUInt8*)data.Retrieve( 1 ));

	if (index < numExposed)
	{
		FvInt32 internalIndex = m_kExposedMethods[ index ];
		return const_cast<FvMethodDescription *>(
			&m_kInternalMethods[ internalIndex ] );
	}
	else
	{
		FV_ERROR_MSG( "FvEntityDescription::serverMethod: "
				"Do not have exposed method %d. There are only %d.\n"
				"	Check that entities.xml is up-to-date.\n",
				(FvInt32)index, (FvInt32)m_kExposedMethods.size() );
		return NULL;
	}
}


FvMethodDescription * FvEntityDescription::FvMethods::Find( const FvString & name ) const
{
	Map::const_iterator iter = m_kMap.find( name.c_str() );

	return (iter != m_kMap.end()) ? this->InternalMethod( iter->second ) : NULL;
}

#if FV_ENABLE_WATCHERS

FvWatcherPtr FvEntityDescription::FvMethods::pWatcher()
{
	//typedef FvSmartPointer< FvSequenceWatcher< List > > SequenceWatcherPtr;

	//static SequenceWatcherPtr watchMe = NULL;

	//if (watchMe == NULL)
	//{
	//	FvMethods * pNull = NULL;
	//	watchMe = new FvSequenceWatcher< List >( pNull->m_kInternalMethods );
	//	watchMe->SetLabelSubPath( "Name" );
	//	watchMe->AddChild( "*", FvMethodDescription::pWatcher() ); 
	//}

	//return watchMe;
	return NULL;
}
#endif // FV_ENABLE_WATCHERS


FvDataLoDLevel::FvDataLoDLevel()
:m_fLow( FLT_MAX )
,m_fHigh( FLT_MAX )
,m_fStart( FLT_MAX )
,m_fHyst( 0.f )
,m_kLabel()
,m_iIndex( -1 )
{
}

void FvDataLoDLevel::Finalise( FvDataLoDLevel * pPrev, bool isLast )
{
	if (pPrev)
	{
		float v = pPrev->m_fStart;
		m_fLow = v * v;
	}
	else
	{
		m_fLow = -1;
	}

	if (!isLast)
	{
		float v = m_fStart + m_fHyst;
		m_fHigh = v * v;
	}
}


FvDataLoDLevels::FvDataLoDLevels() : m_iSize( 1 )
{
	for (FvUInt32 i = 0; i < sizeof( m_akLevel )/sizeof( m_akLevel[0] ); ++i)
	{
		m_akLevel[i].Index( i );
	}

	// Make the initial state valid.
	m_akLevel[0].Finalise( NULL, true );
}


namespace
{
bool CompareLevels( const FvDataLoDLevel & level1, const FvDataLoDLevel & level2 )
{
	return level1.Start() < level2.Start();
}
}


bool FvDataLoDLevels::AddLevels( FvXMLSectionPtr pSection )
{
	if (!pSection)
	{
		return true;
	}

	FvXMLSectionIterator iter = pSection->Begin();

	while (iter != pSection->End())
	{
		if((*iter)->GetType() != TiXmlNode::ELEMENT)
		{
			++iter;
			continue;
		}

		float start = (*iter)->AsFloat();
		float hyst = (*iter)->ReadFloat( "hyst", 10.f );
		FvString label = (*iter)->ReadString( "label" );
		FvDataLoDLevel * pLevel = this->Find( label );

		if (pLevel == NULL)
		{
			if (m_iSize <= FV_MAX_DATA_LOD_LEVELS)
			{
				pLevel = &m_akLevel[ m_iSize - 1 ];
				m_iSize++;
			}
			else
			{
				FV_ERROR_MSG( "FvDataLoDLevels::AddLevels: "
						"Only allowed %d levels.\n", FV_MAX_DATA_LOD_LEVELS );
				return false;
			}
		}

		pLevel->Set( label, start, hyst );

		iter++;
	}

	// Sort and adjust levels.
	{
		FV_IF_NOT_ASSERT_DEV( m_iSize <= FvInt32(sizeof( m_akLevel )/sizeof( FvDataLoDLevel)) )
		{
			return false;
		}

		std::sort( &m_akLevel[0], &m_akLevel[m_iSize-1], CompareLevels );

		FvDataLoDLevel * pPrev = NULL;

		for (FvInt32 i = 0; i <= m_iSize-1; ++i)
		{
			m_akLevel[i].Finalise( pPrev, i == m_iSize-1 );
			pPrev = &m_akLevel[i];
		}
	}

	return true;
}


FvDataLoDLevel * FvDataLoDLevels::Find( const FvString & label )
{
	for (FvInt32 i = 0; i < m_iSize - 1; ++i)
	{
		if (m_akLevel[i].Label() == label)
		{
			return &m_akLevel[i];
		}
	}

	return NULL;
}


bool FvDataLoDLevels::FindLevel( FvInt32 & level, FvXMLSectionPtr pSection ) const
{
	if (pSection)
	{
		const FvString label = pSection->AsString();

		for (FvInt32 i = 0; i < m_iSize-1; ++i)
		{
			if (label == m_akLevel[ i ].Label())
			{
				level = m_akLevel[ i ].Index();
				return true;
			}
		}

		level = 0;

		FV_ERROR_MSG( "FvDataLoDLevels:FindLevel: Did not Find '%s'\n",
				label.c_str() );
	}
	else
	{
		// No section means that it is in the outer detail level.
		level = FvDataLoDLevel::OUTER_LEVEL;
		return true;
	}

	return false;
}


void FvEntityDescription::PropertyEventStamps::AddToStream(
		FvBinaryOStream & stream ) const
{
	Stamps::const_iterator iter = m_kEventStamps.begin();

	while (iter != m_kEventStamps.end())
	{
		stream << (*iter);

		iter++;
	}
}


void FvEntityDescription::PropertyEventStamps::RemoveFromStream(
		FvBinaryIStream & stream )
{
	Stamps::iterator iter = m_kEventStamps.begin();

	while (iter != m_kEventStamps.end())
	{
		stream >> (*iter);

		iter++;
	}
}

FvEntityAttributes::FvEntityAttributes(const FvEntityDescription& entityDesc)
:m_kEntityDesc(entityDesc)
,m_pkPropertyCallBack(NULL)
,m_iSpaceID(0),m_uiSpaceType(0)
{
	m_kAttributs.resize(m_kEntityDesc.PropertyCount(), NULL);
}

FvEntityAttributes::~FvEntityAttributes()
{
	FvUInt32 size = (FvUInt32)m_kAttributs.size();
	for(FvUInt32 i=0; i<size; ++i)
	{
		if(m_kAttributs[i])
			m_kAttributs[i]->Detach();
	}
}

FvObjPtr FvEntityAttributes::GetAttribut(const FvString& name) const
{
	FvUInt32 idx(0);
	if(m_kEntityDesc.FindPropertyIndex(name, idx))
		return m_kAttributs[idx];
	else
		return NULL;
}

FvObjPtr FvEntityAttributes::GetAttribut(const char* name) const
{
	FvUInt32 idx(0);
	if(m_kEntityDesc.FindPropertyIndex(FvString(name), idx))
		return m_kAttributs[idx];
	else
		return NULL;
}

FvObjPtr FvEntityAttributes::GetAttribut(FvUInt32 idx) const
{
	if(m_kEntityDesc.PropertyCount() > idx)
		return m_kAttributs[idx];
	else
		return NULL;
}

bool FvEntityAttributes::SetAttribut(const FvString& name, FvObjPtr pObj)
{
	FvUInt32 idx(0);
	if(m_kEntityDesc.FindPropertyIndex(name, idx))
	{
		FvObjPtr pOldObj = m_kAttributs[idx];
		if(pOldObj) pOldObj->Detach();
		m_kAttributs[idx] = pObj;
		if(pObj)
		{
			pObj->Attach(this);
			pObj->SetRef(idx);
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool FvEntityAttributes::SetAttribut(FvUInt32 idx, FvObjPtr pObj)
{
	if(m_kEntityDesc.PropertyCount() > idx)
	{
		FvObjPtr pOldObj = m_kAttributs[idx];
		if(pOldObj) pOldObj->Detach();
		m_kAttributs[idx] = pObj;
		if(pObj)
		{
			pObj->Attach(this);
			pObj->SetRef(idx);
		}
		return true;
	}
	else
	{
		return false;
	}
}

void FvEntityAttributes::PropertyChanged(FvPropertyOperator op, FvUInt32 opIdx,
							 const ChangeVal& oldVal, const ChangeVal& newVal, ChangePath& path)
{
	if(!m_pkPropertyCallBack)
		return;

	FvUInt32 pathDepth = path.size();
	FvUInt32 topLevelIdx = pathDepth==0 ? opIdx : path[pathDepth-1];
	FvDataDescription* pkDataDes = m_kEntityDesc.Property(topLevelIdx);
	FV_ASSERT(pkDataDes);

	FvMemoryOStream kMsg;
	FvUInt8 uiMessageID(0);
	if(m_pkPropertyCallBack->IsNeedAddToStream() &&
		(pkDataDes->IsClientServerData() || pkDataDes->IsGhostedData()))
	{
		uiMessageID = AddToStream(op, opIdx, newVal, path, kMsg);
	}

	if(pathDepth == 0)
		m_pkPropertyCallBack->HandlePropertyEvent(pkDataDes, oldVal.second.GetObject(), newVal.second.GetObject(), uiMessageID, kMsg);
	else
		m_pkPropertyCallBack->HandlePropertyEvent(pkDataDes, NULL, NULL, uiMessageID, kMsg);
}

FvInt32 FvEntityAttributes::PropertyDivisions()
{
	//return -1;
	return (FvInt32)m_kAttributs.size();
}

FvPropertyOwnerBase* FvEntityAttributes::PropertyVassal(FvUInt32 ref)
{
	FV_ASSERT(ref < (FvUInt32)m_kAttributs.size());
	return m_kAttributs[ref].GetObject();
}

void FvEntityAttributes::PropertyRenovate(FvPropertyOperator op, FvUInt32 opIdx,
							  FvBinaryIStream& data, ChangeVal& oldValue, ChangeVal& newValue)
{
	FvObjPtr oldObj, newObj;

	switch(op)
	{
	case FvPropertyOwnerBase::FVPROP_OP_MOD:
		{
			FV_ASSERT(opIdx < m_kEntityDesc.PropertyCount());
			oldObj = m_kAttributs[opIdx];
			oldObj->Detach();
			FvDataDescription* pDataType = m_kEntityDesc.Property(opIdx);
			newObj = pDataType->CreateFromStream(data, false);
			FV_ASSERT(newObj);
			m_kAttributs[opIdx] = newObj;
			newObj->SetRef(opIdx);
			newObj->Attach(this);
			newObj->SetNewObj();
		}
		break;
	default:
		FV_ASSERT(0);
		break;
	}

	oldValue.second = oldObj;
	newValue.second = newObj;
}

void FvEntityAttributes::OnPropertyEvent(FvUInt8 messageID, FvBinaryIStream& stream,
										 FvUInt32& topLevelIdx, FvUInt32& pathDepth,
										 FvPropertyOwnerBase::ChangeVal& oldVal, FvPropertyOwnerBase::ChangeVal& newVal)
{
	FvPropertyOwnerBase::ChangePath path;
	FvPropertyOwnerBase::FvPropertyOperator op;
	FvUInt32 opIdx;
	FvPropertyOwnerBase* pOwner = GetPathFromStream(messageID, stream, op, opIdx, path);
	pOwner->PropertyRenovate(op, opIdx, stream, oldVal, newVal);
	pathDepth = path.size();
	topLevelIdx = pathDepth==0 ? opIdx : path[pathDepth-1];
}

FvEntityAttributes* FvEntityAttributes::Copy() const
{
	FvEntityAttributes* pkNewDic = new FvEntityAttributes(m_kEntityDesc);

	for(int i=0; i<(int)m_kAttributs.size(); ++i)
	{
		if(m_kAttributs[i])
			pkNewDic->m_kAttributs[i] = m_kAttributs[i]->Copy();
	}

	pkNewDic->m_kPos = m_kPos;
	pkNewDic->m_kDir = m_kDir;
	pkNewDic->m_iSpaceID = m_iSpaceID;
	pkNewDic->m_uiSpaceType = m_uiSpaceType;

	return pkNewDic;
}

void FvEntityAttributes::Dump( FvXMLSectionPtr pSection )
{
	if(!pSection) return;

	FvUInt32 size = (FvUInt32)m_kAttributs.size();
	for(FvUInt32 i=0; i<size; ++i)
	{
		if(m_kAttributs[i])
		{
			m_kEntityDesc.Property(i)->Dump(pSection);
		}
	}
}

void FvEntityAttributes::DumpSizeAndCnt()
{
	FvObj::ClearObjCnt();
	int iSize(0);
	for(int i=0; i<(int)m_kAttributs.size(); ++i)
	{
		if(m_kAttributs[i])
			iSize += m_kAttributs[i]->GetSize();
	}
	FvUInt64* pkCnt = FvObj::GetObjCnt();
	FV_INFO_MSG("DumpSizeAndCnt: Size %d B, %d K\n", iSize, iSize/1024);

#define _PRINT_OBJ_CNT(_TYPE)	FV_INFO_MSG("\t" #_TYPE "\t%I64u\n", pkCnt[_TYPE])

	_PRINT_OBJ_CNT(FVOBJ_RTTI_INT8);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_UINT8);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_INT16);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_UINT16);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_INT32);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_UINT32);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_INT64);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_UINT64);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_FLOAT);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_DOUBLE);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_VECTOR2);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_VECTOR3);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_VECTOR4);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_STRING);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_ARRAY);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_INTMAP);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_STRMAP);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_STRUCT);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_MAILBOX);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_BLOB);
	_PRINT_OBJ_CNT(FVOBJ_RTTI_USERDATAOBJ);

#undef	_PRINT_OBJ_CNT
}

bool FvEntityAttributes::ReadStreamToPosDir(FvBinaryIStream& stream)
{
	if(m_kEntityDesc.HasCellScript())
	{
		stream >> m_kPos >> m_kDir >> m_iSpaceID >> m_uiSpaceType;
	}

	return true;
}

void FvEntityAttributes::AddPosDirToStream(FvBinaryOStream& stream) const
{
	if(m_kEntityDesc.HasCellScript())
	{
		stream << m_kPos << m_kDir << m_iSpaceID << m_uiSpaceType;
	}
}

bool FvEntityDescription::CompressMethodId(int methodId, int& baseId, int& extId) const
{
	//if(!m_pkEntityExtFuncAddr || (FvUInt32)methodId>=m_pkEntityExtFuncAddr->methodCnt)
	//	return false;

	//int numExposed = (int)m_pkEntityExtFuncAddr->methodCnt;
	//int numSubSlots = (numExposed-63 + 255) / 255;
	//int begSubSlot = 62 - numSubSlots;

	//if (numSubSlots <= 0)
	//{
	//	baseId = methodId;
	//	extId = -1;
	//}
	//else
	//{
	//	int overBy = methodId - begSubSlot;
	//	if (overBy < 0)
	//	{
	//		baseId = methodId;
	//		extId = -1;
	//	}
	//	else
	//	{
	//		baseId = begSubSlot + (overBy>>8);
	//		extId = FvUInt8(overBy);
	//	}
	//}

	return true;
}

bool FvEntityDescription::DecompressMethodId(int messageId, int& methodId, FvBinaryIStream& data) const
{
	//if(!m_pkEntityExtFuncAddr)
	//	return false;

	//int numExposed = (int)m_pkEntityExtFuncAddr->methodCnt;
	//// see if topIndex implies a subIndex on the stream
	//int numSubSlots = (numExposed-63 + 255) / 255;
	//int begSubSlot = 62 - numSubSlots;
	//int curSubSlot = messageId - begSubSlot;
	//int index = curSubSlot < 0 ? messageId :
	//	(begSubSlot + (curSubSlot<<8) + *(FvUInt8*)data.Retrieve( 1 ));

	//if (index < numExposed)
	//{
	//	methodId = index;
	//	return true;
	//}
	//else
	//{
	//	return false;
	//}
	return true;
}













