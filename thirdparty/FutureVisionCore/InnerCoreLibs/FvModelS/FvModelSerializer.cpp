#include "FvModelSerializer.h"

#include <FvWatcher.h>
#include <FvDebug.h>

FvModelSerializer::FvModelSerializer()
{
	m_kImplementations.insert(FvModelSerializerImplMap::value_type("node",
		new FvXMLModelSerializerImpl));
}

FvModelSerializer::~FvModelSerializer()
{
	for(FvModelSerializerImplMap::iterator kIt = m_kImplementations.begin();
		kIt != m_kImplementations.end(); ++kIt)
	{
		delete kIt->second;
	}
	m_kImplementations.clear();
}

bool FvModelSerializer::ExportModel(const FvModel *pkModel,const FvString& kFileName,
												const FvString &kImpl)
{
	FvModelSerializerImplMap::iterator kIt = m_kImplementations.find(kImpl);
	if(kIt == m_kImplementations.end())
	{
		FV_ERROR_MSG("Conn't find serializer implementation %s",kImpl.c_str());
		return false;
	}

	return kIt->second->ExportModel(pkModel,kFileName);
}

bool FvModelSerializer::ImportModel(Ogre::DataStreamPtr &kStream, FvModel *pkDest,
												const FvString &kImpl)
{
	FvModelSerializerImplMap::iterator kIt = m_kImplementations.find(kImpl);
	if(kIt == m_kImplementations.end())
	{
		FV_ERROR_MSG("Conn't find serializer implementation %s",kImpl.c_str());
		return false;
	}

	return kIt->second->ImportModel(kStream,pkDest);
}