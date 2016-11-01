#include "FvFloraTexture.h"

#include <FvWatcher.h>

#include <OgreRoot.h>
#include <OgreTextureManager.h>
#include <OgreResourceGroupManager.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

static const Ogre::PixelFormat FV_FLORA_TEXTURE_OGRE_FORMAT[FV_FLORA_TEXTURE_NUMBER] =
{
	Ogre::PF_DXT3,
	Ogre::PF_DXT1
};

static const D3DFORMAT FV_FLORA_TEXTURE_D3D_FORMAT[FV_FLORA_TEXTURE_NUMBER] =
{
	D3DFMT_DXT3,
	D3DFMT_DXT1
};

static int s_iShotID = 0;
static bool s_bSave = false;
static bool s_bUseTexture = true;

FvCompressedMipMapCalculator::FvCompressedMipMapCalculator( bool bIncludeHeader ):
	m_bIncludeHeader( bIncludeHeader ),
	m_iWidth( 0 ),
	m_iHeight( 0 )
{
}

void FvCompressedMipMapCalculator::Dimensions( int iWidth, int iHeight )
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;

	m_kMipMaps.clear();
	m_kData.m_uiNumMipMaps = 0;
	m_kData.m_uiNumBytes = m_bIncludeHeader ? 128 : 0;

	int iW=m_iWidth;
	int iH=m_iHeight;

	while ( iW>=4 && iH>=4 )
	{
		MipData md;
		md.m_uiOffset = m_kData.m_uiNumBytes;
		md.m_uiNumBytesPerRow = iW * 4;
		md.m_uiNumRows = iH/4;
		md.m_uiNumBytes = iW * iH;
		m_kMipMaps.push_back( md );

		m_kData.m_uiNumBytes += md.m_uiNumBytes;
		iW = iW/2;
		iH = iH/2;
		m_kData.m_uiNumMipMaps++;
	}
}

void FvCompressedMipMapCalculator::MipMap( int iLevel, MipData &kRet )
{
	kRet = m_kMipMaps[iLevel];
}

FvUInt32 FvCompressedMipMapCalculator::DDSSize()
{
	return m_kData.m_uiNumBytes;
}

FvUInt32 FvCompressedMipMapCalculator::DataSize()
{
	return m_kData.m_uiNumBytes - (m_bIncludeHeader ? 128 : 0);	
}

FvUInt32 FvCompressedMipMapCalculator::NumMipMaps()
{
	return m_kData.m_uiNumMipMaps;
}

