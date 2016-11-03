#include "FvMachined.h"
#include "FvCommonMachineGuard.h"
#include <FvMemoryStream.h>
#include <FvNetPortMap.h>
#include <FvNetFileStream.h>
#include <FvSysLog.h>
#include <FvProcess.h>
#include <time.h>
//#include <sys/types.h>
//#include <sys/wait.h>
//#include <signal.h>

const char* FvMachined::STATE_FILE = "/var/run/machined.state";

FV_SINGLETON_STORAGE( FvMachined )


static void SigTerm( int sig )
{
	if (FvMachined::pInstance())
		FvMachined::pInstance()->Save();

	exit( 0 );
}


FvMachined::FvMachined() :
	m_kPacketTimeoutHandler(),
	m_uiBroadcastAddr( 0 ),
	m_kEP(),
	m_kEPBroadcast(),
	m_kEPLocal(),
#pragma warning (push)
#pragma warning (disable: 4355)
	m_kCluster( *this ),
	m_kTags(),
	m_kTimingMethod( "rdtsc" ),
	m_kSystemInfo(),
	m_kProcs(),
	m_kBirthListeners( *this ),
	m_kDeathListeners( *this ),
#pragma warning (pop)
	m_kUsers(),
	m_kCallbacks(),
	m_bBreak(false),
	m_uiMachinePort(FV_NET_PORT_MACHINED),
	m_uiMachineDiscoveryPort(FV_NET_PORT_BROADCAST_DISCOVERY)
{

	m_kEP.Socket( SOCK_DGRAM );
	m_kEPLocal.Socket( SOCK_DGRAM );
	m_kEPBroadcast.Socket( SOCK_DGRAM );

	if (!this->ReadConfigFile())
	{
		FvSysLog( FV_SYSLOG_CRIT, "Invalid configuration file" );
		exit( 1 );
	}

	if (m_uiBroadcastAddr == 0 && !this->FindBroadcastInterface())
	{
		FvSysLog( FV_SYSLOG_CRIT, "Failed to determine default broadcast interface" );
		exit( 1 );
	}

	if (!m_kEP.Good() ||
		 m_kEP.Bind( htons( m_uiMachinePort ), m_uiBroadcastAddr ) == -1)
	{
		FvSysLog( FV_SYSLOG_CRIT, "Failed to bind socket to '%s'. %s.",
							inet_ntoa((struct in_addr &)m_uiBroadcastAddr),
							strerror(errno) );
		exit( 1 );
	}

	m_kEP.SetBroadcast( true );

	if (!m_kEPLocal.Good() ||
		 m_kEPLocal.Bind( htons( m_uiMachinePort ), FV_NET_LOCALHOST ) == -1)
	{
		FvSysLog( FV_SYSLOG_CRIT, "Failed to bind socket to (lo). %s.",
							strerror(errno) );
		exit( 1 );
	}

#ifndef _WIN32
	if (!m_kEPBroadcast.Good() ||
		 m_kEPBroadcast.Bind( htons( m_uiMachinePort ), FV_NET_BROADCAST ) == -1)
	{
		FvSysLog( FV_SYSLOG_CRIT, "Failed to bind socket to '%s'. %s.",
							inet_ntoa((struct in_addr &)FV_NET_BROADCAST),
							strerror(errno) );
		exit( 1 );
	}
#endif // !_WIN32

	m_kCluster.m_uiOwnAddr = m_uiBroadcastAddr;

	std::vector<float> speeds;
	GetProcessorSpeeds( speeds );
	if (speeds.size() == 0)
	{
		FvSysLog( FV_SYSLOG_CRIT, "Unable to obtain any valid processor speed info "
						"from /proc/cpuinfo" );
		exit( 1 );
	}

	char bigname[256];
	gethostname( bigname, sizeof( bigname ) );
	char *walk = strchr( bigname, '.' );
	if (walk != NULL) *walk = '\0';

	FvSystemInfo &si = m_kSystemInfo;

	si.m_uiNumCPUs = speeds.size();
	si.m_uiCPUSpeed = (int)speeds[0];
	for (unsigned int j=0; j < si.m_uiNumCPUs; j++)
		si.m_kCPU.push_back( FvMaxStat() );

	si.m_kWholeMessage.m_uiCPUSpeed = si.m_uiCPUSpeed;
	si.m_kWholeMessage.SetNCpus( si.m_uiNumCPUs );
	si.m_kWholeMessage.m_kHostname = bigname;
	si.m_kWholeMessage.m_uiVersion = FV_MACHINED_VERSION;
	si.m_kWholeMessage.OutGoing( true );

	si.m_kHPMessage.m_uiCPUSpeed = si.m_uiCPUSpeed;
	si.m_kHPMessage.SetNCpus( si.m_uiNumCPUs );
	si.m_kHPMessage.m_kHostname = bigname;
	si.m_kHPMessage.m_uiVersion = FV_MACHINED_VERSION;
	si.m_kHPMessage.OutGoing( true );

	this->UpdateSystemInfo();

	this->Load();

	signal( SIGTERM, SigTerm );
}

FvMachined::~FvMachined()
{

}

