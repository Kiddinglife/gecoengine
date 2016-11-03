//{future header message}
#ifndef __FvBinaryStream_H__
#define __FvBinaryStream_H__

#include "FvKernel.h"
#include <FvPowerDefines.h>

#include <vector>
#include <map>
#include <list>
#include <string>
#include <cstring>

const int FV_BS_BUFF_MAX_SIZE = 2048;
class FvBinaryIStream;


//! 声明流化函数为友元
#define FV_ISTREAM_FRIEND_DECLARE								\
	template<class T>											\
	friend FvBinaryIStream& operator>>(FvBinaryIStream&, T&)
#define FV_OSTREAM_FRIEND_DECLARE								\
	template<class T>											\
	friend FvBinaryOStream& operator<<(FvBinaryOStream&, const T&)
#define FV_IOSTREAM_FRIEND_DECLARE	\
	FV_ISTREAM_FRIEND_DECLARE;		\
	FV_OSTREAM_FRIEND_DECLARE


//! 声明流化函数
#define FV_ISTREAM_DECLARE(_PREFIX, _TYPE)								\
	template<>															\
	_PREFIX FvBinaryIStream& operator>>(FvBinaryIStream&, _TYPE&)
#define FV_OSTREAM_DECLARE(_PREFIX, _TYPE)								\
	template<>															\
	_PREFIX FvBinaryOStream& operator<<(FvBinaryOStream&, const _TYPE&)
#define FV_IOSTREAM_DECLARE(_PREFIX, _TYPE)	\
	FV_ISTREAM_DECLARE(_PREFIX, _TYPE);		\
	FV_OSTREAM_DECLARE(_PREFIX, _TYPE)


//! 定义按内存拷贝方式实现流化函数
#define FV_ISTREAM_IMP_BY_MEMCPY(_PREFIX, _TYPE)								\
	template<>																	\
	_PREFIX FvBinaryIStream& operator>>(FvBinaryIStream& kIS, _TYPE& kDes)		\
	{																			\
		kDes = *((_TYPE*)kIS.Retrieve(sizeof(_TYPE)));							\
		return kIS;																\
	}
#define FV_OSTREAM_IMP_BY_MEMCPY(_PREFIX, _TYPE)								\
	template<>																	\
	_PREFIX FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const _TYPE& kSrc)\
	{																			\
		*((_TYPE*)kOS.Reserve(sizeof(_TYPE))) = kSrc;							\
		return kOS;																\
	}
#define FV_IOSTREAM_IMP_BY_MEMCPY(_PREFIX, _TYPE)	\
	FV_ISTREAM_IMP_BY_MEMCPY(_PREFIX, _TYPE)		\
	FV_OSTREAM_IMP_BY_MEMCPY(_PREFIX, _TYPE)


class FV_KERNEL_API FvBinaryOStream
{
public:
	virtual ~FvBinaryOStream() {}

	virtual void *Reserve( int iBytes ) = 0;

	virtual int Size() const	{ return -1; }
	void AppendString( const char *pkData, int iLength );

	void Transfer( FvBinaryIStream & from, int length );
	virtual void AddBlob( const void * pBlob, int size );

	void WritePackedInt( int len );
	void WriteStringLength( int len )	{ this->WritePackedInt( len ); }

	static int CalculatePackedIntSize( int value );
};

class FV_KERNEL_API FvBinaryIStream
{
public:

	FvBinaryIStream() { m_bError = false; }
	virtual ~FvBinaryIStream() {};

	virtual const void *Retrieve( int nBytes ) = 0;
	virtual int RemainingLength() const = 0;
	virtual void Finish() {}

	virtual char Peek() = 0;

	int ReadPackedInt();
	int ReadStringLength()	{ return this->ReadPackedInt(); }

	bool Error() const { return m_bError; }
	void Error( bool bError ) { m_bError = bError; }

	static char ms_acErrBuf[ FV_BS_BUFF_MAX_SIZE ];

protected:
	bool m_bError;
};

FV_INLINE void FvBinaryOStream::Transfer( FvBinaryIStream & from, int length )
{
	this->AddBlob( from.Retrieve( length ), length );
}

FV_INLINE void FvBinaryOStream::AddBlob( const void * pBlob, int size )
{
	std::memcpy( this->Reserve( size ), pBlob, size );
}

template <class T>
FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const T& kData);

template <class T>
FvBinaryIStream& operator>>(FvBinaryIStream& kIS, T& kData);


#include "FvBinaryStream.inl"


template <>
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const std::string& kData)
{
	kOS.AppendString(kData.data(), int(kData.length()));
	return kOS;
}

FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, char* kData)
{
	kOS.AppendString(kData, int(strlen(kData)));
	return kOS;
}

FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const char* kData)
{
	return (kOS << const_cast<char*>(kData));
}

template <class T, class A>
FV_INLINE FvBinaryOStream & operator<<( FvBinaryOStream & b, const std::vector<T,A> & data)
{
	FvUInt32 max = (FvUInt32)data.size();
	b << max;
	for(FvUInt32 i=0; i < max; i++)
		b << data[i];
	return b;
}

