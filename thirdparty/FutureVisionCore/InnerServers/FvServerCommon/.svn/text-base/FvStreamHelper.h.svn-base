//{future header message}
#ifndef __FvStreamHelper_H__
#define __FvStreamHelper_H__

#include <FvNetInterfaceMinder.h>

namespace FvStreamHelper
{
	FV_INLINE void addRealEntity( FvBinaryOStream & /*stream*/ )
	{
	}

	FV_INLINE void addRealEntityWithWitnesses( FvBinaryOStream & stream,
			int maxPacketSize = 2000,
			float ghostDistance = 0.f,
			bool aoiUpdates = false,
			float aoiHyst = 5.f )
	{
		stream << maxPacketSize;
		stream << FvUInt32(0);
		stream << FvUInt32(0);
		stream << ghostDistance;
		stream << aoiUpdates;
		stream << aoiHyst;
		stream << 0.f;
	}

#pragma pack( push, 1 )
	/** @internal */
	struct AddEntityData
	{
		AddEntityData() :
			id( 0 ),
			isOnGround( false ),
			lastEventNumber( 1 ),
			baseAddr( FvNetAddress( 0, 0 ) )
		{}
		AddEntityData( FvEntityID _id, const FvVector3 & _position,
				bool _isOnGround, FvEntityTypeID _typeID,
				const FvDirection3 & _direction,
				FvNetAddress _baseAddr = FvNetAddress( 0, 0 ) ) :
			id( _id ),
			typeID( _typeID ),
			position( _position ),
			isOnGround( _isOnGround ),
			direction( _direction ),
			lastEventNumber( 1 ),
			baseAddr( _baseAddr )
		{}

		FvEntityID id;
		FvEntityTypeID typeID;
		FvVector3 position;
		bool isOnGround;
		FvDirection3 direction;
		FvEventNumber lastEventNumber;
		FvNetAddress baseAddr;
	};
#pragma pack( pop )
}

FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvStreamHelper::AddEntityData)

namespace FvStreamHelper
{
	FV_INLINE void addEntity( FvBinaryOStream & stream, const AddEntityData & data )
	{
		stream << data;
	}

	FV_INLINE int removeEntity( FvBinaryIStream & stream, AddEntityData & data )
	{
		stream >> data;

		return sizeof( data );
	}

	FV_INLINE void addEntityLogOffData( FvBinaryOStream & stream,
			FvEventNumber number )
	{
		stream << number;
	}

	FV_INLINE void removeEntityLogOffData( FvBinaryIStream & stream,
			FvEventNumber & number )
	{
		stream >> number;
	}

	struct CellEntityBackupHeader
	{
		FvInt32 m_iCellClientSize;
		FvEntityID m_uiVehicleID;
	};
}


#endif // __FvStreamHelper_H__
