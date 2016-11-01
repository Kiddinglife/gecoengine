//{future header message}
#ifndef __FvServerCommon_H__
#define __FvServerCommon_H__

#include <FvNetTypes.h>

#ifdef FV_AS_STATIC_LIBS
#define FV_SERVERCOMMON_API
#else
#ifdef FV_SERVERCOMMON_EXPORT
#define FV_SERVERCOMMON_API FV_Proper_Export_Flag
#else
#define FV_SERVERCOMMON_API FV_Proper_Import_Flag
#endif
#endif

const bool DEFAULT_UPDATER_ENABLED = false;

const float GHOST_DISTANCE = 500.f; 				// 500 metres

const long DEFAULT_GAME_UPDATE_HERTZ = 10;

const long DEFAULT_BANDWIDTH_TO_CLIENT = 256;

const float DEFAULT_MAX_CPU_OFFLOAD = 0.02f;

enum FvShutDownStage
{
	SHUTDOWN_NONE,

	SHUTDOWN_REQUEST,

	SHUTDOWN_INFORM,

	SHUTDOWN_DISCONNECT_PROXIES,

	SHUTDOWN_PERFORM,

	SHUTDOWN_TRIGGER
};

FV_INLINE FvBinaryIStream& operator>>(FvBinaryIStream& kIS, FvShutDownStage& kDes)
{
	FvUInt8 t;
	kIS >> t;
	kDes = FvShutDownStage(t);
	return kIS;
}
FV_INLINE FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const FvShutDownStage& kSrc)
{
	return kOS << FvUInt8(kSrc);
}

#endif // __FvServerCommon_H__
