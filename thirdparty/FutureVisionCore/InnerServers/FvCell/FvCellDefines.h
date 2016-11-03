//{future header message}
#ifndef __FvCellDefines_H__
#define __FvCellDefines_H__


#ifdef FV_AS_STATIC_LIBS
#define FV_CELL_API
#else
#ifdef FV_CELL_EXPORT
#define FV_CELL_API FV_Proper_Export_Flag
#else
#define FV_CELL_API FV_Proper_Import_Flag
#endif
#endif

#endif//__FvCellDefines_H__
