#include "FvFlora.h"
#include "FvFloraTexture.h"
#include "FvFloraRenderer.h"
#include "FvFloraVertexType.h"

#include "FvCamera.h"

#include <FvWatcher.h>
#include <OgreHardwareVertexBuffer.h>
#include <OgreHardwareBufferManager.h>

#pragma warning (disable:4355)

namespace 
{
	bool s_bCull = true;
	int s_iNumDrawPrimitiveCalls = 0;
	int s_iNumTris = 0;
	bool s_bDebugBB = false;
	bool s_bDebugStatus = false;
	int s_iAllowedBlockMovesPerFrame = 10;
	float s_fVisibleDistance = 250.f;
	FvUInt32 s_uiMaxVBSize = 1048576;
}

bool FvFlora::ms_bEnabled = true;
std::vector< FvFlora* > FvFlora::ms_kFloras;

FvFlora::FvFlora():
m_spData( NULL ),
m_iLutSeed( 0 ),
m_kLastPos( -30000.f, -30000.f ),
m_bCameraTeleport( true ),
m_uiNumVerticesPerBlock( 0 ),
m_uiMaxVBSize( 0 ),	
m_kLastRefPt( 1e23f, 1e23f ),
m_uiVBSize( 0 ),
m_kDegenerateEcotype( this ),
m_pkFloraTexture( NULL ),
m_fMaxSlope( 30.f ),
m_pkContainer(NULL),
m_uiCapacity(0),
m_uiBufferSize(0),
m_pkLockedBuffer(NULL),
m_bEnableBending(false),
m_iCenterBlockX(FV_FLORA_BLOCK_STRIDE/2),
m_iCenterBlockY(FV_FLORA_BLOCK_STRIDE/2)
{
	m_akMacroBB[0].m_apBlocks[0] = NULL;
	InitialiseOffsetTable();

	m_kDegenerateEcotype.m_bIsLoading = true;

	static bool s_createdWatchers = false;
	if (!s_createdWatchers)
	{
		FV_WATCH( "Client Settings/Flora/Alpha Test Ref",
			s_uiAlphaTestRef,
			FvWatcher::WT_READ_WRITE,
			"Alpha test reference value for the alpha tested drawing pass." );
		FV_WATCH( "Client Settings/Flora/Alpha Test Ref (Shadows)",
			s_uiShadowAlphaTestRef,
			FvWatcher::WT_READ_WRITE,
			"Alpha test reference value for the shadow drawing pass." );
		FV_WATCH( "Client Settings/Flora/Alpha Test Draw Distance",
			s_fAlphaTestDistance,
			FvWatcher::WT_READ_WRITE,
			"Range from the camera to draw the alpha tested drawing pass.  "
			"Set to 0 to disable the alpha test draw pass." );
		FV_WATCH( "Client Settings/Flora/Alpha Test Fade Pct",
			s_fAlphaTestFadePct,
			FvWatcher::WT_READ_WRITE,
			"Percent of the alpha test drawing range over which the flora fades out." );
		FV_WATCH( "Client Settings/Flora/Alpha Blend Draw Distance",
			s_fAlphaBlendDistance,
			FvWatcher::WT_READ_WRITE,
			"Range from flora's far clip plane (usually 50m) towards the camera"
			" used for the alpha blended drawing pass.  Set to 50m to disable"
			" the alpha blended draw pass." );
		FV_WATCH( "Client Settings/Flora/Alpha Blend Fade Pct",
			s_fAlphaBlendFadePct,
			FvWatcher::WT_READ_WRITE,
			"Percent of the alpha blended drawing range over which the flora fades out." );
		FV_WATCH( "Client Settings/Flora/per frame block move max",
			s_iAllowedBlockMovesPerFrame,
			FvWatcher::WT_READ_WRITE,
			"Number of flora blocks that may be updated per-frame.  Set this value higher"
			"if the flora seems to be lagging behind the camera.  Higher values may incur"
			"a higher per-frame cost depending on the speed at which the camera moves." );
		FV_WATCH( "Client Settings/Flora/Debug BB",
			s_bDebugBB,
			FvWatcher::WT_READ_WRITE,
			"Enable visualisation of the flora blocks' bounding boxes, and the flora's"
			"macro bounding box culling system." );
		FV_WATCH( "Client Settings/Flora/Debug Status",
			s_bDebugStatus,
			FvWatcher::WT_READ_WRITE,
			"Enable visualisation of flora block's update status, and the status of the"
			"composite flora texture." );
		FV_WATCH( "Client Settings/Flora/Cull",
			s_bCull,
			FvWatcher::WT_READ_WRITE,
			"Enabled or disable culling of flora.  If disabled, the flora will not move"
			"around with the camera or be clipped by its frustum." );
		FV_WATCH( "Client Settings/Flora/Num Draw Calls",
			s_iNumDrawPrimitiveCalls,
			FvWatcher::WT_READ_ONLY,			
			"Number of draw calls made by the FvFlora in the last frame." );
		FV_WATCH( "Client Settings/Flora/Num Triangles",
			s_iNumTris,
			FvWatcher::WT_READ_ONLY,
			"Number of triangles drawn by the FvFlora in the last frame." );		
		FV_WATCH( "Client Settings/Flora/Repopulate",
			m_bCameraTeleport,
			FvWatcher::WT_READ_WRITE,
			"Set to 1 to reset and repopulate the flora.  Value will "
			"automatically be set back to 0 when complete." );

		s_createdWatchers = true;
	}

	memset( m_apEcotypes, 0, sizeof( FvEcotype* ) * 256 );

	for ( int iY=0;iY<FV_FLORA_BLOCK_STRIDE;iY++ )
	{
		for ( int iX=0;iX<FV_FLORA_BLOCK_STRIDE;iX++ )
		{
			m_apBlocks[iY][iX] = new FvFloraBlock( this );
			FloraBlockDistancePair *pkPair = new FloraBlockDistancePair( FV_FLORA_VISIBILITY * FV_FLORA_VISIBILITY, *m_apBlocks[iY][iX] );
			m_kSortedBlocks.insert(m_kSortedBlocks.end(),pkPair);
		}
	}

	ms_kFloras.push_back( this );
}

