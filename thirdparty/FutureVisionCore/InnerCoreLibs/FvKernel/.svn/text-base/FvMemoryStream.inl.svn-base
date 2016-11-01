
FV_INLINE 
FvMemoryOStream::FvMemoryOStream( int iSize )
{
	m_pcBegin = new char[ iSize ];
	m_pcCurr = m_pcBegin;
	m_pcEnd = m_pcBegin + iSize;
	m_bShouldDelete = true;
	m_pcRead = m_pcBegin;
}

FV_INLINE 
FvMemoryOStream::~FvMemoryOStream()
{
	if (m_bShouldDelete)
		delete [] m_pcBegin;
}

FV_INLINE
FvMemoryOStream::FvMemoryOStream( const FvMemoryOStream & )
{

}

FV_INLINE
FvMemoryOStream &FvMemoryOStream::operator=( const FvMemoryOStream & )
{
	return (*this);
}

FV_INLINE 
int FvMemoryOStream::Size() const
{
	return (int)(m_pcCurr - m_pcBegin);
}

FV_INLINE 
bool FvMemoryOStream::ShouldDelete() const
{
	return m_bShouldDelete;
}

FV_INLINE 
void FvMemoryOStream::ShouldDelete( bool bValue )
{
	FV_ASSERT( m_bShouldDelete || bValue );
	m_bShouldDelete = bValue;
}

FV_INLINE 
void * FvMemoryOStream::Data()
{
	return m_pcBegin;
}

FV_INLINE 
void * FvMemoryOStream::Reserve( int iBytes )
{
	char * pOldCurr = m_pcCurr;
	m_pcCurr += iBytes;

	if (m_pcCurr > m_pcEnd)
	{
		int multiplier = (int)((m_pcCurr - m_pcBegin)/(m_pcEnd - m_pcBegin) + 1);
		int newSize = multiplier * (int)(m_pcEnd - m_pcBegin);
		char * pNewData = new char[ newSize ];
		memcpy( pNewData, m_pcBegin, pOldCurr - m_pcBegin );
		m_pcCurr = m_pcCurr - m_pcBegin + pNewData;
		pOldCurr = pOldCurr - m_pcBegin + pNewData;
		m_pcRead = m_pcRead - m_pcBegin + pNewData;
		FV_ASSERT( m_bShouldDelete );
		delete [] m_pcBegin;
		m_pcBegin = pNewData;
		m_pcEnd = m_pcBegin + newSize;
	}

	return pOldCurr;
}

FV_INLINE 
const void * FvMemoryOStream::Retrieve( int iBytes )
{
	char * pOldRead = m_pcRead;

	FV_IF_NOT_ASSERT_DEV( m_pcRead + iBytes <= m_pcCurr )
	{
		m_bError = true;

		return iBytes <= int( sizeof( ms_acErrBuf ) ) ? ms_acErrBuf : NULL;
	}

	m_pcRead += iBytes;

	return pOldRead;
}

FV_INLINE 
void FvMemoryOStream::Reset()
{
	m_pcRead = m_pcCurr = m_pcBegin;
}

FV_INLINE 
void FvMemoryOStream::Rewind()
{
	m_pcRead = m_pcBegin;
}

FV_INLINE 
int FvMemoryOStream::RemainingLength() const
{
	return (int)(m_pcCurr - m_pcRead);
}

FV_INLINE 
char FvMemoryOStream::Peek()
{
	FV_IF_NOT_ASSERT_DEV( m_pcRead < m_pcCurr )
	{
		m_bError = true;
		return -1;
	}

	return *(char*)m_pcRead;
}

FV_INLINE 
FvMemoryIStream::~FvMemoryIStream()
{
	if (m_pcCurr != m_pcEnd)
	{
		FvDPrintfNormal( "FvMemoryIStream::~FvMemoryIStream: "
			"There are still %d bytes left\n", (int)(m_pcEnd - m_pcCurr) );
	}
}

FV_INLINE 
const void * FvMemoryIStream::Retrieve( int iBytes )
{
	const char * pOldRead = m_pcCurr;

	if (m_pcCurr + iBytes > m_pcEnd)
	{
		m_bError = true;
		return iBytes <= int( sizeof( ms_acErrBuf  ) ) ? ms_acErrBuf : NULL;
	}

	m_pcCurr += iBytes;
	return pOldRead;
}

FV_INLINE 
char FvMemoryIStream::Peek()
{
	if (m_pcCurr >= m_pcEnd)
	{
		m_bError = true;
		return -1;
	}

	return *m_pcCurr;
}

FV_INLINE 
FvMessageStream::FvMessageStream( int iSize ) : FvMemoryOStream( iSize )
{
}

FV_INLINE
bool FvMessageStream::AddToStream( FvBinaryOStream &kStream, FvUInt8 uiMessageID )
{
	int size = this->Size();

	if (size >= 256)
	{
		return false;
	}

	kStream << uiMessageID << (FvUInt8)size;

	memcpy( kStream.Reserve( size ), this->Retrieve( size ), size );

	this->Reset();

	return true;
}

FV_INLINE 
bool FvMessageStream::GetMessage( FvBinaryIStream &kStream,
		int &iMessageID, int &iLength )
{
	this->Reset();

	FV_IF_NOT_ASSERT_DEV( iLength >= 2 )
	{
		return false;
	}

	FvUInt8 uiSize;
	FvUInt8 uiID;

	kStream >> uiID >> uiSize;
	iLength -= sizeof( uiID ) + sizeof( uiSize );

	iMessageID = uiID;

 	FV_IF_NOT_ASSERT_DEV( iLength >= (int)uiSize )
	{
		return false;
	}

	iLength -= uiSize;

	memcpy( this->Reserve( uiSize ), kStream.Retrieve( uiSize ), uiSize );

	return true;
}