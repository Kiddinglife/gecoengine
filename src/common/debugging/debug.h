/*
 * Geco Gaming Company
 * All Rights Reserved.
 * Copyright (c)  2016 GECOEngine.
 *
 * GECOEngine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GECOEngine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// created on 02-June-2016 by Jackie Zhang
#ifndef _INCLUDE_GECO_ENGINE_DEBUG
#define _INCLUDE_GECO_ENGINE_DEBUG

#include <mutex>
#include <thread>
#include <assert.h>
#include <functional>
#include <algorithm>

#include "../geco-engine-feature.h"
#include "../geco-plateform.h"
#include "../ultils/geco-malloc.h"
#include "../ds/array.h"
#include "../ultils/ultils.h"

/**
 *	@file geco-engine-debug.h
 *
 *	This file contains macros and functions related to debugging.
 *
 *	A number of macros are defined to display debug information. They should be
 *	used as you would use the printf function. To use these macros,
 *	DECLARE_DEBUG_COMPONENT needs to be used in the cpp file. Its argument is
 *	the initial component priority.
 *
 *	With each message, there is an associated message priority.
 * A message is only displayed if its priority is not less than
 * DebugFilter::filterThreshold() plus the component's priority.
 *
 *	@param VERBOSE_MSG
 * Used to display trace information. That is, when you enter a method.
 *
 *	@param DEBUG_MSG
 * Used to display debug information such as what a variable is equal to.
 *
 *	@param INFO_MSG
 * Used to display general information such as when a particular process is started.
 *
 *	@param NOTICE_MSG
 * Used to display information that is more important than an
 *	INFO_MSG message but is not a possible error.
 *
 *	@param WARNING_MSG
 * Used to display warning messages. These are messages
 *	that could be errors and should be looked into.
 *
 *	@param ERROR_MSG
 * Used to display error messages. These are messages that are
 *	errors and need to be fixed. The software should hopefully
 *	be able to survive these situations.
 *
 *	@param CRITICAL_MSG
 * Used to display critical error messages. These are message
 *	that are critical and cause the program not to continue.
 *
 *	@param DEV_CRITICAL_MSG
 * Used to display development time only critical
 *	error messages. These are message that are critical and
 *	cause the program not to continue.
 *
 *	@param HACK_MSG
 * Used to display temporary messages.
 * This is the highest priority message that can be used to temporarily print a
 *	message. No code should be commited with this in it.
 *
 *	@param SCRIPT_MSG
 *	Used to display messages printed from Python script.
 *
 *	@param MF_ASSERT
 *	 is a macro that should be used instead of assert or ASSERT.
 */

