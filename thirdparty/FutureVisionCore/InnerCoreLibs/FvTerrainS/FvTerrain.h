//{future header message}
#ifndef __FvTerrain_H__
#define __FvTerrain_H__

#ifdef FV_AS_STATIC_LIBS
#define FV_TERRAIN_API
#else
#ifdef FV_TERRAIN_EXPORT
#define FV_TERRAIN_API FV_Proper_Export_Flag
#else
#define FV_TERRAIN_API FV_Proper_Import_Flag
#endif
#endif

#endif // __FvTerrain_H__