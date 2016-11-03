//{future header message}
#ifndef __FvWatcher_H__
#define __FvWatcher_H__

#include "FvKernel.h"
#include "FvBinaryStream.h"
#include "FvSmartPointer.h"

class FvWatcher;
class FvWatcherPathRequest;
class FvWatcherPathRequestV2;
class FvWatcherPathRequestV1;
class FvDirectoryWatcher;
typedef FvSmartPointer<FvWatcher> FvWatcherPtr;
typedef FvSmartPointer<FvDirectoryWatcher> FvDirectoryWatcherPtr;

#ifndef FV_ENABLE_WATCHERS
//#define FV_ENABLE_WATCHERS 1
#endif // !FV_ENABLE_WATCHERS

#if FV_ENABLE_WATCHERS

enum FvWatcherDataType 
{
	FV_WATCHER_TYPE_UNKNOWN = 0,
	FV_WATCHER_TYPE_INT,
	FV_WATCHER_TYPE_UINT,
	FV_WATCHER_TYPE_FLOAT,
	FV_WATCHER_TYPE_BOOL,
	FV_WATCHER_TYPE_STRING,	
	FV_WATCHER_TYPE_TUPLE,
	FV_WATCHER_TYPE_TYPE
};

#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>

#include "FvBaseTypes.h"
#include "FvBinaryStream.h"

const char FV_WATCHER_SEPARATOR = '/';

class FvWatcherVisitor;

template <class VALUE_TYPE>
bool WatcherStringToValue( const char * valueStr, VALUE_TYPE &value )
{
	std::stringstream stream;
	stream.write( valueStr, std::streamsize(strlen( valueStr )) );
	stream.seekg( 0, std::ios::beg );
	stream >> value;

	return !stream.bad();
}

FV_INLINE
bool WatcherStringToValue( const char * valueStr, const char *& value )
{
	return false;
}

FV_INLINE
bool WatcherStringToValue( const char * valueStr, FvString & value )
{
	value = valueStr;

	return true;
}

FV_INLINE
bool WatcherStringToValue( const char * valueStr, bool & value )
{
	if (FvStriCMP( valueStr, "true" ) == 0)
	{
		value = true;
	}
	else if (FvStriCMP( valueStr, "false" ) == 0)
	{
		value = false;
	}
	else
	{
		std::stringstream stream;
		stream.write( valueStr, std::streamsize( strlen( valueStr )));
		stream.seekg( 0, std::ios::beg );
		stream >> value;

		return !stream.bad();
	}

	return true;
}

bool WatcherStreamToValueType( FvBinaryIStream & stream, FvString & value,
								FvWatcherDataType type );

template <class VALUE_TYPE>
bool WatcherStreamToStringToValue( FvBinaryIStream & stream, VALUE_TYPE &value,
								   const FvWatcherDataType type )
{
	if (type != FV_WATCHER_TYPE_STRING)
	{
		int size;
		size = stream.ReadStringLength();

		if (stream.Error())
			return false;

		stream.Retrieve( size );
		return false;
	}

	FvString extractedValue;
	if (!WatcherStreamToValueType(stream, extractedValue, FV_WATCHER_TYPE_STRING))
	{
		return false;
	}

	return WatcherStringToValue( extractedValue.c_str(), value );
}

template <class VALUE_TYPE>
bool WatcherStreamToValue( FvBinaryIStream & stream, VALUE_TYPE &value )
{
#ifndef _XBOX360
	char type;
	stream >> type;

	if (type == FV_WATCHER_TYPE_STRING)
	{
		return WatcherStreamToStringToValue( stream, value,
											(FvWatcherDataType)type );
	}

	int size;
	size = stream.ReadStringLength();
	if (stream.Error())
	{
		return false;
	}

	int sizeDiff = stream.RemainingLength();
	stream >> value;
	sizeDiff -= stream.RemainingLength();

	if ((stream.Error()) || (sizeDiff != size))
	{
		return false;
	}
	else
	{
		return true;
	}
#else
	return false;
#endif
}

FV_INLINE
bool WatcherStreamToValueType( FvBinaryIStream & stream, bool & value,
							FvWatcherDataType type )
{
	if (type != FV_WATCHER_TYPE_BOOL)
	{
		return WatcherStreamToStringToValue( stream, value, type );
	}

	int size;
	size = stream.ReadStringLength();

	if (size != sizeof(bool))
	{
		stream.Retrieve( size );
		return false;
	}

	stream >> value;
	return !stream.Error();
}

FV_INLINE
bool WatcherStreamToValue( FvBinaryIStream & stream, bool & value )
{
	char type;
	stream >> type;

	if (stream.Error())
	{
		return false;
	}

	return WatcherStreamToValueType( stream, value, (FvWatcherDataType)type );
}

FV_INLINE
bool WatcherStreamToValueType( FvBinaryIStream & stream, FvInt32 & value,
							FvWatcherDataType type )
{
	if (type != FV_WATCHER_TYPE_INT)
	{
		return WatcherStreamToStringToValue( stream, value, type );
	}

	int size;
	size = stream.ReadStringLength();

	if (size != sizeof(FvInt32))
	{
		if (size != sizeof(FvInt64))
		{
			stream.Retrieve( size );
			return false;
		}

		FvInt64 tmpVal;
		stream >> tmpVal;
		value = (int)tmpVal;
	}
	else
	{
		stream >> value;
	}

	return !stream.Error();
}

FV_INLINE
bool WatcherStreamToValueType( FvBinaryIStream & stream, FvInt64 & value,
							FvWatcherDataType type )
{
	if (type != FV_WATCHER_TYPE_INT)
	{
		return WatcherStreamToStringToValue( stream, value, type );
	}

	int size;
	size = stream.ReadStringLength();

	if (size != sizeof(FvInt64))
	{
		if (size != sizeof(FvInt32))
		{
			stream.Retrieve( size );
			return false;
		}

		FvInt32 tmpVal;
		stream >> tmpVal;
		value = (FvInt64)tmpVal;
	}
	else
	{
		stream >> value;
	}

	return !stream.Error();
}


FV_INLINE
bool WatcherStreamToValue( FvBinaryIStream & stream, int & value )
{
	char type;
	stream >> type;

	if (stream.Error())
	{
		return false;
	}

	return WatcherStreamToValueType( stream, (FvInt32&)value,
								(FvWatcherDataType)type );
}


FV_INLINE
bool WatcherStreamToValue( FvBinaryIStream & stream, long & value )
{
	char type;
	stream >> type;

	if (stream.Error())
	{
		return false;
	}

	return WatcherStreamToValueType( stream, (FvInt32&)value,
								(FvWatcherDataType)type );
}

