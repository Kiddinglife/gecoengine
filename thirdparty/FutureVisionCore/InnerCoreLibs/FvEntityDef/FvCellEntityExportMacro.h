//{future header message}
#ifndef __FvCellEntityExportMacro_H__
#define __FvCellEntityExportMacro_H__

#include "FvEntityDefUtility.h"



#define FV_INIT_REF(_TYPE, _NAME)			_NAME(*(_TYPE*)0)



#define DEFINE_ENTITY_START(_CLASS, _PARENT)\
	I##_CLASS::I##_CLASS()					\
	:_PARENT()

#define INIT_REF(_TYPE, _NAME)	,FV_INIT_REF(_TYPE, m_##_NAME)

#define DEFINE_ENTITY_END {}



#define DEFINE_INIT_ATTRIB_START(_CLASS)														\
	void I##_CLASS::InitAttrib(const FvAllData* pkAllData)										\
	{																							\
		const _CLASS##AllData* pkMyAllData = static_cast<const _CLASS##AllData*>(pkAllData);

#define INIT_ATTRIB(_MEMBER)	m_##_MEMBER = pkMyAllData->m_##_MEMBER;

#define DEFINE_INIT_ATTRIB_END	}



#define DEFINE_REALDATA_START(_CLASS)				\
	struct RealData_of_##_CLASS : public FvObjStruct\
	{

#define REALDATA(_TYPE, _MEMBER)	_TYPE m_##_MEMBER;

#define DEFINE_REALDATA_END																					\
		protected:																							\
		virtual void GetStructAddr(FvUInt32& uiCnt, NameAddr*& pkInfo) const;								\
		virtual bool GetPtToID(FvUInt32& uiCnt, FvUInt32*& pBits, FvUInt16*& pIdx, PtIdxToID*& pIDs) const;	\
	};



#define DEFINE_REALDATA_ADDR_START(_CNT)\
	static NameAddr s_kRealDataAddr[_CNT<=0 ? 1 : _CNT] = {

#define REALDATA_ADDR(_CLASS, _MEMBER) { #_MEMBER, FV_OFFSET_OF(RealData_of_##_CLASS, m_##_MEMBER) },

#define DEFINE_REALDATA_ADDR_END };



#define BITSCNT(_CLASS, _LAST)	((FV_OFFSET_OF(_CLASS,_LAST)>>7)+1)

#define DEFINE_REALDATA_IDX_TO_ID(_CNT)	\
	static PtIdxToID s_uiRealDataIdxToID[_CNT<=0 ? 1 : _CNT]

#define DEFINE_REALDATA_EXPORT_START(_CLASS, _CNT, _LASTMEMBER)						\
	static FvUInt32 s_uiRealDataBitsCnt = BITSCNT(RealData_of_##_CLASS, _LASTMEMBER);\
	static FvUInt32 s_uiRealDataBits[BITSCNT(RealData_of_##_CLASS, _LASTMEMBER)];	\
	static FvUInt16 s_uiRealDataBitsIdx[BITSCNT(RealData_of_##_CLASS, _LASTMEMBER)];\
	static FvUInt32 s_kAllRealDataAddrCnt = _CNT;									\
	static FvUInt32 s_kAllRealDataAddr[_CNT] = {

#define REALDATA_EXPORT(_CLASS, _MEMBER)	FV_OFFSET_OF(RealData_of_##_CLASS, _MEMBER),

#define DEFINE_REALDATA_EXPORT_END(_CLASS, _CNT)										\
	};																					\
	void RealData_of_##_CLASS::GetStructAddr(FvUInt32& uiCnt, NameAddr*& pkInfo) const	\
	{																					\
		uiCnt = _CNT;																	\
		pkInfo = s_kRealDataAddr;														\
	}																					\
	bool RealData_of_##_CLASS::GetPtToID(FvUInt32& uiCnt, FvUInt32*& pBits, FvUInt16*& pIdx, PtIdxToID*& pIDs) const\
	{																					\
		uiCnt = s_uiRealDataBitsCnt;													\
		pBits = s_uiRealDataBits;														\
		pIdx = s_uiRealDataBitsIdx;														\
		pIDs = s_uiRealDataIdxToID;														\
		return true;																	\
	}

#define DEFINE_REALDATA_EXPORT_NULL(_CLASS, _CNT)										\
	static FvUInt32 s_uiRealDataBitsCnt = 0;											\
	static FvUInt32 s_uiRealDataBits[1];												\
	static FvUInt16 s_uiRealDataBitsIdx[1];												\
	static FvUInt32 s_kAllRealDataAddrCnt = 0;											\
	static FvUInt32 s_kAllRealDataAddr[1];												\
	void RealData_of_##_CLASS::GetStructAddr(FvUInt32& uiCnt, NameAddr*& pkInfo) const	\
	{																					\
		uiCnt = _CNT;																	\
		pkInfo = s_kRealDataAddr;														\
	}																					\
	bool RealData_of_##_CLASS::GetPtToID(FvUInt32& uiCnt, FvUInt32*& pBits, FvUInt16*& pIdx, PtIdxToID*& pIDs) const\
	{																					\
		uiCnt = s_uiRealDataBitsCnt;													\
		pBits = s_uiRealDataBits;														\
		pIdx = s_uiRealDataBitsIdx;														\
		pIDs = s_uiRealDataIdxToID;														\
		return true;																	\
	}


#define DEFINE_CREATE_FUNC(_CLASS)													\
	FvEntity* EntityCreateFunc_of_##_CLASS() { return new _CLASS(); }				\
	static FvObjStruct* RealDataCreateFunc() { return new RealData_of_##_CLASS(); }

#define DEFINE_CREATE_FUNC_FOR_ABSTRACT(_CLASS)										\
	FvEntity* EntityCreateFunc_of_##_CLASS() { return NULL; }						\
	static FvObjStruct* RealDataCreateFunc() { return new RealData_of_##_CLASS(); }


#define DEFINE_ATTRIBINFO_START(_CNT)	static AttribInfo s_AttribInfo[_CNT<=0 ? 1 : _CNT] = {
#define ATTRIBINFO(_CLASS, _MEMBER, _DATAFLG, _ISREF)	{ FV_OFFSET_OF(CellEntityMirror_of_##_CLASS, m_##_MEMBER), _DATAFLG, _ISREF },
#define DEFINE_ATTRIBINFO_END	};



#define DEFINE_GHOST_ADDR_START(_CNT)	static FvUInt32 s_uiGhostAddr[_CNT<=0 ? 1 : _CNT] = {
#define GHOST_ADDR(_CLASS, _MEMBER)		FV_OFFSET_OF(CellEntityMirror_of_##_CLASS, m_##_MEMBER),
#define DEFINE_GHOST_ADDR_END	};



#define DEFINE_REAL_ADDR_START(_CNT)	static FvUInt32 s_uiRealAddr[_CNT<=0 ? 1 : _CNT] = {
#define REAL_ADDR(_CLASS, _MEMBER)		FV_OFFSET_OF(CellEntityMirror_of_##_CLASS, m_##_MEMBER),
#define DEFINE_REAL_ADDR_END	};



#define DEFINE_DB_TO_CELL(_CNT)			static FvUInt16 s_DBToCell[_CNT<=0 ? 1 : _CNT]
#define DEFINE_CELL_TO_OWNCLIENT(_CNT)	static FvUInt16 s_CellToOwnClient[_CNT<=0 ? 1 : _CNT]
#define DEFINE_CELL_EXP_ID(_CNT)		static MethodExpID s_CellExpID[_CNT<=0 ? 1 : _CNT]



#define DEFINE_CELL_TO_OTHERCLIENT_START(_CNT)	static SyncToClientIdx s_CellToOtherClient[_CNT<=0 ? 1 : _CNT] = {
#define CELL_TO_OTHERCLIENT(_SvrIdx, _CliIdx)	{ _SvrIdx, _CliIdx },
#define DEFINE_CELL_TO_OTHERCLIENT_END	};



#define DEFINE_DATAPATH_NULL								static FvUInt16 s_DataPath_NULL[] = {0};
#define DEFINE_DATAPATH(_ID)								static FvUInt16 s_DataPath_##_ID[]
#define DEFINE_DATAPATH_START(_CNT)							static DataPath s_DataPath[_CNT<=0 ? 1 : _CNT] = {
#define DATAPATH(_CLASS, _MEMBER, _ID, _IDX, _PACKID, _LEN)	{ FV_OFFSET_OF(CellEntityMirror_of_##_CLASS, _MEMBER), s_DataPath_##_ID, _IDX, _PACKID, _LEN },
#define DATAPATH_NULL										{ 0, s_DataPath_NULL, 0xFFFF, 0xFFFF, 0 },
#define DATAPATH_REAL(_IDX)									{ 0, s_DataPath_NULL, _IDX, 0xFFFF, 0 },
#define DEFINE_DATAPATH_END									};



#define DEFINE_GHOST_IDX_TO_ID(_CNT) \
	static PtIdxToID s_uiGhostIdxToID[_CNT<=0 ? 1 : _CNT]

#define DEFINE_GHOST_EXPORT_START(_CLASS, _CNT, _LASTMEMBER)								\
	static FvUInt32 s_uiGhostBitsCnt = BITSCNT(CellEntityMirror_of_##_CLASS, _LASTMEMBER);	\
	static FvUInt32 s_uiGhostBits[BITSCNT(CellEntityMirror_of_##_CLASS, _LASTMEMBER)];		\
	static FvUInt16 s_uiGhostBitsIdx[BITSCNT(CellEntityMirror_of_##_CLASS, _LASTMEMBER)];	\
	static FvUInt32 s_kAllGhostDataAddrCnt = _CNT;											\
	static FvUInt32 s_kAllGhostDataAddr[_CNT] = {

#define GHOST_EXPORT(_CLASS, _MEMBER)	FV_OFFSET_OF(CellEntityMirror_of_##_CLASS, _MEMBER),

#define DEFINE_GHOST_EXPORT_END	};

#define DEFINE_GHOST_EXPORT_NULL				\
	static FvUInt32 s_uiGhostBitsCnt = 0;		\
	static FvUInt32 s_uiGhostBits[1];			\
	static FvUInt16 s_uiGhostBitsIdx[1];		\
	static FvUInt32 s_kAllGhostDataAddrCnt = 0;	\
	static FvUInt32 s_kAllGhostDataAddr[1];



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



#define DEFINE_GETPACKPROPERTY_START(_CLASS)					\
	FvDataObj* I##_CLASS::GetPackProperty(FvUInt32 uiIdx) const	\
	{															\
		const FvDataObj* pkObj = NULL;							\
		if(IsReal())											\
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



#define DEFINE_ENTITY_EXPORT(_PREFIX, _CLASS, _TYPE, _HAS_BASE, _HAS_CELL, _HAS_CLI,	\
									_MEMBER_CNT, _REAL_CNT, _GHOST_CNT,					\
									_BASE_CALL_EXPCNT, _CELL_CALL_EXPCNT, _CLI_CALL_CNT,\
									_DB_TO_CELL_CNT, _CELL_TO_OWNCLI_CNT, _CELL_TO_OTHERCLI_CNT,\
									_DATAID_CNT)										\
	static FvCellEntityExport s_kExport;												\
	_PREFIX void EntityExport_of_##_CLASS(void*& pkEntityExport)						\
	{																					\
		pkEntityExport = &s_kExport;													\
																						\
		s_kExport.pName = #_CLASS;														\
		s_kExport.uiTypeID = _TYPE;														\
																						\
		s_kExport.pFunCreateEntity = EntityCreateFunc_of_##_CLASS;						\
		s_kExport.pFunCreateRealData = RealDataCreateFunc;								\
		s_kExport.pFunEntityMethodsEntry = EntityMethodsEntry_of_##_CLASS;				\
																						\
		s_kExport.kForm.bHasBase = _HAS_BASE;											\
		s_kExport.kForm.bHasCell = _HAS_CELL;											\
		s_kExport.kForm.bHasClient = _HAS_CLI;											\
																						\
		s_kExport.kAttribInfo.uiCnt = _MEMBER_CNT;										\
		s_kExport.kAttribInfo.pkInfo = s_AttribInfo;									\
																						\
		s_kExport.kAddrs.uiRealDataCnt = _REAL_CNT;										\
		s_kExport.kAddrs.uiGhostDataCnt = _GHOST_CNT;									\
		s_kExport.kAddrs.pkRealAddrs = s_uiRealAddr;									\
		s_kExport.kAddrs.pkGhostAddrs = s_uiGhostAddr;									\
																						\
		s_kExport.kDBToCellIdx.uiCnt = _DB_TO_CELL_CNT;									\
		s_kExport.kDBToCellIdx.pkAttribIdx = s_DBToCell;								\
																						\
		s_kExport.kCellToCli.uiCnt = _CELL_TO_OWNCLI_CNT;								\
		s_kExport.kCellToCli.pkAttribIdx = s_CellToOwnClient;							\
																						\
		s_kExport.kCellToOthersCli.uiCnt = _CELL_TO_OTHERCLI_CNT;						\
		s_kExport.kCellToOthersCli.pkAttribIdx = s_CellToOtherClient;					\
																						\
		s_kExport.kMethodCnts.uiBaseExpCnt = _BASE_CALL_EXPCNT;							\
		s_kExport.kMethodCnts.uiCellExpCnt = _CELL_CALL_EXPCNT;							\
		s_kExport.kMethodCnts.uiClientCnt = _CLI_CALL_CNT;								\
		s_kExport.kMethodCnts.pkBaseExpID = NULL;										\
		s_kExport.kMethodCnts.pkCellExpID = s_CellExpID;								\
																						\
		s_kExport.kDataIDToPath.uiDataIDCnt = _DATAID_CNT;								\
		s_kExport.kDataIDToPath.pkDataPath = s_DataPath;								\
																						\
		s_kExport.kPtToIdxToDataID.uiBitsCnt = s_uiGhostBitsCnt;						\
		s_kExport.kPtToIdxToDataID.pkBits = s_uiGhostBits;								\
		s_kExport.kPtToIdxToDataID.pkIdxes = s_uiGhostBitsIdx;							\
		s_kExport.kPtToIdxToDataID.pkIdxToID = s_uiGhostIdxToID;						\
																						\
		s_kExport.pkDataCallBacks = s_kDataCallBackInfos;								\
																						\
		FvMakePtToIdxData(s_uiRealDataBits,												\
			s_uiRealDataBitsCnt,														\
			s_uiRealDataBitsIdx,														\
			s_kAllRealDataAddrCnt,														\
			s_kAllRealDataAddr);														\
																						\
		FvMakePtToIdxData(s_uiGhostBits,												\
			s_uiGhostBitsCnt,															\
			s_uiGhostBitsIdx,															\
			s_kAllGhostDataAddrCnt,														\
			s_kAllGhostDataAddr);														\
	}



#endif//__FvCellEntityExportMacro_H__
