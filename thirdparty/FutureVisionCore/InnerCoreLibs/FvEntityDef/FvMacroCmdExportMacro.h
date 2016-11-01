//{future header message}
#ifndef __FvMacroCmdExportMacro_H__
#define __FvMacroCmdExportMacro_H__



#define DEFINE_COMMON_MACRO_CMD_EXPORT_START(_PREFIX)							\
	_PREFIX void CommonMacroCmdExportFunc(LocalCallFuncByStrHandlerMap& kMap)	\
	{

#define DEFINE_COMMON_MACRO_CMD_EXPORT_END }

#define DEFINE_MACRO_CMD_EXPORT_START							\
	void MacroCmdExportFunc(LocalCallFuncByStrHandlerMap& kMap)	\
	{

#define DEFINE_MACRO_CMD_EXPORT_END }

#define EXPORT_MACRO_CMD(_CMD, _PROTOTYPE, _HANDLER)	\
	kMap[_CMD].push_back(std::make_pair(_PROTOTYPE, (FvLocalCallFuncByStrHandler*)(&g_k##_HANDLER##_StrHandler)));



#endif//__FvMacroCmdExportMacro_H__
