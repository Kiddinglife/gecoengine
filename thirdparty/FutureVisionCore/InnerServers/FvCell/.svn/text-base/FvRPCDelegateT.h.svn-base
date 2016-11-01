
//注意不能加上多次包含保护，比如#pragram once 或 #ifndef  .. #define .. #endif

#define FV_DELEGATE_TEMPLATE_PARAMS    FV_MAKE_PARAMS1(FV_DELEGATE_NUM_ARGS, class T)
// class T0, class T1, class T2, ...
#define FV_DELEGATE_TEMPLATE_ARGS      FV_MAKE_PARAMS1(FV_DELEGATE_NUM_ARGS, T)
// T0, T1, T2, ...
#define FV_DELEGATE_FUNCTION_PARAMS    FV_MAKE_PARAMS2(FV_DELEGATE_NUM_ARGS, T, a)
// T0 a0, T1 a1, T2 a2, ...
#define FV_DELEGATE_FUNCTION_ARGS      FV_MAKE_PARAMS1(FV_DELEGATE_NUM_ARGS, a)
// a0, a1, a2, ...
#define FV_DELEGATE_STREAM_ARGS( s )   FV_MAKE_STREAM(FV_DELEGATE_NUM_ARGS, s, <<, a)
// s<<a0, s<<a1, s<<a2
 
// Comma if nonzero number of arguments
#if FV_DELEGATE_NUM_ARGS == 0
#define FV_DELEGATE_COMMA
#else
#define FV_DELEGATE_COMMA   ,
#endif

//-------------------------------------------------------------------------
// class FvRPCFunction<R (T1, T2, ..., TN)>
template <class R FV_DELEGATE_COMMA FV_DELEGATE_TEMPLATE_PARAMS>
class FvRPCFunction<R (FV_DELEGATE_TEMPLATE_ARGS)>
{
public:
	virtual operator FvUInt16() = 0; 
};

//-------------------------------------------------------------------------
// class FvRPCDelegate<R (T1, T2, ..., TN)>
template <class R FV_DELEGATE_COMMA FV_DELEGATE_TEMPLATE_PARAMS>
class FvRPCDelegate<R (FV_DELEGATE_TEMPLATE_ARGS)>
{
public:
	FvRPCDelegate() :
	m_uiControllerID(FvController::INVALID_ID),
	m_uiMethodID(FvController::INVALID_ID)
	{

	}

	template<class T>
	FvRPCDelegate(T* pkController, 
		FvRPCFunction<R (FV_DELEGATE_TEMPLATE_ARGS)>& kMethod) :
	m_uiControllerID(pkController->GetID()),
	m_uiMethodID(kMethod)
	{
		
	}

	R operator()(FV_DELEGATE_FUNCTION_PARAMS) const
	{
		FvMemoryOStream kOStream;
		FV_DELEGATE_STREAM_ARGS(kOStream);
	}

	FvUInt16 m_uiControllerID;
	FvUInt16 m_uiMethodID;
};

template <class R FV_DELEGATE_COMMA FV_DELEGATE_TEMPLATE_PARAMS>
FV_INLINE FvBinaryIStream& operator>>( FvBinaryIStream &kIS, FvRPCDelegate<R (FV_DELEGATE_TEMPLATE_ARGS)> &kDelegate )
{
	kIS >> kDelegate.m_uiControllerID >> kDelegate.m_uiMethodID;
	return kIS;
}

template <class R FV_DELEGATE_COMMA FV_DELEGATE_TEMPLATE_PARAMS>
FV_INLINE FvBinaryOStream& operator<<( FvBinaryOStream &kOS, const FvRPCDelegate<R (FV_DELEGATE_TEMPLATE_ARGS)> &kDelegate )
{
	kOS << kDelegate.m_uiControllerID << kDelegate.m_uiMethodID;
	return kOS;
}

#undef FV_DELEGATE_TEMPLATE_PARAMS
#undef FV_DELEGATE_TEMPLATE_ARGS
#undef FV_DELEGATE_FUNCTION_PARAMS
#undef FV_DELEGATE_FUNCTION_ARGS
#undef FV_DELEGATE_STREAM_ARGS
#undef FV_DELEGATE_COMMA
