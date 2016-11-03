#include "FvWater.h"
#include "FvWaterScene.h"
#include "FvRenderManager.h"

#include <FvWatcher.h>
#include <FvConcurrency.h>
#include <FvBGTaskManager.h>
#include <FvVectorNoDestructor.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

FvUInt32 FvWaters::ms_uiNextMark = 1;
FvUInt32 FvWaters::ms_uiLastDrawMark = -1;
static FvString s_kWaterMaterial = "FvWater";
static FvString s_kSimulationMaterial = "FvWaterSimulation";
static FvString s_kScreenFadeMap = "Environments/Water/fadetoblack.dds";
static bool s_bEnableDrawPrim = true;

Ogre::VertexData *FvWater::ms_pkVertexData = NULL;
Ogre::HardwareVertexBufferSharedPtr FvWater::ms_spQuadVertexBuffer;
bool FvWater::ms_bCullCells = true;
float FvWater::ms_fCullDistance = 1.2f;

float FvWater::ms_fMaxSimDistance = 200.0f;
float FvWater::ms_fSceneCullDistance = 300.f;
float FvWater::ms_fSceneCullFadeDistance = 30.f;

#ifndef FV_EDITOR_ENABLED
bool FvWater::ms_bBackgroundLoad = true;
#else // !FV_EDITOR_ENABLED
bool FvWater::ms_bBackgroundLoad = false;
#endif  // FV_EDITOR_ENABLED

Ogre::MaterialPtr FvWaters::ms_spMaterial;
Ogre::MaterialPtr FvWaters::ms_spSimulationMaterial;
std::set<FvWaterRefractionListener*> FvWaters::ms_kRefractionListeners;
FvString FvWaters::ms_kRefractionTextureName;
std::set<FvWaterDepthListener*> FvWaters::ms_kDepthListeners;
FvString FvWaters::ms_kDepthTextureName;
Ogre::Vector3 FvWaters::ms_kSunDirection;
Ogre::Vector3 FvWaters::ms_kSunColor;
bool FvWaters::ms_bDrawWaters = true;
bool FvWaters::ms_bDrawReflection = false;
bool FvWaters::ms_bSimulationEnabled = true;
bool FvWaters::ms_bEnableHDR = true;
bool FvWaters::ms_bHighQuality = true;
int	FvWaters::ms_iSimulationLevel = 2;
float FvWaters::ms_fAutoImpactInterval = 0.4f;

#ifdef FV_EDITOR_ENABLED
bool FvWaters::ms_bProjectView = false;
#endif


#ifdef FV_DEBUG_WATER

bool FvWater::ms_bDebugSim = true;
int FvWater::ms_iDebugCell = -1;
int FvWater::ms_iDebugCell2 = -1;

#define WATER_STAT(exp) exp

static unsigned int s_uiWaterCount = 0;
static unsigned int s_uiWaterVisCount = 0;
static unsigned int s_uiActiveCellCount = 0;
static unsigned int s_uiActiveEdgeCellCount = 0;
static unsigned int s_uiMovementCount = 0;

#else
#define WATER_STAT(exp)
#endif // FV_DEBUG_WATER

#define MAX_SIM_TEXTURE_SIZE 256
#define ACTIVITY_THRESHOLD 0.3f
#define RLE_KEY_VALUE 53
#define NM_RENDER_QUEUE_ID 80
#define DF_RENDER_QUEUE_ID 3
#define ENABLE_DEFERRED_SHADING ((FvRenderManager*)Ogre::Root::getSingleton()._getCurrentSceneManager())->GetDeferredShadingEnable()
FvUInt32 FvWaterCell::ms_uiWaterCellIdentify = 0;

FvWaterCell::FvWaterCell() : 
		m_uiNumIndices( 0 ),
		m_pkWater( 0 ),
		m_iXMax( 0 ),
		m_iYMax( 0 ),
		m_iXMin( 0 ),
		m_iYMin( 0 ),
		m_kMin(0,0),
		m_kMax(0,0),		
		m_kSize(0,0),
		m_pkVertexData(NULL),
		m_pkIndexData(NULL)
{
	ms_uiWaterCellIdentify++;

	for (int i=0; i<4; i++)
	{
		m_apkEdgeCells[i]=NULL;
	}
}

FvWaterCell::~FvWaterCell()
{
	FvWaters::RemoveDepthListener(this);
	FvWaters::RemoveRefractionListener(this);
}

int apcNeighbours[4] = { 2,3,4,5 };

void FvWaterCell::BindNeighbour( Ogre::Technique *pkTechnique, int iEdge )
{
	FV_ASSERT(iEdge<4 && iEdge>=0);
	if ( m_apkEdgeCells[iEdge] && m_apkEdgeCells[iEdge]->IsActive() )
	{
		m_apkEdgeCells[iEdge]->BindAsNeighbour( pkTechnique, apcNeighbours[iEdge] );
	}
	else if (!FvSimulationManager::Instance().NullSim().isNull())
	{
		Ogre::TextureUnitState *pkTex = pkTechnique->getPass(0)->getTextureUnitState(
			apcNeighbours[iEdge]);
		FV_ASSERT(pkTex);
		pkTex->setTextureName(FvSimulationManager::Instance().NullSim()->getName());
	}
	else
	{
		Ogre::TextureUnitState *pkTex = pkTechnique->getPass(0)->getTextureUnitState(
			apcNeighbours[iEdge]);
		FV_ASSERT(pkTex);
		pkTex->setTextureName("");
	}
}

void FvWaterCell::ActivateEdgeCell( unsigned int iIndex )
{
	this->AddEdge((iIndex+2) % 4);
	this->ResetIdleTimer();
	this->m_bEdgeActivation = true;
	this->Perturbed( true );
}

void FvWaterCell::CheckActivity( SimulationCellPtrList &kActiveList,
								WaterCellPtrList &kEdgeList )
{
	if (HasMovements())
		Perturbed(true);
	else if ( !m_bEdgeActivation ) 
		Perturbed(false);

	if (ShouldActivate())
	{
		Activate();
		kActiveList.push_back( this );
	}
	else 
	{
		bool bOutOfRange = false;
		if (IsActive())
		{
			bOutOfRange = (m_fCellDistance > FvWater::ms_fMaxSimDistance);			

			if (bOutOfRange)
			{
				ClearMovements();
				Perturbed(false);
			}
		}

		if ( bOutOfRange || ShouldDeactivate() )
		{
			Deactivate();
			kActiveList.remove( this );
			m_bEdgeActivation = false;
		}
	}

	if (FvWaters::Instance().SimulationLevel() > 1) 
	{
		if (IsActive())
		{
			if (m_bEdgeActivation == false)
				Edge(0);

			int iEdges = GetActiveEdge(ACTIVITY_THRESHOLD);

			if (Movements().size())
			{
				this->Edge(iEdges);
				if (iEdges > 0)
				{
					WaterCellPtrList kCornerCells;

					for (int i=0; i<4; i++)
					{
						FvWaterCell *pkEdgeCell = m_apkEdgeCells[i];
						if (pkEdgeCell && (iEdges & (1<<i)))
						{
							pkEdgeCell->ActivateEdgeCell( i );
							kEdgeList.push_back( pkEdgeCell );

							m_bEdgeActivation = true;

							unsigned int uiCornerIdx = (i+1) % 4;
							if ( (iEdges & (1<<(uiCornerIdx))) )
							{
								FvWaterCell *pkCornerCell1 = pkEdgeCell->EdgeCell( uiCornerIdx );
								FvWaterCell *pkCornerCell2 = m_apkEdgeCells[ uiCornerIdx ];
								if ( pkCornerCell1 )
								{
									pkEdgeCell->AddEdge( uiCornerIdx );
									if (pkCornerCell2)
									{
										pkCornerCell1->AddEdge( (uiCornerIdx+1) % 4 );
										pkCornerCell2->AddEdge( i );
									}
									pkCornerCell1->ActivateEdgeCell( uiCornerIdx );
									kCornerCells.push_back( pkCornerCell1 );
								}
							}
						}
					}

					kEdgeList.insert( kEdgeList.end(), kCornerCells.begin(), kCornerCells.end() );
				}
			}

			if (m_bEdgeActivation)
			{
				kEdgeList.push_back( this );

				bool bDeactivate = !HasMovements();
				for (int i=0; i<4 && bDeactivate; i++)
				{
					if ( m_apkEdgeCells[i] && 
							m_apkEdgeCells[i]->IsActive() )
					{
						if ( m_apkEdgeCells[i]->HasMovements() )
							bDeactivate = false;
					}
				}
				if (bDeactivate)
					Perturbed( false );
			}
		}
		else if (m_bEdgeActivation)
		{
			Perturbed( false );
			kEdgeList.push_back( this );
		}
	}
}

void FvWaterCell::CheckEdgeActivity( SimulationCellPtrList &kActiveList )
{
	if (m_bEdgeActivation)
	{
		if (HasMovements())
			Perturbed(true);			
		if (ShouldActivate())
		{
			Activate();
			kActiveList.push_back( this );
		}
		else if (ShouldDeactivate())
		{
			m_bEdgeActivation = false;
			Deactivate();
			kActiveList.remove( this );
		}
	}
}

void FvWaterCell::UpdateDistance(const FvVector3 &kCamPos)
{
	FvBoundingBox kBox = m_pkWater->BB();
	FvVector3 kCellMin(Min().x, Min().y, 0);
	FvVector3 kCellMax(Max().x, Max().y, 0);

	kBox.SetBounds(	kBox.MinBounds() + kCellMin,
					kBox.MinBounds() + kCellMax );
	m_fCellDistance = kBox.Distance(kCamPos);
}

bool FvWaterCell::Init( FvWater *pkWater, FvVector2 kStart, FvVector2 kSize )
{ 
	if (!pkWater)
		return false;

	m_pkWater = pkWater;

	m_kMin = kStart;
	m_kMax = (kStart+kSize);
	m_kSize = kSize;

	m_iXMin = int( ceilf( Min().x / m_pkWater->m_kConfig.m_fTessellation)  );
	m_iYMin = int( ceilf( Min().y / m_pkWater->m_kConfig.m_fTessellation)  );

	m_iXMax = int( ceilf( (Max().x) / m_pkWater->m_kConfig.m_fTessellation)  ) + 1;
	m_iYMax = int( ceilf( (Max().y) / m_pkWater->m_kConfig.m_fTessellation)  ) + 1;

	if (m_iXMax > int( m_pkWater->m_uiGridSizeX ))
		m_iXMax = int( m_pkWater->m_uiGridSizeX );

	if (m_iYMax > int( m_pkWater->m_uiGridSizeY ))
		m_iYMax = int( m_pkWater->m_uiGridSizeY );

	return true;
}

void FvWaterCell::DeleteManaged()
{
	OGRE_DELETE m_pkVertexData;
	m_pkVertexData = NULL;
	OGRE_DELETE m_pkIndexData;
	m_pkIndexData = NULL;
	if(!m_spMaterial.isNull())
	{
		Ogre::MaterialManager::getSingleton().remove(m_spMaterial->getHandle());
		m_spMaterial.setNull();
	}
}

void FvWaterCell::UpdateRefractionTexture(const FvString& kText)
{
	if(!m_spMaterial.isNull())
	{
		for(unsigned short i = 0; i < m_spMaterial->getNumTechniques(); i++)
		{
			Ogre::Technique *pkTech = m_spMaterial->getTechnique(i);
			FV_ASSERT(pkTech && pkTech->getPass(0));
			Ogre::TextureUnitState *pkTex = pkTech->getPass(0)->getTextureUnitState(1);
			FV_ASSERT(pkTex);
			pkTex->setTextureName(kText);
		}
	}
}

void FvWaterCell::UpdateDepthTexture(const FvString& kText)
{
	if(!m_spMaterial.isNull())
	{
		for(unsigned short i = 0; i < m_spMaterial->getNumTechniques(); i++)
		{
			Ogre::Technique *pkTech = m_spMaterial->getTechnique(i);
			FV_ASSERT(pkTech && pkTech->getName().size() >= 2 && pkTech->getPass(0));
			if(pkTech->getName()[1] > 3)
			{
				Ogre::TextureUnitState *pkTex = pkTech->getPass(0)->getTextureUnitState(4);
				FV_ASSERT(pkTex);
				pkTex->setTextureName(kText);
			}
		}
	}
}

const Ogre::MaterialPtr &FvWaterCell::getMaterial(void) const
{
	return m_spMaterial;
}

void FvWaterCell::getRenderOperation(Ogre::RenderOperation &kOP)
{
	kOP.operationType = Ogre::RenderOperation::OT_TRIANGLE_STRIP;
	kOP.useIndexes = true;
	kOP.vertexData = m_pkVertexData;
	kOP.indexData = m_pkIndexData;
}

Ogre::Technique* FvWaterCell::getTechnique(void) const
{
	static char acTechName[3] = {1,1,0};
	bool bFoam = ENABLE_DEFERRED_SHADING?true:false;
	bool bCube = FvWaters::DrawReflection()?m_pkWater->m_kConfig.m_bUseCubeMap:true;

	acTechName[0] = ENABLE_DEFERRED_SHADING?(bFoam?4:3):(bFoam?2:1);
	acTechName[1] = ENABLE_DEFERRED_SHADING?
		(m_pkWater->m_bRaining?(bCube?(bFoam?15:14):(bFoam?13:12)):(SimulationActive()?(bCube?(bFoam?11:10):(bFoam?9:8)):(bCube?(bFoam?7:6):(bFoam?5:4)))):
		(m_pkWater->m_bRaining?(3):(SimulationActive()?2:1));
	return m_spMaterial->getTechnique(acTechName);
}

void FvWaterCell::getWorldTransforms(Ogre::Matrix4 *pkXForm) const
{
	(*pkXForm)[0][0] = m_pkWater->m_kTransform[0][0];
	(*pkXForm)[1][0] = m_pkWater->m_kTransform[0][1];
	(*pkXForm)[2][0] = m_pkWater->m_kTransform[0][2];
	(*pkXForm)[3][0] = m_pkWater->m_kTransform[0][3];

	(*pkXForm)[0][1] = m_pkWater->m_kTransform[1][0];
	(*pkXForm)[1][1] = m_pkWater->m_kTransform[1][1];
	(*pkXForm)[2][1] = m_pkWater->m_kTransform[1][2];
	(*pkXForm)[3][1] = m_pkWater->m_kTransform[1][3];

	(*pkXForm)[0][2] = m_pkWater->m_kTransform[2][0];
	(*pkXForm)[1][2] = m_pkWater->m_kTransform[2][1];
	(*pkXForm)[2][2] = m_pkWater->m_kTransform[2][2];
	(*pkXForm)[3][2] = m_pkWater->m_kTransform[2][3];

	(*pkXForm)[0][3] = m_pkWater->m_kTransform[3][0];
	(*pkXForm)[1][3] = m_pkWater->m_kTransform[3][1];
	(*pkXForm)[2][3] = m_pkWater->m_kTransform[3][2];
	(*pkXForm)[3][3] = m_pkWater->m_kTransform[3][3];
}

Ogre::Real FvWaterCell::getSquaredViewDepth(const Ogre::Camera *pkCam) const
{
	return 0.f;
}

const Ogre::LightList &FvWaterCell::getLights(void) const
{
	static Ogre::LightList s_kLightList;
	return s_kLightList;
}

FvUInt32 FvWater::RemapVertex( FvUInt32 iIndex )
{
	FvUInt32 uiNewIndex = iIndex;
	if (m_kRemappedVerts.size())
	{
		std::map<FvUInt32, FvUInt32> &kCurrentMap = m_kRemappedVerts.back();
		std::map<FvUInt32,FvUInt32>::iterator kIt = kCurrentMap.find(uiNewIndex);
		if (kIt == kCurrentMap.end())
		{
			uiNewIndex = (FvUInt32)kCurrentMap.size();
			kCurrentMap[iIndex] = uiNewIndex;
		}
		else
		{
			uiNewIndex = kIt->second;
		}
	}
	return uiNewIndex;
}

