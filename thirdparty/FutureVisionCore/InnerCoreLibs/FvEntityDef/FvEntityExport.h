//{future header message}
#ifndef __FvEntityExport_H__
#define __FvEntityExport_H__

#include "FvEntityDefDefines.h"
#include <FvMD5.h>
#include <map>
#include <vector>

class FvMD5;
class FvBinaryIStream;

class FvEntity;
enum FvDataFlags;

class FvClientData;
struct FvCellData;
class FvObjStruct;
struct FvAllData;
struct FvUDOAttrib;
class FvDataCallBack;
class FvCallFuncByStrHandler;
class FvLocalCallFuncByStrHandler;


//! 创建Entity子类的函数原型
typedef FvEntity*(*PFUN_CREATE_ENTITY)();
//! 创建FvClientData的函数原型
typedef FvClientData*(*PFUN_CREATE_CLIENTDATA)();
//! 创建FvCellData的函数原型
typedef FvCellData*(*PFUN_CREATE_CELLDATA)();
//! 创建FvRealData的函数原型
typedef FvObjStruct*(*PFUN_CREATE_REALDATA)();
//! 创建FvUDOAttrib的函数原型
typedef FvUDOAttrib*(*PFUN_CREATE_UDOATTRIB)();
//! Entity的消息处理函数入口原型
typedef	void(__cdecl*PFUN_ENTITY_METHODS_ENTRY)(FvEntity*, FvUInt16, FvBinaryIStream&);


struct EntityForm
{
	bool	bHasClient;	//! Entity有没有Client部分
	bool	bHasBase;	//! Entity有没有Base部分
	bool	bHasCell;	//! Entity有没有Cell部分
};

//! 属性差异更新,还原用数据路径
struct DataPath
{
	FvUInt32	uiAddr;		//! Path第一层的地址偏移
	FvUInt16*	pkPath;		//! Path,0xFFFF标记Array
	FvUInt16	uiIdx;		//! Path第一层的Idx,如果路径有多层,也是记录第一层属性的Idx,只有Cell端属性有效
	FvUInt16	uiPackIdx;	//! 第一层Pack属性的Idx,如果没有被Pack,则为0xFFFF
	FvUInt8		uiPathLen;	//! Path长度,0表示不需要路径,直接用地址偏移
};

struct CliDataIDToPath
{
	FvUInt16	uiDataIDCnt;		//! 全局DataID的个数
	DataPath*	pkOwnDataPath;		//! ClientOwnData的DataPath, DataID需要先-1再索引!!!,没有时为NULL
	DataPath*	pkOthersDataPath;	//! ClientOthersData的DataPath, DataID需要先-1再索引!!!,没有时为NULL
};

struct CliAttribAddr
{
	FvUInt32*	pkRefAddr;		//! Client属性的偏移量(所有Client属性都是引用),没有属性为0
	FvUInt16	uiRefCnt;		//! Client属性个数,没有属性为0
	FvUInt16*	pkOwnIdx;		//! OwnData的索引数组
	FvUInt16*	pkOthersIdx;	//! OthersData的索引数组
};

struct MethodExpID
{
	FvUInt16	uiExpToIdx;		//! 暴露方法到所有方法的索引
	FvUInt16	uiGMLevel;		//! GM命令的等级,不是GM命令为0xFFFF
};

struct MethodCnts
{
	FvUInt16	uiBaseExpCnt;	//! Base端暴露的方法个数
	FvUInt16	uiCellExpCnt;	//! Cell端暴露的方法个数
	FvUInt16	uiClientCnt;	//! Client端方法个数
	MethodExpID*pkBaseExpID;	//! Base端暴露方法到Base所有方法的索引+GM命令等级,没有暴露方法/不使用时为NULL
	MethodExpID*pkCellExpID;	//! Cell端暴露方法到Cell所有方法的索引+GM命令等级,没有暴露方法/不使用时为NULL
};

struct AttribIdx
{
	FvUInt16	uiCnt;		//! 特定属性总数
	FvUInt16*	pkAttribIdx;//! 特定属性在Attrib中的索引,没有时为NULL
};

//! 属性更新回调信息
struct DataCallBackInfo
{
	FvDataCallBack*	pkCallBack;	//! 回调函数指针,可能是绑定本节点的,也可能是绑定父节点的(往上多层)
	FvUInt16		uiCBDataID;	//! 回调函数所绑定的节点的DataID,无回调/Pack回调为0xFFFF
	FvUInt8			uiCBType;	//! 0:无回调,1:本节点回调,2:父节点回调,3:父节点Array回调,4:Pack回调
};

