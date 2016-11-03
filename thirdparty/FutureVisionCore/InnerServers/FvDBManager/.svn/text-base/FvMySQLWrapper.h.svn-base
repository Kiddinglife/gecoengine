//{future header message}
#ifndef __FvMySQLWrapper_H__
#define __FvMySQLWrapper_H__

#include <FvBaseTypes.h>
#include <FvDebug.h>

#include <vector>

#include <stdexcept>
#include <mysql.h>
#include <mysqld_error.h>

class FvBinaryOStream;
class FvBinaryIStream;
namespace MySqlUnPrep
{
	class Statement;
	class Bindings;
}
namespace MySqlPrep
{
	class Statement;
	class Bindings;
}

struct FvDBConfigConnection;

#define MYSQL_ENGINE_TYPE "InnoDB"

time_t ConvertMySqlTimeToEpoch(  const MYSQL_TIME& mysqlTime );

template < class CTYPE > struct MySqlTypeTraits
{

};
template <> struct MySqlTypeTraits<FvInt8>
{
	static const enum enum_field_types	colType = MYSQL_TYPE_TINY;
};
template <> struct MySqlTypeTraits<FvUInt8>
{
	static const enum enum_field_types	colType = MYSQL_TYPE_TINY;
};
template <> struct MySqlTypeTraits<FvInt16>
{
	static const enum enum_field_types	colType = MYSQL_TYPE_SHORT;
};
template <> struct MySqlTypeTraits<FvUInt16>
{
	static const enum enum_field_types	colType = MYSQL_TYPE_SHORT;
};
template <> struct MySqlTypeTraits<FvInt32>
{
	static const enum enum_field_types	colType = MYSQL_TYPE_LONG;
};
template <> struct MySqlTypeTraits<FvUInt32>
{
	static const enum enum_field_types	colType = MYSQL_TYPE_LONG;
};
template <> struct MySqlTypeTraits<FvInt64>
{
	static const enum enum_field_types	colType = MYSQL_TYPE_LONGLONG;
};
template <> struct MySqlTypeTraits<FvUInt64>
{
	static const enum enum_field_types	colType = MYSQL_TYPE_LONGLONG;
};
template <> struct MySqlTypeTraits<float>
{
	static const enum enum_field_types	colType = MYSQL_TYPE_FLOAT;
};
template <> struct MySqlTypeTraits<double>
{
	static const enum enum_field_types	colType = MYSQL_TYPE_DOUBLE;
};
template <> struct MySqlTypeTraits<bool>
{
	static const enum enum_field_types	colType = MYSQL_TYPE_BIT;//! add by Uman, 20101119
};
template <> struct MySqlTypeTraits<FvString>
{
	typedef MySqlTypeTraits<FvString> THIS_CLASS;

	static const enum enum_field_types colType( int maxColWidth )
	{
		if (maxColWidth < 1<<8)
			return MYSQL_TYPE_TINY_BLOB;
		else if (maxColWidth < 1<<16)
			return MYSQL_TYPE_BLOB;
		else if (maxColWidth < 1<<24)
			return MYSQL_TYPE_MEDIUM_BLOB;
		else
			return MYSQL_TYPE_LONG_BLOB;
	}
	static const FvString TINYBLOB;
	static const FvString BLOB;
	static const FvString MEDIUMBLOB;
	static const FvString LONGBLOB;
	static const FvString colTypeStr( int maxColWidth )
	{
		switch ( THIS_CLASS::colType( maxColWidth ) )
		{
			case MYSQL_TYPE_TINY_BLOB:
				return TINYBLOB;
			case MYSQL_TYPE_BLOB:
				return BLOB;
			case MYSQL_TYPE_MEDIUM_BLOB:
				return MEDIUMBLOB;
			case MYSQL_TYPE_LONG_BLOB:
				return LONGBLOB;
			default:
				break;
		}
		return NULL;
	}
};

class MySqlError : public std::exception
{
public:
	MySqlError( MYSQL * mysql ) : m_kErr( mysql_error(mysql) ) {}
	MySqlError( MYSQL_STMT * stmt ) : m_kErr( mysql_stmt_error(stmt) ) {}
	~MySqlError() throw() {}
	const char *what() const throw() { return m_kErr.c_str(); }

private:
	const FvString m_kErr;
};

