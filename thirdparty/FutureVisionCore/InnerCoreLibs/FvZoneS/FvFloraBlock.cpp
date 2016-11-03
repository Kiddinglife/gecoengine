#include "FvFloraBlock.h"
#include "FvFlora.h"
#include "FvFloraRenderer.h"

#include "FvCamera.h"

#include <FvPlane.h>
#include <FvWatcher.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

static float s_fEcotypeBlur = 1.0f;
void SetEcotypeBlur( float fAmount )
{
	s_fEcotypeBlur = fAmount;
	std::vector< FvFlora* > kFloras = FvFlora::Floras();
	for( unsigned i = 0; i < kFloras.size(); i++)
	{
		kFloras[i]->InitialiseOffsetTable(fAmount);
		kFloras[i]->FloraReset();
	}
}

float GetEcotypeBlur()
{
	return s_fEcotypeBlur;
}

static float s_fPositionBlur = 1.2f;
void SetPositionBlur( float fAmount )
{
	s_fPositionBlur = fAmount;
	std::vector< FvFlora* > kFloras = FvFlora::Floras();
	for( unsigned i=0; i<kFloras.size(); i++)
	{
		kFloras[i]->InitialiseOffsetTable(fAmount);
		kFloras[i]->FloraReset();
	}
}

float GetPositionBlur()
{
	return s_fPositionBlur;
}

FvFloraBlock::FvFloraBlock( FvFlora *pkFlora ):
	m_bRefill( true ),
	m_kCenter( 0.f, 0.f ),
	m_bCulled( true ),
	m_iBlockID( -1 ),
	m_pkFlora( pkFlora )
{
	m_pkRenderable = new FvFloraRenderable(this);
}

FvFloraBlock::~FvFloraBlock()
{
	delete m_pkRenderable;
}

void FvFloraBlock::Init( const FvVector2& pos, FvUInt32 offset )
{
	static bool s_bAdded = false;
	if (!s_bAdded)
	{		
		s_bAdded = true;
		FV_WATCH( "Client Settings/Flora/Ecotype Blur",
			&::GetEcotypeBlur,
			&::SetEcotypeBlur,
			"Multiplier for ecotype sample point for each flora object.  Set "
			"to higher values means neighbouring ecotypes will encroach upon "
			"a flora block" );
		FV_WATCH( "Client Settings/Flora/Position Blur",
			&::GetPositionBlur,
			&::SetPositionBlur,
			"Multiplier for positioning each flora object.  Set to a higher "
			"value to make flora objects encroach upon neighbouring blocks.");
	}

	m_uiOffset = offset;
	this->Center( pos );
}

class FloraItem
{
public:
	FloraItem( FvEcotype* e, FvMatrix& o, FvVector2& p ):
		m_pkEcotype(e),
		m_kObjectToWorld(o),
		m_kEcotypeSamplePt(p)
	{
	};

	FvEcotype *m_pkEcotype;
	FvMatrix m_kObjectToWorld;
	FvVector2 m_kEcotypeSamplePt;
};

typedef std::vector<FloraItem>	FloraItems;

void FvFloraBlock::Fill( FvUInt32 uiNumVertsAllowed )
{
	FvMatrix kObjectToZone;
	FvMatrix kObjectToWorld;

	FvVector2 kCenter( m_kBB.MinBounds().x, m_kBB.MinBounds().y );
	kCenter.x += FV_FLORA_BLOCK_WIDTH/2.f;
	kCenter.y += FV_FLORA_BLOCK_WIDTH/2.f;
	FvTerrainFinder::Details kDetails = 
		FvTerrainPage::FindTerrainPage( 
			FvVector3(kCenter.x,kCenter.y,0.f) );
	if (!kDetails.m_pkPage)
		return;
	kCenter.x -= FV_FLORA_BLOCK_WIDTH/2.f;	
	kCenter.y -= FV_FLORA_BLOCK_WIDTH/2.f;	

	const FvMatrix &kZoneToWorld = *kDetails.m_pkMatrix;
	m_iBlockID = (int)m_pkFlora->GetTerrainPageID( kZoneToWorld );	
	FvMatrix kWorldToZone = *kDetails.m_pkInvMatrix;	
	FvUInt32 uiNumVertices = uiNumVertsAllowed;	

	FloraItems kItems;

	m_pkFlora->SeedOffsetTable( kCenter );

	FvVector2 kEcotypeSamplePt;
	FvUInt32 uiIndex = 0;
	bool bHasEmptyEcotype = false;

	while (1)
	{
		if (!NextTransform(kCenter, kObjectToWorld, kEcotypeSamplePt))
			break;			

		kObjectToZone.Multiply( kObjectToWorld, kWorldToZone );
		FvEcotype &kEcotype = m_pkFlora->EcotypeAt( kEcotypeSamplePt );

		if (kEcotype.m_bIsLoading)
		{				
			return;
		}

		if (kEcotype.IsEmpty())
		{
			bHasEmptyEcotype = true;
			break;
		}

		FloraItem kFItem( &kEcotype, kObjectToWorld, kEcotypeSamplePt );
		kItems.push_back(kFItem);

		FvUInt32 nVerts = kEcotype.Generate( NULL, uiIndex, uiNumVertices, kFItem.m_kObjectToWorld, kObjectToZone, m_kBB );
		if (nVerts == 0)
			break;			
		uiNumVertices -= nVerts;
		if (uiNumVertices == 0)
			break;			
		uiIndex++;
	}

	uiIndex = 0;
	uiNumVertices = uiNumVertsAllowed;
	m_kBB = FvBoundingBox::ms_kInsideOut;
	FvFloraVertexContainer *pkVerts = m_pkFlora->Lock( m_uiOffset, uiNumVertsAllowed );

	if (!bHasEmptyEcotype)
	{
		m_pkFlora->SeedOffsetTable( kCenter );

		for (FvUInt32 i=0; i<kItems.size(); i++)
		{
			FloraItem& kFItem = kItems[i];
			FvEcotype& kEcotype = *kFItem.m_pkEcotype;
			m_kEcotypes.push_back( &kEcotype );
			kEcotype.IncRef();
			kObjectToZone.Multiply( kFItem.m_kObjectToWorld, kWorldToZone );
			FvUInt32 nVerts = kEcotype.Generate( pkVerts, uiIndex, uiNumVertices, kFItem.m_kObjectToWorld, kObjectToZone, m_kBB );
			if (nVerts == 0)			
				break;			
			uiIndex++;
			uiNumVertices -= nVerts;
			if (uiNumVertices == 0)
				break;
		}
	}

	FV_ASSERT( (uiNumVertices%3) == 0 );
	pkVerts->Clear( uiNumVertices );
	m_pkFlora->Unlock( pkVerts );

	if ( m_kBB == FvBoundingBox::ms_kInsideOut )
		m_iBlockID = -1;
	m_bRefill = false;
}

