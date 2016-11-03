#include "FvZoneManager.h"
#include "FvZoneSpace.h"

FV_DECLARE_DEBUG_COMPONENT2( "Zone", 0 )

FvZoneManager *FvZoneManager::ms_pkZoneManager = NULL;

FvZoneManager::FvZoneManager() :
	m_kCameraTrans( FvMatrix::IDENTITY ),
	m_spCameraSpace( NULL ),
	m_pkCameraZone( NULL )
{

}

FvZoneManager::~FvZoneManager()
{
}

void FvZoneManager::AddSpace( FvZoneSpace *pkSpace )
{
	FV_ASSERT( m_kSpaces.find( pkSpace->ID() ) == m_kSpaces.end() );
	m_kSpaces.insert( std::make_pair( pkSpace->ID(), pkSpace ) );
}

void FvZoneManager::DelSpace( FvZoneSpace *pkSpace )
{
	ZoneSpaces::iterator kFound = m_kSpaces.find( pkSpace->ID() );
	if (kFound != m_kSpaces.end())
		m_kSpaces.erase( kFound );
}

FvZoneManager *FvZoneManager::Create()
{
	if(ms_pkZoneManager == NULL)
		ms_pkZoneManager = new FvZoneManager;
	return ms_pkZoneManager;
}

void FvZoneManager::Destory()
{
	delete ms_pkZoneManager;
}

FvZoneManager &FvZoneManager::Instance()
{
	FV_ASSERT(ms_pkZoneManager);
	return *ms_pkZoneManager;
}

FvZoneSpacePtr FvZoneManager::Space( FvZoneSpaceID uiSpaceID, bool bCreateIfMissing )
{
	if (bCreateIfMissing)
	{
		FV_WARNING_MSG( "FvZoneManager::Space: "
			"createIfMissing must be false for the server\n" );
	}

	ZoneSpaces::iterator iter = m_kSpaces.find( uiSpaceID );
	return (iter != m_kSpaces.end()) ? iter->second : NULL;
}

FvZoneSpacePtr FvZoneManager::CameraSpace() const
{
	return NULL;
}

void FvZoneManager::Camera( const FvMatrix &, FvZoneSpacePtr, FvZone*  )
{
}
