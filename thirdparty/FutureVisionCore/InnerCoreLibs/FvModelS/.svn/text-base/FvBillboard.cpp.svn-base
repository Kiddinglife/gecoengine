#include "FvBillboard.h"
#include <FvDebug.h>
#include <OgreMaterialManager.h>
#include <OgreHardwareBufferManager.h>
#include <d3d9.h>
#include <OgreD3D9RenderSystem.h>
#include <FvMathPower.h>

//----------------------------------------------------------------------------
FvBillboard::FvBillboard(const FvString& kMaterialName,
	FvUInt32 u32CellNumber, const FvVector3& kAxis)
	: m_u32CellNumber(u32CellNumber), m_kAxis(kAxis),
	m_u32InstancePointer(0), m_u32PoolPointer(0), m_u8GroupID(90)
{
	m_spBillboardMaterial = Ogre::MaterialManager::getSingleton().load(
		kMaterialName, "General");
	FV_ASSERT(!m_spBillboardMaterial.isNull());

	Instance* pkTheFirstPool = new Instance[m_u32CellNumber];
	m_kParamsPool.push_back(pkTheFirstPool);

	float fScaleY = 1.0f / FvTanf(FV_MATH_PI_4_F * 0.5f);

	m_kScaleXY = FvVector4(fScaleY * 0.75,fScaleY,0,0);

	Init();
}
//----------------------------------------------------------------------------
FvBillboard::FvBillboard( Ogre::MaterialPtr spMaterial,
	FvUInt32 u32CellNumber, const FvVector3& kAxis)
	: m_u32CellNumber(u32CellNumber), m_kAxis(kAxis),
	m_u32InstancePointer(0), m_u32PoolPointer(0), m_u8GroupID(90)
{
	m_spBillboardMaterial = spMaterial;
	FV_ASSERT(!m_spBillboardMaterial.isNull());

	Instance* pkTheFirstPool = new Instance[m_u32CellNumber];
	m_kParamsPool.push_back(pkTheFirstPool);

	float fScaleY = 1.0f / FvTanf(FV_MATH_PI_4_F * 0.5f);

	m_kScaleXY = FvVector4(fScaleY * 0.75,fScaleY,0,0);

	Init();
}
//----------------------------------------------------------------------------
FvBillboard::~FvBillboard()
{
	for(FvUInt32 i(0); i < m_kParamsPool.size(); ++i)
	{
		delete [] (m_kParamsPool[i]);
	}
	m_kParamsPool.clear();
}
//----------------------------------------------------------------------------
void FvBillboard::Reset()
{
	m_u32InstancePointer = 0;
	m_u32PoolPointer = 0;
}
//----------------------------------------------------------------------------
void FvBillboard::AddInstance(const Instance& kInstance)
{
	if(m_u32InstancePointer < m_u32CellNumber)
	{
		m_kParamsPool[m_u32PoolPointer][m_u32InstancePointer] = kInstance;
		++m_u32InstancePointer;
	}
	else
	{
		++m_u32PoolPointer;

		if(m_u32PoolPointer >= m_kParamsPool.size())
		{
			Instance* pkPool = new Instance[m_u32CellNumber];
			m_kParamsPool.push_back(pkPool);
		}

		m_kParamsPool[m_u32PoolPointer][0] = kInstance;

		m_u32InstancePointer = 1;
	}
}
//----------------------------------------------------------------------------
FvUInt32 FvBillboard::GetInstanceNumber()
{
	return m_u32PoolPointer * m_u32CellNumber + m_u32InstancePointer;
}
//----------------------------------------------------------------------------
void FvBillboard::Init()
{
	m_pkVertexData = OGRE_NEW Ogre::VertexData();

	Ogre::VertexDeclaration *pkDecl = m_pkVertexData->vertexDeclaration;
	Ogre::VertexBufferBinding *pkBind = m_pkVertexData->vertexBufferBinding;

	pkDecl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION, 0);

	m_spVertexBuffer =
		Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		sizeof(FvVector3), m_u32CellNumber * 6, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	pkBind->setBinding(0,m_spVertexBuffer);

	FillVertexBuffer();
}
//----------------------------------------------------------------------------
void FvBillboard::FillVertexBuffer()
{
	FvVector3* pkBuffer =
		(FvVector3*)(m_spVertexBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL));

	for(FvUInt32 i(0); i < m_u32CellNumber; ++i)
	{
		pkBuffer[0] = FvVector3(-0.5f, -0.5f, float(i));
		pkBuffer[1] = FvVector3(0.5f, -0.5f, float(i));
		pkBuffer[2] = FvVector3(-0.5f, 0.5f, float(i));
		pkBuffer[3] = FvVector3(-0.5f, 0.5f, float(i));
		pkBuffer[4] = FvVector3(0.5f, -0.5f, float(i));
		pkBuffer[5] = FvVector3(0.5f, 0.5f, float(i));

		pkBuffer += 6;
	}

	m_spVertexBuffer->unlock();
}
//----------------------------------------------------------------------------
const Ogre::MaterialPtr& FvBillboard::getMaterial() const
{
	return m_spBillboardMaterial;
}
//----------------------------------------------------------------------------
bool FvBillboard::preRender(Ogre::SceneManager* sm, Ogre::RenderSystem* rsys)
{
	Ogre::D3D9RenderSystem* pkRenderSystem =
		(Ogre::D3D9RenderSystem*)rsys;
	LPDIRECT3DDEVICE9 lpD3DDevice = pkRenderSystem->getDevice();

	pkRenderSystem->setVertexDeclaration(m_pkVertexData->vertexDeclaration);
	pkRenderSystem->setVertexBufferBinding(m_pkVertexData->vertexBufferBinding);

	lpD3DDevice->SetVertexShaderConstantF(5, m_kScaleXY, 1);

	for(FvUInt32 i(0); i < m_u32PoolPointer; ++i)
	{
		lpD3DDevice->SetVertexShaderConstantF(6, (const float*)(m_kParamsPool[i]),
			m_u32CellNumber * 3);

		Render(rsys, m_u32CellNumber);
	}

	if(m_u32InstancePointer)
	{
		lpD3DDevice->SetVertexShaderConstantF(6,
			(const float*)(m_kParamsPool[m_u32PoolPointer]), m_u32InstancePointer * 3);

		Render(rsys, m_u32InstancePointer);
	}

	Reset();
	return false;
	//return true;
}
//----------------------------------------------------------------------------
void FvBillboard::getRenderOperation(Ogre::RenderOperation& op)
{
	m_pkVertexData->vertexCount = 6 * m_u32InstancePointer;
	op.vertexData = m_pkVertexData;
	op.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
	op.useIndexes = false;
	op.srcRenderable = this;
}
//----------------------------------------------------------------------------
void FvBillboard::getWorldTransforms(Ogre::Matrix4* xform) const
{
	*xform = Ogre::Matrix4::IDENTITY;
}
//----------------------------------------------------------------------------
unsigned short FvBillboard::getNumWorldTransforms(void)
{
	return 1;
}
//----------------------------------------------------------------------------
Ogre::Real FvBillboard::getSquaredViewDepth(const Ogre::Camera* cam) const
{
	return 0;
}
//----------------------------------------------------------------------------
const Ogre::LightList& FvBillboard::getLights(void) const
{
	static Ogre::LightList s_kLightList;
	return s_kLightList;
}
//----------------------------------------------------------------------------
void FvBillboard::SetGroupID(FvUInt8 u8GroupID)
{
	m_u8GroupID = u8GroupID;
}
//----------------------------------------------------------------------------
FvUInt8 FvBillboard::GetGroupID()
{
	return m_u8GroupID;
}
//----------------------------------------------------------------------------
void FvBillboard::SetAxis(const FvVector3& kAxis)
{
	m_kAxis = kAxis;
}
//----------------------------------------------------------------------------
const FvVector3& FvBillboard::GetAxis()
{
	return m_kAxis;
}
//----------------------------------------------------------------------------
void FvBillboard::Render(void* pvRenderSystem, FvUInt32 u32Number)
{
	Ogre::D3D9RenderSystem* pkRenderSystem =
		(Ogre::D3D9RenderSystem*)pvRenderSystem;
	LPDIRECT3DDEVICE9 lpD3DDevice = pkRenderSystem->getDevice();

	HRESULT hr;

	hr = lpD3DDevice->DrawPrimitive(
			D3DPT_TRIANGLELIST, 0, u32Number * 2); 

	if( FAILED( hr ) )
	{
		FvString msg = DXGetErrorDescription9(hr);
		OGRE_EXCEPT(Ogre::Exception::ERR_RENDERINGAPI_ERROR, "Failed to DrawPrimitive : " + msg, "D3D9RenderSystem::_render" );
	}

}
//----------------------------------------------------------------------------
