//{future header message}
#ifndef __FvBillboard_H__
#define __FvBillboard_H__

#include <OgreRenderable.h>
#include <FvVector4.h>
#include <OgreMaterial.h>
#include <vector>
#include <OgreHardwareVertexBuffer.h>
#include <FvRefList.h>
#include "FvModelDefines.h"

#define FV_MAX_BILLBOARD_CELL_NUMBER (80)

class FV_MODEL_API FvBillboard : public Ogre::Renderable,
	public FvRefNode1<FvBillboard*>
{
public:
	struct Instance
	{
		FvVector4 m_kPosition;
		FvVector4 m_kDimension;
		FvVector4 m_kTexcoordTransform;
	};

	FvBillboard(const FvString& kMaterialName,
		FvUInt32 u32CellNumber = FV_MAX_BILLBOARD_CELL_NUMBER,
		const FvVector3& kAxis = FvVector3::UNIT_Z);

	FvBillboard(Ogre::MaterialPtr spMaterial,
		FvUInt32 u32CellNumber = FV_MAX_BILLBOARD_CELL_NUMBER,
		const FvVector3& kAxis = FvVector3::UNIT_Z);

	virtual ~FvBillboard();

	void Reset();

	void AddInstance(const Instance& kInstance);

	FvUInt32 GetInstanceNumber();

	virtual const Ogre::MaterialPtr& getMaterial(void) const;

	virtual bool preRender(Ogre::SceneManager* sm, Ogre::RenderSystem* rsys);

	virtual void getRenderOperation(Ogre::RenderOperation& op);

	virtual void getWorldTransforms(Ogre::Matrix4* xform) const;

	virtual unsigned short getNumWorldTransforms(void);

	virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;

	virtual const Ogre::LightList& getLights(void) const;

	void SetGroupID(FvUInt8 u8GroupID);

	FvUInt8 GetGroupID();

	void SetAxis(const FvVector3& kAxis);

	const FvVector3& GetAxis();

protected:
	void Init();

	void FillVertexBuffer();

	void Render(void* pvRenderSystem, FvUInt32 u32Number);

	Ogre::MaterialPtr m_spBillboardMaterial;
	const FvUInt32 m_u32CellNumber;
	FvVector3 m_kAxis;
	FvVector4 m_kScaleXY;
	std::vector<Instance*> m_kParamsPool;
	FvUInt32 m_u32InstancePointer;
	FvUInt32 m_u32PoolPointer;
	Ogre::VertexData* m_pkVertexData;
	Ogre::HardwareVertexBufferSharedPtr m_spVertexBuffer;
	FvUInt8 m_u8GroupID;
	

};

#endif // __FvBillboard_H__
