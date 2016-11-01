//{future header message}
#ifndef __FvAreaHitEffect_H__
#define  __FvAreaHitEffect_H__

#include "FvLogicServerDefine.h"
#include "FvHitEffect.h"


#include <FvVector3.h>
#include <FvRefList.h>

class FV_LOGIC_SERVER_API FvAreaHitEffect: public FvHitEffect
{
public:
	void SetPosition(const FvVector3& kPos);
	const FvVector3& _Position()const;

	static FvRefList<FvAreaHitEffect*>& WorldList(){return S_WorldList;}
	static void ClearWorldList();

protected:

	FvAreaHitEffect();
	virtual ~FvAreaHitEffect(void);

private:
	virtual void _Handle() = 0;
	virtual void _OnRoundTarget(FvGameUnitAppearance& kObj){};
	virtual void _OnEnd();


	FvRefNode2<FvAreaHitEffect*> m_kAttachToWorld;

	FvVector3 m_kPos;

	static FvRefList<FvAreaHitEffect*> S_WorldList;
};


#endif //__FvAreaHitEffect_H__
