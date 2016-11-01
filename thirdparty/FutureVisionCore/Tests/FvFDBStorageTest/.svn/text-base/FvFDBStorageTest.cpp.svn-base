#include "FvFDBStorageManager.h"
#include <stdio.h>
#include <vld.h>
#include "PlayerInfo.h"

#include <OgreLogManager.h>
#include <OgreArchiveManager.h>
#include <OgreFileSystem.h>

using namespace Ogre;

void main()
{
	Ogre::LogManager *pkLog = OGRE_NEW Ogre::LogManager;
	Ogre::ArchiveManager *pkArchive = OGRE_NEW Ogre::ArchiveManager;
	pkArchive->addArchiveFactory(OGRE_NEW Ogre::FileSystemArchiveFactory);
	Ogre::ResourceGroupManager *pkGroupManager = OGRE_NEW Ogre::ResourceGroupManager;
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./","FileSystem");
	FvFDBStorageManager::Create();

	FvFDBStoragePtr spStorage = FvFDBStorageManager::getSingleton().load("PlayerInfo.fdb");
	PlayerInfo const*pkPlayerInfo = spStorage->LookupEntry<PlayerInfo>(1);
	pkPlayerInfo = spStorage->LookupEntry<PlayerInfo>(int(2));
	pkPlayerInfo = spStorage->LookupEntry<PlayerInfo>(int(3));
	pkPlayerInfo = spStorage->LookupEntry<PlayerInfo>(int(4));
	pkPlayerInfo = spStorage->LookupEntry<PlayerInfo>(int(5));

	pkPlayerInfo = spStorage->LookupEntry<PlayerInfo>("aaaa",3);
	pkPlayerInfo = spStorage->LookupEntry<PlayerInfo>(L"¹þ¹þ´ó¼ÒºÃ2",4);
	
	spStorage.setNull();
	FvFDBStorageManager::getSingleton().removeAll();
	FvFDBStorageManager::Destory();
	OGRE_DELETE pkGroupManager;
	OGRE_DELETE pkArchive;
	OGRE_DELETE pkLog;
}