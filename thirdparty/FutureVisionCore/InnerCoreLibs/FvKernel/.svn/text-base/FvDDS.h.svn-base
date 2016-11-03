//{future header message}
#ifndef __FvDDS_H__
#define __FvDDS_H__

#include "FvKernel.h"

typedef unsigned char FvUInt8;
typedef unsigned short FvUInt16;
typedef unsigned int FvUInt32;

typedef struct  
{
	FvUInt32 m_u32Size;
	FvUInt32 m_u32Flags;
	FvUInt32 m_u32FourCC;
	FvUInt32 m_u32RGBBitCount;
	FvUInt32 m_u32RBitMask, m_u32GBitMask, m_u32BBitMask;
	FvUInt32 m_u32RGBAlphaBitMask;
} FvDDSPixelFormat;

typedef struct
{
	FvUInt32 m_u32Caps1;
	FvUInt32 m_u32Caps2;
	FvUInt32 m_au32Reserved[2];
} FvDDSCaps;

typedef enum
{
	FV_DDSD_CAPS = 0x00000001,
	FV_DDSD_HEIGHT = 0x00000002,
	FV_DDSD_WIDTH = 0x00000004,
	FV_DDSD_PIXELFORMAT = 0x00001000,
	FV_DDSD_MIPMAPCOUNT = 0x00020000,
	FV_DDSD_DEPTH = 0x00800000,
	
	FV_DDSD_ALWAYS = FV_DDSD_CAPS | FV_DDSD_HEIGHT | FV_DDSD_WIDTH | FV_DDSD_PIXELFORMAT

} FvDDSFlags;

typedef enum
{
	FV_DDPF_RGB = 0x00000040,
	FV_DDPF_FOURCC = 0x00000004,
	FV_DDPF_ALPHAPIXELS = 0x00000001,
	FV_DDPF_LUMINANCE = 0x00020000,
	FV_DDPF_ALPHA = 0x00000002,

	FV_DDPF_BUMPDUDV = 0x00080000,
	FV_DDPF_BUMPLUMINANCE = 0x00040000
} FvDDSPixelFormatFlags;

typedef enum
{
	FV_DDSCAPS_ALPHA = 0x00000002,
	FV_DDSCAPS_COMPLEX = 0x00000008,
	FV_DDSCAPS_MIPMAP = 0x00400000,
	FV_DDSCAPS_TEXTURE = 0x00001000
} FvDDSCaps1;

typedef enum
{
	FV_DDSCAPS2_CUBEMAP = 0x00000200,
	FV_DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400,
	FV_DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800,
	FV_DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000,
	FV_DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000,
	FV_DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000,
	FV_DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000,

	FV_DDSCAPS2_CUBEMAP_ALL = FV_DDSCAPS2_CUBEMAP_POSITIVEX
		| FV_DDSCAPS2_CUBEMAP_NEGATIVEX | FV_DDSCAPS2_CUBEMAP_POSITIVEY
		| FV_DDSCAPS2_CUBEMAP_NEGATIVEY | FV_DDSCAPS2_CUBEMAP_POSITIVEZ
		| FV_DDSCAPS2_CUBEMAP_NEGATIVEZ | FV_DDSCAPS2_CUBEMAP,

	FV_DDSCAPS2_VOLUME = 0x00200000
} FvDDSCaps2;

typedef enum
{
	FV_TYPE_2D,
	FV_TYPE_CUBE,
	FV_TYPE_3D
} FvTextureType;

typedef struct
{
	FvUInt32 m_u32ImageMark;
	FvUInt32 m_u32Size;
	FvUInt32 m_u32Flags;
	FvUInt32 m_u32Height;
	FvUInt32 m_u32Width;
	FvUInt32 m_u32PitchOrLinearSize;
	FvUInt32 m_u32Depth;
	FvUInt32 m_u32MipMapCount;
	FvUInt32 m_au32Reserved1[11];
	FvDDSPixelFormat m_kPixelFormat;
	FvDDSCaps m_kCaps;
	FvUInt32 m_u32Reserved2;
} FvDDSHeader;

#define FV_CHECK_FLAG(value, flag) (((value) & (flag)) == (flag))

#define FV_MAKEFOURCC(ch0, ch1, ch2, ch3) \
	((FvUInt32)(FvUInt8)(ch0) | ((FvUInt32)(FvUInt8)(ch1) << 8) |   \
	((FvUInt32)(FvUInt8)(ch2) << 16) | ((FvUInt32)(FvUInt8)(ch3) << 24 ))

#define FV_DDS_MARK FV_MAKEFOURCC('D', 'D', 'S', ' ')
#define FV_DDS_HEADER_SIZE 124
#define FV_DDS_PIXEL_FORMAT_SIZE 32

