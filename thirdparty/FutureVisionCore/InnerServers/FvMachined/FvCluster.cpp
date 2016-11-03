#include <algorithm>
#include "FvCluster.h"
#include "FvMachined.h"
#include <FvMemoryStream.h>
#include <FvNetPortMap.h>
#include <FvSysLog.h>

FvCluster::FvCluster( FvMachined &machined ) :
	m_kMachined( machined ),
	m_uiBuddyAddr( 0 ),
#pragma warning (push)
#pragma warning (disable: 4355)
	m_kFloodTriggerHandler( *this ),
	m_pkFloodReplyHandler( NULL ),
	m_kBirthHandler( *this )
#pragma warning (pop)
{

}

FvCluster::~FvCluster()
{
	if(m_pkFloodReplyHandler)
		delete m_pkFloodReplyHandler;
}

void FvCluster::ChooseBuddy()
{
	FvUInt32 oldBuddy = m_uiBuddyAddr;
	m_uiBuddyAddr = 0;

	FvUInt32 lowest = 0xFFFFFFFF;
	for (Addresses::iterator it = m_kMachines.begin(); it != m_kMachines.end(); ++it)
	{
		if (*it > m_uiOwnAddr && (m_uiBuddyAddr == 0 || *it < m_uiBuddyAddr))
			m_uiBuddyAddr = *it;
		if (*it < lowest && *it != m_uiOwnAddr)
			lowest = *it;
	}

	if (m_uiBuddyAddr == 0 && m_kMachines.size() > 1)
		m_uiBuddyAddr = lowest;

	FvNetMGMPacket::SetBuddy( m_uiBuddyAddr );

	if (m_uiBuddyAddr != oldBuddy)
	{
		if (m_uiBuddyAddr)
			FvSysLog( FV_SYSLOG_INFO, "Buddy is %s", inet_ntoa( (in_addr&)m_uiBuddyAddr ) );
		else
			FvSysLog( FV_SYSLOG_INFO, "I have no buddy" );
	}
}

void FvCluster::ClusterTimeoutHandler::AddTimer()
{
	m_uiID = m_kCluster.m_kMachined.Callbacks().Add(
		m_kCluster.m_kMachined.TimeStamp() + this->Delay(),
		this->Interval(), this, NULL );
}

void FvCluster::ClusterTimeoutHandler::Cancel()
{
	if (!m_uiID)
	{
		FvSysLog( FV_SYSLOG_ALERT, "Tried to cancel() a CTH before doing addTimer()" );
		return;
	}

	m_kCluster.m_kMachined.Callbacks().Cancel( m_uiID );
	m_uiID = 0;
}

void FvCluster::FloodTriggerHandler::HandleTimeout( FvTimeQueueID id, void *pUser )
{
	FloodReplyHandler *&rpHandler = m_kCluster.m_pkFloodReplyHandler;

	if (rpHandler != NULL)
	{
		if (rpHandler->id())
		{
			FvSysLog( FV_SYSLOG_WARNING,
				"Old FloodReplyHandler seems to be taking a long time" );
			return;
		}
		else
		{
			delete rpHandler;
			rpHandler = NULL;
		}
	}

	rpHandler = new FloodReplyHandler( m_kCluster );

	this->AddTimer();
}

FvTimeQueue::TimeStamp FvCluster::FloodTriggerHandler::Delay() const
{
	FvTimeQueue::TimeStamp min = AVERAGE_INTERVAL;
	FvTimeQueue::TimeStamp max =
		std::max( min+1, FvTimeQueue::TimeStamp(AVERAGE_INTERVAL * m_kCluster.m_kMachines.size() * 2) );

	FvTimeQueue::TimeStamp interval = min + rand() % (max-min);
	return interval;
}

FvCluster::FloodReplyHandler::FloodReplyHandler( FvCluster &cluster ) :
	ClusterTimeoutHandler( cluster ), m_iTries( MAX_RETRIES )
{
	this->SendBroadcast();
	this->AddTimer();
}

