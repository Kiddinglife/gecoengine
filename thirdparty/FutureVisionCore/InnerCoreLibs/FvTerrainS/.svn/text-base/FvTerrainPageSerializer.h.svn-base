//{future header message}
#ifndef __FvTerrainSerializer_H__
#define __FvTerrainSerializer_H__

#include "FvTerrainSettings.h"
#include "FvTerrainPageSerializerImpl.h"

#include <FvBaseTypes.h>
#include <OgreDataStream.h>

class FvTerrainPage;
class FvTerrainPageSerializerImpl;

class FvTerrainPageSerializer
{
public:
	FvTerrainPageSerializer();
	~FvTerrainPageSerializer();

	bool ExportTerrainPage(const FvTerrainPage *pkTerrainPage,const FvString& kFileName,
		const FvString &kImpl = "ftd");
	bool ImportTerrainPage(Ogre::DataStreamPtr &kStream, FvTerrainPage *pkDest,
		const FvString &kImpl = "ftd");

protected:

	typedef std::map<FvString,FvTerrainPageSerializerImpl*> FvTerrainPageSerializerImplMap;
	FvTerrainPageSerializerImplMap m_kImplementations;
};

#endif // __FvTerrainSerializer_H__