FV_INLINE
bool WatcherStreamToValueType( FvBinaryIStream & stream, FvUInt32 & value,
							FvWatcherDataType type )
{
	if (type != FV_WATCHER_TYPE_UINT)
	{
		return WatcherStreamToStringToValue( stream, value, type );
	}

	int size;
	size = stream.ReadStringLength();

	if (size != sizeof(FvUInt32))
	{
		stream.Retrieve( size );
		return false;
	}

	stream >> value;
	return !stream.Error();
}

FV_INLINE
bool WatcherStreamToValueType( FvBinaryIStream & stream, FvUInt64 & value,
							FvWatcherDataType type )
{
	if (type != FV_WATCHER_TYPE_UINT)
	{
		return WatcherStreamToStringToValue( stream, value, type );
	}

	int size;
	size = stream.ReadStringLength();

	if (size != sizeof(FvUInt64))
	{
		if (size != sizeof(FvUInt32))
		{
			stream.Retrieve( size );
			return false;
		}

		FvUInt32 tmpVal;
		stream >> tmpVal;
		value = (FvUInt64)tmpVal;
	}

	stream >> value;
	return !stream.Error();
}


FV_INLINE
bool WatcherStreamToValue( FvBinaryIStream & stream, unsigned int & value )
{
	char type;
	stream >> type;

	if (stream.Error())
	{
		return false;
	}

	return WatcherStreamToValueType( stream, (FvUInt32&)value,
								(FvWatcherDataType)type );
}

FV_INLINE
bool WatcherStreamToValueType( FvBinaryIStream & stream, float & value,
							FvWatcherDataType type )
{
	if (type != FV_WATCHER_TYPE_FLOAT)
	{
		return WatcherStreamToStringToValue( stream, value, type );
	}

	int size;
	size = stream.ReadStringLength();

	if (size != sizeof(float))
	{
		if (size != sizeof(double))
		{
			stream.Retrieve( size );
			return false;
		}

		double tmpVal;
		stream >> tmpVal;
		value = (float)tmpVal;
	}
	else
	{
		stream >> value;
	}

	return !stream.Error();
}

FV_INLINE
bool WatcherStreamToValueType( FvBinaryIStream & stream, double & value,
							FvWatcherDataType type )
{
	if (type != FV_WATCHER_TYPE_FLOAT)
	{
		return WatcherStreamToStringToValue( stream, value, type );
	}

	int size;
	size = stream.ReadStringLength();

	if (size != sizeof(double))
	{
		if (size != sizeof(float))
		{
			stream.Retrieve( size );
			return false;
		}

		float tmpVal;
		stream >> tmpVal;
		value = (double)tmpVal;
	}
	else
	{
		stream >> value;
	}

	return !stream.Error();
}

FV_INLINE
bool WatcherStreamToValue( FvBinaryIStream & stream, float & value )
{
	char type;
	stream >> type;

	if (stream.Error())
	{
		return false;
	}

	return WatcherStreamToValueType( stream, value, (FvWatcherDataType)type );
}

FV_INLINE
bool WatcherStreamToValue( FvBinaryIStream & stream, double & value )
{
	char type;
	stream >> type;

	if (stream.Error())
	{
		return false;
	}

	return WatcherStreamToValueType( stream, value, (FvWatcherDataType)type );
}

FV_INLINE
bool WatcherStreamToValueType( FvBinaryIStream & stream, FvString & value,
							FvWatcherDataType type )
{
	if (type != FV_WATCHER_TYPE_STRING)
	{
		return WatcherStreamToStringToValue( stream, value, type );
	}

	stream >> value;
	return !stream.Error();
}

FV_INLINE
bool WatcherStreamToValue( FvBinaryIStream & stream, FvString & value )
{
	char type;
	stream >> type;

	if (stream.Error())
	{
		return false;
	}

	return WatcherStreamToValueType( stream, value, (FvWatcherDataType)type );
}

template <class VALUE_TYPE>
FvString WatcherValueToString( const VALUE_TYPE & value )
{
	std::stringstream stream;
	stream << value;

	return stream.str();
}

FV_INLINE
FvString WatcherValueToString( const FvString & value )
{
	return value;
}

FV_INLINE
FvString WatcherValueToString( bool value )
{
	return value ? "true" : "false";
}

class FV_KERNEL_API FvWatcher : public FvSafeReferenceCount
{
public:

	enum Mode
	{
		WT_INVALID,			
		WT_READ_ONLY,		
		WT_READ_WRITE,		
		WT_DIRECTORY,		
		WT_CALLABLE			
	};

	FvWatcher( const char * comment = "" );
	virtual ~FvWatcher();

	virtual bool GetAsString( const void * base,
		const char * path,
		FvString & result,
		FvString & desc,
		Mode & mode ) const = 0;

	virtual bool GetAsString( const void * base, const char * path,
		FvString & result, Mode & mode ) const
	{
		FvString desc;
		return this->GetAsString( base, path, result, desc, mode );
	}

	virtual bool SetFromString( void * base,
		const char * path,
		const char * valueStr )	= 0;

	virtual bool SetFromStream( void * base,
		const char * path,
		FvWatcherPathRequestV2 & pathRequest ) = 0;

	virtual bool VisitChildren( const void * base, const char *path,
			FvWatcherPathRequest & pathRequest ) { return false; }

	bool VisitChildren( const void * base,
		const char * path,
		FvWatcherVisitor & visitor );

	virtual bool GetAsStream( const void * base,
		const char * path, FvWatcherPathRequestV2 & pathRequest ) const = 0;

	virtual bool AddChild( const char * path, FvWatcherPtr pChild,
		void * withBase = NULL )
		{ return false; }

	virtual bool RemoveChild( const char * path )
		{	return false;	}

	void SetComment( const FvString & comment = FvString( "" ) )
   				{ m_kComment = comment; }

	const FvString & GetComment() { return m_kComment; }

	static FvWatcher & RootWatcher();

	static void PartitionPath( const FvString path,
			FvString & resultingName,
			FvString & resultingDirectory );

	static void Fini();

protected:

	static bool IsEmptyPath( const char * path )
		{ return (path == NULL) || (*path == '\0'); }

	static bool IsDocPath( const char * path )
	{
		if (path == NULL)
			return false;

		return (strcmp(path, "__doc__") == 0);
	}

	FvString m_kComment;
};

template <class ValueType>
void WatcherValueToStream( FvBinaryOStream & result, const ValueType & value,
						   const FvWatcher::Mode & mode )
{
	result << (unsigned char)FV_WATCHER_TYPE_STRING;
	result << (unsigned char)mode;
	result << WatcherValueToString( value );
}

FV_INLINE
void WatcherValueToStream( FvBinaryOStream & result, FvWatcherDataType value,
						   const FvWatcher::Mode & mode )
{
	result << (unsigned char)FV_WATCHER_TYPE_TYPE;
	result << (unsigned char)mode;
	result.WriteStringLength( sizeof(unsigned char) );
	result << (unsigned char)value;
}

