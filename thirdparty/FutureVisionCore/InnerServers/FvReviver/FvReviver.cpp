#include "FvReviver.h"
#include "FvReviverInterface.h"

#include <FvProcess.h>
#include <FvIntrusiveObject.h>
#include <FvNetPortMap.h>
#include <FvNetWatcherGlue.h>
#include <FvServerResource.h>
#include <FvServerConfig.h>
#include <FvReviverCommon.h>

#include <../FvDBManager/FvDBInterface.h>
#include <../FvBaseAppManager/FvBaseAppManagerInterface.h>
#include <../FvLoginApp/FvLoginIntInterface.h>

#include <set>

FV_SINGLETON_STORAGE( FvReviver )

FV_DECLARE_DEBUG_COMPONENT2( "Reviver", 0 )

FvComponentRevivers *g_pkComponentRevivers;

class FvComponentReviver : public FvNetReplyMessageHandler,
	public FvNetTimerExpiryHandler,
	public FvNetInputMessageHandler,
	public FvIntrusiveObject< FvComponentReviver >
{
public:
	FvComponentReviver( const char * configName, const char * name,
			const char * interfaceName, const char * createParam ) :
		FvIntrusiveObject< FvComponentReviver >( g_pkComponentRevivers ),
		m_pkBirthMessage( NULL ),
		m_pkDeathMessage( NULL ),
		m_pkPingMessage( NULL ),
		m_pkNub( NULL ),
		m_kAddr( 0, 0 ),
		m_kConfigName( configName ),
		m_kName( name ),
		m_kInterfaceName( interfaceName ),
		m_pcCreateParam( createParam ),
		m_kPriority( 0 ),
		m_kTimerID( 0 ),
		m_iPingsToMiss( 0 ),
		m_iMaxPingsToMiss( 3 ),
		m_iPingPeriod( 0 ),
		m_bIsAttached( false ),
		m_bIsEnabled( true )
	{
	}

	bool Init( FvNetNub & nub )
	{
		bool isOkay = true;

		FV_ASSERT( m_pkNub == NULL );
		m_pkNub = &nub;

		FvString prefix = "Reviver/";

		float pingPeriodInSeconds =
			FvServerConfig::Get( (prefix + m_kConfigName + "/pingPeriod").c_str(), -1.f );

		if (pingPeriodInSeconds < 0.f)
			m_iPingPeriod = FvReviver::pInstance()->PingPeriod();
		else
			m_iPingPeriod = int(pingPeriodInSeconds * 1000000);

		m_iMaxPingsToMiss =
			FvServerConfig::Get( (prefix + m_kConfigName + "/timeoutInPings").c_str(),
								FvReviver::pInstance()->MaxPingsToMiss() );
		this->InitInterfaceElements();

		if (nub.FindInterface( m_kInterfaceName.c_str(), 0, m_kAddr, 4 ) !=
											FV_NET_REASON_SUCCESS)
		{
			FV_ERROR_MSG( "FvComponentReviver::Init: "
				"failed to find %s\n", m_kInterfaceName.c_str() );
			isOkay = false;
		}

		nub.RegisterBirthListener( *m_pkBirthMessage,
			const_cast<char *>( m_kInterfaceName.c_str() ) );
		nub.RegisterDeathListener( *m_pkDeathMessage,
			const_cast<char *>( m_kInterfaceName.c_str() ) );

		return isOkay;
	}

	void Revive()
	{
		bool wasAttached = m_bIsAttached;

		this->Deactivate();
		m_kAddr.m_uiIP = 0;
		m_kAddr.m_uiPort = 0;

		if (wasAttached)
		{
			FV_INFO_MSG( "Reviving %s\n", m_kName.c_str() );
			FvReviver::pInstance()->Revive( m_pcCreateParam );
		}
	}

	bool Activate( FvReviverPriority priority )
	{
		m_bIsAttached = false;

		if ((m_kTimerID == 0) && (m_kAddr.m_uiIP != 0))
		{
			m_iPingsToMiss = m_iMaxPingsToMiss;
			m_kTimerID = m_pkNub->RegisterTimer( m_iPingPeriod, this );
			m_kPriority = priority;
			return true;
		}

		return false;
	}

	bool Deactivate()
	{
		if (m_bIsAttached)
		{
			FvReviver::pInstance()->MarkAsDirty();
			FV_INFO_MSG( "FvComponentReviver: %s (%s) has detached\n",
				(char *)m_kAddr, m_kName.c_str() );
			m_bIsAttached = false;
		}

		if (m_kTimerID != 0)
		{
			m_pkNub->CancelTimer( m_kTimerID );
			m_kTimerID = 0;
			m_kPriority = 0;
			return true;
		}

		return false;
	}

	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
	{
		FV_ASSERT( (header.m_uiIdentifier == m_pkBirthMessage->GetID()) ||
					(header.m_uiIdentifier == m_pkDeathMessage->GetID()) );

		FvNetAddress addr;
		data >> addr;

		if (header.m_uiIdentifier == m_pkBirthMessage->GetID())
		{
			m_kAddr = addr;
			FV_INFO_MSG( "FvComponentReviver::HandleMessage: "
					"%s at %s has started.\n",
				m_kName.c_str(),
				(char *)addr );
			return;
		}

		FV_INFO_MSG( "FvComponentReviver::HandleMessage: %s at %s has died.\n",
			m_kName.c_str(),
			(char *)addr );

		if (addr == m_kAddr)
		{
			this->Revive();
		}
		else if (m_bIsAttached)
		{
			FvString currAddrStr = (char *)m_kAddr;
			FV_ERROR_MSG( "FvComponentReviver::HandleMessage: "
					"%s component died at %s. Expected %s\n",
				m_kName.c_str(),
				(char *)addr,
				currAddrStr.c_str() );
		}
	}

	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg )
	{
		FvUInt8 returnCode;
		data >> returnCode;
		if (returnCode == REVIVER_PING_YES)
		{
			m_iPingsToMiss = m_iMaxPingsToMiss;

			if (!m_bIsAttached)
			{
				FvReviver::pInstance()->MarkAsDirty();
				FV_INFO_MSG( "FvComponentReviver: %s (%s) has attached.\n",
					(char *)m_kAddr, m_kName.c_str() );
				m_bIsAttached = true;
			}
		}
		else
		{
			this->Deactivate();
		}
	}

	virtual int HandleTimeout( FvNetTimerID /*id*/, void * /*arg*/ )
	{
		if (m_iPingsToMiss > 0)
		{
			--m_iPingsToMiss;
			FvNetBundle bundle;
			bundle.StartRequest( *m_pkPingMessage, this );
			bundle << m_kPriority;
			m_pkNub->Send( m_kAddr, bundle );
		}
		else
		{
			FV_INFO_MSG( "FvComponentReviver::HandleTimeout: Missed too many\n" );
			this->Revive();
		}

		return 0;
	}

	virtual void HandleException( const FvNetNubException & ne,
		void * /*arg*/ )
	{
		if (m_bIsAttached)
		{
			FV_ERROR_MSG( "ReviverReplyHandler::HandleMessage: "
										"%s got an exception (%s).\n",
					m_kName.c_str(),
					NetReasonToString( ne.Reason() ) );
		}
	}

	FvReviverPriority Priority() const	{ return m_kPriority; }
	void Priority( FvReviverPriority p )	{ m_kPriority = p; }

	bool IsAttached() const				{ return m_bIsAttached; }
	const FvString &name() const	{ return m_kName; }
	const FvNetAddress &addr() const	{ return m_kAddr; }

	bool IsEnabled() const				{ return m_bIsEnabled; }
	void IsEnabled( bool v )			{ m_bIsEnabled = v; }

	const FvString &ConfigName() const		{ return m_kConfigName; }
	const char *CreateName() const				{ return m_pcCreateParam; }

	int MaxPingsToMiss() const					{ return m_iMaxPingsToMiss; }
	int PingPeriod() const						{ return m_iPingPeriod; }

