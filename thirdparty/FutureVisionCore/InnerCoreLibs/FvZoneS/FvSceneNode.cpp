#include "FvSceneNode.h"

#include <OgreSceneManager.h>

FvSceneNode::FvSceneNode( Ogre::SceneManager *pkCreator ): 
SceneNode( pkCreator ),
m_pkZone(NULL)
{

}

FvSceneNode::FvSceneNode( Ogre::SceneManager *pkCreator, const Ogre::String &kName ): 
SceneNode( pkCreator, kName ),
m_pkZone(NULL)
{

}

FvSceneNode::~FvSceneNode()
{
	m_pkZone = NULL;
}

Ogre::SceneNode* FvSceneNode::createChildSceneNode(
	const Ogre::Vector3 &kTranslate, 
	const Ogre::Quaternion &kRotate )
{
	FvSceneNode *pkChildSceneNode = (FvSceneNode*)(this->createChild(kTranslate, kRotate));
	pkChildSceneNode->SetZone(m_pkZone);
	return static_cast<SceneNode*>(pkChildSceneNode);
}

Ogre::SceneNode* FvSceneNode::createChildSceneNode(const Ogre::String &kName, 
												   const Ogre::Vector3 &kTranslate, 
												   const Ogre::Quaternion &kRotate)
{
	FvSceneNode *pkChildSceneNode = (FvSceneNode*)(this->createChild(kName, kTranslate, kRotate));
	pkChildSceneNode->SetZone(m_pkZone);
	return static_cast<SceneNode*>(pkChildSceneNode);
}

void FvSceneNode::SetZone(FvZone *pkZone, bool bChild)
{
	m_pkZone = pkZone;
	if(bChild)
	{
		ChildNodeMap::iterator kChild, kChildend;
		kChildend = mChildren.end();
		for (kChild = mChildren.begin(); kChild != kChildend; ++kChild)
		{
			FvSceneNode *pkSceneChild = static_cast<FvSceneNode*>(kChild->second);
			pkSceneChild->SetZone(pkZone);
		}
	}
}