#define ERRORSTRING( code, ext )											\
	if (hRet == code)														\
{																			\
	FvSNPrintf( acRes, sizeof(acRes), #code "(0x%08x) : %s", code, ext );	\
}

FvString D3D9ErrorAsString( HRESULT hRet )
{
	char acRes[1024];
	ERRORSTRING( D3D_OK, "No error occurred." )
	else ERRORSTRING( D3DOK_NOAUTOGEN, "This is a success code. However, the autogeneration of mipmaps is not supported for this format. This means that resource creation will succeed but the mipmap levels will not be automatically generated." )
	else ERRORSTRING( D3DERR_CONFLICTINGRENDERSTATE, "The currently set render states cannot be used together." )
	else ERRORSTRING( D3DERR_CONFLICTINGTEXTUREFILTER, "The current texture filters cannot be used together." )
	else ERRORSTRING( D3DERR_CONFLICTINGTEXTUREPALETTE, "The current textures cannot be used simultaneously." )
	else ERRORSTRING( D3DERR_DEVICELOST, "The device has been lost but cannot be reset at this time. Therefore, rendering is not possible." )
	else ERRORSTRING( D3DERR_DEVICENOTRESET, "The device has been lost but can be reset at this time." )
	else ERRORSTRING( D3DERR_DRIVERINTERNALERROR, "Internal driver error. Applications should destroy and recreate the device when receiving this error. For hints on debugging this error, see Driver Internal Errors (Direct3D 9)." )
	else ERRORSTRING( D3DERR_DRIVERINVALIDCALL, "Not used." )
	else ERRORSTRING( D3DERR_INVALIDCALL, "The method call is invalid. For example, a method's parameter may not be a valid pointer." )
	else ERRORSTRING( D3DERR_INVALIDDEVICE, "The requested device type is not valid." )
	else ERRORSTRING( D3DERR_MOREDATA, "There is more data available than the specified buffer size can hold." )
	else ERRORSTRING( D3DERR_NOTAVAILABLE, "This device does not support the queried technique." )
	else ERRORSTRING( D3DERR_NOTFOUND, "The requested item was not found." )
	else ERRORSTRING( D3DERR_OUTOFVIDEOMEMORY, "Direct3D does not have enough display memory to perform the operation." )
	else ERRORSTRING( D3DERR_TOOMANYOPERATIONS, "The application is requesting more texture-filtering operations than the device supports." )
	else ERRORSTRING( D3DERR_UNSUPPORTEDALPHAARG, "The device does not support a specified texture-blending argument for the alpha channel." )
	else ERRORSTRING( D3DERR_UNSUPPORTEDALPHAOPERATION, "The device does not support a specified texture-blending operation for the alpha channel." )
	else ERRORSTRING( D3DERR_UNSUPPORTEDCOLORARG, "The device does not support a specified texture-blending argument for color values." )
	else ERRORSTRING( D3DERR_UNSUPPORTEDCOLOROPERATION, "The device does not support a specified texture-blending operation for color values." )
	else ERRORSTRING( D3DERR_UNSUPPORTEDFACTORVALUE, "The device does not support the specified texture factor value. Not used; provided only to support older drivers." )
	else ERRORSTRING( D3DERR_UNSUPPORTEDTEXTUREFILTER, "The device does not support the specified texture filter." )
	else ERRORSTRING( D3DERR_WASSTILLDRAWING, "The previous blit operation that is transferring information to or from this surface is incomplete." )
	else ERRORSTRING( D3DERR_WRONGTEXTUREFORMAT, "The pixel format of the texture surface is not valid." )

	else ERRORSTRING( D3DXERR_CANNOTMODIFYINDEXBUFFER, "The index buffer cannot be modified." )
	else ERRORSTRING( D3DXERR_INVALIDMESH, "The mesh is invalid." )
	else ERRORSTRING( D3DXERR_CANNOTATTRSORT, "Attribute sort (D3DXMESHOPT_ATTRSORT) is not supported as an optimization technique." )
	else ERRORSTRING( D3DXERR_SKINNINGNOTSUPPORTED, "Skinning is not supported." )
	else ERRORSTRING( D3DXERR_TOOMANYINFLUENCES, "Too many influences specified." )
	else ERRORSTRING( D3DXERR_INVALIDDATA, "The data is invalid." )
	else ERRORSTRING( D3DXERR_LOADEDMESHASNODATA, "The mesh has no data." )
	else ERRORSTRING( D3DXERR_DUPLICATENAMEDFRAGMENT, "A fragment with that name already exists." )
	else ERRORSTRING( D3DXERR_CANNOTREMOVELASTITEM, "The last item cannot be deleted." )

	else ERRORSTRING( E_FAIL, "An undetermined error occurred inside the Direct3D subsystem." )
	else ERRORSTRING( E_INVALIDARG, "An invalid parameter was passed to the returning function." )
	//	else ERRORSTRING( E_INVALIDCALL, "The method call is invalid. For example, a method's parameter may have an invalid value." )
	else ERRORSTRING( E_NOINTERFACE, "No object interface is available." )
	else ERRORSTRING( E_NOTIMPL, "Not implemented." )
	else ERRORSTRING( E_OUTOFMEMORY, "Direct3D could not allocate sufficient memory to complete the call." )
	else ERRORSTRING( S_OK, "No error occurred." )
	else
	{
		FvSNPrintf( acRes, sizeof(acRes), "Unknown(0x%08x)", hRet );
	}

	return	FvString(acRes);
}

FvD3DSystemMemoryTexture::FvD3DSystemMemoryTexture(
						 const FvString &kResourceID, 
						 D3DFORMAT eFormat):
m_pkTexture(NULL),
m_uiWidth(0),
m_uiHeight(0),
m_eFormat(eFormat),
m_kResourceID(kResourceID),
m_bFailedToLoad(false)
{
	Ogre::DataStreamPtr spFileStream;
	try
	{
		spFileStream = Ogre::ResourceGroupManager::getSingleton().
			openResource(m_kResourceID);
	}
	catch (...){}

	if(spFileStream.isNull())
	{
		FV_ERROR_MSG("FvD3DSystemMemoryTexture::FvD3DSystemMemoryTexture %s not exist",kResourceID);
		m_bFailedToLoad = true;
		return;
	}

	char *pkData = (char*)OGRE_MALLOC(spFileStream->size(),Ogre::MEMCATEGORY_GENERAL);
	size_t stLength = spFileStream->size();
	spFileStream->read(pkData,stLength);

	IDirect3DDevice9 *pkDevice = static_cast<Ogre::D3D9RenderSystem*>(
		Ogre::Root::getSingleton().getRenderSystem())->getDevice();
	static PALETTEENTRY apPalette[256];
	HRESULT hRet = D3DXCreateTextureFromFileInMemoryEx( pkDevice,pkData, stLength,
		D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, m_eFormat, 
		D3DPOOL_SYSTEMMEM, D3DX_FILTER_BOX|D3DX_FILTER_MIRROR, 
		D3DX_FILTER_BOX|D3DX_FILTER_MIRROR, 0, NULL, apPalette,&m_pkTexture );

	OGRE_FREE(pkData,Ogre::MEMCATEGORY_GENERAL);

	if(SUCCEEDED(hRet))
	{
		IDirect3DSurface9 *pkSurface;
		if( SUCCEEDED( m_pkTexture->GetSurfaceLevel( 0, &pkSurface ) ) )
		{
			D3DSURFACE_DESC kDesc;
			pkSurface->GetDesc( &kDesc );

			m_uiWidth = kDesc.Width;
			m_uiHeight = kDesc.Height;
			m_eFormat = kDesc.Format;

			pkSurface->Release();
			pkSurface = NULL;
		}
	}
	else
	{
		FV_ERROR_MSG( "FvD3DSystemMemoryTexture::FvD3DSystemMemoryTexture(%s): Unknown texture format\n",
			m_kResourceID.c_str() );
		m_bFailedToLoad = true;
	}
}

FvD3DSystemMemoryTexture::~FvD3DSystemMemoryTexture()
{
	if(m_pkTexture)
	{
		m_pkTexture->Release();
		m_pkTexture = NULL;
	}
}

FvFloraTexture::FvFloraTexture():
	m_iWidth( 0 ),
	m_iHeight( 0 ),
	m_iWidthPerBlock( 0 ),
	m_iHeightPerBlock( 0 ),
	m_iNumBlocksWide( FV_FLORA_MAX_ECOTYPES ),
	m_iNumBlocksHigh( 1 ),
	m_kLargeMap( false ),
	m_kMediumMap( true ),
	m_kSmallMap( true )
{
	for ( int i=0; i<FV_FLORA_MAX_ECOTYPES; i++ )
	{
		m_akBlocks[i].m_iEcotype = -1;
		m_akBlocks[i].m_kTexNames.resize(FV_FLORA_TEXTURE_NUMBER,"");
	}

	static bool s_createdWatchers = false;
	if (!s_createdWatchers)
	{
		FV_WATCH( "Client Settings/Flora/Save flora texture",
			s_bSave,
			FvWatcher::WT_READ_WRITE,
			"If set to 0, will save the current composite flora texture to disk"
			" at the next available opportunity." );
		FV_WATCH( "Client Settings/Flora/Use flora texture",
			s_bUseTexture,
			FvWatcher::WT_READ_WRITE,
			"Enabled use of the flora texture.  Setting to false will display "
			"lit flora triangles only." );
		s_createdWatchers = true;
	}

	m_spTextures.resize(FV_FLORA_TEXTURE_NUMBER);
}

FvFloraTexture::~FvFloraTexture()
{
	DeleteUnmanagedObjects();
}

bool FvFloraTexture::Init( FvXMLSectionPtr spSection )
{
	if ( !spSection )
		return false;

	m_iWidthPerBlock = spSection->ReadInt( "texture_width", 256 );
	m_iHeightPerBlock = spSection->ReadInt( "texture_height", 128 );
	m_iNumBlocksWide = FV_FLORA_MAX_ECOTYPES;
	m_iNumBlocksHigh = 1;
	m_iWidth = m_iWidthPerBlock * m_iNumBlocksWide;
	m_iHeight = m_iHeightPerBlock;

	const Ogre::RenderSystemCapabilities *pkCaps = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities();
	FV_ASSERT(pkCaps);
	int iCapsMaxWidth = pkCaps->getMaxTextureWidth();
	int iCapsMaxHeight = pkCaps->getMaxTextureHeight();

	while( iCapsMaxWidth < m_iWidth )
	{
		m_iNumBlocksWide /= 2;
		m_iNumBlocksHigh *= 2;
		m_iWidth /= 2;
		m_iHeight *= 2;
		if ( m_iHeight > iCapsMaxHeight )
		{
			FV_CRITICAL_MSG( "FvFloraTexture::Init: "
				"device cannot support large enough textures for the flora\n" );
		}
	}
	m_kLargeMap.Dimensions( m_iWidth, m_iHeight );
	m_kMediumMap.Dimensions( m_iWidthPerBlock*2, m_iHeightPerBlock*2 );
	m_kSmallMap.Dimensions( m_iWidthPerBlock, m_iHeightPerBlock );

	int i = 0;
	int j = 0;
	while ( i < FV_FLORA_MAX_ECOTYPES )
	{
		m_akBlocks[i].m_kOffset = FvVector2( 
				float(i%m_iNumBlocksWide) / m_iNumBlocksWide, float(j) / m_iNumBlocksHigh);
		m_akBlocks[i].m_iPixelOffsetX = int(m_akBlocks[i].m_kOffset.x * m_iWidth);
		m_akBlocks[i].m_iPixelOffsetY = int(m_akBlocks[i].m_kOffset.y * m_iHeight);	
		for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
			m_akBlocks[i].m_kTexNames[iTex] = "";
		i++;
		j = i / m_iNumBlocksWide;
	}
	
	FV_INFO_MSG( "FvFloraTexture::Init - texture(%d,%d), blocks(%d,%d), blocksize(%d,%d)\n",
		m_iWidth, m_iHeight, m_iNumBlocksWide, m_iNumBlocksHigh, m_iWidthPerBlock, m_iHeightPerBlock);

	return true;
}

void FvFloraTexture::Activate()
{
	this->m_bIsActive = true;
	this->CreateUnmanagedObjects();
}

void FvFloraTexture::Deactivate()
{
	this->DeleteUnmanagedObjects();
	this->m_bIsActive = false;
}

void FvFloraTexture::CreateUnmanagedObjects()
{
	if ( !this->m_bIsActive )
		return;

	for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
	{
		FV_ASSERT(m_spTextures[iTex].isNull());
		char acTexName[16];
		FvSNPrintf(acTexName,sizeof(acTexName),"FloraTexture_%d",iTex);
		FV_ASSERT(!Ogre::TextureManager::getSingleton().resourceExists(acTexName));
		m_spTextures[iTex] = Ogre::TextureManager::getSingleton().createManual(acTexName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,
			m_iWidth,m_iHeight,
			0,
			m_kSmallMap.NumMipMaps() - 1,
			FV_FLORA_TEXTURE_OGRE_FORMAT[iTex],
			Ogre::TU_DYNAMIC);

		if(!m_spTextures[iTex].isNull())
		{
			IDirect3DTexture9 *pkTexture = NULL;
			IDirect3DDevice9 *pkDevice = static_cast<Ogre::D3D9RenderSystem*>(
				Ogre::Root::getSingleton().getRenderSystem())->getDevice();
			HRESULT hRet = pkDevice->CreateTexture(m_iWidth,m_iHeight,
				m_kSmallMap.NumMipMaps(),0,FV_FLORA_TEXTURE_D3D_FORMAT[iTex], D3DPOOL_SYSTEMMEM,&pkTexture,NULL);

			if( SUCCEEDED( hRet ) )
			{
				FvUInt32 uiNumLevels = pkTexture->GetLevelCount();
				for (FvUInt32 i = 0; i < uiNumLevels; i++)
				{
					D3DSURFACE_DESC sd;
					pkTexture->GetLevelDesc( i, &sd );
					D3DLOCKED_RECT lr;
					pkTexture->LockRect( i, &lr, NULL, 0 );
					if(FV_FLORA_TEXTURE_D3D_FORMAT[iTex] == D3DFMT_DXT3)
						ZeroMemory( lr.pBits, sd.Width * sd.Height );		
					else
						ZeroMemory( lr.pBits, sd.Width * sd.Height * 0.5f );	
					*(unsigned char*)lr.pBits = 0xff;
					pkTexture->UnlockRect( i );
				}
				pkDevice->UpdateTexture( pkTexture, static_cast<Ogre::D3D9Texture*>(m_spTextures[iTex].get())->getTexture() );
			}

			if(pkTexture)
				pkTexture->Release();
		}
	}

	for ( int i=0; i<FV_FLORA_MAX_ECOTYPES; i++ )
	{
		if (m_akBlocks[i].m_kTexNames[0] != "")
		{
			std::vector<FvD3DSystemMemoryTexturePtr> spTextures;
			spTextures.resize(FV_FLORA_TEXTURE_NUMBER,NULL);
			for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
				spTextures[iTex] = new FvD3DSystemMemoryTexture(m_akBlocks[i].m_kTexNames[iTex]);
			this->SwapInBlock(i,spTextures);
		}
	}
}

void FvFloraTexture::DeleteUnmanagedObjects()
{
	for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
	{
		if(m_spTextures[iTex].isNull()) continue;
		Ogre::TextureManager::getSingleton().remove(m_spTextures[iTex]->getHandle());
		m_spTextures[iTex].setNull();
	}
}

FvVector2 &FvFloraTexture::Allocate( FvUInt8 uiID, std::vector<FvD3DSystemMemoryTexturePtr> spTextures )
{
	int iActualID = (int)uiID;
	for ( int i = 0; i < FV_FLORA_MAX_ECOTYPES; i++ )
	{
		if ( m_akBlocks[i].m_iEcotype == -(iActualID+1) )	
		{
			m_akBlocks[i].m_iEcotype = iActualID;			
			return m_akBlocks[i].m_kOffset;
		}
	}

	for ( int i = 0; i < FV_FLORA_MAX_ECOTYPES; i++ )
	{
		if ( m_akBlocks[i].m_iEcotype < 0 )
		{
			m_akBlocks[i].m_iEcotype = iActualID;
			for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
				m_akBlocks[i].m_kTexNames[iTex] = (spTextures[iTex]!=NULL)?spTextures[iTex]->ResourceID():"";
			this->SwapInBlock( i, spTextures );
			return m_akBlocks[i].m_kOffset;
		}
	}

	FV_ERROR_MSG( "FvFloraTexture::AllocateTextureBlock - ERROR - all blocks used!!!\n" );
	return m_akBlocks[0].m_kOffset;
}


void FvFloraTexture::Deallocate( FvUInt8 uiID )
{
	int iActualID = (int)uiID;

	for ( int i = 0; i < FV_FLORA_MAX_ECOTYPES; i++ )
	{
		if (m_akBlocks[i].m_iEcotype == iActualID)
		{
			m_akBlocks[i].m_iEcotype = -(m_akBlocks[i].m_iEcotype+1);
			return;
		}
	}
}

void FvFloraTexture::SwapInBlock( int iIndex, std::vector<FvD3DSystemMemoryTexturePtr> spSourceTexture )
{
	FV_ASSERT(spSourceTexture.size() == FV_FLORA_TEXTURE_NUMBER);

	if ( m_spTextures[0].isNull() )
	{
		FV_WARNING_MSG( "FvFloraTexture::SwapInBlock: m_spTexture is NULL.\n" );
		return;
	}

	Block &kBlock = m_akBlocks[iIndex];

	if ( !spSourceTexture[0] )
	{
		FV_CRITICAL_MSG( "FvFloraTexture::SwapInBlock - empty texture passed in \n" );
		return;
	}

	for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
	{
		if(spSourceTexture[iTex] == NULL)
			continue;

		IDirect3DBaseTexture9 *pkBaseTex = spSourceTexture[iTex]->pTexture();
		IDirect3DTexture9 *pkTexture;
		if( pkBaseTex->GetType() == D3DRTYPE_TEXTURE )
		{
			pkTexture = (IDirect3DTexture9*)pkBaseTex;
			pkTexture->AddRef();
		}
		else
		{
			FV_CRITICAL_MSG( "FvFloraTexture::SwapInBlock - %s is not a texture\n", spSourceTexture[iTex]->ResourceID().c_str() );
			return;
		}

		int iMipOffset = 0;
		FvCompressedMipMapCalculator *pkSmallMap = &m_kSmallMap;
		if ( spSourceTexture[iTex]->Width() != m_kSmallMap.Width() )
		{
			FV_ERROR_MSG( "FloraTexture::swapInBlock tried to swap in a flora texture of invalid size %s\n",
				spSourceTexture[iTex]->ResourceID().c_str() );
		}

		int aiMipLevel[2];
		int iNumLevels = pkSmallMap->NumMipMaps() - iMipOffset;
		aiMipLevel[0] = 0;
		aiMipLevel[1] = iMipOffset;

		RECT kSrcRect;
		kSrcRect.left = 0;
		kSrcRect.right = m_kSmallMap.Width();
		kSrcRect.top = 0;
		kSrcRect.bottom = m_kSmallMap.Height();

		POINT kDestPt;
		kDestPt.x = m_akBlocks[iIndex].m_iPixelOffsetX;
		kDestPt.y = m_akBlocks[iIndex].m_iPixelOffsetY;

		while ( iNumLevels )
		{
			IDirect3DSurface9 *pkDestSurface;
			HRESULT hRet = static_cast<Ogre::D3D9Texture*>(m_spTextures[iTex].get())->getNormTexture()->GetSurfaceLevel( aiMipLevel[0], &pkDestSurface );
			if ( FAILED( hRet ) )
			{
				FV_DEBUG_MSG( "FvFloraTexture::SwapInBlock First GetSurfaceLevel failed for surface %d of texture %s ( error %s )\n",
					aiMipLevel[0], spSourceTexture[iTex]->ResourceID().c_str(), D3D9ErrorAsString(hRet).c_str() );
			}

			IDirect3DSurface9 *pkSrcSurface;
			hRet = pkTexture->GetSurfaceLevel( aiMipLevel[1], &pkSrcSurface );
			if ( FAILED( hRet ) )
			{
				FV_DEBUG_MSG( "FvFloraTexture::SwapInBlock Second GetSurfaceLevel failed for surface %d of texture %s ( error %s )\n",
					aiMipLevel[0], spSourceTexture[iTex]->ResourceID().c_str(), D3D9ErrorAsString(hRet).c_str() );
			}

			hRet = static_cast<Ogre::D3D9RenderSystem*>(Ogre::Root::getSingleton().getRenderSystem())->getDevice()
				->UpdateSurface( pkSrcSurface, &kSrcRect, pkDestSurface, &kDestPt );
			if( FAILED( hRet ) )
			{
				FV_DEBUG_MSG( "FvFloraTexture::SwapInBlock UpdateSurface failed for surface %d of texture %s ( error %s )\n",
					aiMipLevel[0], spSourceTexture[iTex]->ResourceID().c_str(), D3D9ErrorAsString(hRet).c_str() );
			}

			kDestPt.x/=2;	
			kDestPt.y/=2;	

			kSrcRect.right = max( (kSrcRect.right/2), 1L );
			kSrcRect.bottom = max( (kSrcRect.bottom/2), 1L );

			aiMipLevel[0]++;
			aiMipLevel[1]++;
			iNumLevels--;

			pkDestSurface->Release();
			pkSrcSurface->Release();
		}

		pkTexture->Release();
	}

	if ( s_bSave )
	{
		char acBuf[256];
		for(int iTex = 0; iTex < FV_FLORA_TEXTURE_NUMBER; iTex++)
		{
			FvSNPrintf( acBuf, sizeof(acBuf), "C:\\FloraTex_%d_%d.dds",iTex,s_iShotID++ );
			D3DXSaveTextureToFile( acBuf, D3DXIFF_DDS, 
				static_cast<Ogre::D3D9Texture*>(m_spTextures[iTex].get())->getTexture(), NULL );

			FvSNPrintf( acBuf, sizeof(acBuf), "C:\\FloraTexSource_%d_%d.dds",iTex,s_iShotID++ );
			D3DXSaveTextureToFile( acBuf, D3DXIFF_DDS, spSourceTexture[iTex]->pTexture(), NULL );
		}
		s_bSave = false;
	}
}