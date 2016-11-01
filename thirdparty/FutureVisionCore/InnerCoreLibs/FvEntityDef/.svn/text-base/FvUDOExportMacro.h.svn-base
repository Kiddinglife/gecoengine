//{future header message}
#ifndef __FvUDOExportMacro_H__
#define __FvUDOExportMacro_H__



#define EXTERN_CREATE_UDO(_PREFIX, _NAME)\
	extern _PREFIX FvUDOAttrib* CreateUDO_of_##_NAME();


#define CREATE_UDO(_PREFIX, _NAME)	\
	_PREFIX FvUDOAttrib* CreateUDO_of_##_NAME() { return new _NAME(); }


#define DEFINE_UDOEXPORT_START(_CNT)						\
	static AllUDOExports s_kAllUDOExports;					\
	static UDOExport s_kUDOExports[_CNT <= 0 ? 1 : _CNT] = {


#define UDOEXPORT(_NAME)	{ #_NAME, CreateUDO_of_##_NAME },


#define DEFINE_UDOEXPORT_END(_CNT)							\
	};														\
	void UDOExportFunc(AllUDOExports*& pkAllUDOExports)		\
	{														\
		pkAllUDOExports = &s_kAllUDOExports;				\
		s_kAllUDOExports.uiCnt = _CNT;						\
		s_kAllUDOExports.pkUDOExport = s_kUDOExports;		\
	}


#endif//__FvUDOExportMacro_H__