FvFlora::~FvFlora()
{
	std::vector< FvFlora* >::iterator kFIt = ms_kFloras.begin();
	std::vector< FvFlora* >::iterator kFEnd = ms_kFloras.end();
	for (; kFIt != kFEnd; ++kFIt)
	{
		if (*kFIt == this)
		{
			ms_kFloras.erase( kFIt );
			break;
		}
	}

	SortedFloraBlocks::iterator kIt = m_kSortedBlocks.begin();
	SortedFloraBlocks::iterator kEnd = m_kSortedBlocks.end();
	while ( kIt != kEnd )
	{
		delete *kIt++;
	}

	for ( int iY=0;iY<FV_FLORA_BLOCK_STRIDE;iY++ )
	{
		for ( int iX=0;iX<FV_FLORA_BLOCK_STRIDE;iX++ )
		{
			delete m_apBlocks[iY][iX];
		}
	}

	{
		FvSimpleMutexHolder kMutex( FvEcotype::ms_kDeleteMutex );
		for ( int i=0; i<256; i++ )
		{
			if ( m_apEcotypes[i] )
				delete m_apEcotypes[i];
			m_apEcotypes[i] = NULL;
		}
	}

	if (m_pkFloraTexture)
	{
		delete m_pkFloraTexture;
		m_pkFloraTexture=NULL;
	}
}

bool FvFlora::Init( FvXMLSectionPtr spSection )
{
	m_spData = spSection;

	s_fVisibleDistance = spSection->ReadFloat( "VisibleDistance", s_fVisibleDistance );
	s_iAllowedBlockMovesPerFrame = spSection->ReadInt( "MaxPerFrameBlockMove", s_iAllowedBlockMovesPerFrame );
	
	this->m_uiMaxVBSize = spSection->ReadInt( "vb_size", s_uiMaxVBSize );

	FV_ASSERT( FV_FLORA_MOVE_DIST == FV_FLORA_VISIBILITY * 2 );
	FV_ASSERT( FV_FLORA_BLOCK_STRIDE == (FV_FLORA_VISIBILITY * 2) / FV_FLORA_BLOCK_WIDTH );

	m_pkFloraTexture = new FvFloraTexture();
	if ( m_pkFloraTexture )
		m_pkFloraTexture->Init( spSection );

	this->CreateUnmanagedObjects();

	m_kTexToEcotype.clear();
	FvEcotype::ID uiID = 0;
	FvXMLSectionPtr spEcotypes = spSection->OpenSection( "ecotypes" );
	FvXMLSectionIterator it = spEcotypes->Begin();
	while (it != spEcotypes->End())
	{
		FvXMLSectionPtr spSect = *it++;
		std::vector<FvString> kTextures;
		spSect->ReadStrings("texture", kTextures);
		for (FvUInt32 i=0; i<kTextures.size(); i++)
		{
			FvString &kTexRoot = kTextures[i];
			Ogre::StringUtil::toLowerCase(kTexRoot);
			m_kTexToEcotype[ kTexRoot ] = uiID;
		}
		uiID++;
	}

	m_fMaxSlope = spSection->ReadFloat( "maxSlope", 30.f );

	return true;
}

