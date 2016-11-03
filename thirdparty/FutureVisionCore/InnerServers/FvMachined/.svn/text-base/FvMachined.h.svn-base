//{future header message}
#ifndef __FvMachined_H__
#define __FvMachined_H__

#include <FvSingleton.h>
#include <FvNetEndpoint.h>
#include "FvCluster.h"
#include "FvListeners.h"
#include "FvUserMap.h"
#include "FvCommonMachineGuard.h"

class FvMachined : public FvSingleton< FvMachined >
{
public:
	FvMachined();
	~FvMachined();

	bool ReadConfigFile();
	int Run();
	void Stop(){m_bBreak = true;}

	static const char *STATE_FILE;
	void Save();
	void Load();

	FvNetEndpoint& EP() { return m_kEP; }
	FvCluster &Cluster() { return m_kCluster; }
	const char *TimingMethod() const { return m_kTimingMethod.c_str(); }

	void CloseEndpoints();

	FvUInt16 GetMachinePort() { return m_uiMachinePort; }

	friend class FvCluster;

protected:

	bool FindBroadcastInterface();
	void UpdateSystemInfo();
	void Update();
	bool BroadcastToListeners( FvNetProcessMessage &pm, int type );
	void RemoveRegisteredProc( unsigned index );
	void SendSignal (const FvNetSignalMessage & sm);
	void HandlePacket( FvNetEndpoint & ep, sockaddr_in &sin, FvNetMGMPacket &packet );
	bool HandleMessage( sockaddr_in &sin, FvNetMachineGuardMessage &mgm,
		FvNetMGMPacket &replies );
	void ReadPacket( FvNetEndpoint & ep, FvTimeQueue64::TimeStamp & tickTime );

	static const FvTimeQueue64::TimeStamp UPDATE_INTERVAL = 1000;

	class IncomingPacket
	{
	public:
		IncomingPacket( FvMachined &machined,
			FvNetMGMPacket *pPacket, FvNetEndpoint &ep, sockaddr_in &sin );
		~IncomingPacket() {	delete m_pkPacket; }
		void Handle();

	private:
		FvMachined &m_kMachined;
		FvNetMGMPacket *m_pkPacket;
		FvNetEndpoint *m_pkEP;
		sockaddr_in m_kSin;
	};

	class PacketTimeoutHandler : public FvTimeQueueHandler
	{
	public:
		void HandleTimeout( FvTimeQueueID id, void *pUser );
		void OnRelease( FvTimeQueueID id, void *pUser );
	};

	PacketTimeoutHandler m_kPacketTimeoutHandler;

	class UpdateHandler : public FvTimeQueueHandler
	{
	public:
		UpdateHandler( FvMachined &machined ) : m_kMachined( machined ) {}
		void HandleTimeout( FvTimeQueueID id, void *pUser );
		void OnRelease( FvTimeQueueID id, void *pUser ) {}

	protected:
		FvMachined &m_kMachined;
	};

	u_int32_t m_uiBroadcastAddr;
	FvNetEndpoint m_kEP;

	FvNetEndpoint m_kEPBroadcast;

	FvNetEndpoint m_kEPLocal;

	FvCluster m_kCluster;

	typedef std::map< FvString, Tags > TagsMap;
	TagsMap m_kTags;
	FvString m_kTimingMethod;

	FvSystemInfo m_kSystemInfo;
	std::vector< FvProcessInfo > m_kProcs;

	FvListeners m_kBirthListeners;
	FvListeners m_kDeathListeners;
	FvUserMap m_kUsers;

	FvTimeQueue64 m_kCallbacks;

	bool m_bBreak;

	FvUInt16 m_uiMachinePort;
	FvUInt16 m_uiMachineDiscoveryPort;

public:
	static const FvTimeQueue64::TimeStamp STAGGER_REPLY_PERIOD = 100;

	FvTimeQueue64::TimeStamp TimeStamp();

	FV_INLINE FvTimeQueue64::TimeStamp TVToTimeStamp( struct timeval &tv ) {
		return tv.tv_sec * 1000 + tv.tv_usec / 1000;
	}

	FV_INLINE void TimeStampToTV( FvTimeQueue64::TimeStamp ms, struct timeval &tv ) {
		tv.tv_sec = (long)(ms/1000);
		tv.tv_usec = (long)((ms%1000)*1000);
	}

	FvTimeQueue64& Callbacks() { return m_kCallbacks; }
};

#endif // __FvMachined_H__
