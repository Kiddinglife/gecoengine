#include "FvMySQLTable.h"

FvString IMySqlColumnMapping::Type::GetAsString( MySql& connection,
		IndexType idxType ) const
{
	struct ResultStream
	{
		std::stringstream 	ss;
		bool				isUnsignedOrBinary;

		ResultStream( bool isUnsignedOrBinaryIn ) : 
			isUnsignedOrBinary( isUnsignedOrBinaryIn )
		{}

		void addType( const char * typeName )
		{
			ss << typeName;
		}
		void addNumericalType( const char * typeName )
		{
			ss << typeName;
			if (isUnsignedOrBinary)
			{
				ss << " UNSIGNED";
			}
		}
		void addVarLenType( const char * typeName, unsigned int length )
		{
			ss << typeName << '(' << length << ')';
		}
		void addVarLenStrType( unsigned int length, const char * prefix = "" )
		{
			const char * charTypeStr = isUnsignedOrBinary ? "BINARY" : "CHAR";
			ss << prefix << charTypeStr << '(' << length << ')';
		}
		void addMultiLenType( const char * lenType )
		{
			const char * blobTypeStr = isUnsignedOrBinary ? "BLOB" : "TEXT";
			ss << lenType << blobTypeStr;
		}
		void addAutoIncrement()
		{
			ss << " AUTO_INCREMENT";
		}
		void addDefaultValue( const FvString& defaultValue, 
				MySql* pConnection )
		{
			if (!defaultValue.empty())
			{
				ss << " DEFAULT ";
				if (pConnection)
				{
					ss << '\'' 
						<<	MySqlEscapedString( *pConnection, defaultValue )
						<< '\''; 
				}
				else
				{
					ss << defaultValue;
				}
			}
		}
		void addPrimaryKey()
		{
			ss << " PRIMARY KEY";
		}
		void addNotNull()
		{
			ss << " NOT NULL";
		}
		void addOnUpdate( const FvString & cmd )
		{
			ss << " ON UPDATE " << cmd;
		}
	} result( this->m_bIsUnsignedOrBinary );

	switch (m_eFieldType)
	{
		case MYSQL_TYPE_TINY:
			result.addNumericalType( "TINYINT" );
			break;
		case MYSQL_TYPE_SHORT:
			result.addNumericalType( "SMALLINT" );
			break;
		case MYSQL_TYPE_INT24:
			result.addNumericalType( "MEDIUMINT" );
			break;
		case MYSQL_TYPE_LONG:
			result.addNumericalType( "INT" );
			break;
		case MYSQL_TYPE_LONGLONG:
			result.addNumericalType( "BIGINT" );
			break;
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
			result.addNumericalType( "DECIMAL" );
			break;
		case MYSQL_TYPE_FLOAT:
			result.addType( "FLOAT" );
			break;
		case MYSQL_TYPE_DOUBLE:
			result.addType( "DOUBLE" );
			break;
		case MYSQL_TYPE_TIMESTAMP:
			result.addType( "TIMESTAMP" );
			break;
		case MYSQL_TYPE_TIME:
			result.addType( "TIME" );
			break;
		case MYSQL_TYPE_NEWDATE:
			result.addType( "DATE" );
			break;
		case MYSQL_TYPE_YEAR:
			result.addVarLenType( "YEAR", this->m_uiLength );
			break;
		case MYSQL_TYPE_VARCHAR:
			result.addVarLenType( "VARCHAR", this->m_uiLength );
			break;
		case MYSQL_TYPE_TINY_BLOB:
			result.addMultiLenType( "TINY" );
			break;
		case MYSQL_TYPE_MEDIUM_BLOB:
			result.addMultiLenType( "MEDIUM" );
			break;
		case MYSQL_TYPE_LONG_BLOB:
			result.addMultiLenType( "LONG" );
			break;
		case MYSQL_TYPE_BLOB:
			result.addMultiLenType( "" );
			break;
		case MYSQL_TYPE_BIT:
			result.addVarLenType( "BIT", this->m_uiLength );
			break;
		case MYSQL_TYPE_VAR_STRING:
			result.addVarLenStrType( this->m_uiLength, "VAR" );
			break;
		case MYSQL_TYPE_STRING:
			result.addVarLenStrType( this->m_uiLength );
			break;
//		case MYSQL_NULL:
//		case MYSQL_TYPE_SET:
//		case MYSQL_TYPE_GEOMETRY:
//		case MYSQL_TYPE_ENUM:
		default:
			FV_ASSERT( false );
			break;
	}

	if (this->m_bIsAutoIncrement)
	{
		result.addAutoIncrement();
	}
	else if (this->IsDefaultValueSupported())
	{
		result.addDefaultValue( this->m_kDefaultValue, 
				this->IsStringType() ? &connection : NULL );
	}

	if (!this->m_kOnUpdateCmd.empty())
	{
		result.addOnUpdate( this->m_kOnUpdateCmd );
	}

	if (idxType == IndexTypePrimary)
	{
		result.addPrimaryKey();
	}

	result.addNotNull();

	return result.ss.str();
}