typedef enum
{
	FVFMT_UNKNOWN              =  0,

	FVFMT_R8G8B8               = 20,
	FVFMT_A8R8G8B8             = 21,
	FVFMT_X8R8G8B8             = 22,
	FVFMT_R5G6B5               = 23,
	FVFMT_X1R5G5B5             = 24,
	FVFMT_A1R5G5B5             = 25,
	FVFMT_A4R4G4B4             = 26,
	FVFMT_R3G3B2               = 27,
	FVFMT_A8                   = 28,
	FVFMT_A8R3G3B2             = 29,
	FVFMT_X4R4G4B4             = 30,
	FVFMT_A2B10G10R10          = 31,
	FVFMT_A8B8G8R8             = 32,
	FVFMT_X8B8G8R8             = 33,
	FVFMT_G16R16               = 34,
	FVFMT_A2R10G10B10          = 35,
	FVFMT_A16B16G16R16         = 36,

	FVFMT_A8P8                 = 40,
	FVFMT_P8                   = 41,

	FVFMT_L8                   = 50,
	FVFMT_A8L8                 = 51,
	FVFMT_A4L4                 = 52,

	FVFMT_V8U8                 = 60,
	FVFMT_L6V5U5               = 61,
	FVFMT_X8L8V8U8             = 62,
	FVFMT_Q8W8V8U8             = 63,
	FVFMT_V16U16               = 64,
	FVFMT_A2W10V10U10          = 67,

	FVFMT_UYVY                 = FV_MAKEFOURCC('U', 'Y', 'V', 'Y'),
	FVFMT_R8G8_B8G8            = FV_MAKEFOURCC('R', 'G', 'B', 'G'),
	FVFMT_YUY2                 = FV_MAKEFOURCC('Y', 'U', 'Y', '2'),
	FVFMT_G8R8_G8B8            = FV_MAKEFOURCC('G', 'R', 'G', 'B'),
	FVFMT_DXT1                 = FV_MAKEFOURCC('D', 'X', 'T', '1'),
	FVFMT_DXT2                 = FV_MAKEFOURCC('D', 'X', 'T', '2'),
	FVFMT_DXT3                 = FV_MAKEFOURCC('D', 'X', 'T', '3'),
	FVFMT_DXT4                 = FV_MAKEFOURCC('D', 'X', 'T', '4'),
	FVFMT_DXT5                 = FV_MAKEFOURCC('D', 'X', 'T', '5'),

	FVFMT_D16_LOCKABLE         = 70,
	FVFMT_D32                  = 71,
	FVFMT_D15S1                = 73,
	FVFMT_D24S8                = 75,
	FVFMT_D24X8                = 77,
	FVFMT_D24X4S4              = 79,
	FVFMT_D16                  = 80,

	FVFMT_D32F_LOCKABLE        = 82,
	FVFMT_D24FS8               = 83,

	FVFMT_L16                  = 81,

	FVFMT_VERTEXDATA           =100,
	FVFMT_INDEX16              =101,
	FVFMT_INDEX32              =102,

	FVFMT_Q16W16V16U16         =110,

	FVFMT_MULTI2_ARGB8         = FV_MAKEFOURCC('M','E','T','1'),

	FVFMT_R16F                 = 111,
	FVFMT_G16R16F              = 112,
	FVFMT_A16B16G16R16F        = 113,

	FVFMT_R32F                 = 114,
	FVFMT_G32R32F              = 115,
	FVFMT_A32B32G32R32F        = 116,

	FVFMT_CxV8U8               = 117
} FvPixelFormat;

class FV_KERNEL_API FvDDS
{
public:
	FvDDS();

	~FvDDS();

	bool LoadFromMem(char *pcData,size_t stSize);

	bool LoadFromFile(const char* pcFileName);

	bool SaveToFile(const char* pcFileName);

	bool Create(FvTextureType eType, FvPixelFormat eFormat,
		FvUInt32 u32Width, FvUInt32 u32Height,
		FvUInt32 u32MipMapCount = 0, FvUInt32 u32Depth = 0);

	bool IsAvailable();

	void Reset();

	void* GetMip(FvUInt32 u32Mip);

	void* GetPiece(FvUInt32 u32Mip, FvUInt32 u32Piece);

	void* GetRow(FvUInt32 u32Mip, FvUInt32 u32Piece, FvUInt32 u32Row);

	void* GetUnit(FvUInt32 u32Mip, FvUInt32 u32Piece, FvUInt32 u32Row, FvUInt32 u32Unit);

	FvTextureType GetTextureType();

	FvUInt32 GetMipMapCount();

	FvUInt32 GetPieceCount();

	FvUInt32 GetPixelSize();

	FvUInt32 GetPixelWidth();

	FvUInt32 GetPixelHeight();

	FvUInt32 GetUnitSize();

	FvUInt32 GetUnitWidth();

	FvUInt32 GetUnitHeight();

	FvPixelFormat GetPixelFormat();

	FvDDSHeader *GetHeader();

	void* GetBuffer();

	FvUInt32 GetBufferSize();

	FvUInt32 GetRowSize(FvUInt32 u32Mip);

	FvUInt32 GetPieceSize(FvUInt32 u32Mip);

	FvUInt32 GetMipSize(FvUInt32 u32Mip);

protected:
	bool CheckHeader();

	bool ConfirmInfo();

	bool ConfirmPixelInfo();

	void CreateData();

	FvDDSHeader m_kHeader;
	void* m_pvData;
	FvUInt32 m_u32DataSize;

	struct
	{
		bool m_bIsAvailable;
		FvTextureType m_eType;
		struct 
		{
			bool m_bIsAvailable;
			FvUInt32 m_u32Count;
			void** m_ppvPointers;
			FvUInt32* m_pu32MipMapSizes;
		} m_kMipMap;

		struct
		{
			FvPixelFormat m_eFormat;
			FvUInt32 m_u32PixelSize;
			FvUInt32 m_u32UnitSize;
		} m_kPixelInfo;

		FvUInt32 m_u32PixelHeight;
		FvUInt32 m_u32PixelWidth;
		FvUInt32 m_u32UnitHeight;
		FvUInt32 m_u32UnitWidth;
		FvUInt32 m_u32Depth;
	} m_kInfoExtra;
};

#endif // __FvDDS_H__
