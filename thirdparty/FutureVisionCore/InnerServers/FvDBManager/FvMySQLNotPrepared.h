//{future header message}
#ifndef __FvMySQLNotPrepared_H__
#define __FvMySQLNotPrepared_H__

#include "FvMySQLWrapper.h"

#include <FvSmartPointer.h>
#include <FvDebug.h>

#include <mysql.h>
#include <vector>
#include <sstream>
#include <ostream>
#include <stdexcept>

namespace MySqlUnPrep
{
	class BindColumn : public FvReferenceCount
	{
	public:
		virtual void AddValueToStream( std::ostream&, MYSQL * ) = 0;
		virtual void GetValueFromString( const char * str, int len ) = 0;
	};

	typedef FvSmartPointer<BindColumn> BindColumnPtr;

	class Bindings
	{
	public:
		typedef std::vector<BindColumnPtr>::size_type size_type;
	public:
		Bindings &attach( BindColumnPtr binding )
		{
			m_kBindings.push_back( binding );
			return *this;
		}

		std::vector<BindColumnPtr>::size_type size() const
		{
			return m_kBindings.size();
		}

		void clear()
		{
			m_kBindings.clear();
		}

		BindColumnPtr * get()
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
		std::vector<BindColumnPtr> m_kBindings;
	};

	class Statement
	{
	public:
		Statement( MySql& con, const FvString& stmt );
		~Statement();

		unsigned int ParamCount() { return m_kParams.size(); }
		unsigned int ResultCount() { return m_kResults.size(); }
		int ResultRows() { return m_pkResultSet? (int)mysql_num_rows( m_pkResultSet ) : 0; }
		void BindParams( const Bindings& bindings );
		void BindResult( const Bindings& bindings );

		bool Fetch();

		FvString GetQuery( MYSQL * sql );
		void GiveResult( MYSQL_RES * resultSet );

	private:
		Statement( const Statement& );
		void operator=( const Statement& );

		MYSQL_RES *m_pkResultSet;
		std::vector<FvString> m_kQueryParts;
		Bindings m_kParams;
		Bindings m_kResults;
	};
}

namespace StringConv
{
	FV_INLINE void ToValue( float& value, const char * str )
	{
		char* remainder;
#ifndef _WIN32
		value = strtof( str, &remainder );
#else // !_WIN32
		value = (float)strtod( str, &remainder );
#endif // _WIN32
		if (*remainder)
			throw std::runtime_error( "not a number" );
	}

	FV_INLINE void ToValue( double& value, const char * str )
	{
		char* remainder;
		value = strtod( str, &remainder );
		if (*remainder)
			throw std::runtime_error( "not a number" );
	}
	
	FV_INLINE void ToValue( FvInt32& value, const char * str )
	{
		char* remainder;
		value = strtol( str, &remainder, 10 );
		if (*remainder)
			throw std::runtime_error( "not a number" );
	}


	FV_INLINE void ToValue( FvInt8& value, const char * str )
	{
		FvInt32 i;
		ToValue( i, str );
		value = FvInt8(i);
		if (value != i)
			throw std::runtime_error( "out of range" );
	}

	FV_INLINE void ToValue( FvInt16& value, const char * str )
	{
		FvInt32 i;
		ToValue( i, str );
		value = FvInt16(i);
		if (value != i)
			throw std::runtime_error( "out of range" );
	}

	FV_INLINE void ToValue( FvUInt32& value, const char * str )
	{
		char* remainder;
		value = strtoul( str, &remainder, 10 );
		if (*remainder)
			throw std::runtime_error( "not a number" );
	}

	FV_INLINE void ToValue( FvUInt8& value, const char * str )
	{
		FvUInt32 ui;
		ToValue( ui, str );
		value = FvUInt8(ui);
		if (value != ui)
			throw std::runtime_error( "out of range" );
	}

	FV_INLINE void ToValue( FvUInt16& value, const char * str )
	{
		FvUInt32 ui;
		ToValue( ui, str );
		value = FvUInt16(ui);
		if (value != ui)
			throw std::runtime_error( "out of range" );
	}

	FV_INLINE void ToValue( FvInt64& value, const char * str )
	{
		char* remainder;
#ifndef _WIN32
		value = strtoll( str, &remainder, 10 );
#else // !_WIN32
		value = _strtoi64( str, &remainder, 10 );
#endif // _WIN32
		if (*remainder)
			throw std::runtime_error( "not a number" );
	}

	FV_INLINE void ToValue( FvUInt64& value, const char * str )
	{
		char* remainder;
#ifndef _WIN32
		value = strtoull( str, &remainder, 10 );
#else // !_WIN32
		value = _strtoui64( str, &remainder, 10 );
#endif // _WIN32
		if (*remainder)
			throw std::runtime_error( "not a number" );
	}

	FV_INLINE void ToValue( bool& value, const char * str )
	{
		if(*str)
			value = true;
		else
			value = false;
	}

