#include "FvBackupHash.h"

#include <FvDebug.h>

#include <map>
#include <algorithm>

FV_DECLARE_DEBUG_COMPONENT2( "Server", 0 );

FvUInt32 FvMiniBackupHash::HashFor( FvEntityID id ) const
{
	if (m_uiSize > 0)
	{
		FvUInt32 hash = ((FvUInt32(id) * m_uiPrime) >> 8) % m_uiVirtualSize;

		if (hash >= m_uiSize)
		{
			hash -= m_uiVirtualSize/2;
		}

		return hash;
	}

	return FvUInt32( -1 );
}

FvUInt32 FvMiniBackupHash::VirtualSizeFor( FvUInt32 index ) const
{
	FvUInt32 miniVirtualSize = m_uiVirtualSize/2;
	if ((index < m_uiSize - miniVirtualSize) || (miniVirtualSize <= index))
	{
		return m_uiVirtualSize;
	}
	return miniVirtualSize;
}


FvBackupHash::FvBackupHash() : FvMiniBackupHash( FvBackupHash::ChoosePrime() )
{

}


FvNetAddress FvBackupHash::AddressFor( FvEntityID id ) const
{
	if (!m_kAddrs.empty())
	{
		return m_kAddrs[ this->HashFor( id ) ];
	}

	return FvNetAddress( 0, 0 );
}


void FvBackupHash::diff( const FvBackupHash & other, DiffVisitor & visitor )
{
	bool primeChanged = ( other.Prime() != this->Prime() );

	std::map< FvNetAddress, FvUInt32 > srcHash;

	for (FvUInt32 i = 0; i < m_kAddrs.size(); ++i)
	{
		srcHash[ m_kAddrs[i] ] = i;
	}

	for (FvUInt32 i = 0; i < other.m_kAddrs.size(); ++i)
	{
		const FvNetAddress & otherAddr = other.m_kAddrs[ i ];
		std::map< FvNetAddress, FvUInt32 >::iterator findIter =
			srcHash.find( otherAddr );

		if (findIter != srcHash.end())
		{
			if ((i != findIter->second) ||
				(other.VirtualSizeFor( i ) != this->VirtualSizeFor( i )) ||
				primeChanged)
			{
				visitor.OnChange( otherAddr, i,
						other.VirtualSizeFor( i ), other.Prime() );
			}
			srcHash.erase( findIter );
		}
		else
		{
			visitor.OnAdd( otherAddr, i,
					other.VirtualSizeFor( i ), other.Prime() );
		}
	}

	std::map< FvNetAddress, FvUInt32 >::iterator mapIter = srcHash.begin();
	while (mapIter != srcHash.end())
	{
		visitor.OnRemove( mapIter->first,
						mapIter->second,
						this->VirtualSizeFor( mapIter->second ),
						this->Prime() );
		++mapIter;
	}
}


void FvBackupHash::push_back( const FvNetAddress & addr )
{
	m_kAddrs.push_back( addr );
	this->HandleSizeChange( m_kAddrs.size() );
}


bool FvBackupHash::erase( const FvNetAddress & addr )
{
	Container::iterator iter = std::find( m_kAddrs.begin(), m_kAddrs.end(), addr );
	if (iter != m_kAddrs.end())
	{
		*iter = m_kAddrs.back();
		m_kAddrs.pop_back();

		this->HandleSizeChange( m_kAddrs.size() );

		return true;
	}

	return false;
}


FvUInt32 FvBackupHash::ChoosePrime()
{
	static const FvUInt32 primes[] =
	{
		0x9e377e55, 0x9e377e53, 0x9e377e43, 0x9e377e41, 0x9e377e37,
		0x9e377e11, 0x9e377e07, 0x9e377de1, 0x9e377db7, 0x9e377da5,
		0x9e377d99, 0x9e377d8f, 0x9e377d8d, 0x9e377d81, 0x9e377d65,
		0x9e377d5f, 0x9e377d53, 0x9e377d47, 0x9e377d21, 0x9e377d03,
		0x9e377cff, 0x9e377cd3, 0x9e377c97, 0x9e377c75, 0x9e377c6d,
		0x9e377c3d, 0x9e377c33, 0x9e377c01, 0x9e377bcb, 0x9e377bbb,
		0x9e377b91, 0x9e377b85, 0x9e377b55, 0x9e377b0b, 0x9e377ae9,
		0x9e377ae3, 0x9e377aab, 0x9e377aa7, 0x9e377a9f, 0x9e377a6f,
		0x9e377a5d, 0x9e377a45, 0x9e377a0b, 0x9e3779f3, 0x9e3779ed,
		0x9e3779e5, 0x9e3779e1, 0x9e3779db, 0x9e3779cd, 0x9e3779b1,
	};

	return primes[ rand() % (sizeof( primes )/sizeof( primes[0])) ];
}