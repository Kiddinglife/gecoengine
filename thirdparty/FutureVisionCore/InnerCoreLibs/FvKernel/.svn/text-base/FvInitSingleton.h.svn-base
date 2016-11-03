//{future header message}
#ifndef __FvInitSingleton_H__
#define __FvInitSingleton_H__

#include "FvSingleton.h"

template <class T>
class FvInitSingleton : public FvSingleton<T>
{
public:

	static bool Init()
	{
		FV_ASSERT_DEV( !ms_bFinalised );

		bool bResult = true;

		if ( ms_iInitedCount == 0 )
		{
			FvInitSingleton<T> *pkBaseInstance = new T();

			bResult = pkBaseInstance->DoInit();
		}

		++ms_iInitedCount;
		return bResult;
	}

	static bool Fini()
	{
		FV_IF_NOT_ASSERT_DEV( !ms_bFinalised )
		{
			return true;
		}

		FV_IF_NOT_ASSERT_DEV( ms_iInitedCount > 0 )
		{
			return true;
		}

		--ms_iInitedCount;

		bool bResult = true;

		if ( ms_iInitedCount == 0 )
		{
			FvInitSingleton<T> *pkBaseInstance = FvSingleton<T>::pInstance();

			bResult = pkBaseInstance->DoFini();

			delete FvSingleton<T>::pInstance();

			ms_bFinalised = true;
		}

		return bResult;
	}

	static bool Finalised() { return ms_bFinalised; }

protected:
	static bool ms_bFinalised;
	static int ms_iInitedCount;

	virtual bool DoInit() { return true; }
	virtual bool DoFini() { return true; }
};

#define FV_INIT_SINGLETON_STORAGE( TYPE )						\
	FV_SINGLETON_STORAGE( TYPE )								\
	template <>													\
	bool FvInitSingleton< TYPE >::ms_bFinalised = false;		\
	template <>													\
	int FvInitSingleton< TYPE >::ms_iInitedCount = 0;

#endif // __FvInitSingleton_H__