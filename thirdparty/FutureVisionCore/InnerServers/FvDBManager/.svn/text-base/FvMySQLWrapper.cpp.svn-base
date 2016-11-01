#include "FvMySQLWrapper.h"

#include "FvMySQLPrepared.h"
#include "FvMySQLNotPrepared.h"
#include "FvDBConfig.h"

#include <FvBinaryStream.h>

#include <errmsg.h>
#include <time.h>

FV_DECLARE_DEBUG_COMPONENT( 0 );

time_t ConvertMySqlTimeToEpoch(  const MYSQL_TIME& mysqlTime )
{
	tm ctime;
	ctime.tm_year = mysqlTime.year - 1900;
	ctime.tm_mon = mysqlTime.month - 1;
	ctime.tm_mday = mysqlTime.day;
	ctime.tm_hour = mysqlTime.hour;
	ctime.tm_min = mysqlTime.minute;
	ctime.tm_hour = mysqlTime.hour;
	ctime.tm_sec = mysqlTime.second;

	ctime.tm_wday = -1;
	ctime.tm_yday = -1;
	ctime.tm_isdst = -1;
#ifndef _WIN32
	return timegm( &ctime );
#else // !_WIN32
	return mktime( &ctime );
#endif // _WIN32
}

const FvString MySqlTypeTraits<FvString>::TINYBLOB( "TINYBLOB" );
const FvString MySqlTypeTraits<FvString>::BLOB( "BLOB" );
const FvString MySqlTypeTraits<FvString>::MEDIUMBLOB( "MEDIUMBLOB" );
const FvString MySqlTypeTraits<FvString>::LONGBLOB( "LONGBLOB" );


MySql::MySql( const FvDBConfigConnection& connectInfo ) :
	m_pkSQL(0),
	m_bInTransaction(false)
{
	try
	{
		m_pkSQL = mysql_init( 0 );
		if (!m_pkSQL)
			this->ThrowError( this->m_pkSQL );
		if (!mysql_real_connect( m_pkSQL, connectInfo.m_kHost.c_str(),
				connectInfo.m_kUsername.c_str(), connectInfo.m_kPassword.c_str(),
				connectInfo.m_kDatabase.c_str(), connectInfo.m_uiPort, NULL, 0 ))
			this->ThrowError( this->m_pkSQL );
	}
	catch (std::exception& e)
	{
		FV_ERROR_MSG( "MySql::MySql: %s\n", e.what() );
		if (m_pkSQL) mysql_close( m_pkSQL );
		throw;
	}
}

MySql::~MySql()
{
	FV_ASSERT( !m_bInTransaction );
	mysql_close( m_pkSQL );
}

namespace MySqlUtils
{
	FV_INLINE unsigned int getErrno( MYSQL* connection )
	{
		return mysql_errno( connection );
	}

	FV_INLINE unsigned int getErrno( MYSQL_STMT* statement )
	{
		return mysql_stmt_errno( statement );
	}
}

template <class MYSQLOBJ>
void MySql::ThrowError( MYSQLOBJ* failedObj )
{
	unsigned int mySqlErrno = MySqlUtils::getErrno( failedObj );
//	FV_DEBUG_MSG( "MySql::ThrowError: error=%d\n", mySqlErrno );
	switch (mySqlErrno)
	{
		case ER_LOCK_DEADLOCK:
		case ER_LOCK_WAIT_TIMEOUT:
			throw MySqlRetryTransactionException( failedObj );
			break;
		case ER_DUP_ENTRY:
			throw MySqlDuplicateEntryException( failedObj );
			break;
		case CR_SERVER_GONE_ERROR:
		case CR_SERVER_LOST:
			{
				MySqlError serverGone( failedObj );
				this->OnFatalError( serverGone );
				throw serverGone;
			}
			break;
		default:
			throw MySqlError( failedObj );
			break;
	}
}