namespace geco
{
namespace debugging
{

//-------------------------------------------------------
//  Section: globals
//-------------------------------------------------------
extern bool g_write2syslog;
extern std::string g_syslog_name;

//-------------------------------------------------------
//	Section: log_msg_filter_t
//-------------------------------------------------------

/*Definition for the critical message common callback functor*/
struct critical_msg_cb_tag
{
};
typedef std::function<
		bool(int component_priority, int msg_priority, const char * format,
				va_list args_list, critical_msg_cb_tag*)> critical_msg_cb_t;

/**
 *  @brief this is  the critical message specified callback class.
 *  it serves as base class for classes that want to handle critical
 *  messages in different ways, and also keeps track of the current critical
 *  message handler through its static methods.
 *  @note win32 has handler_r initialized
 *  other plateforms has to create their own handler class by inherting this class
 */
struct default_critical_msg_handler_t
{
	static default_critical_msg_handler_t* handler_;
public:
	enum Result
	{
		ENTERDEBUGGER = 0, EXITDIRECTLY
	};
	static default_critical_msg_handler_t* get()
	{
		return handler_;
	}
	static default_critical_msg_handler_t* set(
			default_critical_msg_handler_t* handler)
	{
		default_critical_msg_handler_t* old = handler_;
		handler_ = handler;
		return old;
	}
	virtual ~default_critical_msg_handler_t()
	{
	}
	virtual Result ask(const char* msg) { return ENTERDEBUGGER; };
	virtual void recordInfo(bool willExit) {};
};

/**
 *	Definition for the common message common callback functor. If the
 *  function returns true, the default behaviour for
 *  displaying messages is ignored.
 **/
struct debug_msg_cb_tag
{
};
typedef std::function<
		bool(int component_priority, int msg_priority, const char * format,
				va_list args_list, debug_msg_cb_tag*)> debug_msg_cb_t;

struct error_msg_cb_tag
{
};
typedef std::function<
		bool(int component_priority, int msg_priority, const char * format,
				va_list args_list, error_msg_cb_tag*)> error_msg_cb_t;

struct warnning_msg_cb_tag
{
};
typedef std::function<
		bool(int component_priority, int msg_priority, const char * format,
				va_list args_list, warnning_msg_cb_tag*)> warnning_msg_cb_t;

struct info_msg_cb_tag
{
};
typedef std::function<
		bool(int component_priority, int msg_priority, const char * format,
				va_list args_list, info_msg_cb_tag*)> info_msg_cb_t;

struct notice_msg_cb_tag
{
};
typedef std::function<
		bool(int component_priority, int msg_priority, const char * format,
				va_list args_list, notice_msg_cb_tag*)> notice_msg_cb_t;

struct trace_msg_cb_tag
{
};
typedef std::function<
		bool(int component_priority, int msg_priority, const char * format,
				va_list args_list, trace_msg_cb_tag*)> trace_msg_cb_t;

/** This class is used to help filter log messages.*/
struct log_msg_filter_t  //oldname - DebugFilter
{

	static log_msg_filter_t* s_instance_;
	static bool shouldWriteTimePrefix;
	static bool shouldWriteToConsole;

	typedef geco::ds::array_t<critical_msg_cb_t*> critical_msg_cbs_t;
	typedef geco::ds::array_t<error_msg_cb_t*> error_msg_cbs_t;
	typedef geco::ds::array_t<warnning_msg_cb_t*> warnning_msg_cbs_t;
	typedef geco::ds::array_t<debug_msg_cb_t*> debug_msg_cbs_t;
	typedef geco::ds::array_t<info_msg_cb_t*> info_msg_cbs_t;
	typedef geco::ds::array_t<notice_msg_cb_t*> notice_msg_cbs_t;
	typedef geco::ds::array_t<trace_msg_cb_t*> trace_msg_cbs_t;

	critical_msg_cbs_t critical_msg_cbs_;
	error_msg_cbs_t error_msg_cbs_;
	warnning_msg_cbs_t warnning_msg_cbs_;
	debug_msg_cbs_t debug_msg_cbs_;
	info_msg_cbs_t info_msg_cbs_;
	notice_msg_cbs_t notice_msg_cbs_;
	trace_msg_cbs_t trace_msg_cbs_;

	// Only messages with a message priority greater than or equal
	// to this limit will be displayed.
	int filter_threshold_;

	// FALSE: GECO_ASSERT_DEV and GECO_ASSERT_DEV_IFNOT  DO_NOT cause a real assertion.
	// defualt TRUE: GECO_ASSERT_DEV and GECO_ASSERT_DEV_IFNOT  DO cause a real assertion.
	bool has_dev_assert_;

	log_msg_filter_t()
	{
		filter_threshold_ = 0;
		has_dev_assert_ = true;
		critical_msg_cbs_.reserve(64);
		error_msg_cbs_.reserve(64);
		warnning_msg_cbs_.reserve(64);
		debug_msg_cbs_.reserve(64);
		info_msg_cbs_.reserve(64);
		notice_msg_cbs_.reserve(64);
		trace_msg_cbs_.reserve(64);
	}

	/** This method returns the singleton instance of this class.*/
	static log_msg_filter_t& get_instance()
	{
		if (s_instance_ == NULL)
		{
			s_instance_ = new log_msg_filter_t;
		}
		return *s_instance_;
	}

