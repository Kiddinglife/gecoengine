#include "FvFloraRenderer.h"
#include "FvFlora.h"

#include <FvWatcher.h>

#include <OgreVertexIndexData.h>
#include <OgreTechnique.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 );

FvFloraVertexContainer::FvFloraVertexContainer( FvFloraVertex *pkVerts, FvUInt32 uiNumVerts )
{
    Init( pkVerts, uiNumVerts );
}

void FvFloraVertexContainer::Init( FvFloraVertex *pkVerts, FvUInt32 uiNumVerts )
{
	m_pkVertsBase = pkVerts;
	m_pkVerts = pkVerts;
	m_uiNumVerts = uiNumVerts;
}

void FvFloraVertexContainer::AddVertex( const FvFloraVertex *pkVertex, const FvMatrix *pkTransform )
{
	FV_ASSERT( FvUInt32(m_pkVerts - m_pkVertsBase) < m_uiNumVerts );
	m_pkVerts->Set( *pkVertex, pkTransform );
    m_pkVerts->m_aiUV[0] += m_iUOffset;
	m_pkVerts->m_aiUV[1] += m_iVOffset;
	m_pkVerts->m_uiColor = pkVertex->m_uiColor;
	m_pkVerts++;
}

void FvFloraVertexContainer::AddVertices( const FvFloraVertex *pkVertices, FvUInt32 iCount, const FvMatrix *pkTransform )
{
	FV_ASSERT( FvUInt32(m_pkVerts - m_pkVertsBase) <= (m_uiNumVerts - iCount) );
	const FvFloraVertex *pkIt = pkVertices;
	const FvFloraVertex *pkEnd = pkVertices + iCount;
	while (pkIt != pkEnd)
	{
		AddVertex( pkIt++, pkTransform );
	}
}

void FvFloraVertexContainer::Clear( FvUInt32 uiNumVertices )
{
	FV_ASSERT( uiNumVertices <= (m_uiNumVerts - (m_pkVerts - m_pkVertsBase)) );
	ZeroMemory( m_pkVerts, uiNumVertices * sizeof( FvFloraVertex ) );
	m_pkVerts += uiNumVertices;
}

FvFloraRenderable::FvFloraRenderable(FvFloraBlock *pkFloraBlock) :
m_pkFloraBlock(pkFloraBlock)
{
	FV_ASSERT(m_pkFloraBlock);

	m_pkVertexData = OGRE_NEW Ogre::VertexData;
	Ogre::VertexDeclaration *pkDecl = m_pkVertexData->vertexDeclaration;
	size_t stOffset = 0;
	pkDecl->addElement(0, stOffset, Ogre::VET_FLOAT4, Ogre::VES_POSITION, 0);
	stOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT4);
	pkDecl->addElement(0, stOffset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL, 0);
	stOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
	pkDecl->addElement(0, stOffset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);
	stOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);
	pkDecl->addElement(0, stOffset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE, 0);
}

FvFloraRenderable::~FvFloraRenderable()
{
	OGRE_DELETE m_pkVertexData;
}

const Ogre::MaterialPtr &FvFloraRenderable::getMaterial(void) const
{
	FV_ASSERT(m_pkFloraBlock && m_pkFloraBlock->m_pkFlora);
	return m_pkFloraBlock->m_pkFlora->GetMaterial();
}

void FvFloraRenderable::getRenderOperation(Ogre::RenderOperation &kOP)
{
	FV_ASSERT(!m_pkFloraBlock->m_pkFlora->GetVertexBuffer().isNull());

	Ogre::VertexBufferBinding *pkBind = m_pkVertexData->vertexBufferBinding;
	if(!pkBind->isBufferBound(0))
		pkBind->setBinding(0,m_pkFloraBlock->m_pkFlora->GetVertexBuffer());

	kOP.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
	kOP.useIndexes = false;
	kOP.vertexData = m_pkVertexData;
	kOP.indexData = NULL;
}

void FvFloraRenderable::getWorldTransforms(Ogre::Matrix4 *pkXForm) const
{
	*pkXForm = Ogre::Matrix4::IDENTITY;
	pkXForm->setTrans(*(Ogre::Vector3*)&m_pkFloraBlock->m_pkFlora->Transform(*m_pkFloraBlock).ApplyToOrigin());
}

Ogre::Real FvFloraRenderable::getSquaredViewDepth(const Ogre::Camera *pkCam) const
{
	return 0.f;
}

const Ogre::LightList &FvFloraRenderable::getLights(void) const
{
	static Ogre::LightList s_kLightList;
	return s_kLightList;
}

void FvFloraRenderable::SetVertex(FvUInt32 uiFirstVertex, FvUInt32 uiNumVertices)
{
	FV_ASSERT(m_pkVertexData);
	m_pkVertexData->vertexStart = uiFirstVertex;
	m_pkVertexData->vertexCount = uiNumVertices;
}

Ogre::Technique* FvFloraRenderable::getTechnique( void ) const
{
	Ogre::Technique* pkRes = getMaterial()->getBestTechnique(0, this);

	if(m_pkFloraBlock->m_pkFlora->GetDetailBendingEnable())
	{
		Ogre::Technique* pkOverride = getMaterial()->getTechnique(pkRes->getName() + "_DetailBending");
		if(pkOverride) pkRes = pkOverride;
	}

	return pkRes;

}