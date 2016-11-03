//{future header message}
#ifndef __FvBaseEntityExportMacro_H__
#define __FvBaseEntityExportMacro_H__

#include "FvEntityDefUtility.h"



#define DEFINE_CELLDATA_INIT_DEFAULTVAL_FOR_NODB_DATA_START(_CLASS, _PARENT)\
	void _CLASS##CellData::InitDefaultValForNoDBData()						\
	{																		\
		_PARENT::InitDefaultValForNoDBData();

#define DEFINE_CELLDATA_INIT_DEFAULTVAL_FOR_NODB_DATA_END	}



#define DEFINE_CELLDATA_SERIALIZE_INDEX_START(_CLASS)											\
	bool _CLASS##CellData::SerializeToStreamByIdx(FvUInt16 uiIdx, FvBinaryOStream& kOS) const	\
	{																							\
		switch(uiIdx)																			\
		{

#define CELLDATA_SERIALIZE_INDEX(_IDX, _MEMBER)	\
		case _IDX:								\
			kOS << m_##_MEMBER;					\
			break;

#define DEFINE_CELLDATA_SERIALIZE_INDEX_END		\
		default:								\
			FV_ASSERT(0 && "AttribIdx Overflow");\
			break;								\
		}										\
		return true;							\
	}

#define DEFINE_CELLDATA_SERIALIZE_INDEX_START_NULL(_CLASS)										\
	bool _CLASS##CellData::SerializeToStreamByIdx(FvUInt16 uiIdx, FvBinaryOStream& kOS) const	\
	{

#define DEFINE_CELLDATA_SERIALIZE_INDEX_END_NULL\
		FV_ASSERT(0 && "AttribIdx Overflow");	\
		return true;							\
	}



#define CELLDATA_SERIALIZE(_MEMBER)	<< m_##_MEMBER

#define DEFINE_CELLDATA_SERIALIZE_DB_START(_CLASS)									\
	bool _CLASS##CellData::SerializeToStreamForDBData(FvBinaryOStream& kOS) const	\
	{																				\
		kOS

#define DEFINE_CELLDATA_SERIALIZE_DB_END					\
		<< m_kPos << m_kDir << m_iSpaceID << m_uiSpaceType;	\
		return true;										\
	}



#define DEFINE_CELLDATA_SERIALIZE_ALL_START(_CLASS)									\
	bool _CLASS##CellData::SerializeToStreamForAllData(FvBinaryOStream& kOS) const	\
	{																				\
		kOS

#define DEFINE_CELLDATA_SERIALIZE_ALL_END	\
		;return true;						\
	}



#define DEFINE_CELLDATA_DESERIALIZE_INDEX_START(_CLASS)										\
	bool _CLASS##CellData::DeserializeFromStreamByIdx(FvUInt16 uiIdx, FvBinaryIStream& kIS)	\
	{																						\
		switch(uiIdx)																		\
		{

#define CELLDATA_DESERIALIZE_INDEX(_IDX, _MEMBER)\
		case _IDX:								\
			kIS >> m_##_MEMBER;					\
			break;

#define DEFINE_CELLDATA_DESERIALIZE_INDEX_END		\
		default:									\
			FV_ASSERT(0 && "AttribIdx Overflow");	\
			break;									\
		}											\
		FV_ASSERT(!kIS.Error());					\
		return true;								\
	}

#define DEFINE_CELLDATA_DESERIALIZE_INDEX_START_NULL(_CLASS)								\
	bool _CLASS##CellData::DeserializeFromStreamByIdx(FvUInt16 uiIdx, FvBinaryIStream& kIS)	\
	{

#define DEFINE_CELLDATA_DESERIALIZE_INDEX_END_NULL	\
		FV_ASSERT(0 && "AttribIdx Overflow");		\
		return true;								\
	}



#define CELLDATA_DESERIALIZE(_MEMBER)	>> m_##_MEMBER

#define DEFINE_CELLDATA_DESERIALIZE_DB_START(_CLASS)							\
	bool _CLASS##CellData::DeserializeFromStreamForDBData(FvBinaryIStream& kIS)	\
	{																			\
		kIS

#define DEFINE_CELLDATA_DESERIALIZE_DB_END					\
		>> m_kPos >> m_kDir >> m_iSpaceID >> m_uiSpaceType;	\
		FV_ASSERT(!kIS.Error());							\
		return true;										\
	}



#define DEFINE_CELLDATA_DESERIALIZE_ALL_START(_CLASS)							\
	bool _CLASS##CellData::DeserializeFromStreamForAllData(FvBinaryIStream& kIS)\
	{																			\
		kIS

#define DEFINE_CELLDATA_DESERIALIZE_ALL_END	\
		;FV_ASSERT(!kIS.Error());			\
		return true;						\
	}



#define DEFINE_ATTRIBINFO_START(_CNT)	static AttribInfo s_AttribInfo[_CNT<=0 ? 1 : _CNT] = {
#define ATTRIBINFO(_CLASS, _MEMBER, _DATAFLG)	{ FV_OFFSET_OF(BaseEntityMirror_of_##_CLASS, m_##_MEMBER), _DATAFLG, 0 },
#define DEFINE_ATTRIBINFO_END	};



#define DEFINE_BASE_ATTRIBIDXINFO_START(_CNT)	static BaseAttribIdxInfo s_BaseAttribIdxInfo[_CNT<=0 ? 1 : _CNT] = {
#define ATTRIBIDXINFO(_ISBASE, _IDX)	{ _ISBASE, _IDX },
#define DEFINE_BASE_ATTRIBIDXINFO_END	};



#define DEFINE_DB_TO_BASE(_CNT)			static FvUInt16 s_DBToBase[_CNT<=0 ? 1 : _CNT]
#define DEFINE_BASE_TO_CLIENT(_CNT)		static FvUInt16 s_BaseToClient[_CNT<=0 ? 1 : _CNT]
#define DEFINE_BASE_EXP_ID(_CNT)		static MethodExpID s_BaseExpID[_CNT<=0 ? 1 : _CNT]



#define BITSCNT(_CLASS, _LAST)	((FV_OFFSET_OF(BaseEntityMirror_of_##_CLASS,_LAST)>>7)+1)

#define DEFINE_INDEX_TO_ID(_CNT) \
	static PtIdxToID s_uiIdxToID[_CNT<=0 ? 1 : _CNT]

#define DEFINE_POINTER_TO_INDEX_EXPORT_START(_CLASS, _CNT, _LASTMEMBER)	\
	static FvUInt32 s_uiBitsCnt = BITSCNT(_CLASS, _LASTMEMBER);			\
	static FvUInt32 s_uiBits[BITSCNT(_CLASS, _LASTMEMBER)];				\
	static FvUInt16 s_uiBitsIdx[BITSCNT(_CLASS, _LASTMEMBER)];			\
	static FvUInt32 s_uiMemberAddrCnt = _CNT;							\
	static FvUInt32 s_uiMemberAddr[_CNT] = {

#define POINTER_TO_INDEX_EXPORT(_CLASS, _MEMBER)	FV_OFFSET_OF(BaseEntityMirror_of_##_CLASS, _MEMBER),

#define DEFINE_POINTER_TO_INDEX_EXPORT_END };

#define DEFINE_POINTER_TO_INDEX_EXPORT_NULL	\
	static FvUInt32 s_uiBitsCnt = 0;		\
	static FvUInt32 s_uiBits[1];			\
	static FvUInt16 s_uiBitsIdx[1];			\
	static FvUInt32 s_uiMemberAddrCnt = 0;	\
	static FvUInt32 s_uiMemberAddr[1];



#define DEFINE_CREATE_FUNC(_CLASS)												\
	FvEntity* EntityCreateFunc_of_##_CLASS() { return new _CLASS(); }			\
	static FvCellData* CellDataCreateFunc() { return new _CLASS##CellData(); }

#define DEFINE_CREATE_FUNC_FOR_ABSTRACT(_CLASS)									\
	FvEntity* EntityCreateFunc_of_##_CLASS() { return NULL; }					\
	static FvCellData* CellDataCreateFunc() { return new _CLASS##CellData(); }



#define DEFINE_INIT_ATTRIB_START(_CLASS)													\
	void I##_CLASS::InitAttrib(const FvAllData* pkAllData, FvCellData* pkCellData)			\
	{																						\
		const _CLASS##AllData* pkMyAllData = static_cast<const _CLASS##AllData*>(pkAllData);\
		_CLASS##CellData* pkMyCellData = static_cast<_CLASS##CellData*>(pkCellData);

#define INIT_ATTRIB_BASE(_MEMBER)	m_##_MEMBER = pkMyAllData->m_##_MEMBER;
#define INIT_ATTRIB_CELL(_MEMBER)	pkMyCellData->m_##_MEMBER = pkMyAllData->m_##_MEMBER;

#define DEFINE_INIT_ATTRIB_END										\
		if(pkMyCellData)											\
		{															\
			pkMyCellData->m_iSpaceID = pkMyAllData->m_iSpaceID;		\
			pkMyCellData->m_uiSpaceType = pkMyAllData->m_uiSpaceType;\
			pkMyCellData->m_kPos = pkMyAllData->m_kPos;				\
			pkMyCellData->m_kDir = pkMyAllData->m_kDir;				\
		}															\
	}



#define DEFINE_INIT_ATTRIB_NODB_DATA_START(_CLASS, _PARENT)	\
	void I##_CLASS::InitAttribNoDBData()					\
	{														\
		_PARENT::InitAttribNoDBData();

#define DEFINE_INIT_ATTRIB_NODB_DATA_END	}



#define DEFINE_COPY_ATTRIB_START(_CLASS)											\
	void I##_CLASS::CopyAttrib(FvAllData* pkAllData, FvCellData* pkCellData)		\
	{																				\
		_CLASS##AllData* pkMyAllData = static_cast<_CLASS##AllData*>(pkAllData);	\
		_CLASS##CellData* pkMyCellData = static_cast<_CLASS##CellData*>(pkCellData);

#define COPY_ATTRIB_BASE(_MEMBER)	pkMyAllData->m_##_MEMBER = m_##_MEMBER;
#define COPY_ATTRIB_CELL(_MEMBER)	pkMyAllData->m_##_MEMBER = pkMyCellData->m_##_MEMBER;

#define DEFINE_COPY_ATTRIB_END										\
		if(pkMyCellData)											\
		{															\
			pkMyAllData->m_iSpaceID = pkMyCellData->m_iSpaceID;		\
			pkMyAllData->m_uiSpaceType = pkMyCellData->m_uiSpaceType;\
			pkMyAllData->m_kPos = pkMyCellData->m_kPos;				\
			pkMyAllData->m_kDir = pkMyCellData->m_kDir;				\
		}															\
	}



#define DEFINE_GETPACKPROPERTY_START(_CLASS)					\
	FvDataObj* I##_CLASS::GetPackProperty(FvUInt32 uiIdx) const	\
	{															\
		const FvDataObj* pkObj = NULL;

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

#define DEFINE_GETPACKPROPERTY_END				\
		return const_cast<FvDataObj*>(pkObj);	\
	}


#define DEFINE_ENTITY_EXPORT(_PREFIX, _CLASS, _TYPE, _HAS_BASE, _HAS_CELL, _HAS_CLI,	\
									_BASE_MEMBER_CNT, _ALL_MEMBER_CNT,					\
									_BASE_CALL_EXPCNT, _CELL_CALL_EXPCNT, _CLI_CALL_CNT,\
									_DB_TO_BASE_CNT, _BASE_TO_CLI_CNT, _DATAID_CNT)		\
	static FvBaseEntityExport s_kExport;												\
	_PREFIX void EntityExport_of_##_CLASS(void*& pkEntityExport)						\
	{																					\
		pkEntityExport = &s_kExport;													\
																						\
		s_kExport.pName = #_CLASS;														\
		s_kExport.uiTypeID = _TYPE;														\
																						\
		s_kExport.pFunCreateEntity = EntityCreateFunc_of_##_CLASS;						\
		s_kExport.pFunCreateCellData = CellDataCreateFunc;								\
		s_kExport.pFunEntityMethodsEntry = EntityMethodsEntry_of_##_CLASS;				\
																						\
		s_kExport.kForm.bHasBase = _HAS_BASE;											\
		s_kExport.kForm.bHasCell = _HAS_CELL;											\
		s_kExport.kForm.bHasClient = _HAS_CLI;											\
																						\
		s_kExport.kAttribInfo.uiCnt = _BASE_MEMBER_CNT;									\
		s_kExport.kAttribInfo.pkInfo = s_AttribInfo;									\
																						\
		s_kExport.kAllAttribInfo.uiCnt = _ALL_MEMBER_CNT;								\
		s_kExport.kAllAttribInfo.pkIdx = s_BaseAttribIdxInfo;							\
																						\
		s_kExport.kDBToBase.uiCnt = _DB_TO_BASE_CNT;									\
		s_kExport.kDBToBase.pkAttribIdx = s_DBToBase;									\
																						\
		s_kExport.kBaseToCli.uiCnt = _BASE_TO_CLI_CNT;									\
		s_kExport.kBaseToCli.pkAttribIdx = s_BaseToClient;								\
																						\
		s_kExport.kMethodCnts.uiBaseExpCnt = _BASE_CALL_EXPCNT;							\
		s_kExport.kMethodCnts.uiCellExpCnt = _CELL_CALL_EXPCNT;							\
		s_kExport.kMethodCnts.uiClientCnt = _CLI_CALL_CNT;								\
		s_kExport.kMethodCnts.pkBaseExpID = s_BaseExpID;								\
		s_kExport.kMethodCnts.pkCellExpID = NULL;										\
																						\
		s_kExport.kPtToIdxToDataID.uiBitsCnt = s_uiBitsCnt;								\
		s_kExport.kPtToIdxToDataID.pkBits = s_uiBits;									\
		s_kExport.kPtToIdxToDataID.pkIdxes = s_uiBitsIdx;								\
		s_kExport.kPtToIdxToDataID.pkIdxToID = s_uiIdxToID;								\
																						\
		s_kExport.uiDataIDCnt = _DATAID_CNT;											\
																						\
		FvMakePtToIdxData(s_uiBits,														\
			s_uiBitsCnt,																\
			s_uiBitsIdx,																\
			s_uiMemberAddrCnt,															\
			s_uiMemberAddr);															\
	}



#endif//__FvBaseEntityExportMacro_H__
