//{future header message}
#ifndef __FvCluster_H__
#define __FvCluster_H__

#include <FvTimeQueue.h>
#include <FvTimestamp.h>
#include <FvNetEndpoint.h>
#include <FvNetMachineGuard.h>
#include <set>

class FvMachined;

class FvCluster
{
public:
	FvCluster( FvMachined &machined );
	~FvCluster();

	void ChooseBuddy();

	friend class FvMachined;
	typedef std::set< FvUInt32 > Addresses;

protected:

	class ClusterTimeoutHandler : public FvTimeQueueHandler
	{
	public:
		ClusterTimeoutHandler( FvCluster &cluster ) :
			m_kCluster( cluster ), m_uiID( 0 ) {}

		void OnRelease( FvTimeQueueID id, void *pUser ) {}
		void AddTimer();
		FvTimeQueueID id() const { return m_uiID; }
		void Cancel();

		virtual FvTimeQueue::TimeStamp Delay() const = 0;
		virtual FvTimeQueue::TimeStamp Interval() const { return 0; }

	protected:
		FvCluster &m_kCluster;
		FvTimeQueueID m_uiID;
	};

	class FloodTriggerHandler : public ClusterTimeoutHandler
	{
	public:
		static const FvTimeQueue::TimeStamp AVERAGE_INTERVAL = 2000;

		FloodTriggerHandler( FvCluster &cluster ) :
			ClusterTimeoutHandler( cluster ) {}
		void HandleTimeout( FvTimeQueueID id, void *pUser );
		FvTimeQueue::TimeStamp Delay() const;
	};

	class FloodReplyHandler : public ClusterTimeoutHandler
	{
	public:
		static const int MAX_RETRIES = 2;

		FloodReplyHandler( FvCluster &cluster );
		void HandleTimeout( FvTimeQueueID id, void *pUser );
		FV_INLINE void MarkReceived( FvUInt32 addr ) { m_kReplied.insert( addr ); }
		FvUInt16 GetSeq() const { return m_kWMM.seq(); }
		void SendBroadcast();

		FvTimeQueue::TimeStamp Delay() const;
		FvTimeQueue::TimeStamp Interval() const { return this->Delay(); }

	private:
		Addresses m_kReplied;
		int m_iTries;
		FvNetWholeMachineMessage m_kWMM;
	};

	class BirthReplyHandler : public ClusterTimeoutHandler
	{
	public:
		BirthReplyHandler( FvCluster &cluster ) :
			ClusterTimeoutHandler( cluster ) {}
		void AddTimer();
		void HandleTimeout( FvTimeQueueID id, void *pUser );
		void MarkReceived( FvUInt32 addr, FvUInt32 count );

		FvTimeQueue::TimeStamp Delay() const;

	protected:
		FvUInt32 m_uiToldSize;
	};

	FvMachined &m_kMachined;

	Addresses m_kMachines;

	FvUInt32 m_uiOwnAddr;
	FvUInt32 m_uiBuddyAddr;

	FloodTriggerHandler m_kFloodTriggerHandler;
	FloodReplyHandler *m_pkFloodReplyHandler;
	BirthReplyHandler m_kBirthHandler;
};

#endif // __FvCluster_H__
