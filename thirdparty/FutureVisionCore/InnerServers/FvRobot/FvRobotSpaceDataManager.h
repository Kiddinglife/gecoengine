//{future header message}
#ifndef __FvRobotSpaceDataManager_H__
#define __FvRobotSpaceDataManager_H__

#include "FvRobotDefines.h"
#include <FvSingleton.h>
#include <FvNetTypes.h>
#include <FvSpaceDataTypes.h>
#include <map>

class FV_ROBOT_API FvRobotSpaceData
{
public:
	FvRobotSpaceData( const FvSpaceID id );
	~FvRobotSpaceData();

	struct DataEntryMapKey
	{
		FvSpaceEntryID m_kEntryID;
		FvUInt16 m_uiKey;
		bool operator<( const DataEntryMapKey & other ) const
		{ return m_uiKey < other.m_uiKey ||
		(m_uiKey == other.m_uiKey && m_kEntryID < other.m_kEntryID); }
	};
	typedef std::map<DataEntryMapKey, FvString> DataEntryMap;
	typedef std::pair<FvUInt16, const FvString * > DataValueReturn;

	enum EntryStatus
	{
		DATA_ADDED,
		DATA_MODIFIED,
		DATA_DELETED,
		DATA_UNCHANGED,
		DATA_UNKNOWN
	};

	FvRobotSpaceData::EntryStatus DataEntry( const FvSpaceEntryID & entryID,
		FvUInt16 key, const FvString & data );
	DataValueReturn DataRetrieveSpecific( const FvSpaceEntryID & entryID,
		FvUInt16 key = FvUInt16(-1) );

	const FvString *DataRetrieveFirst( FvUInt16 key );
	DataEntryMap::const_iterator DataRetrieve( FvUInt16 key );

private:
	DataEntryMap m_kDataEntries;
	FvSpaceID m_iID;
};

class FV_ROBOT_API FvRobotSpaceDataManager : public FvSingleton< FvRobotSpaceDataManager >
{
public:
	FvRobotSpaceDataManager();
	~FvRobotSpaceDataManager();

	typedef std::map<FvSpaceID, FvRobotSpaceData *> SpaceDataMap;

	FvRobotSpaceData *FindOrAddSpaceData( const FvSpaceID spaceID );

private:
	SpaceDataMap m_kSpaceData;
};

#endif//! __FvRobotSpaceDataManager_H__
