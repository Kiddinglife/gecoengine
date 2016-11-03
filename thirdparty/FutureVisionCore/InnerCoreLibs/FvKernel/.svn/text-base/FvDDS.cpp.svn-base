//{future source message}
#include "FvDDS.h"
#include <malloc.h>
#include <string.h>
#include <stdio.h>

//---------------------------------------------------------------------------
FvDDS::FvDDS()
{
	memset(&m_kHeader, 0, sizeof(FvDDS));
}
//---------------------------------------------------------------------------
FvDDS::~FvDDS()
{
	Reset();
}
//---------------------------------------------------------------------------
void FvDDS::Reset()
{
	if(m_pvData)
		delete[] m_pvData;
 	if(m_kInfoExtra.m_kMipMap.m_ppvPointers)
		delete m_kInfoExtra.m_kMipMap.m_ppvPointers;
	if(m_kInfoExtra.m_kMipMap.m_pu32MipMapSizes)
		delete m_kInfoExtra.m_kMipMap.m_pu32MipMapSizes;
}
//---------------------------------------------------------------------------
bool FvDDS::LoadFromMem(char *pcData,size_t stSize)
{
	if(IsAvailable())
	{
		printf_s("please reset this to load\n");
		return false;
	}

	if(stSize < sizeof(FvDDSHeader))
	{
		printf_s("file size isn't right\n");
		return false;
	}

	memcpy(&m_kHeader,pcData,sizeof(FvDDSHeader));

	bool bResult = false;

	if(CheckHeader())
	{
		if(ConfirmInfo())
		{
			memcpy(m_pvData,(char*)(pcData + sizeof(FvDDSHeader)),(stSize - sizeof(FvDDSHeader)));
			bResult = true;
		}
	}

	return bResult;
}
//---------------------------------------------------------------------------
bool FvDDS::LoadFromFile(const char* pcFileName)
{
	if(IsAvailable())
	{
		printf_s("please reset this to load\n");
		return false;
	}

	FILE *kFile;
	fopen_s(&kFile, pcFileName, "rb");
	if(!kFile)
	{
		printf_s("cannot open this file\n");
		return false;
	}

	if(fread(&m_kHeader, 1, sizeof(FvDDSHeader), kFile) != sizeof(FvDDSHeader))
	{
		printf_s("cannot read this file\n");
		fclose(kFile);
		return false;
	}

	bool bResult = false;

	if(CheckHeader())
	{
		if(ConfirmInfo())
		{
			if(fread(m_pvData, 1, m_u32DataSize, kFile) != m_u32DataSize)
			{
				printf_s("file size is wrong\n");
				fclose(kFile);
				return false;
			}

			bResult = true;
		}
	}

	if(fclose(kFile) == EOF)
	{
		printf_s("cannot close this file\n");
		return false;
	}
	else
		return bResult;

}
//---------------------------------------------------------------------------
bool FvDDS::SaveToFile(const char* pcFileName)
{
	if(!IsAvailable())
	{
		printf_s("DDS is not available to save\n");
		return false;
	}

	FILE* kFile;
	fopen_s(&kFile, pcFileName, "wb");
	if(!kFile)
	{
		printf_s("cannot open this file\n");
		return false;
	}

	if(fwrite(&m_kHeader, 1, sizeof(FvDDSHeader), kFile) != sizeof(FvDDSHeader))
	{
		printf_s("write file error\n");
		fclose(kFile);
		return false;
	}

	if(fwrite(m_pvData, 1, m_u32DataSize, kFile) != m_u32DataSize)
	{
		printf_s("write file error\n");
		fclose(kFile);
		return false;
	}

	if(fclose(kFile) == EOF)
	{
		printf_s("cannot close this file\n");
		return false;
	}
	else
		return true;
}
//---------------------------------------------------------------------------
bool FvDDS::Create(FvTextureType eType, FvPixelFormat eFormat,
	FvUInt32 u32Width, FvUInt32 u32Height, FvUInt32 u32MipMapCount,
	FvUInt32 u32Depth)
{
	if(IsAvailable())
	{
		printf_s("please reset this to create\n");
		return false;
	}
	memset(&m_kHeader, 0, sizeof(FvDDS));

	m_kInfoExtra.m_eType = eType;
	m_kInfoExtra.m_u32PixelWidth = u32Width;
	m_kInfoExtra.m_u32PixelHeight = u32Height;
	m_kInfoExtra.m_u32UnitWidth = u32Width;
	m_kInfoExtra.m_u32UnitHeight = u32Height;

	m_kHeader.m_u32ImageMark = FV_DDS_MARK;
	m_kHeader.m_u32Size = FV_DDS_HEADER_SIZE;
	m_kHeader.m_u32Flags = FV_DDSD_ALWAYS;
	m_kHeader.m_u32Height = u32Height;
	m_kHeader.m_u32Width = u32Width;
	m_kHeader.m_kCaps.m_u32Caps1 = FV_DDSCAPS_TEXTURE;

	m_kHeader.m_kPixelFormat.m_u32Size = FV_DDS_PIXEL_FORMAT_SIZE;

	if(m_kInfoExtra.m_eType == FV_TYPE_3D)
	{
		m_kHeader.m_u32Flags |= FV_DDSD_DEPTH;
		m_kHeader.m_kCaps.m_u32Caps2 = FV_DDSCAPS2_VOLUME;
		m_kHeader.m_u32Depth = u32Depth;
		m_kInfoExtra.m_u32Depth = u32Depth;
	}
	else if(m_kInfoExtra.m_eType == FV_TYPE_CUBE)
	{
		m_kHeader.m_kCaps.m_u32Caps1 |= FV_DDSCAPS_COMPLEX;
		m_kHeader.m_kCaps.m_u32Caps2 = FV_DDSCAPS2_CUBEMAP_ALL;
	}

	switch(eFormat)
	{
	case FVFMT_R3G3B2:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_RGB;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 8;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x000000e0;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x0000001c;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x00000003;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 1;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 1;
		break;
	case FVFMT_A1R5G5B5:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_RGB;
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_ALPHAPIXELS;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 16;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x00007c00;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x000003e0;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x0000001f;
		m_kHeader.m_kPixelFormat.m_u32RGBAlphaBitMask = 0x00008000;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;

		m_kHeader.m_kCaps.m_u32Caps1 |= FV_DDSCAPS_ALPHA;
		break;
	case FVFMT_A4R4G4B4:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_RGB;
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_ALPHAPIXELS;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 16;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x00000f00;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x000000f0;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x0000000f;
		m_kHeader.m_kPixelFormat.m_u32RGBAlphaBitMask = 0x0000f000;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;

		m_kHeader.m_kCaps.m_u32Caps1 |= FV_DDSCAPS_ALPHA;
		break;
	case FVFMT_A8R3G3B2:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_RGB;
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_ALPHAPIXELS;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 16;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x000000e0;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x0000001c;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x00000003;
		m_kHeader.m_kPixelFormat.m_u32RGBAlphaBitMask = 0x0000ff00;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;

		m_kHeader.m_kCaps.m_u32Caps1 |= FV_DDSCAPS_ALPHA;
		break;
	case FVFMT_R5G6B5:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_RGB;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 16;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x0000f800;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x000007e0;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x0000001f;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
		break;
	case FVFMT_X1R5G5B5:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_RGB;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 16;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x00007c00;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x000003e0;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x0000001f;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
		break;
	case FVFMT_X4R4G4B4:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_RGB;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 16;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x00000f00;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x000000f0;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x0000000f;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
		break;
	case FVFMT_R8G8B8:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_RGB;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 24;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x00ff0000;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x0000ff00;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x000000ff;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 3;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 3;
		break;
	case FVFMT_A8R8G8B8:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_RGB;
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_ALPHAPIXELS;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 32;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x00ff0000;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x0000ff00;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x000000ff;
		m_kHeader.m_kPixelFormat.m_u32RGBAlphaBitMask = 0xff000000;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;

		m_kHeader.m_kCaps.m_u32Caps1 |= FV_DDSCAPS_ALPHA;
		break;
	case FVFMT_A8B8G8R8:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_RGB;
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_ALPHAPIXELS;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 32;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x000000ff;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x0000ff00;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x00ff0000;
		m_kHeader.m_kPixelFormat.m_u32RGBAlphaBitMask = 0xff000000;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;

		m_kHeader.m_kCaps.m_u32Caps1 |= FV_DDSCAPS_ALPHA;
		break;
	case FVFMT_A2R10G10B10:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_RGB;
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_ALPHAPIXELS;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 32;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x3ff00000;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x000ffc00;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x000003ff;
		m_kHeader.m_kPixelFormat.m_u32RGBAlphaBitMask = 0xc0000000;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;

		m_kHeader.m_kCaps.m_u32Caps1 |= FV_DDSCAPS_ALPHA;
		break;
	case FVFMT_A2B10G10R10:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_RGB;
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_ALPHAPIXELS;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 32;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x000003ff;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x000ffc00;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x3ff00000;
		m_kHeader.m_kPixelFormat.m_u32RGBAlphaBitMask = 0xc0000000;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;

		m_kHeader.m_kCaps.m_u32Caps1 |= FV_DDSCAPS_ALPHA;
		break;
	case FVFMT_G16R16:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_RGB;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 32;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x0000ffff;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0xffff0000;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
		break;
	case FVFMT_X8R8G8B8:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_RGB;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 32;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x00ff0000;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x0000ff00;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x000000ff;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
		break;
	case FVFMT_X8B8G8R8:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_RGB;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 32;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x000000ff;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x0000ff00c;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x00ff0000;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
		break;
	case FVFMT_A4L4:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_LUMINANCE;
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_ALPHAPIXELS;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 8;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x0000000f;
		m_kHeader.m_kPixelFormat.m_u32RGBAlphaBitMask = 0x000000f0;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 1;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 1;

		m_kHeader.m_kCaps.m_u32Caps1 |= FV_DDSCAPS_ALPHA;
		break;
	case FVFMT_L8:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_LUMINANCE;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 8;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x000000ff;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 1;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 1;
		break;
	case FVFMT_A8L8:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_LUMINANCE;
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_ALPHAPIXELS;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 16;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x000000ff;
		m_kHeader.m_kPixelFormat.m_u32RGBAlphaBitMask = 0x0000ff00;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;

		m_kHeader.m_kCaps.m_u32Caps1 |= FV_DDSCAPS_ALPHA;
		break;
	case FVFMT_L16:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_LUMINANCE;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 16;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x0000ffff;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
		break;
	case FVFMT_A8:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_ALPHA;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 8;
		m_kHeader.m_kPixelFormat.m_u32RGBAlphaBitMask = 0x000000ff;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 1;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 1;

		m_kHeader.m_kCaps.m_u32Caps1 |= FV_DDSCAPS_ALPHA;
		break;
	case FVFMT_V8U8:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_BUMPDUDV;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 16;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x000000ff;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x0000ff00;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
		break;
	case FVFMT_Q8W8V8U8:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_BUMPDUDV;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 32;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x000000ff;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x0000ff00;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x00ff0000;
		m_kHeader.m_kPixelFormat.m_u32RGBAlphaBitMask = 0xff000000;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
		break;
	case FVFMT_V16U16:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_BUMPDUDV;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 32;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x0000ffff;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0xffff0000;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
		break;
	case FVFMT_A2W10V10U10:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_BUMPDUDV;
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_ALPHAPIXELS;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 32;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x3ff00000;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x000ffc00;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x000003ff;
		m_kHeader.m_kPixelFormat.m_u32RGBAlphaBitMask = 0xc0000000;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;

		m_kHeader.m_kCaps.m_u32Caps1 |= FV_DDSCAPS_ALPHA;
		break;
	case FVFMT_L6V5U5:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_BUMPLUMINANCE;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 16;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x0000001f;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x000003e0;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x0000fc00;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
		break;
	case FVFMT_X8L8V8U8:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_BUMPLUMINANCE;
		m_kHeader.m_kPixelFormat.m_u32RGBBitCount = 32;
		m_kHeader.m_kPixelFormat.m_u32RBitMask = 0x000000ff;
		m_kHeader.m_kPixelFormat.m_u32GBitMask = 0x0000ff00;
		m_kHeader.m_kPixelFormat.m_u32BBitMask = 0x00ff0000;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
		break;
	case FVFMT_A16B16G16R16:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
		m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
		break;
	case FVFMT_Q16W16V16U16:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
		m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
		break;
	case FVFMT_DXT1:
		if(((m_kInfoExtra.m_u32PixelWidth & 0xFFFFFFFC) == m_kInfoExtra.m_u32PixelWidth)
			&& ((m_kInfoExtra.m_u32PixelHeight & 0xFFFFFFFC) == m_kInfoExtra.m_u32PixelHeight))
		{
			m_kInfoExtra.m_u32UnitWidth = m_kInfoExtra.m_u32PixelWidth >> 2;
			m_kInfoExtra.m_u32UnitHeight = m_kInfoExtra.m_u32PixelHeight >> 2;

			m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
			m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 0;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
		}
		else
		{
			memset(&m_kHeader, 0, sizeof(FvDDS));
			return false;
		}
		break;
	case FVFMT_DXT2:
		if(((m_kInfoExtra.m_u32PixelWidth & 0xFFFFFFFC) == m_kInfoExtra.m_u32PixelWidth)
			&& ((m_kInfoExtra.m_u32PixelHeight & 0xFFFFFFFC) == m_kInfoExtra.m_u32PixelHeight))
		{
			m_kInfoExtra.m_u32UnitWidth = m_kInfoExtra.m_u32PixelWidth >> 2;
			m_kInfoExtra.m_u32UnitHeight = m_kInfoExtra.m_u32PixelHeight >> 2;

			m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
			m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 1;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 16;
		}
		else
		{
			memset(&m_kHeader, 0, sizeof(FvDDS));
			return false;
		}
		break;
	case FVFMT_DXT3:
		if(((m_kInfoExtra.m_u32PixelWidth & 0xFFFFFFFC) == m_kInfoExtra.m_u32PixelWidth)
			&& ((m_kInfoExtra.m_u32PixelHeight & 0xFFFFFFFC) == m_kInfoExtra.m_u32PixelHeight))
		{
			m_kInfoExtra.m_u32UnitWidth = m_kInfoExtra.m_u32PixelWidth >> 2;
			m_kInfoExtra.m_u32UnitHeight = m_kInfoExtra.m_u32PixelHeight >> 2;

			m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
			m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 1;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 16;
		}
		else
		{
			memset(&m_kHeader, 0, sizeof(FvDDS));
			return false;
		}
		break;
	case FVFMT_DXT4:
		if(((m_kInfoExtra.m_u32PixelWidth & 0xFFFFFFFC) == m_kInfoExtra.m_u32PixelWidth)
			&& ((m_kInfoExtra.m_u32PixelHeight & 0xFFFFFFFC) == m_kInfoExtra.m_u32PixelHeight))
		{
			m_kInfoExtra.m_u32UnitWidth = m_kInfoExtra.m_u32PixelWidth >> 2;
			m_kInfoExtra.m_u32UnitHeight = m_kInfoExtra.m_u32PixelHeight >> 2;

			m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
			m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 1;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 16;
		}
		else
		{
			memset(&m_kHeader, 0, sizeof(FvDDS));
			return false;
		}
		break;
	case FVFMT_DXT5:
		if(((m_kInfoExtra.m_u32PixelWidth & 0xFFFFFFFC) == m_kInfoExtra.m_u32PixelWidth)
			&& ((m_kInfoExtra.m_u32PixelHeight & 0xFFFFFFFC) == m_kInfoExtra.m_u32PixelHeight))
		{
			m_kInfoExtra.m_u32UnitWidth = m_kInfoExtra.m_u32PixelWidth >> 2;
			m_kInfoExtra.m_u32UnitHeight = m_kInfoExtra.m_u32PixelHeight >> 2;

			m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
			m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 1;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 16;
		}
		else
		{
			memset(&m_kHeader, 0, sizeof(FvDDS));
			return false;
		}
		break;
	case FVFMT_G8R8_G8B8:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
		m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
		break;
	case FVFMT_R8G8_B8G8:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
		m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
		break;
	case FVFMT_UYVY:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
		m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
		break;
	case FVFMT_YUY2:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
		m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
		break;
	case FVFMT_R16F:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
		m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
		break;
	case FVFMT_G16R16F:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
		m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
		break;
	case FVFMT_A16B16G16R16F:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
		m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
		break;
	case FVFMT_R32F:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
		m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
		break;
	case FVFMT_G32R32F:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
		m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
		break;
	case FVFMT_A32B32G32R32F:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
		m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
		break;
	case FVFMT_CxV8U8:
		m_kHeader.m_kPixelFormat.m_u32Flags |= FV_DDPF_FOURCC;
		m_kHeader.m_kPixelFormat.m_u32FourCC = eFormat;

		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
		break;
	default:
		memset(&m_kHeader, 0, sizeof(FvDDS));
		return false;
	}

	m_kInfoExtra.m_kPixelInfo.m_eFormat = eFormat;

	if(u32MipMapCount > 1)
	{
		m_kHeader.m_u32Flags |= FV_DDSD_MIPMAPCOUNT;
		m_kHeader.m_kCaps.m_u32Caps1 |= FV_DDSCAPS_COMPLEX;

		m_kHeader.m_u32MipMapCount = u32MipMapCount;

		m_kInfoExtra.m_kMipMap.m_bIsAvailable = true;
		m_kInfoExtra.m_kMipMap.m_u32Count = u32MipMapCount;

		m_kInfoExtra.m_kMipMap.m_ppvPointers = new void*[m_kHeader.m_u32MipMapCount];
		m_kInfoExtra.m_kMipMap.m_pu32MipMapSizes = new FvUInt32[m_kHeader.m_u32MipMapCount];
		memset(m_kInfoExtra.m_kMipMap.m_ppvPointers, 0, 4 * m_kHeader.m_u32MipMapCount);
		memset(m_kInfoExtra.m_kMipMap.m_pu32MipMapSizes, 0, 4 * m_kHeader.m_u32MipMapCount);
	}

	CreateData();

	m_kInfoExtra.m_bIsAvailable = true;

	return true;
}
//---------------------------------------------------------------------------
bool FvDDS::CheckHeader()
{
	if(m_kHeader.m_u32ImageMark == FV_DDS_MARK
		&& m_kHeader.m_u32Size == FV_DDS_HEADER_SIZE)
	{
		if(FV_CHECK_FLAG(m_kHeader.m_u32Flags, FV_DDSD_ALWAYS))
		{
			if(m_kHeader.m_kPixelFormat.m_u32Size == FV_DDS_PIXEL_FORMAT_SIZE)
			{
				if(FV_CHECK_FLAG(m_kHeader.m_kCaps.m_u32Caps1, FV_DDSCAPS_TEXTURE))
				{
					return true;
				}
			}
		}
	}
	printf_s("the header has errors\n");
	return false;
}
//---------------------------------------------------------------------------
bool FvDDS::ConfirmInfo()
{
	switch(m_kHeader.m_kCaps.m_u32Caps2)
	{
	case 0:
		m_kInfoExtra.m_eType = FV_TYPE_2D;
		break;
	case FV_DDSCAPS2_VOLUME:
		m_kInfoExtra.m_eType = FV_TYPE_3D;
		break;
	case FV_DDSCAPS2_CUBEMAP_ALL:
		m_kInfoExtra.m_eType = FV_TYPE_CUBE;
		break;
	default:
		printf_s("texture type is unknown\n");
		return false;
	}

	if(FV_CHECK_FLAG(m_kHeader.m_u32Flags, FV_DDSD_MIPMAPCOUNT))
	{
		if(m_kHeader.m_u32MipMapCount > 1)
		{
			m_kInfoExtra.m_kMipMap.m_bIsAvailable = true;
			m_kInfoExtra.m_kMipMap.m_u32Count = m_kHeader.m_u32MipMapCount;
			m_kInfoExtra.m_kMipMap.m_ppvPointers = new void*[m_kHeader.m_u32MipMapCount];
			m_kInfoExtra.m_kMipMap.m_pu32MipMapSizes = new FvUInt32[m_kHeader.m_u32MipMapCount];
			memset(m_kInfoExtra.m_kMipMap.m_ppvPointers, 0, 4 * m_kHeader.m_u32MipMapCount);
			memset(m_kInfoExtra.m_kMipMap.m_pu32MipMapSizes, 0, 4 * m_kHeader.m_u32MipMapCount);
		}
	}

	if(!ConfirmPixelInfo())
		return false;

	m_kInfoExtra.m_u32PixelWidth = m_kHeader.m_u32Width;
	m_kInfoExtra.m_u32PixelHeight = m_kHeader.m_u32Height;
	m_kInfoExtra.m_u32Depth = m_kHeader.m_u32Depth;
	
	switch(m_kInfoExtra.m_kPixelInfo.m_eFormat)
	{
	case FVFMT_DXT1:
	case FVFMT_DXT2:
	case FVFMT_DXT3:
	case FVFMT_DXT4:
	case FVFMT_DXT5:
		m_kInfoExtra.m_u32UnitWidth = m_kInfoExtra.m_u32PixelWidth >> 2;
		m_kInfoExtra.m_u32UnitHeight = m_kInfoExtra.m_u32PixelHeight >> 2;
		break;
	default:
		m_kInfoExtra.m_u32UnitWidth = m_kInfoExtra.m_u32PixelWidth;
		m_kInfoExtra.m_u32UnitHeight = m_kInfoExtra.m_u32PixelHeight;
		break;
	}

	CreateData();

	m_kInfoExtra.m_bIsAvailable = true;

	return true;
}
//---------------------------------------------------------------------------
#define FV_ISBITMASK( r,g,b,a )\
	( m_kHeader.m_kPixelFormat.m_u32RBitMask == r\
	&& m_kHeader.m_kPixelFormat.m_u32GBitMask == g\
	&& m_kHeader.m_kPixelFormat.m_u32BBitMask == b\
	&& m_kHeader.m_kPixelFormat.m_u32RGBAlphaBitMask == a )
