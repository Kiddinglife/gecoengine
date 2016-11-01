//{future header message}
#ifndef __FvFloraRenderer_H__
#define __FvFloraRenderer_H__

#include <FvVector3.h>
#include <FvMatrix.h>

#include <OgreRenderable.h>

#include "FvFloraVertexType.h"

class FvFloraBlock;

class FvFloraVertexContainer
{
public:
	FvFloraVertexContainer( FvFloraVertex *pkVerts = NULL, FvUInt32 uiNumVerts = 0 );
	void AddVertex( const FvFloraVertex *pkVertex, const FvMatrix *pkTransform );
	void AddVertices( const FvFloraVertex *pkVertices, FvUInt32 uiCount, const FvMatrix *pkTransform );
	void Clear( FvUInt32 uiNumVertices );
	void Init( FvFloraVertex *pkVerts, FvUInt32 uiNumVerts );
	void BlockNum( int iBlockNum ) { m_iBlockNum = iBlockNum; };
	void UVOffset( float fUOffset, float fVOffset ) { m_iUOffset = fUOffset; m_iVOffset = fVOffset; }

protected:

	FvFloraVertex *m_pkVertsBase;
	FvFloraVertex *m_pkVerts;
	FvUInt32 m_uiNumVerts;
	int m_iBlockNum;
	float m_iUOffset;
	float m_iVOffset;
};

class FvFloraRenderable : public Ogre::Renderable
{
public:
	FvFloraRenderable(FvFloraBlock *pkFloraBlock);
	~FvFloraRenderable();

	const Ogre::MaterialPtr &getMaterial(void) const;

	void getRenderOperation(Ogre::RenderOperation &kOP);

	Ogre::Technique* getTechnique(void) const;

	void getWorldTransforms(Ogre::Matrix4 *pkXForm) const;

	Ogre::Real getSquaredViewDepth(const Ogre::Camera *pkCam) const;

	const Ogre::LightList &getLights(void) const;

	void SetVertex(FvUInt32 uiFirstVertex, FvUInt32 uiNumVertices);

private:

	Ogre::VertexData *m_pkVertexData;

	FvFloraBlock *m_pkFloraBlock;

	friend class FvFloraBlock;
};

#endif // __FvFloraRenderer_H__