void FvFlora::VisitRenderable( Ogre::RenderQueue *pkQueue,
							  FvCamera *pkCamera,
							  bool bOnlyShadowCasters )
{
	FV_ASSERT(pkQueue);
	FV_ASSERT(pkCamera);
	s_iNumDrawPrimitiveCalls = 0;                                            
	s_iNumTris = 0;    

	if ( m_spVertexBuffer.isNull() )
		return;

	if ( !ms_bEnabled )
		return;

	if ( !FvTerrainPage::GetTerrainVisible() )
		return;

	if ( !m_akMacroBB[0].m_apBlocks[0] )
		return;

	if( bOnlyShadowCasters )
		return;

	if ( s_bCull )
	{
		this->Cull(pkCamera);
	}

	FvVector2 kCamPos2(pkCamera->getRealPosition().x, pkCamera->getRealPosition().y);

	SortedFloraBlocks::iterator kIt = m_kSortedBlocks.begin();
	SortedFloraBlocks::iterator kEnd = m_kSortedBlocks.end();
	while ( kIt != kEnd )
	{
		FloraBlockDistancePair &kPair = **kIt++;
		const FvFloraBlock &kBlock = kPair.m_kBlock;

		if ( (!kBlock.Culled()) && (kBlock.BlockID() != -1) )		
			kPair.m_fDistance = FvVector2(kBlock.Center() - kCamPos2).LengthSquared();
	}

	std::sort( m_kSortedBlocks.begin(), m_kSortedBlocks.end(), FloraBlockDistancePair::SortReverse );	

	float fAdjBlockWidth = FV_FLORA_BLOCK_WIDTH * 2.f;
	float fSquareDistance = s_fVisibleDistance * s_fVisibleDistance + fAdjBlockWidth * fAdjBlockWidth;
	int iIndex = m_kSortedBlocks.size()-1;
	while ( iIndex >= 0 )
	{
		FloraBlockDistancePair &kPair = *m_kSortedBlocks[iIndex--];
		const FvFloraBlock &kBlock = kPair.m_kBlock;
		if ( kPair.m_fDistance < fSquareDistance )
		{
			if (!kBlock.Culled() && (kBlock.BlockID() != -1))
			{
				FvFloraRenderable *pkRenderable = kBlock.GetRenderable();
				pkRenderable->SetVertex(kBlock.Offset(), m_uiNumVerticesPerBlock);
				pkQueue->addRenderable(pkRenderable,FV_FLORA_RENDER_QUEUE);
				s_iNumDrawPrimitiveCalls++;
				s_iNumTris += (m_uiNumVerticesPerBlock)/3;		
			}
		}
		else
		{
			break;
		}		
	}
}

void FvFlora::Activate()
{
	if (!m_spVertexBuffer.isNull())
	{
		this->FloraReset();
		this->FloraTexture()->Activate();
	}
	else
	{
		FV_ERROR_MSG("FvFlora::Activate: Could not activate flora, the renderer is not initialised.");
	}
}

void FvFlora::Deactivate()
{
	if (!m_spVertexBuffer.isNull())
	{
		this->FloraTexture()->Deactivate();
	}
}

void FvFlora::CreateUnmanagedObjects()
{
	if ( !m_spData )
		return;
	
	float fRatio = 1.0f/*FloraSettings::instance().vbRatio()*/;
	this->VBSize(FvUInt32(this->m_uiMaxVBSize * fRatio));
}

void FvFlora::DeleteUnmanagedObjects()
{
	for ( int z=0; z<FV_FLORA_BLOCK_STRIDE; z++ )
		for ( int x=0; x<FV_FLORA_BLOCK_STRIDE; x++ )
			m_apBlocks[z][x]->Invalidate();

	m_spMaterial.setNull();
}

bool FvFlora::PreLock()
{
	FV_ASSERT( m_pkLockedBuffer == NULL );
	m_pkLockedBuffer = (FvUInt8 *)m_spVertexBuffer->lock(Ogre::HardwareBuffer::HBL_NO_OVERWRITE);
	return ( m_pkLockedBuffer ? true : false );
}

FvFloraVertexContainer *FvFlora::Lock( FvUInt32 uiFirstVertex, FvUInt32 uiNumVertices )
{
	if (!m_pkContainer)
	{
		FvFloraVertexContainer *pkVertexContainer = new FvFloraVertexContainer;
		FvFloraVertex *pkVerts = (FvFloraVertex*)m_pkLockedBuffer;
		if (pkVerts)
		{
			pkVertexContainer->Init( pkVerts + uiFirstVertex, uiNumVertices );
			m_pkContainer = pkVertexContainer;
		}
		return m_pkContainer;
	}
	else
	{
		FV_ERROR_MSG( "FvFlora::Lock - Only allowed to lock once\n" );
	}
	return m_pkContainer;
}

bool FvFlora::Unlock( FvFloraVertexContainer *pkContainer )
{
	if (pkContainer == m_pkContainer)
	{
		m_pkContainer = NULL;
		delete pkContainer;
	}
	return true;
}

void FvFlora::PostUnlock()
{
	FV_ASSERT( m_pkLockedBuffer != NULL );
	m_spVertexBuffer->unlock();
	m_pkLockedBuffer = NULL;
}

