#include "FvEcotypeGenerators.h"
#include "FvFlora.h"
#include "FvFloraTexture.h"
#include "FvFloraRenderer.h"

#include <FvWatcher.h>
#include <FvPerlinNoise.h>

#include <OgreSubMesh.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>
#include <OgreMeshManager.h>
#include <OgreTextureManager.h>
#include <OgreTextureUnitState.h>

typedef FvEcotypeGenerator *(*EcotypeGeneratorCreator)();
typedef std::map< std::string, EcotypeGeneratorCreator > CreatorFns;
CreatorFns g_fnCreatorFns;

#define ECOTYPE_FACTORY_DECLARE( b ) FvEcotypeGenerator* b##Generator() \
{															 \
	return new b##Ecotype;									 \
}

#define REGISTER_ECOTYPE_FACTORY( a, b ) g_fnCreatorFns.insert( std::make_pair( a, b##Generator ) );


typedef FvChooseMaxEcotype::Function* (*FunctionCreator)();
typedef std::map< std::string, FunctionCreator > FunctionCreators;
FunctionCreators g_fnCreators;
#define FUNCTION_FACTORY_DECLARE( b ) FvChooseMaxEcotype::Function* b##Creator() \
{															 \
	return new b##Function;									 \
}

#define REGISTER_FUNCTION_FACTORY( a, b ) g_fnCreators.insert( std::make_pair( a, b##Creator ) );


FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

FvEcotypeGenerator *FvEcotypeGenerator::Create( FvXMLSectionPtr spSection,
										   FvEcotype &kTarget )
{	
	FvEcotypeGenerator *pkGen = NULL;
	FvXMLSectionPtr spInitSection = NULL;

	if (spSection)
	{
		if ( spSection->OpenSection("mesh") )
		{			
			pkGen = new FvMeshsEcotype();
			spInitSection = spSection;			
		}
		else if ( spSection->OpenSection("generator") )
		{			
			FvXMLSectionPtr spGen = spSection->OpenSection("generator");
			const FvString &kName = spGen->SectionName();

			CreatorFns::iterator kIt = g_fnCreatorFns.find(spGen->AsString());
			if ( kIt != g_fnCreatorFns.end() )
			{
				EcotypeGeneratorCreator pfGenCreator = kIt->second;
				pkGen = pfGenCreator();
				spInitSection = spGen;
			}
			else
			{
				FV_ERROR_MSG( "Unknown ecotype generator %s\n", kName.c_str() );
			}
		}

		if (pkGen)
		{
			if (pkGen->Load(spInitSection,kTarget) )
			{
				return pkGen;
			}
			else
			{
				delete pkGen;
				pkGen = NULL;
			}
		}
	}

	for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
	{
		kTarget.TextureResource("",iTex);
		kTarget.pTexture(NULL,iTex);
	}
	return new FvEmptyEcotype();
}

FvUInt32 FvEmptyEcotype::Generate(		
			const FvVector2 &kUVOffset,
			class FvFloraVertexContainer *pkVerts,
			FvUInt32 uiID,
			FvUInt32 uiMaxVerts,
			const FvMatrix &kObjectToWorld,
			const FvMatrix &kObjectToZone,
			FvBoundingBox &kBB)
{
	if (pkVerts)
		pkVerts->Clear( uiMaxVerts );
	return uiMaxVerts;
}

FvMeshsEcotype::FvMeshsEcotype():
m_fDensity( 1.f )
{	
}

bool FvMeshsEcotype::Load( FvXMLSectionPtr spSection, FvEcotype &kTarget)
{
	m_pkFlora = kTarget.Flora();

	m_fDensity = spSection->ReadFloat( "density", 1.f );

	for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
	{
		kTarget.TextureResource("",iTex);
		kTarget.pTexture(NULL,iTex);
	}
	std::vector<FvXMLSectionPtr> spSections;
	spSection->OpenSections( "mesh", spSections );

	for ( unsigned int i=0; i<spSections.size(); i++ )
	{
		FvXMLSectionPtr spCurr = spSections[i];

		Ogre::MeshPtr spMesh;
		try
		{
			spMesh = Ogre::MeshManager::getSingleton().load(
				spCurr->AsString(),
				Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME );
		}
		catch (...){}

		if ( !spMesh.isNull() )
		{
			float fFlex = spCurr->ReadFloat( "flex", 1.f );
			float fScaleVariation = spCurr->ReadFloat( "scaleVariation", 0.f );
			m_kMeshs.push_back( MeshCopy() );
			bool bOK = FindTextureResource( spMesh, kTarget );
			if (bOK) bOK = m_kMeshs.back().Set( spMesh, fFlex, fScaleVariation, m_pkFlora );
			if (!bOK)
			{
				FV_DEBUG_MSG( "FvMeshsEcotype - Removing %s because MeshCopy::Set failed\n", 
					spSections[i]->AsString().c_str() );
				m_kMeshs.pop_back();
			}

			Ogre::MeshManager::getSingleton().remove(spMesh->getHandle());
			spMesh.setNull();
		}
		else
		{
			FV_DEBUG_MSG( "FvMeshEcotype - %s does not exist\n", 
					spSections[i]->AsString().c_str() );
		}
	}

	return (m_kMeshs.size() != 0);
}

