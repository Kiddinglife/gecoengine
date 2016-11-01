#include "FvEntityDescriptionMap.h"

#include <FvDebug.h>
#include <FvXMLSection.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvDataDescription", 0 )

FvEntityDescriptionMap::FvEntityDescriptionMap()
{
}

FvEntityDescriptionMap::~FvEntityDescriptionMap()
{
}

bool FvEntityDescriptionMap::Parse( FvXMLSectionPtr pFatherSection, FvXMLSectionPtr pSection )
{
	if (!pSection)
	{
		FV_ERROR_MSG( "FvEntityDescriptionMap::Parse: pSection is NULL\n" );
		return false;
	}

	bool isOkay = true;
	FvInt32 size1 = 0;
	if(pFatherSection)
		size1 = pFatherSection->CountChildren();
	FvInt32 size2 = pSection->CountChildren();
	FvInt32 size = size1 + size2;
	m_kVector.resize( size );

	FvInt32 idx = 0;
	FvEntityTypeID clientIndex = 0;

	if(pFatherSection)
	{
		for (FvInt32 i = 0; i < size1; ++i, ++idx)
		{
			FvXMLSectionPtr pSubSection = pFatherSection->OpenChild( i );
			FvEntityDescription & desc = m_kVector[idx];

			FvString typeName = pSubSection->SectionName();

			if (!desc.Parse( typeName ))
			{
				FV_ERROR_MSG( "FvEntityDescriptionMap: "
					"Failed to Parse def for entity type %s\n",
					typeName.c_str() );

				isOkay = false;
				continue;
			}

			desc.Index( idx );

#ifdef DEBUG_DEFS
			FV_INFO_MSG( "%-14s has %3d props "
				"and %2d client, %2d Base, %2d cell methods\n",
				desc.Name().c_str(), desc.PropertyCount(),
				desc.ClientMethodCount(),
				desc.Base().Size(),
				desc.server().size() );
			for (FvInt32 index = 0; index < desc.PropertyCount(); index++)
			{
				FvDataDescription * pDataDesc = desc.Property( index );
				FV_INFO_MSG( "%s %s %d\n", pDataDesc->Name().c_str(),
					pDataDesc->Description().c_str(),
					pDataDesc->IsClientServerData() );
			}
#endif // DEBUG_DEFS

			m_kMap[ desc.Name() ] = desc.Index();

			if (desc.IsClientType())
			{
				desc.ClientIndex( clientIndex );
				++clientIndex;
			}
		}
	}

	for (FvInt32 i = 0; i < size2; ++i, ++idx)
	{
		FvXMLSectionPtr pSubSection = pSection->OpenChild( i );
		FvEntityDescription & desc = m_kVector[idx];

		FvString typeName = pSubSection->SectionName();

		if (!desc.Parse( typeName ))
		{
			FV_ERROR_MSG( "FvEntityDescriptionMap: "
				"Failed to Parse def for entity type %s\n",
				typeName.c_str() );

			isOkay = false;
			continue;
		}

		desc.Index( idx );

#ifdef DEBUG_DEFS
		FV_INFO_MSG( "%-14s has %3d props "
					"and %2d client, %2d Base, %2d cell methods\n",
				desc.Name().c_str(), desc.PropertyCount(),
				desc.ClientMethodCount(),
				desc.Base().Size(),
				desc.server().size() );
		for (FvInt32 index = 0; index < desc.PropertyCount(); index++)
		{
			FvDataDescription * pDataDesc = desc.Property( index );
			FV_INFO_MSG( "%s %s %d\n", pDataDesc->Name().c_str(),
					pDataDesc->Description().c_str(),
					pDataDesc->IsClientServerData() );
		}
#endif // DEBUG_DEFS

		m_kMap[ desc.Name() ] = desc.Index();

		if (desc.IsClientType())
		{
			desc.ClientIndex( clientIndex );
			++clientIndex;
		}
	}
/**
	DescriptionVector::iterator iter = m_kVector.begin();

	while (iter != m_kVector.end())
	{
		// if name_ != clientName_
		if (iter->HasClientScript() && !iter->IsClientType())
		{
			FvEntityTypeID id;

			if (this->NameToIndex( iter->ClientName(), id ))
			{
				const FvEntityDescription & alias = this->EntityDescription( id );

				if (alias.ClientIndex() != FV_INVALID_ENTITY_TYPE_ID)
				{
					if ((alias.ClientServerPropertyCount() ==
							iter->ClientServerPropertyCount()) &&
						(alias.ClientMethodCount() ==
						 	iter->ClientMethodCount()))
					{
						iter->ClientIndex( alias.ClientIndex() );
						FV_INFO_MSG( "%s is aliased as %s\n",
								iter->Name().c_str(),
								alias.Name().c_str() );
					}
					else
					{
						FV_ERROR_MSG( "FvEntityDescriptionMap::Parse: "
									"%s has mismatched ClientName %s\n",
								iter->Name().c_str(),
								iter->ClientName().c_str() );
						FV_ERROR_MSG( "FvEntityDescriptionMap::Parse: "
								"There are %d methods and %d props instead of "
								"%d and %d\n",
							alias.ClientMethodCount(),
							alias.ClientServerPropertyCount(),
						 	iter->ClientMethodCount(),
							iter->ClientServerPropertyCount() );
						isOkay = false;
					}
				}
				else
				{
					FV_ERROR_MSG( "FvEntityDescriptionMap::Parse: "
								"%s has server-only ClientName %s\n",
							iter->Name().c_str(),
							iter->ClientName().c_str() );
					isOkay = false;
				}
			}
			else
			{
				FV_ERROR_MSG( "FvEntityDescriptionMap::Parse: "
							"%s has invalid ClientName %s\n",
						iter->Name().c_str(),
						iter->ClientName().c_str() );
				isOkay = false;
			}
		}

		++iter;
	}

	FV_INFO_MSG( "Highest exposed counts:\n" );

	isOkay &= this->CheckCount( "client top-level Property",
						&FvEntityDescription::ClientServerPropertyCount, 61, 256);
	isOkay &= this->CheckCount( "client method",
						&FvEntityDescription::ClientMethodCount, 62, 62*256 );
	isOkay &= this->CheckCount( "Base method",
						&FvEntityDescription::ExposedBaseMethodCount, 62, 62*256);
	isOkay &= this->CheckCount( "cell method",
						&FvEntityDescription::ExposedCellMethodCount, 62, 62*256);
	FV_INFO_MSG( "\n" );
**/
	return isOkay;
}