	FV_INLINE void ToValue( MySqlTimestampNull& value, const char * str )
	{
		MYSQL_TIME& timestamp = value.GetBuf();

		int numAssigned = sscanf( str, "%d-%d-%d %d:%d:%d", &timestamp.year,
				&timestamp.month, &timestamp.day, &timestamp.hour,
				&timestamp.minute, &timestamp.second );
		if (numAssigned == 6)
		{
			timestamp.second_part = 0;
			value.Valuefy();
		}
		else
		{
			throw std::runtime_error( "not a timestamp" );
		}
	}
	
	template <class TYPE>
	FV_INLINE FvString ToStr( const TYPE& value )
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	FV_INLINE FvString ToStr( FvInt8 value )
	{
		return ToStr<int>( value );
	}
	FV_INLINE FvString ToStr( FvUInt8 value )
	{
		return ToStr<int>( value );
	}
}

namespace MySqlUnPrep
{
	template < class TYPE, class STRM_TYPE >
	class ValueWithNullBinding : public BindColumn
	{
		MySqlValueWithNull<TYPE>& x_;

	public:
		ValueWithNullBinding( MySqlValueWithNull<TYPE>& x ) : x_(x) {}
		void AddValueToStream( std::ostream& os, MYSQL * sql )
		{
			const TYPE * val = x_.Get();
			if (val)
				os << STRM_TYPE(*val);
			else
				os << "NULL";
		}
		void GetValueFromString( const char * str, int len )
		{
			if (str)
			{
				TYPE value;
				StringConv::ToValue( value, str );
				x_.Set( value );
			}
			else
			{
				x_.Nullify();
			}
		}
	};

	template < class TYPE, class STRM_TYPE >
	class ValueBinding : public BindColumn
	{
		TYPE& x_;

	public:
		ValueBinding( TYPE& x ) : x_(x) {}
		void AddValueToStream( std::ostream& os, MYSQL * sql )
		{
			os << STRM_TYPE(x_);
		}
		void GetValueFromString( const char * str, int len )
		{
			if (!str)
				throw std::runtime_error("NULL not supported on this field");
			StringConv::ToValue( x_, str );
		}
	};
	
	class MySqlTimestampNullBinding : public BindColumn
	{
		MySqlTimestampNull& x_;

	public:
		MySqlTimestampNullBinding( MySqlTimestampNull& x ) : x_(x) {}
		void AddValueToStream( std::ostream& os, MYSQL * sql )
		{
			const MYSQL_TIME* pMySqlTime = x_.Get();
			if (pMySqlTime)
			{
				os << pMySqlTime->year << '-' << pMySqlTime->month << '-' 
						<< pMySqlTime->day << ' ' << pMySqlTime->hour << ':' 
						<< pMySqlTime->minute << pMySqlTime->second;
			}
			else
			{
				os << "NULL";
			}
		}
		void GetValueFromString( const char * str, int len )
		{
			if (str)
			{
				StringConv::ToValue( x_, str );
			}
			else
			{
				x_.Nullify();
			}
		}
	};
	
}

FV_INLINE MySqlUnPrep::Bindings& operator<<( MySqlUnPrep::Bindings& lhs,
										  MySqlUnPrep::Bindings& rhs )
{
	lhs += rhs;
	return lhs;
}

template < class TYPE >
FV_INLINE MySqlUnPrep::Bindings& operator<<( MySqlUnPrep::Bindings& b,
										  MySqlValueWithNull< TYPE >& x )
{
	b.attach( new MySqlUnPrep::ValueWithNullBinding< TYPE, TYPE >(x) );
	return b;
}

template < class TYPE >
FV_INLINE MySqlUnPrep::Bindings& operator<<( MySqlUnPrep::Bindings& b, TYPE& x )
{
	b.attach( new MySqlUnPrep::ValueBinding< TYPE, TYPE >(x) );
	return b;
}

FV_INLINE MySqlUnPrep::Bindings& operator<<( MySqlUnPrep::Bindings& b, FvInt8& x )
{
	b.attach( new MySqlUnPrep::ValueBinding< FvInt8, int >(x) );
	return b;
}
FV_INLINE MySqlUnPrep::Bindings& operator<<( MySqlUnPrep::Bindings& b, FvUInt8& x )
{
	b.attach( new MySqlUnPrep::ValueBinding< FvUInt8, int >(x) );
	return b;
}
FV_INLINE MySqlUnPrep::Bindings& operator<<( MySqlUnPrep::Bindings& b,
										  MySqlValueWithNull< FvInt8 >& x )
{
	b.attach( new MySqlUnPrep::ValueWithNullBinding< FvInt8, int >(x) );
	return b;
}
FV_INLINE MySqlUnPrep::Bindings& operator<<( MySqlUnPrep::Bindings& b,
										  MySqlValueWithNull< FvUInt8 >& x )
{
	b.attach( new MySqlUnPrep::ValueWithNullBinding< FvUInt8, int >(x) );
	return b;
}

MySqlUnPrep::Bindings& operator<<( MySqlUnPrep::Bindings&, MySqlBuffer& );

FV_INLINE MySqlUnPrep::Bindings& operator<<( MySqlUnPrep::Bindings& b,
		MySqlTimestampNull& x )
{
	b.attach( new MySqlUnPrep::MySqlTimestampNullBinding(x) );
	return b;
}

#endif // __FvMySQLNotPrepared_H__
