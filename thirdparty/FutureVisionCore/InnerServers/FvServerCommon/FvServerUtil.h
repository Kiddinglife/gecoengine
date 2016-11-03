//{future header message}
#ifndef __FvServerUtil_H__
#define __FvServerUtil_H__

#include "FvServerCommon.h"

#include <stdlib.h>

#include <FvVector2.h>
#include <FvVector3.h>
#include <FvNetTypes.h>

namespace FvServerUtil
{
	FV_INLINE
	FvVector2 projectXY( const FvVector3 & pos )
	{
		return FvVector2( pos.x, pos.y );
	}

	template <class X>
	FV_INLINE X sqr( X x )
	{
		return x * x;
	}

	FV_INLINE float randomPos( long min, long max )
	{
		return (float)((rand() % (max - min)) + min);
	}

	FV_INLINE float distSqrBetween(const FvVector2 & v1,
			const FvVector2 & v2)
	{
		return sqr(v1.x - v2.x) + sqr(v1.y - v2.y);
	}

	FV_INLINE float distSqrBetween(const FvVector3 & v1,
			const FvVector3 & v2)
	{
		return sqr(v1.x - v2.x) +
			sqr(v1.y - v2.y) +
			sqr(v1.z - v2.z);
	}

	FV_INLINE
	int bpsToPacketSize( int bitsPerSecond, int updateHertz )
	{
		return (bitsPerSecond / (FV_NET_BITS_PER_BYTE * updateHertz)) -
			FV_NET_UDP_OVERHEAD;
	}

	FV_INLINE
	int bpsToBytesPerTick( int bitsPerSecond, int updateHertz )
	{
		return (bitsPerSecond / (FV_NET_BITS_PER_BYTE * updateHertz));
	}

	FV_INLINE
	int packetSizeToBPS( int packetSize, int updateHertz )
	{
		return (packetSize + FV_NET_UDP_OVERHEAD) * updateHertz *
			FV_NET_BITS_PER_BYTE;
	}
};

#endif // __FvServerUtil_H__

