//{future header message}
#ifndef __FvInstanceEntity_H__
#define __FvInstanceEntity_H__

#include <FvBaseTypes.h>
#include <FvDebug.h>
#include "FvModelDefines.h"

#include <OgreEntity.h>
#include <OgreRenderable.h>
#include <OgreMesh.h>
#include <FvRefList.h>
#include <FvStringMap.h>
#include <FvSmartPointer.h>

#ifndef FV_SERVER

using namespace Ogre;

class FvInstanceEntity;

class FV_MODEL_API FvInstanceEntityFactory : public Ogre::MovableObjectFactory
{
public:
	FvInstanceEntityFactory() {}
	~FvInstanceEntityFactory() {}

	static Ogre::String FACTORY_TYPE_NAME;

	const Ogre::String& getType() const;
	void destroyInstance(Ogre::MovableObject* obj);

	

protected:
	Ogre::MovableObject* createInstanceImpl(
		const Ogre::String& name, const Ogre::NameValuePairList* params);
};

class FvInstanceRenderablePool;

class FV_MODEL_API FvInstanceRenderable : public Ogre::Renderable,
	public FvRefNode1<FvInstanceRenderable*>
{
public:
	FvInstanceRenderable(FvInstanceRenderablePool* pkParent,
		FvUInt32 u32MaxInstanceNumber);

	virtual ~FvInstanceRenderable();

	virtual const MaterialPtr& getMaterial(void) const;

	virtual Technique* getTechnique(void) const;

	virtual bool preRender(SceneManager* sm, RenderSystem* rsys);

	virtual void getRenderOperation(RenderOperation& op);

	virtual void getWorldTransforms(Matrix4* xform) const;

	virtual unsigned short getNumWorldTransforms(void);

	virtual Real getSquaredViewDepth(const Camera* cam) const;

	virtual const LightList& getLights(void) const;

	FvUInt32 AddInstance(const Matrix4* xform);

	FvUInt32 GetUseNumber();

	void Reset();

	void DirtyDump();

	void Dump(RenderQueue* queue);

protected:
	FvInstanceRenderablePool* m_pkParent;
	float* m_fpTransformBuffer;
	FvUInt32 m_u32Available;
	FvUInt32 m_u32UseNumber;
	Ogre::VertexBufferBinding* m_pkOverrideBinding;
	HardwareVertexBufferSharedPtr m_spInstanceRenderBuffer;
	bool m_bNeedDump;

};

class FV_MODEL_API FvInstanceRenderablePool : public FvSafeReferenceCount,
	public FvRefNode1<FvInstanceRenderablePool*>
{
public:
	FvInstanceRenderablePool(const FvString& kName,
		const MeshPtr& spMesh, const MaterialPtr& spMaterial,
		FvUInt32 u32MaxInstance, FvUInt32 u32RenderSceneGroup,
		FvUInt32 u32SubMesh);
	~FvInstanceRenderablePool();

	void AddOutdoorInstance(RenderQueue* queue, const Matrix4* xform);

	void Reset();

	void SetGroupID(FvUInt32 u32GroupID);

protected:
	friend class FvInstanceRenderable;
	friend class FvInstanceEntity;

	FvString m_kName;
	MeshPtr m_spMesh;
	MaterialPtr m_spMaterial;
	FvUInt32 m_u32MaxInstance;
	LightList m_kEmptyLightList;
	Ogre::VertexData* m_pkVertexData;
	FvUInt32 m_u32MaxSource;
	const FvUInt32 m_u32RenderSceneGroup;
	const FvUInt32 m_u32SubMesh;
	FvUInt32 m_u32GroupID;

	FvInstanceRenderable* m_pkOutdoorPointer;

	FvRefListNameSpace<FvInstanceRenderable>::List1 m_kInstanceRenderableOutdoorList;

	
};

typedef FvSmartPointer<FvInstanceRenderablePool> FvInstanceRenderablePoolPtr;

class FV_MODEL_API FvInstanceEntity : public Ogre::Entity,
	public FvRefNode1<FvInstanceEntity*>
{
public:
	FvInstanceEntity(FvInstanceRenderablePool* pkInstanceRenderablePool,
		const Ogre::String& kName, const Ogre::MeshPtr& spMesh);

	virtual ~FvInstanceEntity();

	virtual const Ogre::String& getMovableType(void) const;

	void _updateRenderQueue(RenderQueue* queue);

	const FvInstanceRenderablePoolPtr& GetInstancePool();

protected:
	FvInstanceRenderablePoolPtr m_spInstanceRenderablePool;

};

class FV_MODEL_API FvInstance
{
public:
	static void Init();
	static void Term();

	static void SetInstanceEnable(bool bEnable);
	static bool GetInstanceEnable();

	static void Reset();

	static void Dump(RenderQueue* queue);

	static void SetDetailBendingEnable(bool bEnable);
	static bool GetDetailBendingEnable();

protected:
	friend class FvInstanceRenderablePool;
	friend class FvInstanceEntityFactory;

	static FvInstanceEntityFactory* ms_pkInstanceEntityFactory;

	static FvStringMap<FvInstanceRenderablePool*> ms_kRenderablesMap;

	static bool ms_bInstanceEnable;
	static bool ms_bDetailBendingEnable;

	static FvInstanceRenderable* ms_apkInstanceRenderableSet[512];
	static FvUInt32 m_u32InstanceRenderableSetPointer;

	static FvRefListNameSpace<FvInstanceRenderablePool>::List1 ms_kPoolsNeedReset;
};

#endif

#endif // __FvInstanceEntity_H__