	static void free_instance()
	{
		if (s_instance_ != NULL)
		{
			delete s_instance_;
			s_instance_ = NULL;
		}
	}

	/**
	 * @brief returns whether or not a message should be accepted.
	 * @details
	 * when component_priority <= filter_threshold_, only accept msg with
	 * priority >= filter_threshold_
	 * when component_priority > filter_threshold_, only accept msg with
	 * priority >= component_priority */
	static bool should_accept(int component_priority, int msg_riority)
	{
		return (msg_riority
				>= MAX(log_msg_filter_t::get_instance().filter_threshold_,
						component_priority));
	}

	/**
	 * @brief This method sets a callback associated with a critical message,
	 * log_msg_filter_t now owns the object pointed to by pCallback.
	 * @return index of the position where this cb is stored*/
	unsigned int add(critical_msg_cb_t * pCallback)
	{
		critical_msg_cbs_.push_back(pCallback);
		return (critical_msg_cbs_.size() - 1);
	}
	/**
	 *	@brief This method sets a callback associated with a error message,
	 *  log_msg_filter_t now owns the object pointed to by pCallback.
	 * @return index of the position where this cb is stored*/
	unsigned int add(error_msg_cb_t * pCallback)
	{
		error_msg_cbs_.push_back(pCallback);
		return (error_msg_cbs_.size() - 1);
	}
	/**
	 *	@brief This method sets a callback associated with a warnning message,
	 *   log_msg_filter_t now owns the object pointed to by pCallback.
	 * @return index of the position where this cb is stored*/
	unsigned int add(warnning_msg_cb_t * pCallback)
	{
		warnning_msg_cbs_.push_back(pCallback);
		return (warnning_msg_cbs_.size() - 1);
	}
	/**
	 *	@brief This method sets a callback associated with a info message,
	 *  log_msg_filter_t now owns the object pointed to by pCallback.
	 * @return index of the position where this cb is stored*/
	unsigned int add(info_msg_cb_t * pCallback)
	{
		info_msg_cbs_.push_back(pCallback);
		return (info_msg_cbs_.size() - 1);
	}
	/**
	 *  @brief This method sets a callback associated with a info message,
	 *  log_msg_filter_t now owns the object pointed to by pCallback.
	 * @return index of the position where this cb is stored*/
	unsigned int add(notice_msg_cb_t * pCallback)
	{
		notice_msg_cbs_.push_back(pCallback);
		return (notice_msg_cbs_.size() - 1);
	}
	/**
	 * @brief sets a callback associated with a debug message.
	 * @details If the callback is set and returns true, the default
	 * behaviour for displaying messages is not done.
	 * DebugFilter now owns the object pointed to by pCallback.
	 * @return index of the position where this cb is stored*/
	unsigned int add(debug_msg_cb_t* pCallback)
	{
		debug_msg_cbs_.push_back(pCallback);
		return (debug_msg_cbs_.size() - 1);
	}
	/**
	 *  @brief This method sets a callback associated with a info message,
	 *  log_msg_filter_t now owns the object pointed to by pCallback.
	 * @return index of the position where this cb is stored*/
	unsigned int add(trace_msg_cb_t * pCallback)
	{
		trace_msg_cbs_.push_back(pCallback);
		return (trace_msg_cbs_.size() - 1);
	}
	void remove(unsigned int idx, critical_msg_cb_tag*)
	{
		critical_msg_cbs_.remove_fast(idx);
	}
	void remove(unsigned int idx, error_msg_cb_tag*)
	{
		error_msg_cbs_.remove_fast(idx);
	}
	void remove(unsigned int idx, warnning_msg_cb_tag*)
	{
		warnning_msg_cbs_.remove_fast(idx);
	}
	void remove(unsigned int idx, debug_msg_cb_tag*)
	{
		debug_msg_cbs_.remove_fast(idx);
	}
	void remove(unsigned int idx, info_msg_cb_tag*)
	{
		info_msg_cbs_.remove_fast(idx);
	}
	void remove(unsigned int idx, trace_msg_cb_tag*)
	{
		trace_msg_cbs_.remove_fast(idx);
	}
	void remove(unsigned int idx, notice_msg_cb_tag*)
	{
		notice_msg_cbs_.remove_fast(idx);
	}
};

/**
 *	This enumeration is used to indicate the priority of a message. The higher
 *	the enumeration's value, the higher the priority.
 */
enum LogMsgPriority
{
	LOG_MSG_VERBOSE,
	LOG_MSG_DEBUG,
	LOG_MSG_INFO,
	LOG_MSG_NOTICE,
	LOG_MSG_WARNING,
	LOG_MSG_ERROR,
	LOG_MSG_CRITICAL,
	LOG_MSG_HACK,
	LOG_MSG_SCRIPT,
	LOG_MSG_ASSET,
	NUM_LOG_MSG
};

const char * get_logmsg_prefix_str(LogMsgPriority p);

//-------------------------------------------------------
//	Section: log_msg_helper
//-------------------------------------------------------

/**
 *  @brief This class implements the functionality exposed by message macros.
 *	manages calling registered message callbacks, and handles both critical
 *  and non-critical messages.
 */
struct log_msg_helper
{
	static bool show_error_dialogs_;
	static bool critical_msg_occurs_;
	static std::mutex* mutex_;

