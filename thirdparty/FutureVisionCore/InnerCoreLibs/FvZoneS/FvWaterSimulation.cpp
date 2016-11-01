#include "FvWaterSimulation.h"
#include "FvWater.h"
#include "FvRenderManager.h"

#include <FvWatcher.h>
#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreRenderTexture.h>
#include <OgreTextureManager.h>
#include <OgreHardwarePixelBuffer.h>

#include <RenderSystems/Direct3D9/include/OgreD3D9Texture.h>
#include <RenderSystems/Direct3D9/include/OgreD3D9RenderSystem.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

float FvSimulationCell::ms_fHitTime = -1.f;
float FvSimulationCell::ms_fWaveSpeedSquared = 150.0f;
float FvSimulationCell::ms_fLandScaleX = 50.f;
float FvSimulationCell::ms_fLandScaleY = 20.f;
float FvSimulationCell::ms_fLandScaleZ = 0.0f;

FvSimulationManager *FvSimulationManager::ms_pkInstance = NULL;
float FvSimulationManager::ms_fMaxIdleTime = 5.0f;
int FvSimulationManager::ms_iMaxTextureBlocks = 4;

static int s_iRainDeadZone = 6;

static FvString s_kSimCookieMap = "Environments/Water/water_cookie.dds";
static FvString s_kNullSimMap = "Environments/Water/null_sim.dds";

#define MAX_SIM_TEXTURE_SIZE 256
#define MAX_SIM_MOVEMENTS 10

FvUInt32 FvRenderTarget::ms_uiRenderTargetIdentify = 0;

FvRenderTarget::FvRenderTarget():
m_pkCamera(NULL),
m_pkViewport(NULL),
m_pkRenderTarget(NULL)
{

}

FvRenderTarget::~FvRenderTarget()
{
	if(m_pkRenderTarget)
		m_pkRenderTarget->removeListener(this);

	if(!m_spTexture.isNull())
	{
		Ogre::TextureManager::getSingleton().remove(m_spTexture->getHandle());
		m_spTexture.setNull();
	}

	if(m_pkCamera)
	{
		Ogre::Root::getSingleton().
			_getCurrentSceneManager()->destroyCamera(m_pkCamera);
	}

	m_pkViewport = NULL;
	m_pkRenderTarget = NULL;
}

void FvRenderTarget::preRenderTargetUpdate(const Ogre::RenderTargetEvent &kEvent)
{
	m_eSavedStage = ((FvRenderManager*)Ogre::Root::getSingleton()._getCurrentSceneManager())->GetIlluminationStage();
	((FvRenderManager*)Ogre::Root::getSingleton()._getCurrentSceneManager())->SetIlluminationStage(
		Ogre::SceneManager::IRS_RENDER_TO_TEXTURE);
}

void FvRenderTarget::postRenderTargetUpdate(const Ogre::RenderTargetEvent &kEvent)
{
	((FvRenderManager*)Ogre::Root::getSingleton()._getCurrentSceneManager())->SetIlluminationStage(m_eSavedStage);
}

void FvRenderTarget::Create(int iWidth, int iHeight, bool bReuseMainZBuffer, 
			Ogre::PixelFormat ePixelFormat)
{
	if(m_pkRenderTarget)
		return;

	FV_ASSERT(m_pkCamera == NULL);
	FV_ASSERT(m_pkViewport == NULL);

	ms_uiRenderTargetIdentify++;
	char pcIdentify[9];
	sprintf_s(pcIdentify,9,"%I32x",FvInt32(ms_uiRenderTargetIdentify));
	FvString kTexName = "FvWaterSimRTTex";
	kTexName += pcIdentify;
	FvString kCamName = "FvWaterSimRTCam";
	kCamName += pcIdentify;

	m_spTexture = Ogre::TextureManager::getSingleton().createManual( kTexName, 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 
		iWidth, iHeight, 0, ePixelFormat, Ogre::TU_RENDERTARGET );
	m_pkCamera = static_cast<FvCamera*>(Ogre::Root::getSingleton().
		_getCurrentSceneManager()->createCamera(kCamName));

	m_pkRenderTarget = m_spTexture->getBuffer()->getRenderTarget();
	m_pkRenderTarget->setAutoUpdated(false);
	m_pkRenderTarget->addListener(this);

	m_pkViewport = m_pkRenderTarget->addViewport(m_pkCamera);
	m_pkViewport->setClearEveryFrame(false);
	m_pkViewport->setOverlaysEnabled(false);
	m_pkViewport->setBackgroundColour(Ogre::ColourValue(0.f,1.f,0.f,0.f));
}

