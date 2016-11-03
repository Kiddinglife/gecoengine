#ifndef __FVANIMATIONMODELSERIALIZER_H__
#define __FVANIMATIONMODELSERIALIZER_H__


#include <FvBaseTypes.h>
#include <OgreDataStream.h>
#include <FvXMLSection.h>
#include "FvAnimationModel.h"

class FvAnimationModelSerializerImpl;

class FvAnimationModelSerializer
{
public:
	FvAnimationModelSerializer();
	~FvAnimationModelSerializer();

public:
	bool ExportModel(const FvAnimationModel* pkModel, const FvString& kFileName,
		const FvString& kImpl = "node");
	bool ImportModel(Ogre::DataStreamPtr& kStream, FvAnimationModel* pkDest,
		const FvString& kImpl = "node");

protected:
	typedef std::map<FvString, FvAnimationModelSerializerImpl*> FvAnimationModelSerializerImplMap;
	FvAnimationModelSerializerImplMap m_kImplementations;
};

namespace Ogre
{
	class SceneNode;
	class SceneManager;
}

class FV_MODEL_API FvAnimationModelSerializerImpl
{
public:
	virtual ~FvAnimationModelSerializerImpl(){}
	
	virtual bool ExportModel(const FvAnimationModel* pkModel, const FvString& kFileName) = 0;
	virtual bool ImportModel(Ogre::DataStreamPtr& kStream, FvAnimationModel* pkDest) = 0;
};

class FvXMLAnimationModelSerializerImpl : public FvAnimationModelSerializerImpl
{
public:
	FvXMLAnimationModelSerializerImpl();
	~FvXMLAnimationModelSerializerImpl();

	virtual bool ExportModel(const FvAnimationModel* pkModel, const FvString& kFileName) ;
	virtual bool ImportModel(Ogre::DataStreamPtr& kStream, FvAnimationModel* pkDest) ;

private:
	void ReadNode(FvXMLSectionPtr spSection, SceneNode* pkParent, FvAnimationModel* pkDest);
	void ReadEntity(FvXMLSectionPtr spSection, Ogre::SceneNode* pkNode, FvAnimationModel* pkDest);
	void ReadAnimation(FvXMLSectionPtr spSection, Ogre::SceneNode* pkNode, FvAnimationModel* pkDest);

	void ReadCollision(FvXMLSectionPtr spSection, FvAnimationModel* pkDest);
};
#endif //__FVANIMATIONMODELSERIALIZER_H__