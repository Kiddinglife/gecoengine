#include "FvMySQLNotPrepared.h"
#include "FvMySQLWrapper.h"

#include <FvDebug.h>
#include <FvBinaryStream.h>

#include <sstream>

FV_DECLARE_DEBUG_COMPONENT(0);

namespace MySqlUnPrep
{
	Statement::Statement( MySql&,const FvString& stmt ) :
		m_pkResultSet(0)
	{
		using namespace std;

		if ( stmt == "" )
			return;

		string::const_iterator last = stmt.begin();
		for ( string::const_iterator i = stmt.begin();
				i != stmt.end(); ++i )
		{
			if (*i == '?')
			{
				m_kQueryParts.push_back( string(last,i) );
				last = i+1;
			}
		}
		m_kQueryParts.push_back( string(last,stmt.end()) );
	}

	Statement::~Statement()
	{
		this->GiveResult( NULL );
	}

	void Statement::BindParams( const Bindings& bindings )
	{
		if (m_kQueryParts.size())
		{
			FV_ASSERT( bindings.size() == m_kQueryParts.size()-1 );
			m_kParams = bindings;
		}
	}

	void Statement::BindResult( const Bindings& bindings )
	{
		if (m_kQueryParts.size())
			m_kResults = bindings;
	}

	FvString Statement::GetQuery( MYSQL * sql )
	{
		using namespace std;

		if (!m_kQueryParts.size())
			throw std::runtime_error( "no such query exists" );

		ostringstream os;
		os.precision(8);
		vector<string>::const_iterator pQueryPart = m_kQueryParts.begin();
		os << *pQueryPart;

		if(m_kParams.size())
		{
			BindColumnPtr * ppBinding = m_kParams.get();
			while (++pQueryPart != m_kQueryParts.end())
			{
				(*ppBinding)->AddValueToStream( os, sql );
				++ppBinding;
				os << *pQueryPart;
			}
		}

		return os.str();
	}

	void Statement::GiveResult( MYSQL_RES * resultSet )
	{
		if (m_pkResultSet)
			mysql_free_result( m_pkResultSet );
		m_pkResultSet = resultSet;
		if (m_pkResultSet)
		{
			if (mysql_num_fields( m_pkResultSet ) != m_kResults.size())
			{
				FV_WARNING_MSG( "MySqlUnPrep::Statement::GiveResult: "
						"size mismatch; got %i fields, but expected %lu\n",
						mysql_num_fields( m_pkResultSet ), m_kResults.size() );
				mysql_free_result( m_pkResultSet );
				m_pkResultSet = 0;
			}
		}
	}

	bool Statement::Fetch()
	{
		if (!m_pkResultSet)
			throw std::runtime_error( "error fetching results" );
		MYSQL_ROW row = mysql_fetch_row( m_pkResultSet );
		unsigned long * lengths = mysql_fetch_lengths( m_pkResultSet );
		if (!row)
			return false;
		BindColumnPtr * ppBinding = m_kResults.get();
		for (size_t i=0; i<m_kResults.size(); ++i)
			ppBinding[i]->GetValueFromString( row[i], lengths[i] );
		return true;
	}
}

namespace
{
	struct BindBuffer : public MySqlUnPrep::BindColumn
	{
		BindBuffer( MySqlBuffer * x ) : x_(x) {}
		void AddValueToStream( std::ostream& os, MYSQL * sql )
		{
			if ( x_->IsNull() )
				os << "NULL";
			else
			{
				char * buffer = new char[1 + 2*x_->size()];
				mysql_real_escape_string( sql, buffer, (const char*)x_->Get(), x_->size() );
				os << '\'' << buffer << '\'';
				delete[] buffer;
			}
		}
		void GetValueFromString( const char * str, int len )
		{
			if (str)
				x_->Set( str, len );
			else
				x_->Nullify();
		}
		MySqlBuffer * x_;
	};
}

MySqlUnPrep::Bindings& operator<<( MySqlUnPrep::Bindings& b, MySqlBuffer& x )
{
	b.attach( new BindBuffer(&x) );
	return b;
}