Ogre::MaterialPtr &FvFlora::GetMaterial()
{
	if(m_spMaterial.isNull())
	{
		m_spMaterial = Ogre::MaterialManager::getSingleton().getByName("FvFlora_AlphaTest");
		if(m_spMaterial.isNull())
		{
			FV_ERROR_MSG("FvFlora::GetMaterial material [FvFlora_AlphaTest] is not exist!");
			return m_spMaterial;
		}

		for(int iTech = 0; iTech < m_spMaterial->getNumTechniques(); iTech++)
		{
			Ogre::Pass *pkPass = m_spMaterial->getTechnique(iTech)->getPass(0);
			if(!pkPass) continue;
			for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
			{
				if(iTex >= pkPass->getNumTextureUnitStates()) break;
				Ogre::TextureUnitState *pkTex = pkPass->getTextureUnitState(iTex);
				if(pkTex)
				{
					pkTex->setTextureName(m_pkFloraTexture->pTexture(iTex)->getName());
				}
			}
		}
	}
	return m_spMaterial;
}

bool FvFlora::MoveBlocks( const FvVector2 &kCamPos2 )
{
	FvFloraBlock *pkCenterBlock = m_apBlocks[m_iCenterBlockY][m_iCenterBlockX];
	FvVector2 kDelta = kCamPos2 - pkCenterBlock->Center();
	int iMoveX = 0; int iMoveY = 0;
	float fVIZ = FV_FLORA_BLOCK_WIDTH / 2.f;

	if ( kDelta.x > fVIZ ) iMoveX = 1;
	else if ( kDelta.x < -fVIZ ) iMoveX = -1;

	if ( kDelta.y > fVIZ ) iMoveY = -1;
	else if ( kDelta.y < -fVIZ ) iMoveY = 1;

	if ( iMoveX )
	{
		int iLeft = (m_iCenterBlockX - (FV_FLORA_BLOCK_STRIDE/2) + FV_FLORA_BLOCK_STRIDE) % FV_FLORA_BLOCK_STRIDE;
		int iRight = (m_iCenterBlockX + (FV_FLORA_BLOCK_STRIDE/2)) % FV_FLORA_BLOCK_STRIDE;
		FvVector2 kDelta((float)iMoveX * FV_FLORA_MOVE_DIST, 0.f);
		int iColToMove = (iMoveX > 0) ? iLeft : iRight;
		for (int iY=0; iY<FV_FLORA_BLOCK_STRIDE; iY++)
		{
			FvFloraBlock *pkBlock = m_apBlocks[iY][iColToMove];
			pkBlock->Center(pkBlock->Center() + kDelta);
			m_kMovedBlocks.push_back( pkBlock );
		}
		m_iCenterBlockX = (m_iCenterBlockX + iMoveX + FV_FLORA_BLOCK_STRIDE) % FV_FLORA_BLOCK_STRIDE;
	} 
	else if ( iMoveY )
	{
		int iFront = (m_iCenterBlockY - (FV_FLORA_BLOCK_STRIDE/2) + FV_FLORA_BLOCK_STRIDE) % FV_FLORA_BLOCK_STRIDE;
		int iRear = (m_iCenterBlockY + (FV_FLORA_BLOCK_STRIDE/2)) % FV_FLORA_BLOCK_STRIDE;
		FvVector2 kDelta(0.f, (float)iMoveY * -FV_FLORA_MOVE_DIST);
		int iRowToMove = (iMoveY > 0) ? iFront : iRear;
		for (int iX=0; iX<FV_FLORA_BLOCK_STRIDE; iX++)
		{
			FvFloraBlock* block = m_apBlocks[iRowToMove][iX];
			block->Center(block->Center() + kDelta);
			m_kMovedBlocks.push_back( block );
		}
		m_iCenterBlockY = (m_iCenterBlockY + iMoveY + FV_FLORA_BLOCK_STRIDE) % FV_FLORA_BLOCK_STRIDE;
	}	

	return iMoveX || iMoveY;
}


void FvFlora::FillBlocks()
{
	if ( !m_kMovedBlocks.size() )
		return;

	if ( !this->PreLock() )
		return;

	std::vector<FvFloraBlock*> kBlocks;
	int iNumMovedBlocks = 0;
	while ( m_kMovedBlocks.size() > 0 )
	{
		FvFloraBlock *pkBlock = m_kMovedBlocks.back();
		if ( (iNumMovedBlocks<s_iAllowedBlockMovesPerFrame) && pkBlock->NeedsRefill() )
		{			
			pkBlock->Fill( m_uiNumVerticesPerBlock );
			if (!pkBlock->NeedsRefill())
			{
				iNumMovedBlocks++;
			}
		}
		if ( pkBlock->NeedsRefill() )
		{			
			kBlocks.push_back( pkBlock );
		}		
		m_kMovedBlocks.pop_back();
	}

	while (kBlocks.size())
	{
		m_kMovedBlocks.push_back( kBlocks.back() );
		kBlocks.pop_back();
	}

	this->PostUnlock();

	if (iNumMovedBlocks > 0)
	{
		this->AccumulateBoundingBoxes();
	}
}