template< class T >
FvUInt32 FvWater::Remap( std::vector<T> &kDestIndices,
					const std::vector<FvUInt32> &kSrcIndices )
{
	FvUInt32 uiMaxVerts = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities()->getMaxVertexIndex();
	if (m_kRemappedVerts.size() == 0)
	{
		m_kRemappedVerts.resize(1);
	}
	kDestIndices.resize(kSrcIndices.size());

	for (unsigned int i=0; i<kSrcIndices.size() ; i++)
	{
		kDestIndices[i] = (T)this->RemapVertex( kSrcIndices[i] );
	}

	if ( m_kRemappedVerts.back().size() > uiMaxVerts)
	{
		m_kRemappedVerts.push_back(std::map<FvUInt32, FvUInt32>());	
		for (unsigned int i=0; i<kSrcIndices.size() ; i++)
		{
			kDestIndices[i] = (T)this->RemapVertex( kSrcIndices[i] );
		}

		if ( m_kRemappedVerts.back().size() > uiMaxVerts )
		{
			FV_ERROR_MSG("FvWater::Remap( ): Maximum vertex count "
				"exceeded, please INCREASE the \"Mesh Size\" parameter.\n" );
			m_kRemappedVerts.pop_back();
			return 0;
		}
	}
	return m_kRemappedVerts.size();
}

template< class T >
void FvWaterCell::BuildIndices()
{
	#define WIDX_CHECKMAX  uiMaxIndex = kIndices32.back() > uiMaxIndex ? \
									kIndices32.back() : uiMaxIndex;

	std::vector< T >		kIndices;
	std::vector< FvUInt32 >	kIndices32;

	int iLastIndex = -1;
	bool bInstrip = false;
	FvUInt32 uiGridIndex = 0;
	FvUInt32 uiMaxIndex=0;
	FvUInt32 uiMaxVerts = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities()->getMaxVertexIndex();
	Ogre::HardwareIndexBuffer::IndexType eFormat;
	FvUInt32 uiSize = sizeof(T);

	if (uiSize == 2)
		eFormat = Ogre::HardwareIndexBuffer::IT_16BIT;
	else
		eFormat = Ogre::HardwareIndexBuffer::IT_32BIT;

	uiGridIndex = m_iXMin + (m_pkWater->m_uiGridSizeX*m_iYMin);
	for (unsigned int uiY = unsigned int(m_iYMin); uiY < unsigned int(m_iYMax) - 1; uiY++)
	{
		for (unsigned int uiX = unsigned int(m_iXMin); uiX < unsigned int(m_iXMax); uiX++)
		{
			bool bLastX = (uiX == m_iXMax - 1);
			FvUInt32 uiNewGridIndex = (uiGridIndex - (uiX - m_iXMin)) + ((m_iXMax - m_iXMin - 1) - (uiX - m_iXMin));

			if (!bInstrip && !bLastX &&
				m_pkWater->m_kWaterIndex[ uiNewGridIndex ] >= 0 &&
				m_pkWater->m_kWaterIndex[ uiNewGridIndex - 1 ] >= 0 &&
				m_pkWater->m_kWaterIndex[ uiNewGridIndex + m_pkWater->m_uiGridSizeX ] >= 0 &&
				m_pkWater->m_kWaterIndex[ uiNewGridIndex + m_pkWater->m_uiGridSizeX - 1 ] >= 0
				)
			{
				if (iLastIndex == -1)
					iLastIndex = m_pkWater->m_kWaterIndex[ uiNewGridIndex ];

				kIndices32.push_back( FvUInt32( iLastIndex ) );

				kIndices32.push_back( FvUInt32( m_pkWater->m_kWaterIndex[ uiNewGridIndex ] ));
				WIDX_CHECKMAX

				kIndices32.push_back( FvUInt32( m_pkWater->m_kWaterIndex[ uiNewGridIndex ] ));
				WIDX_CHECKMAX

				kIndices32.push_back( FvUInt32( m_pkWater->m_kWaterIndex[ uiNewGridIndex + m_pkWater->m_uiGridSizeX] ));
				WIDX_CHECKMAX

				bInstrip = true;
			}
			else
			{
				if (m_pkWater->m_kWaterIndex[ uiNewGridIndex ] >= 0 &&
					m_pkWater->m_kWaterIndex[ uiNewGridIndex + m_pkWater->m_uiGridSizeX] >= 0  &&
					bInstrip)
				{
					kIndices32.push_back( FvUInt32(m_pkWater->m_kWaterIndex[ uiNewGridIndex ] ) );
					WIDX_CHECKMAX

					kIndices32.push_back( FvUInt32(m_pkWater->m_kWaterIndex[ uiNewGridIndex + m_pkWater->m_uiGridSizeX]));
					WIDX_CHECKMAX

					iLastIndex = kIndices32.back();
				}
				else
					bInstrip = false;
			}

			if (bLastX)
				bInstrip = false;

			++uiGridIndex;
		}
		uiGridIndex += (m_pkWater->m_uiGridSizeX - m_iXMax) + m_iXMin;
	}

	bool bRemap = (uiMaxIndex > uiMaxVerts);
	if (bRemap)
	{
		m_uiBufferIndex = m_pkWater->Remap<T>( kIndices, kIndices32 );
		if (m_uiBufferIndex == 0) 
		{
			m_uiNumIndices = 0;		
			return;
		}
	}
	else
	{
		m_uiBufferIndex = 0;
		kIndices.resize( kIndices32.size() );
		for (unsigned int i=0; i<kIndices32.size() ; i++)
		{
			kIndices[i] = (T)kIndices32[i];		
		}
	}

	if (kIndices.size() > 2)
	{
		kIndices.erase( kIndices.begin(), kIndices.begin() + 2 );
		m_uiNumIndices = kIndices.size();

		m_pkIndexData->indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(eFormat,m_uiNumIndices,Ogre::HardwareBuffer::HBU_WRITE_ONLY);
		if( !m_pkIndexData->indexBuffer.isNull() )
		{
			T *pkData = static_cast<T*>(m_pkIndexData->indexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));
			if(pkData)
			{
				memcpy(pkData,&kIndices.front(),m_uiNumIndices * sizeof(T));
				m_pkIndexData->indexBuffer->unlock();
			}
		}
	}
#undef WIDX_CHECKMAX
}

void FvWaterCell::CreateManagedIndices()
{
	if (!m_pkWater)
		return;

	FV_ASSERT(m_pkVertexData == NULL);
	m_pkVertexData = OGRE_NEW Ogre::VertexData;
	Ogre::VertexDeclaration *pkDecl = m_pkVertexData->vertexDeclaration;
	size_t stOffset = 0;
	pkDecl->addElement(0, stOffset, Ogre::VET_FLOAT3, Ogre::VES_POSITION, 0);
	stOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
	pkDecl->addElement(0, stOffset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE, 0);
	stOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);
	pkDecl->addElement(0, stOffset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);
	m_pkVertexData->vertexStart = 0;

	FV_ASSERT(m_pkIndexData == NULL);
	m_pkIndexData = OGRE_NEW Ogre::IndexData;
	m_pkIndexData->indexStart = 0;

	if(m_spMaterial.isNull())
	{
		char pcIdentify[9];
		sprintf_s(pcIdentify,9,"%I32x",FvInt32(ms_uiWaterCellIdentify));
		FvString kMaterialName = "FvWaterCellMaterial_";
		kMaterialName += pcIdentify;
		m_spMaterial = FvWaters::ms_spMaterial->clone(kMaterialName);

		FvWaters::AddDepthListener(this);
		FvWaters::AddRefractionListener(this);
	}

	if (Ogre::Root::getSingleton().getRenderSystem()->getCapabilities()->getMaxVertexIndex() <= 0xffff)
		BuildIndices<FvUInt16>();
	else
		BuildIndices<FvUInt32>();
}

namespace 
{
static FvSimpleMutex *s_pkDeletionLock = NULL;
};

FvWater::FvWater( const WaterState &kConfig, FvEnvironmentColliderPtr spCollider )
:	m_kConfig( kConfig ),
	m_uiGridSizeX( int( ceilf( kConfig.m_kSize.x / kConfig.m_fTessellation ) + 1 ) ),
	m_uiGridSizeY( int( ceilf( kConfig.m_kSize.y / kConfig.m_fTessellation ) + 1 ) ),
	m_fTime( 0 ),
	m_fLastDTime( 1.f ),
	m_spCollider( spCollider ),
	/*m_pkWaterScene( NULL),*/
	m_bDrawSelf( true ),
	//m_spSimulationMaterial( NULL ),
	m_bReflectionCleared( false ),
	//m_pkParamCache( new EffectParameterCache() ),
#ifdef FV_EDITOR_ENABLED
	m_bDrawRed( false ),
	m_bHighlight( false ),
#endif
	m_bVisible( true ),
	m_bCreatedCells( false ),
	m_bInitialised( false ),
	m_bEnableSim( true ),
	m_uiDrawMark( 0 ),
	m_bVisited(false),
	m_fTexAnim( 0.f ),
	m_fSimpleReflection( 0.0f ),
	m_kBB(FvVector3(0.f,0.f,0.f),FvVector3(0.f,0.f,0.f))
{
	if (!s_pkDeletionLock)
		s_pkDeletionLock = new FvSimpleMutex;

	FvWaters::Instance().push_back(this);

	m_kWaterRigid.resize( m_uiGridSizeX * m_uiGridSizeY, false );
	m_kWaterAlpha.resize( m_uiGridSizeX * m_uiGridSizeY, 0 );

	m_kTransform.SetRotateZ( m_kConfig.m_fOrientation );
	m_kTransform.PostTranslateBy( m_kConfig.m_kPosition );
	m_kInvTransform.Invert( m_kTransform );

    if (FvWater::BackgroundLoad())
    {
	    FvBGTaskManager::Instance().AddBackgroundTask(
			new FvCStyleBackgroundTask( 
			    &FvWater::DoCreateTables, this,
			    &FvWater::OnCreateTablesComplete, this ) );
    }
    else
    {
	    FvWater::DoCreateTables( this );
	    FvWater::OnCreateTablesComplete( this );
    }

	static bool s_bFirst = true;
	if (s_bFirst)
	{
		s_bFirst = false;
#ifdef FV_DEBUG_WATER
		FV_WATCH( "Client Settings/Water/debug cell", ms_iDebugCell, 
				FvWatcher::WT_READ_WRITE,
				"simulation debugging?" );
		FV_WATCH( "Client Settings/Water/debug cell2", ms_iDebugCell2, 
				FvWatcher::WT_READ_WRITE,
				"simulation debugging?" );
		FV_WATCH( "Client Settings/Water/debug sim", ms_bDebugSim, 
				FvWatcher::WT_READ_WRITE,
				"simulation debugging?" );
#endif
		FV_WATCH( "Client Settings/Water/cell cull enable", ms_bCullCells, 
				FvWatcher::WT_READ_WRITE,
				"enable cell culling?" );
		FV_WATCH( "Client Settings/Water/cell cull fDist", ms_fCullDistance, 
				FvWatcher::WT_READ_WRITE,
				"cell culling distance (far plane)" );

		FV_WATCH( "Render/Performance/DrawPrim WaterCell", s_bEnableDrawPrim,
			FvWatcher::WT_READ_WRITE,
			"Allow WaterCell kTo call drawIndexedPrimitive()." );
	}

	//m_spPyVolume = new PyWaterVolume( this );
}

FvWater::~FvWater()
{
#if FV_UMBRA_ENABLE
	m_kTerrainItems.clear();
#endif // FV_UMBRA_ENABLE

	this->DeleteManagedObjects();
	this->DeleteUnmanagedObjects();
}

bool FvWater::StillValid(FvWater* water)
{
	if (!water)
	{
		return false;
	}

	if (FvWater::BackgroundLoad())
	{
		if (s_pkDeletionLock)
		{
			s_pkDeletionLock->Grab();

			std::vector< FvWater* >::iterator it =
				std::find(	FvWaters::Instance().begin(),
							FvWaters::Instance().end(), water );	

			if (it == FvWaters::Instance().end())
			{
				s_pkDeletionLock->Give();
				return false;
			}
		}
		else
			return false;
	}
	return true;
}

void FvWater::DeleteWater(FvWater *pkWater)
{
	if (s_pkDeletionLock)
	{
		s_pkDeletionLock->Grab();
	
		std::vector< FvWater* >::iterator it = std::find( FvWaters::Instance().begin( ), FvWaters::Instance().end( ), pkWater );
		FvWaters::Instance().erase(it);

		delete pkWater;

		bool deleteMutex = (FvWaters::Instance().size() == 0);
		s_pkDeletionLock->Give();
		if (deleteMutex)
		{
			delete s_pkDeletionLock;
			s_pkDeletionLock = NULL;
		}
	}
	else
	{
		std::vector< FvWater* >::iterator it = std::find( FvWaters::Instance().begin( ), FvWaters::Instance().end( ), pkWater );
		FvWaters::Instance().erase(it);

		delete pkWater;
	}
}

void FvWater::Rebuild( const WaterState &kConfig )
{
	m_bInitialised = false;
	m_kVertexBuffers.clear();

	DeleteManagedObjects();

	m_kConfig = kConfig;
	m_uiGridSizeX = int( ceilf( kConfig.m_kSize.x / kConfig.m_fTessellation ) + 1 );
	m_uiGridSizeY = int( ceilf( kConfig.m_kSize.y / kConfig.m_fTessellation ) + 1 );

	m_kTransform.SetRotateZ( m_kConfig.m_fOrientation );
	m_kTransform.PostTranslateBy( m_kConfig.m_kPosition );
	m_kInvTransform.Invert( m_kTransform );

	if( m_kWaterRigid.size() != m_uiGridSizeX * m_uiGridSizeY )
	{
		m_kWaterRigid.clear();
		m_kWaterAlpha.clear();

		m_kWaterRigid.resize( m_uiGridSizeX * m_uiGridSizeY, false );
		m_kWaterAlpha.resize( m_uiGridSizeX * m_uiGridSizeY, 0 );
	}

	if (m_kConfig.m_bUseEdgeAlpha)
		BuildTransparencyTable();
	else
	{
		m_kWaterRigid.assign( m_kWaterRigid.size(), false );
		m_kWaterAlpha.assign( m_kWaterAlpha.size(), 0 );
	}

	m_kWaterIndex.clear();
	m_kCells.clear();
	m_kActiveSimulations.clear();
	m_uiNumVertices = CreateIndices();
	
#ifdef FV_EDITOR_ENABLED
	m_bEnableSim = true; 
#else
	m_bEnableSim = FvWaters::SimulationEnabled();
#endif

	m_bEnableSim = m_bEnableSim && 
		Ogre::Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_TEXTURE_FLOAT) && 
		m_kConfig.m_bUseSimulation;

	CreateCells();

	Setup2ndPhase();

	StartResLoader();
}

