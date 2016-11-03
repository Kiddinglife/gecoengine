//{future header message}
#ifndef __FvExpress_H__
#define __FvExpress_H__


#include "FvLogicClientDefine.h"

#include <FvMemoryNode.h>
#include <FvReferencePtr.h>


//+-----------------------------------------------------------------------------
///<>
class FV_LOGIC_CLIENT_API FvExpress
{
	FV_MEM_TRACKER(FvExpress);
	FV_NOT_COPY_COMPARE(FvExpress);
public:

	virtual void End();

	void DetachUpdate();

protected:

	FvExpress(void);
	virtual ~FvExpress(void);

	virtual void Update(const float deltaTime) = 0;
	virtual void Update1(const float deltaTime){}
	virtual void Update2(const float deltaTime){}

private:

	FvRefNode2<FvExpress*> m_kUpdateNode;
	FvRefNode2<FvExpress*> m_kOwnNode;

	//+------------------------------------------
	friend class FvExpressManager;
	friend class FvDeletable;
	friend class FvExpressOwnList;

};

class FV_LOGIC_CLIENT_API FvExpressOwnList
{
	FV_MEM_TRACKER(FvExpressOwnList);
	FV_NOT_COPY_COMPARE(FvExpressOwnList);
public:
	typedef FvRefList<FvExpress*, FvRefNode2> ExpressList;

	FvExpressOwnList(){}
	~FvExpressOwnList();

	void End();
	void Add(FvExpress& kExpress);
	ExpressList& List(){return m_kExpressList;}

private:
	ExpressList m_kExpressList;
};

//+-------------------------------------------------------------------------------------------------
#endif //__FvExpress_H__