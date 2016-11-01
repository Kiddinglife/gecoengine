//{future header message}
#ifndef __FvTimeKeeper_H__
#define __FvTimeKeeper_H__

#include "FvServerCommon.h"
#include <FvNetNub.h>
#include <FvBinaryStream.h>


class FV_SERVERCOMMON_API FvTimeKeeper : public FvNetTimerExpiryHandler,
	public FvNetReplyMessageHandler
{
public:
	FvTimeKeeper( FvNetNub & nub, FvNetTimerID trackingTimerID,
			FvTimeStamp & tickCount, int idealTickFrequency,
			const FvNetAddress * masterAddress = NULL,
			const FvNetInterfaceElement * masterRequest = NULL );
	virtual ~FvTimeKeeper();

	bool InputMasterReading( double reading );

	double ReadingAtLastTick() const;
	double ReadingNow() const;
	double ReadingAtNextTick() const;

	void SynchroniseWithPeer( const FvNetAddress & address,
			const FvNetInterfaceElement & request );
	void SynchroniseWithMaster();

private:
	FvInt64 OffsetOfReading( double reading, FvUInt64 stampsAtReceiptExt );

	void ScheduleSyncCheck();

	virtual int HandleTimeout( FvNetTimerID id, void * arg );

	virtual void HandleMessage( const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header, FvBinaryIStream & data, void * );
	virtual void HandleException( const FvNetNubException &, void * );


	FvNetNub &					m_kNub;
	FvNetTimerID				m_kTrackingTimerID;

	FvTimeStamp &				m_uiTickCount;
	double						m_fIdealTickFrequency;

	FvUInt64					m_uiNominalIntervalStamps;
	FvNetTimerID				m_kSyncCheckTimerID;

	const FvNetAddress *		m_pkMasterAddress;
	const FvNetInterfaceElement*m_pkMasterRequest;

	FvUInt64					m_uiLastSyncRequestStamps;
};


#endif // __FvTimeKeeper_H__
