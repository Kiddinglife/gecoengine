//{future header message}
#ifndef __FvSceneNode_H__
#define __FvSceneNode_H__

#include <FvPowerDefines.h>

#include "FvZoneDefine.h"

#include <OgreSceneNode.h>
#include <OgreSceneManager.h>

class FvZone;

class FV_ZONE_API FvSceneNode : public Ogre::SceneNode
{
public:
	FvSceneNode( Ogre::SceneManager *pkCreator );
	FvSceneNode( Ogre::SceneManager *pkCreator, const Ogre::String &kName );
	~FvSceneNode();

	virtual Ogre::SceneNode* createChildSceneNode(
		const Ogre::Vector3 &kTranslate = Ogre::Vector3::ZERO, 
		const Ogre::Quaternion &kRotate = Ogre::Quaternion::IDENTITY );

	virtual Ogre::SceneNode* createChildSceneNode(const Ogre::String &kName, 
		const Ogre::Vector3 &kTranslate = Ogre::Vector3::ZERO, 
		const Ogre::Quaternion &kRotate = Ogre::Quaternion::IDENTITY);

	void SetZone(FvZone *pkZone, bool bChild = false);
	FvZone *GetZone(){ return m_pkZone; }

protected:

	FvZone *m_pkZone;
};

#endif // __FvSceneNode_H__