	int cpn_priority_;
	int msg_priority_;

	log_msg_helper(int componentPriority, int messagePriority) :
			cpn_priority_(componentPriority), msg_priority_(messagePriority)
	{
	}
	log_msg_helper() :
			cpn_priority_(0), msg_priority_(LOG_MSG_CRITICAL)
	{
	}

#ifndef _WIN32
	void message(const char * format, ...)
			__attribute__((format(printf, 2, 3)));
	void critical_msg(const char * format, ...)
			__attribute__((format(printf, 2, 3)));
	void dev_critical_msg(const char * format, ...)
			__attribute__((format(printf, 2, 3)));
#else
	static bool automated_test_;
	static void log2file(const char* line);
	void message(const char * format, ...);
	/**
	 *	This is a helper function used by the CRITICAL_MSG macro.
	 */
	void critical_msg(const char * format, ...);
	/**
	 *	@brief This is a helper function used by the CRITICAL_MSG macro.
	 * If DebugFilter::hasDevelopmentAssertions() is true, this will cause a proper
	 *	critical message otherwise, it'll behaviour similar to a normal error
	 *	message. */
	void dev_critical_msg(const char * format, ...);  // devCriticalMessage
#endif

	/**
	 * @brief Format: <executable path>(<mangled-function-name>+<function instruction offset>) [<eip>].
	 * 一般察看函数运行时堆栈的方法是使用GDB（bt命令）之类的外部调试器,但是,有些时候为了分析程序的BUG,(主要针对长时间运行程序的分析),
	 * 在程序出错时打印出函数的调用堆栈是非常有用的。
	 * 在glibc头文件"execinfo.h"中声明了三个函数用于获取当前线程的函数调用堆栈。
	 */
	void msg_back_trace();
	static void should_write2syslog(bool state = true)
	{
		g_write2syslog = state;
	}

	/**
	 * This method allow tools to have a common method
	 * to set whether to show error dialogs or not
	 *  Do this in a thread-safe way.
	 */
	static void show_error_dialogs(bool show)
	{
		if (mutex_ == NULL)
			mutex_ = new std::mutex;
		mutex_->lock();
		show_error_dialogs_ = show;
		mutex_->unlock();
	}
	static bool show_error_dialogs()
	{
		if (mutex_ == NULL)
			mutex_ = new std::mutex;
		mutex_->lock();
		bool show_error_dialogs = show_error_dialogs_;
		mutex_->unlock();
		return show_error_dialogs;
	}

	//criticalMessageHelper
	void critical_msg_aux(bool isDevAssertion, const char * format,
			va_list argPtr);

