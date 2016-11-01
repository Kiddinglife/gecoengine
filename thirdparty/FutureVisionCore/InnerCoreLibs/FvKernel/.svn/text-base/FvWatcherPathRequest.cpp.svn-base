#ifndef FV_ENABLE_WATCHERS
//#define FV_ENABLE_WATCHERS 1
#endif // !FV_ENABLE_WATCHERS

#if FV_ENABLE_WATCHERS

#include "FvWatcher.h"
#include "FvWatcherPathRequest.h"
#include "FvMemoryStream.h"

void FvWatcherPathRequest::NotifyParent( FvInt32 replies )
{
	if (m_pkParent != NULL)
	{
		m_pkParent->NotifyComplete( *this, replies );
	}
}

FvWatcherPathRequestV2::FvWatcherPathRequestV2( const FvString & path ) :
	FvWatcherPathRequest( path ),
	m_bHasSeqNum( false ),
	m_bVisitingDirectories( false ),
	m_pcStreamData( NULL ),
	m_pkSetStream( NULL )
{ }

FvWatcherPathRequestV2::~FvWatcherPathRequestV2()
{
	if (m_pcStreamData)
		delete [] m_pcStreamData;

	if (m_pkSetStream)
		delete m_pkSetStream;
}

void FvWatcherPathRequestV2::SetResult( const FvString & desc, 
		const FvWatcher::Mode & mode, const FvWatcher * watcher, const void *base )
{
	if (mode == FvWatcher::WT_DIRECTORY)
	{
		if (!m_bVisitingDirectories)
		{
			m_bVisitingDirectories = true;
			m_kOriginalRequestPath = m_kRequestPath;
			((FvWatcher *)watcher)->VisitChildren( base, NULL, *this );
			m_kRequestPath = m_kOriginalRequestPath;
			m_bVisitingDirectories = false;

			this->NotifyParent();
		}
	}
	else if (mode != FvWatcher::WT_INVALID)
	{
		if (m_pkSetStream)
		{
			m_kResult << true;
		}

		if (!m_bVisitingDirectories)
		{
			this->NotifyParent();
		}
	}

	return;
}

bool FvWatcherPathRequestV2::SetWatcherValue()
{
	if (!m_pkSetStream)
		return false;

	bool status = FvWatcher::RootWatcher().SetFromStream( NULL,
				m_kRequestPath.c_str(), *this );

	if (!status)
	{
		if ((FvUInt32)m_kResult.Size() <= sizeof(FvInt32))
		{
			m_kResult << (unsigned char)FV_WATCHER_TYPE_UNKNOWN;
			m_kResult << (unsigned char)FvWatcher::WT_READ_ONLY;
			m_kResult.WriteStringLength( 0 );
			m_kResult << status;
		}

		this->NotifyParent();
	}

	return true;
}

void FvWatcherPathRequestV2::FetchWatcherValue()
{
	if (!FvWatcher::RootWatcher().GetAsStream( NULL,
									m_kRequestPath.c_str(), *this ))
	{
		m_kResult << (unsigned char)FV_WATCHER_TYPE_UNKNOWN;
		m_kResult << (unsigned char)FvWatcher::WT_READ_ONLY;
		m_kResult.WriteStringLength( 0 );

		this->NotifyParent();
	}
}

bool FvWatcherPathRequestV2::AddWatcherPath( const void *base, const char *path,
					 FvString & label, FvWatcher & watcher )
{
	FvString desc;

	if (m_kOriginalRequestPath.size())
		m_kRequestPath = m_kOriginalRequestPath + "/" + label;
	else
		m_kRequestPath = label;

	bool status = watcher.GetAsStream( base, NULL, *this );
	if (!status)
	{
		m_kResult << (unsigned char)FV_WATCHER_TYPE_UNKNOWN;
		m_kResult << (unsigned char)FvWatcher::WT_READ_ONLY;
		m_kResult.WriteStringLength( 0 );
	}

	m_kResult << label;

	return true;
}