FV_INLINE
void WatcherValueToStream( FvBinaryOStream & result, const int & value,
						   const FvWatcher::Mode & mode )
{
	result << (unsigned char)FV_WATCHER_TYPE_INT;
	result << (unsigned char)mode;
	result.WriteStringLength( sizeof(int) );
	result << value;
}

FV_INLINE
void WatcherValueToStream( FvBinaryOStream & result, const unsigned int & value,
						   const FvWatcher::Mode & mode )
{
	result << (unsigned char)FV_WATCHER_TYPE_UINT;
	result << (unsigned char)mode;
	result.WriteStringLength( sizeof(unsigned int) );
	result << value;
}

FV_INLINE
void WatcherValueToStream( FvBinaryOStream & result, const float & value,
						   const FvWatcher::Mode & mode )
{
	result << (unsigned char)FV_WATCHER_TYPE_FLOAT;
	result << (unsigned char)mode;
	result.WriteStringLength( sizeof(float) );
	result << value;
}

FV_INLINE
void WatcherValueToStream( FvBinaryOStream & result, const double & value,
						   const FvWatcher::Mode & mode )
{
	result << (unsigned char)FV_WATCHER_TYPE_FLOAT;
	result << (unsigned char)mode;
	result.WriteStringLength( sizeof(double) );
	result << value;
}

FV_INLINE
void WatcherValueToStream( FvBinaryOStream & result, const bool & value,
						   const FvWatcher::Mode & mode )
{
	result << (unsigned char)FV_WATCHER_TYPE_BOOL;
	result << (unsigned char)mode;
	result.WriteStringLength( sizeof(bool) );
	result << value;
}


FV_INLINE
void WatcherValueToStream( FvBinaryOStream & result, const FvInt64 value,
						   const FvWatcher::Mode & mode )
{
	result << (unsigned char)FV_WATCHER_TYPE_INT;
	result << (unsigned char)mode;
	result.WriteStringLength( sizeof(FvInt64) );
	result << value;
}

class FV_KERNEL_API FvWatcherVisitor
{
public:

	virtual ~FvWatcherVisitor() {};

	virtual bool Visit( FvWatcher::Mode mode,
		const FvString & label,
		const FvString & desc,
		const FvString & valueStr ) = 0;

	virtual bool Visit( FvWatcher::Mode mode,
		const FvString & label,
		const FvString & valueStr )
	{
		FvString desc;
		return this->Visit( mode, label, desc, valueStr );
	}
};

class FV_KERNEL_API FvDirectoryWatcher : public FvWatcher
{
public:
	FvDirectoryWatcher();
	virtual ~FvDirectoryWatcher();

	virtual bool GetAsString( const void * base, const char * path,
		FvString & result, FvString & desc, FvWatcher::Mode & mode ) const;

	virtual bool GetAsStream( const void * base, const char * path,
		FvWatcherPathRequestV2 & pathRequest ) const;

	virtual bool SetFromString( void * base, const char * path,
		const char * valueStr );

	virtual bool SetFromStream( void * base, const char * path,
		FvWatcherPathRequestV2 & pathRequest );

	virtual bool VisitChildren( const void * base, const char * path,
		FvWatcherPathRequest & pathRequest );

	virtual bool AddChild( const char * path, FvWatcherPtr pChild,
		void * withBase = NULL );

	virtual bool RemoveChild( const char * path );

	static const char * Tail( const char * path );

private:
	struct DirData
	{
		FvWatcherPtr m_spWatcher;
		void *m_pkBase;
		FvString m_kLabel;
	};

	DirData * FindChild( const char * path ) const;

	typedef std::vector<DirData> Container;
	Container m_kContainer;
};


#include "FvWatcherPathRequest.h"

template <class SEQ> 
class FvSequenceWatcher : public FvWatcher
{
public:
	typedef typename SEQ::value_type SEQ_value;
#if defined(__GNUC__) && __GNUC__ >= 3 && __GNUC_MINOR__ >= 4
	typedef typename SEQ::iterator::reference SEQ_reference;
#else // (__GNUC__ && __GNUC__ >= 3 && __GNUC_MINOR__ >= 4)
	typedef typename SEQ::reference SEQ_reference;
#endif // ! (__GNUC__ && __GNUC__ >= 3 && __GNUC_MINOR__ >= 4)
	typedef typename SEQ::iterator SEQ_iterator;
	typedef typename SEQ::const_iterator SEQ_constiterator;
	typedef FvString (*SEQ_indexToString)( const void * base, int index );
	typedef int (*SEQ_stringToIndex)( const void * base,
									  const FvString & name );
public:

	FvSequenceWatcher( SEQ & toWatch = *(SEQ*)NULL,
			const char ** labels = NULL ) :
		toWatch_( toWatch ),
		labels_( labels ),
		labelsub_( NULL ),
		child_( NULL ),
		subBase_( 0 ),
		indexToString_( NULL ),
		stringToIndex_( NULL )
	{
	}

	virtual bool GetAsString( const void * base, const char * path,
		FvString & result, FvString & desc, FvWatcher::Mode & mode ) const
	{
		if (IsEmptyPath(path))
		{
			result = "<DIR>";
			mode = WT_DIRECTORY;
			desc = m_kComment;
			return true;
		}

		try
		{
			SEQ_reference rChild = this->FindChild( base, path );

			return child_->GetAsString( (void*)(subBase_ + (FvUIntPtr)&rChild),
				this->tail( path ), result, desc, mode );
		}
		catch (NoSuchChild &)
		{
			return false;
		}
	}

	virtual bool SetFromString( void * base, const char * path,
		const char * valueStr )
	{
		try
		{
			SEQ_reference rChild = this->FindChild( base, path );

			return child_->SetFromString( (void*)(subBase_ + (FvUIntPtr)&rChild),
				this->tail( path ), valueStr );
		}
		catch (NoSuchChild &)
		{
			return false;
		}
	}

	virtual bool GetAsStream( const void * base, const char * path,
		FvWatcherPathRequestV2 & pathRequest ) const
	{
		if (IsEmptyPath(path))
		{
			FvWatcher::Mode mode = WT_DIRECTORY;
			WatcherValueToStream( pathRequest.GetResultStream(), "<DIR>", mode );
			pathRequest.SetResult( m_kComment, mode, this, base );
			return true;
		}
		else if (IsDocPath( path ))
		{
			FvWatcher::Mode mode = FvWatcher::WT_READ_ONLY;
			WatcherValueToStream( pathRequest.GetResultStream(), m_kComment, mode );
			pathRequest.SetResult( m_kComment, mode, this, base );
			return true;
		}

		try
		{
			SEQ_reference rChild = this->FindChild( base, path );

			return child_->GetAsStream( (void*)(subBase_ + (FvUIntPtr)&rChild),
				this->tail( path ), pathRequest );
		}
		catch (NoSuchChild &)
		{
			return false;
		}
	}

