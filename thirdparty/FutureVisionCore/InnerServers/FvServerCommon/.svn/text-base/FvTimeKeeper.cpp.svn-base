#include "FvTimeKeeper.h"
#include <FvDebug.h>
#include <FvNetBundle.h>

FV_DECLARE_DEBUG_COMPONENT( 0 )


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
FvTimeKeeper::FvTimeKeeper( FvNetNub & nub, FvNetTimerID trackingTimerID,
FvTimeStamp & tickCount, int idealTickFrequency,
const FvNetAddress * masterAddress,
const FvNetInterfaceElement * masterRequest )
:m_kNub( nub )
,m_kTrackingTimerID( trackingTimerID )
,m_uiTickCount( tickCount )
,m_fIdealTickFrequency( double(idealTickFrequency) )
,m_uiNominalIntervalStamps( nub.TimerIntervalTime( trackingTimerID ) )
,m_kSyncCheckTimerID( FV_NET_TIMER_ID_NONE )
,m_pkMasterAddress( masterAddress )
,m_pkMasterRequest( masterRequest )
,m_uiLastSyncRequestStamps( 0 )
{
}

FvTimeKeeper::~FvTimeKeeper()
{
	if (m_kSyncCheckTimerID != 0) m_kNub.CancelTimer( m_kSyncCheckTimerID );
}

bool FvTimeKeeper::InputMasterReading( double reading )
{
	FvUInt64 now = Timestamp();
	FvInt64 roundTripStamps = now - m_uiLastSyncRequestStamps;
	double roundTripTime = double( roundTripStamps ) / StampsPerSecondD();
	double readingNowValue = ReadingNow();

	double offset = (reading + roundTripTime / 2) - readingNowValue;
	FvInt64 offsetStamps = FvInt64( offset * StampsPerSecond() );

	FvUInt64 & intervalStamps = m_kNub.TimerIntervalTime( m_kTrackingTimerID );
	FvUInt64 increment = m_uiNominalIntervalStamps / 20;

	m_uiLastSyncRequestStamps = 0;

	FvInt64 threshold = FvInt64( m_uiNominalIntervalStamps / 20 +
		roundTripStamps / 2 );
	if (offsetStamps >  threshold )
	{
		if (intervalStamps == m_uiNominalIntervalStamps)
		{
			FV_DEBUG_MSG( "TimeKeeper::inputMasterReading: "
				"shortening tick interval because we are slow "
				"by %.0fms\n",
				1000 * offsetStamps / StampsPerSecondD()
			);
			intervalStamps = m_uiNominalIntervalStamps - increment;
			this->ScheduleSyncCheck();
		}
		else if (intervalStamps > m_uiNominalIntervalStamps)
		{
			FV_DEBUG_MSG( "TimeKeeper:: we have slowed down enough, "
				"reverting to nominal tick interval\n" );
			intervalStamps = m_uiNominalIntervalStamps;
		}
		else
		{
			this->ScheduleSyncCheck();
		}
	}
	else if (offsetStamps < -threshold)
	{
		if (intervalStamps == m_uiNominalIntervalStamps)
		{
			FV_DEBUG_MSG( "TimeKeeper::inputMasterReading: "
				"lengthening tick interval because we are fast "
				"by %.0fms\n",
				-1000 * offsetStamps / StampsPerSecondD()
			);
			intervalStamps = m_uiNominalIntervalStamps + increment;
			this->ScheduleSyncCheck();
		}
		else if (intervalStamps < m_uiNominalIntervalStamps)
		{
			FV_DEBUG_MSG( "TimeKeeper:: we sped up enough, "
					"reverting back to nominal tick interval\n" );
			intervalStamps = m_uiNominalIntervalStamps;
		}
		else
		{
			this->ScheduleSyncCheck();
		}
	}
	else
	{
		if (intervalStamps != m_uiNominalIntervalStamps)
		{
			FV_DEBUG_MSG( "TimeKeeper::inputMasterReading: "
				"reverting back to nominal tick interval\n" );
			intervalStamps = m_uiNominalIntervalStamps;
		}
	}

	return true;
}

void FvTimeKeeper::ScheduleSyncCheck()
{
	if (m_kSyncCheckTimerID != 0)
	{
		FV_TRACE_MSG( "TimeKeeper::scheduleSyncCheck: existing timer ID\n" );
		return;
	}

	double deliveryIn = double(FvInt64(m_kNub.TimerDeliveryTime(
		m_kTrackingTimerID ) - Timestamp()));
	double s = std::max( deliveryIn / StampsPerSecondD(), 0.001 );
	int callbackPeriod = int( s * 1000000.0) + 1000;
	m_kSyncCheckTimerID = m_kNub.RegisterCallback( callbackPeriod, this );
}

