//{future header message}
#ifndef __FvKernel_H__
#define __FvKernel_H__

#include <FvPowerDefines.h>

#ifdef FV_AS_STATIC_LIBS
#define FV_KERNEL_API
#else
#ifdef FV_KERNEL_EXPORT
#define FV_KERNEL_API FV_Proper_Export_Flag
#else
#define FV_KERNEL_API FV_Proper_Import_Flag
#endif
#endif

//+-------------------------------------------------------------------------------------------------

#define FV_SAFE_DELETE(p) if(p) {delete (p); (p) = NULL;}
#define FV_SAFE_DELETE_ARRAY(p) if(p) {delete [] (p); (p) = NULL;}


//+-------------------------------------------------------------------------------------------------
//! 声明构造和比较函数
#ifndef FV_DECLARE_COPY_CONSTRUCTOR
#define FV_DECLARE_COPY_CONSTRUCTOR(classname)  classname(const classname & );
#endif

#ifndef FV_DECLARE_COPY
#define FV_DECLARE_COPY(classname)  void operator=( const classname& );
#endif

#ifndef FV_DECLARE_COMPARE
#define FV_DECLARE_COMPARE(classname)  bool operator==( const classname& );
#endif

//+-------------------------------------------------------------------------------------------------
//! 声明构造和比较函数
#ifndef FV_DEFINE_COPY_CONSTRUCTOR
#define FV_DEFINE_COPY_CONSTRUCTOR(classname)  classname(const classname & ){}
#endif

#ifndef FV_DEFINE_COPY
#define FV_DEFINE_COPY(classname)  void operator=( const classname& ){}
#endif

#ifndef FV_DEFINE_COMPARE
#define FV_DEFINE_COMPARE(classname)  bool operator==( const classname& ){return false;}
#endif

//+-------------------------------------------------------------------------------------------------

#ifndef FV_NOT_COPY
#define FV_NOT_COPY(classname) \
	FV_DECLARE_COPY(classname);\
	FV_DECLARE_COPY_CONSTRUCTOR(classname);
#endif


#ifndef FV_NOT_COPY_COMPARE
#define FV_NOT_COPY_COMPARE(classname) \
	FV_DECLARE_COPY(classname);\
	FV_DECLARE_COMPARE(classname);\
	FV_DECLARE_COPY_CONSTRUCTOR(classname);
#endif

//+-------------------------------------------------------------------------------------------------



#ifndef FV_CHECK_RETURN
#define FV_CHECK_RETURN(b)  {if((b) == false) {return;}}
#endif

#ifndef FV_CHECK_RETURN_FALSE
#define FV_CHECK_RETURN_FALSE(b)  {if((b) == false) {return false;}}
#endif

#endif // __FvKernel_H__