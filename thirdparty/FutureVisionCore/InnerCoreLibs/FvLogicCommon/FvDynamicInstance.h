//{future header message}
#ifndef __FvDynamicInstance_H__
#define  __FvDynamicInstance_H__

#include "FvLogicCommonDefine.h"
#include <FvRefList.h>

class FV_LOGIC_COMMON_API FvBaseDynamicInstance: private FvRefNode1<FvBaseDynamicInstance*>
{
public:

	static FvUInt32 InstanceSize(){ return ms_InstanceList.Size(); }

protected:

	FvBaseDynamicInstance();
	~FvBaseDynamicInstance();

private:

	static FvRefListNameSpace<FvBaseDynamicInstance>::List1 ms_InstanceList;
};


template <class T>
class FvDynamicInstance
{
public:

	static T* pInstance(){return ms_kInstance;}
	static T& Instance(){FV_ASSERT(ms_kInstance); return *ms_kInstance;}
	static void Start();
	static void End();

protected:

	FvDynamicInstance(void){}
	~FvDynamicInstance(void){}

	static T* ms_kInstance;

private:
};


template <class T>
T* FvDynamicInstance<T>::ms_kInstance = NULL;


template <class T>
void FvDynamicInstance<T>::Start()
{
	FV_ASSERT(ms_kInstance == NULL);
	if(ms_kInstance == NULL)
	{
		ms_kInstance = new T();
	}
}

template <class T>
void FvDynamicInstance<T>::End()
{
	if(ms_kInstance)
	{
		delete ms_kInstance;
		ms_kInstance = NULL;
	}
}


#endif //__FvDynamicInstance_H__