#include "FvTerrainPageSerializerImpl.h"
#include "FvTerrainPage.h"

#include <FvDebug.h>

#ifndef FV_SERVER
#include <OgreImage.h>
#include <OgreImageCodec.h>
#include <OgreTexture.h>
#include <OgrePixelFormat.h>
#include <OgreTextureManager.h>
#include <OgreHardwarePixelBuffer.h>
#endif // !FV_SERVER

#include "zlib.h"

FvFTDTerrainPageSerializerImpl::FvFTDTerrainPageSerializerImpl()
{

}

FvFTDTerrainPageSerializerImpl::~FvFTDTerrainPageSerializerImpl()
{

}

bool FvFTDTerrainPageSerializerImpl::ExportTerrainPage(
	const FvTerrainPage *pkTerrainPage,const FvString& kFileName)
{

	return true;
}

bool FvFTDTerrainPageSerializerImpl::ImportTerrainPage(
	Ogre::DataStreamPtr &kStream, FvTerrainPage *pkDest)
{
	if(kStream.isNull() || kStream->eof())
		return false;

	FTDTerrainHeader kFTDHeader;
	kStream->read(&kFTDHeader,sizeof(FTDTerrainHeader));
	if(kFTDHeader.m_uiIdentify != FTDTerrainHeader::IDENTIFY)
	{
		return false;
	}

	FTDChunkHeader kChunkHeader;
	char *pcTempData = new char[409600];
	while(!kStream->eof())
	{
		if(kStream->read(&kChunkHeader,sizeof(FTDChunkHeader))
			!= sizeof(FTDChunkHeader))
			continue;

		FV_ASSERT(kChunkHeader.m_uiSize < 409600);
		if(kStream->read(pcTempData,kChunkHeader.m_uiSize)
			!= kChunkHeader.m_uiSize)
			continue;

		Ogre::DataStreamPtr spData(OGRE_NEW Ogre::MemoryDataStream(pcTempData,
			kChunkHeader.m_uiSize));
		switch(kChunkHeader.m_uiID)
		{
		case ID_HEIGHTS:
		case ID_HEIGHTS_1:
		case ID_HEIGHTS_2:
		case ID_HEIGHTS_3:
		case ID_HEIGHTS_4:
		case ID_HEIGHTS_5:
		case ID_HEIGHTS_6:
			{
				FvUInt32 uiLodLevel = kChunkHeader.m_uiID - ID_HEIGHTS;
				if(uiLodLevel == pkDest->m_spTerrainSettings->DefaultHeightMapLod())
					this->ReadHeightMap(spData,pkDest,uiLodLevel);
			}
			break;
		case ID_HOLE_MAP:
			this->ReadHoleMap(pcTempData,kChunkHeader.m_uiSize,pkDest);
			break;
		case ID_DOMINANT_TEXUTRES:
			this->ReadDominantTextureMap(pcTempData,kChunkHeader.m_uiSize,pkDest);
			break;
#ifndef FV_SERVER
		case ID_NORMALS:
			this->ReadNormalMap(spData,pkDest);
			break;
		case ID_LOD_TEXTURE:
			this->ReadLodTextureMap(spData,pkDest);
			break;
		case ID_LOD_NORMALS:
			this->ReadLodNormalMap(spData,pkDest);
			break;
		case ID_HORIZON_SHADOWS:
			this->ReadHorizonShadowMap(pcTempData,kChunkHeader.m_uiSize,pkDest);
			break;
		case ID_THUMBNAIL:
			break;
		default:
			if(kChunkHeader.m_uiID >= ID_LAYER_BEGIN)
			{
				this->ReadLayerMap(spData,pkDest,kChunkHeader.m_uiID - ID_LAYER_BEGIN);
			}
			break;
#endif // !FV_SERVER
		}
	}
	delete[] pcTempData;

	if(pkDest->m_kMaterialNames.size() == 0)
		pkDest->m_eOceanType = FvTerrainPage::OCEAN_TYPE_ONLY;

	for(size_t i = 0; i < pkDest->m_kMaterialNames.size(); i++)
	{
		FV_ASSERT(pkDest->m_spTerrainSettings);
		FV_ASSERT(!pkDest->m_kMaterialNames[i].empty());
		if(pkDest->m_spTerrainSettings->IsOceanMap(
			pkDest->m_kMaterialNames[i]))
		{
			pkDest->m_eOceanType = FvTerrainPage::OCEAN_TYPE_LAND;
			break;
		}
	}

	if(pkDest->m_kHeights.getData() == NULL)
		return false;

#ifndef FV_SERVER
	pkDest->GenerateCombinedLayers();
	pkDest->GenerateMaterial();
#endif // !FV_SERVER

	return true;
}