bool FvMachined::FindBroadcastInterface()
{

	std::map< u_int32_t, FvString > interfaces;
	FvNetEndpoint epListen;
	struct timeval tv;
	fd_set fds;
	char streamBuf[ FvNetMGMPacket::MAX_SIZE ];

	epListen.Socket( SOCK_DGRAM );
	if (!epListen.Good() ||
		 epListen.Bind( htons( m_uiMachineDiscoveryPort ) ) == -1)
	{
		FvSysLog( FV_SYSLOG_CRIT, "Couldn't bind broadcast listener socket to port %d",
				m_uiMachineDiscoveryPort );
		return false;
	}
	epListen.SetBroadcast( true );

	if (!epListen.GetInterfaces( interfaces ))
	{
		FvSysLog( FV_SYSLOG_CRIT, "Failed to discover network interfaces" );
		return false;
	}

	FvNetQueryInterfaceMessage qim;
	bool ok = qim.SendTo( epListen, htons( m_uiMachineDiscoveryPort ),
						  FV_NET_BROADCAST, FvNetMGMPacket::PACKET_STAGGER_REPLIES );
	if (!ok)
	{
		FvSysLog( FV_SYSLOG_CRIT, "Failed to send broadcast discovery message." );
		return false;
	}

	tv.tv_sec = 1;
	tv.tv_usec = 0;

	while (1)
	{
		FD_ZERO( &fds );
		FD_SET( (int)epListen, &fds );
		int selgot = select( epListen+1, &fds, NULL, NULL, &tv );
		if (selgot == 0)
		{
			FvSysLog( FV_SYSLOG_CRIT,
				"Timed out before receiving any broadcast discovery responses");
			return false;
		}
		else if (selgot == -1)
		{
			FvSysLog( FV_SYSLOG_CRIT, "Broadcast discovery select error. %s.",
					strerror(errno) );
			return false;
		}
		else
		{
			sockaddr_in	sin;

			int len = epListen.RecvFrom( &streamBuf, sizeof( streamBuf ), sin );
			if (len == -1)
			{
				FvSysLog( FV_SYSLOG_ERR, "Broadcast discovery recvfrom error. %s.",
						strerror( errno ) );
				continue;
			}

			FvSysLog( FV_SYSLOG_INFO, "Broadcast discovery receipt from %s.",
					inet_ntoa((struct in_addr&)sin.sin_addr.s_addr) );

			std::map< u_int32_t, FvString >::iterator iter;

			iter = interfaces.find( (u_int32_t &)sin.sin_addr.s_addr );
			if (iter != interfaces.end())
			{
				FvSysLog( FV_SYSLOG_INFO,
					"Confirmed %s (%s) as default broadcast route interface.",
					inet_ntoa((struct in_addr&)sin.sin_addr.s_addr),
					iter->second.c_str() );
				m_uiBroadcastAddr = sin.sin_addr.s_addr;
				break;
			}

			FvSysLog( FV_SYSLOG_ERR,
					"Broadcast discovery %s not a valid interface.",
					inet_ntoa((struct in_addr&)sin.sin_addr.s_addr) );
		}
	}

	return true;
}


void FvMachined::Save()
{
	FvNetFileStream out( STATE_FILE, "w" );

	if (out.Error())
	{
		FvSysLog( FV_SYSLOG_ERR, "Couldn't write process state to %s: %s",
			STATE_FILE, out.StrError() );
		_unlink( STATE_FILE );
		return;
	}

	for (unsigned i=0; i < m_kProcs.size(); i++)
	{
		FvProcessInfo &pi = m_kProcs[i];
		out << pi.m_kCPU << pi.m_kMem << pi.m_uiAffinity;
		pi.m_kStatsMessage.Write( out );
	}

	if (out.Error())
	{
		FvSysLog( FV_SYSLOG_ERR, "Failed to write process table to %s: %s",
			STATE_FILE, out.StrError() );
		_unlink( STATE_FILE );
		return;
	}

	if (m_kProcs.size() > 0)
	{
		FvSysLog( FV_SYSLOG_INFO, "Wrote %d entries to %s prior to shutdown",
			m_kProcs.size(), STATE_FILE );
	}
}


void FvMachined::Load()
{
	FvNetFileStream in( STATE_FILE, "r" );
	struct stat statinfo;

	if (in.Error())
		return;

	if (in.Stat( &statinfo ) == 0)
	{
		time_t age = time( NULL ) - statinfo.st_mtime;
		if (age > 10 * 60)
		{
			FvSysLog( FV_SYSLOG_INFO, "Ignoring out-of-date %s (%d seconds old)",
				STATE_FILE, (int)age );
			_unlink( STATE_FILE );
			return;
		}
	}
	else
	{
		FvSysLog( FV_SYSLOG_ERR, "Couldn't stat %s: %s",
			STATE_FILE, in.StrError() );
		return;
	}

	int len = in.Length();
	while (in.Tell() < len)
	{
		FvProcessInfo pi;
		in >> pi.m_kCPU >> pi.m_kMem >> pi.m_uiAffinity;
		pi.m_kStatsMessage.Read( in );

		if (in.Error())
		{
			FvSysLog( FV_SYSLOG_ERR, "Process table in %s corrupt",
				STATE_FILE );
			_unlink( STATE_FILE );
			return;
		}

		m_kProcs.push_back( pi );
		FvSysLog( FV_SYSLOG_INFO, "Restored %s (pid:%d uid:%d)",
			pi.m_kStatsMessage.m_kName.c_str(), pi.m_kStatsMessage.m_uiPID, pi.m_kStatsMessage.m_uiUID );
	}

	if (len > 0)
	{
		FvSysLog( FV_SYSLOG_INFO, "Restored %d entries from %s",
			m_kProcs.size(), STATE_FILE );
	}

	_unlink( STATE_FILE );
}


void FvMachined::CloseEndpoints()
{
	m_kEP.Close();
	m_kEPBroadcast.Close();
	m_kEPLocal.Close();
}


