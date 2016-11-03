#include "FvUserDataObjectDescription.h"
#include "FvEntityDefConstants.h"

#include <FvDebug.h>
#include <FvXMLSection.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvDataDescription", 0 )

#include <algorithm>
#include <cctype>

bool FvUserDataObjectDescription::ParseInterface( FvXMLSectionPtr pSection, const char * interfaceName )
{
	if (!pSection)
	{
		return false;
	}
	
	bool result = FvBaseUserDataObjectDescription::ParseInterface( pSection, interfaceName);
	// Read domain here to ensure no interface or parent overrides it
	FvString domainString = pSection->ReadString("Domain");
	std::transform(domainString.begin(), domainString.end(), domainString.begin(), (FvInt32(*)(FvInt32)) toupper);
	if ( domainString.empty() ){
		//DEFAULT TO CELL
		m_eDomain = CELL; 
		FV_WARNING_MSG("FvUserDataObjectDescription::ParseInterface:"
				"domain not set for user data object default is cell");
	} else if (domainString == "BASE"){
		m_eDomain = BASE;
	} else if (domainString == "CELL"){
		m_eDomain = CELL;
	} else if (domainString == "CLIENT"){
		m_eDomain = CLIENT;
	}
	return result;
}


bool FvUserDataObjectDescription::ParseProperties( FvXMLSectionPtr pProperties )
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

			if (!dataDescription.Parse( *iter, m_kName,
					FvDataDescription::PARSE_IGNORE_FLAGS ))
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

			FvInt32 index = m_kProperties.size();
			PropertyMap::const_iterator propIter = m_kPropertyMap.find( dataDescription.Name() );
			if (propIter != m_kPropertyMap.end())
			{
				FV_INFO_MSG( "FvUserDataObjectDescription::ParseProperties: "
						"property %s.%s is being overridden.\n",
					m_kName.c_str(), dataDescription.Name().c_str() );
				index = propIter->second;
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

const FvString FvUserDataObjectDescription::GetDefsDir() const
{
	return FvEntityDef::Constants::UserDataObjectsDefsPath();
}

const FvString FvUserDataObjectDescription::GetShortDefsDir() const
{
	return FvEntityDef::Constants::UserDataObjectsDefsShortPath();
}

const FvString FvUserDataObjectDescription::GetClientDir() const
{
	return FvEntityDef::Constants::UserDataObjectsClientPath();
}

const FvString FvUserDataObjectDescription::GetCellDir() const
{
	return FvEntityDef::Constants::UserDataObjectsCellPath();
}

const FvString FvUserDataObjectDescription::GetBaseDir() const
{
	return FvEntityDef::Constants::UserDataObjectsBasePath();
}