class MySqlRetryTransactionException : public MySqlError
{
public:
	MySqlRetryTransactionException( MYSQL * mysql ) : MySqlError( mysql ) {}
	MySqlRetryTransactionException( MYSQL_STMT * stmt ) : MySqlError( stmt ) {}
};

class MySqlDuplicateEntryException : public MySqlError
{
public:
	MySqlDuplicateEntryException( MYSQL * mysql ) : MySqlError( mysql ) {}
	MySqlDuplicateEntryException( MYSQL_STMT * stmt ) : MySqlError( stmt ) {}
};

class MySqlResult
{
	MYSQL_RES &m_kHandle;
	my_ulonglong m_uiNumRows;
	unsigned int m_uiNumFields;

	MYSQL_ROW m_kCurRow;
	unsigned long *m_puiCurRowLengths;

public:
	MySqlResult( MYSQL_RES& handle ) : m_kHandle( handle ),
		m_uiNumRows( mysql_num_rows( &m_kHandle ) ),
		m_uiNumFields( mysql_num_fields( &m_kHandle ) )
	{}

	~MySqlResult()
	{
		mysql_free_result( &m_kHandle );
	}

	my_ulonglong GetNumRows() const		{ return m_uiNumRows; }
	unsigned int GetNumFields() const 	{ return m_uiNumFields; }

	bool GetNextRow()
	{
		m_kCurRow = mysql_fetch_row( &m_kHandle );
		if (m_kCurRow)
			m_puiCurRowLengths = mysql_fetch_lengths( &m_kHandle );

		return m_kCurRow != NULL;
	}

	const char *GetField( unsigned int n )		{ return m_kCurRow[n]; }
	unsigned long GetFieldLen( unsigned int n )	{ return m_puiCurRowLengths[n]; }
};

class MySql
{
public:
	MySql( const FvDBConfigConnection& connectInfo );
	~MySql();

	MYSQL *Get() { return m_pkSQL; }

	void Execute( const FvString & statement,
		FvBinaryOStream * pResData = NULL );
	void Execute( MySqlUnPrep::Statement& stmt );
	void Execute( MySqlPrep::Statement& stmt );
	int Query( const FvString & statement );
	MYSQL_RES *StoreResult()	{ return mysql_store_result( m_pkSQL ); }
	bool Ping()					{ return mysql_ping( m_pkSQL ) == 0; }
	void GetTableNames( std::vector<FvString>& tableNames,
						const char * pattern );
	my_ulonglong InsertID()		{ return mysql_insert_id( m_pkSQL ); }
	my_ulonglong AffectedRows()	{ return mysql_affected_rows( m_pkSQL ); }
	const char *Info()			{ return mysql_info( m_pkSQL ); }
	const char *GetLastError()	{ return mysql_error( m_pkSQL ); }
	unsigned int GetLastErrorNum() { return mysql_errno( m_pkSQL ); }

	struct Lock
	{
		MySql &m_kSQL;

		Lock(MySql& mysql) : m_kSQL(mysql)
		{
			FV_ASSERT(!m_kSQL.m_bInTransaction);
			m_kSQL.m_bInTransaction = true;
		}
		~Lock()
		{
			FV_ASSERT(m_kSQL.m_bInTransaction);
			m_kSQL.m_bInTransaction = false;
		}
	private:
		Lock(const Lock&);
		Lock& operator=(const Lock&);
	};

	bool HasFatalError() const	{	return !m_kFatalErrorStr.empty();	}
	const FvString &GetFatalErrorStr() const	{	return m_kFatalErrorStr;	}
	void OnFatalError(const std::exception& e)	{ m_kFatalErrorStr = e.what(); }

private:
	template <class MYSQLOBJ>
	void ThrowError( MYSQLOBJ* failedObj );

	MySql( const MySql& );
	void operator=( const MySql& );

	MYSQL *m_pkSQL;
	bool m_bInTransaction;
	FvString m_kFatalErrorStr;
};

