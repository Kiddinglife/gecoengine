#include "FvRobotEntity.h"
#include <FvEntityDescriptionMap.h>
#include "FvServerConnection.h"
#include "FvRobotCfg.h"


FvEntity::FvEntity()
:m_iEntityID(0)
,m_uiType(0)
,m_bCtrler(false)
,m_fSpeed(FvRobotCfg::Instance().GetMoveSpeed())
,m_iSpaceID(0)
,m_iRobotID(-1)
,m_pkServer(NULL)
,m_pkEntityPre(NULL)
,m_pkEntityNex(NULL)
,m_pkClientApp(NULL)
{

}

FvEntity::~FvEntity()
{

}

void FvEntity::InitBase(FvRobotEntityExport* pkExport, FvServerConnection* pServer,
						FvEntityID id, FvEntityTypeID type, FvBinaryIStream & data, int iRobotID)
{
	m_bCtrler = true;
	m_kAttrib.CreateOwnData(id, this, pkExport);
	m_pkServer = pServer;
	m_iEntityID = id;
	m_uiType = type;
	m_kMailBox.Init(this, m_pkServer);
	m_iRobotID = iRobotID;

	m_kAttrib.DeserializeFromStreamForBaseData(this, data);
}

void FvEntity::InitCell(FvSpaceID spaceID, FvEntityID vehicleID, const FvPosition3D & pos,
						float yaw, float pitch, float roll, FvBinaryIStream & data)
{
	m_iSpaceID = spaceID;
	m_kPos = pos;
	m_kDir.SetYaw(yaw);
	m_kDir.SetPitch(pitch);
	m_kDir.SetRoll(roll);

	m_kAttrib.DeserializeFromStreamForOwnCliData(this, data);
}

void FvEntity::InitEntity(FvRobotEntityExport* pkExport, FvServerConnection* pServer,
						  FvEntityID id, FvEntityTypeID type,
						  FvSpaceID spaceID, FvEntityID vehicleID, const FvPosition3D & pos,
						  float yaw, float pitch, float roll, FvBinaryIStream & data)
{
	m_bCtrler = false;
	m_kAttrib.CreateOthersData(id, this, pkExport);
	m_pkServer = pServer;
	m_iEntityID = id;
	m_uiType = type;
	m_iSpaceID = spaceID;
	m_kPos = pos;
	m_kDir.SetYaw(yaw);
	m_kDir.SetPitch(pitch);
	m_kDir.SetRoll(roll);
	m_kMailBox.Init(this, m_pkServer);

	//! 读出Lod可见的属性
	m_kAttrib.OnProperties(this, data);
}

bool FvEntity::Dump(const char* fileName)
{
	return true;
}

void FvEntity::DumpSizeAndCnt()
{

}

void FvEntity::OnEntityMove(FvSpaceID spaceID, FvEntityID vehicleID, const FvVector3 & pos, float yaw, float pitch, float roll, FvUInt32 moveFlag, bool isVolatile)
{

}

void FvEntity::OnEntityPropertys( FvBinaryIStream & data )
{
	//! 读出Lod可见的属性
	m_kAttrib.OnProperties(this, data);
}

void FvEntity::OnEntityProperty(int messageID, FvBinaryIStream& data)
{
	m_kAttrib.OnProperty(IsCtrler(), this, messageID, data);
}

void FvEntity::OnEntityMethod( int messageID, FvBinaryIStream & data )
{
	m_kAttrib.OnMethod(this, messageID, data);
}

void FvEntity::EnterWorld( FvSpaceID spaceID, FvEntityID vehicleID, bool bTransient )
{

}

void FvEntity::LeaveWorld( bool bTransient )
{

}













