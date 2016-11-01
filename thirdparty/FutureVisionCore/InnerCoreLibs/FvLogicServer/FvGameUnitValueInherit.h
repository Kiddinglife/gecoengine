#ifndef __GameUnitValueInherit_H__
#define __GameUnitValueInherit_H__


#include "FvLogicServerDefine.h"

#include <FvRefList.h>
#include <FvValueInherit.h>
#include <FvMemoryNode.h>

class FvGameUnitLogic;


class FV_LOGIC_SERVER_API FvGameUnitValueInherit: public FvValueInherit<FvInt32>
{
	FV_MEM_TRACKER(FvGameUnitValueInherit);
	FV_NOT_COPY_COMPARE(FvGameUnitValueInherit);
public:
	FvGameUnitValueInherit(void);
	~FvGameUnitValueInherit(void);

	void SetTarget(FvGameUnitLogic& kTarget, const FvUInt32 uiValueIdx);

private:

	virtual void _OnUpdated(const FvInt32& oldInheritValue, const FvInt32& newInheritValue);
	virtual void _OnAttach(const bool bUpdate);
	virtual void _OnDetach(const bool bUpdate);

	FvGameUnitLogic* m_rpTarget;
	FvUInt32 m_uiValueIdx;

};

class FV_LOGIC_SERVER_API FvGameUnitValueModifier
{
	FV_MEM_TRACKER(FvGameUnitValueModifier);
	FV_NOT_COPY_COMPARE(FvGameUnitValueModifier);
public:
	FvGameUnitValueModifier(void);
	~FvGameUnitValueModifier(void);

	void Attach(FvGameUnitLogic& kTarget, const FvUInt32 uiValueIdx, const bool bUpdate);
	void SetModValue(FvGameUnitLogic& kTarget, const FvInt32 iDeltaValue);
	void Detach(FvGameUnitLogic& kTarget, const bool bUpdate);

private:

	FvInt32 m_iDeltaValue;
	FvUInt16 m_uiValueIdx;
	bool m_bAttached;
};

#endif // __GameUnitValueInherit_H__