template <bool, class A>
FV_INLINE FvBinaryOStream & operator<<( FvBinaryOStream & b, const std::vector<bool,A> & data)
{
	FvUInt32 max = (FvUInt32)data.size();
	b << max;
	for(FvUInt32 i=0; i < max; i++)
		b << data[i];
	return b;
}

template <class T, class A>
FV_INLINE FvBinaryOStream & operator<<( FvBinaryOStream & b, const std::map<T,A> & data)
{
	FvUInt32 max = (FvUInt32)data.size();
	b << max;
	std::map<T,A>::const_iterator itrB = data.begin();
	std::map<T,A>::const_iterator itrE = data.end();
	while(itrB != itrE)
	{
		b << itrB->first;
		b << itrB->second;
		++itrB;
	}
	return b;
}

template <class T, class A>
FV_INLINE FvBinaryOStream & operator<<( FvBinaryOStream & b, const std::list<T,A> & data)
{
	FvUInt32 max = (FvUInt32)data.size();
	b << max;
	std::list<T,A>::const_iterator itrB = data.begin();
	std::list<T,A>::const_iterator itrE = data.end();
	while(itrB != itrE)
	{
		b << *itrB;
		++itrB;
	}
	return b;
}

FV_INLINE void FvBinaryOStream::WritePackedInt( int iValue )
{
	if (iValue >= 255)
	{
		(*this) << (FvUInt8)0xff;
		FV_PACK3( (char*)this->Reserve( 3 ), iValue );
	}
	else
	{
		(*this) << (FvUInt8)iValue;
	}
}

FV_INLINE int FvBinaryOStream::CalculatePackedIntSize( int iValue )
{
	return (iValue >= 0xff) ? 4 : 1;
}

FV_INLINE int FvBinaryIStream::ReadPackedInt()
{
	int	value = *(FvUInt8*)this->Retrieve( 1 );

	if (value == 0xFF)
	{
		value = FV_UNPACK3( (const char*)this->Retrieve( 3 ) );
		// ASSERT(LITTLE_ENDIAN)
	}

	return value;
}

template <>
FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, std::string& kData)
{
	int	length = kIS.ReadPackedInt();
	char* s = (char*)kIS.Retrieve(length);

	if(s != NULL)
		kData.assign(s, length);

	return kIS;
}

FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, char* str)
{
	int	length = kIS.ReadPackedInt();
	char* s = (char*)kIS.Retrieve(length);

	if(s != NULL)
	{
		memcpy(str, s, length);
		str[length] ='\0';
	}

	return kIS;
}

template <class T, class A>
FV_INLINE FvBinaryIStream & operator>>( FvBinaryIStream & b, std::vector<T,A> & data)
{
	T elt;
	FvUInt32 max = 0;

	b >> max;

	data.clear();
	data.reserve( max );
	for(FvUInt32 i=0; i < max; i++)
	{
		b >> elt;
		data.push_back( elt );
	}
	return b;
}

template <bool, class A>
FV_INLINE FvBinaryIStream & operator>>( FvBinaryIStream & b, std::vector<bool,A> & data)
{
	bool elt;
	FvUInt32 max = 0;

	b >> max;

	data.clear();
	data.reserve( max );
	for(FvUInt32 i=0; i < max; i++)
	{
		b >> elt;
		data.push_back( elt );
	}
	return b;
}

template <class T, class A>
FV_INLINE FvBinaryIStream & operator>>( FvBinaryIStream & b, std::map<T,A> & data)
{
	T idx;
	A elt;
	FvUInt32 max = 0;
	b >> max;
	data.clear();
	for(FvUInt32 i=0; i < max; i++)
	{
		b >> idx;
		b >> elt;
		data.insert( std::make_pair(idx,elt) );
	}
	return b;
}

template <class T, class A>
FV_INLINE FvBinaryIStream & operator>>( FvBinaryIStream & b, std::list<T,A> & data)
{
	T elt;
	FvUInt32 max = 0;

	b >> max;

	data.clear();
	for(FvUInt32 i=0; i < max; i++)
	{
		b >> elt;
		data.push_back( elt );
	}
	return b;
}

FV_INLINE void FvBinaryOStream::AppendString( const char *pkData, int iLength )
{
	this->WritePackedInt( iLength );
	this->AddBlob( pkData, iLength );
}


#ifndef FV_USE_TOKENS
#define FV_USE_TOKENS
#endif

#ifdef FV_USE_TOKENS

#define FV_TOKEN_ADD( stream, TOKEN )										\
	stream << *(int*)TOKEN;

#define FV_TOKEN_CHECK( stream, TOKEN )										\
{																			\
	int iToken;																\
	stream >> iToken;														\
																			\
	if (iToken != *(int*)TOKEN)												\
	{																		\
		FV_ERROR_MSG( "%s(%d): TOKEN_CHECK( %s ) failed!\n",				\
				__FILE__, __LINE__, TOKEN );								\
	}																		\
}

#else

#define FV_TOKEN_ADD( stream )
#define FV_TOKEN_CHECK( stream )

#endif

#endif // __FvBinaryStream_H__
