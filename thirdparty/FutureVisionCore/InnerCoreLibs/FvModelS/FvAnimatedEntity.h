//{future header message}
#ifndef __FvAnimatedEntity_H__
#define __FvAnimatedEntity_H__

#include <OgreEntity.h>
#include <FvBaseTypes.h>
#include <OgreMovableObject.h>
#include <FvRefList.h>
#include <FvStringMap.h>
#include "FvAnimatedCharacterResource.h"

#define FV_COMPONENT_MAX_PARTS (32)

class FvAnimatedEntity;
class FvComponentEntity;

class FvAnimatedEntity : public Ogre::Entity
{
public:
	FvAnimatedEntity(const FvString& kName, const Ogre::MeshPtr& spMesh);

	~FvAnimatedEntity();

	virtual void _notifyMoved();

	void _updateRenderQueue(Ogre::RenderQueue* queue);

	void _notifyCurrentCamera(Ogre::Camera* cam);

	void UpdateAnimationNew();

	void CacheBoneMatrices();

	void SetShow(bool bIsShow);

	bool GetShow();

	void SetBonesSwitchTable(FvUInt8* pu8BonesSwitchTable);

	FvUInt8* GetBonesSwitchTable();

	virtual const Ogre::String& getMovableType(void) const;

	virtual const Ogre::AxisAlignedBox& getBoundingBox(void) const;

	virtual const Ogre::AxisAlignedBox& getWorldBoundingBox(bool derive = false) const;

	void SetBoundingBoxTargetBone(FvUInt16 u16TargetBone, const Ogre::Vector3& kMax, const Ogre::Vector3& kMin);

	void DisableBoundingBoxTargetBone();

	void Attach(Ogre::MovableObject* pkObject, const char* pcName, FvAnimatedCharacterResource::AliasTagPoint& kAlias);

	void Detach(Ogre::MovableObject* pkObject);

	Ogre::TagPoint* RefTagPoint(const char* pcName, FvAnimatedCharacterResource::AliasTagPoint& kAlias);

	void FreeTagPoint(Ogre::TagPoint* pkTag);

	void LinkObject(Ogre::MovableObject* pkObject);

	void UnlinkObject(Ogre::MovableObject* pkObject);

protected:
	void AddBoneRef(Ogre::Bone* pkBone);

	void DelBoneRef(Ogre::Bone* pkBone);

	friend class FvComponentEntity;
	friend class FvAnimatedCharacterBase;
	FvAnimatedCharacterBase* m_pkParent;
	FvStringMap<Ogre::TagPoint*> m_kTagPoints;
	std::list<Ogre::MovableObject*> m_kAttachedObjects;
	std::list<Ogre::MovableObject*> m_kLinkedObjects;
	FvRefList<FvComponentEntity*> m_kVisibleComponentList;
	FvInt32 m_i32BoundingBoxTargetBone;
	bool m_bShow;
	bool m_bMoved;

};

class FvAnimatedEntityFactory : public Ogre::MovableObjectFactory
{
protected:
	Ogre::MovableObject* createInstanceImpl(const Ogre::String& kName, const Ogre::NameValuePairList* params);
	
public:
	FvAnimatedEntityFactory() {}
	~FvAnimatedEntityFactory() {}

	static Ogre::String FACTORY_TYPE_NAME;

	const Ogre::String& getType(void) const;

	void destroyInstance(Ogre::MovableObject* obj);
};

class FvComponentEntity : public Ogre::Entity, public Ogre::Renderable
{
public:
	FvComponentEntity(const Ogre::String& kName, const Ogre::MeshPtr& spMesh,
		const Ogre::MaterialPtr& spMaterial);

	virtual ~FvComponentEntity();

	virtual const Ogre::MaterialPtr& getMaterial() const;

	virtual Ogre::Technique* getTechnique() const;

	virtual void getRenderOperation(Ogre::RenderOperation& kRenderOperation);

	virtual void getWorldTransforms(Ogre::Matrix4* pkXform) const;

	virtual unsigned short getNumWorldTransforms() const;

	virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;

	virtual const Ogre::LightList& getLights() const;

	virtual bool getCastsShadows() const;

	virtual void _updateRenderQueue(Ogre::RenderQueue* queue);

	virtual const Ogre::String& getMovableType(void) const;

	void SetDisplayFlags(FvUInt32 u32Flags);

	void SetParent(FvAnimatedEntity* pkParent);

	FvAnimatedEntity* GetParent();

protected:

	void Init();

	friend class FvAnimatedEntity;

	FvRefNode2<FvComponentEntity*> m_kComponentNode;
	Ogre::IndexData* m_pkIndexData;
	FvAnimatedEntity* m_pkParent;

};

class FvComponentEntityFactory : public Ogre::MovableObjectFactory
{
protected:
	Ogre::MovableObject* createInstanceImpl(const Ogre::String& kName, const Ogre::NameValuePairList* params);

public:
	FvComponentEntityFactory() {}
	~FvComponentEntityFactory() {}

	static Ogre::String FACTORY_TYPE_NAME;

	const Ogre::String& getType(void) const;

	void destroyInstance(Ogre::MovableObject* obj);
};

#endif // __FvAnimatedEntity_H__