bool FvMachined::ReadConfigFile()
{
	m_kTags.clear();

	FILE *pkFile = fopen( g_pcMachinedConfFile, "r" );
	if (pkFile == NULL)
	{
		FvSysLog( FV_SYSLOG_WARNING, "Global config file %s doesn't exist",
			g_pcMachinedConfFile );
		return true;
	}

	char buf[ 512 ];
	FvString currTag;

	bool isOkay = true;

	while (isOkay && (fgets( buf, sizeof( buf ), pkFile ) != NULL))
	{
		if (buf[0] == '#' || buf[0] == 0 || buf[0] == '\n')
			continue;

		int len = strlen( buf );

		if (buf[0] == '[')
		{
			if ((buf[ len - 1 ] == '\n') &&
				(buf[ len - 2 ] == ']'))
			{
				buf[ len - 2 ] = '\0';
				currTag = buf + 1;

				m_kTags[ currTag ];
			}
			else
			{
				isOkay = false;
				FvSysLog( FV_SYSLOG_ERR, "Invalid tag '%s'", buf );
			}
		}
		else if (!currTag.empty())
		{
			if (buf[ len - 1 ] == '\n')
			{
				buf[ len - 1 ] = '\0';
				m_kTags[ currTag ].push_back( FvString( buf ) );
			}
			else
			{
				FvSysLog( FV_SYSLOG_ERR, "Max tag length is %u", sizeof( buf ) - 1 );
				isOkay = false;
			}
		}
	}

	fclose( pkFile );

	if (!isOkay)
	{
		return false;
	}

	TagsMap::iterator it = m_kTags.find( "TimingMethod" );
	if (it != m_kTags.end() && it->second.size() > 0)
	{
		m_kTimingMethod = it->second[0];
		FvSysLog( FV_SYSLOG_INFO, "Using %s timing", m_kTimingMethod.c_str() );
	}


	it = m_kTags.find( "InternalInterface" );
	if (it != m_kTags.end() && it->second.size() > 0)
	{
		FvString internalIfaceStr = it->second[0];

		typedef std::map< u_int32_t, FvString > Interfaces;
		Interfaces kInterfaces;
		if (!m_kEP.GetInterfaces( kInterfaces ) || kInterfaces.empty())
		{
			FvSysLog( FV_SYSLOG_ERR, "Could not get interfaces" );
			return false;
		}

#ifdef _WIN32
		if ( (m_uiBroadcastAddr = inet_addr( internalIfaceStr.c_str() ))
			!= INADDR_NONE )	
#else // _WIN32
		if (inet_aton( internalIfaceStr.c_str(),
			(in_addr *)(&m_uiBroadcastAddr) ))
#endif // !_WIN32s		
		{
			Interfaces::iterator iIface = kInterfaces.find( m_uiBroadcastAddr );
			if (iIface == kInterfaces.end())
			{
				FvSysLog( FV_SYSLOG_ERR, "Could not find configured internal interface "
						"'%s' in interfaces for this machine, falling back to "
						"auto-discovery",
					internalIfaceStr.c_str() );
				m_uiBroadcastAddr = 0;
			}
			else
			{
				FvSysLog( FV_SYSLOG_INFO, "Using configured internal interface "
						"identified by address %s (%s)",
					inet_ntoa( (in_addr&)(m_uiBroadcastAddr)),
					iIface->second.c_str() );
			}
		}
		else
		{
			m_uiBroadcastAddr = 0;

			Interfaces::iterator iIface = kInterfaces.begin();
			bool found = false;

			while (iIface != kInterfaces.end())
			{
				if (iIface->second == internalIfaceStr)
				{
					found = true;
					m_uiBroadcastAddr = iIface->first;
					FvSysLog( FV_SYSLOG_INFO, "Using configured internal interface %s (%s)",
							inet_ntoa( (in_addr&)m_uiBroadcastAddr ),
							iIface->second.c_str() );
					break;
				}
				++iIface;
			}

			if (!found)
			{
				FvSysLog( FV_SYSLOG_ERR, "Could not find configured internal "
						"interface named '%s', falling back to auto-discovery",
					internalIfaceStr.c_str() );
			}
		}
	}

	it = m_kTags.find( "MachinePort" );
	if (it != m_kTags.end() && it->second.size() > 0)
	{
		FvString kPort = it->second[0];
		m_uiMachinePort = (FvUInt16)atoi(kPort.c_str());
	}

	it = m_kTags.find( "MachineDiscoveryPort" );
	if (it != m_kTags.end() && it->second.size() > 0)
	{
		FvString kPort = it->second[0];
		m_uiMachineDiscoveryPort = (FvUInt16)atoi(kPort.c_str());
	}

	FvSysLog( FV_SYSLOG_INFO, "MachinePort: %d, MachineDiscoveryPort: %d", m_uiMachinePort, m_uiMachineDiscoveryPort );

	return true;
}

int FvMachined::Run()
{
	this->TimeStamp();

	m_kCluster.m_kBirthHandler.AddTimer();
	m_kCluster.m_kFloodTriggerHandler.AddTimer();

	UpdateHandler updateHandler( *this );
	m_kCallbacks.Add( this->TimeStamp() + UPDATE_INTERVAL,
		UPDATE_INTERVAL, &updateHandler, NULL );

	FvSysLog( FV_SYSLOG_INFO, "Listening for requests" );

#ifndef _WIN32
	int iMaxfd = std::max( (int)m_kEP, std::max( (int)m_kEPBroadcast, (int)m_kEPLocal ) );
#else // !_WIN32
	int iMaxfd = std::max( (int)m_kEP, (int)m_kEPLocal );
#endif // _WIN32

	while(!m_bBreak)
	{
		struct timeval tv;
		fd_set fds;

		while(!m_bBreak)
		{
			FvTimeQueue64::TimeStamp tickTime = this->TimeStamp();
			m_kCallbacks.Process( tickTime );

			FvTimeQueue64::TimeStamp ttn = m_kCallbacks.NextEXP( tickTime );
			TimeStampToTV( ttn, tv );

			FD_ZERO( &fds );
			FD_SET( (int)m_kEP, &fds );
#ifndef _WIN32
			FD_SET( (int)m_kEPBroadcast, &fds );
#endif // !_WIN32
			FD_SET( (int)m_kEPLocal, &fds );
			int selgot = select( iMaxfd+1, &fds, NULL, NULL, &tv );
			if (selgot == 0) break;
			if (selgot == -1)
			{
#ifndef _WIN32
				if(errno != EINTR)
#else // !_WIN32
				if(WSAGetLastError() != WSAEINTR)
#endif // _WIN32
				{
					FvSysLog( FV_SYSLOG_CRIT, "select got an error: %s",
						strerror( errno ) );
					return 1;
				}

				FvSysLog( FV_SYSLOG_ERR, "select returned %d - %s",
					errno, strerror( errno ) );
				continue;
			}

			if (FD_ISSET( (int)m_kEP, &fds ))
			{
				this->ReadPacket( m_kEP, tickTime );
			}

			if (FD_ISSET( (int)m_kEPLocal, &fds ))
			{
				this->ReadPacket( m_kEPLocal, tickTime );
			}
#ifndef _WIN32
			if (FD_ISSET( (int)m_kEPBroadcast, &fds ))
			{
				this->ReadPacket( m_kEPBroadcast, tickTime );
			}
#endif // !#ifndef
		}
	}

	m_kCallbacks.Clear();

	return 0;
}


