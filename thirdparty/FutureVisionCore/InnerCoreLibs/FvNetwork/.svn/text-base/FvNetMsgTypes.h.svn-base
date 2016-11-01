//{future header message}
#ifndef __FvNetMsgTypes_H__
#define __FvNetMsgTypes_H__

#include "FvNetTypes.h"

#include <FvPowerDefines.h>
#include <FvBinaryStream.h>

#include <float.h>
#include <math.h>

class FvCoord
{
	typedef float Storage;

	public:
		FvCoord(float x, float y, float z) : m_fX(x), m_fY(y), m_fZ(z)
		{}

		FvCoord( const FvVector3 & v ) : m_fX(v.x), m_fY(v.y), m_fZ(v.z)
		{}

		FvCoord()
		{}

		bool operator==( const FvCoord & oth ) const
		{ return m_fX == oth.m_fX && m_fY == oth.m_fY && m_fZ == oth.m_fZ; }

	Storage m_fX;
	Storage m_fY;
	Storage m_fZ;
};

FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvCoord)


FV_INLINE FvInt8 AngleToInt8( float angle )
{
	return (FvInt8)floorf( (angle * 128.f) / FV_MATH_PI_F + 0.5f );
}


FV_INLINE float Int8ToAngle( FvInt8 angle )
{
	return float(angle) * (FV_MATH_PI_F / 128.f);
}


FV_INLINE FvInt8 HalfAngleToInt8( float angle )
{
	return (FvInt8)floorf( (angle * 256.f) / FV_MATH_PI_F + 0.5f );
}


FV_INLINE float Int8ToHalfAngle( FvInt8 angle )
{
	return float(angle) * (FV_MATH_PI_F / 256.f);
}


class FvYawPitch
{
public:
	FvYawPitch( float yaw, float pitch )
	{
		this->Set( yaw, pitch );
	}

	FvYawPitch() {};

	void Set( float yaw, float pitch )
	{
		m_uiYaw   = AngleToInt8( yaw );
		m_uiPitch = HalfAngleToInt8( pitch );
	}

	void Get( float & yaw, float & pitch ) const
	{
		yaw   = Int8ToAngle( m_uiYaw );
		pitch = Int8ToHalfAngle( m_uiPitch );
	}

private:
	FvUInt8	m_uiYaw;
	FvUInt8	m_uiPitch;
};

FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvYawPitch)


class FvYawPitchRoll
{
public:
	FvYawPitchRoll( float yaw, float pitch, float roll )
	{
		this->Set( yaw, pitch, roll );
	}

	FvYawPitchRoll() {};

	bool operator==( const FvYawPitchRoll & oth ) const
	{
		return m_uiYaw == oth.m_uiYaw && m_uiPitch == oth.m_uiPitch && m_uiRoll == oth.m_uiRoll;
	}

	void Set( float yaw, float pitch, float roll )
	{
		m_uiYaw   = AngleToInt8( yaw );
		m_uiPitch = HalfAngleToInt8( pitch );
		m_uiRoll = AngleToInt8( roll );
	}

	void Get( float & yaw, float & pitch, float & roll ) const
	{
		yaw   = Int8ToAngle( m_uiYaw );
		pitch = Int8ToHalfAngle( m_uiPitch );
		roll = Int8ToAngle( m_uiRoll );
	}

	bool NearTo( const FvYawPitchRoll & oth ) const
	{
		return unsigned int( (m_uiYaw-oth.m_uiYaw+1) | (m_uiPitch-oth.m_uiPitch+1) |
			(m_uiRoll-oth.m_uiRoll+1) ) <= 3;
	}

private:
	FvUInt8	m_uiYaw;
	FvUInt8	m_uiPitch;
	FvUInt8	m_uiRoll;
};

FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvYawPitchRoll)


class FvPackedXY
{
protected:
	union MultiType
	{
		float asFloat;
		unsigned int asUint;
		int asInt;
	};

public:
	FvPackedXY() {};
	FvPackedXY( float x, float y );

	FV_INLINE void PackXY( float x, float y );
	FV_INLINE void UnpackXY( float & x, float & y ) const;

	FV_INLINE void GetXYError( float & xError, float & zError ) const;

	FV_IOSTREAM_FRIEND_DECLARE;

private:
	unsigned char m_acData[3];
};


FV_INLINE FvPackedXY::FvPackedXY( float x, float y )
{
	this->PackXY( x, y );
}


FV_INLINE void FvPackedXY::PackXY( float xValue, float yValue )
{
	const float addValues[] = { 2.f, -2.f };
	const FvUInt32 xCeilingValues[] = { 0, 0x7ff000 };
	const FvUInt32 yCeilingValues[] = { 0, 0x0007ff };

	FvPackedXY::MultiType x; x.asFloat = xValue;
	FvPackedXY::MultiType y; y.asFloat = yValue;


	x.asFloat += addValues[ x.asInt < 0 ];
	y.asFloat += addValues[ y.asInt < 0 ];

	unsigned int result = 0;

	result |= xCeilingValues[((x.asUint & 0x7c000000) != 0x40000000) ||
										((x.asUint & 0x3ffc000) == 0x3ffc000)];
	result |= yCeilingValues[((y.asUint & 0x7c000000) != 0x40000000) ||
										((y.asUint & 0x3ffc000) == 0x3ffc000)];


	result |= ((x.asUint >>  3) & 0x7ff000) + ((x.asUint & 0x4000) >> 2);
	result |= ((y.asUint >> 15) & 0x0007ff) + ((y.asUint & 0x4000) >> 14);

	result &= 0x7ff7ff;

	result |=  (x.asUint >>  8) & 0x800000;
	result |=  (y.asUint >> 20) & 0x000800;

	FV_PACK3( (char*)m_acData, result );
}


