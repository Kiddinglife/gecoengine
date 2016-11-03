//{future header message}
#ifndef __FvRobotCfg_H__
#define __FvRobotCfg_H__

#include "FvRobotDefines.h"
#include <vector>
#include <FvSingleton.h>
#include <FvBaseTypes.h>
#include <FvVector2.h>
#include <FvVector3.h>
#include <FvVector4.h>
#include <FvXMLSection.h>


#define _DECLARE_SINGLETON(_CLASS)		\
	static _CLASS &Instance()			\
	{									\
		FV_ASSERT( ms_pkInstance );		\
		return (_CLASS&)*ms_pkInstance;	\
	}									\
	static _CLASS *pInstance()			\
	{									\
		return (_CLASS*)ms_pkInstance;	\
	}


class FV_ROBOT_API FvConfigReader : public FvSingleton<FvConfigReader>
{
public:
	FvConfigReader();
	~FvConfigReader();

	typedef std::vector<int> Result;

	enum ItemType
	{
		IT_BOOL,
		IT_INT,
		IT_FLOAT,
		IT_STRING,
		IT_VECTOR2,
		IT_VECTOR3,
		IT_VECTOR4,
		IT_USER
	};

	enum DataCheck
	{
		DC_NONE = 0,
		DC_REPEAT = 1,			//! 重复性检查,适用于任何非USER类型
		DC_NON_NULL = 2,		//! 非空检查,适用于字符串
		DC_GREATER = 4,			//! 大于检查,适用于单数值,VectorX,参考值位于第1位
		DC_GREATER_EQUAL = 8,	//! 大于等于检查,适用于单数值,VectorX,参考值位于第1位
		DC_LESS = 16,			//! 小于检查,适用于单数值,VectorX,参考值位于第0位
		DC_LESS_EQUAL = 32,		//! 小于等于检查,适用于单数值,VectorX,参考值位于第0位
		DC_RECT = 64,			//! 矩形检查,适用于Vector4
		DC_ORDER = 128,			//! 有序检查,适用于VectorX(<)
		DC_PARTIAL_ORDER = 256,	//! 偏序检查,适用于VectorX(<=)
	};

	struct FV_ROBOT_API Item
	{
		int			iIndex;
		FvString	kName;
		FvString	kNameInCmd;
		ItemType	eType;
		void*		pValue;
		const char*	pcCmdArgs;
		int			iDataCheck;
		float		fVals[2];
		static FvString skCmdStr;
		FvConfigReader* pkCfg;

		Item(int iIndex_, const char* pcName_, ItemType eType_, void* pValue_, const char* pcCmdArgs_)
			:iIndex(iIndex_),kName(pcName_),kNameInCmd("-"+kName)
			,eType(eType_),pValue(pValue_)
			,pcCmdArgs(pcCmdArgs_ ? pcCmdArgs_ : "")
			,iDataCheck(DC_NONE){}

		bool	HasFlag(DataCheck eFlag) const { return iDataCheck&eFlag; }
		void	AddCheck(DataCheck eFlag) { iDataCheck |= eFlag; }
		void	AddCheckForNum(DataCheck eFlag, float fVal);
		const FvString& GetCmdStr() const;
		const FvString& GetValueStr() const;

		bool	CheckBool(bool& bOld, const bool& bNew);
		bool	CheckNum(int& iOld, const int& iNew);
		bool	CheckNum(float& fOld, const float& fNew);
		bool	CheckStr(FvString& kOld, const FvString& kNew);
		bool	CheckVector(FvVector2& kOld, const FvVector2& kNew);
		bool	CheckVector(FvVector3& kOld, const FvVector3& kNew);
		bool	CheckVector(FvVector4& kOld, const FvVector4& kNew);
	};
	typedef std::vector<Item> Items;
	const Items&	GetItems() const { return m_kItems; }

protected:
	Result&			ReadXML(const char* pcFileName);
	Result&			ReadCmd(int iArgc, char* pcArgv[]);
	Result&			ReadCmd(const char* pcCmd);

	Item*			AddItem(int iIndex, const char* pcName, bool& bValue);
	Item*			AddItem(int iIndex, const char* pcName, int& iValue);
	Item*			AddItem(int iIndex, const char* pcName, float& fValue);
	Item*			AddItem(int iIndex, const char* pcName, FvString& kValue);
	Item*			AddItem(int iIndex, const char* pcName, FvVector2& kValue);
	Item*			AddItem(int iIndex, const char* pcName, FvVector3& kValue);
	Item*			AddItem(int iIndex, const char* pcName, FvVector4& kValue);
	Item*			AddItem(int iIndex, const char* pcName, const char* pcCmdStr);

	virtual	bool	CanChange(int iIndex) { return true; }
	virtual	bool	CanRepeat(int iIndex) { return false; }
	virtual bool	HandleChange(int iIndex, FvXMLSectionPtr spSection) { return false; }
	virtual	bool	HandleChange(int iIndex, int iArgc, char* pcArgv[]) { return false; }
	virtual void	GetValueStr(int iIndex, FvString& kValue) {}

protected:
	Items			m_kItems;
	Result			m_kResult;
};


class FV_ROBOT_API FvRobotCfg : public FvConfigReader
{
public:
	FvRobotCfg();
	~FvRobotCfg();

	_DECLARE_SINGLETON(FvRobotCfg);