FvUInt32 FvWater::CreateIndices( )
{
	FvUInt32 uiIndex = 0;
	FvInt32 iWaterIndex = 0;
	for (int iYIndex = 0; iYIndex < int( m_uiGridSizeY ); iYIndex++ )
	{
		for (int iXIndex = 0; iXIndex < int( m_uiGridSizeX ); iXIndex++ )
		{
			int iRealNeighbours = 0;
			for (int iCY = iYIndex - 1; iCY <= iYIndex + 1; iCY++ )
			{
				for (int iCX = iXIndex - 1; iCX <= iXIndex + 1; iCX++ )
				{
					if (( iCX >= 0 && iCX < int( m_uiGridSizeX ) ) &&
						( iCY >= 0 && iCY < int( m_uiGridSizeY ) ))
					{
						if (!m_kWaterRigid[ iCX + ( iCY * m_uiGridSizeX ) ])
						{
							iRealNeighbours++;
						}
					}
				}
			}

			if (iRealNeighbours > 0)
				m_kWaterIndex.push_back(iWaterIndex++);
			else
				m_kWaterIndex.push_back(-1);
		}
	}

#ifdef FV_EDITOR_ENABLED
	if (iWaterIndex > 0xffff)
	{
		Commentary::instance().addMsg( "WARNING! Water surface contains excess"
				" vertices, please INCREASE the \"Mesh Size\" parameter", 1 );
	}
#endif // FV_EDITOR_ENABLED

	return iWaterIndex;
}



#ifdef FV_EDITOR_ENABLED

void FvWater::DeleteData( )
{
	FvString fileName = m_kConfig.m_kTransparencyTable;
	::DeleteFile( BWResource::resolveFilename( fileName ).c_str() );
}

template<class C> void WriteVector( FvString &kData, std::vector<C> &kVec )
{
	int iCLen = sizeof( C );
	int iCLen = iCLen * kVec.size();
	kData.append( (char*) &kVec.front(), iCLen );
}

void WriteValue(FvString &kData, FvUInt32 uiValue )
{
	kData.push_back( char( (uiValue & (0xff << 0 ))    ) );
	kData.push_back( char( (uiValue & (0xff << 8 ))>>8 ) );
	kData.push_back( char( (uiValue & (0xff << 16))>>16) );
	kData.push_back( char( (uiValue & (0xff << 24))>>24) );
}

void WriteValue(FvString &kData, bool bValue )
{
	kData.push_back( char( bValue ) );
}

template <class C>
void CompressVector( FvString &kData, std::vector<C> &kVec )
{
	FV_ASSERT(kVec.size());
	FvUInt32 uiMax = kVec.size();	
	FvUInt32 uiPrevI = 0;
	bool bFirst = true;

	C kCurV = kVec[0];
	C kPrevV = !kCurV;
	for (FvUInt32 i=0; i<uiMax; i++)
	{
		kCurV = kVec[i];
		FvUInt32 cur_i = i;    
		FvUInt32 c = cur_i - uiPrevI;

		if ( (kCurV != kPrevV) || (c > 254) || (i==(uiMax-1)) )
		{
			if (kPrevV == static_cast<C>(RLE_KEY_VALUE)) 
			{
				kData.push_back(static_cast<char>(RLE_KEY_VALUE));
				WriteValue(kData, kPrevV);
				kData.push_back( static_cast<char>(c) );
			}
			else
			{
				if (c > 2)
				{
					kData.push_back(static_cast<char>(RLE_KEY_VALUE));					
					WriteValue(kData, kPrevV);
					kData.push_back( static_cast<char>(c) );
				}
				else
				{
					if (bFirst==false)
					{
						if ( c > 1 )
							WriteValue(kData, kPrevV);
						WriteValue(kData, kPrevV);
					}
				}
			}
			uiPrevI = cur_i;
		}
		kPrevV = kCurV;
		bFirst=false;
	}
	WriteValue(kData, kCurV);
}

void FvWater::SaveTransparencyTable( )
{
	FvString kData1;
	unsigned int uiMaxSize = kData1.max_size();
.
	CompressVector<FvUInt32>(kData1, m_kWaterAlpha);
	FvString kData2;
	CompressVector<bool>(kData2, m_kWaterRigid);

	BinaryPtr spBinaryBlockAlpha = new BinaryBlock( kData1.data(), kData1.size(), "BinaryBlock/Water" );
	BinaryPtr spBinaryBlockRigid = new BinaryBlock( kData2.data(), kData2.size(), "BinaryBlock/Water" );

	FvString kDataName = m_kConfig.m_kTransparencyTable;
	FvXMLSectionPtr spSection = BWResource::openSection( kDataName, false );
	if ( !spSection )
	{
		size_t stLastSep = kDataName.find_last_of('/');
		FvString kParentName = kDataName.substr(0, stLastSep);
		FvXMLSectionPtr spParentSection = BWResource::openSection( kParentName );
		FV_ASSERT(spParentSection);

		FvString kTagName = kDataName.substr(stLastSep + 1);

		spSection = new BinSection( kTagName, new BinaryBlock( NULL, 0, "BinaryBlock/Water" ) );
		spSection->SetParent( spParentSection );
		spSection = DataSectionCensus::add( kDataName, spSection );
	}


	FV_ASSERT( spSection );
	spSection->DelChild( "alpha" );
	FvXMLSectionPtr spAlphaSection = spSection->OpenSection( "alpha", true );
	spAlphaSection->SetParent(spSection);
	if ( !spSection->WriteBinary( "alpha", spBinaryBlockAlpha ) )
	{
		FV_CRITICAL_MSG( "FvWater::SaveTransparencyTable - error while writing BinSection in %s/alpha\n", kDataName.c_str() );
		return;
	}

	spSection->DelChild( "rigid" );
	FvXMLSectionPtr spRigidSection = spSection->OpenSection( "rigid", true );
	spRigidSection->SetParent(spSection);
	if ( !spSection->WriteBinary( "rigid", spBinaryBlockRigid ) )
	{
		FV_CRITICAL_MSG( "FvWater::SaveTransparencyTable - error while writing BinSection in %s/rigid\n", kDataName.c_str() );
		return;
	}

	spSection->DelChild( "iVersion" );
	FvXMLSectionPtr spVersionSection = spSection->OpenSection( "iVersion", true );
	int iVersion = 2;
	BinaryPtr binaryBlockVer = new BinaryBlock( &iVersion, sizeof(int), "BinaryBlock/Water" );
	spVersionSection->SetParent(spSection);
	if ( !spSection->writeBinary( "iVersion", binaryBlockVer ) )
	{
		FV_CRITICAL_MSG( "FvWater::SaveTransparencyTable - error while writing BinSection in %s/iVersion\n", kDataName.c_str() );
		return;
	}
	spVersionSection->save();

	spAlphaSection->save();
	spRigidSection->save();
	spSection->save();
}

#endif // FV_EDITOR_ENABLED

void ReadValue( const char *pcCompressedData, FvUInt32 &uiIndex, FvUInt32 &uiValue )
{
	char cP1 = pcCompressedData[uiIndex]; uiIndex++;	
	char cP2 = pcCompressedData[uiIndex]; uiIndex++;
	char cP3 = pcCompressedData[uiIndex]; uiIndex++;
	char cP4 = pcCompressedData[uiIndex]; uiIndex++;

	uiValue = cP1 & 0xff;
	uiValue |= ((cP2 << 8  ) & 0xff00);
	uiValue |= ((cP3 << 16 ) & 0xff0000);
	uiValue |= ((cP4 << 24 ) & 0xff000000);
}

#pragma warning( push )
#pragma warning( disable : 4800 ) 

void ReadValue( const char *pcCompressedData, FvUInt32 &uiIndex, bool &bValue )
{
	bValue = static_cast<bool>(pcCompressedData[uiIndex]); uiIndex++;	
}

#pragma warning( pop )

template< class C >
void DecompressVector( const char *pcCompressedData, FvUInt32 uiLength, std::vector<C> &kVec )
{
	FvUInt32 i = 0;
	while (i < uiLength)
	{
		if (pcCompressedData[i] == char(RLE_KEY_VALUE))
		{
			i++;
			C kVal = 0;
			ReadValue(pcCompressedData, i, kVal);
			unsigned int c = unsigned int( pcCompressedData[i] & 0xff ); 
			i++;

			for (unsigned int j=0; j < c; j++)
			{
				kVec.push_back(kVal);
			}
		}
		else
		{
			C kVal = 0;
			ReadValue(pcCompressedData, i, kVal);
			kVec.push_back( kVal );
		}
	}
}

#include <FvStringMap.h>
struct BinarySection
{
	BinarySection(char *pkBinaryPtr,int iLength):
m_pkBinaryPtr(pkBinaryPtr),
m_iLength(iLength){}
char *m_pkBinaryPtr;
int m_iLength;
};
typedef FvStringHashMap<BinarySection> BinarySections;
const FvUInt32 BIN_SECTION_MAGIC = 0x42a14e65;

void OpenBinarySections(char *pkData,int iLength,BinarySections &kSections)
{
	if (!pkData) return;
	if (iLength < int(sizeof(int) + sizeof(int)*2)) return;

	int iEntryDataOffset = 0;
	int iEntryNameLenPad = 0;
	if (*(FvUInt32*)pkData == BIN_SECTION_MAGIC)
	{
		iEntryDataOffset = 4;
		iEntryNameLenPad = 3;
	}

	int iIndexLen = *(int*)(pkData + iLength - sizeof(int));
	int iOffset = iLength - sizeof(int) - iIndexLen;

	if (iOffset < iEntryDataOffset || iOffset >= iLength - int(sizeof(int)))
		return;

	while (iOffset <= iLength - (int)(sizeof(int) + sizeof(int)*2))
	{
		int iEntryDataLen = *(int*)(pkData + iOffset);
		iOffset += sizeof(int);
		if (((iEntryDataLen & (1<<31)) || iEntryNameLenPad != 0) &&
			iOffset + sizeof(FvUInt32)*4 <= iLength - (sizeof(int) + sizeof(int)))
		{
			iEntryDataLen &= ~(1<<31);
			iOffset += sizeof(FvUInt32)*4;
		}
		int iEntryNameLen = *(int*)(pkData + iOffset);
		iOffset += sizeof(int);

		if (FvUInt32(iEntryDataLen) > 256*1024*1028 ||
			FvUInt32(iEntryDataOffset + iEntryDataLen) >
			FvUInt32(iLength - sizeof(int) - iIndexLen) ||
			FvUInt32(iEntryNameLen) > 4096 ||
			FvUInt32(iOffset + iEntryNameLen) > FvUInt32(iLength - sizeof(int)))
		{
			kSections.clear();
			return;
		}

		FvString kEntryStr( pkData + iOffset, iEntryNameLen );
		iOffset += (iEntryNameLen + iEntryNameLenPad) & (~iEntryNameLenPad);

		kSections.insert(BinarySections::value_type(kEntryStr,
			BinarySection(pkData + iEntryDataOffset,iEntryDataLen)));

		iEntryDataOffset += (iEntryDataLen + 3) & (~3L);
	}
}

bool FvWater::LoadTransparencyTable( )
{
	FvString kSectionName = m_kConfig.m_kTransparencyTable;
	Ogre::DataStreamPtr spDataSection;
	try
	{
		spDataSection = Ogre::ResourceGroupManager::getSingleton().openResource(kSectionName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	}
	catch (...){}

	if (spDataSection.isNull())
		return false;

	size_t stLength = spDataSection->size();
	char *pkData = new char[stLength];
	spDataSection->read(pkData,stLength);
	BinarySections spSections;
	OpenBinarySections(pkData,stLength,spSections);

	BinarySections::iterator kFind = spSections.find( "version" );
	int iFileVersion = -1;
	if (kFind != spSections.end())
	{
		const int* pVersion = reinterpret_cast<const int*>(kFind->second.m_pkBinaryPtr);
		iFileVersion = pVersion[0];
	}

	kFind = spSections.find( "alpha" );
	if (kFind != spSections.end())
	{
		if (iFileVersion < 2)
		{
			const FvUInt32* pAlphaValues = reinterpret_cast<const FvUInt32*>(kFind->second.m_pkBinaryPtr);
			int iNumAlphaValues = kFind->second.m_iLength / sizeof(FvUInt32);
			m_kWaterAlpha.assign( pAlphaValues, pAlphaValues + iNumAlphaValues );
		}
		else
		{
			m_kWaterAlpha.clear();
			const char* pCompressedValues = reinterpret_cast<const char*>(kFind->second.m_pkBinaryPtr);
			DecompressVector<FvUInt32>(pCompressedValues, kFind->second.m_iLength, m_kWaterAlpha);
		}
	}

	kFind = spSections.find( "rigid" );
	if (kFind != spSections.end())
	{
		if (iFileVersion < 2)
		{
			const bool* pRigidValues = reinterpret_cast<const bool*>(kFind->second.m_pkBinaryPtr);
			int nRigidValues = kFind->second.m_iLength / sizeof(bool); 
			m_kWaterRigid.assign( pRigidValues, pRigidValues + nRigidValues );
		}
		else
		{
			m_kWaterRigid.clear();
			const char* pCompressedValues = reinterpret_cast<const char*>(kFind->second.m_pkBinaryPtr);
			DecompressVector<bool>(pCompressedValues, kFind->second.m_iLength, m_kWaterRigid);
		}
	}

	delete[] pkData;

	return true;
}

#define MAX_DEPTH 100.f

FvUInt32 EncodeDepth( float fVertexHeight, float fTerrainHeight )
{
	float h = 155.f + 100.f * min( 1.f, max( 0.f, (1.f - ( fVertexHeight - fTerrainHeight ))));
	h = h/255.f;					
	h = FvClampEx( 0.f, (h-0.5f)*2.f, 1.f );
	h = 1.f-h;
	h = powf(h,2.f)*2.f;
	h = min( h, 1.f);
	return FvUInt32( h*255.f ) << 24;
}

void FvWater::BuildTransparencyTable( )
{
	float fY = -m_kConfig.m_kSize.y * 0.5f;
	FvUInt32 uiIndex = 0;
	bool bSolidEdges =	(m_kConfig.m_kSize.x <= (m_kConfig.m_fTessellation*2.f)) ||
						(m_kConfig.m_kSize.y <= (m_kConfig.m_fTessellation*2.f));

	float fDepth = 10.f;

	for (FvUInt32 uiYIndex = 0; uiYIndex < m_uiGridSizeY; uiYIndex++ )
	{
		if ((uiYIndex+1) == m_uiGridSizeY)
			fY = m_kConfig.m_kSize.y * 0.5f;

		float fX = -m_kConfig.m_kSize.x * 0.5f;
		for (FvUInt32 uiXIndex = 0; uiXIndex < m_uiGridSizeX; uiXIndex++ )
		{
			if ((uiXIndex+1) == m_uiGridSizeX)
				fX = m_kConfig.m_kSize.x * 0.5f;

			FvVector3 kVec = m_kTransform.ApplyPoint( FvVector3( fX, fY, 0.1f ) );

			if (uiXIndex == 0 ||
				uiYIndex == 0 ||
				uiXIndex == ( m_uiGridSizeX - 1 ) ||
				uiYIndex == ( m_uiGridSizeY - 1 ))
			{			
				if (bSolidEdges)
				{
					m_kWaterRigid[ uiIndex ] = false;
					m_kWaterAlpha[ uiIndex ] = ( 255UL ) << 24;
				}
				else
				{
					m_kWaterRigid[ uiIndex ] = true;				
					m_kWaterAlpha[ uiIndex ] = 0x00000000;
				}
			}
			else
			{
				float fHeight = m_spCollider ? m_spCollider->Ground( kVec ) : (kVec.z - MAX_DEPTH);
				
				if (fHeight == FvEnvironmentCollider::NO_GROUND_COLLISION)
				{
					m_kWaterRigid[ uiIndex ] = false;
					m_kWaterAlpha[ uiIndex ] = ( 255UL ) << 24;
				}
				else if ( fHeight > kVec.z )
				{
					m_kWaterRigid[ uiIndex ] = true;
					m_kWaterAlpha[ uiIndex ] = 0x00000000;
				}
				else
				{
					float fDelta = kVec.z - fHeight;
					if (fDelta > fDepth)
						fDepth = (kVec.z-fHeight);

					m_kWaterRigid[ uiIndex ] = false;
					m_kWaterAlpha[ uiIndex ] = EncodeDepth(kVec.z, fHeight);
				}
			}

			++uiIndex;
			fX += m_kConfig.m_fTessellation;
		}
		fY += m_kConfig.m_fTessellation;
	}

	m_kConfig.m_fDepth = fDepth;
}

void FvWater::CreateCells()
{
	if ( m_bEnableSim )
	{
		float fSimGridSize =  ( ceilf( m_kConfig.m_fSimCellSize / m_kConfig.m_fTessellation ) * m_kConfig.m_fTessellation);

		for (float iY=0.f;iY<m_kConfig.m_kSize.y; iY+=fSimGridSize)
		{
			FvVector2 kActualSize(fSimGridSize,fSimGridSize);
			if ( (iY+fSimGridSize) > m_kConfig.m_kSize.y )
				kActualSize.y = (m_kConfig.m_kSize.y - iY);

			for (float iX=0.f;iX<m_kConfig.m_kSize.x; iX+=fSimGridSize)			
			{
				FvWaterCellPtr spNewCell = new FvWaterCell;
				if ( (iX+fSimGridSize) > m_kConfig.m_kSize.x )
					kActualSize.x = (m_kConfig.m_kSize.x - iX);

				spNewCell->Init( this, FvVector2(iX,iY), kActualSize );
				m_kCells.push_back( spNewCell );
			}
		}
	}
	else
	{
		FvVector2 kActualSize(m_kConfig.m_kSize.x, m_kConfig.m_kSize.y);
		FvWaterCellPtr spNewCell = new FvWaterCell;
		spNewCell->Init( this, FvVector2(0.f,0.f), kActualSize );
		m_kCells.push_back( spNewCell );
	}
}

void FvWater::DoCreateTables( void *pkSelf )
{
	FvWater *pkSelfWater = static_cast< FvWater * >( pkSelf );
	if (FvWater::StillValid(pkSelfWater))
	{
#ifdef FV_EDITOR_ENABLED

		if (!pkSelfWater->LoadTransparencyTable())
		{
			pkSelfWater->BuildTransparencyTable();
		}

		pkSelfWater->m_bEnableSim = true; 

#else

		if (!pkSelfWater->LoadTransparencyTable())
		{
			pkSelfWater->m_kConfig.m_bUseEdgeAlpha = false;
		}

		pkSelfWater->m_bEnableSim = FvWaters::SimulationEnabled();

#endif

		pkSelfWater->m_uiNumVertices = pkSelfWater->CreateIndices();
		pkSelfWater->m_bEnableSim = pkSelfWater->m_bEnableSim && 
			Ogre::Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_TEXTURE_FLOAT) && 
			pkSelfWater->m_kConfig.m_bUseSimulation;

		bool bRes = false;
		static bool s_bFirst = true;

		if (FvWaters::ms_spSimulationMaterial.isNull())
			pkSelfWater->m_bEnableSim = false;

		if (FvWaters::ms_spMaterial.isNull())
			pkSelfWater->m_bDrawSelf = false;
		pkSelfWater->CreateCells();

		if (FvWater::BackgroundLoad())
			s_pkDeletionLock->Give();
	}
}

