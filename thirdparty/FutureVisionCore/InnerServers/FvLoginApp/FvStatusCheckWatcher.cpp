#include "FvStatusCheckWatcher.h"
#include "FvLoginApp.h"

#include <../FvDBManager/FvDBInterface.h>

FvStatusCheckWatcher::ReplyHandler::ReplyHandler(
		FvWatcherPathRequestV2 & pathRequest ) :
	m_kPathRequest( pathRequest )
{
}

void FvStatusCheckWatcher::ReplyHandler::HandleMessage(
		const FvNetAddress & source,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data,
		void * arg )
{
	bool value;
	std::stringstream outputStream;

	data >> value;

	while (!data.Error() && (data.RemainingLength() > 0))
	{
		FvString line;
		data >> line;

		outputStream << line << std::endl;
	}

	const FvString & output = outputStream.str();
	this->SendResult( value, output );

	delete this;
}


void FvStatusCheckWatcher::ReplyHandler::HandleException(
		const FvNetNubException & ne, void * arg )
{
	FV_WARNING_MSG( "FvStatusCheckWatcher::ReplyHandler::HandleException:\n" );
	this->SendResult( false, "No reply from DBManager\n" );
	delete this;
}


void FvStatusCheckWatcher::ReplyHandler::SendResult( bool status,
		const FvString & output )
{
	FvBinaryOStream & resultStream = m_kPathRequest.GetResultStream();
	resultStream << (FvUInt8)FV_WATCHER_TYPE_TUPLE;
	resultStream << (FvUInt8)WT_READ_ONLY;

	const int TUPLE_COUNT = 2;

	int resultSize = FvBinaryOStream::CalculatePackedIntSize( TUPLE_COUNT ) +
		FvCallableWatcher::TupleElementStreamSize( output.size() ) +
		FvCallableWatcher::TupleElementStreamSize( sizeof( bool ) );

	resultStream.WriteStringLength( resultSize );

	resultStream.WriteStringLength( TUPLE_COUNT );

	WatcherValueToStream( resultStream, output, WT_READ_ONLY );
	WatcherValueToStream( resultStream, status, WT_READ_ONLY );

	m_kPathRequest.SetResult( "", WT_READ_ONLY, NULL, NULL );
}


FvStatusCheckWatcher::FvStatusCheckWatcher() :
	FvCallableWatcher( LOCAL_ONLY, "Check the status of the server" )
{
}


bool FvStatusCheckWatcher::SetFromStream( void * base,
		const char * path,
		FvWatcherPathRequestV2 & pathRequest )
{
	FvLoginApp & app = FvLoginApp::Instance();
	FvNetBundle & bundle = app.GetDBManager().Bundle();
	bundle.StartRequest( DBInterface::CheckStatus,
		   new ReplyHandler( pathRequest ) );
	app.GetDBManager().Send();

	return true;
}