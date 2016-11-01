//{future header message}
#ifndef __FvServerConfig_H__
#define __FvServerConfig_H__

#include <string>
#include <vector>
#include <queue>

#include <FvPowerDefines.h>
#include <FvBaseTypes.h>
#include <FvWatcher.h>
#include <FvXMLSection.h>

#include "FvServerCommon.h"

#include <typeinfo>

namespace FvServerConfig
{
	typedef std::pair< FvXMLSectionPtr, FvString > ConfigElement;
	typedef std::vector< ConfigElement > Container;
	FV_SERVERCOMMON_API extern int g_iShouldDebug;

	FV_SERVERCOMMON_API bool Init( int argc, char * argv[] );
	FV_SERVERCOMMON_API bool Hijack( Container & container, int argc, char * argv[] );

	FV_SERVERCOMMON_API bool IsBad();

	template <class C>
		const char *TypeOf( const C & c ) { return typeid(c).name(); }
	FV_INLINE const char *TypeOf( char * ) { return "String"; }
	FV_INLINE const char *TypeOf( FvString ) { return "String"; }
	FV_INLINE const char *TypeOf( bool ) { return "Boolean"; }
	FV_INLINE const char *TypeOf( int ) { return "Integer"; }
	FV_INLINE const char *TypeOf( float ) { return "Float"; }

	void FV_SERVERCOMMON_API PrettyPrint( const char * path,
			const FvString & oldValue, const FvString & newValue,
			const char * typeStr, bool isHit );

	template <class C>
	void PrintDebug( const char *pcPath,
			const C & kOldValue, C kNewValue, bool bIsHit )
	{
		//FvString kOldValueStr = WatcherValueToString( kOldValue );
		//FvString kNewValueStr = WatcherValueToString( kNewValue );

		//PrettyPrint( pcPath, kOldValueStr, kNewValueStr,
		//		TypeOf( kOldValue ), bIsHit );
	}

	FvXMLSectionPtr FV_SERVERCOMMON_API GetSection( const char * path,
		FvXMLSectionPtr defaultValue = NULL );

	class FV_SERVERCOMMON_API SearchIterator
	{
	public:
		SearchIterator();
		SearchIterator( const FvString &kSearchName,
				const Container &kChain );
		SearchIterator( const SearchIterator &kCopy );

		~SearchIterator(){}

		FvXMLSectionPtr operator*();
		SearchIterator & operator++();
		SearchIterator operator++( int );

		FvString CurrentConfigPath() const { return m_kCurrentConfigPath; }

		bool operator==( const SearchIterator &kOther ) const
		{
			return kOther.m_kIter == m_kIter &&
				kOther.m_kChainTail == m_kChainTail;
		}

		bool operator!=( const SearchIterator &kOther ) const
		{
			return !this->operator==( kOther );
		}

		SearchIterator & operator=( const SearchIterator &kOther );

	private:

		void FindNext();

	private:

		FvXMLSection::SearchIterator m_kIter;

		typedef std::queue< ConfigElement > ConfigElementQueue;
		ConfigElementQueue m_kChainTail;

		FvString m_kSearchName;

		FvString m_kCurrentConfigPath;
	};

	FV_SERVERCOMMON_API SearchIterator BeginSearchSections( const char * path );

	FV_SERVERCOMMON_API const SearchIterator &EndSearch();

	void FV_SERVERCOMMON_API GetChildrenNames( std::vector< FvString >& childrenNames,
		const char * parentPath );

	template <class C>
		bool Update( const char * path, C & kValue )
	{
		FvXMLSectionPtr pSect = GetSection( path );

		if (g_iShouldDebug)
		{
			PrintDebug( path, kValue,
					pSect ? pSect->As(kValue) : kValue, pSect );
		}

		if (pSect) kValue = pSect->As(kValue);

		return bool(pSect);
	}

	template <class C>
		C Get( const char *pcPath, const C &kDefaultValue )
	{
		FvXMLSectionPtr spSect = GetSection( pcPath );

		if (g_iShouldDebug)
		{
			PrintDebug( pcPath, kDefaultValue,
				spSect ? spSect->As(kDefaultValue) : kDefaultValue, spSect );
		}

		if (spSect)
		{
			return spSect->As(kDefaultValue);
		}

		return kDefaultValue;
	}

	FV_SERVERCOMMON_API FvString Get( const char * path, const char * defaultValue );

	FV_INLINE
	FvString Get( const char * path )
	{
		return Get( path, "" );
	}
};

#define FV_REGISTER_WATCHER( ID, ABRV, NAME, CONFIG_PATH, NUB )