void FvWater::OnCreateTablesComplete( void *pkSelf )
{
	FvWater *pkSelfWater = static_cast< FvWater * >( pkSelf );
	if (FvWater::StillValid(pkSelfWater))
	{
		pkSelfWater->Setup2ndPhase();

		if (FvWater::BackgroundLoad())
			s_pkDeletionLock->Give();

		pkSelfWater->StartResLoader();
	}
}

void FvWater::StartResLoader( )
{
    if (FvWater::BackgroundLoad())
    {
	   FvBGTaskManager::Instance().AddBackgroundTask(
			new FvCStyleBackgroundTask( 
			    &FvWater::DoLoadResources, this,
			    &FvWater::OnLoadResourcesComplete, this ) );
    }
    else
    {
	    FvWater::DoLoadResources( this );
	    FvWater::OnLoadResourcesComplete( this );
    }
}

void FvWater::Setup2ndPhase( )
{
	FV_DEBUG_MSG( "FvWater::Water: using %d vertices out of %d\n",
		m_uiNumVertices, m_uiGridSizeX * m_uiGridSizeY );

	float fSimGridSize =   ceilf( m_kConfig.m_fSimCellSize / m_kConfig.m_fTessellation ) * m_kConfig.m_fTessellation;

	int iCellX = int(ceilf(m_kConfig.m_kSize.x / fSimGridSize));
	int iCellY = int(ceilf(m_kConfig.m_kSize.y / fSimGridSize)); 
	unsigned int uiCellCount = m_kCells.size();

	for (unsigned int i=0; i<uiCellCount; i++)
	{
		m_kCells[i]->InitSimulation( MAX_SIM_TEXTURE_SIZE, m_kConfig.m_fSimCellSize );

		if (m_bEnableSim)
		{
			int iCol = (i/iCellX)*iCellX;
			int iNegX = i - 1;
			int iPosX = i + 1;
			int iNegY = i - iCellX;
			int iPosY = i + iCellX;

			m_kCells[i]->InitEdge( 1, (iNegX < iCol)						?	NULL	: m_kCells[iNegX].Get() );
			m_kCells[i]->InitEdge( 0, (iNegY < 0)							?	NULL	: m_kCells[iNegY].Get() );
			m_kCells[i]->InitEdge( 3, (iPosX >= (iCol+iCellX))				?	NULL	: m_kCells[iPosX].Get() );
			m_kCells[i]->InitEdge( 2, (unsigned int(iPosY) >= uiCellCount)	?	NULL	: m_kCells[iPosY].Get() );
		}
	}

	m_kBB.SetBounds(	
		FvVector3(	-m_kConfig.m_kSize.x * 0.5f,
					-m_kConfig.m_kSize.y * 0.5f,
					-1.f ),
		FvVector3(	m_kConfig.m_kSize.x * 0.5f,
					m_kConfig.m_kSize.y * 0.5f,
					1.f ) );

	m_kBBDeep = m_kBB;
	m_kBBDeep.SetBounds( m_kBBDeep.MinBounds() - FvVector3(0.f,0.f,2.f), m_kBBDeep.MaxBounds() );

	m_kBBActual.SetBounds(	
		FvVector3(	-m_kConfig.m_kSize.x * 0.5f,
					-m_kConfig.m_kSize.y * 0.5f,
					-m_kConfig.m_fDepth),
		FvVector3(	m_kConfig.m_kSize.x * 0.5f,
					m_kConfig.m_kSize.y * 0.5f,
					0.f ) );

	CreateUnmanagedObjects();

	CreateManagedObjects();
}

void FvWater::DoLoadResources( void *pkSelf )
{
	FvWater *pkSelfWater = static_cast< FvWater * >( pkSelf );
	if (FvWater::StillValid(pkSelfWater))
	{
		try
		{
			pkSelfWater->m_spNormalTexture = Ogre::TextureManager::getSingleton().load(pkSelfWater->m_kConfig.m_kWaveTexture,
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			pkSelfWater->m_spScreenFadeTexture = Ogre::TextureManager::getSingleton().load(s_kScreenFadeMap,
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			pkSelfWater->m_spFoamTexture = Ogre::TextureManager::getSingleton().load(pkSelfWater->m_kConfig.m_kFoamTexture,
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			pkSelfWater->m_spReflectionTexture = Ogre::TextureManager::getSingleton().load(pkSelfWater->m_kConfig.m_kReflectionTexture,
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,Ogre::TEX_TYPE_CUBE_MAP);
			FvSimulationManager::Instance().LoadResources();
		}
		catch (...){}

		if (FvWater::BackgroundLoad())
			s_pkDeletionLock->Give();
	}
}

void FvWater::OnLoadResourcesComplete( void *pkSelf )
{
	FvWater *pkSelfWater = static_cast< FvWater * >( pkSelf );
	if (FvWater::StillValid(pkSelfWater))
	{
		if (pkSelfWater && !FvWaters::ms_spMaterial.isNull())
		{
		//	pkSelfWater->m_pkParamCache->effect(FvWaters::ms_spMaterial->pEffect()->pEffect());
		//	if (pkSelfWater->m_spNormalTexture)
		//		pkSelfWater->m_pkParamCache->setTexture("normalMap", pkSelfWater->m_spNormalTexture->pTexture());
		//		
		//	if (pkSelfWater->m_spScreenFadeTexture)
		//		pkSelfWater->m_pkParamCache->setTexture("screenFadeMap", pkSelfWater->m_spScreenFadeTexture->pTexture());
		//	if (pkSelfWater->m_spFoamTexture)
		//		pkSelfWater->m_pkParamCache->setTexture("foamMap", pkSelfWater->m_spFoamTexture->pTexture());
		//	if (pkSelfWater->m_spReflectionTexture)
		//		pkSelfWater->m_pkParamCache->setTexture("reflectionCubeMap", pkSelfWater->m_spReflectionTexture->pTexture());
		}

		if (pkSelfWater && !FvWaters::ms_spSimulationMaterial.isNull())
		{
		}

		if (FvWater::BackgroundLoad())
			s_pkDeletionLock->Give();
	}
}

void FvWater::DeleteUnmanagedObjects( )
{
	//if (m_pkWaterScene)
	//{
	//	m_pkWaterScene->DeleteUnmanagedObjects();
	//}

	//FvSimulationManager::Fini();
}

void FvWater::CreateUnmanagedObjects( )
{
	FvSimulationManager::Init();

	if (!FvWaters::ms_spMaterial.isNull())
	{
		FvSimulationCell::CreateUnmanaged();

		FvWaterCell::SimulationCellPtrList::iterator cellIt = m_kActiveSimulations.begin();
		for (; cellIt != m_kActiveSimulations.end(); cellIt++)
		{
			(*cellIt)->Deactivate();
			(*cellIt)->EdgeActivation(false);
		}
		m_kActiveSimulations.clear();
	}
}

void FvWater::DeleteManagedObjects( )
{
	for (unsigned int i=0; i<m_kCells.size(); i++)
	{
		m_kCells[i]->DeleteManaged();
	}	
}

void FvWater::CreateManagedObjects()
{
	for (unsigned int i=0; i<m_kCells.size(); i++)
	{
		m_kCells[i]->CreateManagedIndices();
	}
	m_bCreatedCells = true;

	if(ms_spQuadVertexBuffer.isNull() && ms_pkVertexData == NULL)
	{
		ms_spQuadVertexBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			sizeof(FvWaterVertexType),4,Ogre::HardwareBuffer::HBU_STATIC);

		FvWaterVertexType *pkVertex = static_cast<FvWaterVertexType*>(
			ms_spQuadVertexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));
		if (pkVertex)
		{
			pkVertex->m_kPos.Set(-1,-1,0);
			pkVertex->m_uiColor = 0xffffffff;
			pkVertex->m_kUV.Set(0,1);
			pkVertex++;

			pkVertex->m_kPos.Set(1,-1, 0);
			pkVertex->m_uiColor = 0xffffffff;
			pkVertex->m_kUV.Set(1,1);
			pkVertex++;

			pkVertex->m_kPos.Set(1.f, 1.f, 0.f);
			pkVertex->m_uiColor = 0xffffffff;
			pkVertex->m_kUV.Set(1,0);
			pkVertex++;

			pkVertex->m_kPos.Set(-1.f, 1.f, 0.f);
			pkVertex->m_uiColor = 0xffffffff;
			pkVertex->m_kUV.Set(0,0);
			pkVertex++;

			ms_spQuadVertexBuffer->unlock();
		}
		else
		{
			FV_ERROR_MSG(
				"FvWater::CreateManagedObjects: "
				"Could not create/lock vertex buffer\n");
		}

		ms_pkVertexData = OGRE_NEW Ogre::VertexData;
		Ogre::VertexDeclaration *pkDecl = ms_pkVertexData->vertexDeclaration;
		Ogre::VertexBufferBinding *pkBind = ms_pkVertexData->vertexBufferBinding;
		size_t stOffset = 0;
		pkDecl->addElement(0, stOffset, Ogre::VET_FLOAT3, Ogre::VES_POSITION, 0);
		stOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
		pkDecl->addElement(0, stOffset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE, 0);
		stOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);
		pkDecl->addElement(0, stOffset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);
		ms_pkVertexData->vertexStart = 0;
		ms_pkVertexData->vertexCount = 4;
		pkBind->setBinding(0,FvWater::ms_spQuadVertexBuffer);
	}

	FV_ASSERT( !ms_spQuadVertexBuffer.isNull() && ms_pkVertexData != NULL );

	m_bInitialised = false;
	m_kVertexBuffers.clear();
}

void FvWater::RenderSimulation(float fTime)
{
	static bool s_bFirst = true;
	if (s_bFirst || FvSimulationCell::ms_fHitTime==-2.0)
	{
		ResetSimulation();
		s_bFirst = false;
	}

	if (m_bRaining)
	{
		static FvUInt32 s_uiRainMark = 0;
		if (s_uiRainMark != FvWaters::ms_uiNextMark)
		{
			s_uiRainMark = FvWaters::ms_uiNextMark;
			FvSimulationManager::Instance().SimulateRain( fTime, FvWaters::Instance().RainAmount(), 0.f );
		}
	}

	//if (m_bVisible)
	//{

		FvWaterCell::WaterCellPtrList kEdgeList;
		FvWaterCell::WaterCellVector::iterator kCellIt = m_kCells.begin();
		FvWaterCell::WaterCellPtrList::iterator kWCellIt;
		for (; kCellIt != m_kCells.end(); kCellIt++)
		{
			(*kCellIt)->CheckActivity( m_kActiveSimulations, kEdgeList );

			WATER_STAT(s_uiMovementCount += (*kCellIt).Movements().size());
		}

		for (kWCellIt = kEdgeList.begin(); kWCellIt != kEdgeList.end(); kWCellIt++)
		{
			(*kWCellIt)->CheckEdgeActivity( m_kActiveSimulations );
		}
	//}

	if (m_kActiveSimulations.size())
	{
		FvWaterCell::SimulationCellPtrList::iterator kIt = m_kActiveSimulations.begin(); 	
		for (; kIt != m_kActiveSimulations.end(); kIt++)
		{
			if ((*kIt))
			{
				WATER_STAT(s_uiActiveEdgeCellCount = (*kIt)->EdgeActivation() ? s_uiActiveEdgeCellCount + 1 : s_uiActiveEdgeCellCount);
				WATER_STAT(s_uiActiveCellCount++);

				(*kIt)->Simulate( fTime, FvWaters::Instance(), m_kConfig.m_fConsistency );
				(*kIt)->Tick();
				(*kIt)->Mark( FvWaters::ms_uiNextMark );
			}
		}

		if (FvWaters::ms_bHighQuality &&
			FvWaters::Instance().SimulationLevel() > 1)
		{
			for (kIt = m_kActiveSimulations.begin(); kIt != m_kActiveSimulations.end(); kIt++)
			{
				if ((*kIt) && (*kIt)->Mark() == FvWaters::ms_uiNextMark)
				{
					(*kIt)->StitchEdges( fTime, FvWaters::Instance() );
				}
			}
		}

	}
}

void FvWater::ResetSimulation()
{
	for (unsigned int i=0; i<m_kCells.size(); i++)
		m_kCells[i]->Clear();
}

void FvWater::ClearRT()
{
	//Moo::RenderTargetPtr rt = m_pkWaterScene->reflectionTexture();

	//if ((rt) && (rt->valid()) && (rt->push()))
	//{
	//	Moo::rc().beginScene();

	//	Moo::rc().device()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
	//		RGB( 255, 255, 255 ), 1, 0 );

	//	Moo::rc().endScene();

	//	rt->pop();
	//}
}