double FvTimeKeeper::ReadingAtLastTick() const
{
	return double(m_uiTickCount) / m_fIdealTickFrequency;
}

double FvTimeKeeper::ReadingNow() const
{
	double ticksAtNext = double(m_uiTickCount+1);
	double intervalStamps =
		double(m_kNub.TimerIntervalTime( m_kTrackingTimerID ));

	FvUInt64 stampsAtNext = m_kNub.TimerDeliveryTime( m_kTrackingTimerID );
	FvInt64 stampsSinceNext = FvInt64(Timestamp() - stampsAtNext);
	double ticksSinceNext = double(stampsSinceNext) / intervalStamps;

	return (ticksAtNext + ticksSinceNext) / m_fIdealTickFrequency;
}

double FvTimeKeeper::ReadingAtNextTick() const
{
	return double(m_uiTickCount+1) / m_fIdealTickFrequency;
}

void FvTimeKeeper::SynchroniseWithPeer( const FvNetAddress & address,
	const FvNetInterfaceElement & request )
{
	FvNetBundle b;
	b.StartMessage( request );
	b << this->ReadingNow();
	m_kNub.Send( address, b );
}

void FvTimeKeeper::SynchroniseWithMaster()
{
	FV_ASSERT( m_pkMasterAddress != NULL );
	FV_ASSERT( m_pkMasterRequest != NULL );

	if ( *m_pkMasterAddress == FvNetAddress::NONE )
	{
		FV_WARNING_MSG( "TimeKeeper::synchroniseWithMaster: Skipping because "
				"master is not ready\n" );
		return;
	}

	if (m_uiLastSyncRequestStamps)
	{
		FV_WARNING_MSG( "TimeKeeper::synchroniseWithMaster: in progress\n" );
		return;
	}
	FvNetBundle b;
	b.StartRequest( *m_pkMasterRequest, this );
	b << this->ReadingNow();
	m_kNub.Send( *m_pkMasterAddress, b );

	m_uiLastSyncRequestStamps = Timestamp();
}

FvInt64 FvTimeKeeper::OffsetOfReading( double reading, FvUInt64 stampsAtReceiptExt )
{
	double readingAtNextTick = this->ReadingAtNextTick();

	FvUInt64 intervalTime = m_kNub.TimerIntervalTime( m_kTrackingTimerID );
	FvUInt64 deliveryTime = m_kNub.TimerDeliveryTime( m_kTrackingTimerID );

	double stampsPerReadingUnit = double(intervalTime) * m_fIdealTickFrequency;

	FvUInt64 stampsAtReceiptInt = deliveryTime -
		FvUInt64(FvInt64((readingAtNextTick-reading) * stampsPerReadingUnit));
	return FvInt64(stampsAtReceiptExt - stampsAtReceiptInt);
}

int FvTimeKeeper::HandleTimeout( FvNetTimerID id, void * arg )
{
	if (id == m_kSyncCheckTimerID)
	{
		m_kNub.CancelTimer( m_kSyncCheckTimerID );
		m_kSyncCheckTimerID = 0;
		if (m_pkMasterAddress)
		{
			this->SynchroniseWithMaster();
		}
		else
		{
			FvUInt64 & intervalStamps = m_kNub.TimerIntervalTime(
				m_kTrackingTimerID );
			intervalStamps = m_uiNominalIntervalStamps;
		}
		return 0;
	}
	return 0;
}

void FvTimeKeeper::HandleMessage( const FvNetAddress & source,
	FvNetUnpackedMessageHeader & header, FvBinaryIStream & data, void * )
{
	if (header.m_iLength != sizeof(double))
	{
		FV_ERROR_MSG( "TimeKeeper::handleMessage: "
			"Reply from %s expected to be just one 'double' but length is %d\n",
			(char*)source, header.m_iLength );
		return;
	}

	double reading;
	data >> reading;

	if (!this->InputMasterReading( reading ))
	{
		if (m_pkMasterAddress != NULL && m_pkMasterRequest != NULL)
		{
			this->ScheduleSyncCheck();
		}
	}
}

void FvTimeKeeper::HandleException( const FvNetNubException & exception,
		void * )
{
	if (exception.Reason() == FV_NET_REASON_TIMER_EXPIRED)
	{
		FV_WARNING_MSG( "TimeKeeper::handleException: "
			"Reply to game time sync request timed out\n" );
	}
	else
	{
		FV_ERROR_MSG( "TimeKeeper::handleException: %s\n",
				NetReasonToString( exception.Reason() ) );
	}

	if (m_kSyncCheckTimerID != 0)
	{
		m_kNub.CancelTimer( m_kSyncCheckTimerID );
		m_kSyncCheckTimerID = 0;
	}
	m_uiLastSyncRequestStamps = 0;
}



