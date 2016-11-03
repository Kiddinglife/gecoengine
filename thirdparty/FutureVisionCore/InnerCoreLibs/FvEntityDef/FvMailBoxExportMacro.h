//{future header message}
#ifndef __FvMailBoxExportMacro_H__
#define __FvMailBoxExportMacro_H__



//! 实现 FvObjMailBox的GlobalMB接口
#if defined(FV_BASE) || defined(FV_CELL)

#define FVMAILBOX_GLOBALMAILBOX_IMPLEMENT(_MAILBOX)							\
	const _MAILBOX& _MAILBOX::GlobalMB(const FvString& kLabel)				\
	{																		\
		FvMailBox* pkMB = FvEntityManager::Instance().FindGlobalBase(kLabel);\
		if(pkMB)															\
		{																	\
			return *_MAILBOX::Cast(pkMB);									\
		}																	\
		else																\
		{																	\
			FV_ERROR_MSG("CanNot find GlobalMB(%s)\n", kLabel.c_str());		\
			static _MAILBOX s_kMB;											\
			return s_kMB;													\
		}																	\
	}

#else

#define FVMAILBOX_GLOBALMAILBOX_IMPLEMENT(_MAILBOX)

#endif


#define MAILBOX_IOSTREAM_SPECIALIZE(_PREFIX, _MAILBOX)						\
	template<>																\
	_PREFIX FvBinaryIStream& operator>>(FvBinaryIStream& kIS, _MAILBOX& kMB)\
	{																		\
		return operator>><FvMailBox>(kIS, kMB);								\
	}																		\
	template<>																\
	_PREFIX FvBinaryOStream& operator<<(FvBinaryOStream& kOS, const _MAILBOX& kMB)	\
	{																		\
		return operator<<<FvMailBox>(kOS, kMB);								\
	}


#ifdef FV_SERVER

#define MAILBOX_PRINT_SPECIALIZE(_PREFIX, _MAILBOX)									\
	namespace FvDataPrint															\
	{																				\
		template<>																	\
		_PREFIX void Print(const char* pcName, const _MAILBOX& kVal, FvString& kMsg)\
		{																			\
			FvDataPrint::Print(pcName, static_cast<const FvObjMailBox&>(kVal), kMsg);\
		}																			\
		template<>																	\
		_PREFIX void Print(const _MAILBOX& kVal, FvString& kMsg)					\
		{																			\
			FvDataPrint::Print(static_cast<const FvObjMailBox&>(kVal), kMsg);		\
		}																			\
	}

#else

#define MAILBOX_PRINT_SPECIALIZE(_PREFIX, _MAILBOX)									\
	namespace FvDataPrint															\
	{																				\
		template<>																	\
		_PREFIX void Print(const char* pcName, const _MAILBOX& kVal, FvString& kMsg)\
		{																			\
			FV_ASSERT(pcName);														\
			FvDataPrint::Print(kMsg, "%s[MailBox]", pcName);						\
		}																			\
		template<>																	\
		_PREFIX void Print(const _MAILBOX& kVal, FvString& kMsg)					\
		{																			\
			kMsg.append("MAILBOX");													\
		}																			\
	}

#endif


#ifdef FV_SERVER

#define MAILBOX_XMLDATA_SPECIALIZE(_PREFIX, _MAILBOX)								\
	namespace FvXMLData																\
	{																				\
		template<>																	\
		_PREFIX void Read(FvXMLSectionPtr spSection, _MAILBOX& kVal)				\
		{																			\
			FvXMLData::Read(spSection, static_cast<FvObjMailBox&>(kVal));			\
		}																			\
		template<>																	\
		_PREFIX void Write(FvXMLSectionPtr spSection, const _MAILBOX& kVal)			\
		{																			\
			FvXMLData::Write(spSection, static_cast<const FvObjMailBox&>(kVal));	\
		}																			\
	}

#else

#define MAILBOX_XMLDATA_SPECIALIZE(_PREFIX, _MAILBOX)								\
	namespace FvXMLData																\
	{																				\
		template<>																	\
		_PREFIX void Read(FvXMLSectionPtr spSection, _MAILBOX& kVal)				\
		{																			\
		}																			\
		template<>																	\
		_PREFIX void Write(FvXMLSectionPtr spSection, const _MAILBOX& kVal)			\
		{																			\
		}																			\
	}

#endif


#endif//__FvMailBoxExportMacro_H__
