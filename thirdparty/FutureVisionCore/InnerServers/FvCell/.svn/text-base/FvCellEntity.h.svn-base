//{future header message}
#ifndef __FvCellEntity_H__
#define __FvCellEntity_H__

#include "FvCellDefines.h"
#include <FvNetInterfaceMinder.h>

#include "FvCellAppInterface.h"
#include "FvHistoryEvent.h"
#include "FvCellMailbox.h"
#include "FvAoIMgr.h"
#include "FvControllerInclude.h"

#include <FvTimeQueue.h>
#include <FvEntityData.h>
#include <FvEntityRefCnt.h>

#include <set>
#include <vector>
#include <map>
#include <list>


class FvCell;
class FvEntity;
class FvEntityCache;
class FvRealEntity;
class FvSpace;
class FvMemoryOStream;

class BoardVehicleCallBack;

typedef FvSmartPointer< FvEntity > FvEntityPtr;
typedef FvSmartPointer< FvObj > FvObjPtr;

typedef FvUInt16 GhostUpdateNumber;

struct FvCellEntityExport;

class CellAttribEventCallBack : public FvAttribEventCallBack
{
public:
	virtual void HandleEvent(FvUInt8 uiMessageID, FvBinaryIStream& kIS, bool bOwnCli, bool bGhost, bool bOthersCli, bool bDB);

	CellAttribEventCallBack(FvEntity* pkEntity):m_pkEntity(pkEntity) {}

	FvEntity*	m_pkEntity;
};


enum CreateRealInfo
{
	CREATE_REAL_FROM_INIT,
	CREATE_REAL_FROM_OFFLOAD,
	CREATE_REAL_FROM_RESTORE
};

template<class T>
class EntityReferenceCount : public FvReferenceCount
{
public:
	inline void DecRef() const
	{
		if(RefCount() > 1)
			FvReferenceCount::DecRef();
		else
			delete (T*)this;
	}
};

class FV_CELL_API FvEntity : public EntityReferenceCount<FvEntity>
{
	#include "FvControllerDefine.h"
public:
	FvEntity();
	~FvEntity();

	bool			Init(FvEntityID iEntityID, FvDatabaseID iDBID, FvCellEntityExport* pkExport,
						const FvVector3& kPos, const FvDirection3& kDir, FvSpace* pkSpace, const FvNetAddress& kBaseAddr);

	void			InitAoIParam(FvUInt16 uiObjMask, float fVisibility, FvUInt16 uiObsMask=0, float fVision=0.0f, float fDisVisibility=0.0f)
						{ m_uiAoIObjMask=uiObjMask; m_fAoIVisibility=fVisibility; m_uiAoIObsMask=uiObsMask; m_fAoIVision=fVision; m_fAoIDisVisibility=fDisVisibility; }
	FvEntityID		GetEntityID() const { return m_iEntityID; }
	FvEntityTypeID	GetEntityTypeID() const;
	FvDatabaseID	GetDBID() const { return m_iDBID; }
	//FvVector3&	GetPos() { return m_kPos; }
	const FvVector3&GetPos() const { return m_kPos; }
	//FvDirection3&	GetDir() { return m_kDir; }
	const FvDirection3&GetDir() const { return m_kDir; }
	void			SetPosAndDir(const FvVector3& pos, const FvDirection3& dir);
	void			SetDir(const FvDirection3& dir);
	void			SetPos(const FvVector3& pos);
	bool			IsWaitForcePosAck() const { return m_bWaitForcePosAck; }
	bool			Dump(const char* fileName);
	void			Dump(FvXMLSectionPtr pSection, FvInt32 dataDomains);

	void			OnlineLock();
	void			OnlineUnLock();

