//{future header message}
#ifndef __FvFloraTexture_H__
#define __FvFloraTexture_H__

#include <FvPowerDefines.h>
#include <FvXMLSection.h>
#include <FvDebug.h>

#include <OgreTexture.h>
#include <RenderSystems/Direct3D9/include/OgreD3D9Texture.h>
#include <RenderSystems/Direct3D9/include/OgreD3D9RenderSystem.h>

#include <vector>

#include "FvZoneDefine.h"
#include "FvFloraConstants.h"

class FvCompressedMipMapCalculator
{
public:
	struct MipData
	{
		FvUInt32 m_uiOffset;
		FvUInt32 m_uiNumBytesPerRow;
		FvUInt32 m_uiNumRows;
		FvUInt32 m_uiNumBytes;
	};

	struct DDSData
	{
		FvUInt32 m_uiNumBytes;
		FvUInt32 m_uiNumMipMaps;
	};

	FvCompressedMipMapCalculator( bool bIncludeHeader );
	void Dimensions( int iWidth, int iHeight );
	int Width()	{ return m_iWidth; }
	int Height() { return m_iHeight; }
	void MipMap( int iMipmapLevel, MipData &kRet );
	FvUInt32 DDSSize();
	FvUInt32 DataSize();
	FvUInt32 NumMipMaps();
private:
	bool m_bIncludeHeader;
	int m_iWidth;
	int m_iHeight;
	DDSData m_kData;
	std::vector<MipData> m_kMipMaps;
};

class FvD3DSystemMemoryTexture : public FvSafeReferenceCount
{
public:
	FvD3DSystemMemoryTexture(
		const FvString &kResourceID, 
		D3DFORMAT eFormat = D3DFMT_UNKNOWN);
	~FvD3DSystemMemoryTexture();

	IDirect3DBaseTexture9 *pTexture() { return m_pkTexture; }
	FvUInt32 Width( ) const { return m_uiWidth; }
	FvUInt32 Height( ) const { return m_uiHeight; }
	D3DFORMAT Format( ) const { return m_eFormat; }
	const FvString &ResourceID( ) const { return m_kResourceID; }
	bool FailedToLoad() { return m_bFailedToLoad; }

private:

	IDirect3DTexture9 *m_pkTexture;
	FvUInt32 m_uiWidth;
	FvUInt32 m_uiHeight;
	D3DFORMAT m_eFormat;
	FvString m_kResourceID;
	bool m_bFailedToLoad;
};

typedef FvSmartPointer<FvD3DSystemMemoryTexture> FvD3DSystemMemoryTexturePtr;

class FV_ZONE_API FvFloraTexture
{
public:
	FvFloraTexture();
	~FvFloraTexture();

	bool Init( FvXMLSectionPtr spSection );

	void Activate();
	void Deactivate();

	void CreateUnmanagedObjects();
	void DeleteUnmanagedObjects();	

	FvVector2 &Allocate( FvUInt8 uiEcotypeID, std::vector<FvD3DSystemMemoryTexturePtr> spTextures );
	void Deallocate( FvUInt8 uiEcotypeID );

	float Width() const	{ return (float)m_iWidth; }
	int BlocksWide() const	{ return m_iNumBlocksWide; }
	int BlocksHigh() const	{ return m_iNumBlocksHigh; }

	Ogre::TexturePtr &pTexture(int iIndex) {FV_ASSERT(iIndex < FV_FLORA_TEXTURE_NUMBER); return m_spTextures[iIndex]; }

private:

	int m_iWidthPerBlock;
	int m_iHeightPerBlock;
	int m_iWidth;
	int m_iHeight;
	int m_iNumBlocksWide;
	int m_iNumBlocksHigh;

	struct Block
	{
		FvVector2 m_kOffset;
		int m_iPixelOffsetX;
		int m_iPixelOffsetY;
		int m_iEcotype;
		std::vector<FvString> m_kTexNames;
	};

	Block m_akBlocks[FV_FLORA_MAX_ECOTYPES];

	void SwapInBlock( int iIndex, std::vector<FvD3DSystemMemoryTexturePtr> spTextures );	

	FvCompressedMipMapCalculator m_kLargeMap;
	FvCompressedMipMapCalculator m_kMediumMap;
	FvCompressedMipMapCalculator m_kSmallMap;

	bool m_bIsActive;

	std::vector<Ogre::TexturePtr> m_spTextures;
};

#endif // __FvFloraTexture_H__