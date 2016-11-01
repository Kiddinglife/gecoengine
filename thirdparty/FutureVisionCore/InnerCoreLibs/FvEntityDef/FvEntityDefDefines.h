//{future header message}
#ifndef __FvEntityDefDefines_H__
#define __FvEntityDefDefines_H__

#ifdef FV_AS_STATIC_LIBS
#define FV_ENTITYDEF_API
#else
#ifdef FV_ENTITYDEF_EXPORT
#define FV_ENTITYDEF_API FV_Proper_Export_Flag
#else
#define FV_ENTITYDEF_API FV_Proper_Import_Flag
#endif
#endif

#endif // __FvEntityDefDefines_H__
