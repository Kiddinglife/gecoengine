//{future header message}
#ifndef __FvMethodDescription_H__
#define __FvMethodDescription_H__

#include "FvEntityDefDefines.h"
#include "FvObj.h"
#include "FvMemberDescription.h"

#include <FvNetTypes.h>
#include <FvNetNub.h>
#include <FvXMLSection.h>
#include <FvSmartPointer.h>

#include <set>
#include <string>

class FvBinaryOStream;
class FvBinaryIStream;
class FvDataType;
class FvMD5;


//#define FV_C_CALL	__cdecl

//! 定义逻辑dll中导出给exe使用的接口类型
class FvEntity;
typedef FvEntity*(*PFUN_CREATEENTITY)();													//! 创建Entity子类的函数
typedef void	(*PFUN_DESTROYENTITY)(FvEntity*);											//! 销毁Entity子类的函数
typedef	void	(*PFUN_ENTITYMETHOD)(FvEntity*,FvBinaryIStream*);							//! Entity的XML扩展方法原型(流化处理前)
//typedef void	(FV_C_CALL*PFUN_ENTITYPROPERTYUPDATE)(FvEntity*,DWORD,FvObj*,FvObj*);		//! Entity的XML属性更新函数原型:this,属性idx,oldVal,newVal
typedef std::map<std::string, DWORD> EntityExtMethodMap;									//! Entity的扩展函数表

//! 将类成员函数地址转换为DWORD
template<class FuncAddrType>
DWORD GetMemberFuncAddr(FuncAddrType f)
{
	union
	{
		FuncAddrType _f;
		DWORD		 _t;
	}ut;

	ut._f = f;
	return ut._t;
}

template<class FuncAddrType>
FuncAddrType GetFuncByAddr(DWORD t)
{
	union
	{
		FuncAddrType _f;
		DWORD		 _t;
	}ut;

	ut._t = t;
	return ut._f;
}


//! 定义Entity的扩展函数表
#define DEFINE_ENTITY_EXT_MOTHODMAP(_CLASS)	\
	static EntityExtMethodMap s_EntityExtMethodMap_of_##_CLASS
//! 获取Entity的扩展函数表地址
#define GET_ENTITY_EXT_MOTHODMAP(_CLASS)	\
	&s_EntityExtMethodMap_of_##_CLASS
//! 增加一个成员函数地址到Entity的扩展函数表
#define ADD_ENTITY_EXT_MOTHOD(_CLASS, _NAME, _FUNC)	\
	s_EntityExtMethodMap_of_##_CLASS.insert( std::make_pair( _NAME, GetMemberFuncAddr(&_CLASS::_FUNC) ) )

//! 定义Entity的属性更新函数表
#define DEFINE_ENTITY_PROPERTYUPDATE_FUNCLIST(_CLASS, _CNT)	\
	static DWORD s_EntityPropertyUpdateFuncList_of_##_CLASS[_CNT ? _CNT : 1] = {0}
//! 获取Entity的属性更新函数表地址
#define GET_ENTITY_PROPERTYUPDATE_FUNCLIST(_CLASS)	\
	s_EntityPropertyUpdateFuncList_of_##_CLASS
//! 增加一个成员函数地址到属性更新函数表
#define ADD_ENTITY_PROPERTYUPDATE_FUNC(_CLASS, _PROPERTY_IDX, _FUNC)	\
	s_EntityPropertyUpdateFuncList_of_##_CLASS[_PROPERTY_IDX] = GetMemberFuncAddr(&_CLASS::_FUNC)

//! Entity派生类导出函数地址列表
struct EntityFuncAddr
{
	PFUN_CREATEENTITY				pCreateFun;
	PFUN_DESTROYENTITY				pDestroyFun;
	unsigned int					methodCnt;
	PFUN_ENTITYMETHOD*				pMethodList;
	DWORD*							pPropertyUpdateFuncList;
	EntityExtMethodMap*				pExtMethodMap;

	EntityFuncAddr()
	:pCreateFun(NULL),pDestroyFun(NULL)
	,methodCnt(0),pMethodList(NULL)
	,pPropertyUpdateFuncList(NULL),pExtMethodMap(NULL)
	{}
};

