#include "FvTerrainPageSerializer.h"
#include <FvWatcher.h>
#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvTerrain", 0 )

FvTerrainPageSerializer::FvTerrainPageSerializer()
{
	m_kImplementations.insert(FvTerrainPageSerializerImplMap::value_type("ftd",
		new FvFTDTerrainPageSerializerImpl));
}

FvTerrainPageSerializer::~FvTerrainPageSerializer()
{
	for(FvTerrainPageSerializerImplMap::iterator kIt = m_kImplementations.begin();
		kIt != m_kImplementations.end(); ++kIt)
	{
		delete kIt->second;
	}
	m_kImplementations.clear();
}

bool FvTerrainPageSerializer::ExportTerrainPage(const FvTerrainPage *pkTerrainPage,const FvString& kFileName,
												const FvString &kImpl)
{
	FvTerrainPageSerializerImplMap::iterator kIt = m_kImplementations.find(kImpl);
	if(kIt == m_kImplementations.end())
	{
		FV_ERROR_MSG("Conn't find serializer implementation %s",kImpl.c_str());
		return false;
	}

	return kIt->second->ExportTerrainPage(pkTerrainPage,kFileName);
}

bool FvTerrainPageSerializer::ImportTerrainPage(Ogre::DataStreamPtr &kStream, FvTerrainPage *pkDest,
												const FvString &kImpl)
{
	FvTerrainPageSerializerImplMap::iterator kIt = m_kImplementations.find(kImpl);
	if(kIt == m_kImplementations.end())
	{
		FV_ERROR_MSG("Conn't find serializer implementation %s",kImpl.c_str());
		return false;
	}

	return kIt->second->ImportTerrainPage(kStream,pkDest);
}