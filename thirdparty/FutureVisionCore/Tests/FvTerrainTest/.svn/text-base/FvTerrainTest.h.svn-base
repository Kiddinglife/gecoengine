//{future header message}
#ifndef __FvTerrainTest_H__
#define __FvTerrainTest_H__

#include "Ogre.h"
#include "OgreWireBoundingBox.h"
#include "OgreSceneManager.h"

#include "FvTerrainPage.h"
#include "FvTerrainRenderable.h"
#include "FvTerrainLodController.h"

using namespace Ogre;

#define POSITION_BINDING 0
#define TEXCOORD_BINDING 1
class FvLineRenderable : public SimpleRenderable
{
public:
	FvLineRenderable() :
	  m_bRenderer(false)
	  {
		  m_kMatrix = Matrix4::IDENTITY;

		  mRenderOp.vertexData = OGRE_NEW VertexData();

		  mRenderOp.indexData = 0;
		  mRenderOp.vertexData->vertexCount = 2; 
		  mRenderOp.vertexData->vertexStart = 0; 
		  mRenderOp.operationType = RenderOperation::OT_LINE_LIST; 
		  mRenderOp.useIndexes = false; 

		  VertexDeclaration* decl = mRenderOp.vertexData->vertexDeclaration;
		  VertexBufferBinding* bind = mRenderOp.vertexData->vertexBufferBinding;

		  decl->addElement(POSITION_BINDING, 0, VET_FLOAT3, VES_POSITION);

		  HardwareVertexBufferSharedPtr vbuf = 
			  HardwareBufferManager::getSingleton().createVertexBuffer(
			  decl->getVertexSize(POSITION_BINDING),
			  mRenderOp.vertexData->vertexCount,
			  HardwareBuffer::HBU_STATIC_WRITE_ONLY);
		  bind->setBinding(POSITION_BINDING, vbuf);
	  }
	  ~FvLineRenderable()
	  {

	  }

	  void SetLine(FvVector3 kStart,FvVector3 kEnd)
	  {
		  HardwareVertexBufferSharedPtr vbuf = 
			  mRenderOp.vertexData->vertexBufferBinding->getBuffer(POSITION_BINDING);
		  float* pFloat = static_cast<float*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

		  *pFloat++ = kStart.x;
		  *pFloat++ = kStart.y;
		  *pFloat++ = kStart.z;

		  *pFloat++ = kEnd.x;
		  *pFloat++ = kEnd.y;
		  *pFloat++ = kEnd.z;		

		  vbuf->unlock();

		  mBox.merge(*(Vector3*)&kStart);
		  mBox.merge(*(Vector3*)&kEnd);
	  }

	  void getWorldTransforms( Matrix4* xform ) const
	  {
		  *xform = m_kMatrix;
	  }
	  Real getSquaredViewDepth(const Camera* cam) const { return 0; }
	  Real getBoundingRadius(void) const { return 0; }
	  Matrix4 m_kMatrix;
	  bool m_bRenderer;
};
class FvTriangleRenderable : public SimpleRenderable
{
public:
	FvTriangleRenderable():
	  m_bRenderer(false)
	  {       
		  m_kMatrix = Matrix4::IDENTITY;

		  mRenderOp.vertexData = OGRE_NEW VertexData();

		  mRenderOp.indexData = 0;
		  mRenderOp.vertexData->vertexCount = 3; 
		  mRenderOp.vertexData->vertexStart = 0; 
		  mRenderOp.operationType = RenderOperation::OT_TRIANGLE_LIST; 
		  mRenderOp.useIndexes = false; 

		  VertexDeclaration* decl = mRenderOp.vertexData->vertexDeclaration;
		  VertexBufferBinding* bind = mRenderOp.vertexData->vertexBufferBinding;

		  decl->addElement(POSITION_BINDING, 0, VET_FLOAT3, VES_POSITION);
		  decl->addElement(TEXCOORD_BINDING, 0, VET_FLOAT2, VES_TEXTURE_COORDINATES);

		  HardwareVertexBufferSharedPtr vbuf = 
			  HardwareBufferManager::getSingleton().createVertexBuffer(
			  decl->getVertexSize(POSITION_BINDING),
			  mRenderOp.vertexData->vertexCount,
			  HardwareBuffer::HBU_STATIC_WRITE_ONLY);
		  bind->setBinding(POSITION_BINDING, vbuf);


		  HardwareVertexBufferSharedPtr tvbuf = 
			  HardwareBufferManager::getSingleton().createVertexBuffer(
			  decl->getVertexSize(TEXCOORD_BINDING),
			  mRenderOp.vertexData->vertexCount,
			  HardwareBuffer::HBU_STATIC_WRITE_ONLY);
		  bind->setBinding(TEXCOORD_BINDING, tvbuf);

		  float* pTex = static_cast<float*>(
			  tvbuf->lock(HardwareBuffer::HBL_DISCARD));
		  *pTex++ = 0.0f;
		  *pTex++ = 0.0f;
		  *pTex++ = 0.0f;
		  *pTex++ = 1.0f;
		  *pTex++ = 1.0f;
		  *pTex++ = 0.0f;
		  tvbuf->unlock();

		  MaterialPtr spMaterial = MaterialManager::getSingleton().getByName("BaseWhiteNoLighting");
		  spMaterial->load();
		  m_pMaterial = spMaterial->clone("FvTriangleMaterial");		
		  m_pMaterial->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
		  m_pMaterial->getTechnique(0)->getPass(0)->setPolygonMode(PM_WIREFRAME);
		  //m_pMaterial->setDepthFunction();
	  }

