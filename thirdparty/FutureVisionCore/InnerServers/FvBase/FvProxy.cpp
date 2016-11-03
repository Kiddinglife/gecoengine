#include "FvProxy.h"
#include "FvClientInterface.h"
#include <../FvCell/FvCellAppInterface.h>
#include <FvEntityData.h>
#include <FvEntityExport.h>
#include <FvEntityDefUtility.h>



FvProxy::FvProxy()
:m_pkClientChannel(NULL)
,m_kEncryptionKey()
,m_uiSessionKey(0)
,m_bEntitiesEnabled(false)
,m_bCellPlayerCreatedOnClient(false)
,m_uiSendEnableWitnessCnt(0)
{
	m_bIsProxy = true;
}

FvProxy::~FvProxy()
{
}

void FvProxy::GiveClientTo(FvProxy* pkDest)
{
	if(!m_pkClientChannel)
		return;

	if(pkDest && (!pkDest->IsProxy() || pkDest->HasClient()))
		return;

	if(m_bEntitiesEnabled && m_pkCellChannel && m_pkCellChannel->IsEstablished() && !m_bIsDestroyCellPending)
		SendEnableDisableWitness(false);

	if(pkDest)
	{
		if(m_bEntitiesEnabled)
			SendResetToClient();
		pkDest->SetClientChannel(m_pkClientChannel);
		//! 改变client所对应的proxy
		FvEntityManager::Instance().ChangeProxy(m_pkClientChannel->addr(), pkDest);
	}
	else
	{
		DestroyClientChannel(true);
	}

	m_pkClientChannel = NULL;
	m_bEntitiesEnabled = false;
	m_bCellPlayerCreatedOnClient = false;
}

void FvProxy::AvatarUpdateImplicit( const BaseAppExtInterface::AvatarUpdateImplicitArgs & args )
{
	if(!EntitiesEnabled())
		return;

	FV_ASSERT(m_pkCellChannel);
	FvNetBundle& kBundle = m_pkCellChannel->Bundle();
	kBundle.StartMessage(CellAppInterface::AvatarUpdateImplicit);
	CellAppInterface::AvatarUpdateImplicitArgs kArgs;
	kArgs.pos = args.pos;
	kArgs.dir = args.dir;
	kArgs.refNum = args.refNum;
	kBundle << m_iEntityID << kArgs;
	m_pkCellChannel->DelayedSend();
}

void FvProxy::AvatarUpdateExplicit( const BaseAppExtInterface::AvatarUpdateExplicitArgs & args )
{
	if(!EntitiesEnabled())
		return;
}

void FvProxy::AvatarUpdateWardImplicit( const BaseAppExtInterface::AvatarUpdateWardImplicitArgs & args )
{
	if(!EntitiesEnabled())
		return;
}

void FvProxy::AvatarUpdateWardExplicit( const BaseAppExtInterface::AvatarUpdateWardExplicitArgs & args )
{
	if(!EntitiesEnabled())
		return;
}

void FvProxy::AckPhysicsCorrection( const BaseAppExtInterface::AckPhysicsCorrectionArgs & args )
{
	if(!EntitiesEnabled())
		return;

	FV_ASSERT(m_pkCellChannel);
	FvNetBundle& kBundle = m_pkCellChannel->Bundle();
	kBundle.StartMessage(CellAppInterface::AckPhysicsCorrection);
	CellAppInterface::AckPhysicsCorrectionArgs kArgs;
	kBundle << m_iEntityID << kArgs;
	m_pkCellChannel->DelayedSend();
}

void FvProxy::AckWardPhysicsCorrection( const BaseAppExtInterface::AckWardPhysicsCorrectionArgs & args )
{
	if(!EntitiesEnabled())
		return;
}

void FvProxy::RequestEntityUpdate( FvBinaryIStream & data )
{
	if(!EntitiesEnabled())
		return;
}