void FvFlora::Update( float dTime, const FvVector3 &kCamPos )
{
	if ( !ms_bEnabled )
		return;

	if ( m_spVertexBuffer.isNull() )
		return;

	FvVector2 kCamPos2( kCamPos.x, kCamPos.y );

	if ( fabsf( m_kLastPos.x - kCamPos2.x ) > FV_FLORA_MOVE_DIST )
	{
		m_bCameraTeleport = true;
	}
	else if ( fabsf( m_kLastPos.y - kCamPos2.y ) > FV_FLORA_MOVE_DIST )
	{
		m_bCameraTeleport = true;
	}

	if ( s_bCull )
	{
		if ( m_bCameraTeleport )
		{
			this->TeleportCamera( kCamPos2 );
			m_bCameraTeleport = false;
		}
		else
		{
			this->MoveBlocks( kCamPos2 );
		}
	}

	m_kLastPos = kCamPos2;

	if ( m_kMovedBlocks.size() > 0 )
		this->FillBlocks();

	for ( int iY=0; iY<4; iY++ )
	{
		m_abCalculated[iY] = false;
	}
}


void FvFlora::AccumulateBoundingBoxes()
{
	int iLeft = (m_iCenterBlockX - (FV_FLORA_BLOCK_STRIDE/2) + FV_FLORA_BLOCK_STRIDE) % FV_FLORA_BLOCK_STRIDE;
	int iXEnd = iLeft + FV_FLORA_BLOCK_STRIDE;
	int iFront = (m_iCenterBlockY - (FV_FLORA_BLOCK_STRIDE/2) + FV_FLORA_BLOCK_STRIDE) % FV_FLORA_BLOCK_STRIDE;
	int iYEnd = iFront + FV_FLORA_BLOCK_STRIDE;

	int iIndex=0;
	FvBoundingBox kBB = FvBoundingBox::ms_kInsideOut;

	for (int iY=iFront; iY<iYEnd; iY+=5)
	{
		for (int iX=iLeft; iX<iXEnd; iX+=5)
		{
			MacroBB &kMBB = m_akMacroBB[iIndex];
			kMBB.m_kBB = FvBoundingBox::ms_kInsideOut;
			for (int iYY=0; iYY<5; iYY++)
			{
				for (int iXX=0; iXX<5; iXX++)
				{
					FvFloraBlock *pkBlock = m_apBlocks[(iY+iYY)%FV_FLORA_BLOCK_STRIDE][(iX+iXX)%FV_FLORA_BLOCK_STRIDE];
					if (pkBlock->BlockID() != -1)
						kMBB.m_kBB.AddBounds( pkBlock->Bounds() );
					kMBB.m_apBlocks[iYY*5+iXX] = pkBlock;
				}
			}
			iIndex++;
		}
	}
}

void FvFlora::Cull(FvCamera *pkCamera)
{
	for ( int i=0; i<25; i++ )
	{
		MacroBB &kMBB = m_akMacroBB[i];
		bool bFullyCulled = !pkCamera->IsVisible(kMBB.m_kBB);

		if ( bFullyCulled )
		{
			for (int j=0; j<25; j++)
			{
				kMBB.m_apBlocks[j]->Culled( true );
			}
		}
		else
		{
			for (int j=0; j<25; j++)
			{
				kMBB.m_apBlocks[j]->Cull(pkCamera);
			}
		}
	}
}

const FvMatrix& FvFlora::Transform( const FvFloraBlock &kBlock )
{
	int iBlockID = kBlock.BlockID();

	if ( iBlockID == -1 )
	{
		return FvMatrix::IDENTITY;
	}

	FV_ASSERT( iBlockID >= 0 );
	FV_ASSERT( iBlockID < 4 );

	if ( !m_abCalculated[iBlockID] )
	{
		CalculateTransform( kBlock );
		if ( !m_abCalculated[iBlockID] )
			return FvMatrix::IDENTITY;
	}

	return m_akTransform[iBlockID];
}


void FvFlora::CalculateTransform( const FvFloraBlock &kBlock )
{
	int iBlockID = kBlock.BlockID();

	FvVector3 kBlockPos( kBlock.Center().x, kBlock.Center().y, 0.f );
	FvTerrainFinder::Details kDetails = 
        FvTerrainPage::FindTerrainPage( kBlockPos );
	if (kDetails.m_pkMatrix == NULL) 
		return;

	FvMatrix kVertToWorld( *kDetails.m_pkMatrix );
	m_akTransform[iBlockID] = kVertToWorld;
	m_abCalculated[iBlockID] = true;
}

FvEcotype &FvFlora::EcotypeAt( const FvVector2 &kPos )
{
	if ( m_spData )
	{
		FvEcotype::ID uiID = FvEcotype::ID_AUTO;
		uiID = this->GenerateEcotypeID(kPos);

		if ( !m_apEcotypes[uiID] )
		{
			m_apEcotypes[uiID] = new FvEcotype( this );				
		}

		if ( !m_apEcotypes[uiID]->m_bIsInited && !m_apEcotypes[uiID]->m_bIsLoading )
		{
			m_apEcotypes[uiID]->Init( m_spData->OpenSection( "ecotypes" ), uiID );
		}		
		
		return *m_apEcotypes[uiID];
	}	

	return m_kDegenerateEcotype;
}