	  ~FvTriangleRenderable()
	  {

	  }

	  void SetTriangle(FvWorldTriangle kTriangle)
	  {
		  HardwareVertexBufferSharedPtr vbuf = 
			  mRenderOp.vertexData->vertexBufferBinding->getBuffer(POSITION_BINDING);
		  float* pFloat = static_cast<float*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

		  *pFloat++ = kTriangle.Point0().x;
		  *pFloat++ = kTriangle.Point0().y;
		  *pFloat++ = kTriangle.Point0().z;

		  *pFloat++ = kTriangle.Point1().x;
		  *pFloat++ = kTriangle.Point1().y;
		  *pFloat++ = kTriangle.Point1().z;		

		  *pFloat++ = kTriangle.Point2().x;
		  *pFloat++ = kTriangle.Point2().y;
		  *pFloat++ = kTriangle.Point2().z;

		  vbuf->unlock();

		  mBox.merge(*(Vector3*)&kTriangle.Point0());
		  mBox.merge(*(Vector3*)&kTriangle.Point1());
		  mBox.merge(*(Vector3*)&kTriangle.Point2());	
	  }

	  void getWorldTransforms( Matrix4* xform ) const
	  {
		  *xform = m_kMatrix;
	  }
	  Real getSquaredViewDepth(const Camera* cam) const { return 0; }
	  Real getBoundingRadius(void) const { return 0; }
	  Matrix4 m_kMatrix;
	  bool m_bRenderer;
};

class FvTestTerrainCallback : public FvTerrainCollisionCallback
{
public:
	FvTestTerrainCallback()
	{

	}
	bool Collide(FvWorldTriangle const &kTriangle,float fTValue)
	{
		m_kTriangle = kTriangle;
		return true;
	}	
	FvWorldTriangle m_kTriangle;
	float m_fValue;
};

#define TEST_GRID_SIZE 10

class FvTerrainTestManager : public SceneManager
{
public:
	FvTerrainTestManager() : SceneManager("FvTerrainTestManager"),
	m_spTerrainSetting(new FvTerrainSettings),
	m_bLoaded(false)
	{
		m_kType = "FvTerrainTestManager";
	}

	~FvTerrainTestManager()
	{
		delete m_pkTriangleRenderable;
		delete m_pkLineRenderable;
	}

	const Ogre::String &getTypeName() const
	{
		return m_kType;
	}

	virtual void _updateSceneGraph( Ogre::Camera *pkCam )
	{
		SceneManager::_updateSceneGraph(pkCam);

		FvVector3 kPos(pkCam->getRealPosition().x,pkCam->getRealPosition().y,pkCam->getRealPosition().z);
		FvTerrainLodController::Instance().NotifyCameraPosition(kPos);
	}