FvSimulationTextureBlock::FvSimulationTextureBlock() :
		m_iWidth(0), m_iHeight(0),
		m_bLocked( false ),
		m_iSimulationIndex( 0 ),
		m_iTechinqueIndex( 0 )
{
	for (int i=0; i<3; i++)
		m_aspSimTexture[i] = NULL;
}

FvSimulationTextureBlock::FvSimulationTextureBlock( int iWidth, int iHeight ) :
		m_iWidth( iWidth ), m_iHeight( iHeight ),
		m_bLocked( false ),
		m_iSimulationIndex( 0 ),
		m_iTechinqueIndex( 0 )
{
	for (int i=0; i<3; i++)
		m_aspSimTexture[i] = NULL;
}

bool FvSimulationTextureBlock::Lock()
{
	if ( m_bLocked )
		return false;
	else
	{
		m_bLocked = true;
		this->Clear();
		return true;
	}
}

void FvSimulationTextureBlock::Init( int iWidth, int iHeight )
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	for (int i = 0; i < 3; i++)
	{
		m_aspSimTexture[i] = new FvRenderTarget;
	}
}

void FvSimulationTextureBlock::Fini()
{
	for (int i = 0; i < 3; i++)
	{
		m_aspSimTexture[i] = NULL;
	}
}

void FvSimulationTextureBlock::Recreate()
{
	for (int i=0; i<3; i++)
	{
		const Ogre::RenderSystemCapabilities *pkCaps = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities();
		FV_ASSERT(pkCaps);
		if (pkCaps->hasCapability(Ogre::RSC_TEXTURE_FLOAT))
			m_aspSimTexture[i]->Create( m_iWidth, m_iHeight, true , Ogre::PF_SHORT_RGBA );	
		else
			m_aspSimTexture[i]->Create( m_iWidth, m_iHeight, true , Ogre::PF_FLOAT16_RGBA );	
	}
	Clear();
	Unlock();
}

void FvSimulationTextureBlock::Clear()
{
	for (int i = 0; i < 3; i++)
	{
		FvRenderTargetPtr &spSimRTT = m_aspSimTexture[i];

		if(spSimRTT->m_pkRenderTarget && 
			spSimRTT->m_pkCamera)
		{
			spSimRTT->m_pkViewport->setClearEveryFrame(true);
			spSimRTT->m_pkCamera->SetVisibleFinder(&FvCamera::ms_kDefaultVisibleObjectFinder);
			spSimRTT->m_pkRenderTarget->update();
			spSimRTT->m_pkViewport->setClearEveryFrame(false);
		}
	}
}

void FvSimulationTextureBlock::Render(FvCamera::VisibleObjectFinder *pkFinder)
{ 
	FvRenderTargetPtr &spSimRTT = m_aspSimTexture[ m_iSimulationIndex % 3 ];

	if(spSimRTT->m_pkRenderTarget && 
		spSimRTT->m_pkCamera)
	{
		spSimRTT->m_pkCamera->SetVisibleFinder(pkFinder);
		spSimRTT->m_pkRenderTarget->update();
	}
}

void FvSimulationTextureBlock::UpdateHeightTexture()
{
	Ogre::Technique *pkTech = FvWaters::ms_spSimulationMaterial->getTechnique(m_iTechinqueIndex);
	FV_ASSERT(pkTech && pkTech->getPass(0));
	FV_ASSERT(pkTech->getPass(0)->getTextureUnitState(0));
	FV_ASSERT(pkTech->getPass(0)->getTextureUnitState(1));
	pkTech->getPass(0)->getTextureUnitState(0)->setTextureName(
		((FvSimulationTextureBlock*)this)->Previous()->m_spTexture->getName());
	pkTech->getPass(0)->getTextureUnitState(1)->setTextureName(
		((FvSimulationTextureBlock*)this)->Current()->m_spTexture->getName());
}

