//{future header message}
#ifndef __FvRobotUtility_H__
#define __FvRobotUtility_H__

#include <list>
#include <FvBaseTypes.h>


class IDAllocate
{
public:
	IDAllocate();
	~IDAllocate();

	bool	Init(int iStartID, int iCount);
	int		Size() const;
	int		GetID();		//! 没有ID可分配返回-1
	bool	PutID(int iID);	//! iID范围不对返回false

protected:
	int		m_iStartID;
	int		m_iEndID;
	int		m_iIndex;
	typedef std::list<int> List;
	List	m_kList;
};


class CreateMgr
{
public:
	CreateMgr();
	~CreateMgr();

	bool	Empty() const;
	int		Size() const;
	bool	Pop(FvString& kTag);
	bool	Push(int iCount, const FvString& kTag);
	int		Del(int iCount);
	bool	Del(const FvString& kTag);

protected:
	typedef std::pair<int, FvString> CreateInfo;
	typedef std::list<CreateInfo> CreateInfos;
	CreateInfos		m_kCreateInfos;
	int				m_iCreateCnt;
};


class DeadMgr
{
public:
	DeadMgr(IDAllocate& kIDAllocate);
	~DeadMgr();

	bool	Empty() const;
	int		Size() const;
	bool	Pop(int& iID, FvString& kTag);
	bool	Push(int iID, const FvString& kTag);
	int		Del(int iCount);
	bool	Del(const FvString& kTag);

protected:
	typedef std::pair<int, FvString> DeleteInfo;
	typedef std::list<DeleteInfo> DeleteInfos;
	DeleteInfos		m_kDeleteInfos;
	IDAllocate&		m_kIDAllocate;
};


extern void PrintMemoryStatus(const char* pcTitle);


#endif//__FvRobotUtility_H__