struct IndicesHolder
{
	IndicesHolder(
		Ogre::HardwareIndexBuffer::IndexType eType,
		FvUInt32 uiSize
		):
	m_uiSize(uiSize),
	m_eType(eType)
	{
		m_pkIndices = new FvUInt8[uiSize * (m_eType == Ogre::HardwareIndexBuffer::IT_16BIT?2:4)];
	}
	~IndicesHolder()
	{
		delete [] m_pkIndices;
	}
	void *m_pkIndices;
	FvUInt32 m_uiSize;
	Ogre::HardwareIndexBuffer::IndexType m_eType;

	FvUInt32 operator[]( FvInt32 iIndex ) const
	{
		return Get(iIndex);
	}

	void Set( FvInt32 iIndex, FvUInt32 uiValue )
	{
		if( m_eType == Ogre::HardwareIndexBuffer::IT_16BIT )
			( (FvUInt16*)m_pkIndices )[ iIndex ] = (FvUInt16)uiValue;
		else
			( (FvUInt32*)m_pkIndices )[ iIndex ] = uiValue;
	}

	FvUInt32 Get( FvInt32 iIndex ) const
	{
		if( m_eType == Ogre::HardwareIndexBuffer::IT_16BIT )
			return ( (FvUInt16*)m_pkIndices )[ iIndex ];
		else
			return ( (FvUInt32*)m_pkIndices )[ iIndex ];
	}
};