bool FvSimulationTextureBlock::preRender(Ogre::SceneManager *pkSceneManager, Ogre::RenderSystem *pkRender)
{
	int iSize = this->Width(); 
	int iBorderSize = FvWaters::HighQualityMode() ? FV_SIM_BORDER_SIZE : 0;
	D3DVIEWPORT9 kD3DVP;
	kD3DVP.X = iBorderSize; kD3DVP.Y = iBorderSize;
	kD3DVP.Width = iSize-(2*iBorderSize); kD3DVP.Height = iSize-(2*iBorderSize);
	kD3DVP.MinZ = 0.f; kD3DVP.MaxZ = 1.f;
	HRESULT hResult = static_cast<Ogre::D3D9RenderSystem*>(pkRender)->getDevice()->SetViewport( &kD3DVP );
	if( FAILED( hResult ) )
	{
		FV_CRITICAL_MSG( "FvSimulationTextureBlock::preRender: couldn't set viewport\n" );
	}
	
	return true;
}

const Ogre::MaterialPtr &FvSimulationTextureBlock::getMaterial(void) const
{
	return FvWaters::ms_spSimulationMaterial;
}

void FvSimulationTextureBlock::getRenderOperation(Ogre::RenderOperation &kOP)
{
	FV_ASSERT(FvWater::ms_pkVertexData);
	kOP.operationType = Ogre::RenderOperation::OT_TRIANGLE_FAN;
	kOP.useIndexes = false;
	kOP.vertexData = FvWater::ms_pkVertexData;
	kOP.indexData = NULL;
}

Ogre::Technique *FvSimulationTextureBlock::getTechnique(void) const
{
	return FvWaters::ms_spSimulationMaterial->getTechnique(m_iTechinqueIndex);
}

void FvSimulationTextureBlock::getWorldTransforms(Ogre::Matrix4 *pkXForm) const
{
	*pkXForm = Ogre::Matrix4::IDENTITY;
}

Ogre::Real FvSimulationTextureBlock::getSquaredViewDepth(const Ogre::Camera *pkCam) const
{
	return 0.f;
}

const Ogre::LightList &FvSimulationTextureBlock::getLights(void) const
{
	static Ogre::LightList s_kLightList;
	return s_kLightList;
}

void FvSimulationTextureBlock::_updateCustomGpuParameter(
							   const Ogre::GpuProgramParameters::AutoConstantEntry &kConstantEntry,
							   Ogre::GpuProgramParameters *pkParams) const
{
	FV_ASSERT( ((FvSimulationTextureBlock*)this)->Width()
		== ((FvSimulationTextureBlock*)this)->Height() );
	int iSize = ((FvSimulationTextureBlock*)this)->Width();

	switch(kConstantEntry.data)
	{
	case 0:
		{
			// psSimulationTexCoordDelta_x0y1
			float fInvSizeShifted = (1.0f/float(iSize));
			pkParams->_writeRawConstant(kConstantEntry.physicalIndex,
				Ogre::Vector4(-fInvSizeShifted,0.f,0.f,0.f));
		}
		break;
	case 1:
		{
			// psSimulationTexCoordDelta_x2y1
			float fInvSizeShifted = (1.0f/float(iSize));
			pkParams->_writeRawConstant(kConstantEntry.physicalIndex,
				Ogre::Vector4(fInvSizeShifted,0.f,0.f,0.f));
		}
		break;
	case 2:
		{
			// psSimulationTexCoordDelta_x1y0
			float fInvSizeShifted = (1.0f/float(iSize));
			pkParams->_writeRawConstant(kConstantEntry.physicalIndex,
				Ogre::Vector4(0.f,-fInvSizeShifted,0.f,0.f));
		}
		break;
	case 3:
		{
			// psSimulationTexCoordDelta_x1y2
			float fInvSizeShifted = (1.0f/float(iSize));
			pkParams->_writeRawConstant(kConstantEntry.physicalIndex,
				Ogre::Vector4(0.f,fInvSizeShifted,0.f,0.f));
		}
		break;
	case 4:
		{
			// psSimulationPositionWeighting
			pkParams->_writeRawConstant(kConstantEntry.physicalIndex,
				Ogre::Vector4((m_fConsistency + 1.f), m_fConsistency, 0.0f, 0.0f));
		}
		break;
	case 5:
		{
			// psSimulationWaveSpeedSquared
			pkParams->_writeRawConstant(kConstantEntry.physicalIndex,
				FvClampEx(0.f, FvSimulationCell::ms_fWaveSpeedSquared, 400.f));
		}
		break;
	case 6:
		{
			// psSimulationOneHalfTimesDeltaTimeSquared
			pkParams->_writeRawConstant(kConstantEntry.physicalIndex,
				m_fTime*m_fTime);
		}
		break;
	case 7:
		{
			// psSimulationGridSize
			pkParams->_writeRawConstant(kConstantEntry.physicalIndex,
				Ogre::Vector4(FvSimulationCell::ms_fLandScaleX / iSize,0.f,
				FvSimulationCell::ms_fLandScaleY / iSize,0.f));
		}
		break;
	}
}

