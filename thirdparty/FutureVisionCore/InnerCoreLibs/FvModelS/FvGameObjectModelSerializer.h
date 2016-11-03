//{future header message}
#ifndef __FvGameObjectModelSerializer_H__
#define __FvGameObjectModelSerializer_H__

#include <FvBaseTypes.h>
#include "FvGameObjectModel.h"
#include <OgreDataStream.h>
#include <FvXMLSection.h>

class FvGameObjectModelSerializerImpl;

class FvGameObjectModelSerializer
{
public:
	FvGameObjectModelSerializer();
	~FvGameObjectModelSerializer();

	bool ExportModel(const FvGameObjectModel *pkModel,const FvString& kFileName,
		const FvString &kImpl = "node");
	bool ImportModel(Ogre::DataStreamPtr &kStream, FvGameObjectModel *pkDest,
		const FvString &kImpl = "node");

protected:

	typedef std::map<FvString,FvGameObjectModelSerializerImpl*> FvGameObjectModelSerializerImplMap;
	FvGameObjectModelSerializerImplMap m_kImplementations;
};

namespace Ogre
{
	class SceneNode;
	class SceneManager;
}

class FV_MODEL_API FvGameObjectModelSerializerImpl
{
public:
	virtual ~FvGameObjectModelSerializerImpl(){}

	virtual bool ExportModel(const FvGameObjectModel *pkModel,const FvString& kFileName) = 0;
	virtual bool ImportModel(Ogre::DataStreamPtr &kStream, FvGameObjectModel *pkDest) = 0;
};


class FvXMLGameObjectModelSerializerImpl : public FvGameObjectModelSerializerImpl
{
public:
	FvXMLGameObjectModelSerializerImpl();
	~FvXMLGameObjectModelSerializerImpl();

	virtual bool ExportModel(const FvGameObjectModel *pkModel, const FvString& kFileName);
	virtual bool ImportModel(Ogre::DataStreamPtr &kStream, FvGameObjectModel *pkDest);

private:

	void ReadNode(FvXMLSectionPtr spSection,SceneNode *pkParent,FvGameObjectModel *pkDest);
	void ReadEntity(FvXMLSectionPtr spSection,Ogre::SceneNode *pkNode,FvGameObjectModel *pkDest);
	void ReadAnimation(FvXMLSectionPtr spSection,Ogre::SceneNode *pkNode,FvGameObjectModel *pkDest);

	void ReadCollision(FvXMLSectionPtr spSection,FvGameObjectModel *pkDest);
};

#endif // __FvGameObjectModelSerializer_H__