bool FvMeshsEcotype::MeshCopy::Set( Ogre::MeshPtr spMesh, float fFlex, float fScaleVariation, FvFlora *pkFlora )
{
	m_fScaleVariation = fScaleVariation;

	m_kVertices.clear();

	Ogre::SubMesh *pkSubMesh = NULL;
	if(spMesh->getNumSubMeshes() != 1 ||
		!(pkSubMesh = spMesh->getSubMesh(0)) ||
		pkSubMesh->useSharedVertices ||
		!pkSubMesh->vertexData ||
		!pkSubMesh->indexData ||
		!pkSubMesh->vertexData->vertexBufferBinding ||
		pkSubMesh->vertexData->vertexBufferBinding->getBufferCount() != 1 ||
		!pkSubMesh->vertexData->vertexDeclaration)
	{
		FV_ERROR_MSG("FvMeshsEcotype::MeshCopy::Set mesh:[%s] format error!\n",spMesh->getName().c_str());
		return false;
	}

	const Ogre::VertexElement *pkPosElement = NULL;
	const Ogre::VertexElement *pkNormalElement = NULL;
	const Ogre::VertexElement *pkUVElement = NULL;
	const Ogre::VertexElement *pkColorElement = NULL;
	Ogre::VertexDeclaration *pkDecl = pkSubMesh->vertexData->vertexDeclaration;

	for(FvUInt32 iElem = 0; iElem < pkDecl->getElementCount(); iElem++)
	{
		const Ogre::VertexElement *pkElement = pkDecl->getElement(iElem);
		if(!pkElement) continue;
		switch(pkElement->getSemantic())
		{
		case Ogre::VES_POSITION:
			pkPosElement = pkElement;
			break;
		case Ogre::VES_NORMAL:
			pkNormalElement = pkElement;
			break;
		case Ogre::VES_TEXTURE_COORDINATES:
			pkUVElement = pkElement;
			break;
		case Ogre::VES_DIFFUSE:
			pkColorElement = pkElement;
			break;
		}
	}

	if(pkPosElement == NULL || 
		pkNormalElement == NULL ||
		pkUVElement == NULL ||
		pkColorElement == NULL)
	{
		FV_ERROR_MSG("FvMeshsEcotype::MeshCopy::Set mesh:[%s] vertex format error!\n",spMesh->getName().c_str());
		return false;
	}

	const Ogre::HardwareIndexBufferSharedPtr &spIndexBuffer = pkSubMesh->indexData->indexBuffer;
	const Ogre::HardwareVertexBufferSharedPtr &spVertexBuffer = pkSubMesh->vertexData->vertexBufferBinding->getBuffer(0);
	FvUInt32 uiNumVerts = pkSubMesh->vertexData->vertexCount;
	FvUInt32 uiNumIndices = spIndexBuffer->getNumIndexes();
	FvVertexXYZNUVC *pkVerts = new FvVertexXYZNUVC[uiNumVerts];

	char *pkLockVerts = (char*)(spVertexBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
	for(FvUInt32 iVerts = 0; iVerts < uiNumVerts; iVerts++)
	{
		pkVerts[iVerts].m_kPos = *(FvVector3*)(pkLockVerts + pkPosElement->getOffset());
		pkVerts[iVerts].m_kNormal = *(FvVector3*)(pkLockVerts + pkNormalElement->getOffset());
		pkVerts[iVerts].m_kUV = *(FvVector2*)(pkLockVerts + pkUVElement->getOffset());
		pkVerts[iVerts].m_uiColor = *(FvUInt32*)(pkLockVerts + pkColorElement->getOffset());
		pkLockVerts += spVertexBuffer->getVertexSize();
	}
	spVertexBuffer->unlock();

	IndicesHolder kIndices(spIndexBuffer->getType(),uiNumIndices);
	void *pkBuffer = spIndexBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);
	memcpy(kIndices.m_pkIndices,pkBuffer,spIndexBuffer->getSizeInBytes());
	spIndexBuffer->unlock();

	m_kVertices.resize( uiNumIndices );

	FvBoundingBox kBB;
	kBB.SetBounds( FvVector3(0.f,0.f,0.f),
		FvVector3(0.f,0.f,0.f) );

	float fNumBlocksWide = float(pkFlora->FloraTexture()->BlocksWide());
	float fNumBlocksHigh = float(pkFlora->FloraTexture()->BlocksHigh());
	for ( FvUInt32 v=0; v<uiNumVerts; v++ )
	{
		pkVerts[v].m_kUV.x /= fNumBlocksWide;
		pkVerts[v].m_kUV.y /= fNumBlocksHigh;
		kBB.AddBounds( pkVerts[v].m_kPos );
	}

	for ( FvUInt32 i=0; i<uiNumIndices; i++ )
	{
		m_kVertices[i].Set( pkVerts[kIndices[i]] );
	}

	delete [] pkVerts;

	return true;
}

bool FvMeshsEcotype::FindTextureResource(Ogre::MeshPtr spMesh, FvEcotype &kTarget)
{
	FV_ASSERT(spMesh->getNumSubMeshes() == 1);
	Ogre::MaterialPtr spMaterial;
	try
	{
		spMaterial = Ogre::MaterialManager::getSingleton().getByName(
			spMesh->getSubMesh(0)->getMaterialName());
	}
	catch(...){}

	if(!spMaterial.isNull())
	{
		if(spMaterial->getNumTechniques() > 0 && 
			spMaterial->getTechnique(0)->getNumPasses() > 0 &&
			spMaterial->getTechnique(0)->getPass(0)->getNumTextureUnitStates() > 0)
		{
			bool bFailedToLoad = false;
			Ogre::Pass *pkPass = spMaterial->getTechnique(0)->getPass(0);
			for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
			{
				if(iTex >= pkPass->getNumTextureUnitStates()) break;
				Ogre::TextureUnitState *pkTexUnit = pkPass->getTextureUnitState(iTex);
				if(pkTexUnit)
				{
					kTarget.TextureResource(pkTexUnit->getTextureName(),iTex);
					FvD3DSystemMemoryTexturePtr spTexture = new FvD3DSystemMemoryTexture(pkTexUnit->getTextureName());
					kTarget.pTexture(spTexture,iTex);
					bFailedToLoad = spTexture->FailedToLoad();
				}
			}

			if(!bFailedToLoad)
				return true;
		}
	}

	for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
	{
		kTarget.TextureResource("",iTex);
		kTarget.pTexture(NULL,iTex);
	}
	FV_ERROR_MSG( "Could not find a texture property in the flora mesh %s\n", spMesh->getName().c_str() );
	return false;
}

