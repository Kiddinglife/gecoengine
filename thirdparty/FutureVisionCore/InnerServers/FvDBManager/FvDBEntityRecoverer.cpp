#include "FvDBEntityRecoverer.h"
#include "FvDatabase.h"

FV_DECLARE_DEBUG_COMPONENT( 0 )

class RecoveringEntityHandler : public FvNetReplyMessageHandler,
								public FvDatabase::GetEntityHandler,
                                public FvIDatabase::IPutEntityHandler
{
	enum State
	{
		StateInit,
		StateWaitingForSetBaseToLoggingOn,
		StateWaitingForCreateBase,
		StateWaitingForSetBaseToFinal
	};

	State m_eState;
	FvEntityDBKey m_kEntityDBKey;
	FvEntityDBRecordOut	m_kOutRec;
	FvNetBundle m_kCreateBaseBundle;
	FvDBEntityRecoverer &m_kRecoverer;
	bool m_bIsOK;

public:
	RecoveringEntityHandler( FvEntityTypeID entityTypeID, FvDatabaseID dbID,
		FvDBEntityRecoverer& mgr );
	virtual ~RecoveringEntityHandler()
	{
		m_kRecoverer.OnRecoverEntityComplete( m_bIsOK );
	}

	void recover();

	virtual void HandleMessage( const FvNetAddress & srcAddr,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data, void * );
	virtual void HandleException( const FvNetNubException & ne, void * );

	virtual FvEntityDBKey& GetKey()					{	return m_kEntityDBKey;	}
	virtual FvEntityDBRecordOut& OutRec()			{	return m_kOutRec;	}
	virtual void OnGetEntityCompleted( bool isOK );

	virtual void OnPutEntityComplete( bool isOK, FvDatabaseID dbID );

private:

};

RecoveringEntityHandler::RecoveringEntityHandler( FvEntityTypeID typeID,
	FvDatabaseID dbID, FvDBEntityRecoverer& mgr )
	: m_eState(StateInit), m_kEntityDBKey( typeID, dbID ), m_kOutRec(),
	m_kCreateBaseBundle(), m_kRecoverer(mgr), m_bIsOK( true )
{}

void RecoveringEntityHandler::recover()
{
	//FvDatabase::prepareCreateEntityBundle( m_kEntityDBKey.m_uiTypeID, m_kEntityDBKey.m_iDBID,
	//	FvNetAddress( 0, 0 ), this, m_kCreateBaseBundle );

	//m_kOutRec.provideStrm( m_kCreateBaseBundle );
	//FvDatabase::instance().getEntity( *this );
}

void RecoveringEntityHandler::HandleMessage( const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data, void * )
{
	FvNetAddress proxyAddr;
	data >> proxyAddr;
	FvEntityMailBoxRef baseRef;
	data >> baseRef;
	data.Finish();

	m_eState = StateWaitingForSetBaseToFinal;
	FvEntityMailBoxRef*	pBaseRef = &baseRef;
	FvEntityDBRecordIn	erec;
	erec.ProvideBaseMB( pBaseRef );
	FvDatabase::Instance().PutEntity( m_kEntityDBKey, erec, *this );
}

void RecoveringEntityHandler::HandleException( const FvNetNubException & ne, void * )
{
	m_bIsOK = false;
	delete this;
}

void RecoveringEntityHandler::OnGetEntityCompleted( bool isOK )
{
	if (isOK)
	{
		m_eState = StateWaitingForSetBaseToLoggingOn;
		FvEntityMailBoxRef	baseRef;
		FvDatabase::SetBaseRefToLoggingOn( baseRef, m_kEntityDBKey.m_uiTypeID );
		FvEntityMailBoxRef*	pBaseRef = &baseRef;
		FvEntityDBRecordIn erec;
		erec.ProvideBaseMB( pBaseRef );
		FvDatabase::Instance().GetIDatabase().PutEntity( m_kEntityDBKey, erec, *this );
	}
	else
	{
		FV_ERROR_MSG( "RecoveringEntityHandler::OnGetEntityCompleted: "
				"Failed to load entity %"FMT_DBID" of type %d\n",
				m_kEntityDBKey.m_iDBID, m_kEntityDBKey.m_uiTypeID );
		m_bIsOK = false;
		delete this;
	}
}

void RecoveringEntityHandler::OnPutEntityComplete( bool isOK, FvDatabaseID dbID )
{
	FV_ASSERT(isOK);

	if (m_eState == StateWaitingForSetBaseToLoggingOn)
	{
		m_eState = StateWaitingForCreateBase;
		FvDatabase::Instance().GetBaseAppManager().Channel().Send( &m_kCreateBaseBundle );
	}
	else
	{
		FV_ASSERT(m_eState == StateWaitingForSetBaseToFinal);
		delete this;
	}
}



FvDBEntityRecoverer::FvDBEntityRecoverer() :
	m_iNumOutstanding( 0 ),
	m_iNumSent( 0 ),
	m_bHasErrors( false )
{}

void FvDBEntityRecoverer::Reserve( int numEntities )
{
	m_kEntities.reserve( numEntities );
}


void FvDBEntityRecoverer::Start()
{
	const int maxOutstanding = 5;

	while ((m_iNumOutstanding < maxOutstanding) && this->SendNext())
	{
	}
}

void FvDBEntityRecoverer::Abort()
{
	m_kEntities.clear();
	FvDatabase::Instance().StartServerError();
	delete this;
}

void FvDBEntityRecoverer::AddEntity( FvEntityTypeID entityTypeID, FvDatabaseID dbID )
{
	m_kEntities.push_back( std::make_pair( entityTypeID, dbID ) );
}

bool FvDBEntityRecoverer::SendNext()
{
	bool done = this->AllSent();
	if (!done)
	{
		RecoveringEntityHandler* pEntityRecoverer =
			new RecoveringEntityHandler( m_kEntities[m_iNumSent].first,
				m_kEntities[m_iNumSent].second, *this );
		pEntityRecoverer->recover();

		++m_iNumSent;

		++m_iNumOutstanding;
	}

	this->CheckFinished();

	return !done;
}

void FvDBEntityRecoverer::OnRecoverEntityComplete( bool isOK )
{
	--m_iNumOutstanding;
	if (isOK)
	{
		if (!m_bHasErrors)
		{
			this->SendNext();
		}
	}
	else
	{
		m_bHasErrors = true;
		this->CheckFinished();
	}
}

void FvDBEntityRecoverer::CheckFinished()
{
	if (m_iNumOutstanding == 0)
	{
		if (m_bHasErrors)
		{
			FvDatabase::Instance().StartServerError();
		}
		else if (this->AllSent())
		{
			FvDatabase::Instance().StartServerEnd();
		}
		delete this;
	}
}