void FvCluster::FloodReplyHandler::HandleTimeout( FvTimeQueueID id, void *pUser )
{
	bool births = false;
	for (Addresses::iterator it = m_kReplied.begin();
		 it != m_kReplied.end(); ++it)
	{
		if (m_kCluster.m_kMachines.find( *it ) == m_kCluster.m_kMachines.end())
		{
			births = true;
			m_kCluster.m_kMachines.insert( *it );
			FvSysLog( FV_SYSLOG_INFO, "Discovered new machine %s",
				inet_ntoa( (in_addr&)*it ) );
		}
	}

	if (births)
		m_kCluster.ChooseBuddy();

	std::vector< FvUInt32 > deaths;
	for (Addresses::iterator it = m_kCluster.m_kMachines.begin();
		 it != m_kCluster.m_kMachines.end(); ++it)
	{
		if (m_kReplied.find( *it ) == m_kReplied.end())
			deaths.push_back( *it );
	}

	m_iTries--;

	if (!births && deaths.empty())
	{
		this->Cancel();
	}

	else if (deaths.size() && m_iTries > 0)
	{
		this->SendBroadcast();
	}

	else
	{
		FvNetMGMPacket packet;

		if (deaths.size())
		{
			FvSysLog( FV_SYSLOG_INFO, "Machines have died or become unreachable" );
			for (std::vector< FvUInt32 >::iterator it = deaths.begin();
				 it != deaths.end(); ++it)
			{
				FvNetMachinedAnnounceMessage *pMam = new FvNetMachinedAnnounceMessage();
				pMam->m_uiAddr = *it;
				pMam->m_uiType = pMam->ANNOUNCE_DEATH;
				packet.Append( *pMam, true );
			}
		}

		if (births)
		{
			for (Addresses::iterator it = m_kCluster.m_kMachines.begin();
				 it != m_kCluster.m_kMachines.end(); ++it)
			{
				FvNetMachinedAnnounceMessage *pMam = new FvNetMachinedAnnounceMessage();
				pMam->m_uiAddr = *it;
				pMam->m_uiType = pMam->ANNOUNCE_EXISTS;
				packet.Append( *pMam, true );
			}
		}

		FvMemoryOStream os;
		packet.Write( os );

		m_kCluster.m_kMachined.EP().SendTo(
			os.Data(), os.Size(), htons( m_kCluster.m_kMachined.GetMachinePort() ) );

		this->Cancel();
	}
}

void FvCluster::FloodReplyHandler::SendBroadcast()
{
	if (!m_kWMM.SendTo( m_kCluster.m_kMachined.EP(), htons( m_kCluster.m_kMachined.GetMachinePort() ),
			FV_NET_BROADCAST, FvNetMGMPacket::PACKET_STAGGER_REPLIES ))
	{
		FvSysLog( FV_SYSLOG_ERR, "Couldn't send broadcast keepalive poll!" );
	}
}

FvTimeQueue::TimeStamp FvCluster::FloodReplyHandler::Delay() const
{
	return FvCluster::FloodTriggerHandler::AVERAGE_INTERVAL / MAX_RETRIES;
}

void FvCluster::BirthReplyHandler::AddTimer()
{
	FvNetMachinedAnnounceMessage mam;
	mam.m_uiType = mam.ANNOUNCE_BIRTH;
	mam.m_uiCount = 0;
	mam.SendTo( m_kCluster.m_kMachined.EP(), htons( m_kCluster.m_kMachined.GetMachinePort() ),
		FV_NET_BROADCAST, FvNetMGMPacket::PACKET_STAGGER_REPLIES );

	m_uiToldSize = 1;
	m_kCluster.m_kMachines.clear();
	m_kCluster.m_uiBuddyAddr = 0;
	ClusterTimeoutHandler::AddTimer();
}

void FvCluster::BirthReplyHandler::HandleTimeout( FvTimeQueueID id, void *pUser )
{
	if (m_kCluster.m_kMachines.size() == m_uiToldSize)
		this->Cancel();
	else
		this->AddTimer();
}

void FvCluster::BirthReplyHandler::MarkReceived( FvUInt32 addr, FvUInt32 count )
{
	m_kCluster.m_kMachines.insert( addr );

	m_uiToldSize = std::max( m_uiToldSize, count );

	if (m_kCluster.m_kMachines.size() == m_uiToldSize)
	{
		FvSysLog( FV_SYSLOG_INFO,
			"Bootstrap complete; %d machines on network",
			m_kCluster.m_kMachines.size() );
		m_kCluster.ChooseBuddy();
	}
}

FvTimeQueue::TimeStamp FvCluster::BirthReplyHandler::Delay() const
{
	return 2 * FvMachined::STAGGER_REPLY_PERIOD;
}
