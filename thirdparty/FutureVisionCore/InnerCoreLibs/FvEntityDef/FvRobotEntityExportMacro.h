//{future header message}
#ifndef __FvRobotEntityExportMacro_H__
#define __FvRobotEntityExportMacro_H__

#include "FvEntityDefUtility.h"


#define FV_INIT_REF(_TYPE, _NAME)			_NAME(*(_TYPE*)0)



#define DEFINE_ENTITY_START(_PARENT, _CLASS)\
	I##_CLASS::I##_CLASS()					\
	:_PARENT()


#define INIT_REF(_TYPE, _NAME)	,FV_INIT_REF(_TYPE, m_##_NAME)


#define DEFINE_ENTITY_END {}


#define MEMBER(_TYPE, _NAME)	_TYPE m_##_NAME;


#define DEFINE_OWN_CLIENTDATA_START(_CLASS)									\
	FvUInt32 s_OwnClientDataAddr[];											\
	class OwnClientData_of_##_CLASS : public FvClientData					\
	{																		\
	public:


#define DEFINE_OWN_CLIENTDATA_END(_CLASS, _CNT)								\
	protected:																\
		virtual void GetDataAddr(FvUInt32& uiCnt, FvUInt32*& pAddr) const	\
		{																	\
			uiCnt = _CNT;													\
			pAddr = s_OwnClientDataAddr;									\
		}																	\
	};