//---------------------------------------------------------------------------
bool FvDDS::ConfirmPixelInfo()
{
	if(FV_CHECK_FLAG(m_kHeader.m_kPixelFormat.m_u32Flags, FV_DDPF_RGB))
	{
		if(m_kHeader.m_kPixelFormat.m_u32RGBBitCount == 8)
		{
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_R3G3B2;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 1;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 1;
			return true;
		}
		else if(m_kHeader.m_kPixelFormat.m_u32RGBBitCount == 16)
		{
			if(FV_ISBITMASK(0x00007c00, 0x000003e0, 0x0000001f, 0x00008000))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_A1R5G5B5;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
				return true;
			}
			else if(FV_ISBITMASK(0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_A4R4G4B4;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
				return true;
			}
			else if(FV_ISBITMASK(0x000000e0, 0x0000001c, 0x00000003, 0x0000ff00))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_A8R3G3B2;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
				return true;
			}
			else if(FV_ISBITMASK(0x0000f800, 0x000007e0, 0x0000001f, 0x00000000))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_R5G6B5;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
				return true;
			}
			else if(FV_ISBITMASK(0x00007c00, 0x000003e0, 0x0000001f, 0x00000000))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_X1R5G5B5;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
				return true;
			}
			else if(FV_ISBITMASK(0x00000f00, 0x000000f0, 0x0000000f, 0x00000000))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_X4R4G4B4;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
				return true;
			}
		}
		else if(m_kHeader.m_kPixelFormat.m_u32RGBBitCount == 24)
		{
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_R8G8B8;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 3;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 3;
			return true;
		}
		else if(m_kHeader.m_kPixelFormat.m_u32RGBBitCount == 32)
		{
			if(FV_ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_A8R8G8B8;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
				return true;
			}
			else if(FV_ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_A8B8G8R8;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
				return true;
			}
			else if(FV_ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_A2R10G10B10;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
				return true;
			}
			else if(FV_ISBITMASK(0x000003ff, 0x000ffc00, 0x3ff00000, 0xc0000000))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_A2B10G10R10;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
				return true;
			}
			else if(FV_ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_G16R16;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
				return true;
			}
			else if(FV_ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_X8R8G8B8;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
				return true;
			}
			else if(FV_ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_X8B8G8R8;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
				return true;
			}
		}
	}
	else if(FV_CHECK_FLAG(m_kHeader.m_kPixelFormat.m_u32Flags, FV_DDPF_LUMINANCE))
	{
		if(m_kHeader.m_kPixelFormat.m_u32RGBBitCount == 8)
		{
			if(FV_CHECK_FLAG(m_kHeader.m_kPixelFormat.m_u32Flags, FV_DDPF_ALPHAPIXELS))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_A4L4;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 1;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 1;
				return true;
			}
			else
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_L8;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 1;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 1;
				return true;
			}
		}
		else if(m_kHeader.m_kPixelFormat.m_u32RGBBitCount == 16)
		{
			if(FV_CHECK_FLAG(m_kHeader.m_kPixelFormat.m_u32Flags, FV_DDPF_ALPHAPIXELS))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_A8L8;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
				return true;
			}
			else
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_L16;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
				return true;
			}
		}
	}
	else if(FV_CHECK_FLAG(m_kHeader.m_kPixelFormat.m_u32Flags, FV_DDPF_ALPHA))
	{
		m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_A8;
		m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 1;
		m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 1;
		return true;
	}
	else if(FV_CHECK_FLAG(m_kHeader.m_kPixelFormat.m_u32Flags, FV_DDPF_FOURCC))
	{
		switch(m_kHeader.m_kPixelFormat.m_u32FourCC)
		{
		case FVFMT_A16B16G16R16:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_A16B16G16R16;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
			return true;
		case FVFMT_Q16W16V16U16:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_Q16W16V16U16;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
			return true;
		case FVFMT_DXT1:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_DXT1;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 0;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
			return true;
		case FVFMT_DXT2:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_DXT2;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 1;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 16;
			return true;
		case FVFMT_DXT3:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_DXT3;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 1;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 16;
			return true;
		case FVFMT_DXT4:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_DXT4;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 1;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 16;
			return true;
		case FVFMT_DXT5:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_DXT5;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 1;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 16;
			return true;
		case FVFMT_G8R8_G8B8:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_G8R8_G8B8;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
			return true;
		case FVFMT_R8G8_B8G8:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_R8G8_B8G8;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
			return true;
		case FVFMT_UYVY:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_UYVY;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
			return true;
		case FVFMT_YUY2:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_YUY2;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
			return true;
		case FVFMT_R16F:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_R16F;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
			return true;
		case FVFMT_G16R16F:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_G16R16F;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
			return true;
		case FVFMT_A16B16G16R16F:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_A16B16G16R16F;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
			return true;
		case FVFMT_R32F:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_R32F;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
			return true;
		case FVFMT_G32R32F:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_G32R32F;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 8;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 8;
			return true;
		case FVFMT_A32B32G32R32F:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_A32B32G32R32F;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 16;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 16;
			return true;
		case FVFMT_CxV8U8:
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_CxV8U8;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
			return true;
		default:
			break;
		}
	}
	else if(FV_CHECK_FLAG(m_kHeader.m_kPixelFormat.m_u32Flags, FV_DDPF_BUMPDUDV))
	{
		if(m_kHeader.m_kPixelFormat.m_u32RGBBitCount == 16)
		{
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_V8U8;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
			return true;
		}
		else if(m_kHeader.m_kPixelFormat.m_u32RGBBitCount == 32)
		{
			if(FV_ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_Q8W8V8U8;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
				return true;
			}
			else if(FV_ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_V16U16;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
				return true;
			}
			else if(FV_ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
			{
				m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_A2W10V10U10;
				m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
				m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
				return true;
			}
		}
	}
	else if(FV_CHECK_FLAG(m_kHeader.m_kPixelFormat.m_u32Flags, FV_DDPF_BUMPLUMINANCE))
	{
		if(m_kHeader.m_kPixelFormat.m_u32RGBBitCount == 16)
		{
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_L6V5U5;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 2;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 2;
			return true;
		}
		else if(m_kHeader.m_kPixelFormat.m_u32RGBBitCount == 32)
		{
			m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_X8L8V8U8;
			m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 4;
			m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 4;
			return true;
		}
	}

	m_kInfoExtra.m_kPixelInfo.m_eFormat = FVFMT_UNKNOWN;
	m_kInfoExtra.m_kPixelInfo.m_u32PixelSize = 0;
	m_kInfoExtra.m_kPixelInfo.m_u32UnitSize = 0;
	printf_s("pixel format is unknown\n");
	return false;
}
//---------------------------------------------------------------------------
bool FvDDS::IsAvailable()
{
	return m_kInfoExtra.m_bIsAvailable;
}
//---------------------------------------------------------------------------
FvUInt32 FvDDS::GetRowSize(FvUInt32 u32Mip)
{
	FvUInt32 u32Width = m_kInfoExtra.m_u32UnitWidth >> u32Mip;
	u32Width = (u32Width > 0) ? u32Width : 1;
	return m_kInfoExtra.m_kPixelInfo.m_u32UnitSize * u32Width;
}
//---------------------------------------------------------------------------
FvUInt32 FvDDS::GetPieceSize(FvUInt32 u32Mip)
{
	FvUInt32 u32Height = m_kInfoExtra.m_u32UnitHeight >> u32Mip;
	u32Height = (u32Height > 0) ? u32Height : 1;
	return GetRowSize(u32Mip) * u32Height;
}
//---------------------------------------------------------------------------
FvUInt32 FvDDS::GetMipSize(FvUInt32 u32Mip)
{
	switch(m_kInfoExtra.m_eType)
	{
	case FV_TYPE_2D:
		return GetPieceSize(u32Mip);
	case FV_TYPE_CUBE:
		return GetPieceSize(u32Mip) * 6;
	case FV_TYPE_3D:
		return GetPieceSize(u32Mip) * m_kInfoExtra.m_u32Depth;
	}
	return 0;
}
//---------------------------------------------------------------------------
void* FvDDS::GetMip(FvUInt32 u32Mip)
{
	if(m_kInfoExtra.m_kMipMap.m_bIsAvailable)
	{
		if(u32Mip < m_kInfoExtra.m_kMipMap.m_u32Count)
			return m_kInfoExtra.m_kMipMap.m_ppvPointers[u32Mip];
	}
	else
	{
		if(!u32Mip)
			return m_pvData; 
	}
	return 0;
}
//---------------------------------------------------------------------------
void* FvDDS::GetPiece(FvUInt32 u32Mip, FvUInt32 u32Piece)
{
	if(u32Piece < GetPieceCount())
	{
		return (void*)((FvUInt32)GetMip(u32Mip) + u32Piece * GetPieceSize(u32Mip));
	}
	else
	{
		return 0;
	}
}
//---------------------------------------------------------------------------
void* FvDDS::GetRow(FvUInt32 u32Mip, FvUInt32 u32Piece, FvUInt32 u32Row)
{
	if(u32Row < m_kInfoExtra.m_u32UnitHeight)
	{
		return (void*)((FvUInt32)GetPiece(u32Mip, u32Piece) + GetRowSize(u32Mip) * u32Row);
	}
	else
	{
		return 0;
	}
}
//---------------------------------------------------------------------------
void* FvDDS::GetUnit(FvUInt32 u32Mip, FvUInt32 u32Piece, FvUInt32 u32Row,
	FvUInt32 u32Unit)
{
	if(u32Unit < m_kInfoExtra.m_u32UnitWidth)
	{
		return (void*)((FvUInt32)GetRow(u32Mip, u32Piece, u32Row) + GetUnitSize() * u32Unit);
	}
	else
	{
		return 0;
	}
}

