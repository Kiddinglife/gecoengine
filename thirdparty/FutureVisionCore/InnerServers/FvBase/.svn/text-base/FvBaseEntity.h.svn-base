#ifndef __FvBaseEntity_H__
#define __FvBaseEntity_H__

#include "FvBaseDefines.h"
#include "FvBaseAppIntInterface.h"
#include <FvNetTypes.h>
#include <FvNetChannel.h>
#include <FvEntityData.h>
#include <FvSmartPointer.h>
#include <vector>
#include <FvEntityRefCnt.h>
#include "FvBaseMailBox.h"


class FvEntityDescription;
class EntityWriteToDBCallBack;
class RegisterGlobalBaseCallBack;
class FvEntity;
typedef FvSmartPointer<FvEntity> FvEntityPtr;


class BaseAttribEventCallBack : public FvAttribEventCallBack
{
public:
	virtual void HandleEvent(FvUInt8 uiMessageID, FvBinaryIStream& kIS, bool bOwnCli, bool bGhost, bool bOthersCli, bool bDB);

	BaseAttribEventCallBack(FvEntity* pkEntity):m_pkEntity(pkEntity) {}

	FvEntity*	m_pkEntity;
};



class FV_BASE_API FvEntity : public FvReferenceCount
{
public:
	FvEntity();
	~FvEntity();

	bool			Init(FvEntityID id, FvDatabaseID dbID, FvBaseEntityExport* pkExport);

	FvEntityID		GetEntityID() const { return m_iEntityID; }
	FvEntityTypeID	GetEntityTypeID() const;
	FvDatabaseID	GetDBID() const { return m_iDBID; }
	bool			IsProxy() const { return m_bIsProxy; }
	bool			IsDestroy() const { return m_kRefCnt.IsSetDestroy(); }
	FvCellData*		GetCellData() { return m_kAttrib.GetCellData(); }
	void			OnlineLock();
	void			OnlineUnLock();

	//! 脚本调用接口
	void			CreateCellEntity(const FvMailBox* pkCellMB=NULL, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			CreateInNewSpace(FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			Destroy(bool bDeleteFromDB, bool bWriteToDB);
	void			DestroyCellEntity();
	void			WriteToDB(EntityWriteToDBCallBack* pkCallBack=NULL);
	int				AddTimer(float fInitialOffset, float fRepeatOffset=0.0f, void* pUserArg=NULL);
	void			DelTimer(int iID);
	void			RegisterGlobally(const char* pcName, RegisterGlobalBaseCallBack* pkCallBack);
	void			DeregisterGlobally(const char* pcName);
	bool			CopyAllData(FvAllData* pkAllData);
	//! 

	//! CallBack
	virtual void	InitAttrib(const FvAllData* pkAllData, FvCellData* pkCellData) {}//! copy完base/cell属性后,需要copy pos等数据
	virtual void	InitAttribNoDBData() {}
	virtual void	CopyAttrib(FvAllData* pkAllData, FvCellData* pkCellData) {}//! copy完base/cell属性后,需要copy pos等数据
	virtual	void	Initialize(FvUInt8 uiInitFlg) {}
	virtual	void	UnInitialize() {}
	virtual void	OnDestroy() {}
	virtual void	OnCreateCellFailure() {}
	virtual void	OnGetCell() {}
	virtual void	OnLoseCell() {}
	virtual void	OnTimer(int iTimerID, void* pUserData) {}
	virtual void	OnWindowOverflow() {}
	virtual void	OnRPCResult(const FvUInt32 uiCBID, FvBinaryIStream& kIS) { FV_ERROR_MSG("virtual func OnRPCResult un-overload for Entity:%d, DBID:%I64d\n", GetEntityID(), GetDBID()); }
	virtual	void	OnRPCException(const FvUInt32 uiCBID) { FV_ERROR_MSG("virtual func OnRPCException un-overload for Entity:%d, DBID:%I64d\n", GetEntityID(), GetDBID()); }
	virtual bool	CanDoGMCmd(FvUInt16 GMCmdLevel) { return false; }
	//! 

	//! internal interface
	void			CurrentCell( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	void			BackupCellEntity( FvBinaryIStream & data );
	void			WriteToDB( FvBinaryIStream & data );
	void			CellEntityLost( FvBinaryIStream & data );
	void			StartKeepAlive( const FvNetAddress & srcAdr, const BaseAppIntInterface::StartKeepAliveArgs & args );
	void			CallBaseMethod( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	void			CallCellMethod( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	void			RPCCallBack( FvBinaryIStream & data );
	void			RPCCallBackException( const BaseAppIntInterface::RPCCallBackExceptionArgs & args );
	//! 

	bool			Dump(const char* fileName);
	void			Dump(FvXMLSectionPtr pSection, FvInt32 dataDomains);
	void			SetDBID(FvDatabaseID iDBID);
	const FvMailBox&BaseMBInEntity() const { return m_kBaseMB; }
	const FvMailBox&CellMBInEntity() const { return m_pkCellMB ? *m_pkCellMB : ms_kCellMB; }
	const FvMailBox&CallBackMB() const { return m_kBaseMB; }
	bool			HasCellChannel() const { return m_pkCellChannel!=NULL; }
	bool			IsCreateCellPending() { return m_bIsCreateCellPending; }
	void			CallBaseMethod( FvInt32 iMethodIdx, FvBinaryIStream & data );
	void			CallCellMethod( FvInt32 iMethodIdx, FvBinaryIStream & data );
	FvNetChannel*	GetChannelForCellMethod(FvInt32 iMethodIdx);
	void			AfterInitialize();
	FvBaseAttrib&	GetAttrib() { return m_kAttrib; }
	FvUInt32		NewRPCCallBackID();
	void			RealDestroy();

protected:
	void			DestroySelf(bool bDeleteFromDB, bool bWriteToDB);

protected:
	FvEntityID			m_iEntityID;
	FvDatabaseID		m_iDBID;
	bool				m_bIsProxy;
	FvEntityRefCnt		m_kRefCnt;

	FvBaseAttrib		m_kAttrib;

	FvMailBox			m_kBaseMB;
	FvMailBox*			m_pkCellMB;
	static FvMailBox	ms_kCellMB;
	FvNetChannel*		m_pkCellChannel;

	bool				m_bDeleteFromDB;
	bool				m_bWriteToDB;
	bool				m_bIsCreateCellPending;
	bool				m_bIsDestroyCellPending;

	BaseAttribEventCallBack	m_kAttribEventCallBack;

	FvUInt32			m_uiRPCCallBackID;
};


class FV_BASE_API EntityWriteToDBCallBack
{
public:
	virtual void HanldeCallBack(bool bSuccess) = 0;
};

class FV_BASE_API RegisterGlobalBaseCallBack
{
public:
	virtual void HanldeCallBack(bool bSuccess) = 0;
};


#endif//__FvBaseEntity_H__