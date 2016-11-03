#include "FvUserDataObjectDescriptionMap.h"

#include <FvDebug.h>
#include <FvXMLSection.h>

FvInt32 UserDataObjectDescriptionMap_Token =1;
FV_DECLARE_DEBUG_COMPONENT2( "FvUserDataObjectDescriptionMap", 0 )


FvUserDataObjectDescriptionMap::FvUserDataObjectDescriptionMap()
{
}

FvUserDataObjectDescriptionMap::~FvUserDataObjectDescriptionMap()
{
}

bool FvUserDataObjectDescriptionMap::Parse( FvXMLSectionPtr pSection )
{
	if (!pSection)
	{
		FV_ERROR_MSG( "FvUserDataObjectDescriptionMap::Parse: pSection is NULL\n" );
		return false;
	}

	bool isOkay = true;
	FvInt32 size = pSection->CountChildren();

	for (FvInt32 i = 0; i < size; i++)
	{
		FvXMLSectionPtr pSubSection = pSection->OpenChild( i );
		FvUserDataObjectDescription desc;

		FvString typeName = pSubSection->SectionName();

		if (!desc.Parse( typeName ))
		{
			FV_ERROR_MSG( "FvUserDataObjectDescriptionMap: "
				"Failed to Parse def for entity type %s\n",
				typeName.c_str() );

			isOkay = false;
			continue;
		}
		#ifdef FV_SERVER
			if ( (desc.Domain() & (FvUserDataObjectDescription::BASE |
				FvUserDataObjectDescription::CELL) ) == 0){
					continue;
			}
		#else //client or editor
			#ifndef FV_EDITOR_ENABLED //therefore client
				if ( (desc.Domain() & FvUserDataObjectDescription::CLIENT) == 0){
					continue;
				}
			#endif
		#endif

		m_kMap[ desc.Name() ] = desc ;
	}
	return isOkay;
}


FvInt32 FvUserDataObjectDescriptionMap::Size() const
{
	return m_kMap.size();
}

const FvUserDataObjectDescription& FvUserDataObjectDescriptionMap::UdoDescription(
		FvString name ) const
{
	DescriptionMap::const_iterator result = m_kMap.find(name);
	FV_IF_NOT_ASSERT_DEV( result != m_kMap.end() )
	{
		FV_EXIT( "can't find UDO description" );
	}

    return result->second;
}


void FvUserDataObjectDescriptionMap::Clear()
{
	m_kMap.clear();
}

bool FvUserDataObjectDescriptionMap::IsUserDataObject( const FvString& name ) const
{
	return m_kMap.find( name ) != m_kMap.end();
}