void FvMachined::ReadPacket( FvNetEndpoint & ep, FvTimeQueue64::TimeStamp & tickTime )
{
	static char streamBuf[ FvNetMGMPacket::MAX_SIZE ];
	sockaddr_in	sin;

	int len = ep.RecvFrom( &streamBuf, sizeof( streamBuf ), sin );
	if (len == -1)
	{
		FvSysLog( FV_SYSLOG_ERR, "recvfrom got an error: %s", strerror( errno ) );
		return;
	}

	FvMemoryIStream is( streamBuf, len );
	FvNetMGMPacket *packet = new FvNetMGMPacket( is );
	if (is.Error())
	{
		FvSysLog( FV_SYSLOG_ERR, "Dropping packet with bogus message" );
		delete packet;
		return;
	}

	if (packet->m_uiFlags & packet->PACKET_STAGGER_REPLIES)
	{
		m_kCallbacks.Add( tickTime + rand() % STAGGER_REPLY_PERIOD,
						0, &m_kPacketTimeoutHandler,
						new IncomingPacket( *this, packet, m_kEP, sin ) );
		return;
	}

	this->HandlePacket( ep, sin, *packet );
	delete packet;
}


void FvMachined::UpdateSystemInfo()
{
	static bool firstTime = true;
	FvSystemInfo &si = m_kSystemInfo;

	if (!UpdateSystemInfoP( si ))
		return;

	if (firstTime)
	{
		firstTime = false;
		if (!UpdateSystemInfoP( si ))
			return;
	}

	for (unsigned int i=0; i < si.m_uiNumCPUs; i++)
	{
		FvUInt64 uiCPUDiff = std::max( si.m_kCPU[i].max.delta(), (FvUInt64)1 );
		FvUInt8 uiCPULoad = (FvUInt8)(si.m_kCPU[i].val.delta()*0xff / uiCPUDiff);

		si.m_kWholeMessage.m_kCPULoads[i] = uiCPULoad;

		si.m_kHPMessage.m_kCPULoads[i] = uiCPULoad;
	}

	FvUInt64 uiIOWaitDiff = std::max( si.m_kIOWait.max.delta(), (FvUInt64)1 );
	si.m_kHPMessage.m_uiIOWaitTime = (FvUInt8)((si.m_kIOWait.val.delta() * 0xff) / uiIOWaitDiff);

	si.m_kWholeMessage.m_uiMem   = (FvUInt8)(si.m_kMem.val.cur()*0xff / si.m_kMem.max.cur());
	si.m_kHPMessage.m_uiMem = (FvUInt8)(si.m_kMem.val.cur()*0xff / si.m_kMem.max.cur());

	if (si.m_kWholeMessage.m_kIFStats.empty())
	{
		si.m_kWholeMessage.SetNInterfaces( si.m_kIFInfo.size() );
	}

	if (si.m_kHPMessage.m_kIFStats.empty())
	{
		si.m_kHPMessage.SetNInterfaces( si.m_kIFInfo.size() );
	}

	for (int i=0; i < si.m_kWholeMessage.m_uiNInterfaces; i++)
	{
		si.m_kWholeMessage.m_kIFStats[i].m_kName = si.m_kIFInfo[i].m_kName;

		si.m_kWholeMessage.m_kIFStats[i].m_uiBitsIn = (FvUInt8)std::min(
			si.m_kIFInfo[i].m_kBitsTotIn.delta() / BIT_INCREMENT, (FvUInt64)0xff );

		si.m_kWholeMessage.m_kIFStats[i].m_uiBitsOut = (FvUInt8)std::min(
			si.m_kIFInfo[i].m_kBitsTotOut.delta() / BIT_INCREMENT, (FvUInt64)0xff );

		si.m_kWholeMessage.m_kIFStats[i].m_uiPackIn = (FvUInt8)std::min(
			si.m_kIFInfo[i].m_kPackTotIn.delta() / PACK_INCREMENT, (FvUInt64)0xff );

		si.m_kWholeMessage.m_kIFStats[i].m_uiPackOut = (FvUInt8)std::min(
			si.m_kIFInfo[i].m_kPackTotOut.delta() / PACK_INCREMENT, (FvUInt64)0xff );

		si.m_kHPMessage.m_kIFStats[ i ].m_kName    = si.m_kIFInfo[ i ].m_kName;
		si.m_kHPMessage.m_kIFStats[ i ].m_uiBitsIn  = (FvUInt8)si.m_kIFInfo[ i ].m_kBitsTotIn.delta();
		si.m_kHPMessage.m_kIFStats[ i ].m_uiBitsOut = (FvUInt8)si.m_kIFInfo[ i ].m_kBitsTotOut.delta();
		si.m_kHPMessage.m_kIFStats[ i ].m_uiPackIn  = (FvUInt8)si.m_kIFInfo[ i ].m_kPackTotIn.delta();
		si.m_kHPMessage.m_kIFStats[ i ].m_uiPackOut = (FvUInt8)si.m_kIFInfo[ i ].m_kPackTotOut.delta();
	}

	si.m_kWholeMessage.m_uiInDiscards = (FvUInt8)std::min( si.m_kPackDropIn.cur(), (FvUInt64)0xff );
	si.m_kWholeMessage.m_uiOutDiscards = (FvUInt8)std::min( si.m_kPackDropOut.cur(), (FvUInt64)0xff );

	si.m_kHPMessage.m_uiInDiscards = (FvUInt8)std::min( si.m_kPackDropIn.cur(), (FvUInt64)0xff );
	si.m_kHPMessage.m_uiOutDiscards = (FvUInt8)std::min( si.m_kPackDropOut.cur(), (FvUInt64)0xff );
}


void FvMachined::Update()
{
	this->UpdateSystemInfo();

	for (unsigned int i=0; i < m_kProcs.size(); i++)
	{
		FvProcessInfo &pi = m_kProcs[i];
		if (!UpdateProcessStats( pi ) && errno == ENOENT)
		{
			FvSysLog( FV_SYSLOG_ERR, "%s died without deregistering!",
				pi.m_kStatsMessage.c_str() );
			RemoveRegisteredProc( i-- );
		}
	}

	m_kBirthListeners.CheckListeners();
	m_kDeathListeners.CheckListeners();

	//waitpid( -1, NULL, WNOHANG );
}


bool FvMachined::BroadcastToListeners( FvNetProcessMessage &pm, int type )
{
	FvUInt8 uiOldParam = pm.m_uiParam;
	pm.m_uiParam = type | pm.PARAM_IS_MSGTYPE;

	bool ok = pm.SendTo( m_kEP, htons( m_uiMachinePort ), FV_NET_BROADCAST,
		FvNetMGMPacket::PACKET_STAGGER_REPLIES );

	pm.m_uiParam = uiOldParam;
	return ok;
}