	virtual bool SetFromStream( void * base, const char * path,
		FvWatcherPathRequestV2 & pathRequest )
	{
		try
		{
			SEQ_reference rChild = this->FindChild( base, path );

			return child_->SetFromStream( (void*)(subBase_ + (FvUIntPtr)&rChild),
				this->tail( path ), pathRequest );
		}
		catch (NoSuchChild &)
		{
			return false;
		}
	}

	virtual bool VisitChildren( const void * base, const char * path,
		FvWatcherPathRequest & pathRequest )
	{
		if (IsEmptyPath(path))
		{
			SEQ	& useVector = *(SEQ*)(
				((FvUIntPtr)&toWatch_) + ((FvUIntPtr)base) );

			const char ** ppLabel = labels_;

			pathRequest.AddWatcherCount( useVector.size() );

			int count = 0;
			for( SEQ_iterator iter = useVector.begin();
				iter != useVector.end();
				iter++, count++ )
			{
				FvString	callLabel;
				FvString desc;

				SEQ_reference rChild = *iter;

				if ((ppLabel != NULL) && (*ppLabel != NULL))
				{
					callLabel.assign( *ppLabel );
					ppLabel++;
				}
				else if (labelsub_)
				{
					FvWatcher::Mode unused;
					child_->GetAsString( (void*)(subBase_ + (FvUIntPtr)&rChild),
						labelsub_, callLabel, desc, unused );
				}
				else if (indexToString_)
				{
					callLabel.assign( indexToString_( base, count ) );
				}
				if (callLabel.empty())
				{
					char temp[32];
					FvSNPrintf( temp, sizeof(temp), "%u", count );
					callLabel.assign( temp );
				}

				if (!pathRequest.AddWatcherPath( (void*)(subBase_ + (FvUIntPtr)&rChild),
										  this->tail( path ), callLabel, *child_ ))
				{
					break;
				}
			}

			return true;
		}
		else
		{
			try
			{
				SEQ_reference rChild = this->FindChild( base, path );

				return child_->VisitChildren(
					(void*)(subBase_ + (FvUIntPtr)&rChild),
					this->tail( path ), pathRequest );
			}
			catch (NoSuchChild &)
			{
				return false;
			}
		}
	}

	virtual bool AddChild( const char * path, FvWatcherPtr pChild,
		void * withBase = NULL )
	{
		if (IsEmptyPath( path ))
		{
			return false;
		}
		else if (strchr(path,'/') == NULL)
		{
			if (child_ != NULL) return false;

			child_ = pChild;
			subBase_ = (FvUIntPtr)withBase;
			return true;
		}
		else
		{
			return child_->AddChild( this->tail ( path ),
				pChild, withBase );
		}
	}
	
	void SetLabels( const char ** labels )
	{
		labels_ = labels;
	}

	void SetLabelSubPath( const char * subpath )
	{
		labelsub_ = subpath;
	}

	void SetStringIndexConverter( SEQ_stringToIndex stringToIndex,
								  SEQ_indexToString indexToString )
	{
		indexToString_ = indexToString;
		stringToIndex_ = stringToIndex;
	}

	static const char * tail( const char * path )
	{
		return FvDirectoryWatcher::Tail( path );
	}

private:

	class NoSuchChild
	{
	public:
		NoSuchChild( const char * path ): path_( path ) {}

		const char * path_;
	};

	SEQ_reference FindChild( const void * base, const char * path ) const
	{
		SEQ	& useVector = *(SEQ*)(
			((const FvUIntPtr)&toWatch_) + ((const FvUIntPtr)base) );

		char * pSep = strchr( (char*)path, FV_WATCHER_SEPARATOR );
		FvString lookingFor = pSep ?
			FvString( path, pSep - path ) : FvString ( path );

		size_t	cLabels = 0;
		while (labels_ && labels_[cLabels] != NULL) cLabels++;

		size_t	lIter = 0;
		size_t	ende = std::min( cLabels, useVector.size() );
		SEQ_iterator	sIter = useVector.begin();
		while (lIter < ende)
		{
			if (!strcmp( labels_[lIter], lookingFor.c_str() ))
				return *sIter;

			lIter++;
			sIter++;
		}

		if (labelsub_ != NULL)
		{
			for (sIter = useVector.begin(); sIter != useVector.end(); sIter++)
			{
				FvString	tri;
				FvString	desc;

				FvWatcher::Mode unused;
				SEQ_reference ref = *sIter;
				child_->GetAsString( (void*)(subBase_ + (FvUIntPtr)&ref),
					labelsub_, tri, desc, unused );

				if (tri == lookingFor) return *sIter;
			}
		}

		if (stringToIndex_ != NULL)
		{
			lIter = stringToIndex_( base, lookingFor );
		}
		else
		{
			lIter = atoi( lookingFor.c_str() );
		}
		for (sIter = useVector.begin(); sIter != useVector.end() && lIter > 0;
			sIter++) lIter--; 

		if (sIter != useVector.end())
			return *sIter;

		throw NoSuchChild( path );
	}

	SEQ &toWatch_;
	const char **labels_;
	const char *labelsub_;
	FvWatcherPtr child_;
	FvUIntPtr subBase_;

	SEQ_indexToString indexToString_;
	SEQ_stringToIndex stringToIndex_;
};

