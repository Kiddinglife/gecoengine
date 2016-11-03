//{future header message}
#ifndef __FvDataOwner_H__
#define __FvDataOwner_H__

#include "FvEntityDefDefines.h"
#include <FvMemoryStream.h>

struct DataPath;
class FvEntity;
class FvDataObj;
enum FvDataTypeID;
struct DataCallBackInfo;
typedef std::vector<FvUInt32> DataCallBackPath;
class FvDataCallBack;


class FV_ENTITYDEF_API FvDataOwner
{
public:
#ifdef FV_SERVER
	FvDataOwner():m_pkOwner(NULL) {}
#else
	FvDataOwner() {}
#endif
	virtual ~FvDataOwner() {}

	enum OpCode
	{
		OPCD_ADD = 0,	//! Array增加一个节点
		OPCD_DEL,		//! Array删除一个节点
		OPCD_CLR,		//! Array清空,或NumFixArray的=
		OPCD_MOD		//! 最底层数据的修改,或Array的=/resize操作
	};

#ifdef FV_SERVER
	bool				HasOwner() const { return m_pkOwner; }
#endif

protected:
#ifdef FV_SERVER
	virtual FvDataOwner*GetRootData(OpCode uiOpCode, FvDataOwner* pkVassal, FvUInt16 uiDataID=0) { return NULL; }
	virtual void		NotifyDataChanged() {}
#endif
	virtual FvDataOwner*DataVassal(FvUInt32 uiDataID) { return NULL; }
	virtual void		DataRenovate(FvDataCallBack* pkCB, OpCode uiOpCode, FvBinaryIStream& kIS) {}

#ifdef FV_SERVER
	static FvUInt8		AddHeadToStream(OpCode uiOpCode, FvUInt16 uiDataIDCnt, FvUInt16 uiDataID);
#endif
	static FvUInt16		DoDiffRenovate(FvInt32 iMessageID, FvBinaryIStream& kIS, FvEntity* pkEntity, void* pkRoot, FvUInt16 uiDataIDCnt, DataPath* pkPath, DataCallBackInfo* pkCBs);

#ifdef FV_SERVER
	virtual	void		Attach(FvDataOwner* pkOwner, bool bSetOnce=false) { FV_ASSERT(!m_pkOwner); m_pkOwner=pkOwner; }
	virtual	void		Detach() { m_pkOwner=NULL; }
#endif

protected:
#ifdef FV_SERVER
	FvDataOwner*			m_pkOwner;
	static FvMemoryOStream	ms_kDataStream;
	typedef std::basic_string<FvUInt32> ChangePath;
	static ChangePath		ms_kDataPath;
#endif

	friend class FvDataObj;
	template<class, FvDataTypeID> friend class FvSimpleDataObj;
	friend class FvObjBool;
	friend class FvObjString;
	friend class FvObjVector2;
	friend class FvObjVector3;
	friend class FvObjVector4;
	friend class FvObjBlob;
	friend class FvObjMailBox;
	friend class FvObjUDO;
	friend class FvObjStruct;
	template<class, class> friend class FvObjArray;
	template<class, FvUInt32> friend class FvObjNumFixArray;
	friend class FvClientAttrib;
	friend class FvBaseAttrib;
	friend class FvCellAttrib;
	friend class FvRobotAttrib;

};


class FV_ENTITYDEF_API FvDataCallBack
{
public:
	//! ms_bOwnCB		是否是本节点回调
	//! ms_kCBDataPath	从根到回调绑定的数据节点需要走过的路径(每层Array的元素索引)
	//! ms_uiCBIndex	容器内元素的索引(Array和NumFixArray会使用),0xFFFF为无效Index
	//! uiOpCode		数据更新操作码
	//! ms_pkCBObj		回调绑定的数据节点/Pack数据节点
	//! pkOldVal		数据被修改前的原始值,可能为NULL[Blob修改, Array新增节点]
	virtual void Handle(FvDataOwner::OpCode uiOpCode, FvDataObj* pkOldVal) = 0;

	static bool				ms_bOwnCB;
	static FvEntity*		ms_pkEntity;
	static DataCallBackPath	ms_kCBDataPath;
	static FvUInt32			ms_uiCBIndex;
	static FvDataObj*		ms_pkCBObj;
};

//! 无效的CBIndex
const FvUInt32 INVALID_DATACALLBACK_INDEX = 0xFFFFFFFF;


//! 各种情况下属性回调参数值的含义,请参考"项目\Documentation"目录下的"属性回调说明.xlsx"
template<class T, void (T::*HandleFunc)(const DataCallBackPath&, FvUInt32, FvDataOwner::OpCode, const FvDataObj&, const FvDataObj*)>
class FvDataCallBackInEntity : public FvDataCallBack
{
public:
	virtual void Handle(FvDataOwner::OpCode uiOpCode, FvDataObj* pkOldVal)
	{
		T* pkThis = static_cast<T*>(ms_pkEntity);
		(pkThis->*HandleFunc)(ms_kCBDataPath, ms_uiCBIndex, uiOpCode, *ms_pkCBObj, pkOldVal);
	}
};


//! 各种情况下属性回调参数值的含义,请参考"项目\Documentation"目录下的"属性回调说明.xlsx"
template<class TEntity, class TLogic, void (TEntity::*GetLogic)(TLogic*&), void (TLogic::*HandleFunc)(const DataCallBackPath&, FvUInt32, FvDataOwner::OpCode, const FvDataObj&, const FvDataObj*)>
class FvDataCallBackInLogic : public FvDataCallBack
{
public:
	virtual void Handle(FvDataOwner::OpCode uiOpCode, FvDataObj* pkOldVal)
	{
		TEntity* pkThis = static_cast<TEntity*>(ms_pkEntity);
		TLogic* pkLogic(NULL);
		(pkThis->*GetLogic)(pkLogic);
		FV_ASSERT(pkLogic);
		(pkLogic->*HandleFunc)(ms_kCBDataPath, ms_uiCBIndex, uiOpCode, *ms_pkCBObj, pkOldVal);
	}
};


#endif//__FvDataOwner_H__
