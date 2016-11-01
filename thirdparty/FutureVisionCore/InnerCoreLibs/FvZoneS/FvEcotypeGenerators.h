//{future header message}
#ifndef __FvEcotypeGenerators_H__
#define __FvEcotypeGenerators_H__

#include <FvXMLSection.h>
#include <FvBoundingBox.h>

#include "FvEcotype.h"
#include "FvFloraVertexType.h"

class FvEcotypeGenerator
{
public:
	static FvEcotypeGenerator *Create(FvXMLSectionPtr spSection,
		FvEcotype &kTarget );
	virtual ~FvEcotypeGenerator() {};
	virtual bool Load( FvXMLSectionPtr spSection,
		FvEcotype &kTarget ) = 0;

	virtual FvUInt32 Generate(			
		const FvVector2 &kUVOffset,
		class FvFloraVertexContainer *pkVerts,
		FvUInt32 uiID,
		FvUInt32 uiMaxVerts,
		const FvMatrix &kObjectToWorld,
		const FvMatrix &kObjectToZone,
		FvBoundingBox &kBB ) = 0;	

	virtual bool IsEmpty() const = 0;
};

class FvEmptyEcotype : public FvEcotypeGenerator
{
	virtual FvUInt32 Generate(			
		const FvVector2 &kUVOffset,
		class FvFloraVertexContainer *pkVerts,
		FvUInt32 uiID,
		FvUInt32 uiMaxVerts,
		const FvMatrix &kObjectToWorld,
		const FvMatrix &kObjectToZone,
		FvBoundingBox &kBB );	

	virtual bool Load( FvXMLSectionPtr spSection,
		FvEcotype &kTarget )
	{
		for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
		{
			kTarget.TextureResource("",iTex);
			kTarget.pTexture(NULL,iTex);
		}
		return true;
	}

	bool IsEmpty() const
	{
		return true;
	}
};

class FvMeshsEcotype : public FvEcotypeGenerator
{
public:
	FvMeshsEcotype();
	bool Load( FvXMLSectionPtr spSection, FvEcotype &kTarget );
	FvUInt32 Generate(		
		const FvVector2 &kUVOffset,
		class FvFloraVertexContainer  *pkVerts,
		FvUInt32 uiID,
		FvUInt32 uiMaxVerts,
		const FvMatrix &kObjectToWorld,
		const FvMatrix &kObjectToZone,
		FvBoundingBox &kBB );
	bool IsEmpty() const;	

protected:	

	bool FindTextureResource(Ogre::MeshPtr spMesh, FvEcotype &kTarget);

	struct MeshCopy
	{
		bool Set( Ogre::MeshPtr spMesh, float fFlex, float fScaleVariation, class FvFlora *pkFlora );
		std::vector< FvFloraVertex > m_kVertices;
		float m_fScaleVariation;
	};

	typedef std::vector<MeshCopy> MeshCopies;
	MeshCopies m_kMeshs;
	float m_fDensity;
	class FvFlora *m_pkFlora;
};

class FvChooseMaxEcotype : public FvEcotypeGenerator
{
public:	
	~FvChooseMaxEcotype();
	bool Load( FvXMLSectionPtr spSection, FvEcotype &kTarget );
	FvUInt32 Generate(		
		const FvVector2 &kUVOffset,
		class FvFloraVertexContainer *pkVerts,
		FvUInt32 uiID,
		FvUInt32 uiMaxVerts,
		const FvMatrix &kObjectToWorld,
		const FvMatrix &kObjectToZone,
		FvBoundingBox &kBB );	

	class Function
	{
	public:
		virtual void Load(FvXMLSectionPtr spSection) = 0;
		virtual float operator() (const FvVector2 &kInput) = 0;
	};

	bool IsEmpty() const;	

private:
	static Function* CreateFunction( FvXMLSectionPtr spSection );
	FvEcotypeGenerator* ChooseGenerator(const FvVector2 &kPosition);
	typedef std::map< Function*, FvEcotypeGenerator* > SubTypes;
	SubTypes m_kSubTypes;
};

#endif // __FvEcotypeGenerators_H__