void FvWaters::CreateSplashSystem( )
{
#ifdef FV_SPLASH_ENABLED
	//m_kSplashManager.Init();
#endif
}

bool FvWater::Init()
{
	if (m_bInitialised)
		return true;

	//if ( m_pkWaterScene )
	//{
	//	if ( Moo::EffectManager::instance().PSVersionCap()> 0 && !m_pkWaterScene->recreate() )
	//	{
	//		return false;
	//	}
	//}
	//else
	//	return false;

	m_bInitialised = true;
	m_kVertexBuffers.clear();
	m_kVertexBuffers.resize(m_kRemappedVerts.size() + 1);

	Ogre::HardwareVertexBufferSharedPtr &spVerts0 = m_kVertexBuffers[0];
	FvUInt32 uiNumVert0 = 0;
	if (m_kRemappedVerts.size())
		uiNumVert0 = 0xffff + 1;
	else
		uiNumVert0 = m_uiNumVertices;

	std::vector<FvWaterVertexType*> pkLock;
	pkLock.resize(m_kVertexBuffers.size(),NULL);

	spVerts0 = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		sizeof(FvWaterVertexType),uiNumVert0,Ogre::HardwareBuffer::HBU_STATIC);
	bool bSuccess = (!spVerts0.isNull()) && 
		(pkLock[0] = static_cast<FvWaterVertexType*>(spVerts0->lock(Ogre::HardwareBuffer::HBL_DISCARD)));
	for ( unsigned int i = 1; i< m_kVertexBuffers.size(); i++ )
	{
		m_kVertexBuffers[i] = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			sizeof(FvWaterVertexType),m_kRemappedVerts[i-1].size(),Ogre::HardwareBuffer::HBU_STATIC);
		bSuccess = bSuccess &&
			(!m_kVertexBuffers[i].isNull()) &&
			(pkLock[i] = static_cast<FvWaterVertexType*>(m_kVertexBuffers[i]->lock(Ogre::HardwareBuffer::HBL_DISCARD)));
	}

	if (bSuccess)
	{
		FvUInt32 uiIndex = 0;
		WaterAlphas::iterator kWaterAlpha = m_kWaterAlpha.begin();

		float fY = -m_kConfig.m_kSize.y * 0.5f;
		float fYT = 0.f;

		for (FvUInt32 uiYIndex = 0; uiYIndex < m_uiGridSizeY; uiYIndex++)
		{
			if ((uiYIndex+1) == m_uiGridSizeY)
			{
				fY = m_kConfig.m_kSize.y * 0.5f;
				fYT = m_kConfig.m_kSize.y;
			}

			float fX = -m_kConfig.m_kSize.x * 0.5f;
			float fXT = 0.f;
			for (FvUInt32 uiXIndex = 0; uiXIndex < m_uiGridSizeX; uiXIndex++)
			{
				if ((uiXIndex + 1) == m_uiGridSizeX)
				{
					fX = m_kConfig.m_kSize.x * 0.5f;
					fXT = m_kConfig.m_kSize.x;
				}

				if (m_kWaterIndex[ uiIndex ] >= 0)
				{
					for ( unsigned int uiVIdX = 0; uiVIdX < m_kRemappedVerts.size(); uiVIdX++ )
					{
						std::map<FvUInt32, FvUInt32> &kMapping =
												m_kRemappedVerts[uiVIdX];

						std::map<FvUInt32, FvUInt32>::iterator kIt = 
									kMapping.find( m_kWaterIndex[ uiIndex ] );
						if ( kIt != kMapping.end() )
						{					
							FvUInt32 uiIDX = kIt->second;
							FvWaterVertexType &kVert = *(pkLock[uiVIdX + 1] + uiIDX);
							
							kVert.m_kPos.Set( fX, fY, 0 );
							if (m_kConfig.m_bUseEdgeAlpha)
								kVert.m_uiColor = *(kWaterAlpha);
							else
								kVert.m_uiColor = FvUInt32(0xffffffff);

							kVert.m_kUV.Set( fXT / m_kConfig.m_fTextureTessellation, fYT / m_kConfig.m_fTextureTessellation);
						}
					}
					if ( m_kWaterIndex[ uiIndex ] <= 
						(FvInt32)Ogre::Root::getSingleton().getRenderSystem()->getCapabilities()->getMaxVertexIndex() )
					{
						pkLock[0]->m_kPos.Set( fX, fY, 0 );

						if (m_kConfig.m_bUseEdgeAlpha)
							pkLock[0]->m_uiColor = *(kWaterAlpha);
						else
							pkLock[0]->m_uiColor = FvUInt32(0xffffffff);

						pkLock[0]->m_kUV.Set( fXT / m_kConfig.m_fTextureTessellation, fYT / m_kConfig.m_fTextureTessellation);
						++pkLock[0];
					}
				}

				++kWaterAlpha;
				++uiIndex;
				fX += m_kConfig.m_fTessellation;
				fXT += m_kConfig.m_fTessellation;
			}
			fY += m_kConfig.m_fTessellation;
			fYT += m_kConfig.m_fTessellation;
		}
		for ( unsigned int i = 0; i<m_kVertexBuffers.size(); i++ )
		{
			m_kVertexBuffers[i]->unlock();
		}	
	}
	else
	{
		FV_ERROR_MSG(
			"FvWater::CreateManagedObjects: "
			"Could not create/lock vertex buffer\n");
		return false;
	}
	FV_ASSERT( !m_kVertexBuffers[0].isNull() );

	m_kRemappedVerts.clear();

	return true;
}

void FvWater::SetupGPUParameters( FvWaterCell *pkCell, Ogre::Texture *pkReflectionTexture )
{
#if FV_EDITOR_ENABLED
	if (FvWaters::ProjectView())
		m_bRaining = false;
#endif

	Ogre::Technique *pkTech = pkCell->getTechnique();
	FV_ASSERT(pkTech);

	static bool bFirstTime = true;
	static float fTexScale = 0.1f;
	static float fFreqX = 1.f;
	static float fFreqY = 1.f;
	if (bFirstTime)
	{
		FV_WATCH( "Client Settings/Water/texScale", fTexScale, 
			FvWatcher::WT_READ_WRITE,
			"test scaling" );
		FV_WATCH( "Client Settings/Water/freqX", fFreqX, 
			FvWatcher::WT_READ_WRITE,
			"test scaling" );
		FV_WATCH( "Client Settings/Water/freqY", fFreqY, 
			FvWatcher::WT_READ_WRITE,
			"test scaling" );
		bFirstTime=false;
	}

	const FvString &kTechName = pkTech->getName();
	FV_ASSERT(kTechName.size() >= strlen("FF"));
	Ogre::Pass *pkPass = pkTech->getPass(0);
	FV_ASSERT(pkPass);
	Ogre::GpuProgramParametersSharedPtr &spVertexParameters = pkPass->getVertexProgramParameters();
	Ogre::GpuProgramParametersSharedPtr &spPixelParameters = pkPass->getFragmentProgramParameters();
	FV_ASSERT(!spVertexParameters.isNull() && !spPixelParameters.isNull());

	// Vertex Anim
	if(kTechName[0] == 2 ||
		kTechName[0] == 4)
	{
		spVertexParameters->setNamedConstant("texScale",fTexScale);
		spVertexParameters->setNamedConstant("freqX",fFreqX);
		spVertexParameters->setNamedConstant("freqY",fFreqY);

		if(kTechName[0] == 2)
		{
			Ogre::Pass *pkFogPass = pkTech->getPass(1);
			Ogre::GpuProgramParametersSharedPtr &spFogVP = pkFogPass->getVertexProgramParameters();
			spFogVP->setNamedConstant("texScale",fTexScale);
			spFogVP->setNamedConstant("freqX",fFreqX);
			spFogVP->setNamedConstant("freqY",fFreqY);
		}
	}

	// vertex
	FvVector4 kX1(m_kConfig.m_kWaveScale.x,0,0,0);
	FvVector4 kY1(0,m_kConfig.m_kWaveScale.x,0,0);
	FvVector4 kX2(m_kConfig.m_kWaveScale.y,0,0,0);
	FvVector4 kY2(0,m_kConfig.m_kWaveScale.y,0,0);

	kX1.w = m_fTexAnim*m_kConfig.m_kScrollSpeed1.x;
	kY1.w = m_fTexAnim*m_kConfig.m_kScrollSpeed1.y;

	kX2.w = m_fTexAnim*m_kConfig.m_kScrollSpeed2.x;
	kY2.w = m_fTexAnim*m_kConfig.m_kScrollSpeed2.y;

	spVertexParameters->setNamedConstant( "bumpTexCoordTransformX",*(Ogre::Vector4*)&kX1 );
	spVertexParameters->setNamedConstant( "bumpTexCoordTransformY",*(Ogre::Vector4*)&kY1 );
	spVertexParameters->setNamedConstant( "bumpTexCoordTransformX2",*(Ogre::Vector4*)&kX2 );
	spVertexParameters->setNamedConstant( "bumpTexCoordTransformY2",*(Ogre::Vector4*)&kY2 );

	float fMinY = pkCell->Min().y;
	float fMaxY = pkCell->Max().y;
	float fMinX = pkCell->Min().x;
	float fMaxX = pkCell->Max().x;

	float fSimGridSize = ceilf( m_kConfig.m_fSimCellSize / m_kConfig.m_fTessellation ) * m_kConfig.m_fTessellation;

	float fS = (m_kConfig.m_fTextureTessellation) / (fSimGridSize);

	float fCX = -(fMinX + (fSimGridSize)*0.5f);
	float fCY = -(fMinY + (fSimGridSize)*0.5f);

	spVertexParameters->setNamedConstant( "simulationTransformX", 
		Ogre::Vector4( fS, 0.f, 0.f, fCX / m_kConfig.m_fTextureTessellation ) );
	spVertexParameters->setNamedConstant( "simulationTransformY", 
		Ogre::Vector4( 0.f, fS, 0.f, fCY / m_kConfig.m_fTextureTessellation ) );
	
	if(kTechName[0] > 2)
		spVertexParameters->setNamedConstant( "foamTiling", m_kConfig.m_fFoamTiling );

	spPixelParameters->setNamedConstant( "scale", 
		Ogre::Vector4( m_kConfig.m_fReflectionScale, m_kConfig.m_fReflectionScale,
		m_kConfig.m_fRefractionScale, m_kConfig.m_fRefractionScale ));

	spPixelParameters->setNamedConstant( "reflectionTint", *(Ogre::Vector4*)&m_kConfig.m_kReflectionTint );
	spPixelParameters->setNamedConstant( "refractionTint", *(Ogre::Vector4*)&m_kConfig.m_kRefractionTint );

	{
		float fWidth = (float)1024;
		float fHeight = (float)768;		
		float fPixelX = 1.f / fWidth;
		float fPixelY = 1.f / fHeight;
		float fOffsetX = fPixelX*0.25f;
		float fOffsetY = fPixelY*0.25f + 1.f;
		spPixelParameters->setNamedConstant( "screenOffset", 
			Ogre::Vector4( fOffsetX, fOffsetY, fOffsetY, fOffsetX ));
	}

	spPixelParameters->setNamedConstant("sun_direction",FvWaters::ms_kSunDirection);
	spPixelParameters->setNamedConstant("sun_color",FvWaters::ms_kSunColor);
	spPixelParameters->setNamedConstant( "smoothness",
		FV_LERP(FvWaters::Instance().RainAmount(), m_kConfig.m_fSmoothness, 1.f) );

	if (!m_spNormalTexture.isNull())
	{
		Ogre::TextureUnitState *pkTex = pkPass->getTextureUnitState(0);
		FV_ASSERT(pkTex);
		if(pkTex->getTextureName().empty())
			pkTex->setTextureName(m_spNormalTexture->getName());
	}

	//{
	//	Ogre::TextureUnitState *pkTex = pkPass->getTextureUnitState(1);
	//	FV_ASSERT(pkTex);
	//	pkTex->setTextureName(FvWaters::ms_kRefractionTextureName);
	//}

	if(m_kConfig.m_bUseCubeMap || !FvWaters::DrawReflection())
	{
		if(!m_spReflectionTexture.isNull())
		{
			Ogre::TextureUnitState *pkTex = pkPass->getTextureUnitState(2);
			FV_ASSERT(pkTex);
			if(pkTex->getTextureName().empty())
				pkTex->setTextureName(m_spReflectionTexture->getName(),Ogre::TEX_TYPE_CUBE_MAP);
		}
	}
	else
	{
		if(pkReflectionTexture)
		{
			Ogre::TextureUnitState *pkTex = pkPass->getTextureUnitState(2);
			FV_ASSERT(pkTex);
			pkTex->setTextureName(pkReflectionTexture->getName(),Ogre::TEX_TYPE_2D);
		}
	}

	if(!m_spScreenFadeTexture.isNull())
	{
		Ogre::TextureUnitState *pkTex = pkPass->getTextureUnitState(3);
		FV_ASSERT(pkTex);
		if(pkTex->getTextureName().empty())
			pkTex->setTextureName(m_spScreenFadeTexture->getName());
	}

	int iSimTexIndex = 4;
	if(kTechName[1] > 3)
	{
		/*if(!FvWaters::ms_kDepthTextureName.empty())*/
		//{
		//	Ogre::TextureUnitState *pkTex = pkPass->getTextureUnitState(4);
		//	FV_ASSERT(pkTex);
		//	pkTex->setTextureName(FvWaters::ms_kDepthTextureName);
		//}
		iSimTexIndex = 5;

		spPixelParameters->setNamedConstant( "maxDepth", m_kConfig.m_fDepth );
		//if (waterScene_->EyeUnderWater())
		//	spPixelParameters->setNamedAutoConstant( "fadeDepth", (m_kConfig.m_fDepth - 0.001f) );
		//else
			spPixelParameters->setNamedConstant( "fadeDepth", m_kConfig.m_fFadeDepth );
		spPixelParameters->setNamedConstant( "deepColour", *(Ogre::Vector4*)&m_kConfig.m_kDeepColour );

		if((kTechName[1] - 1)%2 == 0)
		{
			if(!m_spFoamTexture.isNull())
			{
				Ogre::TextureUnitState *pkTex = pkPass->getTextureUnitState(5);
				FV_ASSERT(pkTex);
				if(pkTex->getTextureName().empty())
					pkTex->setTextureName(m_spFoamTexture->getName());
			}
			iSimTexIndex = 6;

			spPixelParameters->setNamedConstant( "foamIntersectionFactor", m_kConfig.m_fFoamIntersection );
			spPixelParameters->setNamedConstant( "foamMultiplier", m_kConfig.m_fFoamMultiplier );	
		}
	}

	bool bUsingSim = pkCell->SimulationActive();
	if (m_bRaining && 
		(kTechName[1] == 3 || kTechName[1] > 11))
	{
		spPixelParameters->setNamedConstant("simulationTiling", 10.f);

		if (bUsingSim)
		{
			Ogre::TextureUnitState *pkSimTex = pkPass->getTextureUnitState(iSimTexIndex);
			Ogre::TextureUnitState *pkRainTex = pkPass->getTextureUnitState(iSimTexIndex + 1);
			FV_ASSERT(pkSimTex && pkRainTex);
			pkSimTex->setTextureName(pkCell->SimTexture()->m_spTexture->getName());					
			pkRainTex->setTextureName(FvSimulationManager::Instance().RainTexture()->m_spTexture->getName());
		}
		else
		{
			bUsingSim = true;
			Ogre::TextureUnitState *pkSimTex = pkPass->getTextureUnitState(iSimTexIndex);
			Ogre::TextureUnitState *pkRainTex = pkPass->getTextureUnitState(iSimTexIndex + 1);
			FV_ASSERT(pkSimTex && pkRainTex);
			pkSimTex->setTextureName(FvSimulationManager::Instance().RainTexture()->m_spTexture->getName());					
			pkRainTex->setTextureName(FvSimulationManager::Instance().RainTexture()->m_spTexture->getName());
		}
	}
	else if (bUsingSim &&
		(kTechName[1] == 2 || kTechName[1] > 7))
	{
		Ogre::TextureUnitState *pkSimTex = pkPass->getTextureUnitState(iSimTexIndex);
		FV_ASSERT(pkSimTex);
		pkSimTex->setTextureName(pkCell->SimTexture()->m_spTexture->getName());
	}

	//const Ogre::MaterialPtr &spMaterial = pkCell->getMaterial();
	//if(spMaterial->getLoadingState() == Ogre::Resource::LOADSTATE_UNLOADED)
	//{
	//	for(int i = 0; i < spMaterial->getNumTechniques(); i++)
	//	{
	//		Ogre::Technique *pkTechnique = spMaterial->getTechnique(i);
	//		if(pkTechnique == pkTech) continue;
	//		FV_ASSERT(pkTech);
	//		pkTechnique->getPass(0)->getTextureUnitState(1)->setTextureName("");

	//		if(pkTechnique->getName()[1] > 3)
	//			pkTechnique->getPass(0)->getTextureUnitState(4)->setTextureName("");
	//	}
	//}
}

