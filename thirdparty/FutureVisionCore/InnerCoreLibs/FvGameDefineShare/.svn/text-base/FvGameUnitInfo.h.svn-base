//{future header message}
#ifndef __FvGameUnitInfo_H__
#define  __FvGameUnitInfo_H__


#include <FvKernel.h>
#include <FvDebug.h>
#include "FvGameUnitDefine.h"


struct FvIdxValue
{
	FvIdxValue():m_iType(0), m_iValue(0)
	{}
	bool IsEmpty()const{return (m_iType == 0);}
	FvInt32 m_iType;			///<UnitField>
	FvInt32 m_iValue;
};


struct FvIdxValueRange
{
	FvIdxValueRange():m_iType(0), m_iValueInf(0), m_iValueSup(0)
	{}
	bool IsEmpty()const{return (m_iType == 0);}
	FvInt32 m_iType;			///<UnitField>
	FvInt32 m_iValueInf;
	FvInt32 m_iValueSup;
};


struct FvRefIdxValue
{
	FvRefIdxValue():m_iType(0), m_iPercValue(0)
	{}
	bool IsEmpty()const{return (m_iType == 0 || m_iPercValue == 0);}
	FvInt32 m_iType;			///<UnitField>
	FvInt32 m_iPercValue;		/// [0, 100]
};

struct FvStateCondition
{
	FvStateCondition():m_iReqAuraState(0), m_iNotAuraState(0)
		,m_iReqActionState(0), m_iNotActionState(0)
		,m_iReqMoveState(0), m_iNotMoveState(0){}
	bool IsEmtpy()const{return (m_iReqAuraState|m_iNotAuraState|m_iReqActionState|m_iNotActionState|m_iReqMoveState|m_iNotMoveState);}

	FvInt32 m_iReqAuraState;///<FvAuraType> 需要具备的状态
	FvInt32 m_iNotAuraState;///<FvAuraType> 不能具备的状态
	FvInt32 m_iReqActionState;
	FvInt32 m_iNotActionState;
	FvInt32 m_iReqMoveState;
	FvInt32 m_iNotMoveState;
};


#endif //__FvGameUnitInfo_H__