FvUInt32 FvMeshsEcotype::Generate(	
	const FvVector2 &kUVOffset,
	FvFloraVertexContainer *pkVerts,
	FvUInt32 uiID,
	FvUInt32 uiMaxVerts,
	const FvMatrix &kObjectToWorld,
	const FvMatrix &kObjectToZone,
	FvBoundingBox &kBB )
{
	if ( m_kMeshs.size() )
	{			
		MeshCopy &kMesh = m_kMeshs[uiID%m_kMeshs.size()];

		FvUInt32 uiNumVerts = kMesh.m_kVertices.size();		

		if ( uiNumVerts > uiMaxVerts)
		{
			return 0;
		}

		float fRandomDensity = m_pkFlora->NextRandomFloat();
		float fRandomScale = m_pkFlora->NextRandomFloat();

		if ( fRandomDensity < m_fDensity )
		{
			if (pkVerts)
			{
				FvMatrix kScaledObjectToZone;
				FvMatrix kScaledObjectToWorld;

				float fScale = fabsf(fRandomScale) * kMesh.m_fScaleVariation + 1.f;
				kScaledObjectToZone.SetScale( fScale, fScale, fScale );
				kScaledObjectToZone.PostMultiply( kObjectToZone );
				kScaledObjectToWorld.SetScale( fScale, fScale, fScale );
				kScaledObjectToWorld.PostMultiply( kObjectToWorld );

				int iBlockX = abs((int)floorf(kObjectToWorld.ApplyToOrigin().x / FV_FLORA_BLOCK_WIDTH));
				int iBlockY = abs((int)floorf(kObjectToWorld.ApplyToOrigin().y / FV_FLORA_BLOCK_WIDTH));
				int iBlockNum = (iBlockX%7 + (iBlockY%7) * 7);
			
				pkVerts->UVOffset( kUVOffset.x, kUVOffset.y );
				pkVerts->BlockNum( iBlockNum );
				pkVerts->AddVertices( &*kMesh.m_kVertices.begin(), uiNumVerts, &kScaledObjectToZone );			

				for ( FvUInt32 i=0; i<uiNumVerts; i++ )
				{				
					kBB.AddBounds( kScaledObjectToWorld.ApplyPoint( *(FvVector3*)&kMesh.m_kVertices[i].m_kPos ) );
				}
			}

			return uiNumVerts;			
		}
		else
		{
			if (pkVerts)
			{
				pkVerts->Clear( uiNumVerts );
			}
			return uiNumVerts;
		}
	}

	return 0;
}

bool FvMeshsEcotype::IsEmpty() const
{
	return (m_kMeshs.size() == 0);
}

class FvNoiseFunction : public FvChooseMaxEcotype::Function
{
public:
	void Load( FvXMLSectionPtr spSection )
	{
		m_fFrequency = spSection->ReadFloat( "frequency", 10.f );
	}

	float operator() (const FvVector2 &kInput)
	{
		float fValue = m_kNoise.SumHarmonics2( kInput, 0.75f, m_fFrequency, 3.f );		
		return fValue;
	}

private:
	FvPerlinNoise m_kNoise;
	float m_fFrequency;
};

class FvRandomFunction : public FvChooseMaxEcotype::Function
{
public:
	void Load( FvXMLSectionPtr spSection )
	{		
	}

	float operator() (const FvVector2 &kInput)
	{
		return float(rand()) / (float)RAND_MAX;
	}

private:		
};

class FvFixedFunction : public FvChooseMaxEcotype::Function
{
public:
	void Load( FvXMLSectionPtr spSection )
	{
		m_fValue = spSection->ReadFloat( "value", 0.5f );
	}

	float operator() (const FvVector2 &kInput)
	{
		return m_fValue;
	}

private:
	float m_fValue;
};

FvChooseMaxEcotype::~FvChooseMaxEcotype()
{
	for (SubTypes::iterator kIt = m_kSubTypes.begin(); kIt != m_kSubTypes.end(); ++kIt)
	{
		delete kIt->first;
		delete kIt->second;
	}
}

FvChooseMaxEcotype::Function *FvChooseMaxEcotype::CreateFunction( FvXMLSectionPtr spSection )
{
	const FvString &kName = spSection->SectionName();	
	FunctionCreators::iterator it = g_fnCreators.find(kName);
	if ( it != g_fnCreators.end() )
	{
		FunctionCreator pfCreator = it->second;
		FvChooseMaxEcotype::Function *pkFun = pfCreator();
		pkFun->Load(spSection);
		return pkFun;
	}
	else
	{
		FV_ERROR_MSG( "Unknown FvChooseMaxEcotype::Function %s\n", kName.c_str() );
	}
	return NULL;
}