//---------------------------------------------------------------------------
FvUInt32 FvDDS::GetMipMapCount()
{
	if(m_kInfoExtra.m_kMipMap.m_bIsAvailable)
	{
		return m_kInfoExtra.m_kMipMap.m_u32Count;
	}
	else
		return 1;
	
}
//---------------------------------------------------------------------------
FvUInt32 FvDDS::GetPieceCount()
{
	switch(m_kInfoExtra.m_eType)
	{
	case FV_TYPE_2D:
		return 1;
	case FV_TYPE_CUBE:
		return 6;
	case FV_TYPE_3D:
		return m_kInfoExtra.m_u32Depth;
	}
	return 0;
}
//---------------------------------------------------------------------------
FvUInt32 FvDDS::GetPixelSize()
{
	return m_kInfoExtra.m_kPixelInfo.m_u32PixelSize;
}
//---------------------------------------------------------------------------
FvUInt32 FvDDS::GetPixelWidth()
{
	return m_kInfoExtra.m_u32PixelWidth;
}
//---------------------------------------------------------------------------
FvUInt32 FvDDS::GetPixelHeight()
{
	return m_kInfoExtra.m_u32PixelHeight;
}
//---------------------------------------------------------------------------
FvUInt32 FvDDS::GetUnitSize()
{
	return m_kInfoExtra.m_kPixelInfo.m_u32UnitSize;
}
//---------------------------------------------------------------------------
FvUInt32 FvDDS::GetUnitWidth()
{
	return m_kInfoExtra.m_u32UnitWidth;
}
//---------------------------------------------------------------------------
FvUInt32 FvDDS::GetUnitHeight()
{
	return m_kInfoExtra.m_u32UnitHeight;
}
//---------------------------------------------------------------------------
FvPixelFormat FvDDS::GetPixelFormat()
{
	return m_kInfoExtra.m_kPixelInfo.m_eFormat;
}
//---------------------------------------------------------------------------
FvDDSHeader *FvDDS::GetHeader()
{
	return &m_kHeader;
}
//---------------------------------------------------------------------------
void* FvDDS::GetBuffer()
{
	return m_pvData;
}
//---------------------------------------------------------------------------
FvUInt32 FvDDS::GetBufferSize()
{
	return m_u32DataSize;
}
//---------------------------------------------------------------------------
void FvDDS::CreateData()
{
	if(m_kInfoExtra.m_kMipMap.m_bIsAvailable)
	{
		for(FvUInt32 i(0); i < m_kInfoExtra.m_kMipMap.m_u32Count; i++)
		{
			m_kInfoExtra.m_kMipMap.m_pu32MipMapSizes[i] = GetMipSize(i);
			m_u32DataSize += m_kInfoExtra.m_kMipMap.m_pu32MipMapSizes[i];
		}
	}
	else
	{
		m_u32DataSize = GetMipSize(0);
	}

	m_pvData = new char[m_u32DataSize];

	if(m_kInfoExtra.m_kMipMap.m_bIsAvailable)
	{
		m_kInfoExtra.m_kMipMap.m_ppvPointers[0] = m_pvData;
		for(FvUInt32 i(1); i < m_kInfoExtra.m_kMipMap.m_u32Count; i++)
		{
			m_kInfoExtra.m_kMipMap.m_ppvPointers[i]
			= (void*)((FvUInt32)m_kInfoExtra.m_kMipMap.m_ppvPointers[i - 1]
			+ m_kInfoExtra.m_kMipMap.m_pu32MipMapSizes[i - 1]);
		}
	}
}
//---------------------------------------------------------------------------
FvTextureType FvDDS::GetTextureType()
{
	return m_kInfoExtra.m_eType;
}
//---------------------------------------------------------------------------
