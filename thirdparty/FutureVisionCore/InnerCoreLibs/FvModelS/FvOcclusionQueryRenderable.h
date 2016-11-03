//{future header message}
#ifndef __FvOcclusionQueryRenderable_H__
#define __FvOcclusionQueryRenderable_H__

#include <FvBaseTypes.h>
#include <FvDebug.h>
#include "FvModelDefines.h"

#include <OgreRenderable.h>
#include <OgreMesh.h>
#include <OgreHardwareOcclusionQuery.h>
#include <FvRefList.h>
#include <FvVector4.h>
#include <FvBoundingBox.h>

class FV_MODEL_API FvOcclusionQueryAABB : public Ogre::Renderable
{
public:
	FvOcclusionQueryAABB(const Ogre::MeshPtr& spMesh,
		const Ogre::MaterialPtr& spMaterial);

	virtual ~FvOcclusionQueryAABB();

	virtual const Ogre::MaterialPtr& getMaterial(void) const;

	virtual bool preRender(Ogre::SceneManager* sm, Ogre::RenderSystem* rsys);

	virtual void postRender(Ogre::SceneManager* sm, Ogre::RenderSystem* rsys);

	virtual void getRenderOperation(Ogre::RenderOperation& op);

	virtual void getWorldTransforms(Ogre::Matrix4* xform) const;

	virtual unsigned short getNumWorldTransforms(void);

	virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;

	virtual const Ogre::LightList& getLights(void) const;

	void GetResult();

	bool IsVisible();

	void SetResult(FvUInt32 u32Result);

	void Update(const FvVector3& kScale, const FvVector3& kPosition);

protected:
	Ogre::MeshPtr m_spMesh;
	Ogre::MaterialPtr m_spMaterial;
	Ogre::HardwareOcclusionQuery* m_pkQuery;
	FvVector4 m_kScale;
	FvVector4 m_kPosition;
	FvUInt32 m_u32Result;

};

#endif