void FvWater::UpdateVisibility( FvCamera *pkCamera )
{	
	FvBoundingBox kBB = m_kBB;
	kBB.TransformBy(m_kTransform);
	m_bVisible = pkCamera->IsVisible(kBB);
	WATER_STAT(s_uiWaterVisCount++);
}

bool FvWater::NeedSceneUpdate() const
{
	if (this->ShouldDraw() && 
		this->DrawMark() == (FvWaters::NextDrawMark()) &&
		!this->m_kConfig.m_bUseCubeMap&&
		!(m_kConfig.m_uiVisibility == FvWater::INSIDE_ONLY /*&& WaterSceneRenderer::s_simpleScene_*/) )
	{
		FvVector4 kCamPos /*= Moo::rc().invView().row(3)*/;
		const FvVector3 &kWaterPos = this->Position();

		float fCamDist = (FvVector3(kCamPos.x,kCamPos.y,kCamPos.z) - kWaterPos).Length();
		float fDist = fCamDist - this->Size().Length()*0.5f;
		if (fDist <= ms_fSceneCullDistance)
			return true;
	}

	return false;
}

void FvWater::UpdateSimulation( float fTime )
{
	if (!m_kCells.size() || !m_bCreatedCells || (m_uiNumVertices==0)) return;

	m_fLastDTime = max( 0.001f, fTime );

	m_bRaining = false;

	if (m_bEnableSim)
	{
		m_bRaining = (FvWaters::Instance().RainAmount() || FvSimulationManager::Instance().RainActive());
		m_fTime += fTime * 30.f;
		while (m_fTime >= 1 )
		{
			m_fTime -= floorf( m_fTime );

			RenderSimulation(1.f/30.f);
		}
	}
}

void FvWater::VisitRenderable( FvWaters &kGroup, float fTime,
							  Ogre::RenderQueue *pkQueue,FvCamera *pkCamera,
							  Ogre::Texture *pkReflectionTexture )
{

#ifdef FV_EDITOR_ENABLED
	if (FvWaters::DrawReflection())
	{
		m_bReflectionCleared = false;
	}
	else
	{
		if (!m_bReflectionCleared)
		{
			ClearRT();
			m_bReflectionCleared = true;
		}
	}
#endif // FV_EDITOR_ENABLED

	if (!m_kCells.size() || !m_bCreatedCells || (m_uiNumVertices==0))
		return;	

#ifdef FV_EDITOR_ENABLED
	DWORD colourise = 0;
	if (DrawRed())
	{
		colourise = 0x00AA0000;
	}
	if ( colourise != 0 )
	{
		float fnear = -10000.f;
		float ffar = 10000.f;
	}
#endif // FV_EDITOR_ENABLED

	bool bInited = Init();

	if (m_bVisible && bInited)
	{
		FvUInt32 uiCurrentVBuffer = 0;

		m_fTexAnim += fTime * m_kConfig.m_fWindVelocity;

		for (unsigned int i = 0; i< m_kCells.size(); i++)
		{
#ifdef FV_DEBUG_WATER			
			if (ms_iDebugCell>=0 && (i != ms_iDebugCell && i != ms_iDebugCell2))
				continue;
#endif // FV_DEBUG_WATER
#ifdef FV_EDITOR_ENABLED
			if (ms_bCullCells && !FvWaters::ProjectView())
#else // FV_EDITOR_ENABLED
			if (ms_bCullCells)
#endif // !FV_EDITOR_ENABLED
			{
				if ( m_kCells[i]->CellDistance() > ms_fCullDistance * pkCamera->GetFarPlane() )
					continue;
			}

			if( m_kCells[i]->m_uiNumIndices == 0 )
				continue;

			if (uiCurrentVBuffer != m_kCells[i]->BufferIndex())
			{
				uiCurrentVBuffer = m_kCells[i]->BufferIndex();
			}

			m_kCells[i]->m_pkVertexData->vertexBufferBinding->setBinding(
				0,m_kVertexBuffers[ uiCurrentVBuffer ]);

			SetupGPUParameters(m_kCells[i].Get(),pkReflectionTexture);

			m_kCells[i]->m_pkVertexData->vertexCount = m_kVertexBuffers[ uiCurrentVBuffer ]->getNumVertices();
			m_kCells[i]->m_pkIndexData->indexCount = m_kCells[i]->m_uiNumIndices;

			pkQueue->addRenderable(m_kCells[i].Get(),ENABLE_DEFERRED_SHADING?DF_RENDER_QUEUE_ID:NM_RENDER_QUEUE_ID);
		}

#ifdef FV_EDITOR_ENABLED
		if (Highlight())
		{
			float xoff = m_kConfig.m_kSize.x * 0.5f;
			float yoff = m_kConfig.m_kSize.y * 0.5f;

			for (unsigned int i = 0; i < m_kCells.size(); i++)
			{
				FvVector3 cX0Y0(
				   m_kCells[i].min().x - xoff, -0.5f, m_kCells[i].min().y - yoff );
				FvVector3 cX1Y1(
				   m_kCells[i].max().x - xoff,  0.5f, m_kCells[i].max().y - yoff );

				BoundingBox bb( cX0Y0, cX1Y1 );

				Geometrics::wireBox( bb, 0x00C0FFC0 );
			}
		}
#endif // FV_EDITOR_ENABLED

	}

#ifdef FV_DEBUG_WATER
	if (ms_bDebugSim)
	{
		for (unsigned int i=0; i<m_kCells.size(); i++)
		{
			if (!m_kCells[i].isActive())
				continue;

			float xoff = m_kConfig.m_kSize.x*0.5f;
			float yoff = m_kConfig.m_kSize.y*0.5f;

			FvVector3 cX0Y0(m_kCells[i].min().x - xoff, 1.f, m_kCells[i].min().y - yoff);
			FvVector3 cX0Y1(m_kCells[i].min().x - xoff, 1.f, m_kCells[i].max().y - yoff);
			FvVector3 cX1Y0(m_kCells[i].max().x - xoff, 1.f, m_kCells[i].min().y - yoff);
			FvVector3 cX1Y1(m_kCells[i].max().x - xoff, 1.f, m_kCells[i].max().y - yoff);

			cX0Y0 = m_kTransform.applyPoint(cX0Y0);
			cX0Y1 = m_kTransform.applyPoint(cX0Y1);
			cX1Y0 = m_kTransform.applyPoint(cX1Y0);
			cX1Y1 = m_kTransform.applyPoint(cX1Y1);

			Moo::PackedColour col = 0xffffffff;

			if (m_kCells[i].edgeActivation())
				col = 0xff0000ff;


			LineHelper::instance().drawLine( cX0Y0, cX0Y1, col );
			LineHelper::instance().drawLine( cX0Y0, cX1Y0, col );
			LineHelper::instance().drawLine( cX1Y0, cX1Y1, col );
			LineHelper::instance().drawLine( cX0Y1, cX1Y1, col );
		}
	}
#endif // FV_DEBUG_WATER

#ifdef FV_EDITOR_ENABLED
	if( colourise != 0 )
	{
		FogController::instance().commitFogToDevice();
	}
	Highlight( false );
#endif // FV_EDITOR_ENABLED
}

/*static*/ bool FvWater::BackgroundLoad()
{
    return ms_bBackgroundLoad;
}

/*static*/ void FvWater::BackgroundLoad(bool background)
{
    ms_bBackgroundLoad = background;
}

bool FvWater::AddMovement( const FvVector3 &kFrom, const FvVector3 &kTo, const float fDiameter )
{
	FvWaterCell::WaterCellVector::iterator kCellIt = m_kCells.begin();
	for (; kCellIt != m_kCells.end(); kCellIt++)
	{
		(*kCellIt)->UpdateMovements();
	}
	if (!Visible())
		return false;

	float fDist = (kTo - m_kCamPos).Length();
	if (fDist > FvWater::ms_fMaxSimDistance)
		return false;

	FvVector3 kCTO = m_kInvTransform.ApplyPoint( kTo );
	if (kCTO.z > 0.0f || fabs(kCTO.z) > 10.0f)
		return false;
	FvVector3 kCFrom = m_kInvTransform.ApplyPoint( kFrom );
	
	float fDisplacement = (kTo - kFrom).Length();
	bool bStationary = fDisplacement < 0.001f;
#ifdef FV_SPLASH_ENABLED
	//bool bForceSplash = false;
	//if (bStationary)
	//{
	//	if ( FvWaters::Instance().CheckImpactTimer() )
	//	{
	//		bStationary = false;
	//		fDisplacement = 0.15f;
	//		bForceSplash = true;
	//	}		
	//}
#endif //FV_SPLASH_ENABLED

	bool bInVolume = m_kBBActual.Clip( kCFrom, kCTO );
	if (!bStationary && bInVolume)	
	{
#ifdef FV_SPLASH_ENABLED
		//FvVector4 kImpact(kTo.x,kTo.y,kTo.z,0);
		//kImpact.w = fDisplacement;
		//bool bShouldSplash = true;
		//if (m_kConfig.m_bUseEdgeAlpha)
		//{
		//	FvVector3 kLocalpos = m_kInvTransform.ApplyPoint( 
		//							FvVector3(kImpact.x, kImpact.y, kImpact.z) );

		//	kLocalpos.x += m_kConfig.m_kSize.x * 0.5f;
		//	kLocalpos.y += m_kConfig.m_kSize.y * 0.5f;

		//	int xIdx = min( (int)((kLocalpos.x / m_kConfig.m_fTessellation) + 0.5f), (int)m_uiGridSizeX );
		//	int zIdx = min( (int)((kLocalpos.y / m_kConfig.m_fTessellation) + 0.5f), (int)m_uiGridSizeY );
		//	int finalIdx = xIdx +  m_uiGridSizeX*zIdx;			
		//	if ( finalIdx < (int)m_kWaterAlpha.size() &&
		//		((FvUInt32(m_kWaterAlpha[finalIdx]) >> 24) < 255 ) )
		//	{
		//		bShouldSplash = false;
		//	}
		//	else
		//	{
		//		int xIdxLow = (int)floorf(kLocalpos.x / m_kConfig.m_fTessellation);
		//		int zIdxLow = (int)floorf(kLocalpos.y / m_kConfig.m_fTessellation);
		//		int xIdxHigh = min( (int)ceilf(kLocalpos.x / m_kConfig.m_fTessellation), (int)m_uiGridSizeX );
		//		int zIdxHigh = min( (int)ceilf(kLocalpos.y / m_kConfig.m_fTessellation), (int)m_uiGridSizeY );

		//		int idx1 = xIdxLow	+  m_uiGridSizeX*zIdxLow;
		//		int idx2 = xIdxLow	+  m_uiGridSizeX*zIdxHigh;
		//		int idx3 = xIdxHigh +  m_uiGridSizeX*zIdxLow;
		//		int idx4 = xIdxHigh +  m_uiGridSizeX*zIdxHigh;
		//		if (idx4 < (int)m_kWaterAlpha.size())
		//		{
		//			FvUInt32 avg = FvUInt32(m_kWaterAlpha[idx1]) >> 24;
		//			avg += FvUInt32(m_kWaterAlpha[idx2]) >> 24;
		//			avg += FvUInt32(m_kWaterAlpha[idx3]) >> 24;
		//			avg += FvUInt32(m_kWaterAlpha[idx4]) >> 24;
		//			avg /= 4;
		//			if (avg < 190)
		//			{
		//				bShouldSplash = false;
		//			}
		//		}
		//	}
		//}
		//if (bShouldSplash)
		//{
		//	//FvWaters::Instance().AddSplash(kImpact, m_kTransform._42, bForceSplash);
		//}

		//if (bForceSplash)
		//	return true;
#endif //FV_SPLASH_ENABLED

		FvVector3 kHalfsize( m_kConfig.m_kSize.x*0.5f, m_kConfig.m_kSize.y*0.5f, 0 );
		kCTO += kHalfsize;
		kCFrom += kHalfsize;

		float fSimGridSize = ceilf( m_kConfig.m_fSimCellSize / m_kConfig.m_fTessellation ) * m_kConfig.m_fTessellation;
		float fInvSimSize=1.f/fSimGridSize;

		int iXCell = int( kCTO.x * fInvSimSize);
		int iYCell = int( kCTO.y * fInvSimSize);
		unsigned int uiCell = iXCell + int((ceilf(m_kConfig.m_kSize.x * fInvSimSize)))*iYCell;
		if (uiCell < m_kCells.size())
		{	
			kCTO.x = (kCTO.x*fInvSimSize - iXCell);
			kCTO.y = (kCTO.y*fInvSimSize - iYCell);
			kCFrom.x = (kCFrom.x*fInvSimSize - iXCell);
			kCFrom.y = (kCFrom.y*fInvSimSize - iYCell);

			kCTO.z = fDisplacement;

			m_kCells[uiCell]->AddMovement( kCFrom, kCTO, fDiameter );
		}
		return true;
	}
	return bInVolume;
}


#if FV_UMBRA_ENABLE
void FvWater::AddTerrainItem( FvZoneTerrain *pkItem )
{
	TerrainVector::iterator found = std::find(	m_kTerrainItems.begin(),
												m_kTerrainItems.end(),
												pkItem );
	if (found == m_kTerrainItems.end())
		m_kTerrainItems.push_back(pkItem);
}

void FvWater::EraseTerrainItem( FvZoneTerrain *pkItem )
{
	TerrainVector::iterator found = std::find(	m_kTerrainItems.begin(),
												m_kTerrainItems.end(),
												pkItem );
	if (found != m_kTerrainItems.end())
		m_kTerrainItems.erase(found);
}

void FvWater::DisableOccluders() const
{
	TerrainVector::const_iterator it = m_kTerrainItems.begin();
	for (; it != m_kTerrainItems.end(); it++)
	{
		FvZoneTerrainPtr spTerrain = (*it);
		spTerrain->disableOccluder();
	}	
}

void FvWater::EnableOccluders() const
{
	TerrainVector::const_iterator it = m_kTerrainItems.begin();
	for (; it != m_kTerrainItems.end(); it++)
	{
		FvZoneTerrainPtr spTerrain = (*it);
		spTerrain->enableOccluder();
	}	
}

#endif // FV_UMBRA_ENABLE