void FvFloraBlock::Center( const FvVector2 &kPos )
{
	m_kCenter = kPos;
	this->Invalidate();
}

void FvFloraBlock::Invalidate()
{
	FV_ASSERT(m_pkRenderable && 
		m_pkRenderable->m_pkVertexData && 
		m_pkRenderable->m_pkVertexData->vertexBufferBinding);
	Ogre::VertexBufferBinding* pkBind = m_pkRenderable->m_pkVertexData->vertexBufferBinding;
	if(pkBind->isBufferBound(0))
		pkBind->unsetBinding(0);

	m_kBB.SetBounds(FvVector3(	m_kCenter.x - FV_FLORA_BLOCK_WIDTH/2.f,
								m_kCenter.y - FV_FLORA_BLOCK_WIDTH/2.f, 
								-20000.f ),
					FvVector3(	m_kCenter.x + FV_FLORA_BLOCK_WIDTH/2.f,
								m_kCenter.y + FV_FLORA_BLOCK_WIDTH/2.f, 
								-20000.f ));
	m_iBlockID = -1;

	std::vector<FvEcotype*>::iterator it = m_kEcotypes.begin();
	std::vector<FvEcotype*>::iterator end = m_kEcotypes.end();

	while ( it != end )
	{
		if (*it)
			(*it)->DecRef();
		it++;
	}

	m_kEcotypes.clear();

	m_bRefill = true;
}

void FvFloraBlock::Cull(FvCamera *pkCamera)
{
	if (m_iBlockID != -1 )
	{
		m_bCulled = !pkCamera->IsVisible(m_kBB);
	}
	else
	{
		m_bCulled = true;
	}
}

bool FvFloraBlock::NextTransform( const FvVector2 &kCenter, FvMatrix &kRet, FvVector2 &kRetEcotypeSamplePt )
{
	FvVector2 kOff( m_pkFlora->NextOffset() );	
	kRetEcotypeSamplePt.x = kOff.x * s_fEcotypeBlur + kCenter.x;
	kRetEcotypeSamplePt.y = kOff.y * s_fEcotypeBlur + kCenter.y;

	kOff = m_pkFlora->NextOffset();
	float fRotZ( m_pkFlora->NextRotation() );
	FvVector3 kPos( kCenter.x + (kOff.x * s_fPositionBlur), kCenter.y + (kOff.y * s_fPositionBlur), 0.f );

	FvVector3 kRelPos;
	FvTerrainPagePtr spPage = m_pkFlora->GetTerrainPage( kPos, kRelPos, NULL );

	if ( !spPage )
	{
		return false;
	}
	else
	{		
		kPos.z = spPage->HeightAt( kRelPos.x, kRelPos.y );
		if ( kPos.z == FV_NO_TERRAIN )
			return false;
		FvVector3 kIntNorm = spPage->NormalAt( kRelPos.x, kRelPos.y );

		const FvPlane kPlane( kIntNorm, kIntNorm.DotProduct( kPos ) );
		FvVector3 kXYZ[2];
		kXYZ[0].Set( 0.f, 0.f, kPlane.Z( 0.f, 0.f ) );
		kXYZ[1].Set( 0.f, 1.f, kPlane.Z( 0.f, 1.f ) );
		FvVector3 kUP = kXYZ[1] - kXYZ[0];
		kUP.Normalise();
		kRet[0] = kUP.CrossProduct(kPlane.Normal());
		kRet[1] = kUP;
		kRet[2] = kPlane.Normal();

		FvMatrix kRot;
		kRot.SetRotateZ( fRotZ );
		kRet.PreMultiply( kRot );

		kRet[3] = kPos;
		kRet.m[3][3] = 1.0f;
	}

	return true;
}