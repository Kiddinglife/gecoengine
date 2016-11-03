#include "FvBaseApp.h"
#include "FvBaseAppManager.h"

#include <../FvBase/FvBaseAppIntInterface.h>


FvBaseApp::FvBaseApp( const FvNetAddress & intAddr,
			const FvNetAddress & extAddr,
			int id )
:FvNetChannelOwner( FvBaseAppMgr::Instance().Nub(), intAddr )
,m_kExternalAddr( extAddr )
,m_iId( id )
,m_fLoad( 0.f )
,m_iNumBases( 0 )
,m_iNumProxies( 0 )
,m_pkBackup( NULL )
{
	this->Channel().IsIrregular( true );
}

void FvBaseApp::AddEntity()
{
	m_fLoad =+ 0.01f;
}

FvWatcher * FvBaseApp::MakeWatcher()
{
	return NULL;
/**
	FvWatcher * pWatchCacheVal = new DirectoryWatcher();
	FvBaseApp * pNullBaseApp = NULL;

	pWatchCacheVal->addChild( "id", new DataWatcher<FvBaseAppID>(
		pNullBaseApp->m_iId, FvWatcher::WT_READ_ONLY ) );
	pWatchCacheVal->addChild( "internalChannel",
			FvNetChannelOwner::pWatcher(),
			(FvNetChannelOwner *)pNullBaseApp );
	pWatchCacheVal->addChild( "externalAddr", &FvNetAddress::watcher(),
		&pNullBaseApp->m_kExternalAddr );
	pWatchCacheVal->addChild( "load", new DataWatcher<float>(
		pNullBaseApp->m_fLoad, FvWatcher::WT_READ_ONLY ) );
	pWatchCacheVal->addChild( "numBases",
		new DataWatcher<int>( pNullBaseApp->m_iNumBases,
			FvWatcher::WT_READ_ONLY ) );
	pWatchCacheVal->addChild( "numProxies",
		new DataWatcher<int>( pNullBaseApp->m_iNumProxies,
			FvWatcher::WT_READ_ONLY ) );

	return pWatchCacheVal;
**/
}

bool FvBaseApp::HasTimedOut( FvUInt64 currTime, FvUInt64 timeoutPeriod,
	   FvUInt64 timeSinceAnyHeard ) const
{
	bool result = false;

	FvUInt64 diff = currTime - this->Channel().LastReceivedTime();
	result = (diff > timeoutPeriod);

	if (result)
	{
		FV_INFO_MSG( "BaseApp::hasTimedOut: Timed out - %.2f (> %.2f) %s\n",
				double( (FvInt64)diff )/StampsPerSecondD(),
				double( (FvInt64)timeoutPeriod )/StampsPerSecondD(),
				(char *)this->Addr() );

		if (!FvBaseAppMgr::Instance().ShutDownServerOnBadState())
		{
			if (timeSinceAnyHeard > timeoutPeriod/2)
			{
				FV_INFO_MSG( "BaseApp::hasTimedOut: "
					"Last inform time not recent enough %f\n",
					double((FvInt64)timeSinceAnyHeard)/StampsPerSecondD() );
				result = false;
			}
		}
	}

	return result;
}

FvBackupBaseApp::FvBackupBaseApp( const FvNetAddress & addr, int id ) :
	FvNetChannelOwner( FvBaseAppMgr::Instance().Nub(), addr ),
	m_iId( id ),
	m_fLoad( 0.f )
{
}


bool FvBackupBaseApp::Backup( FvBaseApp & cache )
{
	if (!m_kBackedUp.insert( &cache ).second)
	{
		FV_ERROR_MSG( "BackupBaseApp::backup: "
			"%s already backed up\n", (char *)cache.Addr() );
		return false;
	}

	if (cache.GetBackup() != NULL)
	{
		cache.GetBackup()->StopBackup( cache, true );
	}

	FV_ASSERT( cache.GetBackup() == NULL );

	cache.SetBackup( this );

	FvNetBundle & bundle = this->Bundle();
	bundle.StartMessage( BaseAppIntInterface::OldStartBaseAppBackup );
	bundle << cache.Addr();
	this->Send();

	return true;
}


bool FvBackupBaseApp::StopBackup( FvBaseApp & cache,
		bool tellBackupBaseApp )
{
	bool result = false;

	if (cache.GetBackup())
	{
		if (tellBackupBaseApp)
		{
			FvNetBundle & bundle = this->Bundle();
			bundle.StartMessage( BaseAppIntInterface::OldStopBaseAppBackup );
			bundle << cache.Addr();
			this->Send();
		}

		result = (m_kBackedUp.erase( &cache ) > 0);
		cache.SetBackup( NULL );
	}

	return result;
}

