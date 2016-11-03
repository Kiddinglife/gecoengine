#include "FvRobotSpaceDataManager.h"

FV_SINGLETON_STORAGE( FvRobotSpaceDataManager )

FvRobotSpaceDataManager::FvRobotSpaceDataManager()
{}

FvRobotSpaceDataManager::~FvRobotSpaceDataManager()
{
	SpaceDataMap::iterator itrB = m_kSpaceData.begin();
	SpaceDataMap::iterator itrE = m_kSpaceData.end();
	while(itrB != itrE)
	{
		delete itrB->second;
		++itrB;
	}
	m_kSpaceData.clear();
}

FvRobotSpaceData::FvRobotSpaceData( const FvSpaceID id ) :
	m_iID( id )
{}

FvRobotSpaceData::~FvRobotSpaceData()
{
	m_kDataEntries.clear();
}

FvRobotSpaceData * FvRobotSpaceDataManager::FindOrAddSpaceData( const FvSpaceID spaceID )
{
	FvRobotSpaceData * spaceData = NULL;

	if (spaceID < 0)
	{	
		FV_ERROR_MSG( "Invalid space id %d\n", spaceID );
		return NULL;
	}

	SpaceDataMap::iterator sdemiter = m_kSpaceData.find( spaceID );

	if (sdemiter == m_kSpaceData.end())
	{
		FV_INFO_MSG( "Create a new space data for space %d\n", spaceID );

		spaceData = new FvRobotSpaceData( spaceID );
		m_kSpaceData.insert( std::make_pair( spaceID, spaceData ));
	}
	else
	{
		spaceData = sdemiter->second;
	}
	return spaceData;
}

FvRobotSpaceData::EntryStatus FvRobotSpaceData::DataEntry( const FvSpaceEntryID & entryID, FvUInt16 key,
							const FvString & data )
{
	if (key != FvUInt16(-1))
	{
		DataEntryMapKey demk;
		demk.m_kEntryID = entryID;
		demk.m_uiKey = key;
		DataEntryMap::const_iterator iter = m_kDataEntries.find( demk );
		if (iter == m_kDataEntries.end())
		{
			m_kDataEntries.insert( std::make_pair( demk, data ) );
			return DATA_ADDED;
		}
		else
		{
			if (iter->second == data)
				return DATA_UNCHANGED;
			else
				return DATA_MODIFIED;
		}
	}
	else
	{
		DataEntryMap::iterator it;
		for (it = m_kDataEntries.begin(); it != m_kDataEntries.end(); it++)
		{
			if (it->first.m_kEntryID == entryID) break;
		}
		if (it == m_kDataEntries.end())
			return DATA_UNKNOWN;

		key = it->first.m_uiKey;
		m_kDataEntries.erase( it );
		return DATA_DELETED;
	}
}

FvRobotSpaceData::DataEntryMap::const_iterator FvRobotSpaceData::DataRetrieve( FvUInt16 key )
{
	DataEntryMapKey demk;
	demk.m_kEntryID.m_uiIP = 0;
	demk.m_kEntryID.m_uiPort = 0;
	demk.m_kEntryID.m_uiSalt = 0;
	demk.m_uiKey = key;

	DataEntryMap::const_iterator found = m_kDataEntries.lower_bound( demk );
	if (found != m_kDataEntries.end() && found->first.m_uiKey == key)
		return found;
	else
		return m_kDataEntries.end();
}

const FvString * FvRobotSpaceData::DataRetrieveFirst( FvUInt16 key )
{
	DataEntryMap::const_iterator found = this->DataRetrieve( key );

	return (found != m_kDataEntries.end()) ? &found->second : NULL;
}