class MySqlTransaction
{
public:
	MySqlTransaction( MySql& sql ) : m_kLock(sql), m_bCommitted(false)
	{
		m_kLock.m_kSQL.Execute( "START TRANSACTION" );
	}
	MySqlTransaction( MySql& sql, int& errorNum ) :
		m_kLock(sql), m_bCommitted(false)
	{
		errorNum = this->Query( "START TRANSACTION" );
	}
	~MySqlTransaction()
	{
		if ( !m_bCommitted && !m_kLock.m_kSQL.HasFatalError() )
		{
			try
			{
				m_kLock.m_kSQL.Execute( "ROLLBACK" );
			}
			catch (std::exception& e)
			{
				m_kLock.m_kSQL.OnFatalError( e );
			}
		}
	}

	MySql &Get() { return m_kLock.m_kSQL; }

	void Execute( MySqlUnPrep::Statement& stmt )
	{
		FV_ASSERT( !m_bCommitted );
		m_kLock.m_kSQL.Execute( stmt );
	}
	void Execute( MySqlPrep::Statement& stmt )
	{
		FV_ASSERT( !m_bCommitted );
		m_kLock.m_kSQL.Execute( stmt );
	}
	void Execute( const FvString& stmt, FvBinaryOStream * resdata = NULL )
	{
		FV_ASSERT( !m_bCommitted );
		m_kLock.m_kSQL.Execute( stmt, resdata );
	}
	int Query( const FvString & statement )
	{ return m_kLock.m_kSQL.Query( statement ); }
	MYSQL_RES *StoreResult()	{ return m_kLock.m_kSQL.StoreResult(); }
	my_ulonglong InsertID()		{ return m_kLock.m_kSQL.InsertID(); }
	my_ulonglong AffectedRows()	{ return m_kLock.m_kSQL.AffectedRows(); }
	const char *Info()			{ return m_kLock.m_kSQL.Info();	}
	const char *GetLastError()	{ return m_kLock.m_kSQL.GetLastError(); }
	unsigned int GetLastErrorNum() { return m_kLock.m_kSQL.GetLastErrorNum(); }
	bool ShouldRetry() { return (this->GetLastErrorNum() == ER_LOCK_DEADLOCK); }

	void Commit()
	{
		FV_ASSERT( !m_bCommitted );
		m_kLock.m_kSQL.Execute( "COMMIT" );
		m_bCommitted = true;
	}

private:
	MySqlTransaction( const MySqlTransaction& );
	void operator=( const MySqlTransaction& );

	MySql::Lock	m_kLock;
	bool m_bCommitted;
};

template <class T>
class MySqlValueWithNull
{
public:
	MySqlValueWithNull() : m_bIsNull(1) {}
	MySqlValueWithNull( const T& x ) : m_kValue(x), m_bIsNull(0) {}

	void Nullify() { m_bIsNull = 1; }
	void Valuefy() { m_bIsNull = 0; }
	void Set( const T& x ) { m_kValue = x; m_bIsNull = 0; }
	const T *Get() const { return m_bIsNull? 0 : &m_kValue; }
	T &GetBuf() { return m_kValue; }

protected:
	template <class TYPE>
	friend MySqlPrep::Bindings& operator<<( MySqlPrep::Bindings&, MySqlValueWithNull<TYPE>& );

	T m_kValue;
	my_bool m_bIsNull;
};

class MySqlTimestampNull : public MySqlValueWithNull< MYSQL_TIME >
{
public:
	typedef MySqlValueWithNull< MYSQL_TIME > BaseClass;
	MySqlTimestampNull() {}
	MySqlTimestampNull( const MYSQL_TIME& x ) : BaseClass(x) {}
private:
	friend MySqlPrep::Bindings& operator<<( MySqlPrep::Bindings&, MySqlTimestampNull& );
};

class MySqlBuffer
{
public:
	MySqlBuffer( unsigned long capacity, enum_field_types type = FIELD_TYPE_BLOB ) :
		m_eType( type ),
		m_pcBuffer( new char[capacity] ),
		m_uiSize( 0 ),
		m_uiCapacity( capacity ),
		m_bIsNull( 1 )
	{
	}

	~MySqlBuffer()
	{
		delete[] m_pcBuffer;
	}

