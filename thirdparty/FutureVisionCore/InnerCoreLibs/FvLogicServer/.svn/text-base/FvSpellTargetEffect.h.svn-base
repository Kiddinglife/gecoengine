
//{future header message}
#ifndef __FvSpellTargetEffect_H__
#define  __FvSpellTargetEffect_H__


#include "FvLogicServerDefine.h"
#include "FvTarget.h"

#include <FvSpellStruct.h>
#include <FvTimeNodeEx.h>

class FvGameUnitAppearance;
class FvGameUnitLogic;


class FV_LOGIC_SERVER_API FvSpellTargetEffect
{
	FV_MEM_TRACKER(FvSpellTargetEffect);
	FV_NOT_COPY_COMPARE(FvSpellTargetEffect);
public:

	virtual void OnHandle(FvGameUnitLogic& kCaster, FvTarget& kTarget, const FvSpellInfo& kInfo) = 0;

protected:

	FvSpellTargetEffect(void);
	virtual ~FvSpellTargetEffect(void);

	void End();
	void End(const float fDelayTime);

private:

	void _OnEndTime(FvTimeEventNodeInterface&);
	FvTimeEventNode2<FvSpellTargetEffect, &_OnEndTime> m_kEndTimeNode;

	virtual void _OnEnd(){}
	friend class FvDeletable;
};


#endif //__FvSpellTargetEffect_H__