void FvWaters::SetQualityOption(int iOptionIndex)
{
	//ms_bDrawReflection=true;
	//ms_bHighQuality=false;
	//m_iShaderCap = Moo::EffectManager::instance().PSVersionCap();
	//if ( m_iShaderCap >= 1 && iOptionIndex < 3)
	//{
	//	if (iOptionIndex == 0 && FullScreenBackBuffer::mrtEnabled())
	//	{
	//		ms_bHighQuality=true;
	//		WaterSceneRenderer::s_textureSize_ = 1.f;
	//		WaterSceneRenderer::s_drawTrees_ = true;
	//		WaterSceneRenderer::s_drawDynamics_ = true;
	//		WaterSceneRenderer::s_drawPlayer_ = true;
	//		WaterSceneRenderer::s_simpleScene_ = false;
	//	}
	//	else if (iOptionIndex == 1)
	//	{
	//		WaterSceneRenderer::s_textureSize_ = 1.f;
	//		WaterSceneRenderer::s_drawTrees_ = true;
	//		WaterSceneRenderer::s_drawDynamics_ = false;
	//		WaterSceneRenderer::s_drawPlayer_ = true;
	//		WaterSceneRenderer::s_simpleScene_ = false;
	//	}
	//	else if (iOptionIndex == 2)
	//	{
	//		WaterSceneRenderer::s_textureSize_ = 0.5f;
	//		WaterSceneRenderer::s_drawTrees_ = true;
	//		WaterSceneRenderer::s_drawDynamics_ = false;
	//		WaterSceneRenderer::s_drawPlayer_ = true;
	//		WaterSceneRenderer::s_simpleScene_ = true;
	//	}
	//}
	//else
	//{
	//	WaterSceneRenderer::s_textureSize_ = 0.25f;
	//	WaterSceneRenderer::s_drawTrees_ = false;
	//	WaterSceneRenderer::s_drawDynamics_ = false;
	//	WaterSceneRenderer::s_drawPlayer_ = false;
	//	WaterSceneRenderer::s_simpleScene_ = true;

	//	ms_bDrawReflection=false;
	//	ms_iSimulationLevel=0;
	//}
}

void FvWaters::SetSimulationOption(int iOptionIndex)
{
	//if (Moo::EffectManager::instance().PSVersionCap() >= 3)
	//{
	//	if (iOptionIndex==0)
	//	{
	//		if ( ms_iSimulationLevel == 0 )
	//		{
	//			m_spSimSettings->needsRestart( true );
	//		}
	//		ms_iSimulationLevel = 2;
	//	}
	//	else if (iOptionIndex==1)
	//	{
	//		if ( ms_iSimulationLevel == 0 )
	//		{
	//			m_spSimSettings->needsRestart( true );
	//		}
	//		ms_iSimulationLevel = 1;
	//	}
	//	else 
	//		ms_iSimulationLevel = 0;
	//}
	//else
	//	ms_iSimulationLevel = 0;

}


/*virtual*/ void FvWaters::OnSelectPSVersionCap(int iPSVerCap)
{
	//SetQualityOption( m_spQualitySettings->activeOption() );
	//SetSimulationOption( m_spSimSettings->activeOption() );
}

inline FvWaters& FvWaters::Instance()
{
	static FvWaters inst;
	return inst;
}

void FvWaters::Fini()
{
	ms_spSimulationMaterial.setNull();
	ms_spMaterial.setNull();

	FvSimulationManager::Fini();
#ifdef FV_SPLASH_ENABLED
	//m_kSplashManager.Fini();
#endif

	OGRE_DELETE FvWater::ms_pkVertexData;
	FvWater::ms_pkVertexData = NULL;
	FvWater::ms_spQuadVertexBuffer.setNull();
	if(FvWaters::DrawReflection())
		FvWaterScene::Fini();
}

