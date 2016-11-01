//{future header message}
#ifndef __FvEntityData_H__
#define __FvEntityData_H__

#include "FvDataOwner.h"
#include <FvNetTypes.h>
#include <FvSmartPointer.h>

class FvEntity;
class FvDataObj;
class FvBinaryOStream;
class FvBinaryIStream;
struct FvClientEntityExport;
struct FvBaseEntityExport;
struct FvCellEntityExport;
struct FvGlobalEntityExport;
struct FvRobotEntityExport;
class FvXMLSection;
typedef FvSmartPointer<FvXMLSection> FvXMLSectionPtr;
class FvCallFuncByStrHandler;


//! 属性标签
enum FvDataFlags
{
	FV_DATA_GHOSTED			= 0x01,		///< Synchronised to ghost entities.
	FV_DATA_OTHER_CLIENT	= 0x02,		///< Sent to other clients.
	FV_DATA_OWN_CLIENT		= 0x04,		///< Sent to own client.
	FV_DATA_BASE			= 0x08,		///< Sent to the base.
	FV_DATA_CLIENT_ONLY		= 0x10,		///< Static client-side data only.
	FV_DATA_PERSISTENT		= 0x20,		///< Saved to the database.
	FV_DATA_EDITOR_ONLY		= 0x40,		///< Only read and written by editor.
	FV_DATA_ID				= 0x80,		///< Is an indexed column in the database.
};


//! ClientOwnData和ClientOthersData基类
class FV_ENTITYDEF_API FvClientData
{
public:
	FvClientData() {}
	virtual ~FvClientData() {}

	FvUInt32		Size(FvUInt32*& pAddr) const;
	FvDataObj*		Get(FvUInt32* pAddr, FvUInt32 uiIdx) const;

protected:
	virtual void	GetDataAddr(FvUInt32& uiCnt, FvUInt32*& pAddr) const = 0;
};


//! Base端CellData的基类
struct FV_ENTITYDEF_API FvCellData
{
	FvSpaceID		m_iSpaceID;
	FvUInt16		m_uiSpaceType;
	FvVector3		m_kPos;
	FvDirection3	m_kDir;


	FvCellData():m_iSpaceID(FV_NULL_SPACE),m_uiSpaceType(0) {}
	virtual ~FvCellData() {}

	virtual void	InitDefaultValForNoDBData() {}

	virtual bool	SerializeToStreamByIdx(FvUInt16 uiIdx, FvBinaryOStream& kOS) const = 0;
	virtual bool	SerializeToStreamForDBData(FvBinaryOStream& kOS) const = 0;				//! 在属性流化之后,依次流化pos,dir,spaceid,spacetype
	virtual bool	SerializeToStreamForAllData(FvBinaryOStream& kOS) const = 0;			//! 只需要流化Cell属性,不需要pos等

	virtual bool	DeserializeFromStreamByIdx(FvUInt16 uiIdx, FvBinaryIStream& kIS) = 0;
	virtual bool	DeserializeFromStreamForDBData(FvBinaryIStream& kIS) = 0;				//! 属性反流化后,需要反流化pos,dir,spaceid,spacetype
	virtual bool	DeserializeFromStreamForAllData(FvBinaryIStream& kIS) = 0;				//! 只需要反流化属性,不需要pos等
};


//! AllData的基类
struct FV_ENTITYDEF_API FvAllData
{
	FvSpaceID		m_iSpaceID;
	FvUInt16		m_uiSpaceType;
	FvVector3		m_kPos;
	FvDirection3	m_kDir;


	FvAllData():m_iSpaceID(FV_NULL_SPACE),m_uiSpaceType(0) {}
	virtual ~FvAllData() {}

	virtual FvUInt16 GetType() const = 0;
	virtual bool	HasCell() const = 0;
	virtual bool	SerializeToStream(FvBinaryOStream& kOS) const = 0;				//! 在属性流化之后,依次流化pos,dir,spaceid,spacetype
	virtual bool	SerializeToStreamForCellData(FvBinaryOStream& kOS) const = 0;	//! 只需要流化Cell属性,不需要pos等
	virtual	bool	DeserializeFromSection(FvXMLSectionPtr spSection) = 0;
};