void FvProxy::EnableEntities( const BaseAppExtInterface::EnableEntitiesArgs & args )
{
	//! TODO: 外网接口,防止随意发包导致服务器断言/crash
	if(m_bEntitiesEnabled)
	{
		FV_ERROR_MSG("%s, id:%d\n", __FUNCTION__, m_iEntityID);
		FV_ASSERT(!m_bEntitiesEnabled);
	}
	m_bEntitiesEnabled = true;

	FV_INFO_MSG("%s, id:%d\n", __FUNCTION__, m_iEntityID);

	//! 返回Base部分数据
	//! FvEntityID id;
	//! FvEntityTypeID type;
	//! Base和client公有数据
	FvNetBundle& kBundle = ClientBundle();
	kBundle.StartMessage(ClientInterface::CreateBasePlayer);
	kBundle << m_iEntityID << GetEntityTypeID();
	m_kAttrib.SerializeToStreamForCliData(kBundle);
	m_pkClientChannel->DelayedSend();

	FV_ASSERT(!m_bCellPlayerCreatedOnClient);
	if(m_pkCellChannel && m_pkCellChannel->IsEstablished() && !m_bIsDestroyCellPending)
		SendEnableDisableWitness(true);

	OnEntitiesEnabled();
}

void FvProxy::RestoreClientAck( const BaseAppExtInterface::RestoreClientAckArgs & args )
{
	if(!EntitiesEnabled())
		return;
}

void FvProxy::DisconnectClient( const BaseAppExtInterface::DisconnectClientArgs & args )
{
	FV_INFO_MSG("%s, reason:%d\n", __FUNCTION__, args.reason);

	if(m_bEntitiesEnabled && m_pkCellChannel && m_pkCellChannel->IsEstablished() && !m_bIsDestroyCellPending)
		SendEnableDisableWitness(false);
	m_bCellPlayerCreatedOnClient = false;

	DestroyClientChannel(false);

	OnClientDeath();
}

void FvProxy::GMCmd( FvBinaryIStream & data )
{
	if(!EntitiesEnabled())
	{
		data.Finish();
		return;
	}

	bool bToBase;
	data >> bToBase;
	if(data.Error())
	{
		FV_ERROR_MSG("GMCmd stream Err\n");
		return;
	}

	if(bToBase)
	{
		FvEntityID iEntityID;
		FvUInt16 uiMethodID;
		data >> iEntityID >> uiMethodID;
		MethodCnts& kMethods = m_kAttrib.GetExport()->kMethodCnts;
		if(data.Error() || iEntityID!=GetEntityID() || kMethods.uiBaseExpCnt <= uiMethodID || kMethods.pkBaseExpID[uiMethodID].uiGMLevel == 0xFFFF)
		{
			FV_ERROR_MSG("GMCmd data Err\n");
			data.Finish();
			return;
		}
		//! 判断GM等级
		if(!CanDoGMCmd(kMethods.pkBaseExpID[uiMethodID].uiGMLevel))
		{
			FV_ERROR_MSG("GMCmd level too high\n");
			data.Finish();
			return;
		}
		m_kAttrib.GetExport()->pFunEntityMethodsEntry(this, kMethods.pkBaseExpID[uiMethodID].uiExpToIdx, data);
	}
	else
	{
		FV_ASSERT(m_pkCellChannel);
		FvNetBundle& kBundle = m_pkCellChannel->Bundle();
		kBundle.StartMessage(CellAppInterface::GMCmd);
		kBundle.Transfer(data, data.RemainingLength());
		m_pkCellChannel->DelayedSend();
	}
}

void FvProxy::SendToClient( const BaseAppIntInterface::SendToClientArgs & args )
{
	FV_ASSERT(0);
}

void FvProxy::TickSync( const BaseAppIntInterface::TickSyncArgs & args )
{
	FV_ASSERT(0);
}

void FvProxy::CreateCellPlayer( FvBinaryIStream & data )
{
	FV_ASSERT(m_uiSendEnableWitnessCnt > 0);
	--m_uiSendEnableWitnessCnt;

	FV_ASSERT(!m_bCellPlayerCreatedOnClient);
	if(m_pkClientChannel && m_bEntitiesEnabled && m_uiSendEnableWitnessCnt==0)
		m_bCellPlayerCreatedOnClient = true;

	if(!m_bCellPlayerCreatedOnClient)
	{
		data.Finish();
		return;
	}

	ClientBundle().StartMessage(ClientInterface::CreateCellPlayer);
	ClientBundle().Transfer(data, data.RemainingLength());
	m_pkClientChannel->DelayedSend();

	OnClientGetCell();
}