const Ogre::MaterialPtr &FvMovement::getMaterial(void) const
{
	return FvWaters::ms_spSimulationMaterial;
}

void FvMovement::getRenderOperation(Ogre::RenderOperation &kOP)
{
	FV_ASSERT(FvWater::ms_pkVertexData);
	kOP.operationType = Ogre::RenderOperation::OT_TRIANGLE_FAN;
	kOP.useIndexes = false;
	kOP.vertexData = FvWater::ms_pkVertexData;
	kOP.indexData = NULL;
}

Ogre::Technique *FvMovement::getTechnique(void) const
{
	return FvWaters::ms_spSimulationMaterial->getTechnique(TECHNIQUE_INDEX);
}

void FvMovement::getWorldTransforms(Ogre::Matrix4 *pkXForm) const
{
	FV_ASSERT(m_pkSimulation);
	float fX = 0, fY = 0;
	float fScale = FvClampEx(0.01f, m_fDiameter / m_pkSimulation->Size(), 1.f);
	fX =  ((m_kTo.x - 0.5f)*2.f);
	fY = -((m_kTo.y - 0.5f)*2.f);
	((FvMatrix*)pkXForm)->SetScale(fScale,fScale,fScale);
	((FvMatrix*)pkXForm)->PostTranslateBy(FvVector3(fX,fY,0.f));
	((FvMatrix*)pkXForm)->Transpose();
}

Ogre::Real FvMovement::getSquaredViewDepth(const Ogre::Camera *pkCam) const
{
	return 0.f;
}

const Ogre::LightList &FvMovement::getLights(void) const
{
	static Ogre::LightList s_kLightList;
	return s_kLightList;
}

void FvMovement::_updateCustomGpuParameter(
									   const Ogre::GpuProgramParameters::AutoConstantEntry &kConstantEntry,
									   Ogre::GpuProgramParameters *pkParams) const
{
	switch(kConstantEntry.data)
	{
	case 0:
		pkParams->_writeRawConstant(kConstantEntry.physicalIndex,
			1.0f/30.0f);
		break;
	case 1:
		pkParams->_writeRawConstant(kConstantEntry.physicalIndex,
			FvClampEx(0.f, FvWaters::Instance().MovementImpact()*m_kTo.z,1000.f));
		break;
	}
}

FvSimulation::FvSimulation() : 
m_iSize( 256 ),
m_bPerturb( true ),
m_fIdleTime(999),
m_fLastSimTime(0),
m_bActive( false ),
m_uiLastMovementFrame( FvUInt32(-1) )
{
}

FvSimulation::~FvSimulation()
{

}