	enum
	{
		SingleMode,
		WindowSizeWidth,
		WindowSizeHeight,
		WindowFrameFreq,
		WindowTimerFreq,
		ShouldWriteToConsole,
		ShouldWriteToLog,
		TickFreq,
		TickFragments,
		ServerName,
		AutoReConnect,
		PacketLossRatio,
		NetworkLatency,
		UsernamePrefix,
		Password,
		PublicKey,
		Tag,
		StartID,
		Count,
		CreatePerSec,
		BirthRect,
		MoveType,
		MoveSpeed,
		CtrllerType,
		CtrllerData,
		UpdateMove,
		AddBots,
		DelBots,
		SingleID,
		PrintMemory,
	};

	bool			Init(int iArgc, char* pcArgv[]);
	Result&			ParseCmd(const char* pcCmd);

	bool			GetSingleMode() const;
	int				GetWindowSizeWidth() const;
	int				GetWindowSizeHeight() const;
	int				GetWindowFramePeriod() const;
	int				GetWindowTimerPeriod() const;
	bool			GetShouldWriteToConsole() const;
	bool			GetShouldWriteToLog() const;
	float			GetTickFreq() const;
	int				GetTickFragments() const;
	const FvString&	GetServerName() const;
	bool			GetAutoReConnect() const;
	float			GetPacketLossRatio() const;
	float			GetMinNetworkLatency() const;
	float			GetMaxNetworkLatency() const;
	const FvString& GetUsernamePrefix() const;
	const FvString& GetPassword() const;
	const FvString& GetPublicKey() const;
	const FvString& GetTag() const;
	int				GetStartID() const;
	int				GetCount() const;
	int				GetMaxCount() const;
	int				GetCreateTick() const;
	int				GetCreatePerTick() const;
	FvVector4&		GetBirthRect();
	int				GetMoveType() const;
	int				GetMoveSpeed() const;
	const FvString& GetCtrllerType() const;
	const FvString& GetCtrllerData() const;
/**	struct CtrlInfo
	{
		FvString		kName;
		FvString		kType;
		FvXMLSectionPtr	spData;

		CtrlInfo(const FvString& kName_, const FvString& kType_, FvXMLSectionPtr spData_)
			:kName(kName_),kType(kType_),spData(spData_){}
	};
	typedef std::vector<CtrlInfo> CtrlInfos;
	CtrlInfos&		GetCtrlInfos() { return m_kCtrlInfos; }**/
	const FvString& GetTag4UpdateMove() const;
	const FvString&	GetTag4AddRobots() const;
	int				GetAddRobots() const;
	const FvString&	GetTag4DelRobots() const;
	int				GetDelRobots() const;
	int				GetSingleID() const { return m_iSingleID; }

	void			SetSingleMode(bool bVal);

protected:
	virtual	bool	CanChange(int iIndex);
	virtual	bool	CanRepeat(int iIndex);
	virtual bool	HandleChange(int iIndex, FvXMLSectionPtr spSection);
	virtual	bool	HandleChange(int iIndex, int iArgc, char* pcArgv[]);
	virtual void	GetValueStr(int iIndex, FvString& kValue);
	void			InitData();
	//bool			ReadController(FvString& kName, FvString& kType, FvString& kData);
	void			CalcValue();

protected:
	//! 定义数据
#define DECLARE_VAR(TYPE_, NAME_)	TYPE_ m_k##NAME_

	DECLARE_VAR(bool, SingleMode);
	DECLARE_VAR(int, WindowSizeWidth);
	DECLARE_VAR(int, WindowSizeHeight);
	DECLARE_VAR(float, WindowFrameFreq);
	DECLARE_VAR(float, WindowTimerFreq);
	DECLARE_VAR(bool, ShouldWriteToConsole);
	DECLARE_VAR(bool, ShouldWriteToLog);
	DECLARE_VAR(float, TickFreq);
	DECLARE_VAR(int, TickFragments);
	DECLARE_VAR(FvString, ServerName);
	DECLARE_VAR(bool, AutoReConnect);
	DECLARE_VAR(float, PacketLossRatio);
	DECLARE_VAR(FvVector2, NetworkLatency);
	DECLARE_VAR(FvString, UsernamePrefix);
	DECLARE_VAR(FvString, Password);
	DECLARE_VAR(FvString, PublicKey);
	DECLARE_VAR(FvString, Tag);
	DECLARE_VAR(int, StartID);
	DECLARE_VAR(FvVector2, Count);
	DECLARE_VAR(int, CreatePerSec);
	DECLARE_VAR(FvVector4, BirthRect);
	DECLARE_VAR(int, MoveType);	//! 0:AI,1:Key,2:Auto
	DECLARE_VAR(float, MoveSpeed);
	DECLARE_VAR(FvString, CtrllerType);
	DECLARE_VAR(FvString, CtrllerData);

	//! 
	bool		m_bInInit;
	bool		m_bCmdLineHasCtrl;
	//CtrlInfos	m_kCtrlInfos;
	int			m_iCreateTick;
	int			m_iCreatePerTick;
	FvString	m_kTag4UpdateMove;
	FvString	m_kTag4AddRobots, m_kTag4DelRobots;
	int			m_iAddRobots, m_iDelRobots;
	int			m_iSingleID;
};


#endif//__FvRobotCfg_H__