//! Client端Attrib
class FV_ENTITYDEF_API FvClientAttrib
{
public:
	FvClientAttrib();
	~FvClientAttrib();

	bool	CreateOwnData(FvEntityID uiEntityID, FvEntity* pkEntity, FvClientEntityExport*	pkEntityExport);
	bool	CreateOthersData(FvEntityID uiEntityID, FvEntity* pkEntity, FvClientEntityExport* pkEntityExport);

	bool	DeserializeFromStreamForBaseData(FvEntity* pkEntity, FvBinaryIStream& kIS);
	bool	DeserializeFromStreamForOwnCliData(FvEntity* pkEntity, FvBinaryIStream& kIS);

	bool	OnProperties(FvEntity* pkEntity, FvBinaryIStream& kIS);
	bool	OnProperty(bool bOwn, FvEntity* pkEntity, int iMessageID, FvBinaryIStream& kIS);

	bool	OnMethod(FvEntity* pkEntity, int iMessageID, FvBinaryIStream& kIS);

	FvCallFuncByStrHandler* FindStrFuncHandler(const FvString& kFuncName, const FvString& kProtoType) const;
	bool	HasStrFuncName(const FvString& kFuncName) const;

	FvClientEntityExport* GetExport() const { return m_pkEntityExport; }

protected:
	void	BindData(FvEntity* pkEntity, FvUInt8 bOwn);

protected:
	FvClientEntityExport*	m_pkEntityExport;
	FvClientData*			m_pkClientData;
#ifndef FV_SHIPPING
	FvEntityID				m_uiEntityID;
#endif
};


//! Robot端Attrib
class FV_ENTITYDEF_API FvRobotAttrib
{
public:
	FvRobotAttrib();
	~FvRobotAttrib();

	bool	CreateOwnData(FvEntityID uiEntityID, FvEntity* pkEntity, FvRobotEntityExport* pkEntityExport);
	bool	CreateOthersData(FvEntityID uiEntityID, FvEntity* pkEntity, FvRobotEntityExport* pkEntityExport);

	bool	DeserializeFromStreamForBaseData(FvEntity* pkEntity, FvBinaryIStream& kIS);
	bool	DeserializeFromStreamForOwnCliData(FvEntity* pkEntity, FvBinaryIStream& kIS);

	bool	OnProperties(FvEntity* pkEntity, FvBinaryIStream& kIS);
	bool	OnProperty(bool bOwn, FvEntity* pkEntity, int iMessageID, FvBinaryIStream& kIS);

	bool	OnMethod(FvEntity* pkEntity, int iMessageID, FvBinaryIStream& kIS);

	FvRobotEntityExport* GetExport() const { return m_pkEntityExport; }

protected:
	void	BindData(FvEntity* pkEntity, FvUInt8 bOwn);

protected:
	FvRobotEntityExport*	m_pkEntityExport;
	FvClientData*			m_pkClientData;
#ifndef FV_SHIPPING
	FvEntityID				m_uiEntityID;
#endif
};


#ifdef FV_SERVER

//! 属性修改回调
class FV_ENTITYDEF_API FvAttribEventCallBack
{
public:
	virtual void HandleEvent(FvUInt8 uiMessageID, FvBinaryIStream& kIS, bool bOwnCli, bool bGhost, bool bOthersCli, bool bDB) {}
};


//! Base端Attrib
class FV_ENTITYDEF_API FvBaseAttrib : public FvDataOwner
{
public:
	FvBaseAttrib();
	~FvBaseAttrib();

	bool	Init(FvEntityID uiEntityID, FvEntity* pkEntity, FvBaseEntityExport* pkEntityExport);

	bool	CreateCellData();
	void	DestroyCellData();
	FvCellData*GetCellData() const { return m_pkCellData; }

	void	AttachOwner();

	bool	SerializeToStreamForDBData(FvBinaryOStream& kOS) const;
	bool	SerializeToStreamForCellData(FvBinaryOStream& kOS) const;
	bool	SerializeToStreamForCliData(FvBinaryOStream& kOS) const;
	bool	SerializeToStreamForBaseDBData(FvBinaryOStream& kOS) const;