void FvSimulation::UpdateMovements()
{
	bool bResult = (m_uiLastMovementFrame == FvWaters::NextDrawMark());
	m_uiLastMovementFrame = FvWaters::NextDrawMark();
	if (!bResult)
		m_kMovements.clear();
}

FvRainSimulation::FvRainSimulation()
{
	Perturbed(false);
	Activate();
}

#define GEN_RANDOM_RAIN() float(FvClampEx(s_iRainDeadZone, rand() % 100, (100-s_iRainDeadZone) )) / 100.f

void FvRainSimulation::Simulate( float fTime, float fAmount, float fConsistency )
{
	if (IdleTime() == 0.f)
	{
		static float s_fTime = 0.f;
		s_fTime += fTime;
		float fMaxDropRate = 1.f/110.f;
		float fMinDropRate = 1.f/20.f;
		float fDropRate = FV_LERP(fAmount, fMinDropRate, fMaxDropRate);
		while (s_fTime >= fDropRate)
		{
			s_fTime -= fDropRate;
			float fX = GEN_RANDOM_RAIN();
			float fY = GEN_RANDOM_RAIN();
			AddMovement(FvVector3(fX, fY, 0.0f), FvVector3(fX, fY, 0.5f), 0.5);
		}
	}

	m_kSimulationTarget.m_fConsistency = fConsistency;
	m_kSimulationTarget.m_fTime = fTime;
	m_kSimulationTarget.Render(this);

	ClearMovements();
}

void FvRainSimulation::FindVisibleObject( 
							   Ogre::RenderQueue *pkQueue,
							   FvCamera *pkCamera, 
							   Ogre::VisibleObjectsBoundsInfo *pkVisibleBounds, 
							   bool bOnlyShadowCasters )
{
	m_kSimulationTarget.SetTechnique(
		FvWaters::HighQualityMode()?
		FvMovement::TECHNIQUE_INDEX + 1:
		FvMovement::TECHNIQUE_INDEX + 2);
	m_kSimulationTarget.UpdateHeightTexture();
	pkQueue->addRenderable(&m_kSimulationTarget,Ogre::RENDER_QUEUE_1);

	if (IdleTime() == 0.f)
		FvSimulationManager::Instance().VisitMovements(this,pkQueue);
}

#ifdef _DEBUG
bool FvSimulationManager::ms_bPaused = false;
#endif

FvSimulationManager::FvSimulationManager() : 
m_iAvailableBlocks( 0 ),
m_iSimulationSize( MAX_SIM_TEXTURE_SIZE )
{ 
	InitInternal();
}

FvSimulationManager::~FvSimulationManager()
{ 
	FiniInternal();
}

void FvSimulationManager::TickSimulations()
{
	for (unsigned int i = 0; i < m_kBlocks.size(); i++)
	{
		m_kBlocks[i].TickSimulation();
	}
}