FvEcotype::ID FvFlora::GenerateEcotypeID( const FvVector2 &kPos2 )
{
	FvVector3 kPos3(kPos2.x, kPos2.y, 0.f);
	FvTerrainFinder::Details kDetails = 
		FvTerrainPage::FindTerrainPage( kPos3 );

	if (kDetails.m_pkPage)
	{
		FvTerrainPagePtr spPage = kDetails.m_pkPage;
		FvVector3 kRelPos = kDetails.m_pkInvMatrix->ApplyPoint( kPos3 );				
		FvVector3 kNormal = 
			spPage->NormalAt( kRelPos.x, kRelPos.y );
		float fSlope = FvRadiantToDegreef(acosf(FvClampEx(-1.0f, +1.0f, kNormal.z)));
		if (fSlope < m_fMaxSlope)
		{
			const FvString &kTextureName =
				spPage->MaterialName( kRelPos.x, kRelPos.y );	
			FvString kTexRoot = kTextureName;
			Ogre::StringUtil::toLowerCase(kTexRoot);
			FvStringHashMap<FvEcotype::ID>::iterator it = m_kTexToEcotype.find( kTexRoot );
			if (it != m_kTexToEcotype.end())
			{						
				return it->second;
			}
		}
	}
	
	return 0;
}

void FvFlora::TeleportCamera( const FvVector2 &kCamPos2 )
{
	FV_ASSERT( m_uiNumVerticesPerBlock != 0 );

	FvVector2 kTopLeft = kCamPos2 + FvVector2( -(float)FV_FLORA_BLOCK_STRIDE/2.f*FV_FLORA_BLOCK_WIDTH, (float)FV_FLORA_BLOCK_STRIDE/2.f*FV_FLORA_BLOCK_WIDTH );

	float fLeft = FV_FLORA_BLOCK_WIDTH * floorf( kTopLeft.x / FV_FLORA_BLOCK_WIDTH );
	float fTop = FV_FLORA_BLOCK_WIDTH * floorf( kTopLeft.y / FV_FLORA_BLOCK_WIDTH );

	int iOffset = 0;
	this->PreLock();

	FvVector2 kPos;
	kPos.y = fTop + FV_FLORA_BLOCK_WIDTH/2.f;
	for ( int iY=0; iY<FV_FLORA_BLOCK_STRIDE; iY++ )
	{
		kPos.x = fLeft + FV_FLORA_BLOCK_WIDTH/2.f;

		for ( int iX=0; iX<FV_FLORA_BLOCK_STRIDE; iX++ )
		{
			FvFloraBlock *pkBlock = m_apBlocks[iY][iX];
			pkBlock->Init( kPos, iOffset );
			iOffset += m_uiNumVerticesPerBlock;
			kPos.x += FV_FLORA_BLOCK_WIDTH;
		}

		kPos.y -= FV_FLORA_BLOCK_WIDTH;
	}

	m_bCameraTeleport = false;

	m_kMovedBlocks.clear();

	for ( int iY=0; iY<FV_FLORA_BLOCK_STRIDE; iY++ )
	{
		for ( int iX=0; iX<FV_FLORA_BLOCK_STRIDE; iX++ )
		{
			m_apBlocks[iY][iX]->Fill( m_uiNumVerticesPerBlock );
			if ( m_apBlocks[iY][iX]->NeedsRefill() )
			{
				m_kMovedBlocks.push_back( m_apBlocks[iY][iX] );
			}
		}
	}
	this->PostUnlock();

	m_iCenterBlockX = FV_FLORA_BLOCK_STRIDE/2;
	m_iCenterBlockY = FV_FLORA_BLOCK_STRIDE/2;
	this->AccumulateBoundingBoxes();
}

void FvFlora::ClearVertexBuffer( FvUInt32 uiOffset, FvUInt32 uiSize )
{
	if(!m_spVertexBuffer.isNull())
	{
		if (uiSize == 0 || (uiOffset + uiSize) > m_uiBufferSize )
			uiSize = m_uiBufferSize - uiOffset;

		void *pkBuffer = m_spVertexBuffer->lock(Ogre::HardwareBuffer::HBL_NO_OVERWRITE);
		if (pkBuffer)
		{
			ZeroMemory( (char*)pkBuffer + uiOffset, uiSize );
		}
		else
		{
			FV_ERROR_MSG( "FvFlora::ClearVertexBuffer - unable to lock vertex buffer\n" );
		}
		m_spVertexBuffer->unlock();
	}
	else
	{
		FV_ERROR_MSG( "FvFlora::ClearVertexBuffer - No vertex buffer to clear\n" );
	}
}

void FvFlora::InitialiseOffsetTable( float fBlurAmount /* = 2.f */ )
{
	srand(0);
	
	float fRadius = FV_FLORA_BLOCK_WIDTH / 2.f;
	fRadius = sqrtf( 2.f * (fRadius * fRadius) );

	for ( int i=0; i<FV_FLORA_LUT_SIZE; i++ )
	{
		float fT = ((float)rand() / (float)RAND_MAX) * FV_MATH_2PI_F;
		float fR = ((float)rand() / (float)RAND_MAX) * fRadius;
		m_akOffsets[i].x = fR * cosf(fT);
		m_akOffsets[i].y = fR * sinf(fT);
		m_afRandoms[i] = (float)rand() / (float)RAND_MAX;
	}
}

