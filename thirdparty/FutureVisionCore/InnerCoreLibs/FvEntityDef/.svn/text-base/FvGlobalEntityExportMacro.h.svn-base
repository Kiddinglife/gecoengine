//{future header message}
#ifndef __FvGlobalEntityExportMacro_H__
#define __FvGlobalEntityExportMacro_H__

#include "FvEntityDefUtility.h"



#define DEFINE_CREATE_FUNC(_CLASS)	\
	FvEntity* EntityCreateFunc_of_##_CLASS() { return new _CLASS(); }

#define DEFINE_CREATE_FUNC_FOR_ABSTRACT(_CLASS)	\
	FvEntity* EntityCreateFunc_of_##_CLASS() { return NULL; }



#define DEFINE_ENTITY_EXPORT(_PREFIX, _CLASS, _TYPE)						\
	static FvGlobalEntityExport s_kExport;									\
	_PREFIX void EntityExport_of_##_CLASS(void*& pkEntityExport)			\
	{																		\
		pkEntityExport = &s_kExport;										\
																			\
		s_kExport.pName = #_CLASS;											\
		s_kExport.uiTypeID = _TYPE;											\
																			\
		s_kExport.pFunCreateEntity = EntityCreateFunc_of_##_CLASS;			\
		s_kExport.pFunEntityMethodsEntry = EntityMethodsEntry_of_##_CLASS;	\
	}



#endif//__FvGlobalEntityExportMacro_H__
