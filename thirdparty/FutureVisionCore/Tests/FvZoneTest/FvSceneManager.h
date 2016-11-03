//{future header message}
#ifndef __FvSceneManager_H__
#define __FvSceneManager_H__

#include <Ogre.h>
#include <FvVector3.h>
#include <FvWorldTriangle.h>
#include <FvZoneObstacle.h>

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

class FvTestZoneCallback: public FvCollisionCallback
{
public:

	virtual int operator()( const FvZoneObstacle &kObstacle,
		const FvWorldTriangle & kTriangle, float fDist )
	{
		m_kMatrix = kObstacle.m_kTransform;
		m_kTriangle = kTriangle;
		m_fDist = fDist;
		return COLLIDE_BEFORE;
	}
	FvMatrix m_kInvMatrix;
	FvMatrix m_kMatrix;
	FvWorldTriangle m_kTriangle;
	float m_fDist;
};

class FvSceneManager : public Ogre::SceneManager
{
public:
	FvSceneManager();
	~FvSceneManager();

	const Ogre::String &getTypeName() const;

	virtual void _findVisibleObjects( Ogre::Camera *pkCam, 
		Ogre::VisibleObjectsBoundsInfo *pkVisibleBounds, bool bOnlyShadowCasters );

    virtual Camera* createCamera(const String& name);

	FvTriangleRenderable *m_pkTriangleRenderable;
	FvLineRenderable *m_pkLineRenderable;
};

class FvSceneManagerFactory : public Ogre::SceneManagerFactory
{
public:
	FvSceneManagerFactory(){}
	~FvSceneManagerFactory(){}

	static const Ogre::String FACTORY_TYPE_NAME;
	Ogre::SceneManager *createInstance(const Ogre::String &kInstanceName);
	void destroyInstance(Ogre::SceneManager *pkInstance);

protected:

	void initMetaData() const;
};

#endif // __FvSceneManager_H__