void FvMachined::RemoveRegisteredProc( unsigned index )
{
	if (index >= m_kProcs.size())
	{
		FvSysLog( FV_SYSLOG_ERR, "Can't remove reg proc at index %d/%u",
			index, m_kProcs.size() );
		return;
	}

	FvProcessInfo &pinfo = m_kProcs[ index ];
	FvNetProcessMessage pm;
	pm << pinfo.m_kStatsMessage;
	this->BroadcastToListeners( pm, pm.NOTIFY_DEATH );

	m_kProcs.erase( m_kProcs.begin() + index );
}

void FvMachined::SendSignal (const FvNetSignalMessage & sm)
{
	for (unsigned int i=0; i < m_kProcs.size(); i++)
	{
		FvProcessInfo &pm = m_kProcs[i];

		if (pm.m_kStatsMessage.matches( sm ))
		{
#ifndef _WIN32
			kill( pm.m_kStatsMessage.m_uiPID, sm.m_uiSignal );
#else // _WIN32
			switch( sm.m_uiSignal )
			{
			case SIGINT:
				//SetEvent( m_kProcs[i].hStopEvent );
				break;
			case SIGQUIT:
				//TerminateProcess( m_kProcs[i].hProcess, 0);
				break;
			case SIGUSR1:
				FvSysLog( FV_SYSLOG_ERR,
					"USR1 not yet implement for Windows fvmachined" );
				break;
			}
#endif // !_WIN32
			FvSysLog( FV_SYSLOG_INFO, "sendSignal: signal = %d pid = %d uid = %d",
				sm.m_uiSignal, pm.m_kStatsMessage.m_uiPID, pm.m_kStatsMessage.m_uiUID );
		}
	}
}

void FvMachined::HandlePacket( FvNetEndpoint & ep, sockaddr_in &sin,
	FvNetMGMPacket &packet )
{
	FvNetMGMPacket replies;
	for (unsigned i=0; i < packet.m_kMessages.size(); i++)
		if (!this->HandleMessage( sin, *packet.m_kMessages[i], replies ))
			return;

	if (replies.m_kMessages.size() > 0)
	{
		FvMemoryOStream os;

		if (replies.Write( os ))
		{
			ep.SendTo( os.Data(), os.Size(), sin );
		}
		else
		{
			FvSysLog( FV_SYSLOG_ERR, "handlePacket: Unable to generate reponse to "
				"message from %s.", ep.c_str() );
		}

	}
}