//! 字符串调函数处理接口Map
typedef std::vector<std::pair<FvString, FvCallFuncByStrHandler*>> CallFuncByStrHandlerList;
typedef std::map<FvString, CallFuncByStrHandlerList> CallFuncByStrHandlerMap;
typedef std::vector<std::pair<FvString, FvLocalCallFuncByStrHandler*>> LocalCallFuncByStrHandlerList;
typedef std::map<FvString, LocalCallFuncByStrHandlerList> LocalCallFuncByStrHandlerMap;

//! ClientEntity导出的数据
struct FvClientEntityExport
{
	char*						pName;					//! Entity类型名
	FvUInt16					uiTypeID;				//! Entity类型ID
	PFUN_CREATE_ENTITY			pFunCreateEntity;		//! 创建ClientEntity子类的函数指针
	PFUN_CREATE_CLIENTDATA		pFunCreateOwnData;		//! 创建ClientOwnData的函数指针,可能为NULL表示没有OwnData
	PFUN_CREATE_CLIENTDATA		pFunCreateOthersData;	//! 创建ClientOthersData的函数指针,可能为NULL表示没有OthersData
	PFUN_ENTITY_METHODS_ENTRY	pFunEntityMethodsEntry;	//! ClientEntity子类的消息处理函数入口指针,可能为NULL表示没有Client函数
	EntityForm					kForm;					//! Entity子类的构成方式
	CliAttribAddr				kAttribAddr;			//! Client属性的地址偏移
	MethodCnts					kMethodCnts;			//! 各种方法数量
	AttribIdx					kBaseToCli;				//! Base端需要同步给Client的属性的索引映射
	AttribIdx					kCellToOwnCli;			//! Cell端需要同步给OwnClient的属性的索引映射
	CliDataIDToPath				kDataIDToPath;			//! DataID->DataPath
	DataCallBackInfo*			pkDataCallBacks;		//! 属性更新回调信息数组
	CallFuncByStrHandlerMap		kStrFuncHandlerMap;		//! 字符串调函数处理接口Map
};

//! 所有ClientEntity导出的数据
struct FV_ENTITYDEF_API FvAllClientEntityExports
{
	FvMD5::Digest					kMD5Digest;			//! MD5码
	FvUInt16						uiEntityCnt;		//! Entity类型总数
	FvClientEntityExport**			ppkEntityExport;	//! 每种Entity类型的Client导出数据
	LocalCallFuncByStrHandlerMap	kMacroCommandMap;	//! 宏命令处理接口Map

	FvClientEntityExport*		FindByName(const char* pcEntityTypeName) const;

	FvLocalCallFuncByStrHandler*FindMacroCmdHandler(const FvString& kCmd, const FvString& kProtoType) const;
	bool						HasMacroCmdName(const FvString& kCmd) const;
};

//! RobotEntity导出的数据
struct FvRobotEntityExport
{
	char*						pName;					//! Entity类型名
	FvUInt16					uiTypeID;				//! Entity类型ID
	PFUN_CREATE_ENTITY			pFunCreateEntity;		//! 创建ClientEntity子类的函数指针
	PFUN_CREATE_CLIENTDATA		pFunCreateOwnData;		//! 创建ClientOwnData的函数指针,可能为NULL表示没有OwnData
	PFUN_CREATE_CLIENTDATA		pFunCreateOthersData;	//! 创建ClientOthersData的函数指针,可能为NULL表示没有OthersData
	PFUN_ENTITY_METHODS_ENTRY	pFunEntityMethodsEntry;	//! ClientEntity子类的消息处理函数入口指针,可能为NULL表示没有Client函数
	EntityForm					kForm;					//! Entity子类的构成方式
	CliAttribAddr				kAttribAddr;			//! Client属性的地址偏移
	MethodCnts					kMethodCnts;			//! 各种方法数量
	AttribIdx					kBaseToCli;				//! Base端需要同步给Client的属性的索引映射
	AttribIdx					kCellToOwnCli;			//! Cell端需要同步给OwnClient的属性的索引映射
	CliDataIDToPath				kDataIDToPath;			//! DataID->DataPath
	DataCallBackInfo*			pkDataCallBacks;		//! 属性更新回调信息数组
};


