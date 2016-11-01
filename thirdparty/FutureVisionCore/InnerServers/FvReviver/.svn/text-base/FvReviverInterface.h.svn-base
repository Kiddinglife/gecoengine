//{future header message}
#if defined( DEFINE_INTERFACE_HERE ) || defined( DEFINE_SERVER_HERE )
	#undef __FvReviverInterface_H__
#endif

#ifndef __FvReviverInterface_H__
#define __FvReviverInterface_H__

#include <FvNetInterfaceMinderMacros.h>

#define FV_REVIVER_MSGS( COMPONENT )										\
	MERCURY_FIXED_MESSAGE( Handle##COMPONENT##Birth,						\
							sizeof( FvNetAddress ),							\
							&g_kReviverOf##COMPONENT )						\
	MERCURY_FIXED_MESSAGE( Handle##COMPONENT##Death,						\
							sizeof( FvNetAddress ),							\
							&g_kReviverOf##COMPONENT )						\


#pragma pack(push, 1)
BEGIN_MERCURY_INTERFACE( ReviverInterface )

	//FV_REVIVER_MSGS( CellAppMgr )
	FV_REVIVER_MSGS( BaseAppMgr )
	FV_REVIVER_MSGS( DB )
	FV_REVIVER_MSGS( Login )

END_MERCURY_INTERFACE()
#pragma pack(pop)

#undef BEGIN_REVIVER_MSG
#undef FV_REVIVER_MSGS

#endif // __FvReviverInterface_H__
