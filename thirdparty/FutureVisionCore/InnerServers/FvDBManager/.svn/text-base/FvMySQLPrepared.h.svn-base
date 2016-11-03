//{future header message}
#ifndef __FvMySQLPrepared_H__
#define __FvMySQLPrepared_H__

#include "FvMySQLWrapper.h"

#include <mysql.h>
#include <vector>

namespace MySqlPrep
{
	class Bindings
	{
	public:
		typedef std::vector<MYSQL_BIND>::size_type size_type;

	public:
		Bindings& attach( MYSQL_BIND& binding )
		{
			m_kBindings.push_back( binding );
			return *this;
		}

		std::vector<MYSQL_BIND>::size_type size() const
		{
			return m_kBindings.size();
		}

		bool empty() const
		{
			return m_kBindings.empty();
		}

		void clear()
		{
			m_kBindings.clear();
		}

		MYSQL_BIND * get()
		{
			return &m_kBindings[0];
		}

		void reserve( size_type size )
		{
			m_kBindings.reserve( size );
		}

		Bindings& operator+=( const Bindings& rhs )
		{
			m_kBindings.insert( m_kBindings.end(), rhs.m_kBindings.begin(),
					rhs.m_kBindings.end() );
			return *this;
		}

	private:
		std::vector<MYSQL_BIND> m_kBindings;
	};

	class Statement
	{
	public:
		Statement( MySql& con, const FvString& stmt );
		~Statement();

		MYSQL_STMT *Get() { return m_pkSTMT; }

		unsigned int ParamCount() { return mysql_stmt_param_count( m_pkSTMT ); }
		unsigned int ResultCount() { return m_pkMeta? mysql_num_fields( m_pkMeta ) : 0; }
		int ResultRows() { return (int)mysql_stmt_num_rows( m_pkSTMT ); }
		void BindParams( const Bindings& bindings );
		void BindResult( const Bindings& bindings );

		bool Fetch()
		{
			switch (mysql_stmt_fetch( m_pkSTMT ))
			{
			case 0:
				return true;
			case MYSQL_NO_DATA:
				return false;
			default:
				throw MySqlError( m_pkSTMT );
			}
		}

	private:
		Statement( const Statement& );
		void operator=( const Statement& );

		MYSQL_STMT *m_pkSTMT;
		Bindings m_kParamBindings;
		Bindings m_kResultBindings;
		MYSQL_RES *m_pkMeta;
	};

}

FV_INLINE MySqlPrep::Bindings& operator<<( MySqlPrep::Bindings& lhs,
		MySqlPrep::Bindings& rhs )
{
	lhs += rhs;
	return lhs;
}

template < class TYPE >
FV_INLINE MySqlPrep::Bindings& operator<<( MySqlPrep::Bindings& binding, TYPE& x )
{
	MYSQL_BIND b;
	memset( &b, 0, sizeof(b) );
	b.buffer_type = MySqlTypeTraits<TYPE>::colType;
	b.is_unsigned = !std::numeric_limits<TYPE>::is_signed;
	b.buffer      = reinterpret_cast<char*>( &x );
	b.is_null     = NULL;
	return binding.attach( b );
}

template < class TYPE >
FV_INLINE MySqlPrep::Bindings& operator<<( MySqlPrep::Bindings& binding, MySqlValueWithNull<TYPE>& x )
{
	MYSQL_BIND b;
	memset( &b, 0, sizeof(b) );
	b.buffer_type = MySqlTypeTraits<TYPE>::colType;
	b.is_unsigned = !std::numeric_limits<TYPE>::is_signed;
	b.buffer      = reinterpret_cast<char*>( &x.m_kValue );
	b.is_null     = &x.m_bIsNull;
	return binding.attach( b );
}

FV_INLINE MySqlPrep::Bindings& operator<<( MySqlPrep::Bindings& binding, MySqlTimestampNull& x )
{
	MYSQL_BIND b;
	memset( &b, 0, sizeof(b) );
	b.buffer_type = MYSQL_TYPE_TIMESTAMP;
	b.is_unsigned = false;
	b.buffer      = reinterpret_cast<char*>( &x.m_kValue );
	b.is_null     = &x.m_bIsNull;
	return binding.attach( b );
}

FV_INLINE MySqlPrep::Bindings& operator<<( MySqlPrep::Bindings& binding, MySqlBuffer& buffer )
{
	MYSQL_BIND b;
	memset( &b, 0, sizeof(b) );
	b.buffer_type   = buffer.m_eType;
	b.buffer        = buffer.m_pcBuffer;
	b.buffer_length = buffer.m_uiCapacity;
	b.length        = &buffer.m_uiSize;
	b.is_null       = &buffer.m_bIsNull;
	return binding.attach( b );
}

#endif	// __FvMySQLPrepared_H__
