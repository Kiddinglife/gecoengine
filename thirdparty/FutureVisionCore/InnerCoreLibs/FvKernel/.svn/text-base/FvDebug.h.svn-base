//{future header message}
#ifndef __FvDebug_H__
#define __FvDebug_H__

#include "FvKernel.h"

#include <stdio.h>
#include <assert.h>
#include <FvPrintf.h>
#include <FvPowerDefines.h>

enum FvDebugMessagePriority
{
	FV_DEBUG_MESSAGE_PRIORITY_TRACE,
	FV_DEBUG_MESSAGE_PRIORITY_DEBUG,
	FV_DEBUG_MESSAGE_PRIORITY_INFO,
	FV_DEBUG_MESSAGE_PRIORITY_NOTICE,
	FV_DEBUG_MESSAGE_PRIORITY_WARNING,
	FV_DEBUG_MESSAGE_PRIORITY_ERROR,
	FV_DEBUG_MESSAGE_PRIORITY_CRITICAL,
	FV_DEBUG_MESSAGE_PRIORITY_HACK,
	FV_DEBUG_MESSAGE_PRIORITY_SCRIPT,
	FV_DEBUG_MESSAGE_PRIORITY_ASSET,
	FV_DEBUG_MESSAGE_PRIORITY_NUM
};

FV_INLINE
const char *MessagePrefix( FvDebugMessagePriority ePriority )
{
	static const char *pcPrefixes[] =
	{
		"TRACE",
		"DEBUG",
		"INFO",
		"NOTICE",
		"WARNING",
		"ERROR",
		"CRITICAL",
		"HACK",
		"SCRIPT",
		"ASSET"
	};

	return (ePriority >= 0 && (size_t)ePriority < FV_ARRAY_SIZE(pcPrefixes)) ? pcPrefixes[(int)ePriority] : "";
}

class FvSimpleMutex;
class FV_KERNEL_API FvDebugMsgHelper
{
public:
	FvDebugMsgHelper( int iComponentPriority, int iMessagePriority ) :
	  m_iComponentPriority( iComponentPriority ),
		  m_iMessagePriority( iMessagePriority )
	  {
	  }
	  FvDebugMsgHelper() :
	  m_iComponentPriority( 0 ),
		  m_iMessagePriority( FV_DEBUG_MESSAGE_PRIORITY_CRITICAL )
	  {
	  }
	  static void Fini();

#ifndef FV_WIN32
	  void Message( const char *pcFormat, ... )
		  __attribute__ ( (pcFormat (printf, 2, 3 ) ) );
	  void CriticalMessage( const char *pcFormat, ... )
		  __attribute__ ( (pcFormat (printf, 2, 3 ) ) );
	  void DevCriticalMessage( const char *pcFormat, ... )
		  __attribute__ ( (pcFormat (printf, 2, 3 ) ) );
#else // !FV_WIN32
	  void Message( const char *pcFormat, ... );
	  void CriticalMessage( const char *pcFormat, ... );
	  void DevCriticalMessage( const char *pcFormat, ... );
#endif // FV_WIN32

	  void MessageBackTrace();
	  static void ShouldWriteToSyslog( bool bState = true );

	  static void ShowErrorDialogs( bool bShow );
	  static bool ShowErrorDialogs();

private:
	void CriticalMessageHelper( bool bIsDevAssertion, const char *pcFormat,
		va_list kArgPtr );

	int m_iComponentPriority;
	int m_iMessagePriority;

	static bool ms_bShowErrorDialogs;
	static FvSimpleMutex *ms_pkMutex;
};

//! modify by Uman, 20100524
#ifndef FV_SHIPPING
//#ifndef FV_RELEASE
#define FV_DEBUG_MSG_WITH_PRIORITY( PRIORITY )								\
	FvDebugMsgHelper( ::ms_iComponentPriority, PRIORITY ).Message
	
	namespace
	{
		const int ms_iComponentPriority = 0;
	}
#else // !FV_RELEASE
#define FV_DEBUG_MSG_WITH_PRIORITY( PRIORITY )	DebugMsgNULL
#endif // FV_RELEASE

#ifndef FV_ENABLE_MSG_LOGGING
#define FV_ENABLE_MSG_LOGGING 1
#endif // FV_ENABLE_MSG_LOGGING
#if FV_ENABLE_MSG_LOGGING

#define FV_TRACE_MSG		FV_DEBUG_MSG_WITH_PRIORITY( FV_DEBUG_MESSAGE_PRIORITY_TRACE )

