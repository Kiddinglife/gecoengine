//{future header message}
#ifndef __FvTarget_H__
#define __FvTarget_H__


#include "FvLogicServerDefine.h"

#include <FvVector3.h>
#include <FvTuple.h>
#include <FvReferencePtr.h>
#include <FvDynamicReferencePtr.h>

class FvGameUnitAppearance;

template <FvUInt32 CNT>
class FvTargetList
{
public:
	FvTargetList():m_uiSize(0)
	{
	}
	void Clear()
	{
		m_uiSize = 0;
	}
	void Add(FvGameUnitAppearance& kObj)
	{
		if(m_uiSize < CNT)
		{
			m_rpObjs[m_uiSize] = &kObj;
			++m_uiSize;
		}
	}
	FvUInt32 GetCnt()const{return m_uiSize;}
	FvGameUnitAppearance* GetTarget(const FvUInt32 uiIdx)const
	{
		if(uiIdx < m_uiSize)
		{
			return m_rpObjs[uiIdx];
		}
		else
		{
			return NULL;
		}
	}
	FvGameUnitAppearance& _GetTarget(const FvUInt32 uiIdx)const
	{
		FV_ASSERT_ERROR(uiIdx < m_uiSize); 
		FV_ASSERT_ERROR(m_rpObjs[uiIdx]);
		return *m_rpObjs[uiIdx];
	}

protected:

	FvGameUnitAppearance* m_rpObjs[CNT];
	FvUInt32 m_uiSize;
};


template <FvUInt32 CNT>
class FvTargetRefList
{
public:
	FvTargetRefList():m_uiSize(0)
	{
	}
	void Clear()
	{
		m_uiSize = 0;
	}
	void Add(FvGameUnitAppearance& kObj)
	{
		if(m_uiSize < CNT)
		{
			m_rpObjs[m_uiSize] = &kObj;
			++m_uiSize;
		}
	}
	FvUInt32 GetCnt()const{return m_uiSize;}
	FvGameUnitAppearance* GetTarget(const FvUInt32 uiIdx)const
	{
		if(uiIdx < m_uiSize)
		{
			return m_rpObjs[uiIdx];
		}
		else
		{
			return NULL;
		}
	}
	FvGameUnitAppearance* _GetTarget(const FvUInt32 uiIdx)const
	{
		FV_ASSERT_ERROR(uiIdx < m_uiSize); 
		return m_rpObjs[uiIdx];
	}

protected:

	FvReferencePtr<FvGameUnitAppearance> m_rpObjs[CNT];
	FvUInt32 m_uiSize;
};

class FV_LOGIC_SERVER_API FvTarget
{
public:
	FvTarget();
	~FvTarget();

	enum _Flag
	{
		FOCUS_NONE,
		FOCUS_OBJ,
		FOCUS_POS,
	};

	FvGameUnitAppearance* Obj()const;
	void Set(FvGameUnitAppearance& kObj);
	const FvVector3* Pos()const;
	void Set(const FvVector3& pos);
	void Clear();

	std::vector<FvDynamicReferencePtr>& FixList(){return m_kFixList;}

private:

	FvUInt32 m_uiMask;
	FvReferencePtr<FvGameUnitAppearance> m_rpObj;
	FvVector3 m_kPos;

	std::vector<FvDynamicReferencePtr> m_kFixList;

};


#endif //__FvTarget_H__