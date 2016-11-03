#ifndef __FvControllerFunction_H__
#define __FvControllerFunction_H__

#include "FvRPCDelegate.h"
#include "FvControllerMacro.h"

template<class T>
class FvControllerFunctionBase
{
public:
	FvControllerFunctionBase()
	{
		m_uiMethodID = T::ms_kFunctions.size();
		T::ms_kFunctions.push_back(this);
	}
	virtual ~FvControllerFunctionBase(){}
	virtual void Excute(T* pkController,FvBinaryIStream& kIStream) = 0;
	FV_INLINE const FvUInt16& GetMethodID() const { return m_uiMethodID; }

protected:
	FvUInt16 m_uiMethodID;
};

template <class T, class TSignature>
class FvControllerFunction;

#pragma warning(push)

#pragma warning(disable: 4715)

#define FV_DELEGATE_NUM_ARGS    0 // FvControllerFunction<R ()>
#include "FvControllerFunctionT.h"
#undef FV_DELEGATE_NUM_ARGS

#define FV_DELEGATE_NUM_ARGS    1 // FvControllerFunction<R (T1)>
#include "FvControllerFunctionT.h"
#undef FV_DELEGATE_NUM_ARGS

#define FV_DELEGATE_NUM_ARGS    2 // FvControllerFunction<R (T1, T2)>
#include "FvControllerFunctionT.h"
#undef FV_DELEGATE_NUM_ARGS

#define FV_DELEGATE_NUM_ARGS    3 // FvControllerFunction<R (T1, T2, T3)>
#include "FvControllerFunctionT.h"
#undef FV_DELEGATE_NUM_ARGS

#define FV_DELEGATE_NUM_ARGS    4 // FvControllerFunction<R (T1, T2, T3, T4)>
#include "FvControllerFunctionT.h"
#undef FV_DELEGATE_NUM_ARGS

#define FV_DELEGATE_NUM_ARGS    5 // FvControllerFunction<R (T1, T2, T3, T4, T5)>
#include "FvControllerFunctionT.h"
#undef FV_DELEGATE_NUM_ARGS

#define FV_DELEGATE_NUM_ARGS    6 // FvControllerFunction<R (T1, T2, T3, T4, T5, T6)>
#include "FvControllerFunctionT.h"
#undef FV_DELEGATE_NUM_ARGS

#pragma warning(pop)

#endif // __FvControllerFunction_H__