void FvFTDTerrainPageSerializerImpl::ReadHeightMap(Ogre::DataStreamPtr &spStream, 
												   FvTerrainPage *pkDest, FvUInt32 uiLodLevel)
{
	FV_ASSERT(pkDest);
	FTDHeightMapHeader kHeader;
	spStream->read(&kHeader,sizeof(FTDHeightMapHeader));

	pkDest->m_fMinHeight = kHeader.m_fMinHeight;
	pkDest->m_fMaxHeight = kHeader.m_fMaxHeight;
	pkDest->m_uiHeightLodLevel = uiLodLevel;
	pkDest->m_uiVisiableOffset = pkDest->m_uiHeightLodLevel > 0 ? 0 : FV_DEFAULT_VISIBLE_OFFSET;
	if (pkDest->m_fMaxHeight < pkDest->m_fMinHeight)
		std::swap(pkDest->m_fMinHeight, pkDest->m_fMaxHeight);

	if(kHeader.m_uiVersion == VERSION_PNG)
		pkDest->m_kHeights.load(spStream,"png");
	else if(kHeader.m_uiVersion == VERSION_DDS)
		pkDest->m_kHeights.load(spStream,"dds");

	FV_ASSERT(pkDest->m_kHeights.getWidth() == kHeader.m_uiWidth);
	FV_ASSERT(pkDest->m_kHeights.getHeight() == kHeader.m_uiHeight);
}

void FvFTDTerrainPageSerializerImpl::ReadDominantTextureMap(char *pcData, size_t stSize, 
															FvTerrainPage *pkDest)
{
	FTDDominantTextureMapHeader *pkHeader = (FTDDominantTextureMapHeader*)pcData;

	if(stSize <= sizeof(FTDDominantTextureMapHeader))
	{
		FV_ERROR_MSG("FvFTDTerrainPageSerializerImpl:ReadDominantTextureMap header size error");
		return;
	}

	if ( pkHeader->m_uiMagic != FTDDominantTextureMapHeader::MAGIC ||
		pkHeader->m_uiVersion != FTDDominantTextureMapHeader::VERSION_ZIP )
	{
		FV_ERROR_MSG("FvFTDTerrainPageSerializerImpl::ReadDominantTextureMap magic or version error");
		return;
	}

	FvUInt32 uiTextureDataSize = pkHeader->m_uiTextureNameSize * pkHeader->m_uiNumTextures;
	stSize -= sizeof(FTDDominantTextureMapHeader);
	if(stSize < uiTextureDataSize)
	{
		FV_ERROR_MSG("FvFTDTerrainPageSerializerImpl::ReadDominantTextureMap texture data size error");
		return;
	}

	char *pcNameBuf = new char[pkHeader->m_uiTextureNameSize + 1];
	char *pcPtr = (char*)( pkHeader + 1 );
	for( FvUInt32 i = 0; i < pkHeader->m_uiNumTextures; ++i )
	{
		strncpy( pcNameBuf, pcPtr, pkHeader->m_uiTextureNameSize);
		pcNameBuf[ std::min( FvUInt32(strlen(pcPtr)), pkHeader->m_uiTextureNameSize ) ] = '\0';
		FvString kTexName = pcNameBuf;
		Ogre::StringUtil::toLowerCase(kTexName);
		pkDest->m_kMaterialNames.push_back( kTexName );
		pcPtr += pkHeader->m_uiTextureNameSize;
	}
	delete[] pcNameBuf;

	stSize -= uiTextureDataSize;
	if(stSize <= int( COMPRESSED_HEADER_SZ * sizeof( FvUInt32 ) ))
	{
		FV_ERROR_MSG("FvFTDTerrainPageSerializerImpl::ReadDominantTextureMap image data size error");
		return;
	}

	if(*((FvUInt32*)pcPtr) != COMPRESSED_MAGIC1 ||
		*((FvUInt32*)pcPtr + 1) != COMPRESSED_MAGIC2)
	{
		FV_ERROR_MSG("FvFTDTerrainPageSerializerImpl::ReadDominantTextureMap image data format error");
		return;
	}

	uLongf uiUncompressSize = *((FvUInt32 *)pcPtr + COMPRESSED_HEADER_SZ - 1);
	if(uiUncompressSize != pkHeader->m_uiWidth * pkHeader->m_uiHeight)
	{
		FV_ERROR_MSG("FvFTDTerrainPageSerializerImpl::ReadDominantTextureMap width or height error");
		return;
	}

	pkDest->m_pkMaterialIndexs = new FvUInt8[uiUncompressSize];
	memset(pkDest->m_pkMaterialIndexs,0xff,uiUncompressSize);
	FvUInt8 *pcRawData = (FvUInt8 *)pcPtr + COMPRESSED_HEADER_SZ*sizeof(FvUInt32);
	FvUInt32 uiRawDataSize = FvUInt32(stSize) - COMPRESSED_HEADER_SZ*sizeof(FvUInt32);
	int iResult =
		uncompress
		(
		pkDest->m_pkMaterialIndexs,
		&uiUncompressSize,
		pcRawData,
		uiRawDataSize
		);
	FV_ASSERT(iResult == Z_OK);
	pkDest->m_uiMaterialWidth = pkHeader->m_uiWidth;
	pkDest->m_uiMaterialHeight = pkHeader->m_uiHeight;
}

