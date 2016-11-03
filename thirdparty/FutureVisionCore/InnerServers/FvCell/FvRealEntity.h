//{future header message}
#ifndef __FvRealEntity_H__
#define __FvRealEntity_H__

#include "FvCellDefines.h"
#include "FvCellAppInterface.h"
//#include "FvEntityCache.h"
#include "FvHistoryEvent.h"
#include "FvCellEntity.h"

#include <FvNavigator.h>
#include <FvNetChannel.h>


class FvMemoryOStream;
class FvSpace;
class FvWitness;



class FV_CELL_API FvRealEntity
{
public:
	FvRealEntity( FvEntity & owner );
	~FvRealEntity();

	bool			Init(FvBinaryIStream* pkData, CreateRealInfo createRealInfo);
	void			Destroy();
	void			Offload(FvBinaryOStream& kStream, const FvNetAddress& kDstAddr, bool bTeleport);

	FvEntity&		Entity()			{ return m_kEntity; }
	const FvEntity& Entity() const		{ return m_kEntity; }
	FvWitness*		pWitness()			{ return m_pkWitness; }
	const FvWitness*pWitness() const	{ return m_pkWitness; }
	FvNetChannel*	GetChannel() const	{ return m_pkChannel; }

	void			EnableWitness(FvBinaryIStream& kData);
	void			DisableWitness();
	void			AvatarUpdateForOtherClient(FvEntityID iEntityID, FvUInt32 uiTime, FvVector3& kPos, FvDirection3& kDir, FvUInt8 uiFlag);
	void			CheckGhosts();
	void			CreateGhostIfNo(FvUInt16 uiCellIdx, FvNetAddress& kAddr);
	void			AvatarUpdateToGhosts(FvVector3& kPos, FvDirection3& kDir);
	void			SendHistoryEventToGhosts(bool bPropEvent, FvUInt8 uiMessageID, FvBinaryIStream& kMsg, FvEventNumber uiEventNumber, GhostUpdateNumber uiGhostUpdateNumber, FvDetailLevel uiLevel);
	void			SendGhostDataToGhosts(FvUInt8 uiMessageID, FvBinaryIStream& kMsg, GhostUpdateNumber uiGhostUpdateNumber);
	void			SendVisibleToGhosts(FvUInt16 uiObjMask, float fVisibility);
	bool			IsGhostsWitnessed();
	void			UpdateGhostWitnessed(FvUInt16 uiCellIdx, bool& bWitnessedOld, bool& bWitnessedNew);
	void			SetWitnessed(bool bWitnessed);
	FvInt16			AddTrap(FvUInt16 uiMask, float fVision, float fDisVisibility, FvInt32 iUserData);
	void			DelTrap(FvInt16 iTrapID);
	int				FindTrap(FvInt16 iTrapID) const;
	void			SetTrapMask(FvInt16 iTrapID, FvUInt16 uiMask);
	void			SetTrapVision(FvInt16 iTrapID, float fVision);
	void			SetTrapDisVisibility(FvInt16 iTrapID, float fDisVisibility);
	void			MoveTraps();
	void			CheckTraps();
	void			EntitiesInTrap(FvInt16 iTrapID, FvEntity::Entities& kEntities) const;
	void			GetTraps(std::vector<float>& kTraps);
	void			DeleteAllGhosts();
	void			CreateGhostData(FvBinaryOStream& stream);
	void			CloseTrapsBeforeTeleportLocally();
	void			OpenTrapsAfterTeleportLocally();

	const FvNavLoc &NavLoc() const { return m_kNavLoc; }
	void NavLoc( const FvNavLoc &kLoc ) { m_kNavLoc = kLoc; }

	FvNavigator &Navigator() { return m_kNavigator; }