protected:
	virtual void InitInterfaceElements() = 0;

	const FvNetInterfaceElement *m_pkBirthMessage;
	const FvNetInterfaceElement *m_pkDeathMessage;
	const FvNetInterfaceElement *m_pkPingMessage;

private:
	FvNetNub *m_pkNub;
	FvNetAddress m_kAddr;

	FvString m_kConfigName;
	FvString m_kName;
	FvString m_kInterfaceName;
	const char *m_pcCreateParam;

	FvReviverPriority m_kPriority;

	FvNetTimerID m_kTimerID;
	int m_iPingsToMiss;
	int m_iMaxPingsToMiss;
	int m_iPingPeriod;

	bool m_bIsAttached;

	bool m_bIsEnabled;
};


#define MF_REVIVER_HANDLER( CONFIG, COMPONENT, CREATE_WHAT )				\
	MF_REVIVER_HANDLER2( CONFIG, COMPONENT, COMPONENT, CREATE_WHAT )

#define MF_REVIVER_HANDLER2( CONFIG, COMPONENT, COMPONENT2, CREATE_WHAT )	\
/** @internal */															\
class COMPONENT##Reviver : public FvComponentReviver						\
{																			\
public:																		\
	COMPONENT##Reviver() :													\
		FvComponentReviver( #CONFIG, #COMPONENT, #COMPONENT2 "Interface",	\
				CREATE_WHAT )												\
	{}																		\
	virtual void InitInterfaceElements()									\
	{																		\
		m_pkBirthMessage = &ReviverInterface::Handle##COMPONENT##Birth;		\
		m_pkDeathMessage = &ReviverInterface::Handle##COMPONENT##Death;		\
		m_pkPingMessage = &COMPONENT2##Interface::ReviverPing;				\
	}																		\
} g_kReviverOf##COMPONENT;													\