void FvFTDTerrainPageSerializerImpl::ReadHoleMap(char *pcData, size_t stSize,
												 FvTerrainPage *pkDest)
{
	if(*((FvUInt32*)pcData) == COMPRESSED_MAGIC1 &&
		*((FvUInt32*)pcData + 1) == COMPRESSED_MAGIC2)
	{
		uLongf uiUncompressSize = *((FvUInt32 *)pcData + COMPRESSED_HEADER_SZ - 1);
		FvUInt8 *pcUncompressBuffer = new FvUInt8[uiUncompressSize];	
		FvUInt8 *pcRawData = (FvUInt8 *)pcData + COMPRESSED_HEADER_SZ*sizeof(FvUInt32);
		FvUInt32 uiRawDataSize = FvUInt32(stSize) - COMPRESSED_HEADER_SZ*sizeof(FvUInt32);
		int iResult =
			uncompress
			(
			pcUncompressBuffer,
			&uiUncompressSize,
			pcRawData,
			uiRawDataSize
			);
		FV_ASSERT(iResult == Z_OK);

		FTDHolesHeader *pkHeader = (FTDHolesHeader*)pcUncompressBuffer;
		if(pkHeader->m_uiMagic == FTDHolesHeader::MAGIC)
		{
			pkDest->GenerateHolesMap((FvUInt8*)(pkHeader + 1),
				pkHeader->m_uiWidth, pkHeader->m_uiHeight);
		}

		delete[] pcUncompressBuffer;
	}
}

#ifndef FV_SERVER
void FvFTDTerrainPageSerializerImpl::ReadNormalMap(Ogre::DataStreamPtr &spStream, 
												   FvTerrainPage *pkDest)
{
	FTDNormalMapHeader kHeader;
	spStream->read(&kHeader,sizeof(FTDNormalMapHeader));

	if(kHeader.m_uiVersion == VERSION_PNG)
	{
		Ogre::Codec *pkCodec = 0;
		pkCodec = Ogre::Codec::getCodec("png");

		if(pkCodec == NULL)
			return;

		Ogre::Codec::DecodeResult kRes = pkCodec->decode(spStream);
		Ogre::ImageCodec::ImageData *pkData = 
			static_cast<Ogre::ImageCodec::ImageData*>(kRes.second.getPointer());
		pkDest->m_spNormalMap = Ogre::TextureManager::getSingleton().loadRawData(pkDest->m_kNormalName,
			FvTerrainPage::TERRAIN_INERNAL_RESOURCE_GROUP, Ogre::DataStreamPtr(kRes.first),
			Ogre::ushort(pkData->width), Ogre::ushort(pkData->height), Ogre::PF_BYTE_LA);
	}
}

void FvFTDTerrainPageSerializerImpl::ReadLodTextureMap(Ogre::DataStreamPtr &spStream, 
													   FvTerrainPage *pkDest)
{
	Ogre::Image kImage;
	kImage.load(spStream,"dds");
	pkDest->m_spLodMap = Ogre::TextureManager::getSingleton().loadImage(pkDest->m_kLodName,
		FvTerrainPage::TERRAIN_INERNAL_RESOURCE_GROUP, kImage);
}

void FvFTDTerrainPageSerializerImpl::ReadLodNormalMap(Ogre::DataStreamPtr &spStream, 
													  FvTerrainPage *pkDest)
{
	FTDNormalMapHeader kHeader;
	spStream->read(&kHeader,sizeof(FTDNormalMapHeader));

	if(kHeader.m_uiVersion == VERSION_PNG)
	{
		Ogre::Codec *pkCodec = 0;
		pkCodec = Ogre::Codec::getCodec("png");

		if(pkCodec == NULL)
			return;

		Ogre::Codec::DecodeResult kRes = pkCodec->decode(spStream);
		Ogre::ImageCodec::ImageData *pkData = 
			static_cast<Ogre::ImageCodec::ImageData*>(kRes.second.getPointer());
		pkDest->m_spLodNormalMap = Ogre::TextureManager::getSingleton().loadRawData(pkDest->m_kLodNormalName,
			FvTerrainPage::TERRAIN_INERNAL_RESOURCE_GROUP, Ogre::DataStreamPtr(kRes.first),
			Ogre::ushort(pkData->width), Ogre::ushort(pkData->height), Ogre::PF_BYTE_LA);
	}
}