	void			SetAoIObjMask(FvUInt16 uiMask);
	void			SetAoIObsMask(FvUInt16 uiMask);
	void			SetAoIVisibility(float fVisibility);
	void			SetAoIVision(float fVision);
	void			SetAoIDisVisibility(float fDisVisibility);
	FvUInt16		GetAoIObjMask() const { return m_uiAoIObjMask; }
	FvUInt16		GetAoIObsMask() const { return m_uiAoIObsMask; }
	float			GetAoIVisibility() const { return m_fAoIVisibility; }
	float			GetAoIVision() const { return m_fAoIVision; }
	float			GetAoIDisVisibility() const { return m_fAoIDisVisibility; }

	bool			HasVehicle() const { return m_pkVehicle; }
	FvEntityID		GetVehicleID() const { return m_pkVehicle ? m_pkVehicle->GetEntityID() : FV_NULL_ENTITY; }
	FvEntity*		GetVehicle() const { return m_pkVehicle; }
	bool			BoardVehicle(FvEntityPtr spVehicle, BoardVehicleCallBack* pkCallBack);	//! 上载具,乘客使用
	void			AlightVehicle();														//! 下载具,乘客使用
	void			MoveVehicle(FvMemoryOStream& kData);									//! 移动载具,载具使用
	void			CloseVehicle();															//! 关闭载具,载具使用

	bool			IsToDRunning() const;
	void			StartToD();
	void			StopToD();
	void			SetToD(double fDayTime, float fTimeScaling);
	double			GetToD();
	float			GetToDScaling() const;

	//! 脚本调用接口
	int				AddTimer(float fInitialOffset, float fRepeatOffset=0.0f, FvUInt32 uiUserArg=0);
	void			DelTimer(int iID);
	void			Destroy();
	void			DestroySpace();
	typedef std::vector<FvEntityPtr> Entities;
	void			EntitiesInRange(Entities& kEntities, float fRange, const char* pcEntityTypeName, const FvVector3* pkPos=NULL) const;
	void			EntitiesInRange(Entities& kEntities, float fRange, FvEntityTypeID uiEntityType=-1, const FvVector3* pkPos=NULL) const;
	void			EntitiesInAoi(Entities& kEntities) const;
	void			EntitiesInTrap(FvInt16 iTrapID, Entities& kEntities) const;
	bool			IsReal() const;
	void			Teleport(const FvMailBox& kNearbyMBRef, const FvVector3& kPosition, const FvDirection3& kDirection);
	void			WriteToDB();
	FvInt16			AddTrap(FvUInt16 uiMask, float fVision, float fDisVisibility, FvInt32 iUserData);
	void			DelTrap(FvInt16 iTrapID);
	void			SetTrapMask(FvInt16 iTrapID, FvUInt16 uiMask);
	void			SetTrapVision(FvInt16 iTrapID, float fVision);
	void			SetTrapDisVisibility(FvInt16 iTrapID, float fDisVisibility);
	void			SetVisible(bool bVal);//! 老的接口,临时加回去
	//! 

