//{future header message}
#ifndef FV_COMMON_ARGS
#define FV_COMMON_ARGS( ARGS ) ARGS
#define FV_COMMON_ISTREAM( NAME, XSTREAM ) MERCURY_ISTREAM( NAME, XSTREAM )
#define FV_COMMON_OSTREAM( NAME, XSTREAM ) MERCURY_OSTREAM( NAME, XSTREAM )
#endif

#ifndef FV_END_COMMON_MSG
#define FV_END_COMMON_MSG END_STRUCT_MESSAGE
#endif


FV_BEGIN_COMMON_PASSENGER_MSG( TickSync )
FV_COMMON_ARGS(
	FvUInt8				tickByte; )
FV_END_COMMON_MSG()
FV_COMMON_ISTREAM( TickSync, x.tickByte )
FV_COMMON_OSTREAM( TickSync, x.tickByte )


FV_BEGIN_COMMON_UNRELIABLE_MSG( RelativePositionReference )
FV_COMMON_ARGS(
	FvUInt8				sequenceNumber; )
FV_END_COMMON_MSG()
FV_COMMON_ISTREAM( RelativePositionReference, x.sequenceNumber )
FV_COMMON_OSTREAM( RelativePositionReference, x.sequenceNumber )

FV_BEGIN_COMMON_UNRELIABLE_MSG( RelativePosition )
FV_COMMON_ARGS(
	FvVector3				position; )
FV_END_COMMON_MSG()
FV_COMMON_ISTREAM( RelativePosition, x.position )
FV_COMMON_OSTREAM( RelativePosition, x.position )


FV_BEGIN_COMMON_RELIABLE_MSG( SetVehicle )
FV_COMMON_ARGS(
	FvEntityID			passengerID;
	FvEntityID			vehicleID; )
FV_END_COMMON_MSG()
FV_COMMON_ISTREAM( SetVehicle, x.passengerID >> x.vehicleID )
FV_COMMON_OSTREAM( SetVehicle, x.passengerID << x.vehicleID )


#define AVUPID_NoAlias			FvEntityID		id;
#define AVUPID_STREAM_NoAlias					x.id

#define AVUPID_Alias			FvIDAlias			idAlias;
#define AVUPID_STREAM_Alias						x.idAlias


#define AVUPPOS_FullPos			FvPackedXYZ		position;
#define AVUPPOS_ISTREAM_FullPos					>> x.position
#define AVUPPOS_OSTREAM_FullPos					<< x.position

#define AVUPPOS_OnChunk			FvPackedXYH		position;
#define AVUPPOS_ISTREAM_OnChunk					>> x.position
#define AVUPPOS_OSTREAM_OnChunk					<< x.position

#define AVUPPOS_OnGround		FvPackedXY		position;
#define AVUPPOS_ISTREAM_OnGround				>> x.position
#define AVUPPOS_OSTREAM_OnGround				<< x.position

#define AVUPPOS_NoPos
#define AVUPPOS_ISTREAM_NoPos
#define AVUPPOS_OSTREAM_NoPos


#define AVUPDIR_YawPitchRoll	FvYawPitchRoll	dir;
#define AVUPDIR_ISTREAM_YawPitchRoll			>> x.dir
#define AVUPDIR_OSTREAM_YawPitchRoll			<< x.dir

#define AVUPDIR_YawPitch		FvYawPitch		dir;
#define AVUPDIR_ISTREAM_YawPitch				>> x.dir
#define AVUPDIR_OSTREAM_YawPitch				<< x.dir

#define AVUPDIR_Yaw				FvInt8			dir;
#define AVUPDIR_ISTREAM_Yaw						>> x.dir
#define AVUPDIR_OSTREAM_Yaw						<< x.dir

#define AVUPDIR_NoDir
#define AVUPDIR_ISTREAM_NoDir
#define AVUPDIR_OSTREAM_NoDir


#define AVUPMSG( ID, POS, DIR )											\
	FV_BEGIN_COMMON_UNRELIABLE_MSG( AvatarUpdate##ID##POS##DIR )		\
	FV_COMMON_ARGS(														\
		AVUPID_##ID														\
		AVUPPOS_##POS													\
		AVUPDIR_##DIR )													\
	FV_END_COMMON_MSG()													\
	FV_COMMON_ISTREAM( AvatarUpdate##ID##POS##DIR,						\
		AVUPID_STREAM_##ID AVUPPOS_ISTREAM_##POS AVUPDIR_ISTREAM_##DIR )\
	FV_COMMON_OSTREAM( AvatarUpdate##ID##POS##DIR,						\
		AVUPID_STREAM_##ID AVUPPOS_OSTREAM_##POS AVUPDIR_OSTREAM_##DIR )\


	AVUPMSG( NoAlias, FullPos, YawPitchRoll )
	AVUPMSG( NoAlias, FullPos, YawPitch )
	AVUPMSG( NoAlias, FullPos, Yaw )
	AVUPMSG( NoAlias, FullPos, NoDir )
	AVUPMSG( NoAlias, OnChunk, YawPitchRoll )
	AVUPMSG( NoAlias, OnChunk, YawPitch )
	AVUPMSG( NoAlias, OnChunk, Yaw )
	AVUPMSG( NoAlias, OnChunk, NoDir )
	AVUPMSG( NoAlias, OnGround, YawPitchRoll )
	AVUPMSG( NoAlias, OnGround, YawPitch )
	AVUPMSG( NoAlias, OnGround, Yaw )
	AVUPMSG( NoAlias, OnGround, NoDir )
	AVUPMSG( NoAlias, NoPos, YawPitchRoll )
	AVUPMSG( NoAlias, NoPos, YawPitch )
	AVUPMSG( NoAlias, NoPos, Yaw )
	AVUPMSG( NoAlias, NoPos, NoDir )
	AVUPMSG( Alias, FullPos, YawPitchRoll )
	AVUPMSG( Alias, FullPos, YawPitch )
	AVUPMSG( Alias, FullPos, Yaw )
	AVUPMSG( Alias, FullPos, NoDir )
	AVUPMSG( Alias, OnChunk, YawPitchRoll )
	AVUPMSG( Alias, OnChunk, YawPitch )
	AVUPMSG( Alias, OnChunk, Yaw )
	AVUPMSG( Alias, OnChunk, NoDir )
	AVUPMSG( Alias, OnGround, YawPitchRoll )
	AVUPMSG( Alias, OnGround, YawPitch )
	AVUPMSG( Alias, OnGround, Yaw )
	AVUPMSG( Alias, OnGround, NoDir )
	AVUPMSG( Alias, NoPos, YawPitchRoll )
	AVUPMSG( Alias, NoPos, YawPitch )
	AVUPMSG( Alias, NoPos, Yaw )
	AVUPMSG( Alias, NoPos, NoDir )


#undef FV_BEGIN_COMMON_UNRELIABLE_MSG
#undef FV_BEGIN_COMMON_PASSENGER_MSG
#undef FV_BEGIN_COMMON_RELIABLE_MSG
#undef FV_COMMON_ARGS
#undef FV_END_COMMON_MSG
#undef FV_COMMON_ISTREAM
#undef FV_COMMON_OSTREAM