void FvProxy::SpaceData( FvBinaryIStream & data )
{
	if(!m_bCellPlayerCreatedOnClient)
	{
		data.Finish();
		return;
	}

	ClientBundle().StartMessage(ClientInterface::SpaceData);
	ClientBundle().Transfer(data, data.RemainingLength());
}

void FvProxy::EnterAoI( const BaseAppIntInterface::EnterAoIArgs & args )
{
	if(!m_bCellPlayerCreatedOnClient)
		return;
}

void FvProxy::EnterAoIOnVehicle( const BaseAppIntInterface::EnterAoIOnVehicleArgs & args )
{
	if(!m_bCellPlayerCreatedOnClient)
		return;
}

void FvProxy::LeaveAoI( FvBinaryIStream & data )
{
	if(!m_bCellPlayerCreatedOnClient)
		return;

	ClientBundle().StartMessage(ClientInterface::LeaveAoI);
	ClientBundle().Transfer(data, data.RemainingLength());
}

void FvProxy::SetVehicle( const BaseAppIntInterface::SetVehicleArgs & args )
{
	if(!m_bCellPlayerCreatedOnClient)
		return;

	ClientInterface::SetVehicleArgs& kArgs = ClientInterface::SetVehicleArgs::start(ClientBundle());
	kArgs.passengerID = args.passengerID;
	kArgs.vehicleID = args.vehicleID;
}

void FvProxy::CreateEntity( FvBinaryIStream & data )
{
	if(!m_bCellPlayerCreatedOnClient)
	{
		data.Finish();
		return;
	}

	ClientBundle().StartMessage(ClientInterface::CreateEntity);
	ClientBundle().Transfer(data, data.RemainingLength());
}

void FvProxy::UpdateEntity( FvBinaryIStream & data )
{
	if(!m_bCellPlayerCreatedOnClient)
		return;

	ClientBundle().StartMessage(ClientInterface::UpdateEntity);
	ClientBundle().Transfer(data, data.RemainingLength());
}

void FvProxy::DetailedPosition( const BaseAppIntInterface::DetailedPositionArgs & args )
{
	if(!m_bCellPlayerCreatedOnClient)
		return;

	ClientInterface::DetailedPositionArgs& kArgs = ClientInterface::DetailedPositionArgs::start(ClientBundle());
	kArgs.id = args.id;
	kArgs.position = args.position;
	kArgs.direction = args.direction;
}

void FvProxy::ForcedPosition( const BaseAppIntInterface::ForcedPositionArgs & args )
{
	if(!m_bCellPlayerCreatedOnClient)
		return;

	ClientInterface::ForcedPositionArgs& kArgs = ClientInterface::ForcedPositionArgs::start(ClientBundle());
	kArgs.id		= args.id;
	kArgs.spaceID	= args.spaceID;
	kArgs.vehicleID	= args.vehicleID;
	kArgs.position	= args.position;
	kArgs.direction	= args.direction;
}

void FvProxy::ModWard( const BaseAppIntInterface::ModWardArgs & args )
{
	if(!m_bCellPlayerCreatedOnClient)
		return;
}

void FvProxy::CallClientMethod( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data )
{
	if(!m_pkClientChannel || !m_bEntitiesEnabled)
	{
		FV_ERROR_MSG("%s, No Client can be called\n", __FUNCTION__);
		data.Finish();
		return;
	}

	FvInt32 iMethodIdx;
	data >> iMethodIdx;
	CallClientMethod(iMethodIdx, data);
}

void FvProxy::PipeIntMessage( int type, FvBinaryIStream & data, FvUInt32 length )
{
	if(!m_bCellPlayerCreatedOnClient)
	{
		//FV_WARNING_MSG("%s, Cell isn't Create in Client(%d), MsgID:%d\n", __FUNCTION__, GetEntityID(), type);
		data.Finish();
		return;
	}

	static FvNetInterfaceElement kInterface = ClientInterface::EntityMessage;
	kInterface.SetID(FvUInt8(type));
	ClientBundle().StartMessage(kInterface);
	ClientBundle().Transfer(data, length);
}

