//{future header message}
#ifndef __FvBackupHash_H__
#define __FvBackupHash_H__

#include <FvNetTypes.h>
#include <FvBinaryStream.h>

#include <vector>
#include <stdlib.h>

#include "FvServerCommon.h"

class FV_SERVERCOMMON_API FvMiniBackupHash
{
public:
	FvMiniBackupHash( FvUInt32 prime = 0, FvUInt32 size = 0 ) :
		m_uiPrime( prime ),
		m_uiSize( size ),
		m_uiVirtualSize( 0 )
	{
		this->HandleSizeChange( m_uiSize );
	};
	~FvMiniBackupHash() {};

	FvUInt32 Prime() const		{ return m_uiPrime; }
	FvUInt32 Size() const			{ return m_uiSize; }
	FvUInt32 VirtualSize() const	{ return m_uiVirtualSize; }

	FvUInt32 HashFor( FvEntityID id ) const;
	FvUInt32 VirtualSizeFor( FvUInt32 index ) const;

protected:
	void HandleSizeChange( FvUInt32 newSize )
	{
		m_uiSize = newSize;
		if (m_uiSize > 0)
		{
			m_uiVirtualSize = 1;
			while (m_uiVirtualSize < m_uiSize)
			{
				m_uiVirtualSize *= 2;
			}
		}
		else
		{
			m_uiVirtualSize = 0;
		}
	}

protected:
	FvUInt32 m_uiPrime;

private:
	FvUInt32 m_uiSize;
	FvUInt32 m_uiVirtualSize;

	friend FvBinaryOStream &
		operator<<( FvBinaryOStream & b, const FvMiniBackupHash & hash );
	friend FvBinaryIStream & operator>>( FvBinaryIStream & b,
			FvMiniBackupHash & hash );
};


class FV_SERVERCOMMON_API FvBackupHash : public FvMiniBackupHash
{
public:
	FvBackupHash();

	FvNetAddress AddressFor( FvEntityID id ) const;

	class DiffVisitor
	{
	public:
		virtual ~DiffVisitor() {};
		virtual void OnAdd( const FvNetAddress & addr,
				FvUInt32 index, FvUInt32 virtualSize, FvUInt32 prime ) = 0;
		virtual void OnChange( const FvNetAddress & addr,
				FvUInt32 index, FvUInt32 virtualSize, FvUInt32 prime ) = 0;
		virtual void OnRemove( const FvNetAddress & addr,
				FvUInt32 index, FvUInt32 virtualSize, FvUInt32 prime ) = 0;
	};

	void diff( const FvBackupHash & other, DiffVisitor & visitor );

	void clear()
	{
		m_kAddrs.clear();
		this->HandleSizeChange( 0 );
	}

	bool empty() const	{ return m_kAddrs.empty(); }
	const FvNetAddress & operator[]( const int index ) const
	{
		return m_kAddrs[ index ];
	}

	void swap( FvBackupHash & other )
	{
		this->m_kAddrs.swap( other.m_kAddrs );
		FvUInt32 temp = m_uiPrime;
		m_uiPrime = other.m_uiPrime;
		other.m_uiPrime = temp;

		this->HandleSizeChange( this->m_kAddrs.size() );
		other.HandleSizeChange( other.m_kAddrs.size() );
	}

	void push_back( const FvNetAddress & addr );
	bool erase( const FvNetAddress & addr );

private:
	static FvUInt32 ChoosePrime();

	typedef std::vector< FvNetAddress > Container;
	Container m_kAddrs;

	friend FvBinaryOStream &
		operator<<( FvBinaryOStream & b, const FvBackupHash & hash );
	friend FvBinaryIStream & operator>>( FvBinaryIStream & b, FvBackupHash & hash );
};


FV_INLINE FvBinaryOStream & operator<<( FvBinaryOStream & b,
		const FvMiniBackupHash & hash )
{
	b << hash.m_uiSize << hash.m_uiPrime;
	return b;
}

FV_INLINE FvBinaryIStream & operator>>( FvBinaryIStream & b, FvMiniBackupHash & hash )
{
	b >> hash.m_uiSize >> hash.m_uiPrime;
	hash.HandleSizeChange( hash.m_uiSize );
	return b;
}


FV_INLINE FvBinaryOStream & operator<<( FvBinaryOStream & b, const FvBackupHash & hash )
{
	b << hash.m_uiPrime << hash.m_kAddrs;
	return b;
}

FV_INLINE FvBinaryIStream & operator>>( FvBinaryIStream & b, FvBackupHash & hash )
{
	b >> hash.m_uiPrime >> hash.m_kAddrs;
	hash.HandleSizeChange( hash.m_kAddrs.size() );
	return b;
}

#endif // __FvBackupHash_H__