void FvSimulationManager::LoadResources()
{
	m_spCookieTexture = Ogre::TextureManager::getSingleton().load(s_kSimCookieMap,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	m_spNullSim = Ogre::TextureManager::getSingleton().load(s_kNullSimMap,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	if(!FvWaters::ms_spSimulationMaterial.isNull())
	{
		Ogre::Technique *pkTech = FvWaters::ms_spSimulationMaterial->getTechnique(FvMovement::TECHNIQUE_INDEX);
		FV_ASSERT(pkTech && pkTech->getPass(0) && pkTech->getPass(0)->getTextureUnitState(0));
		pkTech->getPass(0)->getTextureUnitState(0)->setTextureName(m_spCookieTexture->getName());
	}
}

void FvSimulationManager::RecreateBlocks()
{
	const Ogre::RenderSystemCapabilities *pkCaps = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities();
	FV_ASSERT(pkCaps);
	if (pkCaps->hasCapability(Ogre::RSC_TEXTURE_FLOAT))
	{
		for (unsigned int i = 0; i < m_kBlocks.size(); i++)
		{
			m_kBlocks[i].Recreate();
		}
		m_kRainSimulation.Recreate();
	}
}

void FvSimulationManager::SimulateRain( float fTime, float fAmount, float fConsistency )
{
	if (fAmount)
		m_kRainSimulation.ResetIdleTimer();
	else
		m_kRainSimulation.UpdateTime(fTime);

	m_kRainSimulation.Simulate(fTime,fAmount,fConsistency);
	m_kRainSimulation.Tick();
}

FvSimulationTextureBlock *FvSimulationManager::RequestSimulationTextureBlock( FvSimulationCell *pkCell )
{
	FV_ASSERT(pkCell);
	if (pkCell->Perturbed() && pkCell->SimulationBlock() == NULL)
	{
		TextureBlocks::iterator it = m_kBlocks.begin();
		for (;it != m_kBlocks.end(); it++)
		{
			if ( (*it).Lock() )
				return &(*it);
		}
	}
	return 0;
}

void FvSimulationManager::ReleaseSimulationTextureBlock( FvSimulationTextureBlock *pkBlock )
{
	FV_ASSERT(pkBlock);	
	pkBlock->Unlock();
}

/*static*/ void FvSimulationManager::Init()
{
	if (ms_pkInstance == NULL)
	{
		ms_pkInstance = new FvSimulationManager();
	}
}

/*static*/ void FvSimulationManager::Fini()
{
	if (ms_pkInstance)
	{
		delete ms_pkInstance;
		ms_pkInstance = NULL;
	}
}

void FvSimulationManager::InitInternal()
{
	FV_INFO_MSG("Initialising FvSimulationManager\n");
	if (m_kBlocks.size() == 0)
		m_kBlocks.resize(ms_iMaxTextureBlocks);

	for (unsigned int i = 0; i < m_kBlocks.size(); i++)
	{
		m_kBlocks[i].Init(m_iSimulationSize, m_iSimulationSize);
	}

	m_kRainSimulation.Init(m_iSimulationSize);
}

void FvSimulationManager::FiniInternal()
{
	FV_INFO_MSG("Destroying FvSimulationManager\n");

	m_spCookieTexture.setNull();
	m_spNullSim.setNull();

	m_kBlocks.clear();
	m_kRainSimulation.Fini();
}

void FvSimulationManager::VisitMovements( FvSimulation *pkSim, Ogre::RenderQueue *pkQueue )
{
	FvMovements::iterator kIt = pkSim->Movements().begin();
	bool bFirstMove = true;

	for (; kIt != pkSim->Movements().end(); kIt++ )
	{
		if (bFirstMove)
		{
			pkSim->ResetIdleTimer();
			bFirstMove = false;
		}

		pkQueue->addRenderable((*kIt).Get(),Ogre::RENDER_QUEUE_2);
	}
}

FvSimulationCell::FvSimulationCell() : 
	m_iEdge( 0 ),
	m_pkSimulationTarget( NULL ),
	m_bEdgeActivation( false ),
	m_uiTickMark( 0 )
{
	static bool bFirstTime = true;
	if ( bFirstTime )
	{
		ms_fHitTime = -1.f;
		ms_fWaveSpeedSquared = 150.0f;
		ms_fLandScaleX = 50.f;
		ms_fLandScaleY = 20.f;
		ms_fLandScaleZ = 0.0f;

		bFirstTime = false;
		FV_WATCH( "Client Settings/Water/water speed square", ms_fWaveSpeedSquared,
						FvWatcher::WT_READ_WRITE,
						"Simulated wave propagation speed" );
		FV_WATCH( "Client Settings/Water/Rain dead zone", s_iRainDeadZone,
						FvWatcher::WT_READ_WRITE,
						"Area of rain texture border that wont receive rain drops (percentage)" );

#ifdef _DEBUG
		FV_WATCH( "Client Settings/Water/pause sim", FvSimulationManager::ms_bPaused,
						FvWatcher::WT_READ_WRITE,
						"SimulationManager::ms_bPaused" );
#endif						

	}
}

void FvSimulationCell::BindAsNeighbour( Ogre::Technique *pkTechnique, int kBinding )
{
	if (m_pkSimulationTarget)
	{
		Ogre::TextureUnitState *pkTex = pkTechnique->getPass(0)->getTextureUnitState(kBinding);
		FV_ASSERT(pkTex);
		if ( this->Mark() == FvWaters::ms_uiNextMark )
		{
			pkTex->setTextureName(m_pkSimulationTarget->Previous()->m_spTexture->getName());
		}
		else
		{
			pkTex->setTextureName(m_pkSimulationTarget->Current()->m_spTexture->getName());
		}
	}
}

void FvSimulationCell::Simulate( float fTime, FvWaters &kGroup, float fConsistency )
{
	UpdateTime(fTime);
	if ( !IsActive() )
		return;

	m_pkSimulationTarget->m_fConsistency = fConsistency;
	m_pkSimulationTarget->m_fTime = fTime;
	m_pkSimulationTarget->Render(this);
	ClearMovements();
}

void FvSimulationCell::FindVisibleObject( 
					   Ogre::RenderQueue *pkQueue,
					   FvCamera *pkCamera, 
					   Ogre::VisibleObjectsBoundsInfo *pkVisibleBounds, 
					   bool bOnlyShadowCasters )
{
	if (EdgeActivation())
	{
		int iY0=(m_iEdge & 1) ? 1 : 0;
		int iX0=(m_iEdge & 2) ? 1 : 0;
		int iY2=(m_iEdge & 4) ? 1 : 0;
		int iX2=(m_iEdge & 8) ? 1 : 0;	

		int iTech = 3 + (FvWaters::HighQualityMode()?0:1)*16 + iY0*8 + iX0*4 + iY2*2 + iX2;

		m_pkSimulationTarget->SetTechnique(iTech);
		m_pkSimulationTarget->UpdateHeightTexture();
		Ogre::Technique *pkTech = m_pkSimulationTarget->getTechnique();

		BindNeighbour( pkTech, 0 );
		BindNeighbour( pkTech, 1 );
		BindNeighbour( pkTech, 2 );
		BindNeighbour( pkTech, 3 );
	}
	else
	{
		m_pkSimulationTarget->SetTechnique(
			FvWaters::HighQualityMode()?
			FvMovement::TECHNIQUE_INDEX + 1:
			FvMovement::TECHNIQUE_INDEX + 2);
		m_pkSimulationTarget->UpdateHeightTexture();
	}

	pkQueue->addRenderable(m_pkSimulationTarget,Ogre::RENDER_QUEUE_1);

	if( Perturbed() )
		FvSimulationManager::Instance().VisitMovements( this, pkQueue );
}

void FvSimulationCell::StitchEdges( float fTime, FvWaters &kGroup )
{
	if ( !IsActive() )
		return;
	
	if (EdgeActivation())
	{
		int iY0=(m_iEdge & 1) ? 1 : 0;
		int iX0=(m_iEdge & 2) ? 1 : 0;
		int iY2=(m_iEdge & 4) ? 1 : 0;
		int iX2=(m_iEdge & 8) ? 1 : 0;

		int iTech = 3 + (FvWaters::HighQualityMode()?0:1)*16 + iY0*8 + iX0*4 + iY2*2 + iX2;

		m_pkSimulationTarget->SetTechnique(iTech);
		m_pkSimulationTarget->UpdateHeightTexture();
		Ogre::Technique *pkTech = m_pkSimulationTarget->getTechnique();

		BindNeighbour( pkTech, 0 );
		BindNeighbour( pkTech, 1 );
		BindNeighbour( pkTech, 2 );
		BindNeighbour( pkTech, 3 );

		FV_ASSERT(this->Movements().size() == 0);

		m_pkSimulationTarget->Render(this);
	}
}

void FvSimulationCell::Init( int iNewSize, float fCellSize )
{
	m_fCellSize	= fCellSize;	
	m_iEdge		= 0;	
	Size(iNewSize);
	Perturbed(false);	
}

void FvSimulationCell::CreateUnmanaged()
{
	FvSimulationManager::Instance().RecreateBlocks();
}

void FvSimulationCell::Clear()
{
	if ( m_pkSimulationTarget )
		m_pkSimulationTarget->Clear();
}