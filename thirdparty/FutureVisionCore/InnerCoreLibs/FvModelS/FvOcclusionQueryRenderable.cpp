#include "FvOcclusionQueryRenderable.h"
#include <OgreSceneManager.h>
#include <OgreRoot.h>
#include <OgreRenderSystem.h>
#include <OgreD3D9RenderSystem.h>
#include <OgreSubMesh.h>
#include <d3d9.h>

//----------------------------------------------------------------------------
FvOcclusionQueryAABB::FvOcclusionQueryAABB(const Ogre::MeshPtr& spMesh,
	const Ogre::MaterialPtr& spMaterial)
{
	m_spMesh = spMesh;
	m_spMaterial = spMaterial;

	Ogre::RenderSystem* pkRenderSystem =
		Ogre::Root::getSingleton().getRenderSystem();

	FV_ASSERT(pkRenderSystem);

	m_pkQuery = pkRenderSystem->createHardwareOcclusionQuery();
	FV_ASSERT(m_pkQuery);

	m_u32Result = 1;
}
//----------------------------------------------------------------------------
FvOcclusionQueryAABB::~FvOcclusionQueryAABB()
{
	Ogre::RenderSystem* pkRenderSystem =
		Ogre::Root::getSingleton().getRenderSystem();

	pkRenderSystem->destroyHardwareOcclusionQuery(m_pkQuery);
}
//----------------------------------------------------------------------------
const Ogre::MaterialPtr& FvOcclusionQueryAABB::getMaterial( void ) const
{
	return m_spMaterial;
}
//----------------------------------------------------------------------------
bool FvOcclusionQueryAABB::preRender(Ogre::SceneManager* sm,
	Ogre::RenderSystem* rsys)
{
	m_pkQuery->beginOcclusionQuery();

	LPDIRECT3DDEVICE9 lpD3DDevice = ((Ogre::D3D9RenderSystem*)rsys)->getDevice();

	lpD3DDevice->SetVertexShaderConstantF(4, (float*)&m_kScale, 2);

	return true;
}
//----------------------------------------------------------------------------
void FvOcclusionQueryAABB::postRender(Ogre::SceneManager* sm,
	Ogre::RenderSystem* rsys)
{
	m_pkQuery->endOcclusionQuery();
}
//----------------------------------------------------------------------------
void FvOcclusionQueryAABB::getRenderOperation(Ogre::RenderOperation& op)
{
	m_spMesh->getSubMesh(0)->_getRenderOperation(op);
}
//----------------------------------------------------------------------------
void FvOcclusionQueryAABB::getWorldTransforms(Ogre::Matrix4* xform)  const
{
	*xform = Ogre::Matrix4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
}
//----------------------------------------------------------------------------
unsigned short FvOcclusionQueryAABB::getNumWorldTransforms( void )
{
	return 1;
}
//----------------------------------------------------------------------------
Ogre::Real FvOcclusionQueryAABB::getSquaredViewDepth(
	const Ogre::Camera* cam) const
{
	return 0;
}
//----------------------------------------------------------------------------
const Ogre::LightList& FvOcclusionQueryAABB::getLights( void ) const
{
	static Ogre::LightList s_kLightList;
	return s_kLightList;
}
//----------------------------------------------------------------------------
void FvOcclusionQueryAABB::GetResult()
{
	m_pkQuery->pullOcclusionQuery(&m_u32Result);
}
//----------------------------------------------------------------------------
bool FvOcclusionQueryAABB::IsVisible()
{
	return m_u32Result > 0;
}
//----------------------------------------------------------------------------
void FvOcclusionQueryAABB::Update(const FvVector3& kScale,
	const FvVector3& kPosition)
{
	*(FvVector3*)&m_kScale = kScale + FvVector3(5.0f, 5.0f, 10.0f);
	*(FvVector3*)&m_kPosition = kPosition;
}
//----------------------------------------------------------------------------
void FvOcclusionQueryAABB::SetResult(FvUInt32 u32Result)
{
	m_u32Result = u32Result;
}
//----------------------------------------------------------------------------
