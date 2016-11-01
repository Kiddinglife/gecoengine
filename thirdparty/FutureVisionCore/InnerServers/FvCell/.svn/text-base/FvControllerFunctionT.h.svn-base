
//注意不能加上多次包含保护，比如#pragram once 或 #ifndef  .. #define .. #endif

#define FV_DELEGATE_TEMPLATE_PARAMS    FV_MAKE_PARAMS1(FV_DELEGATE_NUM_ARGS, class T)
// class T0, class T1, class T2, ...
#define FV_DELEGATE_TEMPLATE_ARGS      FV_MAKE_PARAMS1(FV_DELEGATE_NUM_ARGS, T)
// T0, T1, T2, ...
#define FV_DELEGATE_FUNCTION_PARAMS    FV_MAKE_PARAMS2(FV_DELEGATE_NUM_ARGS, T, a)
// T0 a0, T1 a1, T2 a2, ...
#define FV_DELEGATE_FUNCTION_ARGS      FV_MAKE_PARAMS1(FV_DELEGATE_NUM_ARGS, a)
// a0, a1, a2, ...
#define FV_DELEGATE_STREAM_ARGS( s )   FV_MAKE_STREAM(FV_DELEGATE_NUM_ARGS, s, >>, a)
// s<<a0, s<<a1, s<<a2
 
// Comma if nonzero number of arguments
#if FV_DELEGATE_NUM_ARGS == 0
#define FV_DELEGATE_COMMA
#else
#define FV_DELEGATE_COMMA   ,
#endif

//-------------------------------------------------------------------------
// class FvControllerFunction<T, R (T1, T2, ..., TN)>
template <class T, class R FV_DELEGATE_COMMA FV_DELEGATE_TEMPLATE_PARAMS>
class FvControllerFunction<T, R (FV_DELEGATE_TEMPLATE_ARGS)> : 
	public FvRPCFunction<R (FV_DELEGATE_TEMPLATE_ARGS)>,
	public FvControllerFunctionBase<T>
{
public:
	typedef R(T::*Function)(FV_DELEGATE_TEMPLATE_ARGS);
	FvControllerFunction(Function pfFunction) :
	m_pfFunction(pfFunction){}

	operator FvUInt16() { return GetMethodID(); }
	void Excute(T* pkController,FvBinaryIStream& kIStream)
	{
		FV_DELEGATE_FUNCTION_PARAMS;
		FV_DELEGATE_STREAM_ARGS(kIStream)
		((*pkController).*m_pfFunction)(FV_DELEGATE_FUNCTION_ARGS);
	}
	Function m_pfFunction;
};

#undef FV_DELEGATE_TEMPLATE_PARAMS
#undef FV_DELEGATE_TEMPLATE_ARGS
#undef FV_DELEGATE_FUNCTION_PARAMS
#undef FV_DELEGATE_FUNCTION_ARGS
#undef FV_DELEGATE_STREAM_ARGS
#undef FV_DELEGATE_COMMA