	//! CallBack
	virtual void	InitAttrib(const FvAllData* pkAllData) {}	//! 只需要拷贝cell属性
	virtual	void	PreInitialize() {}
	virtual	void	Initialize(FvUInt8 uiInitFlg) {}										//! 只有Real会调用
	virtual	void	UnInitialize() {}														//! 只有Real会调用
	virtual void	InitializeGhost() {}													//! 只有Ghost会调用
	virtual void	UnInitializeGhost() {}													//! 只有Ghost会调用
	virtual void	Tick(float dTime) {}													//! 只有Real会调用
	virtual void	OnDestroy() {}															//! 只有Real会调用
	virtual void	OnEnteringCell() {}														//! 只有Ghost会调用
	virtual void	OnEnteredCell() {}														//! 只有Real会调用
	virtual void	OnLeavingCell() {}														//! 只有Real会调用
	virtual void	OnLeftCell() {}															//! 只有Ghost会调用
	virtual void	OnEnterAoi(FvEntity& kEntity) {}										//! 只有Real会调用,当pkEntity进入自己的Aoi后回调
	virtual void	OnLeaveAoi(FvEntity& kEntity) {}										//! 只有Real会调用,当pkEntity离开自己的Aoi前回调
	virtual void	OnLeaveAoiID(FvEntityID iEntityID) {}									//! 只有Real会调用,迁移到新的Cell后,发现原来在Aoi中的Entity不见了,通过该接口回调id
	virtual void	OnEnterTrap(FvEntity& kEntity, FvInt16 iTrapID, FvInt32 iUserData) {}	//! 只有Real会调用,当pkEntity进入Trap后回调
	virtual void	OnLeaveTrap(FvEntity& kEntity, FvInt16 iTrapID, FvInt32 iUserData) {}	//! 只有Real会调用,当pkEntity离开Trap前回调
	virtual void	OnLeaveTrapID(FvEntityID iEntityID, FvInt16 iTrapID, FvInt32 iUserData) {}//! 只有Real会调用,迁移到新的Cell后,发现原来在Trap中的Entity不见了,通过该接口回调id
	virtual void	OnGetWitness() {}														//! 只有Real会调用
	virtual void	OnLoseWitness() {}														//! 只有Real会调用
	//! TODO: 暂时关闭
	//virtual void	OnWitnessed(bool bIsWitnessed) {}										//! 只有Real会调用
	virtual void	OnSpaceGone() {}														//! 只有Real会调用
	virtual void	OnTeleportFailure() {}													//! 
	virtual void	OnTimer(int iTimerID, FvUInt32 pUserData) {}							//! 
	virtual void	OnWindowOverflow() {}													//! 
	virtual void	OnForcePosition() {}													//! 只有Real会调用,切换Space重置坐标时回调
	virtual void	OnForcePositionAck() {}													//! 只有Real会调用,切换Space重置坐标收到客户端Ack时回调
	virtual	bool	OnBoardVehicle(FvEntityPtr spPassenger) { return false; }				//! 只有Real会调用,只有载具会调用,返回true允许上载具,返回false
	virtual	void	OnAlightVehicle(FvEntityPtr spPassenger) {}								//! 只有Real会调用,只有载具会调用
	virtual	void	OnMoveVehicle(FvBinaryIStream& kData) {}								//! 只有Real会调用,只有乘客会调用
	virtual	void	OnCloseVehicle() {}														//! 只有Real会调用,只有乘客会调用
	virtual void	OnRPCResult(const FvUInt32 uiCBID, FvBinaryIStream& kIS) { FV_ERROR_MSG("virtual func OnRPCResult un-overload for Entity:%d, DBID:%I64d\n", GetEntityID(), GetDBID()); }
	virtual	void	OnRPCException(const FvUInt32 uiCBID) { FV_ERROR_MSG("virtual func OnRPCException un-overload for Entity:%d, DBID:%I64d\n", GetEntityID(), GetDBID()); }
	virtual bool	CanDoGMCmd(FvUInt16 GMCmdLevel) { return false; }
	//! 