#define FV_INTERNAL_INTERFACE( CONFIG_PATH )						\
			FvServerConfig::Get( #CONFIG_PATH "/internalInterface",	\
				FvServerConfig::Get( "internalInterface",			\
					FvNetNub::USE_FVMACHINED ) ).c_str()

#define FV_MACHINE_PORT												\
			FvServerConfig::Get( "machinePort",						\
				FvUInt16(FV_NET_PORT_MACHINED) )

#define FV_USER_ID													\
			FvServerConfig::Get( "UserID",							\
				FvUInt16(1) )

#define FV_WRITE_TO_CONSOLE( CONFIG_PATH )							\
			g_iShouldWriteToConsole =								\
			FvServerConfig::Get( #CONFIG_PATH "/shouldWriteToConsole",\
				FvServerConfig::Get( "shouldWriteToConsole",		\
					bool(true) ) )

#define FV_OPEN_LOG_WITH_PRIORITY( CONFIG_PATH, PRIORITY )			\
			bool bWriteToLog =										\
			FvServerConfig::Get( #CONFIG_PATH "/shouldWriteToLog",	\
				FvServerConfig::Get( "shouldWriteToLog",			\
					bool(true) ) );									\
			MessageFile* pkMsgFile(NULL);							\
			if(bWriteToLog)											\
			{														\
				SYSTEMTIME nowTime;									\
				GetLocalTime(&nowTime);								\
				srand(nowTime.wMilliseconds);						\
				char logName[128] = {0};							\
				sprintf_s(logName, sizeof(logName),					\
					#CONFIG_PATH "_%04d%02d%02d_%02d%02d%02d_%05u.log",\
					nowTime.wYear,									\
					nowTime.wMonth,									\
					nowTime.wDay,									\
					nowTime.wHour,									\
					nowTime.wMinute,								\
					nowTime.wSecond,								\
					rand());										\
				pkMsgFile = new MessageFile(logName);				\
				FvDebugFilter::Instance().AddMessageCallback(pkMsgFile);\
				FvDebugFilter::Instance().FilterThreshold(PRIORITY);\
			}void(0)

#define FV_OPEN_LOG( CONFIG_PATH )									\
			FV_OPEN_LOG_WITH_PRIORITY( CONFIG_PATH, FV_DEBUG_MESSAGE_PRIORITY_TRACE)

#define FV_CLOSE_LOG()												\
			if(bWriteToLog)											\
			{														\
				FV_ASSERT(pkMsgFile);								\
				FvDebugFilter::Instance().DeleteMessageCallback(pkMsgFile);\
				delete pkMsgFile;									\
			}void(0)

#define FV_OPEN_LOG_WITH_PRIORITY_FILE( CONFIG_PATH, PRIORITY, PMSGFILE )\
			do														\
			{														\
				bool bWriteToLog =									\
				FvServerConfig::Get( #CONFIG_PATH "/shouldWriteToLog",\
				FvServerConfig::Get( "shouldWriteToLog",			\
				bool(true) ) );										\
				if(bWriteToLog)										\
				{													\
					SYSTEMTIME nowTime;								\
					GetLocalTime(&nowTime);							\
					srand(nowTime.wMilliseconds);					\
					char logName[128] = {0};						\
					sprintf_s(logName, sizeof(logName),				\
						#CONFIG_PATH "_%04d%02d%02d_%02d%02d%02d_%05u.log",\
						nowTime.wYear,								\
						nowTime.wMonth,								\
						nowTime.wDay,								\
						nowTime.wHour,								\
						nowTime.wMinute,							\
						nowTime.wSecond,							\
						rand());									\
					PMSGFILE = new MessageFile(logName);			\
					FvDebugFilter::Instance().AddMessageCallback(PMSGFILE);\
					FvDebugFilter::Instance().FilterThreshold(PRIORITY);\
				}													\
			}while(0)

#define FV_OPEN_LOG_WITH_FILE( CONFIG_PATH, PMSGFILE )				\
			FV_OPEN_LOG_WITH_PRIORITY_FILE( CONFIG_PATH, FV_DEBUG_MESSAGE_PRIORITY_TRACE, PMSGFILE)

#define FV_CLOSE_LOG_WITH_FILE( CONFIG_PATH, PMSGFILE )				\
			do														\
			{														\
				bool bWriteToLog =									\
				FvServerConfig::Get( #CONFIG_PATH "/shouldWriteToLog",\
				FvServerConfig::Get( "shouldWriteToLog",			\
				bool(true) ) );										\
				if(bWriteToLog)										\
				{													\
					FV_ASSERT(PMSGFILE);							\
					FvDebugFilter::Instance().DeleteMessageCallback(PMSGFILE);\
					delete PMSGFILE;								\
				}													\
			}while(0)

#endif // __FvServerConfig_H__
