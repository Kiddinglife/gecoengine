//{future header message}
#ifndef __FvServerResource_H__
#define __FvServerResource_H__

#include <FvSingleton.h>
#include <FvXMLSection.h>

#include "FvServerCommon.h"

namespace Ogre
{
	class LogManager;
	class ResourceGroupManager;
	class ArchiveManager;
	class ZipArchiveFactory;
	class FileSystemArchiveFactory;
}

class FV_SERVERCOMMON_API FvServerResource : public FvSingleton<FvServerResource>
{
public:
	FvServerResource();
	~FvServerResource();

	void Purge( const FvString &kResourceID,
		bool bRecurse = false,
		FvXMLSectionPtr spSection = NULL );

	static void Init(int iArgc, char *pcArgv[]);

	static FvXMLSectionPtr OpenSection(const FvString &kFile, bool bCreate = false);
	static Ogre::DataStreamPtr OpenDataStream(const FvString &kFile);

	static bool	FileExists( const FvString &kFile );

protected:

	static Ogre::LogManager *ms_pkLogManager;

	static Ogre::ResourceGroupManager *ms_pkResourceGroupManager;

	static Ogre::ArchiveManager *ms_pkArchiveManager;

	static Ogre::ZipArchiveFactory *ms_pkZipArchiveFactory;

	static Ogre::FileSystemArchiveFactory *ms_pkFileSystemArchiveFactory;
};

#endif // __FvServerResource_H__