#include "FvUserDataObjectType.h"
#include "FvUserDataObject.h"
#include "FvEntityDefConstants.h"
#include "FvUserDataObjectDescriptionMap.h"

#include <OgreResourceGroupManager.h>
#include <OgreLogManager.h>
#include <OgreArchiveManager.h>
#include <OgreFileSystem.h>


FvUserDataObjectTypes FvUserDataObjectType::ms_kCurTypes;



FvUserDataObjectType::FvUserDataObjectType(const FvUserDataObjectDescription& kDescription)
:m_kDescription(kDescription)
{

}

FvUserDataObjectType::~FvUserDataObjectType()
{

}

FvUserDataObject* FvUserDataObjectType::NewUserDataObject( const FvUserDataObjectInitData& kInitData ) const
{
	FvUserDataObject* pkNewUserDataObject = new FvUserDataObject(const_cast<FvUserDataObjectType*>(this));
	pkNewUserDataObject->Init(kInitData);
	return pkNewUserDataObject;
}

bool FvUserDataObjectType::HasProperty( const char* pcAttr ) const
{
	return m_kDescription.FindProperty(pcAttr) != NULL;
}

bool FvUserDataObjectType::Init()
{
	return FvUserDataObjectType::Load(ms_kCurTypes);
}

bool FvUserDataObjectType::Load( FvUserDataObjectTypes& kTypes )
{
	FV_ASSERT(0);
	//Ogre::DataStreamPtr spDataStream = Ogre::ResourceGroupManager::getSingleton().openResource(FvEntityDef::Constants::UserDataObjectsFile());
	//if(spDataStream.isNull())
	//{
	//	FV_WARNING_MSG( "FvUserDataObjectpType::Init: "
	//		"No user data object mapping present\n" );
	//	return true;
	//}

	//FvXMLSectionPtr spSection = FvXMLSection::OpenSection(spDataStream);
	//if(spSection == NULL)
	//{
	//	FV_WARNING_MSG( "FvUserDataObjectpType::Init: "
	//		"No user data object mapping present\n" );
	//	return false;
	//}

	//FvUserDataObjectDescriptionMap kUserDataObjectDescriptionMap;

	//if(!kUserDataObjectDescriptionMap.Parse(spSection))
	//{
	//	FV_ERROR_MSG( "FvUserDataObjectType::Init: "
	//		"Failed to parse UserDataObjectDescriptionMap\n" );
	//	return false;
	//}

	//for(FvUserDataObjectDescriptionMap::DescriptionMap::const_iterator i = kUserDataObjectDescriptionMap.Begin();
	//	i != kUserDataObjectDescriptionMap.End(); ++i)
	//{
	//	const FvUserDataObjectDescription& kDesc = i->second;
	//	kTypes[kDesc.Name()] = new FvUserDataObjectType(kDesc);
	//}

	return true;
}

void FvUserDataObjectType::Migrate( FvUserDataObjectTypes& kTypes )
{

}

void FvUserDataObjectType::ClearStatics()
{
	ms_kCurTypes.clear();

	extern void DestroyUserDataRefType();
	DestroyUserDataRefType();
}

FvUserDataObjectTypePtr	FvUserDataObjectType::GetType( const char* pcClassName )
{
	FvUserDataObjectTypes::iterator result = ms_kCurTypes.find( pcClassName );
	if(result == ms_kCurTypes.end())
	{
		return NULL;
	}
	else
	{
		return result->second;
	}
}