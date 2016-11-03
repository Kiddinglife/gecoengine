//{future header message}
#ifndef __FvRobotMainApp_H__
#define __FvRobotMainApp_H__

#include <FvMD5.h>
#include <FvNetNub.h>
#include <FvNetPublicKeyCipher.h>
#include <FvEntityExport.h>
#include "FvRobotDefines.h"
#include "FvRobotCfg.h"
#include <FvSingleton.h>
#include <list>
#include "FvRobotUtility.h"

class FvRobotClientApp;
typedef FvSmartPointer<FvRobotClientApp> FvRobotClientAppPtr;
class FvRobotMovementController;
class FvRobotMovementFactory;
struct MessageFile;


class FV_ROBOT_API FvRobotMainApp : public FvNetTimerExpiryHandler,
	public FvSingleton< FvRobotMainApp >
{
public:
	FvRobotMainApp();
	virtual ~FvRobotMainApp();

	bool		Init(int iArgc, char* pcArgv[]);
	void		Run();
	void		ShutDown();

	void		SetFileLog(MessageFile*	pkMsgFile) { m_pkMsgFile=pkMsgFile; }
	FvRobotCfg::Result&	DoCmd(const char* pcCmd);
	void		ChangeSingleMode();

	FvRobotMovementController* CreateMovementController(float& fSpeed, FvVector3& kPosition,
		const FvString& kCtrllerType, const FvString& kCtrllerData);
	static void AddFactory(const FvString& kName, FvRobotMovementFactory& kFactory);

	FvRobotClientApp*FindApp(int iRobotID) const;

	FvNetNub&	Nub()										{ return m_kNub; }
	FvAllRobotEntityExports*GetEntityExports()				{ return m_pkEntityExports; }
	const FvMD5::Digest& GetDigest() const					{ return m_pkEntityExports->kMD5Digest; }

	void		AddEntity(FvEntity* pkEntity);
	void		RemoveEntity(FvEntity* pkEntity);

	void		SetSpaceType(FvSpaceID iSpaceID, FvUInt16 uiSpaceType);

	struct SpaceInfo
	{
		FvUInt16	uiSpaceType;
		FvVector4	kSpaceRect;
		typedef std::vector<FvVector4> Cells;
		Cells		kCells;
	};
	typedef std::vector<SpaceInfo> SpaceList;
	SpaceList&	GetSpaceList() { return m_kSpaceList; }
	const SpaceInfo*FindSpaceInfo(FvUInt16 uiSpaceType) const;

	typedef std::map<FvEntityID, FvEntity*> Entities;
	struct SpaceEntity
	{
		FvSpaceID	m_iSpaceID;
		FvUInt16	m_uiSpaceType;
		Entities	m_kEntities;

		SpaceEntity():m_iSpaceID(0),m_uiSpaceType(0xFFFF) {}
	};
	typedef std::list<SpaceEntity> SpaceEntityList;
	SpaceEntityList&GetSpaceEntityList() { return m_kSpaceEntityList; }

	FvRobotClientAppPtr GetRobotInSingleMode() { return m_spRobotInSingleMode; }

protected:
	virtual int HandleTimeout(FvNetTimerID, void*);
	bool		AddBot(const FvString& kTag, int iRobotID);
	SpaceEntity*FindSpaceEntity(FvSpaceID iSpaceID);

private:
	FvNetNub		m_kNub;
	bool			m_bStop;

	FvRobotClientAppPtr m_spRobotInSingleMode;

	int				m_iTickCnt;
	FvNetTimerID	m_iTimerID;

	typedef std::list<FvRobotClientAppPtr> RoBots;
	RoBots			m_kRoBots;
	RoBots::iterator m_itrClientTickIndex;

	FvAllRobotEntityExports* m_pkEntityExports;

	SpaceList		m_kSpaceList;
	SpaceEntityList	m_kSpaceEntityList;

	IDAllocate		m_kIDAllocate;
	CreateMgr		m_kCreateMgr;
	DeadMgr			m_kDeadMgr;

	MessageFile*	m_pkMsgFile;
};


#endif//! __FvRobotMainApp_H__