void MySql::Execute( const FvString& statement, FvBinaryOStream * resdata )
{
	//FV_DEBUG_MSG("MySqlTransaction::Execute: %s\n", statement.c_str());
	if (mysql_real_query( this->m_pkSQL, statement.c_str(), statement.length() ))
		this->ThrowError( this->m_pkSQL );
	MYSQL_RES * result = mysql_store_result( this->m_pkSQL );
	if (result)
	{
		if (resdata != NULL)
		{
			int nrows = (int)mysql_num_rows( result );
			int nfields = mysql_num_fields( result );
			(*resdata) << nrows << nfields;
			MYSQL_ROW arow;
			while ((arow = mysql_fetch_row( result )) != NULL)
			{
				unsigned long *lengths = mysql_fetch_lengths(result);
				for (int i = 0; i < nfields; i++)
				{
					if (arow[i] == NULL)
						(*resdata) << "NULL";
					else
						resdata->AppendString(arow[i],lengths[i]);
				}
			}
		}
		mysql_free_result( result );
	}
}

void MySql::Execute( MySqlUnPrep::Statement& statement )
{
	statement.GiveResult( 0 );
	FvString query = statement.GetQuery( this->m_pkSQL );
	// FV_DEBUG_MSG("MySqlTransaction::Execute: %s\n", query.c_str());
	if (mysql_real_query( this->m_pkSQL, query.c_str(), query.length() ))
		this->ThrowError( this->m_pkSQL );
	MYSQL_RES * result = mysql_store_result( this->m_pkSQL );
	statement.GiveResult( result );
}

void MySql::Execute( MySqlPrep::Statement& stmt )
{
	if (mysql_stmt_execute( stmt.Get() ))
		this->ThrowError( stmt.Get() );

	if (mysql_stmt_store_result( stmt.Get() ))
		this->ThrowError( stmt.Get() );
}

int MySql::Query( const FvString & statement )
{
	int errorNum =
			mysql_real_query( m_pkSQL, statement.c_str(), statement.length() );

	if ((errorNum == CR_SERVER_GONE_ERROR) || (errorNum == CR_SERVER_LOST))
	{
		this->OnFatalError( MySqlError( m_pkSQL ) );
	}

	return errorNum;
}


void MySql::GetTableNames( std::vector<FvString>& tableNames,
							const char * pattern )
{
	tableNames.clear();

	MYSQL_RES * pResult = mysql_list_tables( m_pkSQL, pattern );
	if (pResult)
	{
		tableNames.reserve( (int)mysql_num_rows( pResult ) );

		MYSQL_ROW row;
		while ((row = mysql_fetch_row( pResult )) != NULL)
		{
			unsigned long *lengths = mysql_fetch_lengths(pResult);
			tableNames.push_back( FvString( row[0], lengths[0] ) );
		}
		mysql_free_result( pResult );
	}
}

void MySqlBuffer::PrintTruncateError( unsigned long size,
			unsigned long capacity )
{
	// DECLARE_DEBUG_COMPONENT()
	FV_ERROR_MSG( "MySqlBuffer::Set: truncating data of size %lu to %lu\n",
		size, capacity );
}

FvBinaryIStream& operator>>( FvBinaryIStream& strm, MySqlBuffer& buffer )
{
	int len = strm.ReadStringLength();
	buffer.Set( strm.Retrieve( len ), len );

	return strm;
}

FvBinaryOStream& operator<<( FvBinaryOStream& strm, const MySqlBuffer& buffer )
{
	const char * pBuf = (const char *) buffer.Get();
	if (!pBuf) throw std::runtime_error("string is null");
	strm.AppendString( pBuf, buffer.m_uiSize );

	return strm;
}


MySqlTableMetadata::MySqlTableMetadata( MySql& connection, 
		const FvString tableName ) :
	m_pkResult( mysql_list_fields( connection.Get(), tableName.c_str(), "%" ) )
{
	if (m_pkResult)
	{
		m_uiNumFields = mysql_num_fields(m_pkResult);
		m_pkFields = mysql_fetch_fields(m_pkResult);
	}
	else
	{
		m_uiNumFields = 0;
		m_pkFields = NULL;
	}
}

MySqlTableMetadata::~MySqlTableMetadata()
{
	if (m_pkResult)
	{
		mysql_free_result( m_pkResult );
	}
}