	bool			HasVehicle() { return m_spVehicle != NULL; }
	bool			HasPassenger() { return !m_kPassengers.empty(); }
	bool			BoardVehicle(FvEntityPtr spVehicle, BoardVehicleCallBack* pkCallBack);	//! 上载具,乘客使用
	void			AlightVehicle();														//! 下载具,乘客使用
	void			MoveVehicle(FvMemoryOStream& kData);									//! 移动载具,载具使用
	void			CloseVehicle();															//! 关闭载具,载具使用
	enum
	{
		VEHICLE_MSGID_BOARD_REQ = 0,
		VEHICLE_MSGID_BOARD_REP,
		VEHICLE_MSGID_ALIGHT,
		VEHICLE_MSGID_MOVE,
		VEHICLE_MSGID_CLOSE,
		VEHICLE_MSGID_SETID,
	};
	void			BoardVehicleReq(FvEntityID iPassengerID, const FvNetAddress& kAddr);
	void			BoardVehicleRep(bool bSuccess, FvEntityID iVehicleID);
	void			AlightVehicleReq(FvEntityID iPassengerID);
	void			MoveVehicleReq(FvEntityID iVehicleID, FvBinaryIStream& kData);
	void			CloseVehicleReq(FvEntityID iVehicleID);
	void			SyncVehicleIDToRealAndGhosts(FvEntityID iVehicleID);

protected:
	void			CreateGhost(FvNetChannel* pkChannel);
	void			DeleteGhost(FvNetChannel* pkChannel);
	FvInt16			GetTrapID();
	void			PutTrapID(FvInt16 iTrapID);

public:
	class Haunt
	{
	public:
		Haunt(FvNetChannel* pChannel, FvUInt16 uiCellIdx, bool bWitnessed):m_pkChannel(pChannel),m_uiCellIdx(uiCellIdx),m_bWitnessed(bWitnessed){}

		FvNetChannel&		Channel()			{ return *m_pkChannel; }
		FvNetBundle&		Bundle()			{ return m_pkChannel->Bundle(); }
		const FvNetAddress&	Addr() const		{ return m_pkChannel->addr(); }
		FvUInt16			CellIdx()			{ return m_uiCellIdx; }
		bool				IsWitnessed()		{ return m_bWitnessed; }
		void				ChangeWitnessed()	{ m_bWitnessed=!m_bWitnessed; }

	private:
		FvNetChannel*	m_pkChannel;
		FvUInt16		m_uiCellIdx;
		bool			m_bWitnessed;
	};

	class Trap
	{
	public:
		Trap(FvEntity& kEntity):m_kEntity(kEntity),m_iTrapID(-1),m_iUserData(0),m_hTrapObserver(FVAOI_NULL_HANDLE),m_uiMask(0),m_fVision(0.0f),m_fDisVisibility(0.0f) {}
		~Trap();

		bool				Init(FvInt16 iTrapID, FvUInt16 uiMask, float fVision, float fDisVisibility, FvInt32 iUserData);
		FvInt16				GetTrapID() { return m_iTrapID; }
		FvInt32				GetUserData() { return m_iUserData; }
		void				Move();
		void				CheckTrap();
		void				EntitiesInTrap(FvEntity::Entities& kEntities) const;
		void				SetMask(FvUInt16 uiMask);
		void				SetVision(float fVision);
		void				SetDisVisibility(float fDisVisibility);
		FvUInt16			GetMask() const { return m_uiMask; }
		float				GetVision() const { return m_fVision; }
		float				GetDisVisibility() const { return m_fDisVisibility; }
		void				OpenTrap();
		void				CloseTrapAndInform();

	protected:
		FvEntity&		m_kEntity;
		FvInt16			m_iTrapID;
		FvInt32			m_iUserData;
		FvAoIHandle		m_hTrapObserver;
		FvUInt16		m_uiMask;
		float			m_fVision;
		float			m_fDisVisibility;

		FV_IOSTREAM_FRIEND_DECLARE;
	};

private:
	FvEntity&			m_kEntity;
	FvWitness*			m_pkWitness;
	FvNetChannel*		m_pkChannel;
	bool				m_bWitnessed;
	typedef std::vector< Haunt > Haunts;
	Haunts				m_kHaunts;
	typedef std::vector<Trap*> TrapList;
	TrapList			m_kTrapList;
	FvInt16				m_iNextTrapID;
	std::vector<FvInt16> m_kFreeTrapIDList;
	FvNavLoc m_kNavLoc;
	FvNavigator m_kNavigator;

	FvEntityPtr			m_spVehicle;
	typedef std::vector<FvEntityPtr> Passengers;
	Passengers			m_kPassengers;
	BoardVehicleCallBack*m_pkBoardVehicleCallBack;
};

FV_IOSTREAM_DECLARE(, FvRealEntity::Trap);


#include "FvRealEntity.h"


#endif // __FvRealEntity_H__