bool FvEntityDescriptionMap::CheckCount( const char * description,
		FvUInt32 (FvEntityDescription::*fn)() const,
		FvInt32 maxEfficient, FvInt32 maxAllowed ) const
{
	if (!m_kVector.empty())
	{
		const FvEntityDescription * pBest = &m_kVector[0];
		FvUInt32 maxCount = 0;

		for (FvEntityTypeID typeID = 0; typeID < this->Size(); ++typeID)
		{
			const FvEntityDescription & eDesc = this->EntityDescription( typeID );

			if ((eDesc.*fn)() > maxCount)
			{
				pBest = &eDesc;
				maxCount = (eDesc.*fn)();
			}
		}

		if (maxCount <= (FvUInt32)maxAllowed)
		{
			FV_INFO_MSG( "\t%s: %s count = %d. Efficient to %d (limit is %d)\n",
					pBest->Name().c_str(), description, maxCount,
					maxEfficient, maxAllowed );
		}
		else
		{
			FV_ERROR_MSG( "FvEntityDescriptionMap::CheckCount: "
					"%s exposed %s count of %d is more than allowed (%d)\n",
				pBest->Name().c_str(), description, maxCount, maxAllowed );
			return false;
		}
	}

	return true;
}


FvInt32 FvEntityDescriptionMap::Size() const
{
	return m_kVector.size();
}


const FvEntityDescription& FvEntityDescriptionMap::EntityDescription(
		FvEntityTypeID index ) const
{
	FV_IF_NOT_ASSERT_DEV( index < FvEntityTypeID(m_kVector.size()) )
	{
		FV_EXIT( "invalid entity type id index" );
	}

	return m_kVector[index];
}


bool FvEntityDescriptionMap::NameToIndex(const FvString& name,
	   	FvEntityTypeID& index) const
{
	DescriptionMap::const_iterator it = m_kMap.find(name);

	if(it != m_kMap.end())
	{
		index = it->second;
		return true;
	}

	return false;
}


void FvEntityDescriptionMap::AddToMD5( FvMD5 & md5 ) const
{
	DescriptionVector::const_iterator iter = m_kVector.begin();

	while (iter != m_kVector.end())
	{
		// Ignore the server side only ones.
		if (iter->IsClientType())
		{
			iter->AddToMD5( md5 );
		}

		iter++;
	}
}


void FvEntityDescriptionMap::AddPersistentPropertiesToMD5( FvMD5 & md5 ) const
{
	DescriptionVector::const_iterator iter = m_kVector.begin();
	
	// Assumes typeID is its order in the vector.
	while (iter != m_kVector.end())
	{
		iter->AddPersistentPropertiesToMD5( md5 );
		iter++;
	}
}


void FvEntityDescriptionMap::Clear()
{
	m_kVector.clear();
	m_kMap.clear();
}

bool FvEntityDescriptionMap::IsEntity( const FvString& name ) const
{
	return m_kMap.find( name ) != m_kMap.end();
}

void FvEntityDescriptionMap::Dump(FvXMLSectionPtr pSection)
{
	if(!pSection) return;

	FvXMLSectionPtr pChild;
	for(int i=0; i<(int)m_kVector.size(); ++i)
	{
		pChild = pSection->NewSection(m_kVector[i].Name());
		m_kVector[i].Dump(pChild);
	}
}
