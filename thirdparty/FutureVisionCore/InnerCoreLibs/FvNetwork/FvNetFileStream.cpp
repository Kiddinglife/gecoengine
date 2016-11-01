#include "FvNetFileStream.h"

#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT2( "Network", 0 )

FvNetFileStream::FvNetFileStream( const char *path, const char *mode ) :
	m_kPath( path ),
	m_kMode( mode ),
	m_pcReadBuf( new char[ INIT_READ_BUF_SIZE ] ),
	m_iReadBufSize( INIT_READ_BUF_SIZE ),
	m_bOpen( false ),
	m_iOffset( 0 ),
	it_( ms_kOpenFiles.end() )
{
	this->Open();
}

FvNetFileStream::~FvNetFileStream()
{
	this->Close();
	delete [] m_pcReadBuf;
}

const char * FvNetFileStream::StrError() const
{
	if (errno)
		return ::strerror( errno );
	else
		return m_kErrorMsg.c_str();
}

long FvNetFileStream::Tell()
{
	if (!this->Open())
		return -1;

	long result = ftell( m_pkFile );
	if (result == -1)
		m_bError = true;

	return result;
}

int FvNetFileStream::Seek( long offset, int whence )
{
	if (!this->Open())
		return -1;

	int result = fseek( m_pkFile, offset, whence );
	if (result == -1)
		m_bError = true;

	return result;
}

long FvNetFileStream::Length()
{
	if (!this->Open())
		return -1;

	struct stat statinfo;
	if (fstat( FvFileNo( m_pkFile ), &statinfo ))
	{
		m_bError = true;
		return -1;
	}
	else
		return statinfo.st_size;
}

bool FvNetFileStream::Commit()
{
	this->SetMode( 'w' );
	if (!this->Open())
		return false;

	if (fwrite( this->Data(), 1, this->Size(), m_pkFile ) !=
		(size_t)this->Size())
	{
		m_bError = true;
		m_kErrorMsg = "Couldn't write all bytes to disk";
		return false;
	}

	if (fflush( m_pkFile ) == EOF)
	{
		m_bError = true;
		return false;
	}

	this->Reset();
	return true;
}


const void * FvNetFileStream::Retrieve( int nBytes )
{
	this->SetMode( 'r' );

	if (m_iReadBufSize < nBytes)
	{
		delete [] m_pcReadBuf;
		m_pcReadBuf = new char[ nBytes ];
		m_iReadBufSize = nBytes;
	}

	if (!this->Open())
		return m_pcReadBuf;

	if (fread( m_pcReadBuf, 1, nBytes, m_pkFile ) != (size_t)nBytes)
	{
		m_bError = true;
		m_kErrorMsg = "Couldn't read desired number of bytes from disk";
	}

	return m_pcReadBuf;
}

int FvNetFileStream::Stat( struct stat *statinfo )
{
	if (!this->Open())
		return -1;

	return fstat( FvFileNo( m_pkFile ), statinfo );
}

void FvNetFileStream::SetMode( char mode )
{
	if (!this->Open())
		return;

	if ((mode == 'w' && m_cLastAction == 'r') ||
		 (mode == 'r' && m_cLastAction == 'w'))
	{
		m_bError |= fseek( m_pkFile, 0, SEEK_CUR ) == -1;
	}

	m_cLastAction = mode;
}

bool FvNetFileStream::Open()
{
	if (!ms_kOpenFiles.empty() && ms_kOpenFiles.front() == this)
		return true;
	else if (m_bOpen)
	{
		this->Remove();
	}
	else
	{
		if ((m_pkFile = fopen( m_kPath.c_str(), m_kMode.c_str() )) == NULL)
		{
			m_bError = true;
			return false;
		}

		m_cLastAction = 0;
		m_bOpen = true;

		if (m_iOffset)
		{
			if (fseek( m_pkFile, m_iOffset, SEEK_SET ) == -1)
			{
				m_bError = true;
				return false;
			}
		}
	}

	ms_kOpenFiles.push_front( this );
	it_ = ms_kOpenFiles.begin();

	if (ms_kOpenFiles.size() > MAX_OPEN_FILES)
	{
		ms_kOpenFiles.back()->Close();
	}

	return true;
}

void FvNetFileStream::Close()
{
	if (!m_bOpen)
		return;

	if (m_pcCurr != m_pcBegin)
		this->Commit();

	m_iOffset = ftell( m_pkFile );
	fclose( m_pkFile );
	m_bOpen = false;

	this->Remove();
}

void FvNetFileStream::Remove()
{
	ms_kOpenFiles.erase( it_ );
	it_ = ms_kOpenFiles.end();
}

FvNetFileStream::FileStreams FvNetFileStream::ms_kOpenFiles;
