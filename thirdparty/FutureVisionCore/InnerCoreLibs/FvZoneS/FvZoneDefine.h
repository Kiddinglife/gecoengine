//{future header message}
#ifndef __FvZoneDefine_H__
#define __FvZoneDefine_H__

#ifdef FV_AS_STATIC_LIBS
#define FV_ZONE_API
#else
#ifdef FV_ZONE_EXPORT
#define FV_ZONE_API FV_Proper_Export_Flag
#else
#define FV_ZONE_API FV_Proper_Import_Flag
#endif
#endif

#endif // __FvZoneDefine_H__