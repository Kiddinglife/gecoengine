//{future header message}
#ifndef __FvRobotClientApp_H__
#define __FvRobotClientApp_H__

#include "FvRobotDefines.h"
#include "FvServerConnection.h"
#include <FvSpaceDataTypes.h>
#include <FvSmartPointer.h>

class FvRobotMovementController;
class FvEntity;
class FvRobotClientApp;
typedef FvSmartPointer<FvRobotClientApp> FvRobotClientAppPtr;
class FvTimeOfDay;


class FV_ROBOT_API FvRobotClientApp : public FvSafeReferenceCount,
	public FvServerMessageHandler,
	public FvNetInputNotificationHandler
{
public:
	FvRobotClientApp(FvNetNub& kMainNub, int iRobotID, const FvString& kTag, const FvString& kName, const FvString& kPassword, const FvVector3& kBirthPos);
	virtual ~FvRobotClientApp();

	bool Tick(float fTime);

	void LogOn();
	void LogOff();
	void DropConnection();
	void SetConnectionLossRatio(float fLossRatio);
	void SetConnectionLatency(float fLatencyMin, float fLatencyMax);
	bool SetMovementController(const FvString& kType, const FvString& kData);
	void Destroy();
	void OnKeyDown(int keyVal, bool bRep);
	void OnKeyUp(int keyVal);

	double	GetToD();
	float	GetToDScaling() const;

	FvServerConnection* GetServerConnection() { return &m_kServerConnection; }
	int RobotID() const					{ return m_iRobotID; }
	const FvString& Tag() const			{ return m_kTag; }
	typedef std::map<FvEntityID, FvEntity*> EntityMap;
	const EntityMap& GetEntityMap()const{ return m_kEntityMap; }
	FvSpaceID SpaceID() const			{ return m_iSpaceID; }
	FvUInt16 SpaceType() const			{ return m_uiSpaceType; }
	FvEntityID PlayerID() const			{ return m_iCtrlEntityID; }
	FvEntityID VehicleID() const		{ return m_iVehicleID; }
	bool IsOnline() const				{ return m_kServerConnection.Online(); }
	bool IsDestroyed() const			{ return m_bIsDestroyed; }
	const FvString& UserName() const	{ return m_kUserName; }
	const FvString& Password() const	{ return m_kUserPasswd; }
	const FvVector3& BirthPos() const	{ return m_kBirthPos; }

	FvEntity*FindEntity(FvEntityID iID);

	//////////////////////////////////////////////////////////////////////////////////
	virtual void OnBasePlayerCreate( FvEntityID id, FvEntityTypeID type,
		FvBinaryIStream & data );
	virtual void OnCellPlayerCreate( FvEntityID id,
		FvSpaceID spaceID, FvEntityID vehicleID, const FvPosition3D & pos,
		float yaw, float pitch, float roll,
		FvBinaryIStream & data );
	virtual void OnEntityEnter( FvEntityID id, FvSpaceID spaceID, FvEntityID );
	virtual void OnEntityLeave( FvEntityID id, const FvCacheStamps & stamps );
	virtual void OnEntityCreate( FvEntityID id, FvEntityTypeID type,
		FvSpaceID spaceID, FvEntityID vehicleID, const FvPosition3D & pos,
		float yaw, float pitch, float roll,
		FvBinaryIStream & data );
	virtual void OnEntityProperties( FvEntityID id, FvBinaryIStream & data );
	virtual void OnEntityProperty( FvEntityID entityID, int propertyID,
		FvBinaryIStream & data );
	virtual void OnEntityMethod( FvEntityID entityID, int methodID,
		FvBinaryIStream & data );
	virtual void OnEntityMove(
		FvEntityID entityID, FvSpaceID spaceID, FvEntityID vehicleID,
		const FvPosition3D & pos, float yaw, float pitch, float roll,
		bool isVolatile );
	virtual void SetTime( FvTimeStamp gameTime,
		float initialTimeOfDay, float gameSecondsPerSecond );
	virtual void SpaceData( FvSpaceID spaceID, FvSpaceEntryID& entryID,
		FvUInt16 key, const FvString & data );
	virtual void SpaceGone( FvSpaceID spaceID );
	virtual void OnProxyData( FvUInt16 proxyDataID, FvBinaryIStream & data );
	virtual void OnEntitiesReset( bool keepPlayerOnBase );
	virtual void OnStreamComplete( FvUInt16 id, const FvString &desc,
		FvBinaryIStream &data );
	//////////////////////////////////////////////////////////////////////////////////

	virtual int HandleInputNotification( int );

protected:
	void	ClearEntities();

protected:
	FvServerConnection	m_kServerConnection;
	EntityMap			m_kEntityMap;

	int					m_iRobotID;
	FvString			m_kTag;
	FvSpaceID			m_iSpaceID;
	FvUInt16			m_uiSpaceType;
	FvEntityID			m_iCtrlEntityID;
	FvEntityID			m_iVehicleID;
	FvEntity*			m_pkCtrlEntity;

	FvTimeOfDay*		m_pkServerToD;

	FvLoginHandlerPtr	m_spLoginInProgress;
	bool				m_bIsDestroyed;
	bool				m_bIsDormant;

	FvNetNub&			m_kMainNub;
	FvString			m_kUserName;
	FvString			m_kUserPasswd;
	FvVector3			m_kBirthPos;
	FvRobotMovementController* m_pkMoveCtrller;

	FvUInt32			m_uiUpdateMoveTime;
	FvUInt32			m_uiUpdateMoveInterval;
	FvUInt32			m_uiPingPongTime;
	FvUInt32			m_uiPingPongInterval;
};

#endif//! __FvRobotClientApp_H__