//! 所有RobotEntity导出的数据
struct FV_ENTITYDEF_API FvAllRobotEntityExports
{
	FvMD5::Digest				kMD5Digest;			//! MD5码
	FvUInt16					uiEntityCnt;		//! Entity类型总数
	FvRobotEntityExport**		ppkEntityExport;	//! 每种Entity类型的Client导出数据

	FvRobotEntityExport*		FindByName(const char* pcEntityTypeName) const;
};

struct FV_ENTITYDEF_API AttribInfo
{
	FvUInt32	uiAddr;		//! 属性的地址偏移
	FvUInt8		uiDataFlag;	//! 属性的标签
	FvUInt8		uiIsRef;	//! 属性是否为引用类型

	bool	IsGhostedData() const;
	bool	IsOtherClientData() const;
	bool	IsOwnClientData() const;
	bool	IsCellData() const;
	bool	IsBaseData() const;
	bool	IsClientOnlyData() const;
	bool	IsClientServerData() const;
	bool	IsPersistent() const;
	bool	IsIdentifier() const;
	bool	IsEditorOnly() const;
	bool	IsOfType(FvDataFlags flags) const;
};


struct AttribInfoSet
{
	FvUInt16	uiCnt;		//! 属性总数
	AttribInfo*	pkInfo;		//! 各属性的信息(地址偏移,属性标签,引用),按定义顺序,没有时为NULL
};

struct BaseAttribIdxInfo
{
	FvUInt16	uiIsBaseAttrib;	//! 属性类型,!=0为Base属性,=0为Cell属性
	FvUInt16	uiAttribIdx;	//! 属性在Attrib中的索引
};

struct BaseAllAttribInfo
{
	FvUInt16	uiCnt;			//! 属性总数
	BaseAttribIdxInfo* pkIdx;	//! 各属性在Attrib中的索引,按定义的顺序,没有时为NULL
};

struct PtIdxToID
{
	FvUInt16	uiDataIdx;	//! 第一层属性的idx,表示自己属于第一层的哪个属性
	FvUInt16	uiDataID;	//! 属性ID,表示自己在本范围内的DataID
};

struct PointerToIdxToDataID
{
	FvUInt32	uiBitsCnt;	//! 位数组的大小
	FvUInt32*	pkBits;		//! 位数组,没有时为NULL
	FvUInt16*	pkIdxes;	//!	与位数组对应的idx的累计值数组,没有时为NULL
	PtIdxToID*	pkIdxToID;	//! idx->DataIdx/DataID索引数组,没有时为NULL
};

//! BaseEntity导出的数据
struct FvBaseEntityExport
{
	char*						pName;					//! Entity类型名
	FvUInt16					uiTypeID;				//! Entity类型ID
	PFUN_CREATE_ENTITY			pFunCreateEntity;		//! 创建BaseEntity子类的函数指针
	PFUN_CREATE_CELLDATA		pFunCreateCellData;		//! 创建CellData的函数指针,可能为NULL表示没有CellData
	PFUN_ENTITY_METHODS_ENTRY	pFunEntityMethodsEntry;	//! BaseEntity子类的消息处理函数入口指针,可能为NULL表示没有Base函数
	EntityForm					kForm;					//! Entity子类的构成方式
	AttribInfoSet				kAttribInfo;			//! Base属性信息(偏移,标签)
	BaseAllAttribInfo			kAllAttribInfo;			//! 全部属性的索引信息(Base/Cell,Idx)
	AttribIdx					kDBToBase;				//! DB存档属性到Base属性的索引映射
	AttribIdx					kBaseToCli;				//! Base端需要同步给Client的属性的索引映射
	MethodCnts					kMethodCnts;			//! 方法数量
	PointerToIdxToDataID		kPtToIdxToDataID;		//! 地址偏移->属性索引->DataID的映射
	FvUInt16					uiDataIDCnt;			//! 全局DataID的个数
};


//! 所有BaseEntity导出的数据
struct FV_ENTITYDEF_API FvAllBaseEntityExports
{
	FvMD5::Digest			kMD5Digest;		//! MD5码
	FvUInt16				uiEntityCnt;	//! Entity类型总数
	FvBaseEntityExport**	ppkEntityExport;//! 每种Entity类型的Base导出数据

	FvBaseEntityExport*		FindByName(const char* pcEntityTypeName) const;
};


struct CellDataIDToPath
{
	FvUInt16	uiDataIDCnt;//! 全局DataID的个数
	DataPath*	pkDataPath;	//! Ghost属性有DataPath, Real属性只有Idx有效, DataID需要先-1再索引!!!, 没有时为NULL
};