#define FV_DEBUG_MSG		FV_DEBUG_MSG_WITH_PRIORITY( FV_DEBUG_MESSAGE_PRIORITY_DEBUG )

#define FV_INFO_MSG			FV_DEBUG_MSG_WITH_PRIORITY( FV_DEBUG_MESSAGE_PRIORITY_INFO )

#define FV_NOTICE_MSG		FV_DEBUG_MSG_WITH_PRIORITY( FV_DEBUG_MESSAGE_PRIORITY_NOTICE )

#define FV_WARNING_MSG																\
	FV_DEBUG_MSG_WITH_PRIORITY( FV_DEBUG_MESSAGE_PRIORITY_WARNING )

#define FV_ERROR_MSG																\
	FV_DEBUG_MSG_WITH_PRIORITY( FV_DEBUG_MESSAGE_PRIORITY_ERROR )

#define FV_HACK_MSG			FV_DEBUG_MSG_WITH_PRIORITY( FV_DEBUG_MESSAGE_PRIORITY_HACK )

#define FV_SCRIPT_MSG		FV_DEBUG_MSG_WITH_PRIORITY( FV_DEBUG_MESSAGE_PRIORITY_SCRIPT )

#define FV_DEV_CRITICAL_MSG	FV_CRITICAL_MSG

#else	// FV_ENABLE_MSG_LOGGING
	#define FV_NULL_MSG(...)														\
	do																				\
	{																				\
		if (false)																	\
			printf(__VA_ARGS__);													\
	}																				\
	while (false)

	#define FV_TRACE_MSG(...)			FV_NULL_MSG(__VA_ARGS__)
	#define FV_DEBUG_MSG(...)			FV_NULL_MSG(__VA_ARGS__)
	#define FV_INFO_MSG(...)			FV_NULL_MSG(__VA_ARGS__)
	#define FV_NOTICE_MSG(...)			FV_NULL_MSG(__VA_ARGS__)
	#define FV_WARNING_MSG(...)			FV_NULL_MSG(__VA_ARGS__)
	#define FV_ERROR_MSG(...)			FV_NULL_MSG(__VA_ARGS__)
	#define FV_HACK_MSG(...)			FV_NULL_MSG(__VA_ARGS__)
	#define FV_SCRIPT_MSG(...)			FV_NULL_MSG(__VA_ARGS__)
	#define FV_DEV_CRITICAL_MSG(...)	FV_NULL_MSG(__VA_ARGS__)

#endif	// !FV_ENABLE_MSG_LOGGING

//! modify by Uman, 20100524
#ifndef FV_SHIPPING
//#ifndef FV_RELEASE
#define FV_CRITICAL_MSG																\
	FvDebugMsgHelper( ::ms_iComponentPriority,										\
	FV_DEBUG_MESSAGE_PRIORITY_CRITICAL ).CriticalMessage
#else // !FV_RELEASE
#define FV_CRITICAL_MSG	FvDebugMsgHelper().CriticalMessage
#endif // FV_RELEASE

#define FV_ENTER( className, methodName )											\
	FV_TRACE_MSG( className "::" methodName "\n" )

inline void DebugMsgNULL( const char * /*pcFormat*/, ... ){}

FV_KERNEL_API const char * DebugMunge( const char *pcPath, const char *pcModule = NULL );

union FvIntFloat
{
	float m_fValue;
	FvUInt32 m_uiValue;
};

FV_INLINE 
bool IsFloatValid( float fValue )
{
	FvIntFloat kIntFloat;
	kIntFloat.m_fValue = fValue;
	return ( kIntFloat.m_uiValue & 0x7f800000 ) != 0x7f800000;
}

//! modify by Uman, 20100524
#ifndef FV_SHIPPING
//#ifndef FV_RELEASE
	#if FV_ENABLE_WATCHERS
		#define FV_DECLARE_DEBUG_COMPONENT2( module, priority )						\
			static int ms_iComponentPriority = priority;							\
			static FvWatcherPtr ms_kNeverUseThisDataWatcherPointer =				\
				new FvDataWatcher<int>(	::ms_iComponentPriority,					\
										FvWatcher::WT_READ_WRITE,					\
										DebugMunge( __FILE__, module ) );
	#else // FV_ENABLE_WATCHERS
		#define FV_DECLARE_DEBUG_COMPONENT2( module, priority )						\
			static int ms_iComponentPriority = priority;
	#endif // !FV_ENABLE_WATCHERS
#else // !FV_RELEASE
	#define FV_DECLARE_DEBUG_COMPONENT2( module, priority )