	bool	DeserializeFromStreamForDBData(FvBinaryIStream& kIS);
	bool	DeserializeFromStreamForAllData(FvBinaryIStream& kIS);

	bool	OnMethodFromClient(int iMessageID, FvBinaryIStream& kIS);
	bool	OnMethodFromClientWithCallBack(int iMessageID, FvBinaryIStream& kIS, FvObjBlob& kMB);
	bool	OnMethodFromServer(FvUInt16 uiMethodIdx, FvBinaryIStream& kIS);

	FvBaseEntityExport* GetExport() const { return m_pkEntityExport; }

	void	SetAttribEventCallBack(FvAttribEventCallBack* pkCallBack) { m_pkEvtCallBack=pkCallBack; }

protected:
	virtual FvDataOwner*GetRootData(OpCode uiOpCode, FvDataOwner* pkVassal, FvUInt16 uiDataID);
	virtual void		NotifyDataChanged();

protected:
	FvEntity*				m_pkEntity;
	FvBaseEntityExport*		m_pkEntityExport;
	FvCellData*				m_pkCellData;
	FvUInt16				m_uiDataIdx;
	FvUInt16				m_uiDataID;
	FvUInt8					m_uiMessageID;
	FvAttribEventCallBack*	m_pkEvtCallBack;
#ifndef FV_SHIPPING
	FvEntityID				m_uiEntityID;
#endif
};


//! Cell端Attrib
class FV_ENTITYDEF_API FvCellAttrib : public FvDataOwner
{
public:
	FvCellAttrib();
	~FvCellAttrib();

	bool	Init(FvEntityID uiEntityID, FvEntity* pkEntity, FvCellEntityExport* pkEntityExport);

	bool	CreateRealData();
	void	DestroyRealData();

	void	AttachOwnerForCellData();
	void	AttachOwnerForRealData();
	void	AttachOwnerForGhostData();

	bool	SerializeToStreamForCellData(FvBinaryOStream& kOS) const;
	bool	SerializeToStreamForRealData(FvBinaryOStream& kOS) const;
	bool	SerializeToStreamForGhostData(FvBinaryOStream& kOS) const;
	bool	SerializeToStreamForCellDBData(FvBinaryOStream& kOS) const;
	bool	SerializeToStreamForOwnCliData(FvBinaryOStream& kOS) const;
	bool	SerializeToStreamForOthersCliData(FvBinaryOStream& kOS) const;

	bool	DeserializeFromStreamForCellData(FvBinaryIStream& kIS);
	bool	DeserializeFromStreamForRealData(FvBinaryIStream& kIS);
	bool	DeserializeFromStreamForGhostData(FvBinaryIStream& kIS);

	bool	OnProperty(int iMessageID, FvBinaryIStream& kIS);

	bool	OnMethodFromClient(int iMessageID, FvBinaryIStream& kIS);
	bool	OnMethodFromClientWithCallBack(int iMessageID, FvBinaryIStream& kIS, FvObjBlob& kMB);
	bool	OnMethodFromServer(FvUInt16 uiMethodIdx, FvBinaryIStream& kIS);

	FvCellEntityExport* GetExport() const { return m_pkEntityExport; }

	void	SetAttribEventCallBack(FvAttribEventCallBack* pkCallBack) { m_pkEvtCallBack=pkCallBack; }

protected:
	virtual FvDataOwner*GetRootData(OpCode uiOpCode, FvDataOwner* pkVassal, FvUInt16 uiDataID);
	virtual void		NotifyDataChanged();

	void	BindRealData();
	void	UnBindRealData();

protected:
	FvEntity*			m_pkEntity;
	FvCellEntityExport*	m_pkEntityExport;
	FvObjStruct*		m_pkRealData;
	FvUInt16			m_uiDataIdx;
	FvUInt16			m_uiDataID;
	FvUInt8				m_uiMessageID;
	FvAttribEventCallBack*	m_pkEvtCallBack;
#ifndef FV_SHIPPING
	FvEntityID			m_uiEntityID;
#endif
};

#endif

#endif//__FvEntityData_H__
