
#define FV_BS_NO_WATCHER_INCLUDE
#include "FvDebug.h"
#undef  FV_BS_NO_WATCHER_INCLUDE

typedef union
{
	FvUInt8 u8;
	FvInt8 i8;
	char c;
	signed char sc;
	unsigned char uc;
	bool b;
} FvNetByte;


typedef union
{
	FvUInt16 u16;
	FvInt16 i16;
	short s;
	unsigned short us;
} FvNetShort;


typedef union
{
	FvUInt32 u32;
	FvInt32 i32;
	int i;
	unsigned int ui;
	long l;
	unsigned long ul;
	float f;
} FvNetLong;


typedef union
{
	FvUInt64 u64;
	FvInt64 i64;
	long long ll;
	unsigned long long ull;
	double d;
} FvNetLongLong;

#ifdef _BIG_ENDIAN

FV_INLINE short FV_HTONS( short x )
{
	short res =
		((x & 0x00ff) << 8) |
		((x & 0xff00) >> 8);
	return res;
}


FV_INLINE long FV_HTONL( long x )
{
	long res =
		((x & 0x000000ff) << 24) |
		((x & 0x0000ff00) << 8) |
		((x & 0x00ff0000) >> 8) |
		((x & 0xff000000) >> 24);
	return res;
}


FV_INLINE long long FV_HTONLL( long long x )
{
	long long res =
		((x & 0x00000000000000ffULL) << 56) |
		((x & 0x000000000000ff00ULL) << 40) |
		((x & 0x0000000000ff0000ULL) << 24) |
		((x & 0x00000000ff000000ULL) << 8) |
		((x & 0x000000ff00000000ULL) >> 8) |
		((x & 0x0000ff0000000000ULL) >> 24) |
		((x & 0x00ff000000000000ULL) >> 40) |
		((x & 0xff00000000000000ULL) >> 56);
	return res;
}


FV_INLINE float FV_HTONF( float f )
{
	FvNetLong n;
	n.f = f;
	n.u32 = FV_HTONL( n.u32 );
	return n.f;
}

FV_INLINE void FV_HTONF_ASSIGN( float &dest, float f )
{
	FvNetLong *pDest = (FvNetLong*)&dest;
	pDest->u32 = *(FvUInt32*)&f;
	pDest->u32 = FV_HTONL( pDest->u32 );
}

FV_INLINE void FV_HTON3_ASSIGN( char *pDest, const char *pData )
{
	pDest[0] = pData[2];
	pDest[1] = pData[1];
	pDest[2] = pData[0];
}

FV_INLINE void FV_PACK3( char *pDest, FvUInt32 src )
{
	pDest[0] = (char)(src >> 16);
	pDest[1] = (char)(src >> 8);
	pDest[2] = (char)src;
}

FV_INLINE FvUInt32 FV_UNPACK3( const char *pData )
{
	const FvUInt8 *data = (const FvUInt8*)pData;
	return (data[0] << 16) | (data[1] << 8) | data[2];
}

#else // BIG_ENDIAN

#define FV_HTONS( x ) x
#define FV_HTONL( x ) x
#define FV_HTONLL( x ) x
#define FV_HTONF( x ) x
#define FV_HTONF_ASSIGN( dest, x ) (dest = x)

FV_INLINE void FV_HTON3_ASSIGN( char *pDest, const char *pData )
{
	pDest[0] = pData[0];
	pDest[1] = pData[1];
	pDest[2] = pData[2];
}

FV_INLINE void FV_PACK3( char *pDest, FvUInt32 src )
{
	pDest[0] = (char)src;
	pDest[1] = (char)(src >> 8);
	pDest[2] = (char)(src >> 16);
}

FV_INLINE FvUInt32 FV_UNPACK3( const char *pData )
{
	const FvUInt8 *data = (const FvUInt8*)pData;
	return data[0] | (data[1] << 8) | (data[2] << 16);
}

#endif