void FvProxy::PipeExtMessage( int type, FvBinaryIStream & data, FvUInt32 length )
{
	if(!EntitiesEnabled())
	{
		//FV_ERROR_MSG("%s, Entity(%d) isn't Enabled, MsgID:%d\n", __FUNCTION__, GetEntityID(), type);
		//! 不返回回调异常
		data.Finish();
		return;
	}

	static FvNetInterfaceElement kInterface = CellAppInterface::RunExposedMethod;

	if((type & 0x40) == 0x40)
		type |= 0x80;//! 0x40->0xC0

	kInterface.SetID(FvUInt8(type));
	FV_ASSERT(m_pkCellChannel);
	FvNetBundle& kBundle = m_pkCellChannel->Bundle();
	kBundle.StartMessage(kInterface);
	kBundle.Transfer(data, data.RemainingLength());
	m_pkCellChannel->DelayedSend();
}

void FvProxy::HandleExtMessage( int type, FvBinaryIStream & data, FvUInt32 length )
{
	if(!EntitiesEnabled())
	{
		//FV_ERROR_MSG("%s, Entity(%d) isn't Enabled, MsgID:%d\n", __FUNCTION__, GetEntityID(), type);
		//! 不返回回调异常
		data.Finish();
		return;
	}

	if(type & 0x20)
	{
		static FvObjBlob kMB;
		kMB.Reset();
		kMB << CallBackMB();
		m_kAttrib.OnMethodFromClientWithCallBack(type, data, kMB);
	}
	else
	{
		m_kAttrib.OnMethodFromClient(type, data);
	}
}

void FvProxy::SyncPropertyUpdate()
{
	//if(!m_pkDic) return;

	//FvMemoryOStream& stream = m_pkDic->GetStream();
	//if(stream.Size())
	//{
	//	m_pkClientChannel->Bundle().StartMessage(ClientInterface::EntityPropertyUpdate, true);
	//	m_pkClientChannel->Bundle() << m_iEntityID;
	//	m_pkClientChannel->Bundle().AddBlob(stream.Data(), stream.Size());
	//	m_pkClientChannel->Send();
	//	stream.Reset();
	//}
}

void FvProxy::DestroyClientChannel(bool bSendLogOff)
{
	if(!m_pkClientChannel)
		return;

	if(bSendLogOff)
	{
		FvNetBundle& kBundle = ClientBundle();
		kBundle.StartMessage(ClientInterface::LoggedOff);
		FvUInt8	reason = 0;
		kBundle << reason;
		m_pkClientChannel->Send();
	}

	FvEntityManager::Instance().EraseProxy(m_pkClientChannel->addr());

	m_pkClientChannel->Destroy();
	m_pkClientChannel = NULL;
	m_bEntitiesEnabled = false;
}

void FvProxy::CellChannelClose()
{
	if(m_pkClientChannel && m_bEntitiesEnabled)
		SendResetToClient();//! 因为CellChannel关闭后,即使重开,Aoi也会改变,需要清理client,重建Aoi

	m_bEntitiesEnabled = false;
	m_bCellPlayerCreatedOnClient = false;
}

void FvProxy::CellChannelOpen()
{
	if(m_bEntitiesEnabled)
		SendEnableDisableWitness(true);
}

void FvProxy::CreateClientChannel(const FvNetAddress& kAddr)
{
	FV_ASSERT(!m_pkClientChannel && !kAddr.IsNone());
	m_pkClientChannel = new FvNetChannel(FvEntityManager::Instance().ExtNub(), kAddr, FvNetChannel::EXTERNAL);
	m_pkClientChannel->IsIrregular(true);
}

void FvProxy::CallClientMethod( FvInt32 iMethodIdx, FvBinaryIStream & data )
{
	FV_ASSERT(iMethodIdx < m_kAttrib.GetExport()->kMethodCnts.uiClientCnt);
	//FV_ASSERT(m_pkClientChannel);
	if(!m_pkClientChannel)
	{
		FV_ERROR_MSG("%s, No Client can be called\n", __FUNCTION__);
		data.Finish();
		return;
	}

	int baseId, extId;
	FvDataCompressID<6>(m_kAttrib.GetExport()->kMethodCnts.uiClientCnt, iMethodIdx, baseId, extId);

	static FvNetInterfaceElement anie = ClientInterface::EntityMessage;
	anie.SetID(baseId | 0xC0);
	FvNetBundle& kBundle = ClientBundle();
	kBundle.StartMessage(anie);
	kBundle << GetEntityID();
	if(extId != -1) kBundle << FvUInt8(extId);
	kBundle.Transfer(data, data.RemainingLength());
}