bool FvMachined::HandleMessage( sockaddr_in &sin, FvNetMachineGuardMessage &mgm,
	FvNetMGMPacket &replies )
{
	switch (mgm.m_uiMessage)
	{

	case FvNetMachineGuardMessage::LISTENER_MESSAGE:
	{
		FvNetListenerMessage &lm = static_cast< FvNetListenerMessage& >( mgm );

		if (lm.m_uiParam == (lm.ADD_BIRTH_LISTENER | lm.PARAM_IS_MSGTYPE))
			m_kBirthListeners.Add( lm, sin.sin_addr.s_addr );
		else if (lm.m_uiParam == (lm.ADD_DEATH_LISTENER | lm.PARAM_IS_MSGTYPE))
			m_kDeathListeners.Add( lm, sin.sin_addr.s_addr );
		else
		{
			FvSysLog( FV_SYSLOG_ERR, "Unrecognised param field for FvNetListenerMessage: %x",
				lm.m_uiParam );
			return false;
		}

		lm.OutGoing( true );
		replies.Append( lm );

		return true;
	}

	case FvNetMachineGuardMessage::WHOLE_MACHINE_MESSAGE:
	{
		if (mgm.OutGoing())
		{
			FvUInt32 inaddr = sin.sin_addr.s_addr;

			if (m_kCluster.m_pkFloodReplyHandler &&
				mgm.seq() == m_kCluster.m_pkFloodReplyHandler->GetSeq())
			{
				m_kCluster.m_pkFloodReplyHandler->MarkReceived( inaddr );
			}
			else
			{
				FvSysLog( FV_SYSLOG_ERR, "Unsolicited WMM from %s:%d",
					inet_ntoa( (in_addr&)inaddr ), ntohs( sin.sin_port ) );
			}
		}
		else
		{
			m_kSystemInfo.m_kWholeMessage.CopySeq( mgm );
			replies.Append( m_kSystemInfo.m_kWholeMessage );
		}
		return true;
	}
	case FvNetMachineGuardMessage::HIGH_PRECISION_MACHINE_MESSAGE:
	{
		if (mgm.OutGoing())
		{
			FvUInt32 inaddr = sin.sin_addr.s_addr;

			if (m_kCluster.m_pkFloodReplyHandler &&
				mgm.seq() == m_kCluster.m_pkFloodReplyHandler->GetSeq())
			{
				m_kCluster.m_pkFloodReplyHandler->MarkReceived( inaddr );
			}
			else
			{
				FvSysLog( FV_SYSLOG_ERR, "Unsolicited HPMM from %s:%d",
					inet_ntoa( (in_addr&)inaddr ), ntohs( sin.sin_port ) );
			}
		}
		else
		{
			m_kSystemInfo.m_kHPMessage.CopySeq( mgm );
			replies.Append( m_kSystemInfo.m_kHPMessage );
		}
		return true;
	}

	case FvNetMachineGuardMessage::PROCESS_MESSAGE:
	{
		FvNetProcessMessage &pm = static_cast< FvNetProcessMessage& >( mgm );

		if (!(pm.m_uiParam & pm.PARAM_IS_MSGTYPE))
		{
			FvSysLog( FV_SYSLOG_ERR, "PROCESS_MESSAGE tried to use param filters! (%x)",
				pm.m_uiParam );
			return false;
		}

		int msgtype = pm.m_uiParam & ~pm.PARAM_IS_MSGTYPE;

		if ((msgtype == pm.REGISTER || msgtype == pm.DEREGISTER) &&
			(FvUInt32&)sin.sin_addr != m_kCluster.m_uiOwnAddr &&
			(FvUInt32&)sin.sin_addr != FV_NET_LOCALHOST)
		{
			FvSysLog( FV_SYSLOG_ERR, "%s tried to register a process here!",
				inet_ntoa( sin.sin_addr ) );
			return false;
		}

		switch (msgtype)
		{
		case FvNetProcessMessage::REGISTER:
		{
			unsigned int i = 0;
			while (i < m_kProcs.size())
			{
				FvNetProcessMessage &psm = m_kProcs[i].m_kStatsMessage;

				if (pm.m_uiPID == psm.m_uiPID && pm.m_uiCategory == psm.m_uiCategory &&
					pm.m_kName == psm.m_kName)
					break;

				if (pm.m_uiPort == psm.m_uiPort)
				{
					FvSysLog( FV_SYSLOG_ERR, "%d registered on port (%d) "
						"that belonged to %d",
						pm.m_uiPID, pm.m_uiPort, psm.m_uiPID );
					RemoveRegisteredProc( i );
				}
				else
					++i;
			}

			if (i < m_kProcs.size())
				FvSysLog( FV_SYSLOG_ERR, "Received re-registration for %s",
					pm.c_str() );
			else
				m_kProcs.push_back( FvProcessInfo() );

			FvProcessInfo &pi = m_kProcs[i];
			pi.m_kStatsMessage << pm;
			pi.m_kStatsMessage.OutGoing( true );

			for (int j=0; j < 2; j++) UpdateProcessStats( pi );

			pi.Init( pm );

			//! TODO: 局域网内广播的包会丢吗?
	 		BroadcastToListeners( pm, pm.NOTIFY_BIRTH );

			pm.OutGoing( true );
			replies.Append( pm );

			FvSysLog( FV_SYSLOG_INFO, "Added %s at %d", pm.c_str(), i );
			return true;
		}

		case FvNetProcessMessage::DEREGISTER:
		{
			unsigned int i;
			for (i=0; i < m_kProcs.size(); i++)
				if (pm.m_uiPID == m_kProcs[i].m_kStatsMessage.m_uiPID)
					break;

			if (i >= m_kProcs.size())
				FvSysLog( FV_SYSLOG_ERR, "Couldn't find pid %d to deregister it",
					pm.m_uiPID );
			else
				RemoveRegisteredProc( i );

			pm.OutGoing( true );
			replies.Append( pm );

			FvSysLog( FV_SYSLOG_INFO, "Deregistered %s", pm.c_str() );
			return true;
		}

		case FvNetProcessMessage::NOTIFY_BIRTH:
		{
			m_kBirthListeners.HandleNotify( pm, sin.sin_addr );
			return true;
		}

		case FvNetProcessMessage::NOTIFY_DEATH:
		{
			m_kDeathListeners.HandleNotify( pm, sin.sin_addr );
			return true;
		}

		default:
			FvSysLog( FV_SYSLOG_ERR, "Unrecognised FvNetProcessMessage type: %d", msgtype );
			return false;
		}
	}

	case FvNetMachineGuardMessage::PROCESS_STATS_MESSAGE:
	{
		FvNetProcessStatsMessage &query = static_cast< FvNetProcessStatsMessage& >( mgm );

		bool found = false;
		for (std::vector< FvProcessInfo >::iterator it = m_kProcs.begin();
			 it != m_kProcs.end(); ++it)
		{
			FvProcessInfo &pi = *it;
			if (pi.m_kStatsMessage.matches( query ))
			{
				FvSystemInfo &si = m_kSystemInfo;
				FvUInt64 cpuDiff = std::max(
					si.m_kCPU[ pi.m_uiAffinity % si.m_uiNumCPUs ].max.delta(),
					(FvUInt64)1 );

				if (pi.m_uiAffinity >= si.m_uiNumCPUs)
					FvSysLog( FV_SYSLOG_ERR, "FvProcessInfo (%s) has invalid m_uiAffinity %d",
						pi.m_kStatsMessage.c_str(), pi.m_uiAffinity );

				FvNetProcessStatsMessage &reply = pi.m_kStatsMessage;
				reply.m_uiCPU = (FvUInt8)(pi.m_kCPU.delta()*0xff / cpuDiff);
				reply.m_uiMem = (FvUInt8)(pi.m_kMem.cur()*0xff / si.m_kMem.max.cur());

				reply.CopySeq( query );
				replies.Append( reply );
				found = true;
			}
		}

		if (!found)
		{
			query.m_uiPID = 0;
			query.OutGoing( true );
			replies.Append( query );
		}

		return true;
	}

	case FvNetMachineGuardMessage::CREATE_MESSAGE:
	case FvNetMachineGuardMessage::CREATE_WITH_ARGS_MESSAGE:
	{
		FvNetCreateMessage &cm = static_cast< FvNetCreateMessage& >( mgm );
		FvSysLog( FV_SYSLOG_INFO, "Got message: %s", cm.c_str() );

		FvNetPidMessage *pPm = new FvNetPidMessage();
		pPm->CopySeq( cm );
		pPm->OutGoing( true );
		replies.Append( *pPm, true );

		FvNetUserMessage *pUm = m_kUsers.Fetch( cm.m_uiUID );

		if (pUm == NULL)
		{
			struct passwd *ent = FvGetPWUID( cm.m_uiUID );
			if (ent == NULL)
			{
				FvSysLog( FV_SYSLOG_ERR, "UID %d doesn't exist on this system, "
					"not starting %s", cm.m_uiUID, cm.m_kName.c_str() );
				pPm->m_uiRunning = 0;
				return true;
			}

			pUm = m_kUsers.Add( ent );
		}

		if (!m_kUsers.GetEnv( *pUm ))
		{
			FvSysLog( FV_SYSLOG_ERR, "Couldn't get env for user %s, not starting %s",
				pUm->m_kUsername.c_str(), cm.c_str() );
			pPm->m_uiRunning = 0;
			return true;
		}

		if (cm.m_kName.find( ".." ) != FvString::npos ||
			cm.m_kConfig.find( ".." ) != FvString::npos)
		{
			FvSysLog( FV_SYSLOG_ERR,
				"Illegal '..' in process name or config, not starting %s/%s",
				cm.m_kName.c_str(), cm.m_kConfig.c_str() );

			pPm->m_uiRunning = 0;
			return true;
		}

		static const unsigned int NUM_SPARE_ARGS_FOR_STARTPROCESS = 2;

		if (mgm.m_uiMessage == FvNetMachineGuardMessage::CREATE_MESSAGE)
		{
			unsigned int argc = 0;
			static const unsigned int MAX_ARGC = 10;

			const char *argv[ MAX_ARGC + NUM_SPARE_ARGS_FOR_STARTPROCESS ];

			argv[ argc++ ] = NULL;

			argv[ argc++ ] = "-machined";

			if (cm.m_uiRecover)
				argv[ argc++ ] = "-recover";

			char forwardArg[32];
			if (cm.m_uiFWDIP != 0)
			{
				FvSNPrintf( forwardArg, sizeof( forwardArg ), "%s:%d",
					inet_ntoa( (in_addr&)cm.m_uiFWDIP ),
					ntohs( cm.m_uiFWDPort ) );
				argv[ argc++ ] = "-forward";
				argv[ argc++ ] = forwardArg;
			}

			if (argc >= MAX_ARGC)
			{
				FvSysLog( FV_SYSLOG_ERR, "Not starting process %s for uid %d: "
					"too many args (%d)", cm.m_kName.c_str(), cm.m_uiUID, argc );
				pPm->m_uiRunning = 0;
			}
			else
			{
				pPm->m_uiRunning = 1;
				pPm->m_uiPID = StartProcess( pUm->m_kRoot.c_str(),
					pUm->m_kResPath.c_str(), cm.m_kConfig.c_str(),
					cm.m_kName.c_str(), pUm->m_uiUID, pUm->m_uiGID, pUm->m_kHome.c_str(),
					argc, argv, *this );
			}
		}
		else
		{
			FvNetCreateWithArgsMessage &cwam =
				static_cast< FvNetCreateWithArgsMessage& >( cm );

			unsigned int argc = 0;
			const char **argv = new const char*[cwam.m_kArgs.size() +
			                        NUM_SPARE_ARGS_FOR_STARTPROCESS + 2];

			argv[ argc++ ] = NULL;

			for ( FvNetCreateWithArgsMessage::Args::const_iterator i =
					cwam.m_kArgs.begin(); i != cwam.m_kArgs.end(); ++i )
			{
				argv[ argc++ ] = i->c_str();
			}

			if (cm.m_uiRecover)
				argv[ argc++ ] = "-recover";


			pPm->m_uiPID = StartProcess( pUm->m_kRoot.c_str(),
				pUm->m_kResPath.c_str(), cm.m_kConfig.c_str(), cm.m_kName.c_str(),
				pUm->m_uiUID, pUm->m_uiGID, pUm->m_kHome.c_str(), argc, argv, *this );

			pPm->m_uiRunning = 1;

			delete [] argv;
		}

		return true;
	}

	case FvNetMachineGuardMessage::SIGNAL_MESSAGE:
	{
		FvNetSignalMessage &sm = static_cast< FvNetSignalMessage& >( mgm );
		SendSignal( sm );
		return true;
	}

	case FvNetMachineGuardMessage::TAGS_MESSAGE:
	{
		FvNetTagsMessage &tm = static_cast< FvNetTagsMessage& >( mgm );

		if (tm.m_kTags.size() != 1)
		{
			FvSysLog( FV_SYSLOG_ERR, "Tags queries must pass only one tag (%d passed)",
				tm.m_kTags.size() );
			return false;
		}
		FvString query = tm.m_kTags[0];
		tm.m_kTags.clear();

		if (query == "")
		{
			for (TagsMap::iterator it = m_kTags.begin(); it != m_kTags.end(); ++it)
				tm.m_kTags.push_back( it->first );
			tm.m_uiExists = true;
		}
		else
		{
			TagsMap::iterator it = m_kTags.find( query );

			if (it != m_kTags.end())
			{
				Tags &tags = it->second;
				tm.m_kTags.resize( tags.size() );
				std::copy( tags.begin(), tags.end(), tm.m_kTags.begin() );
				tm.m_uiExists = true;
			}
			else
				tm.m_uiExists = false;
		}

		tm.OutGoing( true );
		replies.Append( tm );
		return true;
	}

	case FvNetMachineGuardMessage::USER_MESSAGE:
	{
		FvNetUserMessage &um = static_cast< FvNetUserMessage& >( mgm );
		std::vector< FvNetUserMessage* > matches;

#		define FAIL 								\
		{ 											\
			m_kUsers.m_kNotFound.CopySeq( um );		\
			replies.Append( m_kUsers.m_kNotFound );	\
			return true;							\
		}

		if (um.m_uiParam & um.PARAM_USE_UID)
		{
			FvNetUserMessage *pMatch = m_kUsers.Fetch( um.m_uiUID );
			if (pMatch != NULL)
				matches.push_back( pMatch );
		}
		else
		{
			for (FvUserMap::Map::iterator it = m_kUsers.m_kMap.begin();
				 it != m_kUsers.m_kMap.end(); ++it)
			{
				if (it->second.Matches( um ))
					matches.push_back( &it->second );
			}
		}

		if (matches.empty())
		{
			if (um.m_uiParam & (um.PARAM_USE_UID | um.PARAM_USE_NAME))
			{
				struct passwd *ent = um.m_uiParam & um.PARAM_USE_UID ?
					FvGetPWUID( um.m_uiUID ) : FvGetPWNam( um.m_kUsername.c_str() );
				if (ent == NULL)
					FAIL;

				matches.push_back( m_kUsers.Add( ent ) );
			}
			else
				FAIL;
		}

		if (um.m_uiParam & um.PARAM_SET)
		{
			if (matches.size() > 1)
			{
				FvSysLog( FV_SYSLOG_ERR, "Can't set user config for multiple users: %s",
					um.c_str() );
				FAIL;
			}

			FvNetUserMessage &match = *matches[0];
			match.m_kRoot = um.m_kRoot;
			match.m_kResPath = um.m_kResPath;
			match.CopySeq( um );
			if (m_kUsers.SetEnv( match ))
				replies.Append( match );
			else
				FAIL;
		}

		else
		{
			for (unsigned i=0; i < matches.size(); i++)
			{
				matches[i]->CopySeq( um );

				if (um.m_uiParam & um.PARAM_REFRESH_ENV)
				{
					m_kUsers.GetEnv( *matches[i] );
				}

				matches[i]->m_uiParam = um.m_uiParam;
				if (um.m_uiParam & um.PARAM_CHECK_COREDUMPS)
				{
					CheckCoreDumps( matches[i]->m_kRoot.c_str(), *matches[i] );
				}

				replies.Append( *matches[i] );
			}
		}

		return true;
#		undef FAIL
	}

	case FvNetMachineGuardMessage::PID_MESSAGE:
	{
		FvNetPidMessage &pm = static_cast< FvNetPidMessage& >( mgm );
		pm.m_uiRunning = CheckProcess( pm.m_uiPID );
		pm.OutGoing( true );
		replies.Append( pm );
		return true;
	}

	case FvNetMachineGuardMessage::RESET_MESSAGE:
	{
		FvNetResetMessage &rm = static_cast< FvNetResetMessage& >( mgm );
		this->ReadConfigFile();
		m_kUsers.Flush();
		FvSysLog( FV_SYSLOG_INFO, "Flushing tags and user mapping at %s's request",
			inet_ntoa( sin.sin_addr ) );
		rm.OutGoing( true );
		replies.Append( rm );
		return true;
	}

	case FvNetMachineGuardMessage::MACHINED_ANNOUNCE_MESSAGE:
	{
		FvNetMachinedAnnounceMessage &mam =
			static_cast< FvNetMachinedAnnounceMessage& >( mgm );

		if (mam.m_uiType == mam.ANNOUNCE_BIRTH)
		{
			if (mam.OutGoing())
			{
				m_kCluster.m_kBirthHandler.MarkReceived( sin.sin_addr.s_addr,
					mam.m_uiCount );
				return true;
			}
			else
			{
				FvUInt32 inaddr = sin.sin_addr.s_addr;
				if (m_kCluster.m_kMachines.count( inaddr ) == 0)
				{
					m_kCluster.m_kMachines.insert( inaddr );
					m_kCluster.ChooseBuddy();
				}

				mam.OutGoing( true );
				mam.m_uiCount = m_kCluster.m_kMachines.size();
				replies.Append( mam );
				return true;
			}
		}
		else if (mam.m_uiType == mam.ANNOUNCE_DEATH)
		{
			FvUInt32 deadaddr = (FvUInt32)mam.m_uiAddr;

			if (deadaddr != m_kCluster.m_uiOwnAddr)
			{
				char addrstr[32];
				strcpy( addrstr, inet_ntoa( sin.sin_addr ) );
				FvSysLog( FV_SYSLOG_INFO, "%s says %s is gone",
					addrstr, inet_ntoa( (in_addr&)deadaddr ) );

				m_kCluster.m_kMachines.erase( deadaddr );
				m_kCluster.ChooseBuddy();
			}
			else
			{
				m_kCluster.m_kBirthHandler.AddTimer();
				FvSysLog( FV_SYSLOG_INFO,
					"Reports of my death have been greatly exaggerated!" );
			}
			return true;
		}
		else if (mam.m_uiType == mam.ANNOUNCE_EXISTS)
		{
			if (m_kCluster.m_kMachines.count( mam.m_uiAddr ) == 0)
			{
				FvSysLog( FV_SYSLOG_INFO, "Apparently %s is running machined",
					inet_ntoa( (in_addr&)mam.m_uiAddr ) );
				m_kCluster.m_kMachines.insert( mam.m_uiAddr );
				m_kCluster.ChooseBuddy();
			}
			return true;
		}

		else
		{
			FvSysLog( FV_SYSLOG_ERR, "Received unknown MAM type %d", mam.m_uiType );
			return false;
		}
	}
	case FvNetMachineGuardMessage::QUERY_INTERFACE_MESSAGE:
	{
		FvNetQueryInterfaceMessage &qim = static_cast< FvNetQueryInterfaceMessage& >(mgm);

		if (qim.m_uiAddress == FvNetQueryInterfaceMessage::INTERNAL)
		{
			qim.m_uiAddress = m_uiBroadcastAddr;
		}
		else
		{
			FvSysLog( FV_SYSLOG_ERR, "Received QIF request for unknown type %d",
						qim.m_uiAddress );
		}

		qim.OutGoing( true );
		replies.Append( qim );

		return true;
	}

	default:
		if (mgm.m_uiFlags & mgm.MESSAGE_NOT_UNDERSTOOD)
		{
			FvSysLog( FV_SYSLOG_ERR, "Received unknown message: %s", mgm.c_str() );
			mgm.OutGoing( true );
			replies.Append( mgm );
			return true;
		}
		else
		{
			FvSysLog( FV_SYSLOG_ERR, "Unknown message (%d) not marked as "
				"MESSAGE_NOT_UNDERSTOOD!!!", mgm.m_uiMessage );
			return false;
		}

	}

	return false;
}


