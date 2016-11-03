//{future header message}
#ifndef __FvWatcherPathRequest_H__
#define __FvWatcherPathRequest_H__

#ifndef FV_ENABLE_WATCHERS
//#define FV_ENABLE_WATCHERS 1
#endif // !FV_ENABLE_WATCHERS

#if FV_ENABLE_WATCHERS

#include "FvBinaryStream.h"
#include "FvMemoryStream.h"

class FvWatcherPathRequestNotification;

class FvWatcher;

class FV_KERNEL_API FvWatcherPathRequest
{
public:
	FvWatcherPathRequest( const FvString & path ) :
		m_pkParent( NULL ),
		m_kRequestPath( path )
	{ }

	FvWatcherPathRequest(  ) :
		m_pkParent( NULL )
	{ }

	virtual ~FvWatcherPathRequest() {}

	virtual void FetchWatcherValue() { }

	virtual bool SetWatcherValue() { return false; }

	const FvString &GetPath() const
	{
		return m_kRequestPath;
	}

	void SetParent( FvWatcherPathRequestNotification *parent )
	{
		m_pkParent = parent;
	}

	virtual void NotifyParent( FvInt32 replies=1 );

	virtual void AddWatcherCount( FvInt32 count ) {}

	virtual bool AddWatcherPath( const void *base, const char *path,
								 FvString & label, FvWatcher &watcher )
	{
		return false;
	}

	virtual const char *GetData() { return NULL; }

	virtual FvInt32 GetDataSize() { return 0; }


protected:
	FvWatcherPathRequestNotification *m_pkParent; 

	FvString m_kRequestPath;
};

class FV_KERNEL_API FvWatcherPathRequestV2 : public FvWatcherPathRequest
{
public:

	FvWatcherPathRequestV2( const FvString & path );
	~FvWatcherPathRequestV2();

	bool SetPacketData( FvUInt32 size, const char *data );
	bool SetPacketData( FvBinaryIStream & data );

	void SetSequenceNumber( FvUInt32 seqNum );

	virtual bool SetWatcherValue();
	virtual void FetchWatcherValue();

	const char *GetData();
	FvInt32 GetDataSize();

	virtual void SetResult( const FvString & desc,
							const FvWatcher::Mode & mode,
							const FvWatcher * watcher, const void *base );

	void AddWatcherCount( FvInt32 count );
	bool AddWatcherPath( const void *base, const char *path,
						 FvString & label, FvWatcher & watcher );

	FvMemoryOStream & GetResultStream() { return m_kResult; }
	FvBinaryIStream * GetValueStream() { return m_pkSetStream; }

private:
	FvMemoryOStream m_kResult; 
	FvString m_kOriginalRequestPath; 

	bool m_bHasSeqNum; 

	bool m_bVisitingDirectories; 

	char *m_pcStreamData;
	FvMemoryIStream *m_pkSetStream;

	FvWatcherPathRequestNotification *m_pkParent; 
};

class FV_KERNEL_API FvWatcherPathRequestV1 : public FvWatcherPathRequest, public FvWatcherVisitor
{
public:

	FvWatcherPathRequestV1( const FvString & path );

	void SetValueData( const char *valueStr );
	void UseDescription( bool shouldUse ) { m_bUseDescription = shouldUse; }

	virtual bool SetWatcherValue();
	virtual void FetchWatcherValue();

	const char *GetData();
	FvInt32 GetDataSize();

	bool Visit(  FvWatcher::Mode /*mode*/,
		const FvString & label,
		const FvString & desc,
		const FvString & valueStr );

private:
	FvMemoryOStream m_kResultStream; 
	FvString m_kSetValue; 
	FvInt32 m_iContainedReplies;
	bool m_bUseDescription; 
};

class FV_KERNEL_API FvWatcherPathRequestWatcherVisitor : public FvWatcherPathRequest
{
public:

	FvWatcherPathRequestWatcherVisitor( FvWatcherVisitor & visitor ) :
		m_pkVisitor( &visitor )
	{
	}


	bool AddWatcherPath( const void *base, const char *path, FvString & label,
			FvWatcher &watcher )
	{
		FvString valueStr;
		FvString desc;
		FvWatcher::Mode resMode;

		watcher.GetAsString( base, path, valueStr, desc, resMode );

		return (m_pkVisitor->Visit( resMode, label, desc, valueStr ));
	}

private:
	FvWatcherVisitor *m_pkVisitor;

	FvString m_kResult;
	FvString m_kDesc;
	FvWatcher::Mode m_eMode;

};

class FV_KERNEL_API FvWatcherPathRequestNotification : public FvReferenceCount
{
public:

	virtual void NotifyComplete( FvWatcherPathRequest & pathRequest, FvInt32 count ) = 0;

	virtual FvWatcherPathRequest *NewRequest( FvString & path ) = 0;
};

#endif // FV_ENABLE_WATCHERS

#endif // __FvWatcherPathRequest_H__