//{future header message}
#ifndef __FvFDBStorageDefine_H__
#define __FvFDBStorageDefine_H__

#ifdef FV_AS_STATIC_LIBS
#define FV_FDBSTORAGE_API
#else
#ifdef FV_FDBSTORAGE_EXPORT
#define FV_FDBSTORAGE_API FV_Proper_Export_Flag
#else
#define FV_FDBSTORAGE_API FV_Proper_Import_Flag
#endif
#endif

#endif // __FvFDBStorageDefine_H__