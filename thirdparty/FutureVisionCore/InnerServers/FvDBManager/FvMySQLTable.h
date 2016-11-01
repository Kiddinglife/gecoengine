//{future header message}
#ifndef __FvMySQLTable_H__
#define __FvMySQLTable_H__

#include <FvNetTypes.h>

#include "FvMySQLWrapper.h" 

#include <mysql.h>

#include <sstream>

enum MySQLLimits
{
	MySQLMaxTableNameLen = NAME_LEN,
	MySQLMaxColumnNameLen = NAME_LEN,
	MySQLMaxDbNameLen = NAME_LEN,
	MySQLMaxIndexNameLen = NAME_LEN,
	MySQLMaxInnoDbIndexLen = 767,
	MySQLMaxMyIsamDbIndexLen = 1000	
};

enum BWMySQLLimits
{
	BWMySQLMaxTypeNameLen = 64,
	BWMySQLMaxLogOnNameLen = 255,
	BWMySQLMaxLogOnPasswordLen = 255,
	BWMySQLMaxNamePropertyLen = 255
};

#define TABLE_NAME_PREFIX				"tbl"
#define	DEFAULT_SEQUENCE_COLUMN_NAME	"value"
#define DEFAULT_SEQUENCE_TABLE_NAME		"values"
#define ID_COLUMN_NAME					"id"
#define PARENTID_COLUMN_NAME			"parentID"
#define GAME_TIME_COLUMN_NAME			"gameTime"
#define TIMESTAMP_COLUMN_NAME			"timestamp"

const FvString ID_COLUMN_NAME_STR( ID_COLUMN_NAME );
const FvString PARENTID_COLUMN_NAME_STR( PARENTID_COLUMN_NAME );
const FvString TIMESTAMP_COLUMN_NAME_STR( TIMESTAMP_COLUMN_NAME );

static const unsigned long MAX_SECONDARY_DB_LOCATION_LENGTH = 4096;

struct IMySqlColumnMapping
{
	struct IVisitor
	{
		virtual bool OnVisitColumn( IMySqlColumnMapping& column )= 0;
	};

	enum IndexType
	{
		IndexTypeNone 		= 0,
		IndexTypePrimary	= 1,
		IndexTypeName		= 2,
		IndexTypeParentID	= 3
	};

	struct Type
	{
		enum_field_types m_eFieldType;
		bool m_bIsUnsignedOrBinary;
		unsigned int m_uiLength;
		FvString m_kDefaultValue;
		FvString m_kOnUpdateCmd;
		bool m_bIsAutoIncrement;

		Type( enum_field_types type = MYSQL_TYPE_NULL, 
				bool isUnsignedOrBin = false, unsigned int len = 0, 
				const FvString defVal = FvString(),
				bool isAutoInc = false ) :
			m_eFieldType( type ), m_bIsUnsignedOrBinary( isUnsignedOrBin ),
			m_uiLength( len ), m_kDefaultValue( defVal ), 
			m_bIsAutoIncrement( isAutoInc )
		{}
		Type( const MYSQL_FIELD& field ) :
			m_eFieldType( field.type), 
			m_bIsUnsignedOrBinary( DeriveIsUnsignedOrBinary( field ) ), 
			m_uiLength( field.length ), 
			m_kDefaultValue( field.def ? field.def : FvString() ),
			m_bIsAutoIncrement( field.flags & AUTO_INCREMENT_FLAG )
		{
			if (this->m_eFieldType == MYSQL_TYPE_BLOB)
				this->m_eFieldType = MySqlTypeTraits<FvString>::colType( this->m_uiLength );
		}

		bool IsUnsigned() const			{ return m_bIsUnsignedOrBinary; }
		void SetIsUnsigned( bool val )	{ m_bIsUnsignedOrBinary = val; }

		bool IsBinary()	const			{ return m_bIsUnsignedOrBinary; }
		void SetIsBinary( bool val ) 	{ m_bIsUnsignedOrBinary = val; }