FvMachined::IncomingPacket::IncomingPacket( FvMachined &machined,
	FvNetMGMPacket *pPacket, FvNetEndpoint &ep, sockaddr_in &sin ) :
	m_kMachined( machined ),
	m_pkPacket( pPacket ),
	m_pkEP( &ep ),
	m_kSin( sin )
{
	if (!(m_pkPacket->m_uiFlags & m_pkPacket->PACKET_STAGGER_REPLIES))
		FvSysLog( FV_SYSLOG_ERR, "Broadcast packet didn't have flag: %x",
			m_pkPacket->m_uiFlags );
}

void FvMachined::IncomingPacket::Handle()
{
	FvMemoryOStream os;
	m_kMachined.HandlePacket( *m_pkEP, m_kSin, *m_pkPacket );
}

void FvMachined::PacketTimeoutHandler::HandleTimeout(
	FvTimeQueueID id, void *pUser )
{
	IncomingPacket *pIP = (IncomingPacket*)pUser;
	pIP->Handle();
}

void FvMachined::PacketTimeoutHandler::OnRelease( FvTimeQueueID id, void *pUser )
{
	IncomingPacket *pIP = (IncomingPacket*)pUser;
	delete pIP;
}

FvTimeQueue64::TimeStamp FvMachined::TimeStamp()
{
	return Timestamp() * 1000/StampsPerSecond();
}

void FvMachined::UpdateHandler::HandleTimeout( FvTimeQueueID id, void *pUser )
{
	m_kMachined.Update();
}
