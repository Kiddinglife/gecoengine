#include "FvDBCustom.h"
#include "FvDatabase.h"
#include "FvDBEntitydefs.h"

FvXMLSectionPtr CreateNewEntity( FvEntityTypeID uiTypeID, const FvString &kName )
{
	const FvEntityDefs &kEntityDefs = FvDatabase::Instance().GetEntityDefs();
	FvXMLSectionPtr spSection = 
			new FvXMLSection( kEntityDefs.GetEntityDescription( uiTypeID ).Name() );
	
	const FvString &kNameProp = kEntityDefs.GetNameProperty( uiTypeID );
	if (!kNameProp.empty())
	{
		if (kEntityDefs.GetPropertyType( uiTypeID, kNameProp ) == "BLOB" )
			spSection->WriteBlob( kNameProp, kName );
		else
			spSection->WriteString( kNameProp, kName );
	}
	return spSection;
}
