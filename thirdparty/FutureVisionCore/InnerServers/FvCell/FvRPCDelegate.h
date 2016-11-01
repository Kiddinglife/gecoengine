#ifndef __FvRPCDelegate_H__
#define __FvRPCDelegate_H__

#include "FvController.h"
#include "FvControllerMacro.h"

template <class TSignature>
class FvRPCFunction;

template <class TSignature>
class FvRPCDelegate;

#pragma warning(push)

#pragma warning(disable: 4715)

#define FV_DELEGATE_NUM_ARGS    0 // FvRPCDelegate<R ()>
#include "FvRPCDelegateT.h"
#undef FV_DELEGATE_NUM_ARGS

#define FV_DELEGATE_NUM_ARGS    1 // FvRPCDelegate<R (T1)>
#include "FvRPCDelegateT.h"
#undef FV_DELEGATE_NUM_ARGS

#define FV_DELEGATE_NUM_ARGS    2 // FvRPCDelegate<R (T1, T2)>
#include "FvRPCDelegateT.h"
#undef FV_DELEGATE_NUM_ARGS

#define FV_DELEGATE_NUM_ARGS    3 // FvRPCDelegate<R (T1, T2, T3)>
#include "FvRPCDelegateT.h"
#undef FV_DELEGATE_NUM_ARGS

#define FV_DELEGATE_NUM_ARGS    4 // FvRPCDelegate<R (T1, T2, T3, T4)>
#include "FvRPCDelegateT.h"
#undef FV_DELEGATE_NUM_ARGS

#define FV_DELEGATE_NUM_ARGS    5 // FvRPCDelegate<R (T1, T2, T3, T4, T5)>
#include "FvRPCDelegateT.h"
#undef FV_DELEGATE_NUM_ARGS

#define FV_DELEGATE_NUM_ARGS    6 // FvRPCDelegate<R (T1, T2, T3, T4, T5, T6)>
#include "FvRPCDelegateT.h"
#undef FV_DELEGATE_NUM_ARGS

#pragma warning(pop)

#endif // __FvRPCDelegate_H__