FV_INLINE void FvPackedXY::UnpackXY( float & xarg, float & yarg ) const
{
	unsigned int data = FV_UNPACK3( (const char*)m_acData );

	MultiType & xu = (MultiType&)xarg;
	MultiType & yu = (MultiType&)yarg;

	xu.asUint = 0x40000000;
	yu.asUint = 0x40000000;

	xu.asUint |= (data & 0x7ff000) << 3;
	yu.asUint |= (data & 0x0007ff) << 15;

	xu.asFloat -= 2.0f;
	yu.asFloat -= 2.0f;

	xu.asUint |= (data & 0x800000) << 8;
	yu.asUint |= (data & 0x000800) << 20;
}


FV_INLINE void FvPackedXY::GetXYError( float & xError, float & yError ) const
{
	FvUInt32 data = FV_UNPACK3( (const char*)m_acData );

	int xExp = (data >> 20) & 0x7;
	int yExp = (data >>  8) & 0x7;

	xError = (1.f/256.f) * (1 << xExp);
	yError = (1.f/256.f) * (1 << yExp);
}

template<>
FV_INLINE FvBinaryIStream& operator>>( FvBinaryIStream& is, FvPackedXY &xy )
{
	FV_ASSERT( sizeof( xy.m_acData ) == 3 );
	const void *src = is.Retrieve( sizeof( xy.m_acData ) );
	FV_NTOH3_ASSIGN( reinterpret_cast<char *>( xy.m_acData ),
		reinterpret_cast<const char*>( src ) );
	return is;
}

template<>
FV_INLINE FvBinaryOStream& operator<<( FvBinaryOStream& os, const FvPackedXY &xy )
{
	FV_ASSERT( sizeof( xy.m_acData ) == 3 );
	void * dest = os.Reserve( sizeof( xy.m_acData ) );
	FV_HTON3_ASSIGN( reinterpret_cast< char * >( dest ),
		reinterpret_cast< const char* >( xy.m_acData ) );
	return os;
}


class FvPackedXYH : public FvPackedXY
{
public:
	FvPackedXYH() : m_iHeight( 0 ) {};
private:
	FvInt8 m_iHeight;
};


class FvPackedXYZ : public FvPackedXY
{
public:
	FvPackedXYZ();
	FvPackedXYZ( float x, float y, float z );

	FV_INLINE void PackXYZ( float x, float y, float z );
	FV_INLINE void UnpackXYZ( float & x, float & y, float & z ) const;
	FV_INLINE void GetXYZError( float & xErr, float & yErr, float & zErr ) const;

	FV_INLINE void SetZ( float fZ );
	FV_INLINE float GetZ() const;
	FV_INLINE float ZError() const;

	FV_IOSTREAM_FRIEND_DECLARE;

private:
	unsigned char m_acZData[2];
};


FV_INLINE FvPackedXYZ::FvPackedXYZ()
{
}


FV_INLINE FvPackedXYZ::FvPackedXYZ( float x, float y, float z )
{
	this->PackXY( x, y );
	this->SetZ( z );
}


FV_INLINE void FvPackedXYZ::PackXYZ( float x, float y, float z )
{
	this->SetZ( z );
	this->PackXY( x, y );
}


FV_INLINE void FvPackedXYZ::UnpackXYZ( float & x, float & y, float & z ) const
{
	z = this->GetZ();
	this->UnpackXY( x, y );
}


FV_INLINE void FvPackedXYZ::GetXYZError(
		float & xErr, float & yErr, float & zErr ) const
{
	this->GetXYError( xErr, yErr );
	zErr = this->ZError();
}


FV_INLINE void FvPackedXYZ::SetZ( float fZ )
{
	const float addValues[] = { 2.f, -2.f };
	MultiType z; z.asFloat = fZ;
	z.asFloat += addValues[ z.asInt < 0 ];

	FvUInt16& zDataInt = *(FvUInt16*)m_acZData;

	zDataInt = (z.asUint >> 12) & 0x7fff;

 	zDataInt |= ((z.asUint >> 16) & 0x8000);

}


FV_INLINE float FvPackedXYZ::GetZ() const
{
	MultiType z;
	z.asUint = 0x40000000;

	FvUInt16& zDataInt = *(FvUInt16*)m_acZData;

	z.asUint |= (zDataInt & 0x7fff) << 12;

	z.asFloat -= 2.f;

	z.asUint |= (zDataInt & 0x8000) << 16;

	return z.asFloat;
}


FV_INLINE float FvPackedXYZ::ZError() const
{
	FvUInt16& zDataInt = *(FvUInt16*)m_acZData;

	int zExp = (zDataInt >> 11) & 0xf;

	return (2.f/2048.f) * (1 << zExp);
}

template<>
FV_INLINE FvBinaryIStream& operator>>( FvBinaryIStream& is, FvPackedXYZ &xyz )
{
	FV_ASSERT( sizeof( xyz.m_acZData ) == sizeof( FvUInt16 ) );
	return is >> (FvPackedXY&)xyz >> *(FvUInt16*)xyz.m_acZData;
}

template<>
FV_INLINE FvBinaryOStream& operator<<( FvBinaryOStream& os, const FvPackedXYZ &xyz )
{
	FV_ASSERT( sizeof( xyz.m_acZData ) == sizeof( FvUInt16 ) );
	return os << (FvPackedXY&)xyz << *(FvUInt16*)xyz.m_acZData;
}


typedef FvUInt8 FvIDAlias;


FV_INLINE FvVector3 CalculateReferencePosition( const FvVector3 & pos )
{
	return FvVector3( FvRoundf( pos.x ), FvRoundf( pos.y ), FvRoundf( pos.z ) );
}

#endif // __FvNetMsgTypes_H__
