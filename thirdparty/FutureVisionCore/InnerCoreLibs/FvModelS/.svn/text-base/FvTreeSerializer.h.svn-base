//{future header message}
#ifndef __FvTreeSerializer_H__
#define __FvTreeSerializer_H__

#include <FvBaseTypes.h>
#include <FvXMLSection.h>
#include <OgreDataStream.h>
#include "FvModelDefines.h"

class FvTree;
namespace Ogre
{
	class SceneNode;
	class SceneManager;
}

class FV_MODEL_API FvTreeSerializerImpl
{
public:
	virtual ~FvTreeSerializerImpl(){}

	virtual bool ExportTree(const FvTree *pkTree,const FvString& kFileName) = 0;
	virtual bool ImportTree(Ogre::DataStreamPtr &kStream, FvTree *pkDest) = 0;
};

class FvXMLTreeSerializerImpl : public FvTreeSerializerImpl
{
public:
	FvXMLTreeSerializerImpl();
	~FvXMLTreeSerializerImpl();

	virtual bool ExportTree(const FvTree *pkTree, const FvString& kFileName);
	virtual bool ImportTree(Ogre::DataStreamPtr &kStream, FvTree *pkDest);

private:

	void ReadNode(FvXMLSectionPtr spSection,Ogre::SceneNode *pkParent,FvTree *pkDest);
	void ReadEntity(FvXMLSectionPtr spSection,Ogre::SceneNode *pkNode,FvTree *pkDest);

	void ReadCollision(FvXMLSectionPtr spSection,FvTree *pkDest);
};

class FV_MODEL_API FvTreeSerializer
{
public:
	FvTreeSerializer();
	~FvTreeSerializer();

	bool ExportTree(const FvTree *pkTree,const FvString& kFileName,
		const FvString &kImpl = "node");
	bool ImportTree(Ogre::DataStreamPtr &kStream, FvTree *pkDest,
		const FvString &kImpl = "node");

protected:
	typedef std::map<FvString,FvTreeSerializerImpl*> FvTreeSerializerImplMap;
	FvTreeSerializerImplMap m_kImplementations;
	
};

#endif // __FvTreeSerializer_H__