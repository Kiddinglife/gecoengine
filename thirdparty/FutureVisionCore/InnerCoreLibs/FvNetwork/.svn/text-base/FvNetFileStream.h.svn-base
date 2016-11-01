//{future header message}
#ifndef __FvNetFileStream_H__
#define __FvNetFileStream_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>
#include <list>

#include "FvNetwork.h"
#include <FvBaseTypes.h>
#include <FvMemoryStream.h>

class FV_NETWORK_API FvNetFileStream : public FvMemoryOStream
{
public:
	static const int INIT_READ_BUF_SIZE = 128;

	FvNetFileStream( const char *path, const char *mode );
	virtual ~FvNetFileStream();

	inline bool Good() const { return !m_bError; }
	inline bool Error() const { return m_bError; }
	const char *StrError() const;

	long Tell();
	int Seek( long offset, int whence = SEEK_SET );
	long Length();
	bool Commit();
	virtual const void *Retrieve( int nBytes );
	int Stat( struct stat *statinfo );

protected:
	void SetMode( char mode );
	bool Open();
	void Close();
	void Remove();

	FvString m_kPath;
	FvString m_kMode;
	FILE *m_pkFile;
	char *m_pcReadBuf;
	int m_iReadBufSize;
	FvString m_kErrorMsg;

	char m_cLastAction;

	bool m_bOpen;

	long m_iOffset;

	typedef std::list< FvNetFileStream* > FileStreams;
	static FileStreams ms_kOpenFiles;
	static const unsigned MAX_OPEN_FILES = 20;

	FileStreams::iterator it_;
};

#endif // __FvNetFileStream_H__