	//! internal interface
	//! Entity
	void			DestroyEntity( const CellAppInterface::DestroyEntityArgs & args );
	void			AvatarUpdateImplicit( const CellAppInterface::AvatarUpdateImplicitArgs & args );
	void			AvatarUpdateExplicit( const CellAppInterface::AvatarUpdateExplicitArgs & args );
	void			AckPhysicsCorrection( const CellAppInterface::AckPhysicsCorrectionArgs & args );
	void			EnableWitness( FvBinaryIStream & data, int length );
	void			WitnessCapacity( const CellAppInterface::WitnessCapacityArgs & args );
	void			RequestEntityUpdate( FvBinaryIStream & data, int length );
	void			WriteToDBRequest( const FvNetAddress & srcAddr, FvNetUnpackedMessageHeader & header, FvBinaryIStream & stream );
	void			Witnessed( const CellAppInterface::WitnessedArgs & args );
	void			CallBaseMethod( FvBinaryIStream & data, int length );
	void			CallClientMethod( FvBinaryIStream & data, int length );
	void			RunExposedMethod( int type, FvBinaryIStream & data, int length, const FvMailBox* pkMB );
	void			RunScriptMethod( FvBinaryIStream & data, int length );
	void			DelControlledBy( const CellAppInterface::DelControlledByArgs & args );
	void			ForwardedBaseEntityPacket( FvBinaryIStream & data, int length );
	void			VehicleMsg( FvBinaryIStream & data );
	void			RPCCallBack( FvBinaryIStream & data );
	void			RPCCallBackException( const CellAppInterface::RPCCallBackExceptionArgs & args );
	void			GMCmd( FvBinaryIStream & data, int length );
	//! Ghost
	void			DelGhost( const CellAppInterface::DelGhostArgs & args );
	void			GhostVolatileInfo(	const CellAppInterface::GhostVolatileInfoArgs & args );
	void			GhostAvatarUpdate(	const CellAppInterface::GhostAvatarUpdateArgs & args );
	void			GhostHistoryEvent( FvBinaryIStream & data, int length );
	void			GhostedDataUpdate( FvBinaryIStream & data, int length );
	void			GhostedOtherClientDataUpdate( FvBinaryIStream & data, int length );
	void			AoiPriorityUpdate(	const CellAppInterface::AoiPriorityUpdateArgs & args );
	void			GhostSetReal( const CellAppInterface::GhostSetRealArgs & args );
	void			GhostSetNextReal( const CellAppInterface::GhostSetNextRealArgs & args );
	void			GhostControllerExist( FvBinaryIStream & data, int length );
	void			GhostControllerUpdate( FvBinaryIStream & data, int length );
	void			CheckGhostWitnessed( const CellAppInterface::CheckGhostWitnessedArgs & args );
	void			Onload( const FvNetAddress & srcAddr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	void			VisibleUpdate(const CellAppInterface::VisibleUpdateArgs& args);
	//! 

	bool			IsDestroy() const { return m_kRefCnt.IsSetDestroy(); }
	FvSpace*		GetSpace() { return m_pkSpace; }
	const FvSpace*	GetSpace() const { return m_pkSpace; }
	FvSpaceID		GetSpaceID() const;
	FvUInt16		GetSpaceType() const;
	bool			IsSpaceLoaded() const;
	void			CheckEventSync();
	bool			CheckTransition();	//! 返回TRUE表示已经迁移
	void			CheckGhosts();
	void			CheckAoI();
	void			CheckTraps();
	bool			HasWitness() const;
	bool			InitReal(FvBinaryIStream& stream, FvUInt8 uiInitFlg);
	bool			InitReal(const FvAllData* pkAllData, FvUInt8 uiInitFlg);
	bool			InitGhost(FvBinaryIStream& stream, FvNetAddress& kRealAddr);
	bool			ConvertRealToGhost(FvBinaryOStream& stream, const FvNetAddress& kRealAddr, bool bTeleport);
	bool			ConvertGhostToReal(FvBinaryIStream& stream);
	FvNetAddress&	BaseAddr() { return m_kBaseAddr; }
	const FvNetAddress& BaseAddr() const { return m_kBaseAddr; }
	void			AvatarUpdateForOtherClient(FvEntityID iEntityID, FvUInt32 uiTime, FvVector3& kPos, FvDirection3& kDir, FvUInt8 uiFlag);
	FvEventNumber	LastEventNumber() const { return m_uiLastEventNumber; }
	FvVolatileNumber LastVolatileUpdateNumber() const { return m_uiLastVolatileUpdateNumber; }
	GhostUpdateNumber LastGhostUpdateNumber() const { return m_uiLastGhostUpdateNumber; }
	void			SendToClient();
	void			AddOwnClientPropertyEvent(FvUInt8 uiMessageID, FvBinaryIStream& kMsg);
	void			AddGhostPropertyEvent(FvUInt8 uiMessageID, FvBinaryIStream& kMsg, bool bOthersCli);
	void			DoGhostDataUpdate(FvUInt8 uiMessageID, FvBinaryIStream& kMsg);
	void			TrimHistoryEvent();
	FvEventHistory&	GetEventHistory() { return m_kEventHistory; }
	//! 当ghost消息乱序时,保存ghost消息
	struct GhostEventCache
	{
		GhostUpdateNumber uiGhostUpdateNumber;
		FvUInt8			uiMessageID;
		char*			pcGhostData;
		int				iGhostDataLen;
		FvEventNumber	uiLastEventNumber;
		FvHistoryEvent*	pkEvent;

		GhostEventCache(GhostUpdateNumber ghostNumber, FvUInt8 msgID, const void* pcData, int len);
		GhostEventCache(GhostUpdateNumber ghostNumber, FvUInt8 msgID, const void* pcData, int len, FvEventNumber eventNumber, FvHistoryEvent* pkEvent_);
		~GhostEventCache();
	};
	void			InsertGhostEventCache(GhostEventCache* pkCache);
	void			CheckGhostEventCache();
	void			CallBaseMethodByIdx( FvInt32 iMethodIdx, FvBinaryIStream& data );
	void			CallClientMethodByIdx( FvInt32 iMethodIdx, FvBinaryIStream& data );
	void			RunScriptMethodByIdx( FvInt32 iMethodIdx, FvBinaryIStream& data );
	void			CallClientsMethodEvent(FvUInt8 uiMessageID, FvBinaryIStream& kMsg, bool bAllClients);
	FvNetChannel*	GetChannelForBaseMethod(FvInt32 iMethodIdx);
	FvNetChannel*	GetChannelForClientMethod(FvInt32 iMethodIdx);
	const FvMailBox&BaseMBInEntity() const { return m_pkBaseMB ? *m_pkBaseMB : ms_kBaseMB; }
	const FvMailBox&CellMBInEntity() const { return m_kCellMB; }
	const FvClientsMailBox&AllClientsMBInEntity() const { return m_pkAllClients ? *m_pkAllClients : ms_kAllClients; }
	const FvClientsMailBox&OtherClientsMBInEntity() const { return m_pkOtherClients ? *m_pkOtherClients : ms_kOtherClients; }
	const FvMailBox&CallBackMB() const { return m_kCellMB; }
	void			GetTraps(std::vector<float>& kTraps);
	void			TeleportFinish(FvSpaceID iOldSpaceID);
	void			TeleportLocally(FvSpaceID iSpaceID, const FvVector3& kPosition, const FvDirection3& kDirection);
	void			SyncPosDir();
	void			SetEntitiesIdx(int iIdx) { m_iEntitiesIdx = iIdx; }
	int				GetEntitiesIdx() { return m_iEntitiesIdx; }
	void			SetRealEntitiesIdx(int iIdx) { m_iRealEntitiesIdx = iIdx; }
	int				GetRealEntitiesIdx() { return m_iRealEntitiesIdx; }
	void			OpenAoI();
	void			CloseAoI();
	FvAoIHandle		GetAoIObjHandle() const { return m_hAoIObject; }
	FvAoIHandle		GetAoIObsHandle() const { return m_hAoIObserver; }
	void			SetAoIObjHandle(FvAoIHandle hHandle) { m_hAoIObject=hHandle; }
	void			SetAoIObsHandle(FvAoIHandle hHandle) { m_hAoIObserver=hHandle; }
	void			SetDummy(FvUInt64 uiDummy) { m_uiDummy=uiDummy; }
	FvUInt64		GetDummy() const { return m_uiDummy; }
	static void		AddGlobalDummy() { ++m_uiGlobalDummy; }
	static FvUInt64	GetGlobalDummy() { return m_uiGlobalDummy; }
	FvNetChannel*	GetRealChannel() const;
	void			SetVehicleID(FvEntityID iVehicleID);
	void			BoardVehicleReq(FvEntityID iPassengerID, const FvNetAddress& kAddr);
	void			BoardVehicleRep(bool bSuccess, FvEntityID iVehicleID);
	void			AlightVehicleReq(FvEntityID iPassengerID);
	void			MoveVehicleReq(FvEntityID iVehicleID, FvBinaryIStream& kData);
	void			CloseVehicleReq(FvEntityID iVehicleID);
	void			SyncVehicleToObservers();
	void			SetVehicleIDForOtherClient(FvEntityID iPassengerID, FvEntityID iVehicleID);
	FvCellAttrib&	GetAttrib()	{ return m_kAttrib; }
	void			WritePosDirToStream(FvBinaryOStream& kOS);
	void			SetDestroyEntityIdx(FvUInt32 uiIdx) { m_uiDestroyEntityIdx=uiIdx; }
	FvUInt32		GetDestroyEntityIdx() const { return m_uiDestroyEntityIdx; }
	void			ClearDestroyEntityIdx() { m_uiDestroyEntityIdx=0xFFFFFFFF; }
	bool			IsDestroyEntity() const { return m_uiDestroyEntityIdx!=0xFFFFFFFF; }
	FvUInt32		NewRPCCallBackID();
	void			RealDestroy();
	void			SendSpaceDataToClient(const FvSpaceEntryID& kEntryID, FvUInt16 uiKey, const FvString& kData);

protected:
	bool			GhostUpdateNumberLessThan(GhostUpdateNumber a, GhostUpdateNumber b);

//private:
protected:
	FvEntityID				m_iEntityID;
	FvDatabaseID			m_iDBID;
	FvVector3				m_kPos;
	FvDirection3			m_kDir;
	FvCellAttrib			m_kAttrib;
	FvEntityRefCnt			m_kRefCnt;
	FvSpace*				m_pkSpace;
	FvNetAddress			m_kBaseAddr;
	FvNetChannel*			m_pkRealChannel;
	FvRealEntity*			m_pkReal;
	FvEventHistory			m_kEventHistory;
	FvEventNumber			m_uiLastEventNumber;
	FvVolatileNumber		m_uiLastVolatileUpdateNumber;
	GhostUpdateNumber		m_uiLastGhostUpdateNumber;
	FvMailBox*				m_pkBaseMB;
	static FvMailBox		ms_kBaseMB;
	FvMailBox				m_kCellMB;
	FvClientsMailBox*		m_pkAllClients;
	FvClientsMailBox*		m_pkOtherClients;
	static FvClientsMailBox	ms_kAllClients;
	static FvClientsMailBox	ms_kOtherClients;

	//! TODO: 记录一定数量的位置同步信息,为迁移的对象做cache

	CellAttribEventCallBack	m_kAttribEventCallBack;

	typedef std::list<GhostEventCache*>	GhostEventCacheList;
	GhostEventCacheList		m_kGhostEventCacheList;

	bool					m_bWaitForcePosAck;

	int						m_iEntitiesIdx;
	int						m_iRealEntitiesIdx;

	FvAoIHandle				m_hAoIObject;
	FvAoIHandle				m_hAoIObserver;
	FvUInt16				m_uiAoIObjMask;
	FvUInt16				m_uiAoIObsMask;
	float					m_fAoIVisibility;
	float					m_fAoIVision;
	float					m_fAoIDisVisibility;
	FvUInt64				m_uiDummy;
	static FvUInt64			m_uiGlobalDummy;

	FvEntity*				m_pkVehicle;

	FvUInt32				m_uiDestroyEntityIdx;
	FvUInt32				m_uiRPCCallBackID;
};


class FV_CELL_API BoardVehicleCallBack
{
public:
	virtual void HandleCallBack(bool bSuccess) = 0;
};



#endif // __FvCellEntity_H__