//! 全部Entity派生类的导出函数集合
struct EntityFuncAddrList
{
	unsigned int		entityCnt;
	EntityFuncAddr*		pEntityFuncAddrList;

	EntityFuncAddrList()
	:entityCnt(0),pEntityFuncAddrList(NULL)
	{}
};


typedef FvSmartPointer<FvDataType> FvDataTypePtr;

typedef FvUInt16 MethodIndex;

class FV_ENTITYDEF_API FvMethodDescription : public FvMemberDescription
{
public:
	FvMethodDescription();
	FvMethodDescription( const FvMethodDescription& description );
	~FvMethodDescription();

	FvMethodDescription & operator=( const FvMethodDescription & description );

	enum eComponent
	{
		CLIENT,
		CELL,
		BASE,
		GLOBAL
	};

	bool Parse( FvXMLSectionPtr pSection, eComponent component );
	bool ParseReturnValues( FvXMLSectionPtr pSection );

	bool AddToStream( bool isFromServer, FvObj * args, FvBinaryOStream & stream ) const;
	bool CallMethod( FvObj * self,
		FvBinaryIStream & data,
		FvEntityID sourceID = 0,
		FvInt32 replyID = -1,
		const FvNetAddress* pReplyAddr = NULL,
		FvNetNub* pNub = NULL ) const;

	FvObjPtr GetArgsAsTuple( FvBinaryIStream & data,
		FvEntityID sourceID = 0 ,
		FvInt32 replyID = -1,
		const FvNetAddress* pReplyAddr = NULL,
		FvNetNub* pNub = NULL ) const;

	FvUInt32 ReturnValueCnt() const;
	const FvString& ReturnValueName( FvUInt32 index ) const;
	FvDataTypePtr ReturnValueType( FvUInt32 index ) const;

	const FvString&	Name() const			{ return m_kName; }

	bool IsExposed() const					{ return !!(m_uiFlags & IS_EXPOSED); }
	void SetExposed()						{ m_uiFlags |= IS_EXPOSED; }

	eComponent Component() const			{ return eComponent(m_uiFlags & 0x3); }
	void Component( eComponent c )			{ m_uiFlags |= c; }

	MethodIndex InternalIndex() const		{ return MethodIndex( m_iInternalIndex ); }
	void InternalIndex( FvInt32 index )		{ m_iInternalIndex = index; }
	MethodIndex ExposedIndex() const		{ return MethodIndex( m_iExposedIndex ); }
	void ExposedIndex( MethodIndex index, FvInt32 subIndex = -1 ) { m_iExposedIndex = index; m_iExposedSubIndex = subIndex;	}

	void AddToMD5( FvMD5 & md5, FvInt32 legacyExposedIndex ) const;

	float Priority() const { return m_fPriority; }

	void Dump(FvXMLSectionPtr pSection);

#if FV_ENABLE_WATCHERS
	static FvWatcherPtr pWatcher();
#endif
private:
	enum
	{
		IS_EXPOSED = 0x4
	};

	FvString		m_kName;					///< The Name of the method.

	// Lowest two bits for which Component. Next for whether it can be called by
	// the client.
	FvUInt8			m_uiFlags;

	typedef			std::vector< FvDataTypePtr > Args;
	Args			m_kArgs;

	typedef			std::pair< FvString, FvDataTypePtr > ReturnValue;
	typedef			std::vector< ReturnValue > ReturnValues;
	ReturnValues	m_kReturnValues;

	FvInt32			m_iInternalIndex;			///< Used within the server
	FvInt32			m_iExposedIndex;			///< Used between client and server
	FvInt32			m_iExposedSubIndex;			///< Used to extend address space

	float			m_fPriority;

#if FV_ENABLE_WATCHERS
	mutable FvUInt64 m_uiTimeSpent;
	mutable FvUInt64 m_uiTimesCalled;
#endif

};


#include "FvMethodDescription.inl"

#endif // __FvMethodDescription_H__