bool FvWatcherPathRequestV2::SetPacketData( FvUInt32 size, const char *data )
{
	if ((m_pcStreamData != NULL) || (m_pkSetStream != NULL))
		return false;

	m_pcStreamData = new char[size];
	if (!m_pcStreamData)
		return false;

	memcpy( m_pcStreamData, data, size );

	m_pkSetStream = new FvMemoryIStream( m_pcStreamData, (FvInt32)size );
	if (!m_pkSetStream)
	{
		delete [] m_pcStreamData;
		m_pcStreamData = NULL;
		return false;
	}

	return true;
}

bool FvWatcherPathRequestV2::SetPacketData( FvBinaryIStream & data )
{
	if ((m_pcStreamData != NULL) || (m_pkSetStream != NULL))
		return false;

	int size = data.RemainingLength();
	m_pkSetStream = new FvMemoryIStream( data.Retrieve( size ) , size );
	if (!m_pkSetStream)
	{
		return false;
	}

	return true;
}

void FvWatcherPathRequestV2::AddWatcherCount( FvInt32 count )
{
	m_kResult << count;
}

void FvWatcherPathRequestV2::SetSequenceNumber( FvUInt32 seqNum )
{
	if (m_bHasSeqNum)
		return;

	m_bHasSeqNum = true;
	m_kResult << seqNum;
}

const char *FvWatcherPathRequestV2::GetData()
{
	return (const char *)m_kResult.Data();
}

FvInt32 FvWatcherPathRequestV2::GetDataSize()
{
	return m_kResult.Size();
}

FvWatcherPathRequestV1::FvWatcherPathRequestV1( const FvString & path ) :
	FvWatcherPathRequest( path ),
	m_iContainedReplies( 1 ),
	m_bUseDescription( false )
{ }

void FvWatcherPathRequestV1::SetValueData( const char *valueStr )
{
	m_kSetValue = valueStr;
}

bool FvWatcherPathRequestV1::SetWatcherValue()
{
	if (!FvWatcher::RootWatcher().SetFromString( NULL, m_kRequestPath.c_str(),
			m_kSetValue.c_str() ) )
	{
		this->NotifyParent( 0 );
		return false;
	}

	this->FetchWatcherValue();

	return true;
}

void FvWatcherPathRequestV1::FetchWatcherValue()
{
	FvString desc;
	FvWatcher::Mode mode;
	FvString result;

	if (!FvWatcher::RootWatcher().GetAsString( NULL, m_kRequestPath.c_str(), 
											desc, result, mode ))
	{
		return;
	}

	if (mode == FvWatcher::WT_DIRECTORY)
	{
		m_iContainedReplies = 0;
		FvWatcherVisitor *visitor = (FvWatcherVisitor *)this;

		FvWatcher::RootWatcher().VisitChildren( NULL, m_kRequestPath.c_str(), 
				*visitor );

		this->NotifyParent( m_iContainedReplies );
	}
	else if (mode != FvWatcher::WT_INVALID)
	{
		m_kResultStream.AddBlob( m_kRequestPath.c_str(), 
										m_kRequestPath.size() + 1 );
		m_kResultStream.AddBlob( result.c_str(), result.size() + 1 );
		if (m_bUseDescription)
			m_kResultStream.AddBlob( desc.c_str(), desc.size() + 1 );

		this->NotifyParent();
	}

	return;
}

const char *FvWatcherPathRequestV1::GetData()
{
	return (const char *)m_kResultStream.Data();
}

FvInt32 FvWatcherPathRequestV1::GetDataSize()
{
	return m_kResultStream.Size();
}

bool FvWatcherPathRequestV1::Visit( FvWatcher::Mode /*mode*/,
		const FvString & label,
		const FvString & desc,
		const FvString & valueStr )
{
	FvString path;

	if (m_kRequestPath.size())
	{
		path = m_kRequestPath + "/" + label;
	}
	else
	{
		path = label;
	}
	m_kResultStream.AddBlob( path.c_str(), path.size() + 1 );
	m_kResultStream.AddBlob( valueStr.c_str(), valueStr.size() + 1 );
	if (m_bUseDescription)
		m_kResultStream.AddBlob( desc.c_str(), desc.size() + 1 );
	m_iContainedReplies++;
	return true;
}

#endif // FV_ENABLE_WATCHERS