#define FV_NTOHS( x ) FV_HTONS( x )
#define FV_NTOHL( x ) FV_HTONL( x )
#define FV_NTOHLL( x ) FV_HTONLL( x )
#define FV_NTOHF( x ) FV_HTONF( x )
#define FV_NTOHF_ASSIGN( dest, x ) FV_HTONF_ASSIGN( dest, x )
#define FV_NTOH3_ASSIGN( pDest, pData ) FV_HTON3_ASSIGN( pDest, pData )

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvNetByte& kData)
{
	FV_ASSERT(sizeof(FvNetByte) == 1);
	*(char*)kOS.Reserve(sizeof(kData)) = kData.c;
	return kOS;
}

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvNetShort& kData)
{
	FV_ASSERT(sizeof(FvNetShort) == 2);
	*(short*)kOS.Reserve(sizeof(kData)) = FV_HTONS(kData.s);
	return kOS;
}

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvNetLong& kData)
{
	FV_ASSERT(sizeof(FvNetLong) == 4);
	*(FvUInt32*)kOS.Reserve(sizeof(kData)) = FV_HTONL(kData.u32);
	return kOS;
}

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvNetLongLong& kData)
{
	FV_ASSERT(sizeof(FvNetLongLong) == 8);
	*(FvUInt64*)kOS.Reserve(sizeof(kData)) = FV_HTONLL(kData.u64);
	return kOS;
}

/**
FV_INLINE FvBinaryOStream& operator<<( FvBinaryOStream &out, FvUInt8 x )
{
	FvNetByte n;
	n.u8 = x;
	return out << n;
}

FV_INLINE FvBinaryOStream& operator<<( FvBinaryOStream &out, FvUInt16 x )
{
	FvNetShort n;
	n.u16 = x;
	return out << n;
}

FV_INLINE FvBinaryOStream& operator<<( FvBinaryOStream &out, FvUInt32 x )
{
	FvNetLong n;
	n.u32 = x;
	return out << n;
}

FV_INLINE FvBinaryOStream& operator<<( FvBinaryOStream &out, FvUInt64 x )
{
	FvNetLongLong n;
	n.u64 = x;
	return out << n;
}

FV_INLINE FvBinaryOStream& operator<<( FvBinaryOStream &out, FvInt8 x )
{
	FvNetByte n;
	n.i8 = x;
	return out << n;
}

FV_INLINE FvBinaryOStream& operator<<( FvBinaryOStream &out, FvInt16 x )
{
	FvNetShort n;
	n.i16 = x;
	return out << n;
}

FV_INLINE FvBinaryOStream& operator<<( FvBinaryOStream &out, FvInt32 x )
{
	FvNetLong n;
	n.i32 = x;
	return out << n;
}

FV_INLINE FvBinaryOStream& operator<<( FvBinaryOStream &out, FvInt64 x )
{
	FvNetLongLong n;
	n.i64 = x;
	return out << n;
}
**/

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const char& kData)
{
	FvNetByte n;
	n.c = kData;
	return kOS << n;
}

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const signed char& kData)
{
	FvNetByte n;
	n.sc = kData;
	return kOS << n;
}

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const unsigned char& kData)
{
	FvNetByte n;
	n.uc = kData;
	return kOS << n;
}

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const bool& kData)
{
	FvNetByte n;
	n.b = kData;
	return kOS << n;
}

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const short& kData)
{
	FvNetShort n;
	n.s = kData;
	return kOS << n;
}

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const unsigned short& kData)
{
	FvNetShort n;
	n.us = kData;
	return kOS << n;
}

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const int& kData)
{
	FvNetLong n;
	n.i = kData;
	return kOS << n;
}

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const unsigned int& kData)
{
	FvNetLong n;
	n.ui = kData;
	return kOS << n;
}

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const long& kData)
{
	FvNetLong n;
	n.l = kData;
	return kOS << n;
}

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const unsigned long& kData)
{
	FvNetLong n;
	n.ul = kData;
	return kOS << n;
}

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const float& kData)
{
	FvNetLong n;
	n.f = kData;
	return kOS << n;
}

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const long long& kData)
{
	FvNetLongLong n;
	n.ll = kData;
	return kOS << n;
}

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const unsigned long long& kData)
{
	FvNetLongLong n;
	n.ull = kData;
	return kOS << n;
}