	static void fini()
	{
		if (mutex_)
			delete mutex_;
		mutex_ = NULL;
		log_msg_filter_t::free_instance();
	}
};

/* This class is used to query if the current thread is the main thread.*/
struct main_thread_tracker_t
{
	/*
	 *	This static thread-local variable is initialised to false, and set to true
	 *	in the constructor of the static s_mainThreadTracker object below
	 */
	static thread_local bool is_curr_thread_main_thread_;

	main_thread_tracker_t()
	{
		main_thread_tracker_t::is_curr_thread_main_thread_ = true;
	}

	/*
	 * @brief
	 * Static method that returns true if the current thread is the main thread,
	 * false otherwise.
	 *	@return      true if the current thread is the main thread, false if not
	 */
	bool is_curr_thread_main_thread()
	{
		return main_thread_tracker_t::is_curr_thread_main_thread_;
	}
};

//-------------------------------------------------------
//	Section: printf functions
//-------------------------------------------------------
#if ENABLE_DPRINTF
// This function prints a debug message.
#ifndef _WIN32
void dprintf(const char * format, ...) __attribute__((format(printf, 1, 2)));
void dprintf(int componentPriority, int messagePriority, const char * format,
		...) __attribute__((format(printf, 3, 4)));
#else
void dprintf(const char * format, ...);
void dprintf(int componentPriority, int messagePriority,
		const char * format, ...);
#endif
void vdprintf(const char * format, va_list argPtr, const char * prefix =
NULL);
void vdprintf(int componentPriority, int messagePriority, const char * format,
		va_list argPtr, const char * prefix = NULL);
#else
/// This function prints a debug message.
inline void dprintf(const char * format, ...)
{}
inline void dprintf(int componentPriority, int messagePriority,
		const char * format, ...)
{}
inline void vdprintf(const char * format, va_list argPtr,
		const char * prefix = NULL)
{}
inline void vdprintf(int componentPriority, int messagePriority,
		const char * format, va_list argPtr, const char * prefix = NULL)
{}
#endif
}
}

//-------------------------------------------------------
//	Section: Debug Macros
//-------------------------------------------------------
#define LOG_BUFSIZ 5012
#include <assert.h>
#ifdef __ASSERT_FUNCTION
#define GECO_FUNCNAME __ASSERT_FUNCTION
#else
#define GECO_FUNCNAME ""
#endif

#ifdef GECO_USE_ASSERTS
#define GECO_REAL_ASSERT assert(0);
#else
#define GECO_REAL_ASSERT
#endif

#ifdef _DEBUG
// GECO_ASSERT_DEBUG is like GECO_ASSERT except only evaluated in debug builds.
#define GECO_ASSERT_DEBUG		GECO_ASSERT
#else
/**
 *	This macro should be used instead of assert. It is enabled only
 *	in debug builds, unlike MF_ASSERT which is enabled in both
 *	debug and hybrid builds.
 */
#define GECO_ASSERT_DEBUG( exp )
#endif

/**
 * This macro should be used instead of assert.
 * @see GECO_ASSERT_DEBUG
 */
#if !defined( _RELEASE )
#define GECO_ASSERT( exp )\
if (!(exp)){log_msg_helper().critical_msg(	"ASSERTION FAILED: " #exp "\n" \
__FILE__ "(%d)%s%s\n",(int)__LINE__, *GECO_FUNCNAME ? " in " : "", GECO_FUNCNAME); GECO_REAL_ASSERT}
#else	// _RELEASE
#define GECO_ASSERT( exp )
#endif // !_RELEASE

/**
 *	 An assertion which is only lethal when not in a production environment
 */
#if defined( SERVER_BUILD ) || !defined( _RELEASE )
#define GECO_ASSERT_DEV( exp )													\
if (!(exp)){log_msg_helper().dev_critical_msg("GECO_ASSERT_DEV FAILED: " #exp "\n"	\
__FILE__ "(%d)%s%s\n",(int)__LINE__, *GECO_FUNCNAME ? " in " : "", GECO_FUNCNAME );}
#else
/**Empty versions of above function - not available on client release builds.*/
#define GECO_ASSERT_DEV( exp )
#endif

