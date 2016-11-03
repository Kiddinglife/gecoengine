#include "FvBaseUserDataObjectDescription.h"

#include <FvMD5.h>
#include <FvDebug.h>
#include <FvBinaryStream.h>
#include <OgreResourceGroupManager.h>

#include "FvEntityDefConstants.h"

FV_DECLARE_DEBUG_COMPONENT2( "FvDataDescription", 0 )


FvBaseUserDataObjectDescription::FvBaseUserDataObjectDescription()
{
}

FvBaseUserDataObjectDescription::~FvBaseUserDataObjectDescription()
{
}

bool FvBaseUserDataObjectDescription::Parse( const FvString & name,
		FvXMLSectionPtr pSection, bool isFinal )
{
	if (!pSection)
	{
		FvString kFilename = this->GetDefsDir() + "/" + name + ".def";
		pSection = OpenXMLInDef(this->GetShortDefsDir(), name + ".def");
		if (!pSection)
		{
			FV_ERROR_MSG( "FvBaseUserDataObjectDescription::Parse: Could not open %s\n",
					kFilename.c_str() );
			return false;
		}
	}

	FvString parentName = pSection->ReadString( "Parent" );

	if (!parentName.empty())
	{
		if (!this->Parse( parentName, NULL, false ))
		{
			FV_ERROR_MSG( "FvBaseUserDataObjectDescription::Parse: "
						"Could not Parse %s, parent of %s\n",
					parentName.c_str(), name.c_str() );
			return false;
		}
	}

	m_kName = name;
	bool result = this->ParseInterface( pSection, m_kName.c_str() );
	return result;
}

bool FvBaseUserDataObjectDescription::ParseInterface( FvXMLSectionPtr pSection,
									const char * interfaceName )
{
	if (!pSection)
	{
		return false;
	}

	bool result =
		this->ParseImplements( pSection->OpenSection( "Implements" ) ) &&
		this->ParseProperties( pSection->OpenSection( "Properties" ) ); 
	return result;
}

bool FvBaseUserDataObjectDescription::ParseImplements( FvXMLSectionPtr pInterfaces )
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
				FV_ERROR_MSG( "FvBaseUserDataObjectDescription::ParseImplements: "
					"Failed parsing interface %s\n", interfaceName.c_str() );
				result = false;
			}

			++iter;
		}
	}

	return result;

}

void FvBaseUserDataObjectDescription::AddToDictionary( FvXMLSectionPtr pSection, FvObj* pDict ) const
{
}

FvUInt32 FvBaseUserDataObjectDescription::PropertyCount() const
{
	return m_kProperties.size();
}

FvDataDescription* FvBaseUserDataObjectDescription::Property( FvUInt32 n ) const
{
	FV_ASSERT_DEV(n < m_kProperties.size());
	if (n >= m_kProperties.size()) return NULL;

	return const_cast<FvDataDescription *>( &m_kProperties[n] );
}

FvDataDescription* FvBaseUserDataObjectDescription::FindProperty( const FvString& name ) const
{
	PropertyMap::const_iterator iter = m_kPropertyMap.find( name );

	return (iter != m_kPropertyMap.end()) ? this->Property( iter->second ) : NULL;
}

bool FvBaseUserDataObjectDescription::FindPropertyIndex( const FvString& name, FvUInt32& idx ) const
{
	PropertyMap::const_iterator iter = m_kPropertyMap.find( name );

	if((iter != m_kPropertyMap.end()))
	{
		idx = iter->second;
		return true;
	}
	else
	{
		return false;
	}
}

FV_ENTITYDEF_API FvString g_kFatherDefsPath;

FvXMLSectionPtr	OpenXMLInDef(const FvString& kDir, const FvString& kFile)
{
	if(!g_kFatherDefsPath.empty())
	{
		FvString kFullPath = g_kFatherDefsPath + "/" + kDir + "/" + kFile;

		FILE *pkSrcFile;
		int iError;
		iError = fopen_s(&pkSrcFile, kFullPath.c_str(), "rb");
		if(iError == 0)
		{
			Ogre::DataStreamPtr spDataStream(OGRE_NEW Ogre::FileHandleDataStream(pkSrcFile));
			FvXMLSectionPtr spSrcSection = FvXMLSection::OpenSection(spDataStream);
			fclose(pkSrcFile);
			if(!spSrcSection)
			{
				return NULL;
			}
			return spSrcSection;
		}
	}

	Ogre::DataStreamPtr spDataStream;
	try
	{
		spDataStream = Ogre::ResourceGroupManager::getSingleton().openResource("EntityDef/" + kDir + "/" + kFile);
	}
	catch(Ogre::Exception){}
	return FvXMLSection::OpenSection(spDataStream);
}

FvXMLSectionPtr OpenAliasesFile()
{
	Ogre::DataStreamPtr spDataStream;
	try
	{
		spDataStream = Ogre::ResourceGroupManager::getSingleton().openResource(FvEntityDef::Constants::AliasesFile());
	}
	catch(Ogre::Exception){}
	return FvXMLSection::OpenSection(spDataStream);
}

FvXMLSectionPtr OpenFatherAliasesFile()
{
	if(g_kFatherDefsPath.empty())
		return NULL;

	FvString kFullPath = g_kFatherDefsPath + "/alias.xml";

	FILE *pkSrcFile;
	int iError;
	iError = fopen_s(&pkSrcFile, kFullPath.c_str(), "rb");
	if(iError != 0)
	{
		return NULL;
	}

	Ogre::DataStreamPtr spDataStream(OGRE_NEW Ogre::FileHandleDataStream(pkSrcFile));
	FvXMLSectionPtr spSrcSection = FvXMLSection::OpenSection(spDataStream);
	fclose(pkSrcFile);
	if(!spSrcSection)
	{
		return NULL;
	}
	return spSrcSection;
}