template <class MAP> class FvMapWatcher : public FvWatcher
{
private:
	typedef typename FvMapTypes<MAP>::_Tref MAP_reference;
	typedef typename MAP::key_type MAP_key;
	typedef typename MAP::iterator MAP_iterator;
	typedef typename MAP::const_iterator MAP_constiterator;
public:

	FvMapWatcher( MAP & toWatch = *(MAP*)NULL ) :
		m_kToWatch( toWatch ),
		m_spChild( NULL ),
		m_spSubBase( 0 )
	{
	}

	virtual bool GetAsString( const void * base, const char * path,
		FvString & result, FvString & desc, FvWatcher::Mode & mode ) const
	{
		if (IsEmptyPath(path))
		{
			result = "<DIR>";
			mode = WT_DIRECTORY;
			desc = m_kComment;
			return true;
		}

		try
		{
			MAP_reference rChild = this->FindChild( base, path );

			return m_spChild->GetAsString( (void*)(m_spSubBase + (FvUIntPtr)&rChild),
				this->Tail( path ), result, desc, mode );
		}
		catch (NoSuchChild &)
		{
			return false;
		}
	}

	virtual bool SetFromString( void * base, const char * path,
		const char * valueStr )
	{
		try
		{
			MAP_reference rChild = this->FindChild( base, path );

			return m_spChild->SetFromString( (void*)(m_spSubBase + (FvUIntPtr)&rChild),
			this->Tail( path ), valueStr );
		}
		catch (NoSuchChild &)
		{
			return false;
		}
	}

	virtual bool GetAsStream( const void * base, const char * path,
		FvWatcherPathRequestV2 & pathRequest ) const
	{
		if (IsEmptyPath(path))
		{
			FvWatcher::Mode mode = WT_DIRECTORY;
			WatcherValueToStream( pathRequest.GetResultStream(), "<DIR>", mode );
			pathRequest.SetResult( m_kComment, mode, this, base );
			return true;
		}
		else if (IsDocPath( path ))
		{
			FvWatcher::Mode mode = FvWatcher::WT_READ_ONLY;
			WatcherValueToStream( pathRequest.GetResultStream(), m_kComment, mode );
			pathRequest.SetResult( m_kComment, mode, this, base );
			return true;
		}

		try
		{
			MAP_reference rChild = this->FindChild( base, path );

			return m_spChild->GetAsStream( (void*)(m_spSubBase + (FvUIntPtr)&rChild),
				this->Tail( path ), pathRequest );
		}
		catch (NoSuchChild &)
		{
			return false;
		}
	}

	virtual bool SetFromStream( void * base, const char * path,
		FvWatcherPathRequestV2 & pathRequest )
	{
		try
		{
			MAP_reference rChild = this->FindChild( base, path );

			return m_spChild->SetFromStream( (void*)(m_spSubBase + (FvUIntPtr)&rChild),
				this->Tail( path ), pathRequest );
		}
		catch (NoSuchChild &)
		{
			return false;
		}
	}


	virtual bool VisitChildren( const void * base, const char * path,
		FvWatcherPathRequest & pathRequest )
	{
		if (IsEmptyPath(path))
		{
			MAP	& useMap = *(MAP*)( ((FvUIntPtr)&m_kToWatch) + ((FvUIntPtr)base) );

			pathRequest.AddWatcherCount( useMap.size() );

			int count = 0;
			MAP_iterator iter = useMap.begin();
			while(iter != useMap.end())
			{
				FvString callLabel( WatcherValueToString( (*iter).first ) );

				if (!pathRequest.AddWatcherPath(
						(void*)(m_spSubBase + (FvUIntPtr)&(*iter).second),
						this->Tail( path ), callLabel, *m_spChild ))
				{
					break;
				}

				iter++;
				count++;
			}

			return true;
		}
		else
		{
			try
			{
				MAP_reference rChild = this->FindChild( base, path );

				return m_spChild->VisitChildren(
					(void*)(m_spSubBase + (FvUIntPtr)&rChild),
					this->Tail( path ), pathRequest );
			}
			catch (NoSuchChild &)
			{
				return false;
			}
		}
	}


	virtual bool AddChild( const char * path, FvWatcherPtr pChild,
		void * withBase = NULL )
	{
		if (IsEmptyPath( path ))
		{
			return false;
		}
		else if (strchr(path,'/') == NULL)
		{
			if (m_spChild != NULL) return false;

			m_spChild = pChild;
			m_spSubBase = (FvUIntPtr)withBase;
			return true;
		}
		else
		{
			return m_spChild->AddChild( this->Tail ( path ),
				pChild, withBase );
		}
	}

	static const char * Tail( const char * path )
	{
		return FvDirectoryWatcher::Tail( path );
	}

private:

	class NoSuchChild
	{
	public:
		NoSuchChild( const char * path ): path_( path ) {}

		const char * path_;
	};

	MAP_reference FindChild( const void * base, const char * path ) const
	{
		MAP	& useMap = *(MAP*)(
			((const FvUIntPtr)&m_kToWatch) + ((const FvUIntPtr)base) );

		char * pSep = strchr( (char*)path, FV_WATCHER_SEPARATOR );
		FvString lookingFor = pSep ?
			FvString( path, pSep - path ) : FvString ( path );

		MAP_key key;

		if (WatcherStringToValue( lookingFor.c_str(), key ))
		{
			MAP_iterator iter = useMap.find( key );
			if (iter != useMap.end())
			{
				return (*iter).second;
			}
		}

		throw NoSuchChild( path );
	}

	MAP &m_kToWatch;
	FvWatcherPtr m_spChild;
	FvUIntPtr m_spSubBase;
};

class FV_KERNEL_API FvDereferenceWatcher : public FvWatcher
{
public:

	FvDereferenceWatcher( FvWatcherPtr watcher, void * withBase = NULL );

	virtual bool GetAsString( const void * base, const char * path,
							  FvString & result, FvString & desc,
							  FvWatcher::Mode & mode ) const;

	virtual bool SetFromString( void * base, const char * path,
								const char * valueStr );

	virtual bool GetAsStream( const void * base, const char * path,
							  FvWatcherPathRequestV2 & pathRequest ) const;

	virtual bool SetFromStream( void * base, const char * path,
								FvWatcherPathRequestV2 & pathRequest );

	virtual bool VisitChildren( const void * base, const char * path,
								FvWatcherPathRequest & pathRequest );

	virtual bool AddChild( const char * path, FvWatcherPtr pChild,
						   void * withBase = NULL );
protected:
	FvWatcherPtr m_spWatcher;
	FvUIntPtr m_uiSB;

	virtual FvUIntPtr Dereference(const void *) const = 0;
};

class FV_KERNEL_API FvBaseDereferenceWatcher : public FvDereferenceWatcher
{
public:

	FvBaseDereferenceWatcher( FvWatcherPtr watcher, void * withBase = NULL ) :
		FvDereferenceWatcher( watcher, withBase )
	{ }

protected:

	FvUIntPtr Dereference(const void * base) const
	{
		return *(FvUIntPtr*)base;
	}
};

class FV_KERNEL_API FvSmartPointerDereferenceWatcher : public FvDereferenceWatcher
{
	typedef FvSmartPointer<unsigned int> Pointer;
public:

	FvSmartPointerDereferenceWatcher( FvWatcherPtr watcher, void * withBase = NULL ) :
		FvDereferenceWatcher( watcher, withBase )
	{ }

protected:

	FvUIntPtr Dereference(const void * base) const
	{
		return (FvUIntPtr)((Pointer*)base)->Get();
	}

};

template <class CONTAINER_TYPE, class KEY_TYPE >
class FvContainerBounceWatcher : public FvDereferenceWatcher
{
	CONTAINER_TYPE * indirection_;
public:

	FvContainerBounceWatcher( FvWatcherPtr watcher,
							CONTAINER_TYPE * indirection,
							void * withBase = NULL ) :
		FvDereferenceWatcher( watcher, withBase ),
		indirection_( indirection )
	{ }

	void ChangeContainer( CONTAINER_TYPE * indirection )
	{
		FV_ASSERT( indirection != NULL );
		indirection_ = indirection;
	}

protected:

	FvUIntPtr Dereference(const void * base) const
	{
		FV_ASSERT( indirection_ != NULL );
		FV_ASSERT( base != NULL );
		return (FvUIntPtr)&(*indirection_)[*(KEY_TYPE *)base];
	}
};

template <class RETURN_TYPE,
	class OBJECT_TYPE,
	class CONSTRUCTION_TYPE = RETURN_TYPE>
