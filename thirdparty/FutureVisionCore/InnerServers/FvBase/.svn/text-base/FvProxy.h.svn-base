//{future header message}
#ifndef __FvProxy_H__
#define __FvProxy_H__

#include "FvBaseDefines.h"
#include "FvBaseEntity.h"
#include "FvBaseEntityManager.h"
#include "FvBaseAppIntInterface.h"

#include <FvMemoryStream.h>
#include <FvConcurrency.h>
#include "FvBaseAppExtInterface.h"
#include "FvClientInterface.h"
#include <FvNetChannel.h>

//#include "FvBaseMailbox.h"
//#include "FvLoadingThread.h"

class ClientEntityMailBox;
class BaseEntityMailBox;
class FvMemoryOStream;


class FV_BASE_API FvProxy: public FvEntity
{
public:
	static const int MAX_INCOMING_PACKET_SIZE = 1024;
	static const int MAX_OUTGOING_PACKET_SIZE = 1024;

	FvProxy();
	~FvProxy();

	//! 脚本调用接口
	void			GiveClientTo(FvProxy* pkDest);
	//! 

	//! CallBack
	virtual void	OnClientDeath() {}
	virtual void	OnClientGetCell() {}
	virtual void	OnEntitiesEnabled() {}
	//virtual void	OnLogOnAttempt(ip, port, password );
	//! 

	//! external interface
	void			AvatarUpdateImplicit( const BaseAppExtInterface::AvatarUpdateImplicitArgs & args );
	void			AvatarUpdateExplicit( const BaseAppExtInterface::AvatarUpdateExplicitArgs & args );
	void			AvatarUpdateWardImplicit( const BaseAppExtInterface::AvatarUpdateWardImplicitArgs & args );
	void			AvatarUpdateWardExplicit( const BaseAppExtInterface::AvatarUpdateWardExplicitArgs & args );
	void			AckPhysicsCorrection( const BaseAppExtInterface::AckPhysicsCorrectionArgs & args );
	void			AckWardPhysicsCorrection( const BaseAppExtInterface::AckWardPhysicsCorrectionArgs & args );
	void			RequestEntityUpdate( FvBinaryIStream & data );
	void			EnableEntities( const BaseAppExtInterface::EnableEntitiesArgs & args );
	void			RestoreClientAck( const BaseAppExtInterface::RestoreClientAckArgs & args );
	void			DisconnectClient( const BaseAppExtInterface::DisconnectClientArgs & args );
	void			GMCmd( FvBinaryIStream & data );
	//! internal interface
	void			SendToClient( const BaseAppIntInterface::SendToClientArgs & args );
	void			TickSync( const BaseAppIntInterface::TickSyncArgs & args );
	void			CreateCellPlayer( FvBinaryIStream & data );
	void			SpaceData( FvBinaryIStream & data );
	void			EnterAoI( const BaseAppIntInterface::EnterAoIArgs & args );
	void			EnterAoIOnVehicle( const BaseAppIntInterface::EnterAoIOnVehicleArgs & args );
	void			LeaveAoI( FvBinaryIStream & data );
	void			SetVehicle( const BaseAppIntInterface::SetVehicleArgs & args );
	void			CreateEntity( FvBinaryIStream & data );
	void			UpdateEntity( FvBinaryIStream & data );
	void			DetailedPosition( const BaseAppIntInterface::DetailedPositionArgs & args );
	void			ForcedPosition( const BaseAppIntInterface::ForcedPositionArgs & args );
	void			ModWard( const BaseAppIntInterface::ModWardArgs & args );
	void			CallClientMethod( const FvNetAddress & addr, const FvNetUnpackedMessageHeader & header, FvBinaryIStream & data );
	//! 

	bool			HasClient() const { return m_pkClientChannel != NULL; }
	bool			EntitiesEnabled() const	{ return m_bEntitiesEnabled; }
	FvSessionKey	SessionKey() const	{ return m_uiSessionKey; }
	void			PipeIntMessage( int type, FvBinaryIStream & data, FvUInt32 length );
	void			PipeExtMessage( int type, FvBinaryIStream & data, FvUInt32 length );
	void			HandleExtMessage( int type, FvBinaryIStream & data, FvUInt32 length );
	const FvString& EncryptionKey() const { return m_kEncryptionKey; }
	void			EncryptionKey( const FvString & data ) { m_kEncryptionKey = data; }
	void			SyncPropertyUpdate();
	void			DestroyClientChannel(bool bSendLogOff);
	void			CellChannelClose();
	void			CellChannelOpen();
	void			CreateClientChannel(const FvNetAddress& kAddr);
	FvNetChannel&	ClientChannel() { FV_ASSERT(m_pkClientChannel); return *m_pkClientChannel; }
	void			CallClientMethod( FvInt32 iMethodIdx, FvBinaryIStream & data );
	FvNetChannel*	GetChannelForClientMethod(FvInt32 iMethodIdx);
	bool			IsCellPlayerCreatedOnClient() { return m_bCellPlayerCreatedOnClient; }
	void			SyncAttribToOwnCli(FvUInt8 uiMessageID, FvBinaryIStream& kIS);
	void			SendToClient();
	void			CallClientRPCCallBack(FvUInt8 uiCBID, FvBinaryIStream& kIS);
	void			CallClientRPCCallBackException(FvUInt8 uiCBID);

protected:
	void			SetClientChannel(FvNetChannel* pkChannel);
	FvNetBundle&	ClientBundle() { FV_ASSERT(m_pkClientChannel); return m_pkClientChannel->Bundle(); }
	void			SendEnableDisableWitness( bool enable = true, bool isRestore = false );
	void			SendResetToClient();

	FvNetChannel*		m_pkClientChannel;

	FvString			m_kEncryptionKey;
	FvSessionKey		m_uiSessionKey;

	bool				m_bEntitiesEnabled;
	bool				m_bCellPlayerCreatedOnClient;
	FvUInt32			m_uiSendEnableWitnessCnt;

};


#endif // __FvProxy_H__
