//{future header message}
#ifndef __FvAreaAura_H__
#define  __FvAreaAura_H__

#include "FvLogicServerDefine.h"
#include "FvAura.h"

#include <FvRefList.h>
#include <FvVector3.h>


class FV_LOGIC_SERVER_API FvAreaAura: public FvAura
{
public:

	virtual void SetPosition(const FvVector3& kPos) = 0;
	virtual const FvVector3& _Position()const = 0;

	void AttachToWorld(FvRefList<FvAreaAura*>& kList);
	void AttachToSpellFlow(FvRefList<FvAreaAura*>& kList);

protected:

	FvAreaAura();
	virtual ~FvAreaAura(void);

	virtual void _OnActive(const bool bUpdate){}
	virtual void _OnDeactive(const bool bUpdate){}

	virtual void _OnTick(){}

private:

	FvRefNode2<FvAreaAura*> m_kWorldAttachNode;
	FvRefNode2<FvAreaAura*> m_kSpellFlowAttachNode;

};


#endif // __FvAreaAura_H__
