#include "FvServerResource.h"

#include <OgreLogManager.h>
#include <OgreArchiveManager.h>
#include <OgreZip.h>
#include <OgreDDSCodec.h>
#include <OgreFreeImageCodec.h>
#include <OgreFileSystem.h>
#include <OgreResourceGroupManager.h>

FV_SINGLETON_STORAGE(FvServerResource)

Ogre::LogManager *FvServerResource::ms_pkLogManager = NULL;
Ogre::ResourceGroupManager *FvServerResource::ms_pkResourceGroupManager = NULL;
Ogre::ArchiveManager *FvServerResource::ms_pkArchiveManager = NULL;
Ogre::ZipArchiveFactory *FvServerResource::ms_pkZipArchiveFactory = NULL;
Ogre::FileSystemArchiveFactory *FvServerResource::ms_pkFileSystemArchiveFactory = NULL;

FvServerResource::FvServerResource()
{

}

FvServerResource::~FvServerResource()
{
	Ogre::FreeImageCodec::shutdown();
	Ogre::DDSCodec::shutdown();

	if(ms_pkLogManager)
		delete ms_pkLogManager;
	if(ms_pkResourceGroupManager)
		delete ms_pkResourceGroupManager;
	if(ms_pkArchiveManager)
		delete ms_pkArchiveManager;
	if(ms_pkZipArchiveFactory)
		delete ms_pkZipArchiveFactory;
	if(ms_pkFileSystemArchiveFactory)
		delete ms_pkFileSystemArchiveFactory;
}

void FvServerResource::Purge( const FvString &kResourceID,
		   bool bRecurse,
		   FvXMLSectionPtr spSection )
{

}

void FvServerResource::Init(int iArgc, char *pcArgv[])
{	
	ms_pkLogManager = new Ogre::LogManager;
	ms_pkResourceGroupManager = new Ogre::ResourceGroupManager;
	ms_pkArchiveManager = new Ogre::ArchiveManager;
	ms_pkZipArchiveFactory = new Ogre::ZipArchiveFactory;
	Ogre::ArchiveManager::getSingleton().addArchiveFactory( ms_pkZipArchiveFactory );
	ms_pkFileSystemArchiveFactory = new Ogre::FileSystemArchiveFactory;
	Ogre::ArchiveManager::getSingleton().addArchiveFactory( ms_pkFileSystemArchiveFactory );
#ifdef FV_SHIPPING
	ms_pkResourceGroupManager->addResourceLocation(std::string("../Data"), std::string("FileSystem"));
	ms_pkResourceGroupManager->addResourceLocation(std::string("../ServerData"), std::string("FileSystem"));
#else // FV_SHIPPING
	ms_pkResourceGroupManager->addResourceLocation(std::string("../../Data"),std::string("FileSystem"));
	ms_pkResourceGroupManager->addResourceLocation(std::string("../../ServerData"), std::string("FileSystem"));
#endif // !FV_SHIPPING
	ms_pkResourceGroupManager->initialiseAllResourceGroups();

	Ogre::FreeImageCodec::startup();
	Ogre::DDSCodec::startup();
}

FvXMLSectionPtr FvServerResource::OpenSection(const FvString &kFile, bool bCreate)
{
	Ogre::DataStreamPtr spDataStream;
	try
	{
		spDataStream = ms_pkResourceGroupManager->openResource(kFile);
	}
	catch (Ogre::Exception kExce)
	{
		FV_ERROR_MSG("FvServerResource::OpenSection: "
			"Failed open seciton %s\n", kFile.c_str());
		return FvXMLSectionPtr();
	}

	FvXMLSectionPtr spSection = FvXMLSection::OpenSection(spDataStream);
	if(spSection == NULL && bCreate)
		spSection = FvXMLSection::CreateSection(kFile);

	return spSection;
}

Ogre::DataStreamPtr FvServerResource::OpenDataStream(const FvString &kFile)
{
	Ogre::DataStreamPtr spDataStream;
	try
	{
		spDataStream = ms_pkResourceGroupManager->openResource(kFile);
	}
	catch (Ogre::Exception kExce)
	{
		FV_ERROR_MSG("FvServerResource::OpenSection: "
			"Failed open seciton %s\n", kFile.c_str());
		return Ogre::DataStreamPtr();
	}
	return spDataStream;
}

bool FvServerResource::FileExists( const FvString &kFile )
{
	FV_ASSERT(ms_pkResourceGroupManager);
	return false;
}
