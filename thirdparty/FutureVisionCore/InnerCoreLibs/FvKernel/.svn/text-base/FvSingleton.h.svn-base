//{future header message}
#ifndef __FvSingleton_H__
#define __FvSingleton_H__

#include "FvKernel.h"
#include "FvDebug.h"

template <class T>
class FvSingleton
{
protected:
	static T *ms_pkInstance;

public:
	FvSingleton()
	{
		FV_ASSERT( NULL == ms_pkInstance );
		ms_pkInstance = static_cast< T * >( this );
	}

	~FvSingleton()
	{
		FV_ASSERT( this == ms_pkInstance );
		ms_pkInstance = NULL;
	}

	static T &Instance()
	{
		FV_ASSERT( ms_pkInstance );
		return *ms_pkInstance;
	}

	static T *pInstance()
	{
		return ms_pkInstance;
	}
};

#define FV_SINGLETON_STORAGE( TYPE )						\
template <>													\
TYPE * FvSingleton< TYPE >::ms_pkInstance = NULL;			\


#endif // __FvSingleton_H__
