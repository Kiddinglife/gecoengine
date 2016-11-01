//{future header message}
#ifndef __FvUnitHitEffect_H__
#define  __FvUnitHitEffect_H__

#include "FvLogicServerDefine.h"
#include "FvHitEffect.h"

class FvGameUnitLogic;

class FV_LOGIC_SERVER_API FvUnitHitEffect: public FvHitEffect
{
public:

	void SetTarget(FvGameUnitLogic& qTarget);
	FvGameUnitLogic& Target()const;

protected:

	FvUnitHitEffect();
	virtual ~FvUnitHitEffect(void);

	virtual void _Handle();
	virtual void _OnHandle() = 0;
	virtual void _OnEnd();

private:
	FvRefNode2<FvUnitHitEffect*> m_kTargetAttachNode;
	FvGameUnitLogic* m_rpTarget;//! ²»»áÎª¿Õ
};

#endif //__FvUnitHitEffect_H__
