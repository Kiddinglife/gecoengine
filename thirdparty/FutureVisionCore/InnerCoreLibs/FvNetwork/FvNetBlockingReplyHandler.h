//{future header message}
#ifndef __FvNetBlockingReplyHandler_H__
#define __FvNetBlockingReplyHandler_H__

#include "FvNetwork.h"
#include "FvNetNub.h"
#include "FvNetChannel.h"

#include <FvPowerDefines.h>

class FvNetBlockingReplyHandler :
	public FvNetReplyMessageHandler,
	public FvNetTimerExpiryHandler
{
public:
	FvNetBlockingReplyHandler( FvNetNub & nub, FvNetReplyMessageHandler * pHandler );

	FvNetReason WaitForReply( FvNetChannel * pChannel = NULL,
		   int maxWaitMicroSeconds = 10 * 1000000 );

protected:
	virtual int HandleTimeout( FvNetTimerID id, void * arg );

	virtual void HandleMessage( const FvNetAddress & addr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg );

	virtual void HandleException( const FvNetNubException & ex, void * );

private:
	FvNetNub &m_kNub;
	bool m_bIsDone;
	FvNetReason m_eError;

	FvNetTimerID m_kTimerID;

	FvNetReplyMessageHandler *m_pkHandler;
};


template <class RETURNTYPE>
class FvNetBlockingReplyHandlerWithResult : public FvNetBlockingReplyHandler
{
public:
	FvNetBlockingReplyHandlerWithResult( FvNetNub & nub );

	RETURNTYPE & get();

private:
	virtual void HandleMessage( const FvNetAddress & addr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg );

	RETURNTYPE		result_;
};

FV_INLINE FvNetBlockingReplyHandler::FvNetBlockingReplyHandler(
		FvNetNub & nub,
		FvNetReplyMessageHandler * pHandler ) :
	m_kNub( nub ),
	m_bIsDone( false ),
	m_eError( FV_NET_REASON_SUCCESS ),
	m_kTimerID( FV_NET_TIMER_ID_NONE ),
	m_pkHandler( pHandler )
{
}


FV_INLINE FvNetReason FvNetBlockingReplyHandler::WaitForReply( FvNetChannel * pChannel,
	   int maxWaitMicroSeconds )
{
	bool wasBroken = m_kNub.ProcessingBroken();
	bool isRegularChannel = pChannel && !pChannel->IsIrregular();

	if (isRegularChannel)
		pChannel->IsIrregular( true );

	if (maxWaitMicroSeconds > 0)
	{
		m_kTimerID = m_kNub.RegisterTimer( maxWaitMicroSeconds, this );
	}

	while (!m_bIsDone)
	{
		try
		{
			m_kNub.ProcessContinuously();
		}
		catch (FvNetNubException & ne)
		{
			m_eError = ne.Reason();
		}
	}

	if (m_kTimerID != FV_NET_TIMER_ID_NONE)
	{
		m_kNub.CancelTimer( m_kTimerID );
	}

	if (isRegularChannel)
		pChannel->IsIrregular( false );

	m_kNub.BreakProcessing( wasBroken );

	return m_eError;
}


FV_INLINE int FvNetBlockingReplyHandler::HandleTimeout( FvNetTimerID id, void * arg )
{
	FV_INFO_MSG( "BlockingReplyHandler::HandleTimeout: Timer expired\n" );

	m_kNub.CancelReplyMessageHandler( this, FV_NET_REASON_TIMER_EXPIRED );

	return 0;
}


FV_INLINE void FvNetBlockingReplyHandler::HandleMessage(
		const FvNetAddress & addr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg )
{
	if (m_pkHandler)
	{
		m_pkHandler->HandleMessage( addr, header, data, arg );
	}

	m_eError = FV_NET_REASON_SUCCESS;

	m_kNub.BreakProcessing();
	m_bIsDone = true;
}


FV_INLINE void FvNetBlockingReplyHandler::HandleException(
		const FvNetNubException & ex, void * )
{
	if (m_eError == FV_NET_REASON_SUCCESS)
	{
		m_eError = ex.Reason();
	}

	m_kNub.BreakProcessing();
	m_bIsDone = true;
}


template <class RETURNTYPE> FV_INLINE
FvNetBlockingReplyHandlerWithResult<RETURNTYPE>::FvNetBlockingReplyHandlerWithResult(
	FvNetNub & nub ) :
	FvNetBlockingReplyHandler( nub, NULL )
{
}


template <class RETURNTYPE>
FV_INLINE RETURNTYPE & FvNetBlockingReplyHandlerWithResult<RETURNTYPE>::get()
{
	return result_;
}

template <class RETURNTYPE>
FV_INLINE void FvNetBlockingReplyHandlerWithResult<RETURNTYPE>::HandleMessage(
		const FvNetAddress & addr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg )
{
	data >> result_;

	this->FvNetBlockingReplyHandler::HandleMessage( addr, header, data, arg );
}

#endif // __FvNetBlockingReplyHandler_H__
