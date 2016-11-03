//{future header message}
#ifndef __FvStructObjExportMacro_H__
#define __FvStructObjExportMacro_H__

#include "FvEntityDefUtility.h"


#define BITSCNT(_CLASS, _LAST)	((FV_OFFSET_OF(_CLASS,_LAST)>>7)+1)


#define DEFINE_IDX_TO_ID(_STRUCT)	\
	static PtIdxToID s_uiIdxToID_of_##_STRUCT##Obj[]


#define DEFINE_DATAID_TO_ADDR_START(_STRUCT, _CNT)			\
	static FvUInt32 s_uiDataIDToAddrCnt_of_##_STRUCT##Obj = _CNT;\
	static FvUInt32 s_uiDataIDToAddr_of_##_STRUCT##Obj[_CNT] = {

#define DATAID_TO_ADDR(_STRUCT, _MEMBER) FV_OFFSET_OF(_STRUCT##Obj, _MEMBER),
#define DATAID_TO_ADDR_NULL		0,

#define DEFINE_DATAID_TO_ADDR_END };


#define DEFINE_OHTER_EXPORT_START(_STRUCT, _CNT, _LASTMEMBER)							\
	static FvUInt32 s_uiBitsCnt_of_##_STRUCT##Obj = BITSCNT(_STRUCT##Obj, _LASTMEMBER);	\
	static FvUInt32 s_uiBits_of_##_STRUCT##Obj[BITSCNT(_STRUCT##Obj, _LASTMEMBER)];		\
	static FvUInt16 s_uiBitsIdx_of_##_STRUCT##Obj[BITSCNT(_STRUCT##Obj, _LASTMEMBER)];	\
	static FvUInt32 s_uiAllAddr_of_##_STRUCT##Obj_Cnt = _CNT;							\
	static FvUInt32 s_kAllAddr_of_##_STRUCT##Obj[_CNT] = {

#define OHTER_EXPORT(_STRUCT, _MEMBER)	FV_OFFSET_OF(_STRUCT##Obj, _MEMBER),

#define DEFINE_OHTER_EXPORT_END };


#define DEFINE_STRUCT_ADDR_EXPORT_START(_STRUCT, _CNT)	\
	static NameAddr s_kInfo_of_##_STRUCT##Obj[_CNT] = {

#define STRUCT_ADDR(_STRUCT, _MEMBER) { #_MEMBER, FV_OFFSET_OF(_STRUCT##Obj, m_##_MEMBER) },

#define DEFINE_STRUCT_ADDR_EXPORT_END };


#ifdef FV_SERVER
#define DEFINE_EQUAL_TO_START(_STRUCT)	void _STRUCT##Obj::EqualTo(const _STRUCT##Obj& kRight){
#define EQUAL_TO(_MEMBER)				ChildEqualTo(m_##_MEMBER, kRight.m_##_MEMBER);
#define DEFINE_EQUAL_TO_END				}
#else
#define DEFINE_EQUAL_TO_START(_STRUCT)
#define EQUAL_TO(_MEMBER)
#define DEFINE_EQUAL_TO_END
#endif


#define DEFINE_STRUCT_EXPORT(_PREFIX, _STRUCT, _CNT)														\
	void _STRUCT##Obj::GetStructAddr(FvUInt32& uiCnt, NameAddr*& pkInfo) const								\
	{																										\
		uiCnt = _CNT;																						\
		pkInfo = s_kInfo_of_##_STRUCT##Obj;																	\
	}																										\
	bool _STRUCT##Obj::GetPtToID(FvUInt32& uiCnt, FvUInt32*& pBits, FvUInt16*& pIdx, PtIdxToID*& pIDs) const\
	{																										\
		uiCnt = s_uiBitsCnt_of_##_STRUCT##Obj;																\
		pBits = s_uiBits_of_##_STRUCT##Obj;																	\
		pIdx = s_uiBitsIdx_of_##_STRUCT##Obj;																\
		pIDs = s_uiIdxToID_of_##_STRUCT##Obj;																\
		return true;																						\
	}																										\
	bool _STRUCT##Obj::GetIDToAddr(FvUInt32& uiCnt, FvUInt32*& pAddr) const									\
	{																										\
		uiCnt = s_uiDataIDToAddrCnt_of_##_STRUCT##Obj;														\
		pAddr = s_uiDataIDToAddr_of_##_STRUCT##Obj;															\
		return true;																						\
	}																										\
	void _PREFIX StructExport_of_##_STRUCT##Obj()															\
	{																										\
		FvMakePtToIdxData(s_uiBits_of_##_STRUCT##Obj,														\
			s_uiBitsCnt_of_##_STRUCT##Obj,																	\
			s_uiBitsIdx_of_##_STRUCT##Obj,																	\
			s_uiAllAddr_of_##_STRUCT##Obj_Cnt,																\
			s_kAllAddr_of_##_STRUCT##Obj);																	\
	}


#define DEFINE_TRIVIAL_STRUCT_EXPORT(_PREFIX, _STRUCT, _CNT)												\
	void _STRUCT##Obj::GetStructAddr(FvUInt32& uiCnt, NameAddr*& pkInfo) const								\
	{																										\
		uiCnt = _CNT;																						\
		pkInfo = s_kInfo_of_##_STRUCT##Obj;																	\
	}																										\
	bool _STRUCT##Obj::GetPtToID(FvUInt32& uiCnt, FvUInt32*& pBits, FvUInt16*& pIdx, PtIdxToID*& pIDs) const\
	{																										\
		return false;																						\
	}																										\
	bool _STRUCT##Obj::GetIDToAddr(FvUInt32& uiCnt, FvUInt32*& pAddr) const									\
	{																										\
		return false;																						\
	}																										\
	void _PREFIX StructExport_of_##_STRUCT##Obj()															\
	{																										\
	}


#endif//__FvStructObjExportMacro_H__