//MF_REVIVER_HANDLER( cellAppMgr, CellAppMgr, "CellAppManager" )
MF_REVIVER_HANDLER( BaseAppManager, BaseAppMgr, "FvBaseAppManager" )
MF_REVIVER_HANDLER( DBManager,      DB,         "FvDBManager" )
MF_REVIVER_HANDLER2( LoginApp,  Login, LoginInt,   "FvLoginApp" )

FvReviver::FvReviver( FvNetNub & nub ) :
	m_kNub( nub ),
	m_iPingPeriod( 0 ),
	m_iMaxPingsToMiss( DEFAULT_REVIVER_TIMEOUT_IN_PINGS ),
	m_bShuttingDown( false ),
	m_bShutDownOnRevive( true ),
	m_bIsDirty( true )
{
}


FvReviver::~FvReviver()
{
}


bool FvReviver::Init( int argc, char * argv[] )
{
	FV_ASSERT( m_kComponents.empty() );

	float reattachPeriod = FvServerConfig::Get( "Reviver/reattachPeriod", 10.f );
	float pingPeriodInSeconds = FvServerConfig::Get( "Reviver/pingPeriod", 0.1f );
	m_iPingPeriod = int(pingPeriodInSeconds * 1000000);
	FvServerConfig::Update( "Reviver/shutDownOnRevive", m_bShutDownOnRevive );
	FvServerConfig::Update( "Reviver/timeoutInPings", m_iMaxPingsToMiss );

	FV_INFO_MSG( "\tNub address         = %s\n", m_kNub.c_str() );
	FV_INFO_MSG( "\tReattach Period     = %.1f seconds\n", reattachPeriod );
	FV_INFO_MSG( "\tDefault Ping Period = %.1f seconds\n", pingPeriodInSeconds );
	FV_INFO_MSG( "\tDefault Timeout     = %d pings\n", m_iMaxPingsToMiss );
	FV_INFO_MSG( "\tShut down on revive = %s\n",
									m_bShutDownOnRevive ? "True" : "False" );

	ReviverInterface::RegisterWithNub( m_kNub );

	if (ReviverInterface::RegisterWithMachined( m_kNub, 0 ) !=
		FV_NET_REASON_SUCCESS)
	{
		FV_WARNING_MSG( "FvReviver::Init: Unable to register with nub\n" );
		return false;
	}

	if (g_pkComponentRevivers == NULL)
	{
		FV_ERROR_MSG( "FvReviver::Init: No component revivers\n" );
		return false;
	}

	m_kComponents = *g_pkComponentRevivers;

	if (!this->QueryMachinedSettings())
	{
		return false;
	}

	FvComponentRevivers::iterator endIter = m_kComponents.end();

	bool isFirstAdd = true;
	bool isFirstDel = true;

	for (int i = 1; i < argc - 1; ++i)
	{
		const bool isAdd = (strcmp( argv[i], "--add" ) == 0);
		const bool isDel = (strcmp( argv[i], "--del" ) == 0);

		if (isAdd || isDel)
		{
			++i;

			if (isAdd && isFirstAdd)
			{
				isFirstAdd = false;
				FvComponentRevivers::iterator iter = m_kComponents.begin();

				while (iter != endIter)
				{
					(*iter)->IsEnabled( false );

					++iter;
				}
			}

			{
				bool found = false;
				FvComponentRevivers::iterator iter = m_kComponents.begin();

				while (iter != endIter)
				{
					if (((*iter)->ConfigName() == argv[i]) ||
							strcmp( (*iter)->CreateName(), argv[i] ) == 0)
					{
						found = true;
						(*iter)->IsEnabled( isAdd );
					}

					++iter;
				}

				if (!found)
				{
					FV_ERROR_MSG( "FvReviver::Init: Invalid command line. "
							"No such component %s\n", argv[i] );
					return false;
				}
			}
		}
	}

	if (!isFirstAdd && !isFirstDel)
	{
		FV_ERROR_MSG( "FvReviver::Init: "
					"Cannot mix --add and --del command line options\n" );
		return false;
	}

	{
		FvComponentRevivers::iterator iter = m_kComponents.begin();

		while (iter != endIter)
		{
			if ((*iter)->IsEnabled())
			{
				(*iter)->Init( m_kNub );
			}

			++iter;
		}
	}

	{
		FV_INFO_MSG( "Monitoring the following component types:\n" );
		FvComponentRevivers::iterator iter = m_kComponents.begin();

		while (iter != endIter)
		{
			if ((*iter)->IsEnabled())
			{
				FV_INFO_MSG( "\t%s\n", (*iter)->name().c_str() );
				FV_INFO_MSG( "\t\tPing Period = %.1f seconds\n",
						double((*iter)->PingPeriod())/1000000.f );
				FV_INFO_MSG( "\t\tTimeout     = %d pings\n",
						(*iter)->MaxPingsToMiss() );
			}

			++iter;
		}
	}

	{
		FvReviverPriority priority = 0;

		FvComponentRevivers::iterator iter = m_kComponents.begin();

		while (iter != endIter)
		{
			if ((*iter)->IsEnabled())
			{
				(*iter)->Activate( ++priority );
			}

			++iter;
		}
	}

	m_kNub.RegisterTimer( int(reattachPeriod * 1000000),
			this,
			(void *)TIMEOUT_REATTACH );

	return true;
}

