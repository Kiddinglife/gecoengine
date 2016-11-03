//{future header message}
#ifndef __FvRangeAttackNode_H__
#define  __FvRangeAttackNode_H__

#include "FvLogicServerDefine.h"

#include <FvMemoryNode.h>
#include <FvGameUnitInfo.h>
#include <FvTimeNodeEx.h>
#include <FvReferencePtr.h>
#include <FvRefList.h>
#include <FvGameTimer.h>
#include <FvDestroyManager.h>


template<class TAttacker, class TVictim, class TDamage>
class FvRangeAttackNode: private FvRefNode1<FvRangeAttackNode<TAttacker, TVictim, TDamage>*>
{
	FV_MEM_TRACKER(FvRangeAttackNode);
	FV_NOT_COPY_COMPARE(FvRangeAttackNode);
public:
	
	FvRangeAttackNode(TVictim& kVictim);

	void SetAttacker(TAttacker* pkAttacker){m_rpAttacker = pkAttacker;}
	void SetDmgInfo(const TDamage& kDmg){m_kDmg.Set(kDmg);}
	void SetTime(const FvUInt32 uiDeltaTime){FvGameTimer::SetTime(uiDeltaTime, m_kHitTimeNode);}
	void DoDamage();

private:

	~FvRangeAttackNode(void){}

	_FRIEND_CLASS_TimeNodeEvent_;
	void _OnTimeHit(FvTimeEventNodeInterface&);
	FvTimeEventNode2<FvRangeAttackNode, &_OnTimeHit> m_kHitTimeNode;

	TVictim& m_kVictim;
	FvReferencePtr<TAttacker> m_rpAttacker;

	TDamage m_kDmg;

	friend class FvDeletable;
	template<class, class, class>friend class FvRangeAttackList;
};

template<class TAttacker, class TVictim, class TDamage>
class FvRangeAttackList
{
public:
	typedef FvRangeAttackNode<TAttacker, TVictim, TDamage> Node;
	typedef FvRefList<Node*, FvRefNode1> List;

	FvRangeAttackList(){}
	~FvRangeAttackList(){	FV_ASSERT_WARNING(m_kList.Size() == 0);}

	void Clear();
	void Add(TVictim& kVictim, TAttacker* pkAttacker, const TDamage& kDmg, FvUInt32 uiDelayTime);

private:
	List m_kList;
};

#include "FvRangeAttackNode.inl"


#endif //__FvRangeAttackNode_H__
