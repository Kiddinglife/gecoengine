#include "FvInstanceEntity.h"
#include <Ogre.h>
#include <OgreD3D9RenderSystem.h>

#ifndef FV_SERVER

Ogre::String FvInstanceEntityFactory::FACTORY_TYPE_NAME = "FvInstanceEntity";

FvInstanceEntityFactory* FvInstance::ms_pkInstanceEntityFactory = NULL;
FvStringMap<FvInstanceRenderablePool*> FvInstance::ms_kRenderablesMap;
bool FvInstance::ms_bInstanceEnable = false;

FvInstanceRenderable* FvInstance::ms_apkInstanceRenderableSet[512];
FvUInt32 FvInstance::m_u32InstanceRenderableSetPointer = 0;
bool FvInstance::ms_bDetailBendingEnable = false;
FvRefListNameSpace<FvInstanceRenderablePool>::List1 FvInstance::ms_kPoolsNeedReset;

//----------------------------------------------------------------------------
FvInstanceEntity::FvInstanceEntity(
	FvInstanceRenderablePool* pkInstanceRenderablePool,
	const Ogre::String& kName, const Ogre::MeshPtr& spMesh)
	: Ogre::Entity(kName, spMesh)
{
	//FV_ASSERT(spMesh->getNumSubMeshes() == 1);
	m_spInstanceRenderablePool = pkInstanceRenderablePool;
}
//----------------------------------------------------------------------------
FvInstanceEntity::~FvInstanceEntity()
{
	m_spInstanceRenderablePool = NULL;
}
//----------------------------------------------------------------------------
const Ogre::String& FvInstanceEntity::getMovableType() const
{
	return FvInstanceEntityFactory::FACTORY_TYPE_NAME;
}
//----------------------------------------------------------------------------
void FvInstanceEntity::_updateRenderQueue(RenderQueue* queue)
{
	if(FvInstance::GetInstanceEnable())
	{
		m_spInstanceRenderablePool->AddOutdoorInstance(
			queue, &_getParentNodeFullTransform());
	}
	else
	{
		Ogre::SubEntity* pkEntity = Ogre::Entity::getSubEntity(
			m_spInstanceRenderablePool->m_u32SubMesh);

		queue->addRenderable(pkEntity,
			m_spInstanceRenderablePool->m_u32RenderSceneGroup);
	}
}
//----------------------------------------------------------------------------
const FvInstanceRenderablePoolPtr& FvInstanceEntity::GetInstancePool()
{
	return m_spInstanceRenderablePool;
}
//----------------------------------------------------------------------------
const Ogre::String& FvInstanceEntityFactory::getType() const
{
	return FACTORY_TYPE_NAME;
}
//----------------------------------------------------------------------------
void FvInstanceEntityFactory::destroyInstance(Ogre::MovableObject* obj)
{
	OGRE_DELETE obj;
}
//----------------------------------------------------------------------------
Ogre::MovableObject* FvInstanceEntityFactory::createInstanceImpl(
	const Ogre::String& name, const Ogre::NameValuePairList* params)
{
	// must have mesh parameter
	Ogre::MeshPtr pMesh;
	Ogre::MaterialPtr pMaterial; 
	if (params != 0)
	{
		Ogre::NameValuePairList::const_iterator ni = params->find("mesh");
		if (ni != params->end())
		{
			// Get mesh (load if required)
			pMesh = Ogre::MeshManager::getSingleton().load(
				ni->second,
				// autodetect group location
				Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME );
		}

		{
			NameValuePairList::const_iterator ni = params->find("material");
			if (ni != params->end())
			{
				// Get mesh (load if required)
				pMaterial = MaterialManager::getSingleton().load(
					ni->second,
					// autodetect group location
					ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME );
			}
		}

	}
	if (pMesh.isNull())
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
			"'mesh' parameter required when constructing an Entity.",
			"FvInstanceEntityFactory::createInstance");
	}

	if(pMaterial.isNull())
	{
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
			"'material' parameter required when constructing an Entity.",
			"EntityFactory::createInstance");
	}
	
	//FV_ASSERT(pMesh->getNumSubMeshes() == 1);

	FvString kRenderQueueGroup = params->find("render_queue_group")->second;
	FvString kSubMesh = params->find("sub_mesh")->second;
	FvString kZoneName = params->find("zone_name")->second;

	FvString kKey = pMesh->getName() + "_" + pMaterial->getName() + "_" + kRenderQueueGroup + "_" + kSubMesh + kZoneName;

	FvStringMap<FvInstanceRenderablePool*>::iterator it =
		FvInstance::ms_kRenderablesMap.find(kKey);

	FvInstanceRenderablePool* pkPool;

	if(it == FvInstance::ms_kRenderablesMap.end())
	{
		FvInt32 i32InstanceNumber = 80;
		{
			NameValuePairList::const_iterator ni = params->find("instance_number");
			if (ni != params->end())
			{
				i32InstanceNumber = atoi(ni->second.c_str());
			}
		}

		FvUInt32 u32RenderQueueGroup = atoi(kRenderQueueGroup.c_str());
		FvUInt32 u32SubMesh = atoi(kSubMesh.c_str());

		FV_ASSERT(i32InstanceNumber <= 80);

		pkPool = new FvInstanceRenderablePool(kKey, pMesh, pMaterial, i32InstanceNumber,
			u32RenderQueueGroup, u32SubMesh);
	}
	else
	{
		pkPool = it->second;
	}

	FvInstanceEntity* pkEntity = OGRE_NEW FvInstanceEntity(pkPool, name, pMesh);
	pkEntity->setMaterial(pMaterial);

	return pkEntity;
}
//----------------------------------------------------------------------------
void FvInstance::Init()
{
	FV_ASSERT(!ms_pkInstanceEntityFactory);

	ms_pkInstanceEntityFactory = OGRE_NEW FvInstanceEntityFactory();
	FV_ASSERT(ms_pkInstanceEntityFactory);
	Ogre::Root::getSingleton().addMovableObjectFactory(ms_pkInstanceEntityFactory);
}
//----------------------------------------------------------------------------
void FvInstance::Term()
{
	FV_ASSERT(ms_pkInstanceEntityFactory);
	Ogre::Root* pkRoot = Ogre::Root::getSingletonPtr();
	if(pkRoot)
	{
		pkRoot->removeMovableObjectFactory(ms_pkInstanceEntityFactory);
	}
	OGRE_DELETE ms_pkInstanceEntityFactory;
	ms_pkInstanceEntityFactory = NULL;
}
//----------------------------------------------------------------------------
void FvInstance::SetInstanceEnable(bool bEnable)
{
	ms_bInstanceEnable = bEnable;
}
//----------------------------------------------------------------------------
bool FvInstance::GetInstanceEnable()
{
	return ms_bInstanceEnable;
}
//----------------------------------------------------------------------------
void FvInstance::Reset()
{
	/*FvStringMap<FvInstanceRenderablePool*>::iterator it =
		ms_kRenderablesMap.begin();

	while(it != ms_kRenderablesMap.end())
	{
		it->second->Reset();
		++it;
	}*/

	ms_kPoolsNeedReset.BeginIterator();
	while(!ms_kPoolsNeedReset.IsEnd())
	{
		FvRefListNameSpace<FvInstanceRenderablePool>::List1::iterator iter =
			ms_kPoolsNeedReset.GetIterator();
		FvInstanceRenderablePool* obj = static_cast<FvInstanceRenderablePool*>(iter);
		ms_kPoolsNeedReset.Next();

		obj->Reset();

		obj->Detach();
	}

	FV_ASSERT(!ms_kPoolsNeedReset.Size());
}
//----------------------------------------------------------------------------
void FvInstance::Dump(RenderQueue* queue)
{
	/*FvStringMap<FvInstanceRenderablePool*>::iterator it =
		ms_kRenderablesMap.begin();

	while(it != ms_kRenderablesMap.end())
	{
		it->second->Dump();
		++it;
	}*/

	for(FvUInt32 i(0); i < m_u32InstanceRenderableSetPointer; ++i)
	{
		ms_apkInstanceRenderableSet[i]->Dump(queue);
		//queue->addRenderable(ms_apkInstanceRenderableSet[i], RENDER_QUEUE_MAX);
	}

	FvInstance::m_u32InstanceRenderableSetPointer = 0;
}
//----------------------------------------------------------------------------
void FvInstance::SetDetailBendingEnable(bool bEnable)
{
	ms_bDetailBendingEnable = bEnable;
}
//----------------------------------------------------------------------------
bool FvInstance::GetDetailBendingEnable()
{
	return ms_bDetailBendingEnable;
}
//----------------------------------------------------------------------------
FvInstanceRenderable::FvInstanceRenderable(
	FvInstanceRenderablePool* pkParent, FvUInt32 u32MaxInstanceNumber)
{
	FV_ASSERT(pkParent);
	m_pkParent = pkParent;
	m_fpTransformBuffer = new float[u32MaxInstanceNumber * 12];
	m_u32Available = u32MaxInstanceNumber;
	m_u32UseNumber = 0;
	m_bNeedDump = false;

	m_pkOverrideBinding = HardwareBufferManager::getSingleton().
		createVertexBufferBinding();

	const VertexBufferBinding::VertexBufferBindingMap& bindings = 
		pkParent->m_pkVertexData->vertexBufferBinding->getBindings();
	VertexBufferBinding::VertexBufferBindingMap::const_iterator vbi, vbend;
	vbend = bindings.end();
	for (vbi = bindings.begin(); vbi != vbend; ++vbi)
	{
		HardwareVertexBufferSharedPtr srcbuf = vbi->second;
		HardwareVertexBufferSharedPtr dstBuf;
		
		dstBuf = srcbuf;

		// Copy binding
		m_pkOverrideBinding->setBinding(vbi->first, dstBuf);
	}

	m_spInstanceRenderBuffer = HardwareBufferManager::getSingleton().
		createVertexBuffer(4, u32MaxInstanceNumber,
		HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	float* fpBuffer = (float*)m_spInstanceRenderBuffer->lock(HardwareBuffer::HBL_NORMAL);

	for(FvUInt32 i(0); i < u32MaxInstanceNumber; ++i)
	{
		fpBuffer[i] = ((float)(u32MaxInstanceNumber-1-i)) * 3.f;
	}

	m_spInstanceRenderBuffer->unlock();

	m_pkOverrideBinding->setBinding(
		pkParent->m_u32MaxSource + 1, m_spInstanceRenderBuffer);

}
//----------------------------------------------------------------------------
FvInstanceRenderable::~FvInstanceRenderable()
{
	FV_SAFE_DELETE_ARRAY(m_fpTransformBuffer);
	HardwareBufferManager::getSingleton().
		destroyVertexBufferBinding(m_pkOverrideBinding);
}
//----------------------------------------------------------------------------
const MaterialPtr& FvInstanceRenderable::getMaterial( void ) const
{
	return m_pkParent->m_spMaterial;
}
//----------------------------------------------------------------------------
void FvInstanceRenderable::getRenderOperation(RenderOperation& op)
{
	m_pkParent->m_spMesh->getSubMesh(m_pkParent->m_u32SubMesh)->_getRenderOperation(op);

	if(m_u32UseNumber > 1)
	{
		op.vertexData = m_pkParent->m_pkVertexData;
		op.m_pkOverrideBinding = m_pkOverrideBinding;
	}
}
//----------------------------------------------------------------------------
void FvInstanceRenderable::getWorldTransforms(Matrix4* xform) const
{
	if(m_u32UseNumber)
	{
		FvCrazyCopyEx(xform, m_fpTransformBuffer, 4, 12);
		(*xform)[3][0] = 0;
		(*xform)[3][1] = 0;
		(*xform)[3][2] = 0;
		(*xform)[3][3] = 1;
	}
	else
	{
		*xform = Matrix4(1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1);
	}
}
//----------------------------------------------------------------------------
unsigned short FvInstanceRenderable::getNumWorldTransforms()
{
	return 1;
}
//----------------------------------------------------------------------------
Ogre::Real FvInstanceRenderable::getSquaredViewDepth(const Camera* cam) const
{
	return 0;
}
//----------------------------------------------------------------------------
const LightList& FvInstanceRenderable::getLights( void ) const
{
	return m_pkParent->m_kEmptyLightList;
}
//----------------------------------------------------------------------------
FvUInt32 FvInstanceRenderable::AddInstance(const Matrix4* xform)
{
	++m_u32UseNumber;

	if(m_u32UseNumber > m_u32Available)
	{
		m_u32UseNumber = m_u32Available;
		return 0;
	}
	else
	{
		FvCrazyCopyEx(m_fpTransformBuffer + (m_u32UseNumber - 1) * 12,
			xform, 4, 12);
		
		return m_u32UseNumber;
	}
}
//----------------------------------------------------------------------------
FvUInt32 FvInstanceRenderable::GetUseNumber()
{
	return m_u32UseNumber;
}
//----------------------------------------------------------------------------
void FvInstanceRenderable::Reset()
{
	m_u32UseNumber = 0;
}
//----------------------------------------------------------------------------
void FvInstanceRenderable::Dump(RenderQueue* queue)
{
	if(m_bNeedDump && (m_u32UseNumber > 1))
	{
		FvUInt32 u32Offset = (m_u32Available - m_u32UseNumber) * 4;

		//m_spInstanceRenderBuffer->writeData(u32Offset, m_u32UseNumber * 48,
		//	m_fpTransformBuffer, true);

		m_spInstanceRenderBuffer->SetOffset(u32Offset);

		m_pkOverrideBinding->SetFrequencyEnable(true);

		for(FvUInt32 i(0); i <= m_pkParent->m_u32MaxSource; ++i)
		{
			m_pkOverrideBinding->SetFrequency(i, INDEXED_DATA, m_u32UseNumber);
		}
		
		m_pkOverrideBinding->SetFrequency(m_pkParent->m_u32MaxSource + 1, INSTANCE_DATA, 1);

		m_bNeedDump = false;
	}

	Ogre::Technique* pkTech = getTechnique();
	const FvString& kName = pkTech->getName();
	Ogre::Technique* pkOverwriteTech = NULL;

	if(FvInstance::GetDetailBendingEnable() && (m_pkParent->m_u32RenderSceneGroup == 53))
	{
		pkOverwriteTech = pkTech->getParent()->getTechnique(
			kName + "_DetailBending");
		if(pkOverwriteTech) pkTech = pkOverwriteTech;
	}

	RenderQueueGroup* pGroup = queue->getQueueGroup(m_pkParent->m_u32GroupID);
	pGroup->addRenderable(this,pkTech,OGRE_RENDERABLE_DEFAULT_PRIORITY);

	//queue->addRenderable(this, m_pkParent->m_u32RenderSceneGroup);
}
//----------------------------------------------------------------------------
void FvInstanceRenderable::DirtyDump()
{
	m_bNeedDump = true;
}
//----------------------------------------------------------------------------
Technique* FvInstanceRenderable::getTechnique( void ) const
{
	Technique* pkRes = getMaterial()->getBestTechnique(0, this);
	FV_ASSERT(pkRes);

	if(m_u32UseNumber > 1)
	{
		Technique* pkOverride = getMaterial()->getTechnique(pkRes->getName() + "_instance");
		if(pkOverride) pkRes = pkOverride;
	}

	return pkRes;
}
//----------------------------------------------------------------------------
bool FvInstanceRenderable::preRender( SceneManager* sm, RenderSystem* rsys )
{
	if(m_u32UseNumber > 1)
	{
		LPDIRECT3DDEVICE9 lpD3DDevice = ((D3D9RenderSystem*)rsys)->getDevice();

		FvUInt32 u32Count = m_u32UseNumber * 3;
		FV_ASSERT(u32Count <= 240);
		//u32Count = u32Count > 240 ? 240 : u32Count;

		lpD3DDevice->SetVertexShaderConstantF(15, m_fpTransformBuffer, u32Count);
	}

	return true;
}
//----------------------------------------------------------------------------
FvInstanceRenderablePool::FvInstanceRenderablePool(const FvString& kName,
	const MeshPtr& spMesh,const MaterialPtr& spMaterial,
	FvUInt32 u32MaxInstance, FvUInt32 u32RenderSceneGroup,
	FvUInt32 u32SubMesh) : m_u32RenderSceneGroup(u32RenderSceneGroup),
	m_u32SubMesh(u32SubMesh)
{
	m_kName = kName;
	m_spMesh = spMesh;
	m_spMaterial = spMaterial;
	m_u32MaxInstance = u32MaxInstance;
	m_pkOutdoorPointer = NULL;
	m_u32MaxSource = 0;
	m_u32GroupID = RENDER_QUEUE_MAX;

	FvInstance::ms_kRenderablesMap.insert(
		FvStringMap<FvInstanceRenderablePool*>::value_type(
		m_kName.c_str(), this));

	RenderOperation kOp;

	m_spMesh->getSubMesh(u32SubMesh)->_getRenderOperation(kOp);

	VertexData* pkSourceData = kOp.vertexData;

	m_pkVertexData = pkSourceData->clone(false);

	VertexDeclaration::VertexElementList& kList =
		*(VertexDeclaration::VertexElementList*)(void*)
		&(m_pkVertexData->vertexDeclaration->getElements());

	for(VertexDeclaration::VertexElementList::iterator it = kList.begin();
		it != kList.end(); ++it)
	{
		FvUInt32 u32CurrentSource = (*it).getSource();

		if(m_u32MaxSource < u32CurrentSource) m_u32MaxSource = u32CurrentSource;
	}

	m_pkVertexData->vertexDeclaration->addElement(
		m_u32MaxSource + 1, 0, VET_FLOAT1, VES_BLEND_INDICES, 0);
	//m_pkVertexData->vertexDeclaration->addElement(
	//	m_u32MaxSource + 1, 16, VET_FLOAT4, VES_BLEND_WEIGHTS, 1);
	//m_pkVertexData->vertexDeclaration->addElement(
	//	m_u32MaxSource + 1, 32, VET_FLOAT4, VES_BLEND_WEIGHTS, 2);

}
//----------------------------------------------------------------------------
FvInstanceRenderablePool::~FvInstanceRenderablePool()
{
	m_kInstanceRenderableOutdoorList.BeginIterator();
	while (!m_kInstanceRenderableOutdoorList.IsEnd())
	{
		FvRefListNameSpace<FvInstanceRenderable>::List1::iterator iter =
			m_kInstanceRenderableOutdoorList.GetIterator();

		FvInstanceRenderable* obj = static_cast<FvInstanceRenderable*>(iter);

		m_kInstanceRenderableOutdoorList.Next();

		FV_SAFE_DELETE(obj);
	}

	FvInstance::ms_kRenderablesMap.erase(m_kName);

	OGRE_DELETE m_pkVertexData;
}
//----------------------------------------------------------------------------
void FvInstanceRenderablePool::AddOutdoorInstance(RenderQueue* queue,
	const Matrix4* xform)
{
	if(!m_pkOutdoorPointer)
	{
		//FV_ASSERT(!m_kInstanceRenderableOutdoorList.Size());
		m_pkOutdoorPointer = new FvInstanceRenderable(this, m_u32MaxInstance);
		m_kInstanceRenderableOutdoorList.AttachBack(*m_pkOutdoorPointer);
	}

	FV_ASSERT(m_pkOutdoorPointer);

	FvUInt32 u32Res = m_pkOutdoorPointer->AddInstance(xform);

	FV_ASSERT(u32Res);

	if(u32Res == 1)
	{
		//queue->addRenderable(m_pkOutdoorPointer, RENDER_QUEUE_MAX);
		m_pkOutdoorPointer->DirtyDump();
		FV_ASSERT(FvInstance::m_u32InstanceRenderableSetPointer < 512);
		FvInstance::ms_apkInstanceRenderableSet[FvInstance::m_u32InstanceRenderableSetPointer++] = m_pkOutdoorPointer;
		
	}

	if(u32Res == m_u32MaxInstance)
	{
		m_pkOutdoorPointer = static_cast<FvInstanceRenderable*>(m_pkOutdoorPointer->GetNextContent());
	}

	FvInstance::ms_kPoolsNeedReset.AttachBack(*this);
}
//----------------------------------------------------------------------------
void FvInstanceRenderablePool::Reset()
{
	m_pkOutdoorPointer = static_cast<FvInstanceRenderable*>(m_kInstanceRenderableOutdoorList.GetHead());
	FV_ASSERT(!((!m_pkOutdoorPointer) && (m_kInstanceRenderableOutdoorList.Size())));

	m_kInstanceRenderableOutdoorList.BeginIterator();
	while (!m_kInstanceRenderableOutdoorList.IsEnd())
	{
		FvRefListNameSpace<FvInstanceRenderable>::List1::iterator iter = m_kInstanceRenderableOutdoorList.GetIterator();
		FvInstanceRenderable* obj = static_cast<FvInstanceRenderable*>(iter);
		m_kInstanceRenderableOutdoorList.Next();
		obj->Reset();
	}
}
//----------------------------------------------------------------------------
void FvInstanceRenderablePool::SetGroupID(FvUInt32 u32GroupID)
{
	m_u32GroupID = u32GroupID;
}
//----------------------------------------------------------------------------

#endif