void FvFTDTerrainPageSerializerImpl::ReadHorizonShadowMap(char *pcData, size_t stSize,
														  FvTerrainPage *pkDest)
{
	if(*((FvUInt32*)pcData) == COMPRESSED_MAGIC1 &&
		*((FvUInt32*)pcData + 1) == COMPRESSED_MAGIC2)
	{
		uLongf uiUncompressSize = *((FvUInt32 *)pcData + COMPRESSED_HEADER_SZ - 1);
		FvUInt8 *pcUncompressBuffer = new FvUInt8[uiUncompressSize];	
		FvUInt8 *pcRawData = (FvUInt8 *)pcData + COMPRESSED_HEADER_SZ*sizeof(FvUInt32);
		FvUInt32 uiRawDataSize = FvUInt32(stSize) - COMPRESSED_HEADER_SZ*sizeof(FvUInt32);
		int iResult =
			uncompress
			(
			pcUncompressBuffer,
			&uiUncompressSize,
			pcRawData,
			uiRawDataSize
			);
		FV_ASSERT(iResult == Z_OK);

		FTDShadowHeader *pkHeader = (FTDShadowHeader*)pcUncompressBuffer;
		if(pkHeader->m_uiMagic == FTDShadowHeader::MAGIC)
		{
			Ogre::DataStreamPtr spStreamData(OGRE_NEW Ogre::MemoryDataStream((pkHeader + 1),
				uiUncompressSize - sizeof(FTDShadowHeader)));
			pkDest->m_spHorizonShadowMap = Ogre::TextureManager::getSingleton().loadRawData(pkDest->m_kHorizonShadowName,
				FvTerrainPage::TERRAIN_INERNAL_RESOURCE_GROUP, spStreamData,
				Ogre::ushort(pkHeader->m_uiWidth), Ogre::ushort(pkHeader->m_uiHeight), Ogre::PF_SHORT_GR);
		}

		delete[] pcUncompressBuffer;
	}
}

void FvFTDTerrainPageSerializerImpl::ReadLayerMap(Ogre::DataStreamPtr &spStream, 
												  FvTerrainPage *pkDest, FvUInt32 uiLayerIndex)
{
	FTDLayerMapHeader kHeader;
	spStream->read(&kHeader,sizeof(FTDLayerMapHeader));
	FvTerrainPage::TextureLayerPtr spLayer = new FvTerrainPage::TextureLayer;

	FvUInt32 uiNameSize;
	spStream->read(&uiNameSize,sizeof(FvUInt32));

	char acTextureName[256];
	FV_ASSERT(uiNameSize < 256);
	spStream->read(acTextureName,uiNameSize);
	acTextureName[uiNameSize] = 0;
	spLayer->m_kTextureName = acTextureName;

	if(kHeader.m_uiVersion == VERSION_PNG)
	{
		spLayer->m_pkBlends = new Ogre::Image;
		spLayer->m_pkBlends->load(spStream,"png");
	}
	if(kHeader.m_uiVersion == VERSION_DDS)
	{
		spLayer->m_pkBlends = new Ogre::Image;
		spLayer->m_pkBlends->load(spStream,"dds");
	}

	FV_ASSERT(spLayer->m_pkBlends);

	try
	{
		spLayer->m_spTexture = Ogre::TextureManager::getSingleton().load(spLayer->m_kTextureName,
			pkDest->m_spTerrainSettings->TextureGroup());
	}
	catch (...){}

	if(spLayer->m_spTexture.isNull())
	{
		FV_ERROR_MSG("FvFTDTerrainPageSerializerImpl::ReadLayerMap texture %s not exist",
			spLayer->m_kTextureName.c_str());
	}

	spLayer->m_uiWidth = kHeader.m_uiWidth;
	spLayer->m_uiHeight = kHeader.m_uiHeight;
	memcpy(&spLayer->m_kUProjection, kHeader.m_kUProjection, sizeof(Ogre::Vector4)); 
	memcpy(&spLayer->m_kVProjection, kHeader.m_kVProjection, sizeof(Ogre::Vector4)); 

	pkDest->m_kTextureLayers.push_back(spLayer);
}
#endif // !FV_SERVER