struct CellAddrs
{
	FvUInt16	uiRealDataCnt;	//! RealData的个数
	FvUInt16	uiGhostDataCnt;	//! GhostData的个数
	FvUInt32*	pkRealAddrs;	//! RealData的地址偏移量(RealData为引用)
	FvUInt32*	pkGhostAddrs;	//! GhostData的地址偏移量
};

struct SyncToClientIdx
{
	FvUInt16	uiServerIdx;	//! 属性在服务器端Attrib里的Idx
	FvUInt16	uiClientIdx;	//! 属性在客户端Attrib里的Idx
};

struct SyncToClientIdxSet
{
	FvUInt16		uiCnt;		//! 属性总数
	SyncToClientIdx*pkAttribIdx;//! 需要同步的属性索引(SvrIdx+CliIdx),没有时为NULL
};

struct UDOExport
{
	char*					pName;		//! UDO类型名
	PFUN_CREATE_UDOATTRIB	pFunCreate;	//! UDO创建函数指针
};

struct AllUDOExports
{
	FvUInt16	uiCnt;			//! UDO类型总数
	UDOExport*	pkUDOExport;	//! 每种UDO类型的导出数据

	FvUInt16	NameToType(const char* pcTypeName) const;
};

//! CellEntity导出的数据
struct FvCellEntityExport
{
	char*						pName;					//! Entity类型名
	FvUInt16					uiTypeID;				//! Entity类型ID
	PFUN_CREATE_ENTITY			pFunCreateEntity;		//! 创建CellEntity子类的函数指针
	PFUN_CREATE_REALDATA		pFunCreateRealData;		//! 创建RealData的函数指针,可能为NULL表示没有RealData
	PFUN_ENTITY_METHODS_ENTRY	pFunEntityMethodsEntry;	//! CellEntity子类的消息处理函数入口指针,可能为NULL表示没有Cell函数
	EntityForm					kForm;					//! Entity子类的构成方式
	AttribInfoSet				kAttribInfo;			//! Cell属性信息(偏移,标签,是否引用)
	CellAddrs					kAddrs;					//! Cell属性地址偏移(Real+Ghost)
	AttribIdx					kDBToCellIdx;			//! DB存档属性到Cell属性的索引映射
	AttribIdx					kCellToCli;				//! Cell端需要同步给OwnClient的属性的索引映射
	SyncToClientIdxSet			kCellToOthersCli;		//! Cell端需要同步给OthersClient的属性的索引映射
	MethodCnts					kMethodCnts;			//! 方法数量
	CellDataIDToPath			kDataIDToPath;			//! DataID->DataPath
	PointerToIdxToDataID		kPtToIdxToDataID;		//! 地址偏移->属性索引->DataID的映射,只是Ghost部分
	DataCallBackInfo*			pkDataCallBacks;		//! 属性更新回调信息数组
};


//! 所有BaseEntity导出的数据
struct FV_ENTITYDEF_API FvAllCellEntityExports
{
	FvMD5::Digest			kMD5Digest;		//! MD5码
	FvUInt16				uiEntityCnt;	//! Entity类型总数
	FvCellEntityExport**	ppkEntityExport;//! 每种Entity类型的Cell导出数据
	AllUDOExports*			pkUDOExports;	//! UDO导出数据

	FvCellEntityExport*		FindByName(const char* pcEntityTypeName) const;
};


//! GlobalEntity导出的数据
struct FvGlobalEntityExport
{
	char*						pName;					//! Entity类型名
	FvUInt16					uiTypeID;				//! Entity类型ID
	PFUN_CREATE_ENTITY			pFunCreateEntity;		//! 创建GlobalEntity子类的函数指针
	PFUN_ENTITY_METHODS_ENTRY	pFunEntityMethodsEntry;	//! GlobalEntity子类的消息处理函数入口指针
};


//! 所有GlobalEntity导出的数据
struct FV_ENTITYDEF_API FvAllGlobalEntityExports
{
	FvMD5::Digest			kMD5Digest;		//! MD5码
	FvUInt16				uiEntityCnt;	//! Entity类型总数
	FvGlobalEntityExport**	ppkEntityExport;//! 每种Entity类型的Global导出数据

	FvGlobalEntityExport*	FindByName(const char* pcEntityTypeName) const;
};


#endif//__FvEntityExport_H__
