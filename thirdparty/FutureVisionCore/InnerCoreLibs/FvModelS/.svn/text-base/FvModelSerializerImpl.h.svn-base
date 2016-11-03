//{future header message}
#ifndef __FvModelSerializerImpl_H__
#define __FvModelSerializerImpl_H__

#include <FvBaseTypes.h>
#include <FvXMLSection.h>

#include <OgreDataStream.h>

#include "FvModelDefines.h"
#include "FvModel.h"


class FvModel;
namespace Ogre
{
	class SceneNode;
	class SceneManager;
}

class FV_MODEL_API FvModelSerializerImpl
{
public:
	virtual ~FvModelSerializerImpl(){}

	virtual bool ExportModel(const FvModel *pkModel,const FvString& kFileName) = 0;
	virtual bool ImportModel(Ogre::DataStreamPtr &kStream, FvModel *pkDest) = 0;
};

class FvXMLModelSerializerImpl : public FvModelSerializerImpl
{
public:
	FvXMLModelSerializerImpl();
	~FvXMLModelSerializerImpl();

	virtual bool ExportModel(const FvModel *pkModel, const FvString& kFileName);
	virtual bool ImportModel(Ogre::DataStreamPtr &kStream, FvModel *pkDest);

private:

	void ReadNode(FvXMLSectionPtr spSection,SceneNode *pkParent,FvModel *pkDest);
	void ReadEntity(FvXMLSectionPtr spSection,Ogre::SceneNode *pkNode,FvModel *pkDest);
	void ReadAnimation(FvXMLSectionPtr spSection,Ogre::SceneNode *pkNode,FvModel *pkDest);

	void ReadCollision(FvXMLSectionPtr spSection,FvModel *pkDest);
};

#endif // __FvModelSerializerImpl_H__