	unsigned long size() const
	{
		return m_bIsNull? 0 : m_uiSize;
	}

	unsigned long Capacity() const
	{
		return m_uiCapacity;
	}

	void Set( const void * ptr, unsigned long size )
	{
		if (size > m_uiCapacity)
		{
			PrintTruncateError( size, m_uiCapacity );
			size = m_uiCapacity;
		}

		memcpy( m_pcBuffer, ptr, size );
		m_uiSize = size;
		m_bIsNull = 0;
	}

	void SetString( const FvString& s )
	{
		Set( s.data(), s.length() );
	}

	FvString GetString() const
	{
		const char * p = (const char *) this->Get();
		if (!p) throw std::runtime_error("string is null");
		return FvString( p, m_uiSize );
	}

	void Nullify()
	{
		m_bIsNull = 1;
	}

	const void *Get() const
	{
		return m_bIsNull? 0 : m_pcBuffer;
	}

	bool IsNull() const
	{
		return m_bIsNull? true : false;
	}

private:
	friend MySqlUnPrep::Bindings& operator<<( MySqlUnPrep::Bindings& binding, MySqlBuffer& buffer );
	friend MySqlPrep::Bindings& operator<<( MySqlPrep::Bindings& binding, MySqlBuffer& buffer );
	friend FvBinaryIStream& operator>>( FvBinaryIStream& strm, MySqlBuffer& buffer );
	friend FvBinaryOStream& operator<<( FvBinaryOStream& strm, const MySqlBuffer& buffer );

	MySqlBuffer( const MySqlBuffer& );
	MySqlBuffer& operator=( const MySqlBuffer& );

	static void PrintTruncateError( unsigned long size,
			unsigned long capacity );

	enum_field_types m_eType;
	char *m_pcBuffer;
	unsigned long m_uiSize;
	unsigned long m_uiCapacity;
	my_bool m_bIsNull;
};
FvBinaryIStream& operator>>( FvBinaryIStream& strm, MySqlBuffer& buffer );
FvBinaryOStream& operator<<( FvBinaryOStream& strm, const MySqlBuffer& buffer );

class MySqlEscapedString
{
public:
	MySqlEscapedString( MySql& connection, const FvString& string ) :
		m_pcEscapedString( new char[ string.length() * 2 + 1 ] )
	{
		mysql_real_escape_string( connection.Get(), m_pcEscapedString,
				string.data(), string.length() );
	}
	~MySqlEscapedString()
	{
		delete [] m_pcEscapedString;
	}

	operator char*()	{ return m_pcEscapedString; }

private:
	char *m_pcEscapedString;
};

class MySqlTableMetadata
{
public:
	MySqlTableMetadata( MySql& connection, const FvString tableName );
	~MySqlTableMetadata();

	bool IsValid() const 				{ return m_pkResult; }
	unsigned int GetNumFields() const	{ return m_uiNumFields; }
	const MYSQL_FIELD &GetField( unsigned int index ) const
	{ return m_pkFields[index]; }

private:
	MYSQL_RES *m_pkResult;
	unsigned int m_uiNumFields;
	MYSQL_FIELD *m_pkFields;
};

#ifdef FV_USE_MYSQL_PREPARED_STATEMENTS
	#include "FvMySQLPrepared.h"
	typedef	MySqlPrep::Bindings		MySqlBindings;
	typedef MySqlPrep::Statement	MySqlStatement;
#else
	#include "FvMySQLNotPrepared.h"
	typedef	MySqlUnPrep::Bindings	MySqlBindings;
	typedef MySqlUnPrep::Statement	MySqlStatement;
#endif

template <class QUERY>
bool WrapInTransaction( MySql& connection, QUERY& query )
{
	bool retry;
	do
	{
		retry = false;
		try
		{
			MySqlTransaction transaction( connection );

			query.Execute( transaction.Get() );

			transaction.Commit();
		}
		catch (MySqlRetryTransactionException& e)
		{
			const char* pMsg = e.what();
			retry = true;
		}
		catch (std::exception& e)
		{
			query.SetExceptionStr( e.what() );
			return false;
		}
	} while (retry);

	return true;
};

#endif // __FvMySQLWrapper_H__
