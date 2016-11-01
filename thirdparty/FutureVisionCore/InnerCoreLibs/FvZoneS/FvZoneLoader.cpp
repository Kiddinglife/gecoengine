#include "FvZoneLoader.h"
#include "FvZoneManager.h"
#include "FvZoneSpace.h"
#include "FvZone.h"

#include <FvDebug.h>
#include <FvBGTaskManager.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 );

class FvLoadZoneTask : public FvBackgroundTask
{
public:
	FvLoadZoneTask( FvZone *pkZone ) : m_pkZone( pkZone ) {}

	virtual void DoBackgroundTask( FvBGTaskManager &kBGMgr )
	{
		FvLoadZoneTask::Load( m_pkZone );
	}

	static void Load( FvZone *pkZone )
	{
		// FV_GUARD;
		pkZone->Load();

		FV_TRACE_MSG( "FvZoneLoader: Loaded zone '%s'\n",
			pkZone->ResourceID().c_str() );
	}

private:
	FvZone *m_pkZone;
};

class FvFindSeedTask : public FvBackgroundTask
{
public:
	FvFindSeedTask( FvZoneSpace *pkSpace, const FvVector3 &kWhere,
			FvZone *&rpkZone ) :
		m_spSpace( pkSpace ),
		m_kWhere( kWhere ),
		m_rpkZone( rpkZone ),
		m_pkFoundZone( NULL )
	{}

	virtual void DoBackgroundTask( FvBGTaskManager &kBGMgr )
	{
		m_pkFoundZone = m_spSpace->GuessZone( m_kWhere );
		kBGMgr.AddMainThreadTask( this );
	}

	virtual void DoMainThreadTask( FvBGTaskManager &kBGMgr )
	{
		m_rpkZone = m_pkFoundZone;
	}

private:
	FvZoneSpacePtr m_spSpace;
	FvVector3 m_kWhere;
	FvZone *&m_rpkZone;
	FvZone *m_pkFoundZone;
};

void FvZoneLoader::Load( FvZone *pkZone, int iPriority )
{
	// FV_GUARD;
	FvBGTaskManager::Instance().AddBackgroundTask(
		new FvLoadZoneTask( pkZone ),
		iPriority );
}

void FvZoneLoader::LoadNow( FvZone *pkZone )
{
	// FV_GUARD;
	FvLoadZoneTask::Load( pkZone );
}

void FvZoneLoader::FindSeed( FvZoneSpace *pkSpace, const FvVector3 &kWhere,
	FvZone *&rpkZone )
{
	// FV_GUARD;
	FvBGTaskManager::Instance().AddBackgroundTask(
		new FvFindSeedTask( pkSpace, kWhere, rpkZone ), 15 );
}
