//{future header message}
#ifndef __FvRobotEntity_H__
#define __FvRobotEntity_H__


#include "FvRobotDefines.h"
#include <FvNetTypes.h>
#include <FvEntityData.h>
#include <FvDirection3.h>
#include "FvRobotMailBox.h"


class FvEntityDescription;
class FvServerConnection;
class FvRobotClientApp;


class FV_ROBOT_API FvEntity
{
public:
	FvEntity();
	virtual ~FvEntity();

	void				InitBase(FvRobotEntityExport* pkExport, FvServerConnection* pServer,
							FvEntityID id, FvEntityTypeID type, FvBinaryIStream & data, int iRobotID);
	void				InitCell(FvSpaceID spaceID, FvEntityID vehicleID, const FvPosition3D & pos,
							float yaw, float pitch, float roll, FvBinaryIStream & data);
	void				InitEntity(FvRobotEntityExport* pkExport, FvServerConnection* pServer,
							FvEntityID id, FvEntityTypeID type,
							FvSpaceID spaceID, FvEntityID vehicleID, const FvPosition3D & pos,
							float yaw, float pitch, float roll, FvBinaryIStream & data);

	bool				IsCtrler() const { return m_bCtrler; }
	FvEntityID			GetEntityID() const { return m_iEntityID; }
	FvEntityTypeID		GetEntityTypeID() const { return m_uiType; }
	int					GetRobotID() const { return m_iRobotID; }
	float&				GetSpeed()	{ return m_fSpeed; }
	FvVector3&			GetPos()	{ return m_kPos; }
	FvDirection3&		GetDir()	{ return m_kDir; }

	const float&		GetSpeed() const{ return m_fSpeed; }
	const FvVector3&	GetPos() const	{ return m_kPos; }
	const FvDirection3&	GetDir() const	{ return m_kDir; }

	void				SetPos(const FvVector3& pos) { m_kPos=pos; }
	void				SetDir(const FvDirection3& dir) { m_kDir=dir; }

	FvRobotAttrib&		GetAttrib() { return m_kAttrib; }
	bool				Dump(const char* fileName);

	//! 进入世界
	void				EnterWorld( FvSpaceID spaceID, FvEntityID vehicleID, bool transient );
	//! 离开世界
	void				LeaveWorld( bool transient );

	void				OnEntityMove(FvSpaceID spaceID, FvEntityID vehicleID, const FvVector3 & pos, float yaw, float pitch, float roll, FvUInt32 moveFlag, bool isVolatile);
	void				OnEntityPropertys( FvBinaryIStream & data );
	void				OnEntityProperty(int messageID, FvBinaryIStream& data);
	void				OnEntityMethod( int messageID, FvBinaryIStream & data );

	virtual	void		Initialize() {}
	virtual	void		UnInitialize() {}
	virtual void		Tick(const float fDeltaTime) {}
	virtual	void		OnKeyDown(int keyVal, bool bRep) {}
	virtual	void		OnKeyUp(int keyVal) {}

	const FvMailBox&	ClientMBInEntity() const { return m_kMailBox; }

	FvSpaceID			GetSpaceID() const { return m_iSpaceID; }
	void				SetSpaceID(FvSpaceID id) { m_iSpaceID=id; }

	void				SetClientApp(FvRobotClientApp* pkApp) { m_pkClientApp=pkApp; }
	FvRobotClientApp*	GetClientApp() const { return m_pkClientApp; }

	void				DumpSizeAndCnt();

protected:

	FvEntityID				m_iEntityID;
	FvEntityTypeID			m_uiType;
	bool					m_bCtrler;
	float					m_fSpeed;
	FvVector3				m_kPos;
	FvDirection3			m_kDir;
	FvSpaceID				m_iSpaceID;
	int						m_iRobotID;

	FvRobotAttrib			m_kAttrib;

	FvServerConnection*		m_pkServer;
	FvMailBox				m_kMailBox;

	FvEntity*				m_pkEntityPre;
	FvEntity*				m_pkEntityNex;
	friend class FvRobotMainApp;

	FvRobotClientApp*		m_pkClientApp;
};


#endif//__FvRobotEntity_H__