bool FvChooseMaxEcotype::Load( FvXMLSectionPtr spSection, FvEcotype &kTarget )
{
	for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
	{
		kTarget.TextureResource("",iTex);
		kTarget.pTexture(NULL,iTex);
	}

	FvXMLSectionIterator kIt = spSection->Begin();
	FvXMLSectionIterator kEnd = spSection->End();

	while (kIt != kEnd)
	{
		FvXMLSectionPtr spFunction = *kIt++;
		Function *pkFun = FvChooseMaxEcotype::CreateFunction(spFunction);		

		if (pkFun)
		{			
			std::vector<FvString> kOldTextureResources;
			kOldTextureResources.resize(FV_FLORA_TEXTURE_NUMBER,"");
			std::vector<FvD3DSystemMemoryTexturePtr> spTextures;
			spTextures.resize(FV_FLORA_TEXTURE_NUMBER,NULL);
			for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
			{
				kOldTextureResources[0] = kTarget.TextureResource(iTex);
				spTextures[0] = kTarget.pTexture(iTex);
			}

			FvEcotypeGenerator *pkEcotype = FvEcotypeGenerator::Create(spFunction,kTarget);
			if (kOldTextureResources[0] != "")
			{
				if (kTarget.TextureResource(0) != "" && kTarget.TextureResource(0) != kOldTextureResources[0])
				{
					FV_ERROR_MSG( "All ecotypes within a choose_max section must reference the same texture\n" );					
				}
				for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
				{
					kTarget.TextureResource( kOldTextureResources[iTex],iTex );
					kTarget.pTexture( spTextures[iTex],iTex );
				}
			}			

			m_kSubTypes.insert( std::make_pair(pkFun,pkEcotype) );		
		}
	}	

	return true;
}

FvUInt32 FvChooseMaxEcotype::Generate(
		const FvVector2 &kUVOffset,
		class FvFloraVertexContainer *pkVerts,
		FvUInt32 uiID,
		FvUInt32 uiMaxVerts,
		const FvMatrix &kObjectToWorld,
		const FvMatrix &kObjectToZone,
		FvBoundingBox &kBB )
{
	FvVector2 kPos2( kObjectToWorld.ApplyToOrigin().x, kObjectToWorld.ApplyToOrigin().y );
	FvEcotypeGenerator *pkGen = ChooseGenerator(kPos2);
	if (pkGen)	
		return pkGen->Generate(kUVOffset, pkVerts, uiID, uiMaxVerts, kObjectToWorld, kObjectToZone, kBB);	
	return 0;
}

FvEcotypeGenerator *FvChooseMaxEcotype::ChooseGenerator(const FvVector2 &kPos)
{
	FvEcotypeGenerator *pkChosen = NULL;
	float fBest = -0.1f;

	SubTypes::iterator kIt = m_kSubTypes.begin();
	SubTypes::iterator kEnd = m_kSubTypes.end();

	while (kIt != kEnd)
	{
		Function &kFun = *kIt->first;		
		float fCurr = kFun(kPos);
		if (fCurr>fBest)
		{
			fBest = fCurr;
			pkChosen = kIt->second;
		}
		kIt++;
	}

	return pkChosen;
}

bool FvChooseMaxEcotype::IsEmpty() const
{
	return false;
}

ECOTYPE_FACTORY_DECLARE( FvEmpty );
ECOTYPE_FACTORY_DECLARE( FvMeshs );
ECOTYPE_FACTORY_DECLARE( FvChooseMax );

FUNCTION_FACTORY_DECLARE( FvNoise );
FUNCTION_FACTORY_DECLARE( FvRandom );
FUNCTION_FACTORY_DECLARE( FvFixed );

bool RegisterFactories()
{
	REGISTER_ECOTYPE_FACTORY( "empty", FvEmpty );
	REGISTER_ECOTYPE_FACTORY( "mesh", FvMeshs );
	REGISTER_ECOTYPE_FACTORY( "chooseMax", FvChooseMax );

	REGISTER_FUNCTION_FACTORY( "noise", FvNoise );
	REGISTER_FUNCTION_FACTORY( "random", FvRandom );
	REGISTER_FUNCTION_FACTORY( "fixed", FvFixed );
	return true;
};
bool bAlwaysSuccessful = RegisterFactories();