FvUInt32 FvFlora::MaxVBSize() const
{
	return this->m_uiMaxVBSize;
}

void FvFlora::SeedOffsetTable( const FvVector2 &kCenter )
{
	m_iLutSeed = (int)( ( fabsf(kCenter.x) * 5.f ) + ( fabsf(kCenter.y) * 13.f ) );
}

const FvVector2 &FvFlora::NextOffset()
{
	m_iLutSeed = (m_iLutSeed+1)%FV_FLORA_LUT_SIZE;
	return m_akOffsets[m_iLutSeed];
}


float FvFlora::NextRotation()
{
	m_iLutSeed = (m_iLutSeed+1)%FV_FLORA_LUT_SIZE;
	return fmodf( m_akOffsets[m_iLutSeed].x, 6.283f );
}


float FvFlora::NextRandomFloat()
{
	m_iLutSeed = (m_iLutSeed+1)%FV_FLORA_LUT_SIZE;
	return m_afRandoms[m_iLutSeed];
}

FvTerrainPagePtr FvFlora::GetTerrainPage( const FvVector3 &kPos, FvVector3 &kRelativePos, const FvVector2 *pkRefPt ) const
{
	if ( !pkRefPt || *pkRefPt != m_kLastRefPt )
	{
		m_kDetails = FvTerrainPage::FindTerrainPage( kPos );
		if ( pkRefPt )
			m_kLastRefPt = *pkRefPt;
	}

	if (m_kDetails.m_pkInvMatrix != NULL)
	{
		kRelativePos = m_kDetails.m_pkInvMatrix->ApplyPoint( kPos );
	}

	return m_kDetails.m_pkPage;
}

int FvFlora::GetTerrainPageID( const FvMatrix &kTerrainPageTransform ) const
{
	FvVector3 kPos( kTerrainPageTransform.ApplyToOrigin() );
	kPos /= 100.f;
	int iX = abs((int)(floorf(kPos.x + 0.5f)));
	int iY = abs((int)(floorf(kPos.y + 0.5f)));
	return ( iX%2 | (iY%2 << 1) );
}


const char* FvFlora::GroundType( float fX, float fY )
{
	FvEcotype &kEcotype = EcotypeAt( FvVector2(fX,fY) );
	return kEcotype.SoundTag().c_str();
}


void FvFlora::ResetBlockAt( float fX, float fY )
{
	FvFloraBlock *centerBlock = m_apBlocks[m_iCenterBlockY][m_iCenterBlockX];
	FvVector2 center = centerBlock->Center();

	float dx = fX - center.x;
	float dz = fY - center.y;
	if ( fabsf(dx) >= FV_FLORA_VISIBILITY )
		return;
	if ( fabsf(dz) >= FV_FLORA_VISIBILITY )
		return;
	
	int dxIdx = dx > 0.f ? int(dx/FV_FLORA_BLOCK_WIDTH + 0.5f) : int(dx/FV_FLORA_BLOCK_WIDTH - 0.5f);
	int dzIdx = dz > 0.f ? int(dz/FV_FLORA_BLOCK_WIDTH + 0.5f) : int(dz/FV_FLORA_BLOCK_WIDTH - 0.5f);

	int xIdx = ((m_iCenterBlockX + dxIdx) + FV_FLORA_BLOCK_STRIDE) % FV_FLORA_BLOCK_STRIDE;
	int zIdx = ((m_iCenterBlockY - dzIdx) + FV_FLORA_BLOCK_STRIDE) % FV_FLORA_BLOCK_STRIDE;

	FV_ASSERT( xIdx >= 0 )
	FV_ASSERT( zIdx >= 0 )
	FV_ASSERT( xIdx < FV_FLORA_BLOCK_STRIDE )
	FV_ASSERT( zIdx < FV_FLORA_BLOCK_STRIDE )

	FvFloraBlock* block = m_apBlocks[zIdx][xIdx];
	if (!block->NeedsRefill())
	{
		block->Invalidate();
		m_kMovedBlocks.push_back(block);
	}
}

void FvFlora::FloraReset()
{
	std::vector< FvFlora* > kFloras = FvFlora::Floras();
	for( unsigned i=0; i<kFloras.size(); i++)
	{
		kFloras[i]->m_bCameraTeleport = true;
	}
}

