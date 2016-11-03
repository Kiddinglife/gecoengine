//{future header message}
#ifndef __FvModelSerializer_H__
#define __FvModelSerializer_H__

#include <FvBaseTypes.h>

#include <OgreDataStream.h>

#include "FvModelSerializerImpl.h"

class FvModel;
class FvModelSerializerImpl;

class FvModelSerializer
{
public:
	FvModelSerializer();
	~FvModelSerializer();

	bool ExportModel(const FvModel *pkModel,const FvString& kFileName,
		const FvString &kImpl = "node");
	bool ImportModel(Ogre::DataStreamPtr &kStream, FvModel *pkDest,
		const FvString &kImpl = "node");

protected:

	typedef std::map<FvString,FvModelSerializerImpl*> FvModelSerializerImplMap;
	FvModelSerializerImplMap m_kImplementations;
};

#endif // __FvModelSerializer_H__