void FvWaters::LoadResources( void *pkSelf )
{
	ms_spSimulationMaterial = Ogre::MaterialManager::getSingleton().getByName(s_kSimulationMaterial);
	if (ms_spSimulationMaterial.isNull())
	{
		ms_spSimulationMaterial = Ogre::MaterialManager::getSingleton().
			create(s_kSimulationMaterial,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	}

	ms_spSimulationMaterial->removeAllTechniques();

	FV_ASSERT(!FvWaters::ms_spSimulationMaterial.isNull());
	FV_ASSERT(FvWaters::ms_spSimulationMaterial->getNumTechniques() == 0);
	Ogre::Technique *pkTech = FvWaters::ms_spSimulationMaterial->createTechnique();
	Ogre::Pass *pkPass = pkTech->createPass();
	pkPass->setVertexProgram("FvSimulation_2_vs_2_0_VP");
	pkPass->setFragmentProgram("FvSimulation_1_ps_2_0_FP");
	pkPass->setDepthWriteEnabled(false);
	pkPass->setDepthCheckEnabled(false);
	Ogre::TextureUnitState *pkTex = pkPass->createTextureUnitState();
	pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
	pkTex->setTextureBorderColour(Ogre::ColourValue(0.5f,0.5f,0.5f));

	pkTech = ms_spSimulationMaterial->createTechnique();
	pkPass = pkTech->createPass();
	pkPass->setVertexProgram("FvSimulation_1_vs_2_0_VP");
	pkPass->setFragmentProgram("FvSimulation_0_ps_2_0_FP");
	pkPass->setDepthWriteEnabled(false);
	pkPass->setDepthCheckEnabled(false);
	pkTex = pkPass->createTextureUnitState();
	pkTex->setTextureFiltering(Ogre::TFO_NONE);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	pkTex = pkPass->createTextureUnitState();
	pkTex->setTextureFiltering(Ogre::TFO_NONE);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

	pkTech = ms_spSimulationMaterial->createTechnique();
	pkPass = pkTech->createPass();
	pkPass->setVertexProgram("FvSimulation_0_vs_2_0_VP");
	pkPass->setFragmentProgram("FvSimulation_0_ps_2_0_FP");
	pkPass->setDepthWriteEnabled(false);
	pkPass->setDepthCheckEnabled(false);
	pkTex = pkPass->createTextureUnitState();
	pkTex->setTextureFiltering(Ogre::TFO_NONE);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	pkTex = pkPass->createTextureUnitState();
	pkTex->setTextureFiltering(Ogre::TFO_NONE);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

	bool bBorder = true;
	float fBorderSize = ms_bHighQuality ? (float)FV_SIM_BORDER_SIZE : 0.0f;
	float fPixSize = 1.0f / 256.0f;
	float fPix2 = 2.0f * fBorderSize * fPixSize;
	for(int i = 0; i < 32; i++)
	{
		int iIndex = i;
		if(i > 15)
		{
			iIndex -= 16;
			bBorder = false;
		}
		iIndex += 2;
		char acFragmentProgramName[32];
		pkTech = FvWaters::ms_spSimulationMaterial->createTechnique();
		pkPass = pkTech->createPass();
		FvString kVertexProgramName = bBorder?"FvSimulation_1_vs_2_0_VP":"FvSimulation_0_vs_2_0_VP";
		sprintf_s(acFragmentProgramName,32,"FvSimulation_%d_ps_2_0_FP",iIndex);
		pkPass->setVertexProgram(kVertexProgramName);
		pkPass->setFragmentProgram(acFragmentProgramName);
		pkPass->setDepthWriteEnabled(false);
		pkPass->setDepthCheckEnabled(false);
		if(iIndex != 2)
		{
			Ogre::GpuProgramParametersSharedPtr &spPixelParameters = pkPass->getFragmentProgramParameters();
			spPixelParameters->setNamedConstant("g_cellInfo",
				Ogre::Vector4(fBorderSize * fPixSize, 1 - (fBorderSize*fPixSize), fPix2, 0.0f));
		}
		pkTex = pkPass->createTextureUnitState();
		pkTex->setTextureFiltering(Ogre::TFO_NONE);
		pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
		pkTex = pkPass->createTextureUnitState();
		pkTex->setTextureFiltering(Ogre::TFO_NONE);
		pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
		pkTex = pkPass->createTextureUnitState();
		pkTex->setTextureFiltering(Ogre::TFO_NONE);
		pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
		pkTex = pkPass->createTextureUnitState();
		pkTex->setTextureFiltering(Ogre::TFO_NONE);
		pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
		pkTex = pkPass->createTextureUnitState();
		pkTex->setTextureFiltering(Ogre::TFO_NONE);
		pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
		pkTex = pkPass->createTextureUnitState();
		pkTex->setTextureFiltering(Ogre::TFO_NONE);
		pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
	}

	ms_spMaterial = Ogre::MaterialManager::getSingleton().getByName(s_kWaterMaterial);
	if (ms_spMaterial.isNull())
	{
		ms_spMaterial = Ogre::MaterialManager::getSingleton().
			create(s_kWaterMaterial,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	}
	ms_spMaterial->removeAllTechniques();

	static const FvString kFogFP = "FvWater_FOG_FP";
	for(int i = 0; i < 4; i++)
	{
		char kVertexProgram[32];
		char kFogVP[32];
		sprintf_s(kFogVP,"FvWater_FOG%d_VP",i%2);
		for(int j = 0; j < 15; j++)
		{
			char kFragProgram[32];
			char kTechName[3];
			if(j > 2)
			{

				sprintf_s(kVertexProgram,"FvWater_%d_vs_3_0_VP",i);
				sprintf_s(kFragProgram,"FvWater_%d_ps_3_0_FP",j - 3);
			}
			else
			{

				sprintf_s(kVertexProgram,"FvWater_%d_VP",i);
				sprintf_s(kFragProgram,"FvWater_%d_ps_2_0_FP",j);
			}
			kTechName[0] = i + 1;
			kTechName[1] = j + 1;
			kTechName[2] = 0;
			Ogre::Technique *pkTech = ms_spMaterial->createTechnique();
			pkTech->setName(kTechName);
			FV_ASSERT(pkTech);
			Ogre::Pass *pkPass = pkTech->createPass();
			pkPass->setDepthWriteEnabled(false);
			pkPass->setVertexProgram(kVertexProgram);
			pkPass->setFragmentProgram(kFragProgram);
			pkPass->setSceneBlending(Ogre::SBT_ADD);
			pkPass->setSceneBlending(Ogre::SBF_SOURCE_ALPHA,Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
			pkPass->setColourWriteEnabled(unsigned int(0xffffff));
			Ogre::TextureUnitState *pkNormal = pkPass->createTextureUnitState();
			pkNormal->setTextureFiltering(Ogre::TFO_TRILINEAR);
			pkNormal->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
			Ogre::TextureUnitState *pkRefraction = pkPass->createTextureUnitState();
			pkRefraction->setTextureFiltering(Ogre::TFO_TRILINEAR);
			pkRefraction->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
			Ogre::TextureUnitState *pkReflectionCub = pkPass->createTextureUnitState();
			pkReflectionCub->setTextureFiltering(Ogre::TFO_TRILINEAR);
			pkReflectionCub->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
			Ogre::TextureUnitState *pkFade = pkPass->createTextureUnitState();
			pkFade->setTextureFiltering(Ogre::TFO_TRILINEAR);
			pkFade->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);

			if(j > 2)
			{
				Ogre::TextureUnitState *pkDeth = pkPass->createTextureUnitState();
				pkDeth->setTextureFiltering(Ogre::TFO_NONE);
				pkDeth->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
			}

			if(j > 2 && j%2 == 0)
			{
				Ogre::TextureUnitState *pkFoam = pkPass->createTextureUnitState();
				pkFoam->setTextureFiltering(Ogre::TFO_TRILINEAR);
				pkFoam->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);	
			}

			if(j == 1 || j == 2 || j > 6)
			{
				Ogre::TextureUnitState *pkSim = pkPass->createTextureUnitState();
				pkSim->setTextureFiltering(Ogre::TFO_TRILINEAR);
				pkSim->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
			}

			if(j == 2 || j > 10)
			{
				Ogre::TextureUnitState *pkRainSim = pkPass->createTextureUnitState();
				pkRainSim->setTextureFiltering(Ogre::TFO_TRILINEAR);
				pkRainSim->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
			}

			// FOG
			if(j < 3)
			{
				pkPass = pkTech->createPass();
				pkPass->setVertexProgram(kFogVP);
				pkPass->setFragmentProgram(kFogFP);
				pkPass->setColourWriteEnabled(unsigned int(0xff000000));
				pkPass->setDepthWriteEnabled(false);
			}
		}
	}
}

void FvWaters::LoadResourcesComplete( void * self )
{
}

void FvWaters::Init( FvCamera *pkCamera, bool bHDR )
{
	ms_bEnableHDR = bHDR;
	FvWaterScene::ms_pkCamera = pkCamera;
	if(FvWaters::DrawReflection())
		FvWaterScene::Init(bHDR?Ogre::PF_FLOAT16_RGBA:Ogre::PF_R8G8B8);
	FvSimulationManager::Init();

	//bool bSupported = Moo::rc().vsVersion() >= 0x200 && Moo::rc().psVersion() >= 0x200;
	//bool bSupportedHQ = Moo::rc().vsVersion() >= 0x300 && Moo::rc().psVersion() >= 0x300;

	//m_spQualitySettings = 
	//	Moo::makeCallbackGraphicsSetting(
	//	"WATER_QUALITY", "Water Quality", *this, 
	//		&FvWaters::SetQualityOption, 
	//		bSupported ? (bSupportedHQ ? 0 : 1) : 2, false, false);


	//m_spQualitySettings->addOption("HIGH",  "High", bSupportedHQ);
	//m_spQualitySettings->addOption("MEDIUM", "Medium", bSupported);
	//m_spQualitySettings->addOption("LOW", "Low", true);
	//m_spQualitySettings->addOption("LOWEST", "Lowest", true);
	//Moo::GraphicsSetting::add(m_spQualitySettings);
	//SetQualityOption(m_spQualitySettings->activeOption());

	//// simulation toggle
	//typedef Moo::GraphicsSetting::GraphicsSettingPtr GraphicsSettingPtr;
	//m_spSimSettings = 
	//	Moo::makeCallbackGraphicsSetting(
	//		"WATER_SIMULATION", "Water Simulation", *this, 
	//		&FvWaters::SetSimulationOption, 
	//		bSupported ? 0 : 2, false, false);

	//bool simulationSupported = Moo::rc().psVersion() >= 0x300 && (Moo::rc().supportsTextureFormat( D3DFMT_A16B16G16R16F ));

	//m_spSimSettings->addOption("HIGH",  "High", simulationSupported);
	//m_spSimSettings->addOption("LOW",  "Low", simulationSupported);
	//m_spSimSettings->addOption("OFF", "Off", true);
	//Moo::GraphicsSetting::add(m_spSimSettings);
	//SetSimulationOption(m_spSimSettings->activeOption());

    //if (FvWater::BackgroundLoad())
    //{
	//    FvBGTaskManager::Instance().AddBackgroundTask(
	//		new FvCStyleBackgroundTask( 
	//		    &FvWaters::LoadResources, this,
	//		    &FvWaters::LoadResourcesComplete, this ) );
    //}
    //else
    //{
	    LoadResources(this);
	    LoadResourcesComplete(this);
    //}

	CreateSplashSystem();
}

FvWaters::FvWaters() :
	m_bDrawWireframe( false ),
	m_fMovementImpact( 200.0f ),
	m_fRainAmount( 0.f ),
	m_fImpactTimer( 0.f )
{
	static bool bFirstTime = true;
	if (bFirstTime)
	{
		FV_WATCH( "Client Settings/Water/draw", ms_bDrawWaters, 
					FvWatcher::WT_READ_WRITE,
					"Draw water?" );
		FV_WATCH( "Client Settings/Water/wireframe", m_bDrawWireframe,
					FvWatcher::WT_READ_WRITE,
					"Draw water in wire frame mode?" );
		FV_WATCH( "Client Settings/Water/character impact", m_fMovementImpact,
					FvWatcher::WT_READ_WRITE,
					"Character simulation-impact scale" );
#ifdef FV_SPLASH_ENABLED
		FV_WATCH( "Client Settings/Water/Splash/auto interval", ms_fAutoImpactInterval,
					FvWatcher::WT_READ_WRITE,
					"Interval for automatically generated splashes when bStationary" );
#endif //#ifdef FV_SPLASH_ENABLED

#ifdef FV_DEBUG_WATER
		FV_WATCH( "Client Settings/Water/Stats/water count", s_uiWaterCount,
			FvWatcher::WT_READ_ONLY, "Water Stats");
		FV_WATCH( "Client Settings/Water/Stats/visible count", s_uiWaterVisCount,
			FvWatcher::WT_READ_ONLY, "Water Stats");
		FV_WATCH( "Client Settings/Water/Stats/cell count", s_uiActiveCellCount,
			FvWatcher::WT_READ_ONLY, "Water Stats");
		FV_WATCH( "Client Settings/Water/Stats/iEdge cell count", s_uiActiveEdgeCellCount,
			FvWatcher::WT_READ_ONLY, "Water Stats");
		FV_WATCH( "Client Settings/Water/Stats/impact movement count", s_uiMovementCount,
			FvWatcher::WT_READ_ONLY, "Water Stats");
#endif // FV_DEBUG_WATER

		bFirstTime = false;
	}
}

FvWaters::~FvWaters()
{
	//ms_spSimulationMaterial = NULL;
	//ms_spMaterial = NULL;
}

void FvWaters::AddMovement( const FvVector3 &kFrom, const FvVector3 &kTo, const float fDiameter )
{
	FvWaters::iterator kIt = this->begin();
	FvWaters::iterator kEnd = this->end();
	while (kIt!=kEnd)
	{
		(*kIt++)->AddMovement( kFrom, kTo, fDiameter );
	}
}

bool FvWater::IsInside( FvWaterProviderPtr pMat )
{
	FvVector3 kVec = m_kInvTransform.ApplyPoint(pMat->GetData0());
	return m_kBBActual.Intersects(kVec);
}

bool FvWater::IsInsideBoundary( FvMatrix kMat )
{
	FvVector3 kVec = m_kInvTransform.ApplyPoint( kMat.ApplyToOrigin() );

	FvBoundingBox kCheckBox( kVec, kVec );
	kCheckBox.AddZBounds( FLT_MAX );
	kCheckBox.AddZBounds( -FLT_MAX );

	return m_kBBActual.Intersects( kCheckBox );
}

void FvWaters::CheckAllListeners()
{
	///<ÐÞ¸Ä»Øµ÷>
	/*
	struct CallbackArguments
	{
	public:
		CallbackArguments( 
			VolumeCallback *pkCallback,
			bool bEntering,
			FvWater *pkWater
			):
			m_pkCallback( pkCallback ),
			m_bEntering( bEntering ),
			m_pkWater( pkWater )
		{
		}

		VolumeCallback	*m_pkCallback;
		bool m_bEntering;
		FvWater *m_pkWater;
	};
	
	std::vector<CallbackArguments> kFiredCallbacks;

	VolumeListeners::iterator kIt = m_kListeners.begin();
	VolumeListeners::iterator kEnd = m_kListeners.end();
	while (kIt != kEnd)
	{
		VolumeListener& l= *kIt;
		
		if (!l.Inside())
		{
			for (unsigned int i=0; i < this->size(); i++)
			{
				FvWater *pkWater = (*this)[i];
				bool bInside = pkWater->IsInside(l.Source());
				if (bInside)
				{
					kFiredCallbacks.push_back( CallbackArguments(l.Callback(),true,pkWater) );
					l.Water( pkWater );
					break;
				}
			}
		}
		else
		{
			bool bFoundWater = false;
			for (unsigned int i=0; i < this->size(); i++)
			{
				FvWater *pkWater = (*this)[i];
				if (l.Water() == pkWater)
				{
					bFoundWater = true;
					bool inside = pkWater->IsInside(l.Source());
					if (!inside)
					{
						kFiredCallbacks.push_back( CallbackArguments(l.Callback(),false,pkWater) );
						l.Water( NULL  );
					}
					break;					
				}
			}

			if (!bFoundWater)
			{
				kFiredCallbacks.push_back( CallbackArguments(l.Callback(),false,NULL) );
				l.Water(NULL);
			}
		}			
		kIt++;
	}	

	for (size_t i = 0; i < kFiredCallbacks.size(); i++)
	{
		CallbackArguments &kCBData = kFiredCallbacks[i];
		kCBData.m_pkCallback->Callback(kCBData.m_bEntering,kCBData.m_pkWater);
	}

	kFiredCallbacks.clear(); 
	*/


	m_kListeners.BeginIterator();
	while (!m_kListeners.IsEnd())
	{
		VolumeListeners::iterator iter = m_kListeners.GetIterator();
		VolumeListener* pkListener = (*iter).Content();
		FV_ASSERT(pkListener);
		m_kListeners.Next();
		UpdateVolumeListener(*pkListener);
	}
}

//FvUInt32 FvWaters::AddWaterVolumeListener( FvWaterProviderPtr spDynamicSource, FvWaters::VolumeCallback *pkCallback )
//{	
//	FV_ASSERT(pkCallback)
//	FvWaters::Instance().m_kListeners.push_back( VolumeListener(spDynamicSource, pkCallback) );
//	return FvWaters::Instance().m_kListeners.back().GetID();
//}
void FvWaters::AddWaterVolumeListener(VolumeListener& kListener)
{
	FvWaters::Instance().m_kListeners.AttachBack(kListener);
}
void FvWaters::UpdateVolumeListener(VolumeListener& kListener)
{
	if (!kListener.Inside())
	{
		for (unsigned int i=0; i < FvWaters::Instance().size(); i++)
		{
			FvWater *pkWater = FvWaters::Instance()[i];
			bool bInside = pkWater->IsInside(kListener.Source());
			if (bInside)
			{
				kListener.Water(pkWater);
				kListener.Callback(true);
				break;
			}
		}
	}
	else
	{
		bool bFoundWater = false;
		for (unsigned int i=0; i < FvWaters::Instance().size(); i++)
		{
			FvWater *pkWater = FvWaters::Instance()[i];
			if (kListener.Water() == pkWater)
			{
				bFoundWater = true;
				bool inside = pkWater->IsInside(kListener.Source());
				if (!inside)
				{
					kListener.Water(NULL);
					kListener.Callback(false);
				}
				break;					
			}
		}
		if (!bFoundWater)
		{
			kListener.Water(NULL);
			kListener.Callback(false);
		}
	}			
}
//void FvWaters::DelWaterVolumeListener( FvUInt32 uiID )
//{
//	m_kListeners.BeginIterator();
//	while (!m_kListeners.IsEnd())
//	{
//		VolumeListeners::iterator iter = m_kListeners.GetIterator();
//		VolumeListener* pkListener = (*iter).Content();
//		FV_ASSERT(pkListener);
//		m_kListeners.Next();
//	}
//	VolumeListeners::iterator kIt = FvWaters::Instance().m_kListeners.begin();
//	VolumeListeners::iterator kEnd = FvWaters::Instance().m_kListeners.end();
//	while (kIt != kEnd)
//	{
//		VolumeListener& l= *kIt;
//		if (l.GetID() == uiID)		
//		{
//			FvWaters::Instance().m_kListeners.erase(kIt);
//			return;
//		}
//		++kIt;
//	}	
//}

static FvVectorNoDestructor< FvWater * > s_kWaterDrawList;

void FvWaters::SetEnableHDR( bool bHDR )
{
	ms_bEnableHDR = bHDR;
	if(FvWaters::DrawReflection())
		FvWaterScene::SetTextureFormat(bHDR?Ogre::PF_FLOAT16_RGBA:Ogre::PF_R8G8B8);
}

void FvWaters::DrawReflection( bool val )
{ 
	if(ms_bDrawReflection == val) return;
	ms_bDrawReflection = val;
	if(ms_bDrawReflection)
		FvWaterScene::Init(ms_bEnableHDR?Ogre::PF_FLOAT16_RGBA:Ogre::PF_R8G8B8);
	else
		FvWaterScene::Fini();
}

void FvWaters::AddRefractionListener(FvWaterRefractionListener *pkListener)
{
	FV_ASSERT(pkListener);
	pkListener->UpdateRefractionTexture(ms_kRefractionTextureName);
	ms_kRefractionListeners.insert(pkListener);
}

void FvWaters::RemoveRefractionListener(FvWaterRefractionListener *pkListener)
{
	FV_ASSERT(pkListener);
	ms_kRefractionListeners.erase(pkListener);
}

void FvWaters::SetRefractionTexture(const FvString &kTex)
{
	ms_kRefractionTextureName =  kTex;
	std::set<FvWaterRefractionListener*>::iterator kIt = ms_kRefractionListeners.begin();
	for( ; kIt != ms_kRefractionListeners.end(); ++kIt)
	{
		FV_ASSERT(*kIt);
		(*kIt)->UpdateRefractionTexture(kTex);
	}
}

void FvWaters::AddDepthListener(FvWaterDepthListener *pkListener)
{
	FV_ASSERT(pkListener);
	pkListener->UpdateDepthTexture(ms_kDepthTextureName);
	ms_kDepthListeners.insert(pkListener);
}

void FvWaters::RemoveDepthListener(FvWaterDepthListener *pkListener)
{
	FV_ASSERT(pkListener);
	ms_kDepthListeners.erase(pkListener);
}

void FvWaters::SetDepthTexture(const FvString &kTex)
{
	ms_kDepthTextureName = kTex;
	std::set<FvWaterDepthListener*>::iterator kIt = ms_kDepthListeners.begin();
	for( ; kIt != ms_kDepthListeners.end(); ++kIt)
	{
		FV_ASSERT(*kIt);
		(*kIt)->UpdateDepthTexture(ms_kDepthTextureName);
	}
}

void FvWaters::Tick( float fTime )
{
	WATER_STAT(s_uiWaterVisCount=0);
	WATER_STAT(s_uiActiveCellCount=0);
	WATER_STAT(s_uiActiveEdgeCellCount=0);
	WATER_STAT(s_uiMovementCount=0);

	this->CheckAllListeners();
}

void FvWaters::UpdateSimulations( float fTime )
{	
	if (ms_bDrawWaters)
	{
		if(FvWaters::DrawReflection())
			FvWaterScene::Render();

		if (SimulationEnabled())
		{
			for (unsigned int i=0; i < this->size(); i++)
			{
				(*this)[i]->UpdateSimulation( fTime );
			}
		}
	}
}

unsigned int FvWaters::DrawCount() const
{
	return s_kWaterDrawList.size();
}

void FvWaters::AddToDrawList( FvWater * pWater )
{
	if ( pWater->m_bDrawSelf && !(ms_uiNextMark == pWater->DrawMark()) )
	{
		s_kWaterDrawList.push_back( pWater );
		pWater->DrawMark( ms_uiNextMark );
	}
}

static FvCamera *s_pkSortCamera;
bool WaterSortReverse( const FvWater *pkA, 
						const FvWater *pkB )
{
	FV_ASSERT(s_pkSortCamera);
	float fACamDistance = ((*(FvVector3*)&s_pkSortCamera->getRealPosition()) - pkA->Position()).LengthSquared();
	float fBCamDistance = ((*(FvVector3*)&s_pkSortCamera->getRealPosition()) - pkB->Position()).LengthSquared();
	return (fACamDistance < fBCamDistance);
}

void FvWaters::FindWatersList( float fTime,
							Ogre::RenderQueue *pkQueue, FvCamera *pkCamera )
{
	static std::vector<FvWater*> kSortWaters;
	static std::map<float,std::set<FvWater*>> kHeightWaters;
	kSortWaters.reserve(s_kWaterDrawList.size());
	kSortWaters.clear();
	kHeightWaters.clear();

	if (ms_bDrawWaters)
	{
		for (unsigned int i = 0; i < s_kWaterDrawList.size(); i++)
		{
			s_kWaterDrawList[i]->m_bVisited = false;
			s_kWaterDrawList[i]->UpdateVisibility(pkCamera);

			if(s_kWaterDrawList[i]->m_bVisible && 
				!s_kWaterDrawList[i]->m_kConfig.m_bUseCubeMap && 
				FvWaters::DrawReflection())
			{
				float fKey = s_kWaterDrawList[i]->Position().z;
				fKey = floorf(fKey+0.5f);
				kSortWaters.push_back(s_kWaterDrawList[i]);
				kHeightWaters[fKey].insert(s_kWaterDrawList[i]);
				continue;
			}
			s_kWaterDrawList[i]->VisitRenderable( *this, fTime, pkQueue, pkCamera );
			s_kWaterDrawList[i]->m_bVisited = true;
		}
	}

	if(kSortWaters.size() > 0 && FvWaters::DrawReflection())
	{
		FV_ASSERT(FvWaterScene::GetTextureNumber() > 0);
		s_pkSortCamera = pkCamera;
		std::sort( kSortWaters.begin(), kSortWaters.end(), WaterSortReverse );
		
		FvUInt32 uiNumber = (FvWaterScene::GetTextureNumber() < kSortWaters.size())?FvWaterScene::GetTextureNumber():kSortWaters.size();
		for(size_t i = 0; i < uiNumber; i++)
		{
			if(kSortWaters[i]->m_bVisited) continue;

			float fKey = kSortWaters[i]->Position().z;
			fKey = floorf(fKey+0.5f);
			
			std::set<FvWater*> &kHeightSet = kHeightWaters[fKey];
			std::set<FvWater*>::iterator kIt = kHeightSet.begin();
			for(; kIt != kHeightSet.end(); ++kIt)
			{
				(*kIt)->VisitRenderable( *this, fTime, pkQueue, pkCamera, FvWaterScene::AllocateTexture(i,fKey) );
				(*kIt)->m_bVisited = true;
			}
		}

		for(size_t i = uiNumber; i < kSortWaters.size(); i++)
		{
			if(kSortWaters[i]->m_bVisited) continue;

			kSortWaters[i]->VisitRenderable( *this, fTime, pkQueue, pkCamera, FvWaterScene::GetTexture(0) );
			kSortWaters[i]->m_bVisited = true;
		}
	}

	WATER_STAT(s_uiWaterCount = this->size());

	ms_uiLastDrawMark = ms_uiNextMark;
	ms_uiNextMark++;
	s_kWaterDrawList.clear();

	m_fImpactTimer += fTime;

#ifdef FV_SPLASH_ENABLED
	//m_kSplashManager.Draw(fTime);
#endif // FV_SPLASH_ENABLED
}

void FvWater::Tick( FvCamera *pkCamera )
{
	m_kCamPos = *(FvVector3*)&pkCamera->getDerivedPosition();

	if (m_bInitialised)
	{
		for (unsigned int i=0; i<m_kCells.size(); i++)
		{
			m_kCells[i]->UpdateDistance(m_kInvTransform.ApplyPoint(m_kCamPos));
		}
	}
}

bool FvWater::CheckVolume()
{
	bool bInsideVol = m_kBBActual.Distance( m_kCamPos ) == 0.f;
	return bInsideVol;
}

void FvWaters::CheckVolumes( FvCamera *pkCamera )
{
	FvVector4 kFogColour(1,1,1,1);
	unsigned int i = 0;
	InsideVolume(false);
	for (; i < this->size(); i++)
	{
		(*this)[i]->Tick(pkCamera);

		if (!FvWaters::Instance().InsideVolume())
			FvWaters::Instance().InsideVolume( (*this)[i]->CheckVolume() );
	}
}