#define DEFINE_OWN_CLIENTDATA_ADDR_START(_CNT) \
	static FvUInt32 s_OwnClientDataAddr[_CNT<=0 ? 1 : _CNT] = {


#define OWN_ADDR(_CLASS, _MEMBER) FV_OFFSET_OF(OwnClientData_of_##_CLASS, m_##_MEMBER),


#define DEFINE_OWN_CLIENTDATA_ADDR_END };


#define DEFINE_OTHERS_CLIENTDATA_START(_CLASS)								\
	FvUInt32 s_OthersClientDataAddr[];										\
	class OthersClientData_of_##_CLASS : public FvClientData				\
	{																		\
	public:


#define DEFINE_OTHERS_CLIENTDATA_END(_CLASS, _CNT)							\
	protected:																\
		virtual void GetDataAddr(FvUInt32& uiCnt, FvUInt32*& pAddr) const	\
		{																	\
			uiCnt = _CNT;													\
			pAddr = s_OthersClientDataAddr;									\
		}																	\
	};


#define DEFINE_OTHERS_CLIENTDATA_ADDR_START(_CNT)	\
	static FvUInt32 s_OthersClientDataAddr[_CNT<=0 ? 1 : _CNT] = {


#define OTHERS_ADDR(_CLASS, _MEMBER)	FV_OFFSET_OF(OthersClientData_of_##_CLASS, m_##_MEMBER),


#define DEFINE_OTHERS_CLIENTDATA_ADDR_END };


#define DEFINE_CREATE_FUNC(_CLASS)																	\
	FvEntity* EntityCreateFunc_of_##_CLASS() { return new _CLASS(); }								\
	static FvClientData* OwnClientDataCreateFunc() { return new OwnClientData_of_##_CLASS(); }		\
	static FvClientData* OthersClientDataCreateFunc() { return new OthersClientData_of_##_CLASS(); }

#define DEFINE_CREATE_FUNC_FOR_ABSTRACT(_CLASS)														\
	FvEntity* EntityCreateFunc_of_##_CLASS() { return NULL; }										\
	static FvClientData* OwnClientDataCreateFunc() { return new OwnClientData_of_##_CLASS(); }		\
	static FvClientData* OthersClientDataCreateFunc() { return new OthersClientData_of_##_CLASS(); }


#define DEFINE_OWN_DATAINDEX(_CNT)				static FvUInt16 s_OwnDataIdx[_CNT<=0 ? 1 : _CNT]
#define DEFINE_OTHERS_DATAINDEX(_CNT)			static FvUInt16 s_OthersDataIdx[_CNT<=0 ? 1 : _CNT]
#define DEFINE_BASE_TO_CLIENT_INDEX(_CNT)		static FvUInt16 s_BaseToCliIdx[_CNT<=0 ? 1 : _CNT]
#define DEFINE_CELL_TO_OWN_CLIENT_INDEX(_CNT)	static FvUInt16 s_CellToOwnCli[_CNT<=0 ? 1 : _CNT]


#define DEFINE_OWN_DATAPATH_NULL							static FvUInt16 s_OwnDataPath_NULL[] = {0};
#define DEFINE_OWN_DATAPATH(_ID)							static FvUInt16 s_OwnDataPath_##_ID[]
#define DEFINE_OWN_DATAPATH_START(_CNT)						static DataPath s_OwnDataPath[_CNT<=0 ? 1 : _CNT] = {
#define OWN_DATAPATH(_CLASS, _NAME, _ID, _PACKID, _LEN)		{ FV_OFFSET_OF(OwnClientData_of_##_CLASS, _NAME), s_OwnDataPath_##_ID, 0xFFFF, _PACKID, _LEN },
#define OWN_DATAPATH_NULL									{ 0, s_OwnDataPath_NULL, 0xFFFF, 0xFFFF, 0 },
#define DEFINE_OWN_DATAPATH_END								};


#define DEFINE_OTHERS_DATAPATH_NULL							static FvUInt16 s_OthersDataPath_NULL[] = {0};
#define DEFINE_OTHERS_DATAPATH(_ID)							static FvUInt16 s_OthersDataPath_##_ID[]
#define DEFINE_OTHERS_DATAPATH_START(_CNT)					static DataPath s_OthersDataPath[_CNT<=0 ? 1 : _CNT] = {
#define OTHERS_DATAPATH(_CLASS, _NAME, _ID, _PACKID, _LEN)	{ FV_OFFSET_OF(OthersClientData_of_##_CLASS, _NAME), s_OthersDataPath_##_ID, 0xFFFF, _PACKID, _LEN },
#define OTHERS_DATAPATH_NULL								{ 0, s_OthersDataPath_NULL, 0xFFFF, 0xFFFF, 0 },
#define DEFINE_OTHERS_DATAPATH_END							};


#define DEFINE_REF_ADDR_START(_CNT)		static FvUInt32 s_uiRefAddr[_CNT<=0 ? 1 : _CNT] = {
#define REF_ADDR(_CLASS, _MEMBER)		FV_OFFSET_OF(ClientEntityMirror_of_##_CLASS, m_##_MEMBER),
#define DEFINE_REF_ADDR_END				};


#define DEFINE_PACKDATACB(_CLASS, _FUNC)\
	static FvDataCallBackInEntity<_CLASS, &_CLASS::_FUNC> s_kPackDataCallBack;

#define DEFINE_DATACB_INCLASS(_ID, _CLASS, _FUNC)\
	static FvDataCallBackInEntity<_CLASS, &_CLASS::_FUNC> s_kDataCallBack_##_ID;

#define DEFINE_DATACB_INLOGIC(_ID, _CLASS, _LOGIC, _FUNC)\
	static FvDataCallBackInLogic<_CLASS, _LOGIC, &_CLASS::GetLogic, &_LOGIC::_FUNC> s_kDataCallBack_##_ID;

#define DEFINE_DATACB_EXPORT_START(_CNT)\
	static DataCallBackInfo s_kDataCallBackInfos[_CNT<=0 ? 1 : _CNT] = {

#define DATACB(_ID, _TYPE)	{ &s_kDataCallBack_##_ID, _ID, _TYPE },
#define PACKDATACB			{ &s_kPackDataCallBack, 0xFFFF, 4 },
#define DATACB_NULL			{ NULL, 0xFFFF, 0 },

#define DEFINE_DATACB_EXPORT_END };


#define DEFINE_PACK_DATACB(_CLASS, _FUNC)


#define DEFINE_GETPACKPROPERTY_START(_CLASS)					\
	FvDataObj* I##_CLASS::GetPackProperty(FvUInt32 uiIdx) const	\
	{															\
		const FvDataObj* pkObj = NULL;							\
		if(IsCtrler())											\
		{

#define DEFINE_GETPACKPROPERTY_MIDDLE	\
		}								\
		else							\
		{

#define DEFINE_GETPACKPROPERTY_END				\
		}										\
		return const_cast<FvDataObj*>(pkObj);	\
	}

#define GETPACKPROPERTY_SWITCH_START\
	switch(uiIdx)					\
	{

#define GETPACKPROPERTY(_CLASS, _NAME, _MEMBER)	\
	case _CLASS##PackProperty::_NAME:			\
		pkObj = &_MEMBER;						\
		break;

#define GETPACKPROPERTY_SWITCH_END	\
	default:						\
		break;						\
	}


#define DEFINE_ENTITY_EXPORT(_PREFIX, _CLASS, _TYPE, _HAS_BASE, _HAS_CELL, _HAS_CLI, _MEMBER_CNT,\
									_BASE_CALL_EXPCNT, _CELL_CALL_EXPCNT, _CLI_CALL_CNT,		\
									_BASE_TO_CLI_CNT, _CELL_TO_OWNCLI_CNT, _DATAID_CNT)			\
	static FvClientEntityExport s_kExport;														\
	_PREFIX void EntityExport_of_##_CLASS(void*& pkEntityExport)								\
	{																							\
		pkEntityExport = &s_kExport;															\
																								\
		s_kExport.pName = #_CLASS;																\
		s_kExport.uiTypeID = _TYPE;																\
																								\
		s_kExport.pFunCreateEntity = EntityCreateFunc_of_##_CLASS;								\
		s_kExport.pFunCreateOwnData = OwnClientDataCreateFunc;									\
		s_kExport.pFunCreateOthersData = OthersClientDataCreateFunc;							\
		s_kExport.pFunEntityMethodsEntry = EntityMethodsEntry_of_##_CLASS;						\
																								\
		s_kExport.kForm.bHasBase = _HAS_BASE;													\
		s_kExport.kForm.bHasCell = _HAS_CELL;													\
		s_kExport.kForm.bHasClient = _HAS_CLI;													\
																								\
		s_kExport.kAttribAddr.pkRefAddr = s_uiRefAddr;											\
		s_kExport.kAttribAddr.uiRefCnt = _MEMBER_CNT;											\
		s_kExport.kAttribAddr.pkOwnIdx = s_OwnDataIdx;											\
		s_kExport.kAttribAddr.pkOthersIdx = s_OthersDataIdx;									\
																								\
		s_kExport.kMethodCnts.uiBaseExpCnt = _BASE_CALL_EXPCNT;									\
		s_kExport.kMethodCnts.uiCellExpCnt = _CELL_CALL_EXPCNT;									\
		s_kExport.kMethodCnts.uiClientCnt = _CLI_CALL_CNT;										\
		s_kExport.kMethodCnts.pkBaseExpID = NULL;												\
		s_kExport.kMethodCnts.pkCellExpID = NULL;												\
																								\
		s_kExport.kBaseToCli.uiCnt = _BASE_TO_CLI_CNT;											\
		s_kExport.kBaseToCli.pkAttribIdx = s_BaseToCliIdx;										\
																								\
		s_kExport.kCellToOwnCli.uiCnt = _CELL_TO_OWNCLI_CNT;									\
		s_kExport.kCellToOwnCli.pkAttribIdx = s_CellToOwnCli;									\
																								\
		s_kExport.kDataIDToPath.uiDataIDCnt = _DATAID_CNT;										\
		s_kExport.kDataIDToPath.pkOwnDataPath = s_OwnDataPath;									\
		s_kExport.kDataIDToPath.pkOthersDataPath = s_OthersDataPath;							\
																								\
		s_kExport.pkDataCallBacks = s_kDataCallBackInfos;										\
	}


#endif//__FvRobotEntityExportMacro_H__