template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const double& kData)
{
	FvNetLongLong n;
	n.d = kData;
	return kOS << n;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvNetByte& kData)
{
	FV_ASSERT(sizeof(FvNetByte) == 1);
	kData.c = *(char*)kIS.Retrieve(sizeof(kData));
	return kIS;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvNetShort& kData)
{
	FV_ASSERT(sizeof(FvNetShort) == 2);
	kData.s = FV_NTOHS(*(short*)kIS.Retrieve(sizeof(kData)));
	return kIS;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvNetLong& kData)
{
	FV_ASSERT(sizeof(FvNetLong) == 4);
	kData.u32 = FV_NTOHL(*(FvUInt32*)kIS.Retrieve(sizeof(kData)));
	return kIS;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvNetLongLong& kData)
{
	FV_ASSERT(sizeof(FvNetLongLong) == 8);
	kData.u64 = FV_NTOHLL(*(FvUInt64*)kIS.Retrieve(sizeof(kData)));
	return kIS;
}

/**
FV_INLINE FvBinaryIStream& operator>>( FvBinaryIStream &in, FvUInt8 &x )
{
	FvNetByte n;
	in >> n;
	x = n.u8;
	return in;
}

FV_INLINE FvBinaryIStream& operator>>( FvBinaryIStream &in, FvUInt16 &x )
{
	FvNetShort n;
	in >> n;
	x = n.u16;
	return in;
}

FV_INLINE FvBinaryIStream& operator>>( FvBinaryIStream &in, FvUInt32 &x )
{
	FvNetLong n;
	in >> n;
	x = n.u32;
	return in;
}

FV_INLINE FvBinaryIStream& operator>>( FvBinaryIStream &in, FvUInt64 &x )
{
	FvNetLongLong n;
	in >> n;
	x = n.u64;
	return in;
}

FV_INLINE FvBinaryIStream& operator>>( FvBinaryIStream &in, FvInt8 &x )
{
	FvNetByte n;
	in >> n;
	x = n.i8;
	return in;
}

FV_INLINE FvBinaryIStream& operator>>( FvBinaryIStream &in, FvInt16 &x )
{
	FvNetShort n;
	in >> n;
	x = n.i16;
	return in;
}

FV_INLINE FvBinaryIStream& operator>>( FvBinaryIStream &in, FvInt32 &x )
{
	FvNetLong n;
	in >> n;
	x = n.i32;
	return in;
}

FV_INLINE FvBinaryIStream& operator>>( FvBinaryIStream &in, FvInt64 &x )
{
	FvNetLongLong n;
	in >> n;
	x = n.i64;
	return in;
}
**/

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, char& kData)
{
	FvNetByte n;
	kIS >> n;
	kData = n.c;
	return kIS;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, signed char& kData)
{
	FvNetByte n;
	kIS >> n;
	kData = n.sc;
	return kIS;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, unsigned char& kData)
{
	FvNetByte n;
	kIS >> n;
	kData = n.uc;
	return kIS;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, bool& kData)
{
	FvNetByte n;
	kIS >> n;
	kData = n.b;
	return kIS;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, short& kData)
{
	FvNetShort n;
	kIS >> n;
	kData = n.s;
	return kIS;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, unsigned short& kData)
{
	FvNetShort n;
	kIS >> n;
	kData = n.us;
	return kIS;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, int& kData)
{
	FvNetLong n;
	kIS >> n;
	kData = n.i;
	return kIS;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, unsigned int& kData)
{
	FvNetLong n;
	kIS >> n;
	kData = n.ui;
	return kIS;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, long& kData)
{
	FvNetLong n;
	kIS >> n;
	kData = n.l;
	return kIS;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, unsigned long& kData)
{
	FvNetLong n;
	kIS >> n;
	kData = n.ul;
	return kIS;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, float& kData)
{
	FvNetLong n;
	kIS >> n;
	kData = n.f;
	return kIS;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, long long& kData)
{
	FvNetLongLong n;
	kIS >> n;
	kData = n.ll;
	return kIS;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, unsigned long long& kData)
{
	FvNetLongLong n;
	kIS >> n;
	kData = n.ull;
	return kIS;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, double& kData)
{
	FvNetLongLong n;
	kIS >> n;
	kData = n.d;
	return kIS;
}


/**
#if defined( PLAYSTATION3 )

FV_INLINE FvBinaryOStream& operator<<( FvBinaryOStream &out, long x )
{
FvNetLong n;
n.i32 = x;
return out << n;
}

FV_INLINE FvBinaryIStream& operator>>( FvBinaryIStream &in, FvInt8 &x )
{
	FvNetByte n;
	in >> n;
	x = n.b;
	return in;
}

#endif
**/