	virtual void _findVisibleObjects( Ogre::Camera *pkCam, 
		Ogre::VisibleObjectsBoundsInfo *pkVisibleBounds, bool bOnlyShadowCasters )
	{
		if(!m_bLoaded)
		{
			int i = TEST_GRID_SIZE/2;
			int j = TEST_GRID_SIZE/2;
			//for(int i = 0; i < TEST_GRID_SIZE;i++)
			//{
			//	for(int j = 0; j < TEST_GRID_SIZE;j++)
			//	{
					char pcTerrainFileCStr[64];
					m_kTerrainPage[i][j] = new FvTerrainPage(m_spTerrainSetting);
					FvVector3 kPos0((i-TEST_GRID_SIZE/2)*100.f,(j-TEST_GRID_SIZE/2)*100.f,0.f);
					sprintf_s( pcTerrainFileCStr, sizeof(pcTerrainFileCStr), "%04x%04xo.ftd", 
						int(FvUInt16(i-TEST_GRID_SIZE/2)), int(FvUInt16(j-TEST_GRID_SIZE/2)));
					m_kTerrainPage[i][j]->Load(pcTerrainFileCStr,"General",kPos0,FvVector3::ZERO);
					//FvTerrainLodController::Instance().AddPage(m_kTerrainPage[i][j].Get());
			//	}
			//}
			m_pkTriangleRenderable = new FvTriangleRenderable;
			m_pkLineRenderable = new FvLineRenderable;
			m_bLoaded = true;
		}
		int x = TEST_GRID_SIZE/2;
		int y = TEST_GRID_SIZE/2;
		//for(int x = 0; x < TEST_GRID_SIZE;x++)
		//{
		//	for(int y = 0; y < TEST_GRID_SIZE;y++)
		//	{
				if(m_kTerrainPage[x][y])
				{		
					if(!m_kTerrainPage[x][y]->IsInLodControl())
						FvTerrainLodController::Instance().AddPage(m_kTerrainPage[x][y].GetObject());

					for(FvUInt32 i = 0; i < m_spTerrainSetting->NumTitles()*m_spTerrainSetting->NumTitles() ; i++)
					{
						FvTerrainRenderable *pkRenderable = m_kTerrainPage[x][y]->GetRenderable(i);		

						//Ogre::AxisAlignedBox kBB(
						//	pkRenderable->BoundingBox().MinBounds().x,
						//	pkRenderable->BoundingBox().MinBounds().y,
						//	pkRenderable->BoundingBox().MinBounds().z,
						//	pkRenderable->BoundingBox().MaxBounds().x,
						//	pkRenderable->BoundingBox().MaxBounds().y,
						//	pkRenderable->BoundingBox().MaxBounds().z
						//	);
						//if(pkCam->isVisible(kBB))
						//{	
							getRenderQueue()->addRenderable(pkRenderable);
						//}
						//getRenderQueue()->addRenderable(pkRenderable->GetWireBB());
					}
					//getRenderQueue()->addRenderable(m_kTerrainPage[x][y]->GetWireBB());
				}
		//	}
		//}

		getRootSceneNode()->_findVisibleObjects(pkCam, getRenderQueue(), pkVisibleBounds, true, 
			mDisplayNodes, bOnlyShadowCasters);

		if(m_pkTriangleRenderable && m_pkTriangleRenderable->m_bRenderer)
			this->getRenderQueue()->addRenderable(m_pkTriangleRenderable,RENDER_QUEUE_9);
		if(m_pkLineRenderable && m_pkLineRenderable->m_bRenderer)
			this->getRenderQueue()->addRenderable(m_pkLineRenderable);
	}

	void RemovePage(int x,int y)
	{
		if(x < TEST_GRID_SIZE && y < TEST_GRID_SIZE)
			m_kTerrainPage[x][y] = NULL;
	}

	FvTerrainSettingsPtr m_spTerrainSetting;
	FvTerrainPagePtr m_kTerrainPage[TEST_GRID_SIZE][TEST_GRID_SIZE];
	bool m_bLoaded;
	FvString m_kType;
	FvTriangleRenderable *m_pkTriangleRenderable;
	FvLineRenderable *m_pkLineRenderable;
};

class FvTerrainTestManagerFactory : public Ogre::SceneManagerFactory
{
public:
	FvTerrainTestManagerFactory(){}
	~FvTerrainTestManagerFactory(){}

	Ogre::SceneManager *createInstance(const Ogre::String &kInstanceName)
	{
		return OGRE_NEW FvTerrainTestManager;
	}

	void destroyInstance(Ogre::SceneManager *pkInstance)
	{
		OGRE_DELETE pkInstance;
	}

protected:

	void initMetaData() const
	{
		mMetaData.typeName = "FvTerrainTestManager";
		mMetaData.description = "Scene manager terrain test";
		mMetaData.sceneTypeMask = ST_INTERIOR;
		mMetaData.worldGeometrySupported = false;
	}
};

#endif // __FvTerrainTest_H__