		FvString GetAsString( MySql& connection, IndexType idxType ) const;
		FvString GetDefaultValueAsString( MySql& connection ) const;
		bool IsDefaultValueSupported() const;
		bool IsStringType() const;
		bool IsSimpleNumericalType() const;

		bool operator==( const Type& other ) const;
		bool operator!=( const Type& other ) const
		{	return !this->operator==( other );	}

		static bool DeriveIsUnsignedOrBinary( const MYSQL_FIELD& field );


	private:
		void AdjustBlobTypeForSize();
	};

	virtual const FvString &GetColumnName() const = 0;
	virtual void GetColumnType( Type& type ) const = 0; 
	virtual IndexType GetColumnIndexType() const = 0;
	virtual bool HasBinding() const = 0;

	virtual void AddSelfToBindings( MySqlBindings& bindings ) = 0;
};

const IMySqlColumnMapping::Type PARENTID_COLUMN_TYPE( MYSQL_TYPE_LONGLONG );
const IMySqlColumnMapping::Type ID_COLUMN_TYPE( MYSQL_TYPE_LONGLONG, false, 
		0, FvString(), true );

struct IMySqlIDColumnMapping : public IMySqlColumnMapping
{
	struct IVisitor
	{
		virtual bool OnVisitIDColumn( IMySqlIDColumnMapping& column )= 0;
	};

	virtual FvDatabaseID &GetIDBuffer() = 0;
};

struct IMySqlTableMapping
{
	struct IVisitor
	{
		virtual bool OnVisitTable( IMySqlTableMapping& table ) = 0;
	};

	virtual const FvString &GetTableName() const = 0;
	virtual bool VisitColumnsWith( IMySqlColumnMapping::IVisitor& visitor ) = 0;
	virtual bool VisitIDColumnWith( IMySqlIDColumnMapping::IVisitor& visitor ) = 0;
	virtual bool VisitSubTablesWith( IVisitor& visitor ) = 0;

	struct IRowBuffer
	{
		virtual void AddBoundData() = 0;
		virtual void SetBoundData( int row ) = 0;
		virtual int GetNumRows() const = 0;
		virtual void clear() = 0;
	};
	virtual IRowBuffer *GetRowBuffer() = 0;

	bool HasSubTables();
	FvDatabaseID &GetIDColumnBuffer();
};

struct hasSubTables_SubTableVisitor : public IMySqlTableMapping::IVisitor
{
	virtual bool OnVisitTable( IMySqlTableMapping& table )
	{
		return false;
	}
};
FV_INLINE bool IMySqlTableMapping::HasSubTables()
{
	hasSubTables_SubTableVisitor visitor;

	return !this->VisitSubTablesWith( visitor );
}

struct getIDColumnBuffer_IDColVisitor : public IMySqlIDColumnMapping::IVisitor
{
	FvDatabaseID *m_pkIDColBuf;
	getIDColumnBuffer_IDColVisitor() : m_pkIDColBuf( NULL ) {}
	virtual bool OnVisitIDColumn( IMySqlIDColumnMapping& column )
	{
		m_pkIDColBuf = &column.GetIDBuffer();
		return false;
	}
};
FV_INLINE FvDatabaseID& IMySqlTableMapping::GetIDColumnBuffer()
{
	getIDColumnBuffer_IDColVisitor visitor;
	this->VisitIDColumnWith( visitor );
	return *visitor.m_pkIDColBuf;
}