FvString IMySqlColumnMapping::Type::GetDefaultValueAsString( 
		MySql& connection ) const
{
	if (this->IsStringType())
	{
		std::stringstream ss;
		ss << '\'' <<  MySqlEscapedString( connection, this->m_kDefaultValue )
				<< '\'';
		return ss.str();
	}
	else
	{
		return this->m_kDefaultValue;
	}
}

bool IMySqlColumnMapping::Type::IsDefaultValueSupported() const
{
	switch (this->m_eFieldType)
	{
		case MYSQL_TYPE_TINY_BLOB:
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		case MYSQL_TYPE_BLOB:
			return false;
		default:
			return true;
	}
}

bool IMySqlColumnMapping::Type::IsStringType() const
{
	switch (this->m_eFieldType)
	{
		case MYSQL_TYPE_TINY_BLOB:
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		case MYSQL_TYPE_BLOB:
		case MYSQL_TYPE_VAR_STRING:
		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_VARCHAR:
			return true;
		default:
			return false;
	}
}

bool IMySqlColumnMapping::Type::IsSimpleNumericalType() const
{
	switch (this->m_eFieldType)
	{
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_FLOAT:
		case MYSQL_TYPE_DOUBLE:
		case MYSQL_TYPE_TINY:
		case MYSQL_TYPE_SHORT:
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_LONGLONG:
			return true;
		default:
			return false;
	}
}

bool IMySqlColumnMapping::Type::operator==( const Type& other ) const
{
	struct HelperFunctions
	{
		static bool equalOptionalStuff( const Type& self, const Type& other )
		{
			switch (self.m_eFieldType)
			{
				case MYSQL_TYPE_DECIMAL:
				case MYSQL_TYPE_NEWDECIMAL:
				case MYSQL_TYPE_FLOAT:
				case MYSQL_TYPE_DOUBLE:
				case MYSQL_TYPE_TIMESTAMP:
				case MYSQL_TYPE_TIME:
				case MYSQL_TYPE_NEWDATE:
				case MYSQL_TYPE_BIT://! add by Uman, 20101122
					return true;
				case MYSQL_TYPE_TINY:
				case MYSQL_TYPE_SHORT:
				case MYSQL_TYPE_INT24:
				case MYSQL_TYPE_LONG:
				case MYSQL_TYPE_LONGLONG:
					return (self.m_bIsUnsignedOrBinary == other.m_bIsUnsignedOrBinary)
							&& (self.m_bIsAutoIncrement == other.m_bIsAutoIncrement);
				case MYSQL_TYPE_TINY_BLOB:
				case MYSQL_TYPE_MEDIUM_BLOB:
				case MYSQL_TYPE_LONG_BLOB:
				case MYSQL_TYPE_BLOB:
					return self.m_bIsUnsignedOrBinary == other.m_bIsUnsignedOrBinary;
				case MYSQL_TYPE_VAR_STRING:
				case MYSQL_TYPE_STRING:
					return (self.m_uiLength == other.m_uiLength) && 
						self.m_bIsUnsignedOrBinary == other.m_bIsUnsignedOrBinary;
				case MYSQL_TYPE_YEAR:
				case MYSQL_TYPE_VARCHAR:
					return self.m_uiLength == other.m_uiLength;
				default:
					FV_ASSERT( false );
					return true;
			}
		}

		static bool equalDefaultValue( const Type& self, const Type& other )
		{
			if (self.m_bIsAutoIncrement || !self.IsDefaultValueSupported())
				return true;

			if (self.m_kDefaultValue == other.m_kDefaultValue)
				return true;

			if (self.IsSimpleNumericalType())
			{
				return (self.m_kDefaultValue.empty() && (other.m_kDefaultValue == "0")) ||
					((self.m_kDefaultValue == "0") && other.m_kDefaultValue.empty());
			}

			if (self.m_eFieldType == MYSQL_TYPE_STRING)
			{
				FvString nullString( self.m_uiLength, 
						self.IsBinary() ? '\0' : ' ' );
				return (self.m_kDefaultValue.empty() && 
								(other.m_kDefaultValue == nullString)) ||
						((self.m_kDefaultValue == nullString) && 
								other.m_kDefaultValue.empty());
			}

			if (self.m_eFieldType == MYSQL_TYPE_TIMESTAMP)
			{
				return true;
			}

			return false;
		}
	};

	return (this->m_eFieldType == other.m_eFieldType) &&
			HelperFunctions::equalOptionalStuff( *this, other ) &&
			(this->m_bIsAutoIncrement == other.m_bIsAutoIncrement) &&
			HelperFunctions::equalDefaultValue( *this, other );
}

bool IMySqlColumnMapping::Type::DeriveIsUnsignedOrBinary( 
		const MYSQL_FIELD& field )
{
	switch (field.type)
	{
		case MYSQL_TYPE_TINY:
		case MYSQL_TYPE_SHORT:
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_LONGLONG:
			return (field.flags & UNSIGNED_FLAG);
		case MYSQL_TYPE_TINY_BLOB:
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		case MYSQL_TYPE_BLOB:
		case MYSQL_TYPE_VAR_STRING:
		case MYSQL_TYPE_STRING:
			return (field.charsetnr == 63);
		default:
			return false;
	}
}