FvNetChannel* FvProxy::GetChannelForClientMethod(FvInt32 iMethodIdx)
{
	FV_ASSERT(iMethodIdx < m_kAttrib.GetExport()->kMethodCnts.uiClientCnt);
	//FV_ASSERT(m_pkClientChannel);
	if(!m_pkClientChannel)
	{
		FV_ERROR_MSG("%s, No Client can be called\n", __FUNCTION__);
		return NULL;
	}

	int baseId, extId;
	FvDataCompressID<6>(m_kAttrib.GetExport()->kMethodCnts.uiClientCnt, iMethodIdx, baseId, extId);

	static FvNetInterfaceElement anie = ClientInterface::EntityMessage;
	anie.SetID(baseId | 0xC0);
	FvNetBundle& kBundle = ClientBundle();
	kBundle.StartMessage(anie);
	kBundle << GetEntityID();
	if(extId != -1) kBundle << FvUInt8(extId);
	return m_pkClientChannel;
}

void FvProxy::SyncAttribToOwnCli(FvUInt8 uiMessageID, FvBinaryIStream& kIS)
{
	if(!EntitiesEnabled())
		return;

	static FvNetInterfaceElement kInterface = ClientInterface::EntityMessage;
	kInterface.SetID(uiMessageID | 0x80);
	FvNetBundle& kBundle = ClientBundle();
	kBundle.StartMessage(kInterface);
	kBundle << GetEntityID();
	kBundle.AddBlob(kIS.Retrieve(0), kIS.RemainingLength());
}

void FvProxy::SendToClient()
{
	ClientBundle().StartMessage(ClientInterface::TickSync);
	ClientBundle() << FvUInt8(FvEntityManager::Instance().GameTime());

	if(m_pkClientChannel)
		m_pkClientChannel->Send();

	if(m_pkCellChannel)
		m_pkCellChannel->Send();
}

void FvProxy::CallClientRPCCallBack(FvUInt8 uiCBID, FvBinaryIStream& kIS)
{
	if(!m_pkClientChannel)
	{
		FV_ERROR_MSG("%s, No Client can be called\n", __FUNCTION__);
		kIS.Finish();
		return;
	}

	FvNetBundle& kBundle = ClientBundle();
	kBundle.StartMessage(ClientInterface::RPCCallBack);
	kBundle << uiCBID;
	kBundle.Transfer(kIS, kIS.RemainingLength());
}

void FvProxy::CallClientRPCCallBackException(FvUInt8 uiCBID)
{
	if(!m_pkClientChannel)
	{
		FV_ERROR_MSG("%s, No Client can be called\n", __FUNCTION__);
		return;
	}

	ClientInterface::RPCCallBackExceptionArgs& kArgs =
		ClientInterface::RPCCallBackExceptionArgs::start(ClientBundle());
	kArgs.id = uiCBID;
}

void FvProxy::SetClientChannel(FvNetChannel* pkChannel)
{
	FV_ASSERT(!m_pkClientChannel && pkChannel);
	FV_ASSERT(!m_bCellPlayerCreatedOnClient && !m_bEntitiesEnabled);
	m_pkClientChannel = pkChannel;
	m_pkClientChannel->ClearBundle();
}

void FvProxy::SendEnableDisableWitness(bool enable, bool isRestore)
{
	FV_ASSERT(m_pkCellChannel && m_pkCellChannel->IsEstablished());

	if(enable)
		++m_uiSendEnableWitnessCnt;

	FvNetBundle& kBundle = m_pkCellChannel->Bundle();
	kBundle.StartMessage(CellAppInterface::EnableWitness);
	kBundle << GetEntityID() << enable;
	m_pkCellChannel->DelayedSend();
}

void FvProxy::SendResetToClient()
{
	FvNetBundle& kBundle = ClientBundle();
	kBundle.StartMessage(ClientInterface::ResetEntities);
	bool keepPlayerOnBase = false;
	kBundle << keepPlayerOnBase;
	m_pkClientChannel->Send();
}


