//{future header message}
#ifndef __FvMemoryStream_H__
#define __FvMemoryStream_H__

#include <FvPowerDefines.h>
#include "FvKernel.h"
#include "FvBinaryStream.h"

class FV_KERNEL_API FvMemoryOStream : public FvBinaryOStream, public FvBinaryIStream
{
public: 
	FvMemoryOStream( int iSize = 64 );

	virtual ~FvMemoryOStream();

	virtual void * Reserve( int iBytes );
	virtual int Size() const;

	virtual const void *Retrieve( int iBytes );
	virtual int RemainingLength() const;
	virtual char Peek();

	bool ShouldDelete() const;
	void ShouldDelete( bool bValue );
	void *Data();
	void Reset();
	void Rewind();

protected:

	FvMemoryOStream( const FvMemoryOStream & );
	FvMemoryOStream & operator=( const FvMemoryOStream & );

	char *m_pcBegin;
	char *m_pcCurr;
	char *m_pcEnd;
	char *m_pcRead;
	bool m_bShouldDelete;
};

class FV_KERNEL_API FvMemoryIStream : public FvBinaryIStream
{
public: 
	FvMemoryIStream( const char *pcData, int iLength ) :
		FvBinaryIStream(),
		m_pcCurr( pcData ),
		m_pcEnd( m_pcCurr + iLength )
	{}

	FvMemoryIStream( const void *pcData, int iLength ) :
		m_pcCurr( reinterpret_cast< const char * >( pcData ) ),
		m_pcEnd( m_pcCurr + iLength )
	{}

	virtual ~FvMemoryIStream();

	virtual const void *Retrieve( int iBytes );

	virtual int RemainingLength() const { return (int)(m_pcEnd - m_pcCurr); }

	virtual char Peek();

	virtual void Finish()	{ m_pcCurr = m_pcEnd; }

	inline const void *Data() { return m_pcCurr; }

private:
	const char *m_pcCurr;
	const char *m_pcEnd;
};

class FV_KERNEL_API FvMessageStream : public FvMemoryOStream
{
public:
	FvMessageStream( int iSize = 64 );

	virtual ~FvMessageStream() {}

public: 
	bool AddToStream( FvBinaryOStream &kStream, FvUInt8 uiMessageID );
	bool GetMessage( FvBinaryIStream &kStream,
		int &iMessageID, int &iLength );
};

#include "FvMemoryStream.inl"

#endif // __FvMemoryStream_H__