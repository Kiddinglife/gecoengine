#include "FvEcotype.h"
#include "FvFlora.h"
#include "FvFloraTexture.h"
#include "FvFloraRenderer.h"
#include "FvEcotypeGenerators.h"

#include <FvWatcher.h>
#include <FvBGTaskManager.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

FvSimpleMutex FvEcotype::ms_kDeleteMutex;

FvEcotype::FvEcotype( FvFlora *pkFlora) :
m_kTag( "None" ),
m_iRefCount( 0 ),
m_uiID( 0 ),
m_kUVOffset( 0.f, 0.f ),	
m_pkGenerator( NULL ),
m_pkLoadInfo( NULL ),
m_bIsLoading( false ),
m_bIsInited ( false ),
m_pkFlora( pkFlora )
{
	m_kTextureResources.resize(FV_FLORA_TEXTURE_NUMBER,"");
	m_spTextures.resize(FV_FLORA_TEXTURE_NUMBER,NULL);
}


FvEcotype::~FvEcotype()
{
	if ( m_pkLoadInfo )
		m_pkLoadInfo->m_pkEcotype = NULL;

	if ( m_pkGenerator )
		delete m_pkGenerator;
}

void FvEcotype::Init( FvXMLSectionPtr spAllEcotypesSection, FvUInt8 uiID  )
{		
	this->m_uiID = uiID;
	for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
		this->m_kTextureResources[iTex] = "";

	BkLoadInfo *pkLInfo = new BkLoadInfo;	
	pkLInfo->m_spSection = spAllEcotypesSection;	
	pkLInfo->m_pkLoadingTask = new FvCStyleBackgroundTask( &FvEcotype::BackgroundInit, pkLInfo,
						&FvEcotype::OnBackgroundInitComplete, pkLInfo );
	pkLInfo->m_pkEcotype = this;
	this->m_pkLoadInfo = pkLInfo;
	this->m_bIsLoading = true;
	this->m_bIsInited = true;

	FvBGTaskManager::Instance().AddBackgroundTask( pkLInfo->m_pkLoadingTask );
}

void FvEcotype::Finz()
{
	FV_ASSERT( m_iRefCount == 0 );
	FV_ASSERT( !this->m_bIsLoading );
	this->m_bIsInited = false;
	for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
	{
		m_spTextures[iTex] = NULL;
		m_kTextureResources[iTex] = "";
	}
	if (m_pkGenerator)
	{
		delete m_pkGenerator;
		m_pkGenerator = NULL;
	}
}

void FvEcotype::BackgroundInit( void *pkLoadInfo )
{
	FvSimpleMutexHolder kMutex( ms_kDeleteMutex );

	BkLoadInfo *pkLInfo = (BkLoadInfo*)pkLoadInfo;
	FvEcotype *pkEcotype = pkLInfo->m_pkEcotype;

	if (!pkEcotype)
		return;

	FvXMLSectionPtr spSection = pkLInfo->m_spSection->OpenChild( pkEcotype->GetID() );
	if ( spSection )
	{
		pkEcotype->Tag( spSection->ReadString( "sound_tag", "None" ) );
		pkEcotype->Generator( FvEcotypeGenerator::Create(spSection, *pkEcotype) );
		return;
	}

	FV_ASSERT( !pkEcotype->Generator() );
	pkEcotype->Generator( new FvEmptyEcotype() );
	for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
		pkEcotype->TextureResource("",iTex);
	FV_ERROR_MSG( "Could not find ecotype definition for ecotype id %d\n", pkEcotype->GetID() );	
}

void FvEcotype::OnBackgroundInitComplete( void *pkLoadInfo )
{
	BkLoadInfo *pkLInfo = (BkLoadInfo*)pkLoadInfo;

	if ( pkLInfo->m_pkEcotype )
	{
		pkLInfo->m_pkEcotype->m_bIsLoading = false;
		pkLInfo->m_pkEcotype->m_pkLoadInfo = NULL;
		//FvXMLSectionPtr spSection = pkLInfo->m_spSection->OpenChild( pkLInfo->m_pkEcotype->GetID() );
		//if ( spSection )
		//{
		//	FvD3DSystemMemoryTexturePtr spTex = pkLInfo->m_pkEcotype->pTexture();
		//}
	}

	delete pkLInfo;	
}

void FvEcotype::IncRef()
{
	++m_iRefCount;
	if (m_iRefCount==1)
	{		
		FV_ASSERT( !this->m_bIsLoading );
		FV_ASSERT( this->m_bIsInited );
		GrabTexture();
	}
}

void FvEcotype::DecRef()
{
	m_iRefCount--;
	if (!m_iRefCount)
	{
		FreeTexture();
		this->Finz();
	}
}

void FvEcotype::GrabTexture()
{
	if ( m_spTextures[0] )
	{
		this->OffsetUV( m_pkFlora->FloraTexture()->Allocate( m_uiID, m_spTextures ) );
		for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
			m_spTextures[iTex] = NULL;
	}	
}

void FvEcotype::FreeTexture()
{
	if ( m_kTextureResources[0] != "" )
	{				
		m_pkFlora->FloraTexture()->Deallocate( m_uiID );
	}
}

void FvEcotype::OffsetUV( FvVector2 &kOffset )
{
	m_kUVOffset = kOffset;	
}

FvUInt32 FvEcotype::Generate(
		class FvFloraVertexContainer *pkVerts,
		FvUInt32 uiIndex,
		FvUInt32 uiNumVerts,
		const FvMatrix &kObjectToWorld,
		const FvMatrix &kObjectToZone,
		FvBoundingBox &kBB )
{
	if ( m_pkGenerator )
		return m_pkGenerator->Generate( m_kUVOffset, pkVerts, 
		uiIndex, uiNumVerts, kObjectToWorld, kObjectToZone, kBB );
	else
	{
		pkVerts->Clear(uiNumVerts);
		return uiNumVerts;
	}
}

bool FvEcotype::IsEmpty() const
{
	if (m_pkGenerator)
		return m_pkGenerator->IsEmpty();

	return true;
}