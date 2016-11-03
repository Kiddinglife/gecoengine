#include "FvAnonymousChannelClient.h"

FvAnonymousChannelClient::FvAnonymousChannelClient() :
	m_pkChannelOwner( NULL )
{
}


FvAnonymousChannelClient::~FvAnonymousChannelClient()
{
	delete m_pkChannelOwner;
}


bool FvAnonymousChannelClient::Init( FvNetNub & nub,
		FvNetInterfaceMinder & interfaceMinder,
		const FvNetInterfaceElement & birthMessage,
		const char * componentName,
		int numRetries )
{
	m_kInterfaceName = componentName;

	bool result = true;

	interfaceMinder.handler( birthMessage.GetID(), this );

	if (nub.RegisterBirthListener( birthMessage, componentName ) !=
		FV_NET_REASON_SUCCESS)
	{
		FV_ERROR_MSG( "FvAnonymousChannelClient::Init: "
			"Failed to register birth listener for %s\n",
			componentName );

		result = false;
	}

	FvNetAddress serverAddr( FvNetAddress::NONE );

	if (nub.FindInterface( componentName, 0, serverAddr, numRetries ) !=
		FV_NET_REASON_SUCCESS)
	{
		result = false;
	}

	m_pkChannelOwner = new FvNetChannelOwner( nub, serverAddr );
	m_pkChannelOwner->Channel().IsIrregular( true );

	return result;
}


void FvAnonymousChannelClient::HandleMessage(
		const FvNetAddress & srcAddr,
		FvNetUnpackedMessageHeader & header,
		FvBinaryIStream & data )
{
	InterfaceListenerMsg kMsg;
	data >> kMsg;

	FV_ASSERT( data.RemainingLength() == 0 && !data.Error() );

	m_pkChannelOwner->Addr( FvNetAddress(kMsg.uiIP, kMsg.uiPort) );

	FV_INFO_MSG( "FvAnonymousChannelClient::HandleMessage: "
		"Got new %s at %s\n",
		m_kInterfaceName.c_str(), m_pkChannelOwner->Channel().c_str() );
}
