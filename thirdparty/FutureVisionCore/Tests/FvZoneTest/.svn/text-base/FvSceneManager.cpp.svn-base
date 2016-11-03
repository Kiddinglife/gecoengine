#include "FvSceneManager.h"

#include <FvCamera.h>
#include <FvZoneManager.h>

FvSceneManager::FvSceneManager() :
SceneManager("FvSceneManager"),
m_pkTriangleRenderable(0),
m_pkLineRenderable(0)
{
	this->setShadowTechnique(Ogre::SHADOWDETAILTYPE_TEXTURE);
}

FvSceneManager::~FvSceneManager()
{
	delete m_pkTriangleRenderable;
	delete m_pkLineRenderable;
}

const Ogre::String &FvSceneManager::getTypeName() const
{
	return FvSceneManagerFactory::FACTORY_TYPE_NAME;
}

void FvSceneManager::_findVisibleObjects( Ogre::Camera *pkCam, 
								 Ogre::VisibleObjectsBoundsInfo *pkVisibleBounds, bool bOnlyShadowCasters )
{
	if(!m_pkTriangleRenderable)
		m_pkTriangleRenderable = new FvTriangleRenderable;
	if(!m_pkLineRenderable)
		m_pkLineRenderable = new FvLineRenderable;

	FvCamera *pkCamera = static_cast<FvCamera*>(pkCam);
	pkCamera->RemoveAllExtraCullingPlanes();
	pkCamera->Update();

	//FvZoneManager::RenderableFlags eFlags = FvZoneManager::RENDERABLE_NONE;
	//if(bOnlyShadowCasters)
	//	eFlags = FvZoneManager::RENDERABLE_SHADOW_CASTER;

	//FvZoneManager::Instance().FindVisibleObjects(pkCamera,
	//	this->getRenderQueue(),m_kVisibleZones,pkVisibleBounds,eFlags);
	if(m_pkTriangleRenderable && m_pkTriangleRenderable->m_bRenderer)
		this->getRenderQueue()->addRenderable(m_pkTriangleRenderable,RENDER_QUEUE_9);
	if(m_pkLineRenderable && m_pkLineRenderable->m_bRenderer)
		this->getRenderQueue()->addRenderable(m_pkLineRenderable);
}

Camera* FvSceneManager::createCamera(const String& name)
{
	if (mCameras.find(name) != mCameras.end())
	{
		OGRE_EXCEPT(
			Exception::ERR_DUPLICATE_ITEM,
			"A camera with the name " + name + " already exists",
			"SceneManager::createCamera" );
	}

	FvCamera *c = OGRE_NEW FvCamera(name, this);
	mCameras.insert(CameraList::value_type(name, c));

	mCamVisibleObjectsMap[c] = VisibleObjectsBoundsInfo();

	return c;
}

const Ogre::String FvSceneManagerFactory::FACTORY_TYPE_NAME = "FvSceneManager";
Ogre::SceneManager *FvSceneManagerFactory::createInstance(const Ogre::String &kInstanceName)
{
	return OGRE_NEW FvSceneManager;
}

void FvSceneManagerFactory::destroyInstance(Ogre::SceneManager *pkInstance)
{
	OGRE_DELETE pkInstance;
}

void FvSceneManagerFactory::initMetaData() const
{
	mMetaData.typeName = FACTORY_TYPE_NAME;
	mMetaData.description = "Scene manager zone";
	mMetaData.sceneTypeMask = 0xFFFF;
	mMetaData.worldGeometrySupported = false;
}