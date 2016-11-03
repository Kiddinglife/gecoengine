
FV_INLINE 
FvNetInterfaceMinder::FvNetInterfaceMinder( const char * name ) :
	m_pcName( name )
{
	m_kElements.reserve( 256 );
}

FV_INLINE 
FvNetInputMessageHandler * FvNetInterfaceMinder::handler( int index )
{
	return m_kElements[ index ].GetHandler();
}

FV_INLINE 
void FvNetInterfaceMinder::handler( int index, FvNetInputMessageHandler * pHandler )
{
	m_kElements[ index ].SetHandler( pHandler );
}

FV_INLINE
const FvNetInterfaceElement & FvNetInterfaceMinder::InterfaceElement( FvUInt8 id ) const
{
	return m_kElements[ id ];
}