bool FvReviver::TagsHandler::OnTagsMessage( FvNetTagsMessage &tm, FvUInt32 addr )
{
	if (tm.m_uiExists)
	{
		Tags &tags = tm.m_kTags;
		FvComponentRevivers::iterator iter = m_kReviver.m_kComponents.begin();
		FvComponentRevivers::iterator endIter = m_kReviver.m_kComponents.end();

		while (iter != endIter)
		{
			FvComponentReviver & component = **iter;

			if (std::find( tags.begin(), tags.end(), component.CreateName() )
				!= tags.end() ||
				std::find( tags.begin(), tags.end(), component.ConfigName() )
				!= tags.end())
			{
				component.IsEnabled( true );
			}
			else
			{
				FV_INFO_MSG( "\t%s disabled via fvmachined's Components tags\n",
					   component.name().c_str() );
				component.IsEnabled( false );
			}

			++iter;
		}
	}
	else
	{
		FV_ERROR_MSG( "FvReviver::Init: FvMachined has no Components tags\n" );
	}

	return false;
}

bool FvReviver::QueryMachinedSettings()
{
	FvNetTagsMessage query;
	query.m_kTags.push_back( FvString( "Components" ) );

	TagsHandler handler( *this );
	int reason;

	if ((reason = query.SendAndRecv( 0, FV_NET_LOCALHOST, &handler )) !=
		FV_NET_REASON_SUCCESS)
	{
		FV_ERROR_MSG( "FvReviver::QueryMachinedSettings: MGM query failed (%s)\n",
			NetReasonToString( (FvNetReason&)reason ) );
		return false;
	}

	return true;
}