/**
 *	An assertion which is only lethal when not in a production environment.
 *	In a production environment, the block of code following the macro will
 *	be executed if the assertion fails.
 *	@see GECO_ASSERT_DEV
 */
#define GECO_ASSERT_DEV_IFNOT( exp )\
if ((!( exp )) && (log_msg_helper().dev_critical_msg("GECO_ASSERT_DEV_IFNOT FAILED: " #exp "\n" \
__FILE__ "(%d)%s%s\n", (int )__LINE__,*GECO_FUNCNAME ? " in " : "", GECO_FUNCNAME ),true))
        // leave trailing block after message

/** this is a placeholder until a better solution can be implemented. */
#define GECO_EXIT(msg) \
{log_msg_helper().critical_msg("FATAL ERROR: " #msg "\n" __FILE__ "(%d)%s%s\n", \
(int)__LINE__,*GECO_FUNCNAME ? " in " : "",GECO_FUNCNAME ); GECO_REAL_ASSERT}

/**
 *	This macro is used to assert a pre-condition.
 *	@see GECO_ASSERT
 *	@see POST
 */
#define PRE( exp )	GECO_ASSERT( exp )

/**
 *	This macro is used to assert a post-condition.
 *	@see GECO_ASSERT
 *	@see PRE
 */
#define POST( exp )	GECO_ASSERT( exp )

/**
 *	This macro used to display trace information. Can be used later to add in
 *	our own callstack if necessary.
 */
#define Enter(className,methodName) VERBOSE_MSG( "Enter " className "::" methodName "\n" )
#define Leave(className,methodName) VERBOSE_MSG( "Leave " className "::" methodName "\n" )

/**
 *	This macro is used to verify an expression. In non-release it
 *	asserts on failure, and in release the expression is still
 *	evaluated.
 *	@see GECO_ASSERT
 */
#ifdef _RELEASE
#define GECO_VERIFY( exp ) (exp)
#define GECO_VERIFY_DEV( exp ) (exp)
#else
#define GECO_VERIFY GECO_ASSERT
#define GECO_VERIFY_DEV GECO_ASSERT_DEV
#endif

        //------------------------------------------------------------------
		//                	Section: 	*_MSG macros.
		//--------------------------------------------------------------------
		/// This macro prints a debug message with CRITICAL priority.
		/// CRITICAL_MSG is always enabled no matter what the build target is.
#define CRITICAL_MSG \
geco::debugging::log_msg_helper(const_cpnt_priority,LOG_MSG_CRITICAL ).critical_msg
/// This macro prints a development time only message CRITICAL priority.
#define DEV_CRITICAL_MSG	CRITICAL_MSG

#if ENABLE_MSG_LOGGING
#define MSG_BACK_TRACE(PRIORITY)\
geco::debugging::log_msg_helper(::const_cpnt_priority,PRIORITY).msg_back_trace
#define TRACE_BACKTRACE		    MSG_BACK_TRACE( LOG_MSG_VERBOSE )
#define DEBUG_BACKTRACE		MSG_BACK_TRACE( LOG_MSG_DEBUG )
#define INFO_BACKTRACE		    MSG_BACK_TRACE( LOG_MSG_INFO )
#define NOTICE_BACKTRACE	    MSG_BACK_TRACE( LOG_MSG_NOTICE )
#define WARNING_BACKTRACE	MSG_BACK_TRACE( LOG_MSG_WARNING )
#define ERROR_BACKTRACE		MSG_BACK_TRACE( LOG_MSG_ERROR )
#define CRITICAL_BACKTRACE	    MSG_BACK_TRACE( LOG_MSG_CRITICAL )
#define HACK_BACKTRACE		    MSG_BACK_TRACE( LOG_MSG_HACK )