#endif // FV_RELEASE

#define FV_DECLARE_DEBUG_COMPONENT( priority )										\
	FV_DECLARE_DEBUG_COMPONENT2( NULL, priority )

#include <assert.h>

#ifdef __ASSERT_FUNCTION
#	define FV_FUNCNAME __ASSERT_FUNCTION
#else
#	define FV_FUNCNAME ""
#endif

#ifdef FV_USE_ASSERTS
#	define FV_REAL_ASSERT assert(0);
#else
#	define FV_REAL_ASSERT
#endif

//#if !defined( FV_RELEASE )
#	define FV_ASSERT( exp )															\
		if (!(exp))																	\
		{																			\
			FvDebugMsgHelper().CriticalMessage(										\
				"ASSERTION FAILED: " #exp "\n"										\
					__FILE__ "(%d)%s%s\n", (int)__LINE__,							\
					*FV_FUNCNAME ? " in " : "",										\
					FV_FUNCNAME );													\
																					\
			FV_REAL_ASSERT															\
		}
//#else	// !FV_RELEASE
//#	define FV_ASSERT( exp )
//#endif // FV_RELEASE

#ifdef FV_DEBUG
#	define FV_ASSERT_DEBUG		FV_ASSERT
#else // FV_DEBUG
#	define FV_ASSERT_DEBUG( exp )
#endif // !FV_DEBUG

//! modify by Uman, 20100524
#if defined( FV_SERVER ) || defined ( FV_EDITOR_ENABLED ) || !defined( FV_SHIPPING )
//#if defined( FV_SERVER ) || defined ( FV_EDITOR_ENABLED ) || !defined( FV_RELEASE )
#	define FV_ASSERT_DEV( exp )														\
			if (!(exp))																\
			{																		\
				FvDebugMsgHelper().DevCriticalMessage(								\
						"FV_ASSERT_DEV FAILED: " #exp "\n"							\
							__FILE__ "(%d)%s%s\n",									\
						(int)__LINE__,												\
						*FV_FUNCNAME ? " in " : "", FV_FUNCNAME );					\
			}

#else // (FV_SERVER || FV_EDITOR_ENABLED || !FV_RELEASE)
#	define FV_ASSERT_DEV( exp )
#endif // !(FV_SERVER || FV_EDITOR_ENABLED || !FV_RELEASE)

#define FV_IF_NOT_ASSERT_DEV( exp )												\
		if ((!( exp )) && (															\
			FvDebugMsgHelper().DevCriticalMessage(									\
				"FV_ASSERT_DEV FAILED: " #exp "\n"									\
					__FILE__ "(%d)%s%s\n", (int)__LINE__,							\
					*FV_FUNCNAME ? " in " : "", FV_FUNCNAME ),						\
			true))		


//#define FV_PRE( exp )	FV_ASSERT( exp )
//
//#define FV_POST( exp )	FV_ASSERT( exp )

//! modify by Uman, 20100524
#ifdef FV_SHIPPING
//#ifdef FV_RELEASE
#define FV_VERIFY( exp ) (exp)
#define FV_VERIFY_DEV( exp ) (exp)
#else
#define FV_VERIFY FV_ASSERT
#define FV_VERIFY_DEV FV_ASSERT_DEV
#endif

#define FV_EXIT(msg) {\
	FvDebugMsgHelper().CriticalMessage(												\
	"FATAL ERROR: " #msg "\n"														\
	__FILE__ "(%d)%s%s\n", (int)__LINE__,											\
	*FV_FUNCNAME ? " in " : "",														\
	FV_FUNCNAME );																	\
	FV_REAL_ASSERT																	\
}

class FV_KERNEL_API FvMainThreadTracker
{
public:
	FvMainThreadTracker();

	static bool IsCurrentThreadMain();
};

class FV_KERNEL_API FvCriticalErrorHandler
{
	static FvCriticalErrorHandler *ms_pkHandler;
public:
	enum Result
	{
		ENTERDEBUGGER = 0,
		EXITDIRECTLY
	};
	static FvCriticalErrorHandler *Get();
	static FvCriticalErrorHandler *Set( FvCriticalErrorHandler* );
	virtual ~FvCriticalErrorHandler(){}
	virtual Result Ask( const char *pcMsg ) = 0;
	virtual void RecordInfo( bool bWillExit ) = 0;
};


#if FV_ENABLE_STACK_TRACKER
DWORD ExceptionFilter(DWORD iExceptionCode);
#endif // FV_ENABLE_STACK_TRACKER

#endif // __FvDebug_H__ 