int FvReviver::HandleTimeout( FvNetTimerID id, void * arg )
{
	typedef std::map< FvReviverPriority, FvComponentReviver * > Map;

	switch (FvUIntPtr(arg))
	{
		case TIMEOUT_REATTACH:
		{
			Map activeSet;
			FvComponentRevivers deactive;

			m_kComponents = *g_pkComponentRevivers;
			FvComponentRevivers::iterator iter = m_kComponents.begin();
			FvComponentRevivers::iterator endIter = m_kComponents.end();

			while (iter != endIter)
			{
				if ((*iter)->IsEnabled())
				{
					FvReviverPriority priority = (*iter)->Priority();

					if (priority > 0)
					{
						activeSet[ priority ] = (*iter);
					}
					else
					{
						deactive.push_back( *iter );
					}
				}

				++iter;
			}

			{
				Map::iterator mapIter = activeSet.begin();
				FvReviverPriority priority = 0;

				while (mapIter != activeSet.end())
				{
					++priority;
					if (mapIter->first != priority)
					{
						mapIter->second->Priority( priority );
					}

					++mapIter;
				}

				std::random_shuffle( deactive.begin(), deactive.end() );

				iter = deactive.begin();
				endIter = deactive.end();

				while (iter != endIter)
				{
					(*iter)->Activate( ++priority );
					++iter;
				}
			}

			if (m_bIsDirty)
			{
				FV_INFO_MSG( "---- Attached components summary ----\n" );

				if (!activeSet.empty())
				{

					Map::iterator mapIter = activeSet.begin();

					while (mapIter != activeSet.end())
					{
						FV_INFO_MSG( "%d: (%s) %s\n",
							mapIter->second->Priority(),
							(char *)mapIter->second->addr(),
							mapIter->second->name().c_str() );

						++mapIter;
					}
				}
				else
				{
					FV_INFO_MSG( "No attached components\n" );
				}

				m_bIsDirty = false;
			}
		}
		break;
	}

	return 0;
}


void FvReviver::Run()
{
	if (this->HasEnabledComponents())
	{
		m_kNub.ProcessUntilBreak();
	}
	else
	{
		FV_INFO_MSG( "FvReviver::Run:"
				"No components enabled to revive. Shutting down.\n" );
	}
}


void FvReviver::ShutDown()
{
	m_bShuttingDown = true;
	m_kNub.BreakProcessing();
}


void FvReviver::Revive( const char * createComponent )
{
	if (m_bShuttingDown)
	{
		FV_INFO_MSG( "FvReviver::Revive: "
			"Trying to revive a process while shutting down.\n" );
		return;
	}

	FvNetCreateMessage cm;
	cm.m_uiUID = FvGetUserID();
	cm.m_uiRecover = 1;
	cm.m_kName = createComponent;
#if defined FV_DEBUG
#if defined FV_AS_STATIC_LIBS
	cm.m_kConfig = "DebugLib";
#else
	cm.m_kConfig = "DebugDll";
#endif // FV_AS_STATIC_LIBS
#elif defined FV_RELEASE
#if defined FV_AS_STATIC_LIBS
	cm.m_kConfig = "ReleaseLib";
#else
	cm.m_kConfig = "ReleaseDll";
#endif // FV_AS_STATIC_LIBS
#else 
	cm.m_kConfig = "";
#endif

	FvUInt32 srcaddr = 0, destaddr = htonl( 0x7f000001U );
	if (cm.SendAndRecv( srcaddr, destaddr ) != FV_NET_REASON_SUCCESS)
	{
		FV_ERROR_MSG( "FvComponentReviver::Revive: Could not send request.\n" );
	}

	if (m_bShutDownOnRevive)
	{
		m_bShuttingDown = true;
		this->ShutDown();
	}
}


bool FvReviver::HasEnabledComponents() const
{
	FvComponentRevivers::const_iterator iter = m_kComponents.begin();
	FvComponentRevivers::const_iterator endIter = m_kComponents.end();

	while (iter != endIter)
	{
		if ((*iter)->IsEnabled())
		{
			return true;
		}
		iter++;
	}

	return false;
}

#define DEFINE_INTERFACE_HERE
#include <../FvDBManager/FvDBInterface.h>

#define DEFINE_INTERFACE_HERE
#include <../FvBaseAppManager/FvBaseAppManagerInterface.h>

#define DEFINE_INTERFACE_HERE
#include <../FvLoginApp/FvLoginIntInterface.h>

#define DEFINE_SERVER_HERE
#include "FvReviverInterface.h"