// The following macros are used to display debug information.
// all msgs with pri lower than PRIORITY will NOT be printed out.
#define MSG_DEBUG(PRIORITY) geco::debugging::log_msg_helper(::const_cpnt_priority,PRIORITY).message
/// This macro prints a debug message with TRACE priority.
#define VERBOSE_MSG		MSG_DEBUG( LOG_MSG_VERBOSE )
/// This macro prints a debug message with DEBUG priority.
#define DEBUG_MSG		MSG_DEBUG( LOG_MSG_DEBUG )
/// This macro prints a debug message with INFO priority.
#define INFO_MSG		    MSG_DEBUG( LOG_MSG_INFO )
/// This macro prints a debug message with NOTICE priority.
#define NOTICE_MSG		MSG_DEBUG( LOG_MSG_NOTICE )
/// This macro prints a debug message with WARNING priority.
#define WARNING_MSG	MSG_DEBUG( LOG_MSG_WARNING )
/// This macro prints a debug message with ERROR priority.
#define ERROR_MSG	    MSG_DEBUG( LOG_MSG_ERROR )
/// This macro prints a debug message with HACK priority.
#define HACK_MSG		MSG_DEBUG( LOG_MSG_HACK )
/// This macro prints a debug message with SCRIPT priority.
#define SCRIPT_MSG		MSG_DEBUG( LOG_MSG_SCRIPT )
#else
#define NULL_MSG(...)														\
	do																		\
	{																		\
		if (false)															\
			printf(__VA_ARGS__);											\
	}																		\
	while (false)
#define VERBOSE_MSG(...)			NULL_MSG(__VA_ARGS__)
#define DEBUG_MSG(...)			NULL_MSG(__VA_ARGS__)
#define INFO_MSG(...)			NULL_MSG(__VA_ARGS__)
#define NOTICE_MSG(...)			NULL_MSG(__VA_ARGS__)
#define WARNING_MSG(...)		NULL_MSG(__VA_ARGS__)
#define ERROR_MSG(...)			NULL_MSG(__VA_ARGS__)
#define HACK_MSG(...)			NULL_MSG(__VA_ARGS__)
#define SCRIPT_MSG(...)			NULL_MSG(__VA_ARGS__)
#define DEV_CRITICAL_MSG(...)	NULL_MSG(__VA_ARGS__)
#endif

// This is the default s_componentPriority for files that does not
// have DECLARE_DEBUG_COMPONENT2(). Useful for hpp and ipp files that
// uses debug macros. s_componentPriority declared by
//	DECLARE_DEBUG_COMPONENT2() will have precedence over this one.
namespace
{
const int const_cpnt_priority = 0;
}

#ifndef _RELEASE
#if ENABLE_WATCHERS
/**
 *	 @brief
 *  needs to be placed in a cpp file before any of the *_MSG macros can be used.
 *
 *	@param module
 * The name (or path) of the watcher module that the component
 * priority should be displayed in.
 *	@param priority	The initial component priority of the messages in the file.
 */
    //impled in watcher.cpp
extern bool init_value_watcher(int& value, const char * path);
#define DECLARE_DEBUG_COMPONENT2(module, priority)\
static int const_cpnt_priority = priority;\
static bool IGNORE_THIS_COMPONENT_WATCHER_INIT = \
init_value_watcher(::const_cpnt_priority,get_base_path( __FILE__, module ) );
#else
#define DECLARE_DEBUG_COMPONENT2( module, priority )\
static int const_cpnt_priority = priority;
#endif
#else
#define DECLARE_DEBUG_COMPONENT2( module, priority )
#endif

/**
 *	needs to be placed in a cpp file before any of the *_MSG macros can be used.
 *
 *	@param priority	The initial component priority of the messages in the file.
 */
#define DECLARE_DEBUG_COMPONENT(priority) \
DECLARE_DEBUG_COMPONENT2( NULL, priority )

using namespace geco::debugging;
#endif