class FvMemberWatcher : public FvWatcher
{
	public:

		FvMemberWatcher( OBJECT_TYPE & rObject,
				RETURN_TYPE (OBJECT_TYPE::*getMethod)() const,
				void (OBJECT_TYPE::*setMethod)( RETURN_TYPE ) = NULL ) :
			rObject_( rObject ),
			getMethod_( getMethod ),
			setMethod_( setMethod )
		{}

	protected:

		virtual bool GetAsString( const void * base, const char * path,
			FvString & result, FvString & desc, FvWatcher::Mode & mode ) const
		{
			if (!IsEmptyPath( path )) return false;

			if (getMethod_ == (GetMethodType)NULL)
			{
				result = "";
				mode = WT_READ_ONLY;
				desc = m_kComment;
				return true;
			}

			const OBJECT_TYPE & useObject = *(OBJECT_TYPE*)(
				((const FvUIntPtr)&rObject_) + ((const FvUIntPtr)base) );

			RETURN_TYPE value = (useObject.*getMethod_)();

			result = WatcherValueToString( value );
			desc = m_kComment;

			mode = (setMethod_ != (SetMethodType)NULL) ?
				WT_READ_WRITE : WT_READ_ONLY;
			return true;
		};

		virtual bool GetAsStream( const void * base, const char * path,
				FvWatcherPathRequestV2 & pathRequest ) const
		{
			if (IsEmptyPath( path ))
			{
				if (getMethod_ == (GetMethodType)NULL)
				{
					FvWatcher::Mode mode = WT_READ_ONLY;
					WatcherValueToStream( pathRequest.GetResultStream() , "", mode );
					pathRequest.SetResult( m_kComment, mode, this, base );
					return true;
				}

				const OBJECT_TYPE & useObject = *(OBJECT_TYPE*)(
					((const FvUIntPtr)&rObject_) + ((const FvUIntPtr)base) );

				RETURN_TYPE value = (useObject.*getMethod_)();

				FvWatcher::Mode mode = (setMethod_ != (SetMethodType)NULL) ?
						 WT_READ_WRITE : WT_READ_ONLY;

				WatcherValueToStream( pathRequest.GetResultStream() , value, mode );
				pathRequest.SetResult( m_kComment, mode, this, base );

				return true;
			}
			else if (IsDocPath( path ))
			{
				FvWatcher::Mode mode = FvWatcher::WT_READ_ONLY;
				WatcherValueToStream( pathRequest.GetResultStream(), m_kComment, mode );
				pathRequest.SetResult( m_kComment, mode, this, base );
				return true;
			}

			return false;
		}


		virtual bool SetFromString( void * base, const char *path,
			const char * valueStr )
		{
			if (!IsEmptyPath( path ) || (setMethod_ == (SetMethodType)NULL))
				return false;

			bool wasSet = false;

			CONSTRUCTION_TYPE value = CONSTRUCTION_TYPE();
			if (WatcherStringToValue( valueStr, value ))
			{
				OBJECT_TYPE & useObject =
					*(OBJECT_TYPE*)( ((FvUIntPtr)&rObject_) + ((FvUIntPtr)base) );
				(useObject.*setMethod_)( value );
				wasSet = true;
			}

			return wasSet;
		}


		virtual bool SetFromStream( void * base, const char *path,
			FvWatcherPathRequestV2 & pathRequest )
		{
			if (!IsEmptyPath( path ) || (setMethod_ == (SetMethodType)NULL))
				return false;

			bool wasSet = false;

			CONSTRUCTION_TYPE value = CONSTRUCTION_TYPE();
			FvBinaryIStream *istream = pathRequest.GetValueStream();
			if (istream && WatcherStreamToValue( *istream, value ))
			{
				OBJECT_TYPE & useObject =
					*(OBJECT_TYPE*)( ((FvUIntPtr)&rObject_) + ((FvUIntPtr)base) );
				(useObject.*setMethod_)( value );
				wasSet = true;

				FvWatcher::Mode mode = FvWatcher::WT_READ_WRITE;
				WatcherValueToStream( pathRequest.GetResultStream(), value, mode );
				pathRequest.SetResult( m_kComment, mode, this, base );
			}

			return wasSet;
		}

	private:

		typedef RETURN_TYPE (OBJECT_TYPE::*GetMethodType)() const;
		typedef void (OBJECT_TYPE::*SetMethodType)( RETURN_TYPE);

		OBJECT_TYPE & rObject_;
		RETURN_TYPE (OBJECT_TYPE::*getMethod_)() const;
		void (OBJECT_TYPE::*setMethod_)( RETURN_TYPE );
};

template <class RETURN_TYPE, class OBJECT_TYPE>
FvWatcherPtr MakeWatcher( OBJECT_TYPE & rObject,
			const RETURN_TYPE & (OBJECT_TYPE::*getMethod)() const )
{
	return new FvMemberWatcher<const RETURN_TYPE &, OBJECT_TYPE, RETURN_TYPE>(
			rObject, getMethod, NULL );
}

template <class RETURN_TYPE, class OBJECT_TYPE, class DUMMY_TYPE>
FvWatcherPtr MakeWatcher( OBJECT_TYPE & rObject,
						const RETURN_TYPE & (OBJECT_TYPE::*getMethod)() const,
						void (OBJECT_TYPE::*setMethod)(DUMMY_TYPE) )
{
	return new FvMemberWatcher<const RETURN_TYPE &, OBJECT_TYPE, RETURN_TYPE>(
			rObject, getMethod, setMethod );
}

template <class RETURN_TYPE, class OBJECT_TYPE>
FvWatcherPtr MakeWatcher(	OBJECT_TYPE & rObject,
						RETURN_TYPE (OBJECT_TYPE::*getMethod)() const )
{
	return new FvMemberWatcher<RETURN_TYPE, OBJECT_TYPE, RETURN_TYPE>(
				rObject,
				getMethod,
				static_cast< void (OBJECT_TYPE::*)( RETURN_TYPE ) >( NULL ) );
}

template <class RETURN_TYPE, class OBJECT_TYPE, class DUMMY_TYPE>
FvWatcherPtr MakeWatcher(	const char * path,
						OBJECT_TYPE & rObject,
						RETURN_TYPE (OBJECT_TYPE::*getMethod)() const,
						void (OBJECT_TYPE::*setMethod)(DUMMY_TYPE),
	   					const char * comment = NULL	)
{
	return new FvMemberWatcher<RETURN_TYPE, OBJECT_TYPE>(
			rObject, getMethod, setMethod );
}

template <class TYPE>
class FvDataWatcher : public FvWatcher
{
public:

	explicit FvDataWatcher( TYPE & rValue,
			FvWatcher::Mode access = FvWatcher::WT_READ_ONLY,
			const char * path = NULL ) :
		rValue_( rValue ),
		access_( access )
	{
		if (access_ != WT_READ_WRITE && access_ != WT_READ_ONLY)
			access_ = WT_READ_ONLY;

		if (path != NULL)
			FvWatcher::RootWatcher().AddChild( path, this );
	}

protected:

	virtual bool GetAsString( const void * base, const char * path,
		FvString & result, FvString & desc, FvWatcher::Mode & mode ) const
	{
		if (IsEmptyPath( path ))
		{
			const TYPE & useValue = *(const TYPE*)(
				((const FvUIntPtr)&rValue_) + ((const FvUIntPtr)base) );

			result = WatcherValueToString( useValue );
			desc = m_kComment;
			mode = access_;
			return true;
		}
		else
		{
			return false;
		}
	};

	virtual bool SetFromString( void * base, const char * path,
		const char * valueStr )
	{
		if (IsEmptyPath( path ) && (access_ == WT_READ_WRITE))
		{
			TYPE & useValue =
				*(TYPE*)( ((FvUIntPtr)&rValue_) + ((FvUIntPtr)base) );

			return WatcherStringToValue( valueStr, useValue );
		}
		else
		{
			return false;
		}
	}

	virtual bool GetAsStream( const void * base, const char * path,
		FvWatcherPathRequestV2 & pathRequest ) const
	{
		if (IsEmptyPath( path ))
		{
			const TYPE & useValue = *(const TYPE*)(
				((const FvUIntPtr)&rValue_) + ((const FvUIntPtr)base) );

			FvWatcher::Mode mode = access_;
			WatcherValueToStream( pathRequest.GetResultStream(), useValue, mode );
			pathRequest.SetResult( m_kComment, mode, this, base );
			return true;
		}
		else if (IsDocPath( path ))
		{
			FvWatcher::Mode mode = FvWatcher::WT_READ_ONLY;
			WatcherValueToStream( pathRequest.GetResultStream(), m_kComment, mode );
			pathRequest.SetResult( m_kComment, mode, this, base );
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual bool SetFromStream( void * base, const char * path,
		FvWatcherPathRequestV2 & pathRequest )
	{
		bool status = false;

		if (IsEmptyPath( path ) && (access_ == WT_READ_WRITE))
		{
			TYPE & useValue =
				*(TYPE*)( ((FvUIntPtr)&rValue_) + ((FvUIntPtr)base) );

			FvBinaryIStream *istream = pathRequest.GetValueStream();
			if (istream && WatcherStreamToValue( *istream, useValue ))
			{
				status = true;
			}

			FvWatcher::Mode mode = FvWatcher::WT_READ_WRITE;
			WatcherValueToStream( pathRequest.GetResultStream(), useValue, mode );
			pathRequest.SetResult( m_kComment, mode, this, base );
		}

		return status;
	}

private:
	TYPE & rValue_;
	FvWatcher::Mode access_;
};

template <class TYPE>
FvWatcherPtr MakeWatcher( TYPE & rValue,
				FvWatcher::Mode access = FvWatcher::WT_READ_ONLY )
{
	return new FvDataWatcher< TYPE >( rValue, access );
}

template <class RETURN_TYPE, class CONSTRUCTION_TYPE = RETURN_TYPE>
class FvFunctionWatcher : public FvWatcher
{
public:

	explicit FvFunctionWatcher( RETURN_TYPE (*getFunction)(),
			void (*setFunction)( RETURN_TYPE ) = NULL,
			const char * path = NULL ) :
		getFunction_( getFunction ),
		setFunction_( setFunction )
	{
		if (path != NULL)
			FvWatcher::RootWatcher().AddChild( path, this );
	}

protected:

	virtual bool GetAsString( const void * base, const char * path,
		FvString & result, FvString & desc, FvWatcher::Mode & mode ) const
	{
		if (IsEmptyPath( path ))
		{
			result = WatcherValueToString( (*getFunction_)() );
			mode = (setFunction_ != NULL) ? WT_READ_WRITE : WT_READ_ONLY;
			desc = m_kComment;
			return true;
		}
		else
		{
			return false;
		}
	};

	virtual bool SetFromString( void * base, const char * path,
		const char * valueStr )
	{
		bool result = false;

		if (IsEmptyPath( path ) && (setFunction_ != NULL))
		{
			CONSTRUCTION_TYPE value = CONSTRUCTION_TYPE();
			result = WatcherStringToValue( valueStr, value );

			if (result)
			{
				(*setFunction_)( value );
			}
		}

		return result;
	}

	virtual bool GetAsStream( const void * base, const char * path,
		FvWatcherPathRequestV2 & pathRequest ) const
	{
		if (IsEmptyPath( path ))
		{
			FvWatcher::Mode mode = (setFunction_ != NULL) ?  WT_READ_WRITE : WT_READ_ONLY;
			WatcherValueToStream( pathRequest.GetResultStream(), (*getFunction_)(), mode );
			pathRequest.SetResult( m_kComment, mode, this, base );
			return true;
		}
		else if (IsDocPath( path ))
		{
			FvWatcher::Mode mode = FvWatcher::WT_READ_ONLY;
			WatcherValueToStream( pathRequest.GetResultStream(), m_kComment, mode );
			pathRequest.SetResult( m_kComment, mode, this, base );
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual bool SetFromStream( void * base, const char * path,
		FvWatcherPathRequestV2 & pathRequest )
	{
		bool status = false;

		if (IsEmptyPath( path ) && (setFunction_ != NULL))
		{
			CONSTRUCTION_TYPE value = CONSTRUCTION_TYPE();

			FvBinaryIStream *istream = pathRequest.GetValueStream();
			if (istream && WatcherStreamToValue( *istream, value ))
			{
				status = true;
				(*setFunction_)( value );
			}

			FvWatcher::Mode mode = FvWatcher::WT_READ_WRITE;
			WatcherValueToStream( pathRequest.GetResultStream(), value, mode );
			pathRequest.SetResult( m_kComment, mode, this, base );
		}

		return status;
	}

private:
	RETURN_TYPE (*getFunction_)();
	void (*setFunction_)( RETURN_TYPE );
};

class FV_KERNEL_API FvCallableWatcher : public FvWatcher
{

private:

	class ArgDescription
	{
	public:
		ArgDescription( FvWatcherDataType type, const FvString & description );

		FvWatcherDataType type() const			{ return type_; }
		const FvString & description() const	{ return description_; }

	private:
		FvWatcherDataType type_;
		FvString description_;
	};

public:
	enum ExposeHint
	{
		WITH_ENTITY,
		ALL,
		WITH_SPACE,
		LEAST_LOADED,
		LOCAL_ONLY
	};

	FvCallableWatcher( ExposeHint hint = LOCAL_ONLY,
			const char * comment = "" );

	void AddArg( FvWatcherDataType type, const char * description = "" );

	ExposeHint GetExposedHint() const		{ return m_eExposeHint; }
	void SetExposeHint( ExposeHint hint )	{ m_eExposeHint = hint; }

protected:

	virtual bool GetAsString( const void * base, const char * path,
		FvString & result, FvString & desc, FvWatcher::Mode & mode ) const;

	virtual bool SetFromString( void * base, const char * path,
		const char * valueStr );

	virtual bool GetAsStream( const void * base, const char * path,
		FvWatcherPathRequestV2 & pathRequest ) const;

	static int TupleElementStreamSize( int payloadSize )
	{
		const int TYPE_OVERHEAD = 2 * sizeof( FvUInt8 );

		return TYPE_OVERHEAD +
			FvBinaryOStream::CalculatePackedIntSize( payloadSize ) +
			payloadSize;
	}

private:
	static bool IsArgsPath( const char * path );
	static bool IsExposePath( const char * path );

private:
	ExposeHint m_eExposeHint;
	std::vector< ArgDescription > m_kArgList;
};

class FV_KERNEL_API FvNoArgCallableWatcher : public FvCallableWatcher
{
public:
	typedef bool (*Function)( FvString &, FvString & );

	FvNoArgCallableWatcher( Function func,
			ExposeHint hint = LOCAL_ONLY, const char * comment = "" );

protected:
	virtual bool SetFromStream( void * base,
			const char * path,
			FvWatcherPathRequestV2 & pathRequest );

private:
	Function m_pfFunc;
};

template <class RETURN_TYPE, class OBJECT_TYPE>
FvWatcherPtr AddReferenceWatcher( const char * path,
			OBJECT_TYPE & rObject,
			const RETURN_TYPE & (OBJECT_TYPE::*getMethod)() const,
	   		const char * comment = NULL	)
{
	FvWatcherPtr pNewWatcher = MakeWatcher( rObject, getMethod, NULL );

	if (!FvWatcher::RootWatcher().AddChild( path, pNewWatcher ))
	{
		pNewWatcher = NULL;
	}
	else if (comment)
	{
		FvString s( comment );
		pNewWatcher->SetComment( s );
	}

	return pNewWatcher;
}

template <class RETURN_TYPE, class OBJECT_TYPE, class DUMMY_TYPE>
FvWatcherPtr AddReferenceWatcher( const char * path,
						OBJECT_TYPE & rObject,
						const RETURN_TYPE & (OBJECT_TYPE::*getMethod)() const,
						void (OBJECT_TYPE::*setMethod)(DUMMY_TYPE),
						const char * comment = NULL	)
{
	FvWatcherPtr pNewWatcher = MakeWatcher( rObject, getMethod, setMethod );

	if (!FvWatcher::RootWatcher().AddChild( path, pNewWatcher ))
	{
		pNewWatcher = NULL;
	}
	else if (comment)
	{
		FvString s( comment );
		pNewWatcher->SetComment( s );
	}

	return pNewWatcher;
}

template <class RETURN_TYPE, class OBJECT_TYPE>
FvWatcherPtr AddWatcher(	const char * path,
						OBJECT_TYPE & rObject,
						RETURN_TYPE (OBJECT_TYPE::*getMethod)() const,
	   					const char * comment = NULL	)
{
	FvWatcherPtr pNewWatcher =
		new FvMemberWatcher<RETURN_TYPE, OBJECT_TYPE, RETURN_TYPE>(
				rObject,
				getMethod,
				static_cast< void (OBJECT_TYPE::*)( RETURN_TYPE ) >( NULL ) );

	if (!FvWatcher::RootWatcher().AddChild( path, pNewWatcher ))
	{
		pNewWatcher = NULL;
	}
	else if (comment)
	{
		FvString s( comment );
		pNewWatcher->SetComment( s );
	}

	return pNewWatcher;
}

template <class RETURN_TYPE, class OBJECT_TYPE, class DUMMY_TYPE>
FvWatcherPtr AddWatcher(	const char * path,
						OBJECT_TYPE & rObject,
						RETURN_TYPE (OBJECT_TYPE::*getMethod)() const,
						void (OBJECT_TYPE::*setMethod)(DUMMY_TYPE),
	   					const char * comment = NULL	)
{
	FvWatcherPtr pNewWatcher = new FvMemberWatcher<RETURN_TYPE, OBJECT_TYPE>(
			rObject, getMethod, setMethod );

	if (!FvWatcher::RootWatcher().AddChild( path, pNewWatcher ))
	{
		pNewWatcher = NULL;
	}
	else if (comment)
	{
		FvString s( comment );
		pNewWatcher->SetComment( s );
	}

	return pNewWatcher;
}

template <class RETURN_TYPE>
FvWatcherPtr AddWatcher(	const char * path,
						RETURN_TYPE (*getFunction)(),
						void (*setFunction)( RETURN_TYPE ) = NULL,
	   					const char * comment = NULL	)
{
	FvWatcherPtr pNewWatcher =
		new FvFunctionWatcher<RETURN_TYPE>( getFunction, setFunction );

	if (!FvWatcher::RootWatcher().AddChild( path, pNewWatcher ))
	{
		pNewWatcher = NULL;
	}
	else if (comment)
	{
		FvString s( comment );
		pNewWatcher->SetComment( s );
	}

	return pNewWatcher;
}

template <class TYPE>
FvWatcherPtr AddWatcher(	const char * path,
						TYPE & rValue,
						FvWatcher::Mode access = FvWatcher::WT_READ_WRITE,
   						const char * comment = NULL	)
{
	FvWatcherPtr pNewWatcher = MakeWatcher( rValue, access );

	if (!FvWatcher::RootWatcher().AddChild( path, pNewWatcher ))
	{
		pNewWatcher = NULL;
	}
	else if (comment)
	{
		FvString s( comment );
		pNewWatcher->SetComment( s );
	}

	return pNewWatcher;
}

#define FV_WATCH				::AddWatcher

#define FV_WATCH_REF			::AddReferenceWatcher

#define FV_ACCESSORS( TYPE, CLASS, METHOD )							\
	static_cast< TYPE (CLASS::*)() const >(&CLASS::METHOD),			\
	static_cast< void (CLASS::*)(TYPE)   >(&CLASS::METHOD)

#define FV_WRITE_ACCESSOR( TYPE, CLASS, METHOD )					\
	static_cast< TYPE (CLASS::*)() const >( NULL ),					\
	static_cast< void (CLASS::*)(TYPE)   >( &CLASS::METHOD )

#else // FV_ENABLE_WATCHERS

#if defined(_WIN32) && ( _MSC_VER < 1400 )
	#define FV_WATCH false &&
#else
	#define FV_WATCH( ... )
#endif

#define FV_ACCESSORS( TYPE, CLASS, METHOD )	NULL

class FvWatcher
{
public:
	enum Mode
	{
		WT_INVALID,		
		WT_READ_ONLY,	
		WT_READ_WRITE,
		WT_DIRECTORY
	};
};

#endif // !FV_ENABLE_WATCHERS

#endif // __FvWatcher_H__