//{future header message}
#ifndef __FvGlobalEntity_H__
#define __FvGlobalEntity_H__

#include "FvGlobalDefines.h"

#include <FvNetTypes.h>
#include <FvSmartPointer.h>
#include <FvEntityRefCnt.h>
#include "FvGlobalMailbox.h"
#include "FvGlobalAppInterface.h"


class FvEntityDescription;
class FvEntity;
typedef FvSmartPointer<FvEntity> FvEntityPtr;
struct FvGlobalEntityExport;
class FvNetUnpackedMessageHeader;


class FV_GLOBAL_API FvEntity : public FvReferenceCount
{
public:
	FvEntity();
	~FvEntity();

	bool			Init(FvEntityID id, FvGlobalEntityExport* pkExport);

	FvEntityID		GetEntityID() const { return m_iEntityID; }
	FvEntityTypeID	GetEntityTypeID() const;
	FvDatabaseID	GetDBID() const { return m_iDBID; }
	const FvMailBox&GlobalMBInEntity() const { return m_kGlobalMB; }
	const FvMailBox&CallBackMB() const { return m_kGlobalMB; }
	bool			IsDestroy() const { return m_kRefCnt.IsSetDestroy(); }

	//! 脚本调用接口
	void			Destroy();
	bool			RegisterGlobally(const char* pcName);
	bool			DeregisterGlobally(const char* pcName);
	void			OnlineLock();
	void			OnlineUnLock();
	//! 

	//! CallBack
	virtual	void	Initialize(FvUInt8 uiInitFlg) {}
	virtual	void	UnInitialize() {}
	virtual void	OnDestroy() {}
	virtual void	OnRPCResult(const FvUInt32 uiCBID, FvBinaryIStream& kIS) { FV_ERROR_MSG("virtual func OnRPCResult un-overload for Entity:%d, DBID:%I64d\n", GetEntityID(), GetDBID()); }
	virtual	void	OnRPCException(const FvUInt32 uiCBID) { FV_ERROR_MSG("virtual func OnRPCException un-overload for Entity:%d, DBID:%I64d\n", GetEntityID(), GetDBID()); }
	//! 

	//! internal interface
	void			CallGlobalMethod(FvBinaryIStream& kData);
	void			RPCCallBack(FvBinaryIStream& kData);
	void			RPCCallBackException(const GlobalAppInterface::RPCCallBackExceptionArgs& kArgs);
	//! 

	void			CallGlobalMethod( FvInt32 iMethodIdx, FvBinaryIStream & data );
	FvUInt32		NewRPCCallBackID();
	void			RealDestroy();

protected:

protected:
	FvEntityID			m_iEntityID;
	FvDatabaseID		m_iDBID;
	FvEntityRefCnt		m_kRefCnt;
	FvGlobalEntityExport*m_pkExport;
	FvMailBox			m_kGlobalMB;
	FvUInt32			m_uiRPCCallBackID;
};



#endif//__FvGlobalEntity_H__