void FvFlora::VBSize( FvUInt32 uiNumBytes )
{
	if ( !m_spData )
		return;

	m_bCameraTeleport = true;
	
	FvUInt32 uiVertexSize = sizeof(FvFloraVertex);
	m_uiNumVertices =  uiNumBytes / uiVertexSize;

	const Ogre::RenderSystemCapabilities *pkCaps = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities();
	FV_ASSERT(pkCaps);
	FvUInt32 uiMaxPrims = pkCaps->getMaxPrimitiveCount();
	if (m_uiNumVertices > uiMaxPrims * 3)
	{	
		m_uiNumVertices = uiMaxPrims * 3;
		uiNumBytes = m_uiNumVertices * uiVertexSize;
		FV_INFO_MSG( "FvFlora::CreateUnmanagedObjects - reduced memory usage to %d, due to VB size restriction\n", uiNumBytes );
	}

	FvUInt32 uiBlockSize = (FV_FLORA_BLOCK_STRIDE*FV_FLORA_BLOCK_STRIDE);
	m_uiNumVerticesPerBlock  = std::max(m_uiNumVertices / uiBlockSize, (FvUInt32)3U);
	if ( m_uiNumVerticesPerBlock%3 != 0 )
	{
		m_uiNumVerticesPerBlock -= m_uiNumVerticesPerBlock % 3;
		m_uiNumVertices = m_uiNumVerticesPerBlock * uiBlockSize;
		uiNumBytes = m_uiNumVertices * uiVertexSize;
		FV_INFO_MSG( "FvFlora::VBSize - reduced memory usage to %d, due to alignment requirements\n", uiNumBytes );
	}
	m_uiNumVertices = m_uiNumVerticesPerBlock * uiBlockSize;
	uiNumBytes = m_uiNumVertices * uiVertexSize;
	m_uiVBSize = uiNumBytes;

	FV_INFO_MSG( "FvFlora::VBSize - %d triangles per flora block\n", m_uiNumVerticesPerBlock / 3 );
	FV_INFO_MSG( "FvFlora::VBSize - %d KB used for vertex buffer\n", m_uiVBSize / 1024 );

	m_spVertexBuffer.setNull();
	m_uiCapacity = 0;
	m_spVertexBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		uiVertexSize,
		m_uiNumVertices, 
		Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY );
	m_uiBufferSize = uiNumBytes;
	if(!m_spVertexBuffer.isNull())
	{
		m_uiCapacity = uiNumBytes / sizeof( FvFloraVertex );
		this->ClearVertexBuffer();
	}

	m_bCameraTeleport = true;
}

void FvFlora::FloraVBSize( FvUInt32 uiNumBytes )
{
	std::vector< FvFlora* > kFloras = FvFlora::Floras();
	for( unsigned i=0; i<kFloras.size(); i++)
	{
		kFloras[i]->VBSize( uiNumBytes );
	}
}

//void FloraSettings::init( FvXMLSectionPtr resXML )
//{
//	this->floraSettings_ = 
//		Moo::makeCallbackGraphicsSetting(
//			"FLORA_DENSITY", "FvFlora Density", *this, 
//			&FloraSettings::setFloraOption, 0, 
//			false, false);
//				
//	if (resXML.exists())
//	{
//		DataSectionIterator sectIt  = resXML->begin();
//		DataSectionIterator sectEnd = resXML->end();
//		while (sectIt != sectEnd)
//		{
//			static const float undefined = -1.0f;
//			float value = (*sectIt)->readFloat("value", undefined);
//			std::string label = (*sectIt)->readString("label");
//			if (!label.empty() && value != undefined)
//			{
//				this->floraSettings_->addOption(label, label, true);
//				this->floraOptions_.push_back(value);
//			}
//			++sectIt;
//		}
//	}
//	else
//	{
//		this->floraSettings_->addOption("HIGHT", "Height", true);
//		this->floraOptions_.push_back(1.0f);
//	}
//	Moo::GraphicsSetting::add(this->floraSettings_);
//}
//
//float FloraSettings::vbRatio() const
//{
//	return this->floraSettings_.exists()
//		? this->floraOptions_[this->floraSettings_->activeOption()]
//		: 1.0f;
//}
//
//bool FloraSettings::isInitialised() const
//{
//	return this->floraSettings_.exists();
//}
//
//FloraSettings & FloraSettings::instance()
//{
//	static FloraSettings instance;
//	return instance;
//}
//
//void FloraSettings::setFloraOption(int optionIndex)
//{
//	float ratio = this->floraOptions_[optionIndex];
//
//	std::vector< FvFlora* > kFloras = FvFlora::kFloras();
//	for( unsigned i=0; i<kFloras.size(); i++)
//	{
//		kFloras[i]->vbSize(int32(kFloras[i]->maxVBSize() * this->vbRatio()));
//	}
//
//	FvFlora::enabled( ratio != 0.f );
//}

//Ogre::String FvFloraFactory::FACTORY_TYPE_NAME = "Flora";
//
//const Ogre::String &FvFloraFactory::getType(void) const
//{
//	return FACTORY_TYPE_NAME;
//}
//
//Ogre::MovableObject *FvFloraFactory::createInstanceImpl( const Ogre::String &kName,
//												 const Ogre::NameValuePairList *pkParams )
//{
//	return OGRE_NEW FvFlora;
//}
//
//void FvFloraFactory::destroyInstance( Ogre::MovableObject *pkObj )
//{
//	OGRE_DELETE pkObj;
//}
