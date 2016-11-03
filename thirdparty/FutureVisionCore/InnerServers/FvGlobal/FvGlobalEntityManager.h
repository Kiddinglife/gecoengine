//{future header message}
#ifndef __FvGlobalEntityManager_H__
#define __FvGlobalEntityManager_H__

#include "FvGlobalDefines.h"

#include <FvSingleton.h>
#include <FvNetNub.h>
#include <FvEntityData.h>

#include "FvGlobalAppInterface.h"
#include "FvGlobalIDPool.h"
#include "FvGlobalEntity.h"


class FvMailBox;
struct FvAllGlobalEntityExports;


class FV_GLOBAL_API FvEntityManager : public FvNetTimerExpiryHandler, public FvSingleton< FvEntityManager >
{
public:
	FvEntityManager(FvNetNub& kNub);
	~FvEntityManager();

	bool			Init(int iArgc, char* pcArgv[]);
	void			ShutDown();

	FvEntity*		FindEntity(FvEntityID iID) const;
	FvEntity*		CreateEntity(const char* pcEntityTypeName, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	FvEntity*		CreateEntity(FvEntityTypeID uiEntityType, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);
	void			CreateEntityInNewGlobalSpace(const FvAllData& kAllData, FvUInt8 uiInitFlg=FV_ENTITY_DEF_INIT_FLG);//! TODO: 增加失败回调
	FvMailBox*		FindGlobalEntity(const char* pcLabel) const;
	FvMailBox*		FindGlobalEntity(const FvString& kLabel) const;

	//! internal interface
	void			HandleCellAppMgrBirth(const InterfaceListenerMsg& kMsg);
	void			Register(const FvNetAddress& kAddr, const GlobalAppInterface::RegisterArgs& kArgs);
	void			Deregister(const FvNetAddress& kAddr, const GlobalAppInterface::DeregisterArgs& kArgs);
	void			PutEntityIDs(const FvNetAddress& kAddr, const FvNetUnpackedMessageHeader& kHeader, FvBinaryIStream& kData);
	void			GetEntityIDs(const FvNetAddress& kAddr, const FvNetUnpackedMessageHeader& kHeader, FvBinaryIStream& kData);
	//! 

	FvNetNub&		Nub() { return m_kNub; }
	const FvNetAddress& Address() const;
	FvNetChannel*	FindChannel(const FvNetAddress& kAddr) { return m_kNub.FindChannel(kAddr); }
	FvNetChannel*	FindOrCreateChannel(const FvNetAddress& kAddr) { return &m_kNub.FindOrCreateChannel(kAddr); }
	void			AddEntity(FvEntity* pkEntity);
	void			DelEntity(FvEntity* pkEntity);
	bool			AddGlobalMailBox(const char* pcName, FvMailBox& kMailBox);
	bool			DelGlobalMailBox(const char* pcName, FvMailBox& kMailBox);
	void			OnEntityDestroyed(FvEntity* pkEntity);
	void			AddDestroyEntity(FvEntity* pkEntity) { m_kDestroyEntities.push_back(pkEntity); }

protected:
	virtual int		HandleTimeout( FvNetTimerID id, void * arg );
	void			StartUpdate() { ++m_iUpdateFlag; }
	void			StopUpdate() { --m_iUpdateFlag; }
	void			ClearAddDelEntityList();
	void			CheckDelEntities();
	void			BroadCastGlobalMailBox(bool bAdd, const char* pcName, FvMailBox* pkMailBox);

private:
	FvNetNub&				m_kNub;
	FvNetAddress			m_kGlobalCellAppMgr;

	typedef std::map<FvEntityID, FvEntityPtr> Entities;
	Entities				m_kEntities;
	FvAllGlobalEntityExports*m_pkEntityExports;

	FvGlobalIDPool			m_kGlobalEntityIDPool;

	enum TimeOutType
	{
		TIMEOUT_GAME_TICK
	};
	FvTimeStamp				m_uiTime;
	FvInt32					m_iUpdateHertz;
	FvInt32					m_iSystemManagePeriod;

	typedef std::map<FvString, FvMailBox> GlobalMap;
	GlobalMap				m_kGlobalMap;

	class GlobalsListener
	{
	public:
		GlobalsListener(bool bBaseApp, const FvNetAddress& kAddr);
		~GlobalsListener();

		void	AddGlobal(const char* pcName, FvMailBox& kMailBox);
		void	RemoveGlobal(const char* pcName);
		bool	IsBaseApp() const { return m_bBaseApp; }
		const FvNetAddress& Address() const;

	protected:
		bool			m_bBaseApp;
		FvNetChannel*	m_pkChannel;
	};
	typedef std::vector<GlobalsListener*> GlobalsListeners;
	GlobalsListeners		m_kGlobalsListeners;

	int						m_iUpdateFlag;//! 防止在容器迭代过程中对容器进行操作
	struct AddDelEntity
	{
		AddDelEntity(FvEntity* pkEntity, int iflg)
		:m_pkEntity(pkEntity),m_iFlag(iflg)
		{}

		FvEntity*	m_pkEntity;
		int			m_iFlag;	//! 0:del, 1:add
	};
	typedef std::vector<AddDelEntity> AddDelEntityList;
	AddDelEntityList		m_kAddDelEntityList;

	typedef std::vector<FvEntity*> DelEntityList;
	DelEntityList			m_kDelEntityList;

	typedef std::vector<FvEntityPtr> DestroyEntities;
	DestroyEntities			m_kDestroyEntities;
};



#endif // __FvGlobalEntityManager_H__
