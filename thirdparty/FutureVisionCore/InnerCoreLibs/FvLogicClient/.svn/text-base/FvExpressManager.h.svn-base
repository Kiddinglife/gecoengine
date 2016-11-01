//{future header message}
#ifndef __FvExpressManager_H__
#define __FvExpressManager_H__

#include "FvLogicClientDefine.h"
//+-----------------------------------------------------------------------------
#include "FvExpress.h"
#include <FvRefList.h>

class FV_LOGIC_CLIENT_API FvExpressManager
{
	FV_MEM_TRACKER(FvExpressManager);
	FV_NOT_COPY_COMPARE(FvExpressManager);
public:
	typedef FvRefList<FvExpress*, FvRefNode2> ExpressList;

	FvExpressManager(void);
	~FvExpressManager(void);

	void ClearAllExpress();

	//+-----------------------------------------------------------------------------
	template <class T> T * MakeExpress();

	void Update(const float deltaTime);

	void AttachIdle(FvExpress& kExpress);
	void AttachUpdate(FvExpress& kExpress);
	void AttachUpdate1(FvExpress& kExpress);
	void AttachUpdate2(FvExpress& kExpress);

private:

	//+-----------------------------------------------------------------------------
	void _Clear();
	//+-----------------------------------------------------------------------------
	ExpressList m_ExpressList;//! ViRefList最大的特点就是当ViRefNode对象被析构的时候, 所有ViRefList都会自动踢除指向ViRefNode的指针
	ExpressList m_ExpressList1;
	ExpressList m_ExpressList2;
	ExpressList m_IdleList;
};


template <class T> 
T * FvExpressManager::MakeExpress()
{
	T* pNew = new T();
	if(pNew)
	{
		AttachIdle(*pNew);
	}
	return pNew;
}



#endif