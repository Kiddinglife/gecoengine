//{future header message}
#ifndef __FvTerrainPageSerializerImpl_H__
#define __FvTerrainPageSerializerImpl_H__

#include "FvTerrain.h"

#include <FvBaseTypes.h>

#include <OgreDataStream.h>

class FvTerrainPage;

class FV_TERRAIN_API FvTerrainPageSerializerImpl
{
public:
	virtual ~FvTerrainPageSerializerImpl(){}

	virtual bool ExportTerrainPage(const FvTerrainPage *pkTerrainPage,const FvString& kFileName) = 0;
	virtual bool ImportTerrainPage(Ogre::DataStreamPtr &kStream, FvTerrainPage *pkDest) = 0;
};

class FvFTDTerrainPageSerializerImpl : public FvTerrainPageSerializerImpl
{
public:

	static const FvUInt32 COMPRESSED_MAGIC1		= 0x7a697000;	// "zip\0"
	static const FvUInt32 COMPRESSED_MAGIC2		= 0x42af9021;	// random number
	static const FvUInt32 COMPRESSED_HEADER_SZ 	= 3;			// 

	enum FTDTerrainChunkID
	{
		ID_NONE					= 0x0000,
		ID_NORMALS				= 0x0001,
		ID_LOD_TEXTURE			= 0x0002,
		ID_LOD_NORMALS			= 0x0003,
		ID_HORIZON_SHADOWS		= 0x0004,
		ID_HEIGHTS				= 0x0005,
		ID_HEIGHTS_1			= 0x0006,
		ID_HEIGHTS_2			= 0x0007,
		ID_HEIGHTS_3			= 0x0008,
		ID_HEIGHTS_4			= 0x0009,
		ID_HEIGHTS_5			= 0x000A,
		ID_HEIGHTS_6			= 0x000B,
		ID_DOMINANT_TEXUTRES	= 0x000C,
		ID_HOLE_MAP				= 0x000D,
		ID_THUMBNAIL			= 0x000E,
		ID_LAYER_BEGIN			= 0x000F,
		ID_MAX
	};

	enum FTDVersions
	{
		VERSION_PNG     	= 1,
		VERSION_DDS			= 2
	};

	struct FTDTerrainHeader
	{
		FvUInt32 m_uiIdentify;
		FvUInt16 m_uiVersion;
		static const FvUInt32 IDENTIFY = 0x00445446; // "FTD\0"
	};

	struct FTDChunkHeader
	{
		FvUInt16 m_uiID;
		FvUInt32 m_uiSize;
	};

	struct FTDHeightMapHeader
	{
		FvUInt32 m_uiVersion;	
		FvUInt32 m_uiWidth;
		FvUInt32 m_uiHeight;
		float m_fMinHeight;
		float m_fMaxHeight;
	};

	struct FTDHolesHeader
	{
		FvUInt32 m_uiMagic;
		FvUInt32 m_uiWidth;
		FvUInt32 m_uiHeight;
		FvUInt32 m_uiVersion;
		// static const FvUInt32 MAGIC = '\0loh';
		static const FvUInt32 MAGIC = 0x006c6f68;
	};

	struct FTDShadowHeader
	{
		FvUInt32 m_uiMagic; 
		FvUInt32 m_uiWidth;
		FvUInt32 m_uiHeight;
		FvUInt32 m_uiBpp;
		FvUInt32 m_uiVersion;
		FvUInt32 m_auiPad[3];
		// static const FvUInt32 MAGIC = '\0dhs';
		static const FvUInt32 MAGIC = 0x00646873;
	};

	struct FTDNormalMapHeader
	{
		static const FvUInt32 VERSION_16_BIT_PNG = 1;
		FvUInt32 m_uiVersion;
	};

	struct FTDLayerMapHeader
	{
		FvUInt32 m_uiVersion;
		FvUInt32 m_uiWidth;
		FvUInt32 m_uiHeight;	
		float m_kUProjection[4];
		float m_kVProjection[4];
	};

	struct FTDDominantTextureMapHeader
	{
		static const FvUInt32 VERSION_ZIP = 1;
		FvUInt32 m_uiMagic;
		FvUInt32 m_uiVersion;
		FvUInt32 m_uiNumTextures;
		FvUInt32 m_uiTextureNameSize;
		FvUInt32 m_uiWidth;
		FvUInt32 m_uiHeight;
		FvUInt32 m_auiPad[2];
		// static const FvUInt32 MAGIC = '\0tam';
		static const FvUInt32 MAGIC = 0x0074616d;
	};

	FvFTDTerrainPageSerializerImpl();
	~FvFTDTerrainPageSerializerImpl();

	virtual bool ExportTerrainPage(const FvTerrainPage *pkTerrainPage, const FvString& kFileName);
	virtual bool ImportTerrainPage(Ogre::DataStreamPtr &kStream, FvTerrainPage *pkDest);

protected:

	void ReadHeightMap(Ogre::DataStreamPtr &spStream, FvTerrainPage *pkDest, FvUInt32 uiLodLevel);
	void ReadDominantTextureMap(char *pcData, size_t stSize, FvTerrainPage *pkDest);
	void ReadHoleMap(char *pcData, size_t stSize, FvTerrainPage *pkDest);
#ifndef FV_SERVER
	void ReadNormalMap(Ogre::DataStreamPtr &spStream, FvTerrainPage *pkDest);	
	void ReadLodTextureMap(Ogre::DataStreamPtr &spStream, FvTerrainPage *pkDest);
	void ReadLodNormalMap(Ogre::DataStreamPtr &spStream, FvTerrainPage *pkDest);
	void ReadHorizonShadowMap(char *pcData, size_t stSize, FvTerrainPage *pkDest);
	void ReadLayerMap(Ogre::DataStreamPtr &spStream, FvTerrainPage *pkDest, FvUInt32 uiLayerIndex);
#endif // !FV_SERVER
};

#endif // __FvTerrainPageSerializerImpl_H__