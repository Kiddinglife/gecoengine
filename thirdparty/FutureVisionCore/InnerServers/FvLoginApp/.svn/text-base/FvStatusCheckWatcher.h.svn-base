//{future header message}
#ifndef __FvStatusCheckWatcher_H__
#define __FvStatusCheckWatcher_H__

#include <FvWatcher.h>
#include <FvNetInterfaces.h>

class FvStatusCheckWatcher : public FvCallableWatcher
{
public:
	FvStatusCheckWatcher();

protected:
	virtual bool SetFromStream( void * base,
			const char * path,
			FvWatcherPathRequestV2 & pathRequest );

private:
	class ReplyHandler : public FvNetReplyMessageHandler
	{
	public:
		ReplyHandler( FvWatcherPathRequestV2 & pathRequest );

	private:
		virtual void HandleMessage( const FvNetAddress & source,
			FvNetUnpackedMessageHeader & header,
			FvBinaryIStream & data,
			void * arg );

		virtual void HandleException( const FvNetNubException & ne,
			void * arg );

	void SendResult( bool status, const FvString & output );

		FvWatcherPathRequestV2 &m_kPathRequest;
	};

};

#endif // __FvStatusCheckWatcher_H__