struct visitSubTablesRecursively_Visitor : public IMySqlTableMapping::IVisitor
{
	IMySqlTableMapping::IVisitor& origVisitor_;
public:
	visitSubTablesRecursively_Visitor( IMySqlTableMapping::IVisitor& origVisitor ) :
		origVisitor_( origVisitor )
	{}
	virtual bool OnVisitTable( IMySqlTableMapping& table )
	{
		return origVisitor_.OnVisitTable( table ) &&
				table.VisitSubTablesWith( *this );
	}
};
FV_INLINE bool VisitSubTablesRecursively( IMySqlTableMapping& table,
		IMySqlTableMapping::IVisitor& visitor )
{
	visitSubTablesRecursively_Visitor proxyVisitor( visitor );
	return proxyVisitor.OnVisitTable( table );
}

template <class VISITOR, class ARG>
class TableVisitorArgPasser : public IMySqlTableMapping::IVisitor
{
	VISITOR &m_kOrigVisitor;
	ARG &m_kArg;
public:
	TableVisitorArgPasser( VISITOR& origVisitor, ARG& arg ) :
		m_kOrigVisitor( origVisitor ), m_kArg( arg )
	{}
	virtual bool OnVisitTable( IMySqlTableMapping& table )
	{
		return m_kOrigVisitor.OnVisitTable( table, m_kArg );
	}
};

template <class VISITOR, class ARG>
class ColumnVisitorArgPasser : public IMySqlColumnMapping::IVisitor
{
	VISITOR &m_kOrigVisitor;
	ARG &m_kArg;
public:
	ColumnVisitorArgPasser( VISITOR& origVisitor, ARG& arg ) :
		m_kOrigVisitor( origVisitor ), m_kArg( arg )
	{}
	virtual bool OnVisitColumn( IMySqlColumnMapping& column )
	{
		return m_kOrigVisitor.OnVisitColumn( column, m_kArg );
	}
};

template < class BOUNDTYPE >
class MySqlColumnMappingAdapter : public IMySqlColumnMapping
{
	const FvString &m_kName;
	const Type &m_kColumnType;
	IndexType m_kIndexType;
	BOUNDTYPE &m_kBindBuffer;

public:
	MySqlColumnMappingAdapter( const FvString& name,
			const Type& columnType, IndexType indexType,
			BOUNDTYPE& bindBuffer ) :
		m_kName( name ), m_kColumnType( columnType ), m_kIndexType( indexType ),
		m_kBindBuffer( bindBuffer )
	{}

	BOUNDTYPE &GetBindBuffer()
	{
		return m_kBindBuffer;
	}

	virtual const FvString& GetColumnName() const
	{
		return m_kName;
	}
	virtual void GetColumnType( Type& type ) const
	{
		type = m_kColumnType;
	}
	virtual IndexType GetColumnIndexType() const
	{
		return m_kIndexType;
	}
	virtual bool HasBinding() const
	{
		return true;
	}
	virtual void AddSelfToBindings( MySqlBindings& bindings )
	{
		bindings << m_kBindBuffer;
	}
};

class MySqlIDColumnMappingAdapter : public MySqlColumnMappingAdapter< FvDatabaseID >,
									public IMySqlIDColumnMapping
{
	typedef MySqlColumnMappingAdapter< FvDatabaseID > BaseClass;
public:
	MySqlIDColumnMappingAdapter( FvDatabaseID& idBinding ) :
		BaseClass( ID_COLUMN_NAME_STR, ID_COLUMN_TYPE,
				IndexTypePrimary, idBinding )
	{}

	virtual const FvString& GetColumnName() const
	{
		return BaseClass::GetColumnName();
	}
	virtual void GetColumnType( Type& type ) const
	{
		BaseClass::GetColumnType( type ); 
	}
	virtual IndexType GetColumnIndexType() const
	{
		return BaseClass::GetColumnIndexType();
	}
	virtual bool HasBinding() const
	{
		return true;
	}
	virtual void AddSelfToBindings( MySqlBindings& bindings )
	{
		BaseClass::AddSelfToBindings( bindings );
	}

	virtual FvDatabaseID& GetIDBuffer()
	{
		return BaseClass::GetBindBuffer();
	}
};

#endif // __FvMySQLTable_H__
