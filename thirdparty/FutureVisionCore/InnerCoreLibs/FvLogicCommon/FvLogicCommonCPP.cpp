

#include "FvTupleCallback.h"
#include "FvFuncSet.h"
#include "FvPriorityAction.h"
#include "FvLogicPriority.h"
#include "FvDoubleLink.h"
#include "FvNotifier.h"


template<> template<> FV_LOGIC_COMMON_API void FvTupleAsynCaller<TupleCallback_Demo::From>::ParamCheck<0, FvUInt32>(){}
template<> template<> FV_LOGIC_COMMON_API void FvTupleAsynCaller<TupleCallback_Demo::From>::ParamCheck<0, FvUInt32, float>(){}
template<> template<> FV_LOGIC_COMMON_API void FvTupleAsynCaller<TupleCallback_Demo::From>::ParamCheck<0, FvUInt32, float, TupleCallback_Demo::Param*>(){}
