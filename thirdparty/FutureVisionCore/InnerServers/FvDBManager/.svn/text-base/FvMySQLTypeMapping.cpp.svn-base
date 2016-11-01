#include "FvMySQLTypeMapping.h"
#include "FvDatabase.h"
#include "FvDBEntityDefs.h"
#include "FvMySQLNotPrepared.h"
#include <FvXMLSection.h>
#include <FvDebug.h>
#include <FvBase64.h>
#include <FvMemoryStream.h>
#include <FvSmartPointer.h>
#include <FvUniqueID.h>
#include <FvServerConfig.h>
#include <FvEntityDescriptionMap.h>
#include <FvDataTypes.h>

#include <vector>
#include <set>
#include <map>
#include <stack>

FV_DECLARE_DEBUG_COMPONENT( 0 );

#define PRIMARY_INDEX_NAME 	"PRIMARY"
#define PARENTID_INDEX_NAME	"parentIDIndex"

const FvString PRIMARY_INDEX_NAME_STR( PRIMARY_INDEX_NAME );
const FvString PARENTID_INDEX_NAME_STR( PARENTID_INDEX_NAME );

namespace
{
	FvString generateIndexName( const FvString& colName )
	{
		FvString::size_type underscorePos = colName.find( '_' );
		return (underscorePos == FvString::npos) ?
			colName + "Index" :
			colName.substr( underscorePos + 1 ) + "Index";
	}

	struct IndexInfoBuffers
	{
		MySqlBuffer				tableName;
		int						nonUnique;
		MySqlBuffer				keyName;
		int						indexSeq;
		MySqlBuffer				columnName;
		MySqlBuffer				collation;
		int						cardinality;
		MySqlValueWithNull<int>	subPart;
		MySqlBuffer				packed;
		MySqlBuffer				nullable;
		MySqlBuffer				indexType;
		MySqlBuffer				comment;

		IndexInfoBuffers( MySqlBindings& bindings ) :
			tableName( MySQLMaxTableNameLen ),
			keyName( MySQLMaxIndexNameLen ),
			columnName( MySQLMaxColumnNameLen ),
			collation( 64 ),
			packed( 64 ),
			nullable( 16 ),
			indexType( 64 ),
			comment( 256 )
		{
			bindings << tableName << nonUnique << keyName << indexSeq
					<< columnName << collation << cardinality << subPart
					<< packed << nullable << indexType << comment;
		}
	};

}

class TableIndices
{
	typedef std::map< FvString, FvString > ColumnToIndexMap;
	ColumnToIndexMap	colToIndexMap_;

public:
	TableIndices( MySql& connection, const FvString& tableName );

	const FvString* getIndexName( const FvString& colName ) const
	{
		ColumnToIndexMap::const_iterator found = colToIndexMap_.find(colName);
		return (found != colToIndexMap_.end()) ? &found->second : NULL;
	}
};

TableIndices::TableIndices( MySql& connection, const FvString& tableName )
{
	std::stringstream query;
	query << "SHOW INDEX FROM " << tableName;
	MySqlStatement getIndexesStmt( connection, query.str() );

	MySqlBindings bindings;
	IndexInfoBuffers buf( bindings );
	getIndexesStmt.BindResult( bindings );

	connection.Execute( getIndexesStmt );
	while ( getIndexesStmt.Fetch() )
	{
		colToIndexMap_[ buf.columnName.GetString() ] = buf.keyName.GetString();
	}
}


TableMetaData::ColumnInfo::ColumnInfo( const MYSQL_FIELD& field,
		const TableIndices& indices ) :
	m_kColumnType( field ), m_kIndexType( DeriveIndexType( field, indices ) )
{}

TableMetaData::ColumnInfo::ColumnInfo() :
	m_kColumnType( MYSQL_TYPE_NULL, false, 0, FvString() ),
	m_kIndexType( IMySqlColumnMapping::IndexTypeNone )
{}

bool TableMetaData::ColumnInfo::operator==( const ColumnInfo& other ) const
{
	return (this->m_kColumnType == other.m_kColumnType) &&
			(this->m_kIndexType == other.m_kIndexType);
}

IMySqlColumnMapping::IndexType TableMetaData::ColumnInfo::DeriveIndexType(
		const MYSQL_FIELD& field, const TableIndices& indices )
{
	if (field.flags & PRI_KEY_FLAG)
	{
		return IMySqlColumnMapping::IndexTypePrimary;
	}
	else if (field.flags & UNIQUE_KEY_FLAG)
	{
		const FvString* pIndexName = indices.getIndexName( field.name );
		FV_ASSERT( pIndexName );
		if (*pIndexName == generateIndexName( field.name ))
		{
			return IMySqlColumnMapping::IndexTypeName;
		}
		else
		{
			FV_WARNING_MSG( "TableMetaData::ColumnInfo::DeriveIndexType: "
					"Found unknown unique index %s for column %s\n",
					pIndexName->c_str(), field.name );
		}
	}
	else if (field.flags & MULTIPLE_KEY_FLAG)
	{
		const FvString* pIndexName = indices.getIndexName( field.name );
		FV_ASSERT( pIndexName );
		if (*pIndexName == PARENTID_INDEX_NAME_STR)
		{
			return IMySqlColumnMapping::IndexTypeParentID;
		}
		else
		{
			FV_WARNING_MSG( "TableMetaData::ColumnInfo::DeriveIndexType: "
					"Found unknown multiple key index %s for column %s\n",
					pIndexName->c_str(), field.name );
		}
	}

	return IMySqlColumnMapping::IndexTypeNone;
}

void TableMetaData::GetEntityTables( StrSet& tables, MySql& connection )
{
	MySqlUnPrep::Statement stmtGetTables( connection,
			"SHOW TABLES LIKE '"TABLE_NAME_PREFIX"_%'" );

	MySqlBuffer 			bufferTableName( MySQLMaxTableNameLen );
	MySqlUnPrep::Bindings	bindings;
	bindings << bufferTableName;
	stmtGetTables.BindResult( bindings );

	connection.Execute( stmtGetTables );
	while ( stmtGetTables.Fetch() )
	{
		tables.insert( bufferTableName.GetString() );
	}
}

void TableMetaData::GetTableColumns( TableMetaData::NameToColInfoMap& columns,
		MySql& connection, const FvString& tableName )
{
	MySqlTableMetadata	tableMetadata( connection, tableName );
	if (tableMetadata.IsValid())	// table exists
	{
		TableIndices tableIndices( connection, tableName );
		for (unsigned int i = 0; i < tableMetadata.GetNumFields(); i++)
		{
			const MYSQL_FIELD& field = tableMetadata.GetField( i );
			columns[ field.name ] =
				TableMetaData::ColumnInfo( field, tableIndices );
		}
	}
}

FutureVisionMetaData::FutureVisionMetaData( MySql& connection ) :
	m_kConnection( connection ),
	m_kSTMTGetEntityTypeID( m_kConnection,
			"SELECT FutureVisionID FROM FutureVisionEntityTypes WHERE name=?" ),
	m_kSTMTSetEntityTypeID( m_kConnection,
			"UPDATE FutureVisionEntityTypes SET FutureVisionID=? WHERE name=?" ),
	m_kSTMTAddEntityType( m_kConnection,
			"INSERT INTO FutureVisionEntityTypes (typeID, FutureVisionID, name)"
			"VALUES (NULL, ?, ?)" ),
	m_kSTMTRemoveEntityType( m_kConnection,
			"DELETE FROM FutureVisionEntityTypes WHERE name=?" ),
	m_kBufferTypeName( BWMySQLMaxTypeNameLen ),
	m_iBufferInteger( -1 )
{
	MySqlBindings b;

	b.clear();
	b << m_kBufferTypeName;
	m_kSTMTGetEntityTypeID.BindParams( b );

	b.clear();
	b << m_iBufferInteger;
	m_kSTMTGetEntityTypeID.BindResult( b );

	b.clear();
	b << m_iBufferInteger << m_kBufferTypeName;
	m_kSTMTSetEntityTypeID.BindParams( b );

	b.clear();
	b << m_iBufferInteger << m_kBufferTypeName;
	m_kSTMTAddEntityType.BindParams( b );

	b.clear();
	b << m_kBufferTypeName;
	m_kSTMTRemoveEntityType.BindParams( b );
}

FvEntityTypeID FutureVisionMetaData::GetEntityTypeID( const FvString& entityName )
{
	m_kBufferTypeName.SetString( entityName );
	m_kConnection.Execute( m_kSTMTGetEntityTypeID );

	FvEntityTypeID typeID = INVALID_TYPEID;
	if (m_kSTMTGetEntityTypeID.ResultRows() > 0)
	{
		FV_ASSERT(m_kSTMTGetEntityTypeID.ResultRows() == 1);

		m_kSTMTGetEntityTypeID.Fetch();
		typeID = m_iBufferInteger;
	}
	return typeID;
}

void FutureVisionMetaData::SetEntityTypeID( const FvString& entityName,
		FvEntityTypeID typeID	)
{
	m_iBufferInteger = typeID;
	m_kBufferTypeName.SetString( entityName );
	m_kConnection.Execute( m_kSTMTSetEntityTypeID );
}

void FutureVisionMetaData::AddEntityType( const FvString& entityName,
		FvEntityTypeID typeID )
{
	m_iBufferInteger = typeID;
	m_kBufferTypeName.SetString( entityName );
	m_kConnection.Execute( m_kSTMTAddEntityType );
}

void FutureVisionMetaData::RemoveEntityType( const FvString& entityName )
{
	m_kBufferTypeName.SetString( entityName );
	m_kConnection.Execute( m_kSTMTRemoveEntityType );
}


namespace
{
	void classifyColumns( TableMetaData::NameToColInfoMap& oldColumns,
		TableMetaData::NameToColInfoMap& newColumns,
		TableMetaData::NameToUpdatedColInfoMap& updatedColumns )
	{
		typedef TableMetaData::NameToColInfoMap 		Columns;
		typedef TableMetaData::NameToUpdatedColInfoMap 	UpdatedColumns;

		for ( Columns::iterator oldCol = oldColumns.begin();
				oldCol != oldColumns.end(); /*++oldCol*/ )
		{
			Columns::iterator newCol = newColumns.find( oldCol->first );
			if (newCol != newColumns.end())
			{
				if (newCol->second != oldCol->second)
				{
					updatedColumns.insert(
							UpdatedColumns::value_type( newCol->first,
									TableMetaData::UpdatedColumnInfo(
											newCol->second, oldCol->second )));
				}

				TableMetaData::NameToColInfoMap::iterator curOldCol = oldCol;
				++oldCol;
				oldColumns.erase( curOldCol );
				newColumns.erase( newCol );
			}
			else
			{
				++oldCol;
			}
		}
	}

	void CreateEntityTableIndex( MySqlTransaction& transaction,
		const FvString& tblName, const FvString& colName,
		const TableMetaData::ColumnInfo& colInfo )
	{
		CreateEntityTableIndex( transaction.Get(), tblName, colName, colInfo );
	}
}
	void CreateEntityTableIndex( MySql& connection,
		const FvString& tblName, const FvString& colName,
		const TableMetaData::ColumnInfo& colInfo )
	{
		switch (colInfo.m_kIndexType)
		{
			case IMySqlColumnMapping::IndexTypeNone:
				break;
			case IMySqlColumnMapping::IndexTypePrimary:
				break;
			case IMySqlColumnMapping::IndexTypeName:
				{
					const char * indexLengthConstraint = "";
					if (colInfo.m_kColumnType.m_eFieldType != MYSQL_TYPE_VAR_STRING)
					{
						indexLengthConstraint = "(255)";
					}
					FvString indexName = generateIndexName( colName );

					try
					{
						connection.Execute( "CREATE UNIQUE INDEX " + indexName +
							" ON " + tblName + " (" + colName +
							indexLengthConstraint + ")" );
					}
					catch (...)
					{
						FV_ERROR_MSG( "CreateEntityTableIndex: Failed to create "
								"name index on column '%s.%s'. Please ensure "
								"all that values in the column are unique "
								"before attempting to create a name index.\n",
								tblName.c_str(), colName.c_str() );
						throw;
					}
				}
				break;
			case IMySqlColumnMapping::IndexTypeParentID:
				connection.Execute( "CREATE INDEX "PARENTID_INDEX_NAME" ON " +
						tblName + " (" + colName + ")" );
				break;
			default:
				FV_CRITICAL_MSG( "CreateEntityTableIndex: Unknown index type %d\n",
						colInfo.m_kIndexType );
				break;
		}
	}

namespace
{
	void removeEntityTableIndex( MySql& connection,
		const FvString& tblName, const FvString& colName,
		IMySqlColumnMapping::IndexType indexType )
	{

		try
		{
			switch (indexType)
			{
				case IMySqlColumnMapping::IndexTypeNone:
					break;
				case IMySqlColumnMapping::IndexTypePrimary:

					break;
				case IMySqlColumnMapping::IndexTypeName:
					{
						FvString indexName = generateIndexName( colName );

						connection.Execute( "ALTER TABLE " + tblName +
								" DROP INDEX " + indexName );
					}
					break;
				case IMySqlColumnMapping::IndexTypeParentID:
					connection.Execute( "ALTER TABLE " + tblName +
							" DROP INDEX "PARENTID_INDEX_NAME );
					break;
				default:
					FV_CRITICAL_MSG( "removeEntityTableIndex: Unknown index type "
							"%d\n", indexType );
					break;
			}
		}
		catch (std::exception& e)
		{
			FV_ERROR_MSG( "removeEntityTableIndex: %s\n", e.what() );
		}
	}

	void setColumnValue( MySql& con, const FvString& tableName,
			const FvString& columnName, const FvString& columnValue )
	{
		std::stringstream ss;
		ss << "UPDATE " << tableName << " SET " << columnName
				<< "='" << MySqlEscapedString( con, columnValue ) << '\'';
		con.Execute( ss.str() );
	}

	class ColumnsCollector : public IMySqlColumnMapping::IVisitor,
							public IMySqlIDColumnMapping::IVisitor
	{
		TableMetaData::NameToColInfoMap	columns_;

	public:
		virtual bool OnVisitColumn( IMySqlColumnMapping& column )
		{
			TableMetaData::ColumnInfo& colInfo =
					columns_[ column.GetColumnName() ];
			column.GetColumnType( colInfo.m_kColumnType );
			colInfo.m_kIndexType = column.GetColumnIndexType();

			return true;
		}
		virtual bool OnVisitIDColumn( IMySqlIDColumnMapping& column )
		{
			return this->OnVisitColumn( column );
		}

		TableMetaData::NameToColInfoMap& getColumnsInfo()	{ return columns_; }
	};

	class TableInspector: public IMySqlTableMapping::IVisitor
	{
	public:
		TableInspector( MySql& connection ) :
			connection_( connection ), isSynced_( true )
		{}

		MySql& connection()		{ return connection_; }

		bool deleteUnvisitedTables();

		bool isSynced() const	{ return isSynced_; }

		virtual bool OnVisitTable( IMySqlTableMapping& table );

	protected:
		virtual bool onNeedNewTable( const FvString& tableName,
				const TableMetaData::NameToColInfoMap& columns ) = 0;
		virtual bool onNeedUpdateTable( const FvString& tableName,
				const TableMetaData::NameToColInfoMap& obsoleteColumns,
				const TableMetaData::NameToColInfoMap& newColumns,
				const TableMetaData::NameToUpdatedColInfoMap& updatedColumns )
				= 0;
		virtual bool onNeedDeleteTables( const StrSet& tableNames ) = 0;

	protected:
		MySql& 	connection_;

	private:
		bool	isSynced_;
		StrSet 	visitedTables_;
	};

	bool TableInspector::OnVisitTable( IMySqlTableMapping& table )
	{
		ColumnsCollector colCol;
		table.VisitIDColumnWith( colCol );
		table.VisitColumnsWith( colCol );

		TableMetaData::NameToColInfoMap& newColumns( colCol.getColumnsInfo() );
		const FvString& tableName( table.GetTableName() );

		if (!visitedTables_.insert( tableName ).second)
		{
			throw std::runtime_error( "table " + tableName +
					" requested twice" );
		}

		TableMetaData::NameToColInfoMap oldColumns;
		TableMetaData::GetTableColumns( oldColumns, connection_, tableName );

		if (oldColumns.size() == 0)	
		{
			if (!this->onNeedNewTable( tableName, colCol.getColumnsInfo() ))
			{
				isSynced_ = false;
			}
		}
		else
		{
			TableMetaData::NameToUpdatedColInfoMap updatedColumns;
			classifyColumns( oldColumns, newColumns, updatedColumns );

			if ((oldColumns.size() + newColumns.size() + updatedColumns.size())
					> 0)
			{
				if (!this->onNeedUpdateTable( tableName, oldColumns, newColumns,
						updatedColumns ))
				{
					isSynced_ = false;
				}
			}
		}

		return true;
	}

	bool mod_lesser(const FvString& elem1, const FvString& elem2)
	{
		std::string str1 = elem1;
		std::string str2 = elem2;
		std::transform(
			str1.begin(),
			str1.end(),
			str1.begin(),
			tolower);

		std::transform(
			str2.begin(),
			str2.end(),
			str2.begin(),
			tolower);

		int ret = str1.compare(str2);
		if(ret < 0) 
			return true;
		else 
			return false;
	}

	bool TableInspector::deleteUnvisitedTables()
	{
		StrSet obsoleteTables;
		{
			StrSet existingTables;
			TableMetaData::GetEntityTables( existingTables, connection_ );

			std::set_difference( existingTables.begin(), existingTables.end(),
						visitedTables_.begin(), visitedTables_.end(),
						std::insert_iterator<StrSet>( obsoleteTables,
								obsoleteTables.begin() ),mod_lesser );
		}

		bool isDeleted = true;

		if (obsoleteTables.size() > 0)
		{
			isDeleted = this->onNeedDeleteTables( obsoleteTables );
			if (!isDeleted)
			{
				isSynced_ = false;
			}
		}

		return isDeleted;
	}

	class TableValidator : public TableInspector
	{
	public:
		TableValidator( MySql& connection ) : TableInspector( connection ) {}

	protected:
		virtual bool onNeedNewTable( const FvString& tableName,
				const TableMetaData::NameToColInfoMap& columns );
		virtual bool onNeedUpdateTable( const FvString& tableName,
				const TableMetaData::NameToColInfoMap& obsoleteColumns,
				const TableMetaData::NameToColInfoMap& newColumns,
				const TableMetaData::NameToUpdatedColInfoMap& updatedColumns );
		virtual bool onNeedDeleteTables( const StrSet& tableNames );
	};

	bool TableValidator::onNeedNewTable( const FvString& tableName,
			const TableMetaData::NameToColInfoMap& columns )
	{
		FV_INFO_MSG("\tRequire table %s\n", tableName.c_str());

		return false;
	}

	bool TableValidator::onNeedUpdateTable(
			const FvString& tableName,
			const TableMetaData::NameToColInfoMap& obsoleteColumns,
			const TableMetaData::NameToColInfoMap& newColumns,
			const TableMetaData::NameToUpdatedColInfoMap& updatedColumns )
	{
		for ( TableMetaData::NameToColInfoMap::const_iterator i =
				newColumns.begin(); i != newColumns.end(); ++i )
		{
			FV_INFO_MSG( "\tNeed to add column %s into table %s\n",
					i->first.c_str(), tableName.c_str() );
		}
		for ( TableMetaData::NameToColInfoMap::const_iterator i =
			obsoleteColumns.begin(); i != obsoleteColumns.end(); ++i )
		{
			FV_INFO_MSG( "\tNeed to delete column %s from table %s\n",
					i->first.c_str(), tableName.c_str() );
		}
		for ( TableMetaData::NameToUpdatedColInfoMap::const_iterator i =
				updatedColumns.begin(); i != updatedColumns.end(); ++i )
		{
			const char * indexedStr = (i->second.m_kIndexType ==
					IMySqlColumnMapping::IndexTypeNone) ?
					"non-indexed" : "indexed";

			FV_INFO_MSG( "\tNeed to update column %s in table %s to "
					"%s (%s)\n", i->first.c_str(), tableName.c_str(),
					i->second.m_kColumnType.GetAsString( connection_,
							i->second.m_kIndexType ).c_str(),
					indexedStr );
		}

		return false;
	}

	bool TableValidator::onNeedDeleteTables( const StrSet& tableNames )
	{
		for ( StrSet::const_iterator i = tableNames.begin();
				i != tableNames.end(); ++i )
		{
			FV_INFO_MSG( "Need to remove table %s\n", i->c_str() );
		}

		return false;
	}

	class TableInitialiser : public TableInspector
	{
	public:
		TableInitialiser( MySql& con ) : TableInspector( con ) {}

	protected:
		virtual bool onNeedNewTable( const FvString& tableName,
				const TableMetaData::NameToColInfoMap& columns );
		virtual bool onNeedUpdateTable( const FvString& tableName,
				const TableMetaData::NameToColInfoMap& obsoleteColumns,
				const TableMetaData::NameToColInfoMap& newColumns,
				const TableMetaData::NameToUpdatedColInfoMap& updatedColumns );
		virtual bool onNeedDeleteTables( const StrSet& tableNames );

	private:
		void addNewColumns( const FvString& tableName,
				const TableMetaData::NameToColInfoMap& columns,
				bool shouldPrintInfo );
	};

	bool TableInitialiser::onNeedNewTable( const FvString& tableName,
			const TableMetaData::NameToColInfoMap& columns )
	{
		FV_INFO_MSG("\tCreating table %s\n", tableName.c_str());
		connection_.Execute( "CREATE TABLE IF NOT EXISTS " + tableName +
				" (id BIGINT AUTO_INCREMENT, PRIMARY KEY idKey (id)) "
				"ENGINE="MYSQL_ENGINE_TYPE );
		bool deleteIDCol = false;
		TableMetaData::NameToColInfoMap newColumns( columns );
		TableMetaData::NameToColInfoMap::iterator idIter =
				newColumns.find( ID_COLUMN_NAME );
		if (idIter != newColumns.end())
		{
			newColumns.erase( idIter );
		}
		else
		{
			deleteIDCol = true;
		}

		this->addNewColumns( tableName, newColumns, false );

		if (deleteIDCol)
		{
			connection_.Execute( "ALTER TABLE " + tableName + " DROP COLUMN "
					ID_COLUMN_NAME );
		}

		return true;
	}

	bool TableInitialiser::onNeedUpdateTable( const FvString& tableName,
			const TableMetaData::NameToColInfoMap& obsoleteColumns,
			const TableMetaData::NameToColInfoMap& newColumns,
			const TableMetaData::NameToUpdatedColInfoMap& updatedColumns )
	{
		this->addNewColumns( tableName, newColumns, true );

		for ( TableMetaData::NameToColInfoMap::const_iterator
				i = obsoleteColumns.begin(); i != obsoleteColumns.end(); ++i )
		{
			FV_INFO_MSG( "\tDeleting column %s from table %s\n",
						i->first.c_str(), tableName.c_str() );
			removeEntityTableIndex( connection_, tableName, i->first,
					i->second.m_kIndexType );
			connection_.Execute( "ALTER TABLE " + tableName + " DROP COLUMN "
					+ i->first );
		}

		for ( TableMetaData::NameToUpdatedColInfoMap::const_iterator i =
				updatedColumns.begin(); i != updatedColumns.end(); ++i )
		{
			FvString columnTypeStr =
					i->second.m_kColumnType.GetAsString( connection_,
							i->second.m_kIndexType );

			FV_INFO_MSG( "\tUpdating type of column %s in table %s to %s "
					"(%sindexed)\n",
					i->first.c_str(), tableName.c_str(),
					columnTypeStr.c_str(),
					(i->second.m_kIndexType == IMySqlColumnMapping::IndexTypeNone) ?
						"non-": "" );

			removeEntityTableIndex( connection_, tableName, i->first,
					i->second.oldIndexType );

			connection_.Execute( "ALTER TABLE " + tableName + " MODIFY COLUMN "
					+ i->first + " " + columnTypeStr );

			CreateEntityTableIndex( connection_, tableName, i->first,
					i->second );
		}

		return true;
	}

	bool TableInitialiser::onNeedDeleteTables( const StrSet& tableNames )
	{
		for ( StrSet::const_iterator i = tableNames.begin();
				i != tableNames.end(); ++i )
		{
			FV_INFO_MSG( "\tDeleting table %s\n", i->c_str() );
			connection_.Execute( "DROP TABLE " + *i );
		}

		return true;
	}

	void TableInitialiser::addNewColumns( const FvString& tableName,
			const TableMetaData::NameToColInfoMap& columns,
			bool shouldPrintInfo )
	{
		for ( TableMetaData::NameToColInfoMap::const_iterator
				i = columns.begin(); i != columns.end(); ++i )
		{
			if (shouldPrintInfo)
			{
				FV_INFO_MSG( "\tAdding column %s into table %s\n",
							i->first.c_str(), tableName.c_str() );
			}

			connection_.Execute( "ALTER TABLE " + tableName + " ADD COLUMN "
					+ i->first + " " +
					i->second.m_kColumnType.GetAsString( connection_,
							i->second.m_kIndexType ) );
			CreateEntityTableIndex( connection_, tableName, i->first,
					i->second );

			if (!i->second.m_kColumnType.IsDefaultValueSupported())
			{
				setColumnValue( connection_, tableName, i->first,
						i->second.m_kColumnType.m_kDefaultValue );
			}
		}
	}

	class TypesCollector
	{
	public:
		TypesCollector( MySql& connection ) : metaData_( connection ), types_()
		{}

		void addType( FvEntityTypeID iFutureVisionID, const FvString& name );

		void deleteUnwantedTypes();

	private:
		FutureVisionMetaData	metaData_;
		StrSet 				types_;
	};

	void TypesCollector::addType( FvEntityTypeID iFutureVisionID,
			const FvString& name )
	{
		bool inserted = types_.insert( name ).second;
		if (!inserted)
			throw std::runtime_error( "type " + name + " requested twice" );

		FvEntityTypeID typeID = metaData_.GetEntityTypeID( name );
		if ( typeID == INVALID_TYPEID )
			metaData_.AddEntityType( name, iFutureVisionID );
		else if ( typeID != iFutureVisionID )
			metaData_.SetEntityTypeID( name, iFutureVisionID );
	}

	void TypesCollector::deleteUnwantedTypes()
	{
		FvString cleanupTypes = "DELETE FROM FutureVisionEntityTypes WHERE 1=1";
		for ( StrSet::const_iterator i = types_.begin(); i != types_.end(); ++i )
		{
			cleanupTypes += " AND name != '" + *i + "'";
		}
		metaData_.Connection().Execute( cleanupTypes );
	}
}


bool SimpleTableCollector::OnVisitTable( IMySqlTableMapping& table )
{
	ColumnsCollector colCol;
	table.VisitIDColumnWith( colCol );
	table.VisitColumnsWith( colCol );

	m_kTables[table.GetTableName()] = colCol.getColumnsInfo();

	return true;
}

SimpleTableCollector& SimpleTableCollector::operator+=(
		const SimpleTableCollector& rhs )
{
	for ( NewTableDataMap::const_iterator i = rhs.m_kTables.begin();
			i != rhs.m_kTables.end(); ++i )
	{
		m_kTables.insert( *i );
	}
	return *this;
}

namespace
{
	FvXMLSectionPtr getChildDefaultSection( const FvDataType& childType,
			const FvString childName, FvXMLSectionPtr pParentDefault )
	{
		FvXMLSectionPtr pChildDefault = (pParentDefault) ?
				pParentDefault->OpenSection( childName ) :
				FvXMLSectionPtr( NULL );
		if (!pChildDefault)
		{
			pChildDefault = childType.pDefaultSection();
		}

		return pChildDefault;
	}

	FvXMLSectionPtr getDefaultSection( const FvDataDescription& dataDesc )
	{
		FvXMLSectionPtr pDefaultSection = dataDesc.pDefaultSection();
		if (!pDefaultSection)
		{
			pDefaultSection = dataDesc.DataType()->pDefaultSection();
		}
		return pDefaultSection;
	}

	class Namer
	{
		typedef std::vector< FvString >			Strings;
		typedef std::vector< Strings::size_type >	Levels;

		FvString tableNamePrefix_;
		Strings		names_;
		Levels		tableLevels_;

	public:
		Namer( const FvString& entityName,
				const FvString& tableNamePrefix ) :
			tableNamePrefix_( tableNamePrefix ), names_( 1, entityName ),
			tableLevels_( 1, 1 )
		{}

		Namer( const Namer& existing, const FvString& propName, bool isTable ) :
			tableNamePrefix_( existing.tableNamePrefix_ ),
			names_( existing.names_ ), tableLevels_( existing.tableLevels_ )
		{
			if ( propName.empty() )
				names_.push_back( isTable ? DEFAULT_SEQUENCE_TABLE_NAME :
									DEFAULT_SEQUENCE_COLUMN_NAME  );
			else
				names_.push_back( propName );
			if (isTable)
				tableLevels_.push_back( names_.size() );
		}

		FvString buildColumnName( const FvString& prefix,
									const FvString& propName ) const
		{
			FvString suffix =
				(propName.empty()) ? DEFAULT_SEQUENCE_COLUMN_NAME : propName;
			return this->buildName( prefix, suffix, tableLevels_.back() );
		}

		FvString buildTableName( const FvString& propName ) const
		{
			FvString suffix =
				(propName.empty()) ? DEFAULT_SEQUENCE_TABLE_NAME : propName;
			return this->buildName( tableNamePrefix_, suffix, 0 );
		}

	private:
		FvString buildName( const FvString& prefix,
							const FvString& suffix,
							Strings::size_type startIdx ) const
		{
			FvString name = prefix;
			for ( Strings::size_type i = startIdx; i < names_.size(); ++i )
			{
				name += '_';
				name += names_[i];
			}
			if (!suffix.empty())
			{
				name += '_';
				name += suffix;
			}
			return name;
		}
	};

	class CompositePropertyMapping : public PropertyMapping
	{
	public:
		CompositePropertyMapping( const FvString& propName ) :
			PropertyMapping( propName )
		{}

		void addChild( PropertyMappingPtr child )
		{
			if (!child)
			{
				FV_ERROR_MSG( "CompositePropertyMapping::addChild: "
						"child is null (ignoring)\n" );
				return;
			}
			children_.push_back( child );
		}

		PropertyMappingPtr::ConstProxy getChild( int idx ) const
		{
			return children_[idx];
		}

		PropertyMappingPtr getChild( int idx )
		{
			return children_[idx];
		}

		int getNumChildren() const
		{
			return int(children_.size());
		}

		virtual void PrepareSQL( MySql& con )
		{
			for (Children::iterator ppChild = children_.begin();
					ppChild != children_.end(); ++ppChild)
			{
				(**ppChild).PrepareSQL( con );
			}
		}

		virtual void StreamToBound( FvBinaryIStream& strm )
		{
			for (Children::iterator ppChild = children_.begin();
					ppChild != children_.end(); ++ppChild)
			{
				(**ppChild).StreamToBound( strm );
			}
		}
		virtual void BoundToStream( FvBinaryOStream & strm ) const
		{
			for (Children::const_iterator ppChild = children_.begin();
					ppChild != children_.end(); ++ppChild)
			{
				(**ppChild).BoundToStream( strm );
			}
		}

		virtual void DefaultToStream( FvBinaryOStream & strm ) const
		{
			for (Children::const_iterator ppChild = children_.begin();
					ppChild != children_.end(); ++ppChild)
			{
				(**ppChild).DefaultToStream( strm );
			}
		}

		virtual void DefaultToBound()
		{
			for (Children::const_iterator ppChild = children_.begin();
					ppChild != children_.end(); ++ppChild)
			{
				(**ppChild).DefaultToBound();
			}
		}

		virtual bool HasTable() const
		{
			for (Children::const_iterator ppChild = children_.begin();
					ppChild != children_.end(); ++ppChild)
			{
				if ((*ppChild)->HasTable())
					return true;
			}
			return false;
		}

		virtual void UpdateTable( MySqlTransaction& transaction,
			FvDatabaseID parentID )
		{
			for (Children::iterator ppChild = children_.begin();
					ppChild != children_.end(); ++ppChild)
			{
				(**ppChild).UpdateTable( transaction, parentID );
			}
		}

		virtual void GetTableData( MySqlTransaction& transaction,
			FvDatabaseID parentID )
		{
			for (Children::iterator ppChild = children_.begin();
					ppChild != children_.end(); ++ppChild)
			{
				(**ppChild).GetTableData( transaction, parentID );
			}
		}

		virtual void DeleteChildren( MySqlTransaction& t, FvDatabaseID databaseID )
		{
			for (Children::iterator ppChild = children_.begin();
					ppChild != children_.end(); ++ppChild)
			{
				(**ppChild).DeleteChildren( t, databaseID );
			}
		}

		virtual bool VisitParentColumns( IMySqlColumnMapping::IVisitor& visitor )
		{
			for (Children::const_iterator ppChild = children_.begin();
					ppChild != children_.end(); ++ppChild)
			{
				if (!(**ppChild).VisitParentColumns( visitor ))
					return false;
			}
			return true;
		}

		virtual bool VisitTables( IMySqlTableMapping::IVisitor& visitor )
		{
			for (Children::const_iterator ppChild = children_.begin();
					ppChild != children_.end(); ++ppChild)
			{
				if (!(**ppChild).VisitTables( visitor ))
					return false;
			}
			return true;
		}

	protected:
		typedef std::vector<PropertyMappingPtr> Children;

		class SequenceBuffer : public PropertyMapping::ISequenceBuffer
		{
		protected:
			typedef std::vector< PropertyMapping::ISequenceBuffer* > SequenceBuffers;

			auto_container< SequenceBuffers > childBuffers_;

		public:
			SequenceBuffer( const CompositePropertyMapping::Children& children )
			{
				childBuffers_.container.reserve( children.size() );
				for ( CompositePropertyMapping::Children::const_iterator i =
					  children.begin(); i < children.end(); ++i )
				{
					childBuffers_.container.push_back(
						(*i)->CreateSequenceBuffer() );
				}
			}

			virtual void StreamToBuffer( int numElems, FvBinaryIStream& strm )
			{
				for ( int i = 0; i < numElems; ++i )
				{
					for ( SequenceBuffers::iterator j =
						  childBuffers_.container.begin();
						  j < childBuffers_.container.end(); ++j )
					{
						(*j)->StreamToBuffer( 1, strm );
					}
				}
			}

			virtual void BufferToStream( FvBinaryOStream& strm, int idx ) const
			{
				for ( SequenceBuffers::const_iterator j =
						childBuffers_.container.begin();
						j < childBuffers_.container.end(); ++j )
				{
					(*j)->BufferToStream( strm, idx );
				}
			}

			virtual void BufferToBound( PropertyMapping& binding,
				                        int idx ) const
			{
				CompositePropertyMapping& compositeProp =
					static_cast<CompositePropertyMapping&>(binding);

				int i = 0;
				for ( SequenceBuffers::const_iterator childBuf =
					  childBuffers_.container.begin();
					  childBuf < childBuffers_.container.end();
				      ++i, ++childBuf )
				{
					(*childBuf)->BufferToBound( *compositeProp.getChild(i),
						                        idx );
				}
			}

			virtual void BoundToBuffer( const PropertyMapping& binding )
			{
				const CompositePropertyMapping& compositeProp =
					static_cast<const CompositePropertyMapping&>(binding);

				int i = 0;
				for ( SequenceBuffers::const_iterator childBuf =
					  childBuffers_.container.begin();
					  childBuf < childBuffers_.container.end();
				      ++i, ++childBuf )
				{
					(*childBuf)->BoundToBuffer( *compositeProp.getChild(i) );
				}
			}

			virtual int	GetNumElems() const
			{
				return (childBuffers_.container.size()) ?
					childBuffers_.container[0]->GetNumElems() : 0;
			}

			virtual void Reset()
			{
				for ( SequenceBuffers::iterator i =
					  childBuffers_.container.begin();
					  i < childBuffers_.container.end(); ++i )
				{
					(*i)->Reset();
				}
			}
		};

	public:
		virtual ISequenceBuffer* CreateSequenceBuffer() const
		{	return new SequenceBuffer( children_ );	}

	protected:
		Children children_;
	};

	typedef FvSmartPointer<CompositePropertyMapping> CompositePropertyMappingPtr;
	class UserTypeMapping : public CompositePropertyMapping
	{
	public:
		UserTypeMapping( const FvString& propName )
			: CompositePropertyMapping( propName )
		{}

		virtual void StreamToBound( FvBinaryIStream& strm )
		{
			FvString encStr;
			strm >> encStr;
			FvMemoryIStream encStrm( const_cast<char*>(encStr.c_str()),
				encStr.length() );
			CompositePropertyMapping::StreamToBound( encStrm );
		}

		virtual void BoundToStream( FvBinaryOStream & strm ) const
		{
			FvMemoryOStream encStrm;
			CompositePropertyMapping::BoundToStream( encStrm );
			strm.AppendString( reinterpret_cast<const char *>(encStrm.Data()),
				encStrm.Size() );
		}

	private:
		class UserPropSequenceBuffer : public SequenceBuffer
		{
		public:
			UserPropSequenceBuffer( const CompositePropertyMapping::Children& children )
				: SequenceBuffer( children )
			{}

			virtual void StreamToBuffer( int numElems, FvBinaryIStream& strm )
			{
				for ( int i = 0; i < numElems; ++i )
				{
					FvString encStr;
					strm >> encStr;
					FvMemoryIStream encStrm(
						const_cast<char*>(encStr.c_str()), encStr.length() );
					SequenceBuffer::StreamToBuffer( 1, encStrm );
				}
			}

			virtual void BufferToStream( FvBinaryOStream& strm, int idx ) const
			{
				FvMemoryOStream encStrm;
				SequenceBuffer::BufferToStream( encStrm, idx );
				strm.AppendString( reinterpret_cast<const char *>(
									encStrm.Data()), encStrm.Size() );
			}
		};

	public:
		virtual ISequenceBuffer* CreateSequenceBuffer() const
		{	return new UserPropSequenceBuffer( children_ );	}
	};

	class ClassMapping : public CompositePropertyMapping,
			private IMySqlColumnMapping
	{
		bool		allowNone_;
		FvString	colName_;
		FvUInt8		hasProps_;

	public:
		ClassMapping( const Namer& namer, const FvString& propName,
					bool allowNone )
			: CompositePropertyMapping( propName ), allowNone_(allowNone),
			colName_( allowNone_ ? namer.buildColumnName( "fm", propName ) : "" ),
			hasProps_(1)
		{}

		virtual void StreamToBound( FvBinaryIStream& strm )
		{
			if (allowNone_)
				strm >> hasProps_;
			if (hasProps_)
				CompositePropertyMapping::StreamToBound( strm );
			else
				this->DefaultToBound();
		}

		virtual void BoundToStream( FvBinaryOStream & strm ) const
		{
			if (allowNone_)
				strm << hasProps_;
			if (hasProps_)
				CompositePropertyMapping::BoundToStream( strm );
		}

		virtual void DefaultToStream( FvBinaryOStream & strm ) const
		{
			if (allowNone_)
				strm << FvUInt8(0) ;
			else
				CompositePropertyMapping::DefaultToStream( strm );
		}

		virtual void DefaultToBound()
		{
			this->setHasProps( 0 );
			CompositePropertyMapping::DefaultToBound();
		}

		bool  isAllowNone() const			{ return allowNone_; }
		FvUInt8 getHasProps() const			{ return hasProps_; }
		void setHasProps( FvUInt8 hasProps )	{ hasProps_ = hasProps; }

		virtual bool VisitParentColumns( IMySqlColumnMapping::IVisitor& visitor )
		{
			if (allowNone_)
			{
				if (!visitor.OnVisitColumn( *this ))
					return false;
			}
			return CompositePropertyMapping::VisitParentColumns( visitor );
		}

		virtual const FvString& GetColumnName() const
		{
			return colName_;
		}
		virtual void GetColumnType( Type& type ) const
		{
			type.m_eFieldType = MYSQL_TYPE_TINY;
			type.SetIsUnsigned( true );
			type.m_kDefaultValue = "1";
		}
		virtual IndexType GetColumnIndexType() const
		{
			return IndexTypeNone;
		}
		virtual bool HasBinding() const
		{
			return true;
		}
		virtual void AddSelfToBindings( MySqlBindings& bindings )
		{
			bindings << hasProps_;
		}

	private:
		class ClassPropSequenceBuffer : public SequenceBuffer
		{
			std::vector<int> nonNullIdxs_;

		public:
			ClassPropSequenceBuffer( const CompositePropertyMapping::Children& children )
				: SequenceBuffer( children ), nonNullIdxs_( 1, 0 )
			{}

			virtual void StreamToBuffer( int numElems, FvBinaryIStream& strm )
			{
				FvUInt8 hasProps;
				for ( int i = 0; i < numElems; ++i )
				{
					strm >> hasProps;
					if (hasProps)
					{
						nonNullIdxs_.push_back( nonNullIdxs_.back() + 1 );
						SequenceBuffer::StreamToBuffer( 1, strm );
					}
					else
					{
						nonNullIdxs_.push_back( nonNullIdxs_.back() );
					}
				}
			}

			virtual void BufferToStream( FvBinaryOStream& strm, int idx ) const
			{
				int	realIdx = nonNullIdxs_[idx];
				FvUInt8 hasProps = (realIdx < nonNullIdxs_[idx + 1]) ? 1 : 0;
				strm << hasProps;
				if (hasProps)
					SequenceBuffer::BufferToStream( strm, realIdx );
			}

			virtual void BufferToBound( PropertyMapping& binding,
				                        int idx ) const
			{
				ClassMapping& classMapping =
					static_cast<ClassMapping&>(binding);
				int	realIdx = nonNullIdxs_[idx];
				if (realIdx < nonNullIdxs_[idx + 1])
				{
					classMapping.setHasProps( 1 );
					SequenceBuffer::BufferToBound( binding, realIdx );
				}
				else
				{
					classMapping.DefaultToBound();
				}
			}

			virtual void BoundToBuffer( const PropertyMapping& binding )
			{
				const ClassMapping& classMapping =
					static_cast<const ClassMapping&>(binding);
				if (classMapping.getHasProps())
				{
					SequenceBuffer::BoundToBuffer( binding );
					nonNullIdxs_.push_back( nonNullIdxs_.back() + 1);
				}
				else
				{
					nonNullIdxs_.push_back( nonNullIdxs_.back() );
				}
			}

			virtual int	GetNumElems() const
			{
				return int(nonNullIdxs_.size() - 1);
			}

			virtual void Reset()
			{
				nonNullIdxs_.resize(1);
				SequenceBuffer::Reset();
			}
		};

	public:
		virtual ISequenceBuffer* CreateSequenceBuffer() const
		{
			return (allowNone_) ? new ClassPropSequenceBuffer( children_ ) :
				CompositePropertyMapping::CreateSequenceBuffer();
		}

	};

	class SequenceMapping : public PropertyMapping, public IMySqlTableMapping,
							private IMySqlTableMapping::IRowBuffer
	{
	public:
		SequenceMapping( const Namer& namer, const FvString& propName,
			PropertyMappingPtr child, int size = 0 ) :
			PropertyMapping( propName ),
			tblName_( namer.buildTableName( propName ) ),
			child_(child), size_(size), pBuffer_( 0 ), childHasTable_(false)
		{
			//! add by Uman, 20100625, 转成小写,与mysql一致,防止数据库名对比时出错
			std::transform(
				tblName_.begin(),
				tblName_.end(),
				tblName_.begin(),
				tolower);
			//! add end
		}

		~SequenceMapping()
		{
			delete pBuffer_;
		}

		const PropertyMapping& getChildMapping() const	{	return *child_;	}

		bool isFixedSize() const	{ return size_ != 0; }
		int getFixedSize() const	{ return size_;	}

		virtual void PrepareSQL( MySql& con )
	   	{
			pBuffer_ = child_->CreateSequenceBuffer();
			if (!pBuffer_)
				FV_ERROR_MSG( "Persistence to MySQL is not supported for the "
						   "type of sequence used by '%s'.", PropName().c_str() );

			childHasTable_ = child_->HasTable();

			FvString stmt;
			MySqlBindings b;

			CommaSepColNamesBuilder colNamesBuilder( *child_ );
			FvString	childColNames = colNamesBuilder.GetResult();
			int			childNumColumns = colNamesBuilder.GetCount();
			FV_ASSERT( childHasTable_ || (childNumColumns > 0) );

			ColumnsBindingsBuilder	childColumnsBindings( *child_ );

			stmt = "SELECT ";
			if (childHasTable_)
				stmt += "id";
		   	if (childNumColumns)
			{
				if (childHasTable_)
					stmt += ",";
				stmt += childColNames;
			}
			stmt += " FROM " + tblName_ + " WHERE parentID=? ORDER BY id";
			pSelect_.reset( new MySqlStatement( con, stmt ) );
			b.clear();
			if (childHasTable_)
				b << childID_;
			b << childColumnsBindings.getBindings();
			pSelect_->BindResult( b );
			b.clear();
			b << queryID_;
			pSelect_->BindParams( b );

			stmt = "SELECT id FROM " + tblName_ + " WHERE parentID=? ORDER BY "
					"id FOR UPDATE";
			pSelectChildren_.reset( new MySqlStatement( con, stmt ) );
			b.clear();
			b << childID_;
			pSelectChildren_->BindResult( b );
			b.clear();
			b << queryID_;
			pSelectChildren_->BindParams( b );

			stmt = "INSERT INTO " + tblName_ + " (parentID";
			if (childNumColumns)
				stmt += "," + childColNames;
		   	stmt += ") VALUES (" +
				BuildCommaSeparatedQuestionMarks( 1 + childNumColumns ) + ")";
			pInsert_.reset( new MySqlStatement( con, stmt ) );

			stmt = "UPDATE " + tblName_ + " SET parentID=?";
			if (childNumColumns)
			{
				CommaSepColNamesBuilderWithSuffix
						updateColNamesBuilder( *child_, "=?" );
				FvString	updateCols = updateColNamesBuilder.GetResult();

				stmt += "," + updateCols;
			}
			stmt += " WHERE id=?";
			pUpdate_.reset( new MySqlStatement( con, stmt ) );

			b.clear();
			b << queryID_;
			b << childColumnsBindings.getBindings();
			pInsert_->BindParams( b );
			b << childID_;
			pUpdate_->BindParams( b );

			stmt = "DELETE FROM " + tblName_ + " WHERE parentID=?";
			pDelete_.reset( new MySqlStatement( con, stmt ) );
			stmt += " AND id >= ?";
			pDeleteExtra_.reset( new MySqlStatement( con, stmt ) );
			b.clear();
			b << queryID_;
			pDelete_->BindParams( b );
			b << childID_;
			pDeleteExtra_->BindParams( b );

			child_->PrepareSQL( con );
		}

		int getNumElemsFromStrm( FvBinaryIStream& strm ) const
		{
			if (this->isFixedSize())
				return this->getFixedSize();

			int numElems;
			strm >> numElems;
			return numElems;
		}

		virtual void StreamToBound( FvBinaryIStream& strm )
		{
			int numElems = this->getNumElemsFromStrm( strm );

			if (pBuffer_)
			{
				pBuffer_->Reset();
				pBuffer_->StreamToBuffer( numElems, strm );
			}
			else
			{
				for ( int i = 0; i < numElems; ++i )
					child_->StreamToBound( strm );
			}
		}

		int setNumElemsInStrm( FvBinaryOStream & strm, int numElems ) const
		{
			if (this->isFixedSize())
				return this->getFixedSize();

			strm << numElems;
			return numElems;
		}

		virtual void BoundToStream( FvBinaryOStream & strm ) const
		{
			if (pBuffer_)
			{
				int numAvailElems = pBuffer_->GetNumElems();
				int numElems = setNumElemsInStrm( strm, numAvailElems );
				int numFromBuf = std::min( numElems, numAvailElems );
				for ( int i = 0; i < numFromBuf; ++i )
				{
					pBuffer_->BufferToStream( strm, i );
				}
				for ( int i = numFromBuf; i < numElems; ++i )
				{
					child_->DefaultToStream( strm );
				}
			}
		}

		virtual void DefaultToStream( FvBinaryOStream & strm ) const
		{
			if (this->isFixedSize())
			{
				int numElems = this->getFixedSize();
				strm << numElems;
				for ( int i = 0; i < numElems; ++i )
				{
					child_->DefaultToStream( strm );
				}
			}
			else
			{
				strm << int(0);
			}
		}

		virtual void DefaultToBound()
		{
			if (pBuffer_)
				pBuffer_->Reset();
		}

		virtual bool HasTable() const	{	return true;	}

		virtual void UpdateTable( MySqlTransaction& transaction,
			FvDatabaseID parentID )
		{
			if (pBuffer_)
			{
				int numElems = pBuffer_->GetNumElems();
				if (numElems == 0)
				{
					this->DeleteChildren( transaction, parentID );
				}
				else
				{

					queryID_ = parentID;
					transaction.Execute( *pSelectChildren_ );
					int numRows = pSelectChildren_->ResultRows();
					int numUpdates = std::min( numRows, numElems );

					for ( int i = 0; i < numUpdates; ++i )
					{
						pSelectChildren_->Fetch();
						pBuffer_->BufferToBound( *child_, i );
						transaction.Execute( *pUpdate_ );


						if (childHasTable_)
						{
							child_->UpdateTable( transaction, childID_ );
						}
					}

					if (pSelectChildren_->Fetch())
					{
						transaction.Execute( *pDeleteExtra_ );
						if (childHasTable_)
						{
							do
							{
								child_->DeleteChildren( transaction, childID_ );
							} while ( pSelectChildren_->Fetch() );
						}
					}
					else if (numElems > numRows)
					{
						for (int i = numRows; i < numElems; ++i)
						{
							pBuffer_->BufferToBound( *child_, i );
							transaction.Execute( *pInsert_ );
							if (childHasTable_)
							{
								FvDatabaseID insertID = transaction.InsertID();
								child_->UpdateTable( transaction, insertID );
							}
						}
					}
				}
			}
		}

		virtual void GetTableData( MySqlTransaction& transaction,
			FvDatabaseID parentID )
		{
			if (pBuffer_)
			{
				pBuffer_->Reset();

				queryID_ = parentID;
				transaction.Execute( *pSelect_ );
				int numElems = pSelect_->ResultRows();

				for ( int i = 0; i < numElems; ++i )
				{
					pSelect_->Fetch();
					if (childHasTable_)
						child_->GetTableData( transaction, childID_ );
					pBuffer_->BoundToBuffer( *child_ );
				}
			}
		}

		virtual void DeleteChildren( MySqlTransaction& t, FvDatabaseID databaseID )
		{
			queryID_ = databaseID;
			if (childHasTable_)
			{
				t.Execute( *pSelectChildren_ );
				while ( pSelectChildren_->Fetch() )
				{
					child_->DeleteChildren( t, childID_ );
				}
			}
			t.Execute( *pDelete_ );
		}

		PropertyMapping::ISequenceBuffer* swapChildSeqBuffer(
			PropertyMapping::ISequenceBuffer* pBuffer )
		{
			FV_ASSERT( pBuffer_ );
			PropertyMapping::ISequenceBuffer* pCurBuf = pBuffer_;
			pBuffer_ = pBuffer;
			return pCurBuf;
		}

		virtual bool VisitParentColumns( IMySqlColumnMapping::IVisitor& visitor )
		{
			return true;
		}
		virtual bool VisitTables( IMySqlTableMapping::IVisitor& visitor )
		{
			return visitor.OnVisitTable( *this );
		}

		virtual const FvString& GetTableName() const
		{	return tblName_;	}
		virtual bool VisitColumnsWith( IMySqlColumnMapping::IVisitor& visitor )
		{
			MySqlColumnMappingAdapter< FvDatabaseID > parentIdColumn(
					PARENTID_COLUMN_NAME_STR, PARENTID_COLUMN_TYPE,
					IMySqlColumnMapping::IndexTypeParentID, queryID_ );
			if (!visitor.OnVisitColumn( parentIdColumn ))
				return false;

			return child_->VisitParentColumns( visitor );
		}
		virtual bool VisitIDColumnWith( IMySqlIDColumnMapping::IVisitor& visitor )
		{
			MySqlIDColumnMappingAdapter idColumn( childID_ );
			return visitor.OnVisitIDColumn( idColumn );
		}
		virtual bool VisitSubTablesWith( IMySqlTableMapping::IVisitor& visitor )
		{
			return child_->VisitTables( visitor );
		}
		virtual IMySqlTableMapping::IRowBuffer* GetRowBuffer()
		{
			return (pBuffer_) ? this : NULL;
		}

		virtual void AddBoundData()
		{
			pBuffer_->BoundToBuffer( *child_ );
		}
		virtual void SetBoundData( int row )
		{
			pBuffer_->BufferToBound( *child_, row );
		}
		virtual int GetNumRows() const	{ return pBuffer_->GetNumElems(); }
		virtual void clear()			{ pBuffer_->Reset(); }

	private:
		class SequenceBuffer : public PropertyMapping::ISequenceBuffer
		{
			typedef std::vector< PropertyMapping::ISequenceBuffer* > SequenceBuffers;

			const SequenceMapping& mapping_;
			auto_container< SequenceBuffers > childBuffers_;
			int	numUsed_;
			mutable int swappedIdx_;

		public:
			SequenceBuffer(const SequenceMapping& mapping)
				: mapping_(mapping), numUsed_(0), swappedIdx_(-1)
			{
				childBuffers_.container.push_back(
					mapping_.getChildMapping().CreateSequenceBuffer() );
			}

			virtual void StreamToBuffer( int numElems, FvBinaryIStream& strm )
			{
				int numRequired = numUsed_ + numElems;
				for ( int i = int(childBuffers_.container.size());
						i < numRequired; ++i )
				{
					childBuffers_.container.push_back(
						mapping_.getChildMapping().CreateSequenceBuffer() );
				}

				for ( int i = numUsed_; i < numRequired; ++i )
				{
					int numChildElems = mapping_.getNumElemsFromStrm( strm );
					childBuffers_.container[i]->
						StreamToBuffer( numChildElems, strm );
				}
				numUsed_ = numRequired;
				swappedIdx_ = -1;
			}

			virtual void BufferToStream( FvBinaryOStream& strm, int idx ) const
			{
				FV_ASSERT( swappedIdx_ < 0 );
				PropertyMapping::ISequenceBuffer* pChildSeqBuf =
					childBuffers_.container[idx];
				int numAvail = pChildSeqBuf->GetNumElems();
				int numElems = mapping_.setNumElemsInStrm( strm, numAvail );
				int numFromBuf = std::min( numElems, numAvail );
				for ( int i = 0; i < numFromBuf; ++i )
				{
					pChildSeqBuf->BufferToStream( strm, i );
				}
				for ( int i = numFromBuf; i < numElems; ++i )
				{
					mapping_.getChildMapping().DefaultToStream( strm );
				}
			}

			virtual void BufferToBound( PropertyMapping& binding,
										int idx ) const
			{
				FV_ASSERT( idx < numUsed_ );
				if (swappedIdx_ == idx)
					return;

				SequenceBuffer* pThis = const_cast<SequenceBuffer*>(this);
				SequenceMapping& seqMapping = static_cast<SequenceMapping&>(binding);
				PropertyMapping::ISequenceBuffer* pPrevBuffer =
					seqMapping.swapChildSeqBuffer( childBuffers_.container[idx] );

				if (swappedIdx_ >= 0)
				{
					pThis->childBuffers_.container[idx] =
							pThis->childBuffers_.container[swappedIdx_];
					pThis->childBuffers_.container[swappedIdx_] = pPrevBuffer;
			}
				else
				{
					pThis->childBuffers_.container[idx] = pPrevBuffer;
				}
				swappedIdx_ = idx;
			}

			virtual void BoundToBuffer( const PropertyMapping& binding )
			{
				FV_ASSERT( swappedIdx_ < 0 );
				if (numUsed_ == int(childBuffers_.container.size()))
				{
					childBuffers_.container.push_back(
						mapping_.getChildMapping().CreateSequenceBuffer() );
				}

				SequenceMapping& seqMapping = const_cast<SequenceMapping&>(
					static_cast<const SequenceMapping&>(binding) );
				childBuffers_.container[numUsed_] =
					seqMapping.swapChildSeqBuffer( childBuffers_.container[numUsed_] );
				++numUsed_;
			}

			virtual int	GetNumElems() const
			{
				return numUsed_;
			}

			virtual void Reset()
			{
				numUsed_ = 0;
				swappedIdx_ = -1;
				for ( SequenceBuffers::iterator i = childBuffers_.container.begin();
					  i < childBuffers_.container.end(); ++i )
				{
					(*i)->Reset();
				}
			}
		};

	public:
		virtual ISequenceBuffer* CreateSequenceBuffer()	const
		{
			return new SequenceBuffer(*this);
		}

	private:

		FvString tblName_;
		PropertyMappingPtr child_;
		int	size_;
		ISequenceBuffer* pBuffer_;
		FvDatabaseID queryID_;
		FvDatabaseID childID_;
		bool childHasTable_;

		std::auto_ptr<MySqlStatement> pSelect_;
		std::auto_ptr<MySqlStatement> pSelectChildren_;
		std::auto_ptr<MySqlStatement> pDelete_;
		std::auto_ptr<MySqlStatement> pDeleteExtra_;
		std::auto_ptr<MySqlStatement> pInsert_;
		std::auto_ptr<MySqlStatement> pUpdate_;
	};

	template < class STRM_TYPE, class MAPPING_TYPE >
	class PrimitiveSequenceBuffer : public PropertyMapping::ISequenceBuffer
	{
		typedef	std::vector< STRM_TYPE > Buffer;
		Buffer	buffer_;

	public:
		PrimitiveSequenceBuffer()
			: buffer_()
		{}

		virtual void StreamToBuffer( int numElems,
				                        FvBinaryIStream& strm )
		{
			typename Buffer::size_type numUsed = buffer_.size();
			buffer_.resize( buffer_.size() + numElems );

			for ( typename Buffer::iterator pVal = buffer_.begin() + numUsed;
				  pVal < buffer_.end(); ++pVal )
				strm >> *pVal;
		}

		virtual void BufferToStream( FvBinaryOStream& strm, int idx ) const
		{
			strm << buffer_[idx];
		}

		virtual void BufferToBound( PropertyMapping& binding,
				                    int idx ) const
		{
			static_cast<MAPPING_TYPE&>(binding).setValue( buffer_[idx] );
		}

		virtual void BoundToBuffer( const PropertyMapping& binding )
		{
			buffer_.push_back( static_cast<const MAPPING_TYPE&>(binding).getValue() );
		}

		virtual int	GetNumElems() const	{ return int(buffer_.size());	}

		virtual void Reset()	{	buffer_.clear();	}
	};

	template <class STRM_NUM_TYPE>
	class NumMapping : public PropertyMapping, public IMySqlColumnMapping
	{
	public:
		NumMapping( const FvString& propName,
				FvXMLSectionPtr pDefaultValue ) :
			PropertyMapping( propName ),
			colName_( propName ),
			defaultValue_(0)
		{
			if (pDefaultValue)
				defaultValue_ = pDefaultValue->As<STRM_NUM_TYPE>();
		}

		NumMapping( const Namer& namer, const FvString& propName,
				FvXMLSectionPtr pDefaultValue ) :
			PropertyMapping( propName ),
			colName_( namer.buildColumnName( "sm", propName ) ),
			defaultValue_(0)
		{
			if (pDefaultValue)
				defaultValue_ = pDefaultValue->As<STRM_NUM_TYPE>();
		}

		virtual void StreamToBound( FvBinaryIStream& strm )
		{
			STRM_NUM_TYPE i;
			strm >> i;
			value_.Set(i);
		}

		virtual void BoundToStream( FvBinaryOStream & strm ) const
		{
			const STRM_NUM_TYPE* pi = value_.Get();
			if (pi)
				strm << *pi;
			else
				strm << defaultValue_;
		}

		virtual void DefaultToStream( FvBinaryOStream & strm ) const
		{
			strm << defaultValue_;
		}

		virtual void DefaultToBound()
		{
			this->setValue( defaultValue_ );
		}

		virtual bool HasTable() const	{	return false;	}
		virtual void UpdateTable( MySqlTransaction& transaction,
			FvDatabaseID parentID ) {}
		virtual void GetTableData( MySqlTransaction& transaction,
			FvDatabaseID parentID ) {}

		virtual void PrepareSQL( MySql& ) {}
		virtual void DeleteChildren( MySqlTransaction&, FvDatabaseID ) {}

		virtual ISequenceBuffer* CreateSequenceBuffer() const
		{	return new PrimitiveSequenceBuffer< STRM_NUM_TYPE, NumMapping< STRM_NUM_TYPE > >();	}

		void setValue( STRM_NUM_TYPE val )	{	value_.Set( val );	}
		STRM_NUM_TYPE getValue() const
		{
			const STRM_NUM_TYPE* pNum = value_.Get();
			return (pNum) ? *pNum : defaultValue_;
		}

		virtual bool VisitParentColumns( IMySqlColumnMapping::IVisitor& visitor )
		{
			return visitor.OnVisitColumn( *this );
		}
		virtual bool VisitTables( IMySqlTableMapping::IVisitor& visitor )
		{
			return true;
		}

		virtual const FvString& GetColumnName() const
		{
			return colName_;
		}
		virtual void GetColumnType( Type& type ) const
		{
			type.m_eFieldType = MySqlTypeTraits<STRM_NUM_TYPE>::colType;
			type.SetIsUnsigned( !std::numeric_limits<STRM_NUM_TYPE>::is_signed );
			type.m_kDefaultValue = StringConv::ToStr( defaultValue_ );
		}
		virtual IndexType GetColumnIndexType() const
		{
			return IndexTypeNone;
		}
		virtual bool HasBinding() const
		{
			return true;
		}
		virtual void AddSelfToBindings( MySqlBindings& bindings )
		{
			bindings << value_;
		}

	private:
		FvString colName_;
		MySqlValueWithNull<STRM_NUM_TYPE> value_;
		STRM_NUM_TYPE defaultValue_;
	};

	template <class Vec, int DIM>
	class VectorMapping : public PropertyMapping
	{
	public:
		VectorMapping( const Namer& namer, const FvString& propName,
					FvXMLSectionPtr pDefaultValue ) :
			PropertyMapping( propName ),
			colNameTemplate_( namer.buildColumnName( "vm_%i", propName ) ),
			defaultValue_()
		{
			if (pDefaultValue)
				defaultValue_ = pDefaultValue->As<Vec>();
		}

		bool isNull() const
		{
			for ( int i=0; i<DIM; i++ )
				if (!value_[i].Get())
					return true;
			return false;
		}

		virtual void StreamToBound( FvBinaryIStream& strm )
		{
			Vec v;
			strm >> v;
			this->setValue(v);
		}

		virtual void BoundToStream( FvBinaryOStream & strm ) const
		{
			if (!this->isNull())
			{
				Vec v;
				for ( int i = 0; i < DIM; ++i )
					v[i] = *value_[i].Get();
				strm << v;
			}
			else
			{
				strm << defaultValue_;
			}
		}

		virtual void DefaultToStream( FvBinaryOStream & strm ) const
		{
			strm << defaultValue_;
		}

		virtual void DefaultToBound()
		{
			this->setValue( defaultValue_ );
		}

		virtual bool HasTable() const	{	return false;	}
		virtual void UpdateTable( MySqlTransaction& transaction,
			FvDatabaseID parentID ) {}
		virtual void GetTableData( MySqlTransaction& transaction,
			FvDatabaseID parentID ) {}

		virtual void PrepareSQL( MySql& ) {}
		virtual void DeleteChildren( MySqlTransaction&, FvDatabaseID ) {}

		virtual ISequenceBuffer* CreateSequenceBuffer() const
		{	return new PrimitiveSequenceBuffer< Vec, VectorMapping< Vec, DIM > >();	}

		void setValue( const Vec& v )
		{
			for ( int i = 0; i < DIM; ++i )
				value_[i].Set(v[i]);
		}
		Vec getValue() const
		{
			if (this->isNull())
				return defaultValue_;

			Vec	v;
			for ( int i = 0; i < DIM; ++i )
				v[i] = *value_[i].Get();
			return v;
		}

		virtual bool VisitParentColumns( IMySqlColumnMapping::IVisitor& visitor )
		{
			char buffer[512];
			for ( int i=0; i<DIM; i++ )
			{
				FvSNPrintf( buffer, sizeof(buffer),
					colNameTemplate_.c_str(), i );
				FvString colName( buffer );
				IMySqlColumnMapping::Type colType( MYSQL_TYPE_FLOAT , false, 0,
						StringConv::ToStr( defaultValue_[i] ) );
				MySqlColumnMappingAdapter< MySqlValueWithNull<float> >
						column( colName, colType,
								IMySqlColumnMapping::IndexTypeNone, value_[i] );
				if (!visitor.OnVisitColumn( column ))
					return false;
			}
			return true;
		}
		virtual bool VisitTables( IMySqlTableMapping::IVisitor& visitor )
		{
			return true;
		}

	private:
		FvString colNameTemplate_;
		MySqlValueWithNull<float> value_[DIM];
		Vec defaultValue_;
	};

	template <>
	class PrimitiveSequenceBuffer < bool, NumMapping<bool> > : public PropertyMapping::ISequenceBuffer
	{
		typedef	std::vector< bool > Buffer;
		Buffer	buffer_;

	public:
		PrimitiveSequenceBuffer()
			: buffer_()
		{}

		virtual void StreamToBuffer( int numElems,
			FvBinaryIStream& strm )
		{
			Buffer::size_type numUsed = buffer_.size();
			buffer_.resize( buffer_.size() + numElems );

			bool bVal;
			for ( Buffer::iterator pVal = buffer_.begin() + numUsed;
				pVal < buffer_.end(); ++pVal )
			{
				strm >> bVal;
				*pVal = bVal;
			}
		}

		virtual void BufferToStream( FvBinaryOStream& strm, int idx ) const
		{
			strm << buffer_[idx];
		}

		virtual void BufferToBound( PropertyMapping& binding,
			int idx ) const
		{
			static_cast<NumMapping<bool>&>(binding).setValue( buffer_[idx] );
		}

		virtual void BoundToBuffer( const PropertyMapping& binding )
		{
			buffer_.push_back( static_cast<const NumMapping<bool>&>(binding).getValue() );
		}

		virtual int	GetNumElems() const	{ return int(buffer_.size());	}

		virtual void Reset()	{	buffer_.clear();	}
	};
}

class StringLikeMapping : public PropertyMapping, public IMySqlColumnMapping
{
public:
	StringLikeMapping( const Namer& namer, const FvString& propName,
			bool isNameIndex, int length ) :
		PropertyMapping( propName ),
		colName_( namer.buildColumnName( "sm", propName ) ),
		value_(length), isNameIndex_( isNameIndex ), defaultValue_()
	{}

	void getString( FvString& str ) const	{	str = value_.GetString();	}
	void setString( const FvString& str )	{	value_.SetString(str);		}

	virtual void StreamToBound( FvBinaryIStream& strm )
	{
		strm >> value_;
	}

	virtual void BoundToStream( FvBinaryOStream & strm ) const
	{
		if (!value_.IsNull())
		{
			strm << value_;
		}
		else
		{
			strm << defaultValue_;
		}
	}

	virtual void DefaultToStream( FvBinaryOStream & strm ) const
	{
		strm << defaultValue_;
	}

	virtual void DefaultToBound()
	{
		this->setValue( defaultValue_ );
	}

	virtual bool HasTable() const	{	return false;	}
	virtual void UpdateTable( MySqlTransaction& transaction,
		FvDatabaseID parentID ) {}
	virtual void GetTableData( MySqlTransaction& transaction,
		FvDatabaseID parentID ) {}

	virtual void PrepareSQL( MySql& ) {}
	virtual void DeleteChildren( MySqlTransaction&, FvDatabaseID ) {}

	virtual ISequenceBuffer* CreateSequenceBuffer() const
	{	return new PrimitiveSequenceBuffer< FvString, StringLikeMapping >();	}

	void setValue( const FvString& str )	{	value_.SetString(str);	}
	FvString getValue() const
	{
		if (value_.IsNull())
			return defaultValue_;
		else
			return value_.GetString();
	}

	virtual bool VisitParentColumns( IMySqlColumnMapping::IVisitor& visitor )
	{
		return visitor.OnVisitColumn( *this );
	}
	virtual bool VisitTables( IMySqlTableMapping::IVisitor& visitor )
	{
		return true;
	}

	virtual const FvString& GetColumnName() const
	{
		return colName_;
	}
	virtual void GetColumnType( Type& type ) const
	{
		type.m_eFieldType =
				MySqlTypeTraits<FvString>::colType( value_.Capacity() );
		if (type.m_eFieldType == MYSQL_TYPE_LONG_BLOB)
		{
			FV_CRITICAL_MSG( "StringLikeMapping::StringLikeMapping: Property '%s' "
					"has DatabaseLength %lu that exceeds the maximum supported "
					"length of 16777215\n", this->PropName().c_str(),
					value_.Capacity() );
		}

		type.m_kDefaultValue = defaultValue_;
	}
	virtual IndexType GetColumnIndexType() const
	{
		return (isNameIndex_) ? IndexTypeName : IndexTypeNone;
	}
	virtual bool HasBinding() const
	{
		return true;
	}
	virtual void AddSelfToBindings( MySqlBindings& bindings )
	{
		bindings << value_;
	}

	FvString colName_;
	MySqlBuffer value_;
	bool		isNameIndex_;
	FvString	defaultValue_;
};

namespace
{

class TimestampMapping : public PropertyMapping, public IMySqlColumnMapping
{
public:
	TimestampMapping() : PropertyMapping( TIMESTAMP_COLUMN_NAME ) {}

	virtual void StreamToBound( FvBinaryIStream& strm ) {}
	virtual void BoundToStream( FvBinaryOStream & strm ) const {}
	virtual void DefaultToStream( FvBinaryOStream & strm ) const {}
	virtual void DefaultToBound() {}
	virtual void AddSelfToBindings( MySqlBindings& bindings ) {}

	virtual bool HasTable() const	{	return false;	}
	virtual void UpdateTable( MySqlTransaction& transaction,
		FvDatabaseID parentID ) {}
	virtual void GetTableData( MySqlTransaction& transaction,
		FvDatabaseID parentID ) {}
	virtual void DeleteChildren( MySqlTransaction&, FvDatabaseID ) {}

	virtual void PrepareSQL( MySql& ) {}
	virtual ISequenceBuffer* CreateSequenceBuffer() const
	{ 	return NULL; }

	virtual bool VisitParentColumns( IMySqlColumnMapping::IVisitor& visitor )
	{	return visitor.OnVisitColumn( *this );	}
	virtual bool VisitTables( IMySqlTableMapping::IVisitor& visitor )
	{	return true;	}

	virtual const FvString& GetColumnName() const
	{	return TIMESTAMP_COLUMN_NAME_STR; }
	virtual void GetColumnType( Type& type ) const
	{	type.m_eFieldType = MYSQL_TYPE_TIMESTAMP;
		type.m_kDefaultValue = "CURRENT_TIMESTAMP";
		type.m_kOnUpdateCmd = "CURRENT_TIMESTAMP";
	}

	virtual IndexType GetColumnIndexType() const { return IndexTypeNone; }

	virtual bool HasBinding() const
	{	return false;	}
};

class StringMapping : public StringLikeMapping
{
public:
	StringMapping( const Namer& namer, const FvString& propName,
			bool isNameIndex, int length, FvXMLSectionPtr pDefaultValue ) :
		StringLikeMapping( namer, propName, isNameIndex, length )
	{
		if (pDefaultValue)
		{
			defaultValue_ = pDefaultValue->As<FvString>();

			if (defaultValue_.length() > value_.Capacity())
			{
				defaultValue_.resize( value_.Capacity() );
				FV_WARNING_MSG( "StringMapping::StringMapping: Default value for "
						"property %s has been truncated to '%s'\n",
						propName.c_str(), defaultValue_.c_str() );
			}
		}
	}

	virtual void GetColumnType( Type& type ) const
	{
		StringLikeMapping::GetColumnType( type );

		if (value_.Capacity() < 256)
		{
			type.m_eFieldType = MYSQL_TYPE_VAR_STRING;
			type.m_uiLength = value_.Capacity();
		}

		type.SetIsBinary( false );
	}
};

class BlobMapping : public StringLikeMapping
{
public:
	BlobMapping( const Namer& namer, const FvString& propName,
			bool isNameIndex, int length, FvXMLSectionPtr pDefaultValue ) :
		StringLikeMapping( namer, propName, isNameIndex, length )
	{
		if (pDefaultValue)
		{
			BlobMapping::decodeSection( defaultValue_, pDefaultValue );

			if (defaultValue_.length() > value_.Capacity())
			{
				defaultValue_.resize( value_.Capacity() );
				FV_WARNING_MSG( "BlobMapping::BlobMapping: Default value for "
						"property %s has been truncated\n",
						propName.c_str() );
			}
		}
	}

	virtual void GetColumnType( Type& type ) const
	{
		StringLikeMapping::GetColumnType( type );

		type.SetIsBinary( true );
	}

	static void decodeSection( FvString& output, FvXMLSectionPtr pSection )
	{
		output = pSection->As<FvString>();
		int len = output.length();
		if (len <= 256)
		{
			char decoded[256];
			int length = FvBase64::Decode( output, decoded, 256 );
			output.assign(decoded, length);
		}
		else
		{
			char * decoded = new char[ len ];
			int length = FvBase64::Decode( output, decoded, len );
			output.assign(decoded, length);
			delete [] decoded;
		}
	}
};

//class PythonMapping : public StringLikeMapping
//{
//public:
//	PythonMapping( const Namer& namer, const FvString& propName,
//			bool isNameIndex, int length, FvXMLSectionPtr pDefaultValue ) :
//		StringLikeMapping( namer, propName, isNameIndex, length )
//	{
//		if (pDefaultValue)
//			defaultValue_ = pDefaultValue->as<FvString>();
//
//		if (defaultValue_.length() == 0)
//		{
//			defaultValue_ = PythonMapping::getPickler().pickle( Py_None );
//		}
//		else if (PythonDataType_IsExpression( defaultValue_ ))
//		{
//			PythonMapping::pickleExpression( defaultValue_, defaultValue_ );
//		}
//		else
//		{
//			BlobMapping::decodeSection( defaultValue_, pDefaultValue );
//		}
//
//		if (defaultValue_.length() > value_.Capacity())
//		{
//			FV_WARNING_MSG( "PythonMapping::PythonMapping: Default value for "
//					"property %s is too big to fit inside column. Defaulting"
//					"to None\n", propName.c_str() );
//			defaultValue_ = PythonMapping::getPickler().pickle( Py_None );
//			if (defaultValue_.length() > value_.Capacity())
//			{
//				FV_CRITICAL_MSG( "PythonMapping::PythonMapping: Even None cannot"
//						"fit inside column. Please increase DatabaseSize of"
//						"property %s\n", propName.c_str() );
//			}
//		}
//
//	}
//
//	virtual void GetColumnType( Type& type ) const
//	{
//		StringLikeMapping::GetColumnType( type );
//
//		type.SetIsBinary( true );
//	}
//
//	virtual void BoundToStream( FvBinaryOStream & strm ) const
//	{
//		if (!value_.IsNull() && value_.size() > 0)
//		{
//			strm << value_.GetString();
//		}
//		else
//		{
//			strm << defaultValue_;
//		}
//	}
//
//private:
//	static void pickleExpression( FvString& output, const FvString& expr )
//	{
//		PyObjectPtr pResult( Script::runString( expr.c_str(), false ),
//				PyObjectPtr::STEAL_REFERENCE );
//
//		PyObject* 	pToBePickled = pResult ? pResult.getObject() : Py_None;
//
//		output = PythonMapping::getPickler().pickle( pToBePickled );
//	}
//
//	static Pickler & getPickler()
//	{
//		static Pickler pickler;
//
//		return pickler;
//	}
//};

class UniqueIDMapping : public PropertyMapping, public IMySqlColumnMapping
{
public:
	UniqueIDMapping( const Namer& namer, const FvString& propName,
			FvXMLSectionPtr pDefaultValue ) :
		PropertyMapping( propName ),
		colName_( namer.buildColumnName( "sm", propName ) ),
		value_( sizeof(FvUInt32) * 4 )
	{
		if (pDefaultValue)
			defaultValue_ = FvUniqueID( pDefaultValue->AsString() );
	}

	virtual const FvString& GetColumnName() const
	{
		return colName_;
	}
	virtual void GetColumnType( Type& type ) const
	{
		type.m_eFieldType = MYSQL_TYPE_STRING;
		type.SetIsBinary( true );
		type.m_uiLength = value_.Capacity();

		MySqlBuffer defaultValue( value_.Capacity() );
		UniqueIDMapping::setBuffer( defaultValue, defaultValue_ );
		type.m_kDefaultValue = defaultValue.GetString();
	}
	virtual IndexType GetColumnIndexType() const
	{
		return IndexTypeNone;
	}
	virtual bool HasBinding() const
	{
		return true;
	}
	virtual void AddSelfToBindings( MySqlBindings& bindings )
	{
		bindings << value_;
	}

	static void setBuffer( MySqlBuffer& buf, const FvUniqueID& uniqueID )
	{
		FvUInt32 id[4];

		id[0] = uniqueID.GetA();
		id[1] = uniqueID.GetB();
		id[2] = uniqueID.GetC();
		id[3] = uniqueID.GetD();

		buf.Set( id, sizeof(id) );
	}

	void setValue( const FvUniqueID& uniqueID )
	{
		UniqueIDMapping::setBuffer( value_, uniqueID );
	}

	FvUniqueID getValue() const
	{
		const FvUInt32* id = reinterpret_cast<const FvUInt32*>( value_.Get() );
		if (id)
			return FvUniqueID( id[0], id[1], id[2], id[3] );
		else
			return  defaultValue_;
	}

	virtual void StreamToBound( FvBinaryIStream & strm )
	{
		FvUniqueID uniqueId;
		strm >> uniqueId;

		this->setValue( uniqueId );
	}

	virtual void BoundToStream( FvBinaryOStream & strm ) const
	{
		strm << this->getValue();
	}

	virtual void DefaultToStream( FvBinaryOStream & strm ) const
	{
		strm << defaultValue_;
	}

	virtual void DefaultToBound()
	{
		this->setValue( defaultValue_ );
	}

	virtual void PrepareSQL( MySql& ) {}
	virtual bool HasTable() const	{ return false; }
	virtual void UpdateTable( MySqlTransaction& transaction,
			FvDatabaseID parentID ) {}
	virtual void GetTableData( MySqlTransaction& transaction,
			FvDatabaseID parentID ) {}
	virtual void DeleteChildren( MySqlTransaction&, FvDatabaseID parentID ) {}

	virtual ISequenceBuffer* CreateSequenceBuffer() const
	{
		return new PrimitiveSequenceBuffer< FvUniqueID, UniqueIDMapping >();
	}

	virtual bool VisitParentColumns( IMySqlColumnMapping::IVisitor& visitor )
	{
		return visitor.OnVisitColumn( *this );
	}
	virtual bool VisitTables( IMySqlTableMapping::IVisitor& visitor )
	{
		return true;
	}

private:
	FvString	colName_;
	FvUniqueID 	defaultValue_;
	MySqlBuffer value_;
};

class UDORefMapping : public UniqueIDMapping
{
public:
	UDORefMapping( const Namer& namer, const FvString& propName,
			FvXMLSectionPtr pDefaultValue ) :
		UniqueIDMapping( namer, propName, getGuidSection( pDefaultValue ) )
	{}

private:
	static FvXMLSectionPtr getGuidSection( FvXMLSectionPtr pParentSection )
	{
		return (pParentSection) ? pParentSection->OpenSection( "guid" ) : NULL;
	}
};

}

namespace
{
//	class PyUserTypeBinder : public PyObjectPlus
//	{
//		Py_Header( PyUserTypeBinder, PyObjectPlus )
//
//	public:
//		PyUserTypeBinder( const Namer& namer, const FvString& propName,
//			FvXMLSectionPtr pDefaultValue, PyTypePlus * pType = &s_type_ ) :
//			PyObjectPlus( pType )
//		{
//			if (propName.empty())
//				m_kTables.push( Context( new UserTypeMapping( propName ),
//									namer, pDefaultValue ) );
//			else
//				m_kTables.push( Context( new UserTypeMapping( propName ),
//									namer, propName, false,
//									pDefaultValue ) );
//		}
//
//		void beginTable( const FvString& name );
//		PY_AUTO_METHOD_DECLARE( RETVOID, beginTable, ARG( FvString, END ) );
//		bool endTable();
//		PY_AUTO_METHOD_DECLARE( RETOK, endTable, END );
//		bool bind( const FvString& propName, const FvString& typeName,
//				int databaseLength );
//		PY_AUTO_METHOD_DECLARE( RETOK, bind,
//				ARG( FvString, ARG( FvString, OPTARG( int, 255, END ) ) ) );
//
//		PropertyMappingPtr GetResult();
//
//		PyObject * pyGetAttribute( const char * attr )
//		{
//			PY_GETATTR_STD();
//			return PyObjectPlus::pyGetAttribute( attr );
//		}
//
//	private:
//		struct Context
//		{
//			CompositePropertyMappingPtr	pCompositeProp;
//			Namer						namer;
//			FvXMLSectionPtr				pDefaultValue;
//
//			Context( CompositePropertyMappingPtr pProp,
//					const Namer& inNamer,
//					const FvString& propName, bool isTable,
//					FvXMLSectionPtr pDefault ) :
//				pCompositeProp(pProp), namer( inNamer, propName, isTable ),
//				pDefaultValue(pDefault)
//			{}
//
//			Context( CompositePropertyMappingPtr pProp,
//					const Namer& inNamer,
//					FvXMLSectionPtr pDefault ) :
//				pCompositeProp(pProp), namer( inNamer ),
//				pDefaultValue(pDefault)
//			{}
//
//		};
//		std::stack<Context> m_kTables;
//
//		const Context& curContext()
//		{
//			FV_ASSERT( m_kTables.size() );
//			return m_kTables.top();
//		}
//	};
//
	PropertyMappingPtr createPropertyMapping( const Namer& namer,
			const FvString& propName, const FvDataType& type,
			int databaseLength, FvXMLSectionPtr pDefaultValue,
			bool isNameIndex = false );
//
//	bool PyUserTypeBinder::bind( const FvString& propName,
//			const FvString& typeName, int databaseLength )
//	{
//		const Context& context = curContext();
//		FvXMLSectionPtr pPropDefault;
//		if (context.pDefaultValue)
//			pPropDefault = context.pDefaultValue->openSection( propName );
//
//		std::stringstream typeStrm;
//		typeStrm << "<Type>" << typeName << "</Type>";
//		XMLSectionPtr pXMLTypeSection =
//			XMLSection::createFromStream( FvString(),  typeStrm );
//		FvXMLSectionPtr pTypeSection( pXMLTypeSection.getObject() );
//		DataTypePtr pType = FvDataType::buildDataType( pTypeSection );
//		if (pType.exists())
//		{
//			context.pCompositeProp->addChild(
//				createPropertyMapping( context.namer, propName, *pType,
//										databaseLength, pPropDefault ) );
//		}
//		else
//		{
//			FV_ERROR_MSG( "PyUserTypeBinder::bind: Invalid type name %s.\n",
//						typeName.c_str() );
//			PyErr_SetString( PyExc_TypeError, typeName.c_str() );
//		}
//		return pType.exists();
//	}
//
//	void PyUserTypeBinder::beginTable( const FvString& propName )
//	{
//		const Context& context = curContext();
//		FvXMLSectionPtr pPropDefault;
//		if (context.pDefaultValue)
//			pPropDefault = context.pDefaultValue->openSection( propName );
//		CompositePropertyMappingPtr pChild( new CompositePropertyMapping( propName ) );
//		PropertyMappingPtr pSequence( new SequenceMapping( context.namer,
//			propName, pChild ) );
//        context.pCompositeProp->addChild( pSequence );
//		m_kTables.push( Context( pChild, context.namer, propName, true,
//							   pPropDefault ) );
//	}
//
//	bool PyUserTypeBinder::endTable()
//	{
//		bool isOK = ( m_kTables.size() > 1 );
//		if ( isOK )
//			m_kTables.pop();
//		else
//			PyErr_SetString( PyExc_RuntimeError, "No matching beginTable." );
//		return isOK;
//	}
//
//	PropertyMappingPtr PyUserTypeBinder::GetResult()
//	{
//		if ( m_kTables.size() == 1 )
//			return PropertyMappingPtr( curContext().pCompositeProp.GetObject() );
//		else
//			return PropertyMappingPtr();
//	}
//
//	typedef FvSmartPointer<PyUserTypeBinder> PyUserTypeBinderPtr;

}

//PY_TYPEOBJECT( PyUserTypeBinder )
//
//PY_BEGIN_METHODS( PyUserTypeBinder )
//	PY_METHOD( beginTable )
//	PY_METHOD( endTable )
//	PY_METHOD( bind )
//PY_END_METHODS()
//
//PY_BEGIN_ATTRIBUTES( PyUserTypeBinder )
//PY_END_ATTRIBUTES()

namespace
{
	//PropertyMappingPtr createUserTypeMapping( const Namer& namer,
	//		const FvString& propName, const UserDataType& type,
	//		FvXMLSectionPtr pDefaultValue )
	//{
	//	const FvString& moduleName = type.moduleName();
	//	const FvString& instanceName = type.instanceName();

	//	PyObjectPtr pModule(
	//			PyImport_ImportModule(
	//				const_cast< char * >( moduleName.c_str() ) ),
	//			PyObjectPtr::STEAL_REFERENCE );
	//	if (!pModule)
	//	{
	//		FV_ERROR_MSG( "createUserTypeMapping: unable to import %s from %s\n",
	//				instanceName.c_str(), moduleName.c_str() );
	//		PyErr_Print();
	//		return PropertyMappingPtr();
	//	}

	//	PyObjectPtr pObject(
	//			PyObject_GetAttrString( pModule.getObject(),
	//				const_cast<char*>( instanceName.c_str() ) ) );
	//	if (!pObject)
	//	{
	//		FV_ERROR_MSG( "createUserTypeMapping: unable to import %s from %s\n",
	//				instanceName.c_str(), moduleName.c_str() );
	//		PyErr_Print();
	//		return PropertyMappingPtr();
	//	}
	//	else
	//	{
	//		Py_DECREF( pObject.getObject() );
	//	}

	//	PyUserTypeBinderPtr pBinder(
	//		new PyUserTypeBinder( namer, propName, pDefaultValue )
	//							  , true );

	//	PyObjectPtr pResult(
	//			PyObject_CallMethod( pObject.getObject(), "bindSectionToDB",
	//				"O", pBinder.getObject() ) );

	//	if (!pResult)
	//	{
	//		FV_ERROR_MSG( "createUserTypeMapping: (%s.%s) bindSectionToDB failed\n",
	//				moduleName.c_str(), instanceName.c_str() );
	//		PyErr_Print();
	//		return PropertyMappingPtr();
	//	}

	//	PropertyMappingPtr pTypeMapping = pBinder->GetResult();
	//	if (!pTypeMapping.exists())
	//	{
	//		FV_ERROR_MSG( "createUserTypeMapping: (%s.%s) bindSectionToDB missing "
	//					"endTable\n", moduleName.c_str(), instanceName.c_str() );
	//	}

	//	return pTypeMapping;
	//}

	template <class DATATYPE>
	ClassMapping* createClassTypeMapping( const Namer& classNamer,
			const FvString& propName, const DATATYPE& type,
			int databaseLength, FvXMLSectionPtr pDefaultValue )
	{
		ClassMapping* pClassMapping =
			new ClassMapping( classNamer, propName, type.AllowNone() );

		Namer childNamer( classNamer, propName, false );
		const Namer& namer = (propName.empty()) ? classNamer : childNamer;

		const DATATYPE::Fields& fields = type.GetFields();

		for ( DATATYPE::Fields::const_iterator i = fields.begin();
				i < fields.end(); ++i )
		{
			if (i->m_bIsPersistent)
			{
				FvXMLSectionPtr pPropDefault = getChildDefaultSection(
						*(i->m_spType), i->m_kName, pDefaultValue );
				PropertyMappingPtr pMemMapping =
					createPropertyMapping( namer, i->m_kName, *(i->m_spType),
											i->m_iDBLen, pPropDefault );
				if (pMemMapping.Exists())
					pClassMapping->addChild( pMemMapping );
			}
		}

		return pClassMapping;
	}

	template <class KEYDATATYPE, class VALDATATYPE>
	ClassMapping* createMapTypeChildMapping( const Namer& classNamer,
		const KEYDATATYPE& keyType, const VALDATATYPE& valType,
		int databaseLength, FvXMLSectionPtr pDefaultValue )
	{
		ClassMapping* pClassMapping =
			new ClassMapping( classNamer, "value", false );

		//Namer childNamer( classNamer, DEFAULT_SEQUENCE_TABLE_NAME, false );
		const Namer& namer = classNamer;

		FvXMLSectionPtr spKeyDefault = getChildDefaultSection( keyType, "key", pDefaultValue );
		PropertyMappingPtr spKeyMapping = createPropertyMapping( namer, "key", keyType, databaseLength, spKeyDefault );
		if(spKeyMapping.Exists())
			pClassMapping->addChild( spKeyMapping );

		FvXMLSectionPtr spValDefault = getChildDefaultSection( valType, "val", pDefaultValue );
		PropertyMappingPtr spValMapping = createPropertyMapping( namer, "val", valType, databaseLength, spValDefault );
		if(spValMapping.Exists())
			pClassMapping->addChild( spValMapping );

		return pClassMapping;
	}

	PropertyMappingPtr createPropertyMapping( const Namer& namer,
			const FvString& propName, const FvDataType& type,
			int databaseLength, FvXMLSectionPtr pDefaultValue,
			bool isNameIndex )
	{
		PropertyMappingPtr pResult;

		if (type.TypeID() == FV_DATATYPEID_ARRAY)
		{
			const FvArrayDataType *pSeqType = static_cast<const FvArrayDataType*>(&type);
			const FvDataType &childType = *pSeqType->GetElemType();
			FvXMLSectionPtr pChildDefault = childType.pDefaultSection();

			PropertyMappingPtr childMapping =
				createPropertyMapping( Namer( namer, propName, true ),
										FvString(), childType,
										databaseLength, pChildDefault );

			if (childMapping.Exists())
				pResult = new SequenceMapping( namer, propName, childMapping );//! modify by Uman,20100209,采用默认值0,表示可变的数组
				//pResult = new SequenceMapping( namer, propName, childMapping, pSeqType->GetSize() );
		}
		else if (type.TypeID() == FV_DATATYPEID_STRUCT)
		{
			const FvStructDataType *pStructType = static_cast<const FvStructDataType*>(&type);
			pResult = createClassTypeMapping( namer, propName,
							*pStructType, databaseLength, pDefaultValue );
		}
		else if(type.TypeID() == FV_DATATYPEID_NUM_FIXARRAY)
		{
			const FvNumFixArrayDataType *pFixArrayType = static_cast<const FvNumFixArrayDataType*>(&type);
			pResult = createClassTypeMapping( namer, propName,
							*pFixArrayType, databaseLength, pDefaultValue );
		}
/**
		else if(type.TypeID() == FV_DATATYPEID_INTMAP)
		{
			const FvIntMapDataType* pkMapType = static_cast<const FvIntMapDataType*>(&type);
			const FvDataType& kKeyType = *pkMapType->GetKeyType();
			const FvDataType& kValType = *pkMapType->GetElemType();

			PropertyMappingPtr childMapping =
				createMapTypeChildMapping( Namer( namer, propName, true ),
				kKeyType, kValType,
				databaseLength, pDefaultValue );

			if (childMapping.Exists())
				pResult = new SequenceMapping( namer, propName, childMapping );
		}
		else if(type.TypeID() == FV_DATATYPEID_STRMAP)
		{
			const FvStringMapDataType* pkMapType = static_cast<const FvStringMapDataType*>(&type);
			const FvDataType& kKeyType = *pkMapType->GetKeyType();
			const FvDataType& kValType = *pkMapType->GetElemType();

			PropertyMappingPtr childMapping =
				createMapTypeChildMapping( Namer( namer, propName, true ),
				kKeyType, kValType,
				databaseLength, pDefaultValue );

			if (childMapping.Exists())
				pResult = new SequenceMapping( namer, propName, childMapping );
		}
**/
		else
		{
			const FvMetaDataType * pMetaType = type.pMetaDataType();
			FV_ASSERT(pMetaType);
			const char *metaName = pMetaType->Name();
			if (strcmp( metaName, "UINT8" ) == 0)
				pResult = new NumMapping< FvUInt8 >( namer, propName, pDefaultValue );
			else if (strcmp( metaName, "UINT16" ) == 0)
				pResult = new NumMapping< FvUInt16 >( namer, propName, pDefaultValue );
			else if (strcmp( metaName, "UINT32" ) == 0)
				pResult = new NumMapping< FvUInt32 >( namer, propName, pDefaultValue );
			else if (strcmp( metaName, "UINT64" ) == 0)
				pResult = new NumMapping< FvUInt64 >( namer, propName, pDefaultValue );
			else if (strcmp( metaName, "INT8" ) == 0)
				pResult = new NumMapping< FvInt8 >( namer, propName, pDefaultValue );
			else if (strcmp( metaName, "INT16" ) == 0)
				pResult = new NumMapping< FvInt16 >( namer, propName, pDefaultValue );
			else if (strcmp( metaName, "INT32" ) == 0)
				pResult = new NumMapping< FvInt32 >( namer, propName, pDefaultValue );
			else if (strcmp( metaName, "INT64" ) == 0)
				pResult = new NumMapping< FvInt64 >( namer, propName, pDefaultValue );
			else if (strcmp( metaName, "FLOAT32" ) == 0)
				pResult = new NumMapping< float >( namer, propName, pDefaultValue );
			else if (strcmp( metaName, "FLOAT64" ) == 0)
				pResult = new NumMapping< double >( namer, propName, pDefaultValue );
			else if (strcmp( metaName, "BOOL" ) == 0)
				pResult = new NumMapping< bool >( namer, propName, pDefaultValue );
			else if (strcmp( metaName, "VECTOR2" ) == 0)
				pResult = new VectorMapping<FvVector2,2>( namer, propName, pDefaultValue );
			else if (strcmp( metaName, "VECTOR3" ) == 0)
				pResult = new VectorMapping<FvVector3,3>( namer, propName, pDefaultValue );
			else if (strcmp( metaName, "VECTOR4" ) == 0)
				pResult = new VectorMapping<FvVector4,4>( namer, propName, pDefaultValue );
			else if (strcmp( metaName, "STRING" ) == 0)
				pResult = new StringMapping( namer, propName, isNameIndex, databaseLength, pDefaultValue );
			//else if (strcmp( metaName, "PYTHON" ) == 0)
			//	pResult = new PythonMapping( namer, propName, isNameIndex, databaseLength, pDefaultValue );
			else if (strcmp( metaName, "BLOB" ) == 0)
				pResult = new BlobMapping( namer, propName, isNameIndex, databaseLength, pDefaultValue );
			//else if (strcmp( metaName, "PATROL_PATH" ) == 0)
			//	pResult = new UniqueIDMapping( namer, propName, pDefaultValue );
			else if (strcmp( metaName, "UDO" ) == 0)
				pResult = new UDORefMapping( namer, propName, pDefaultValue );
		}

		if (!pResult.Exists())
			FV_WARNING_MSG( "createPropertyMapping: don't know how to "
					"persist property %s of type %s to the database; ignoring.\n",
					propName.c_str(), type.TypeName().c_str() );

		return pResult;
	}

	void upgradeDatabase1_9NonNull( MySql& db )
	{
		FV_INFO_MSG( "Dropping column 'version' from tables FutureVisionSpaces and "
				"FutureVisionSpaceData\n" );
		db.Execute( "ALTER TABLE FutureVisionSpaces DROP COLUMN version" );
		db.Execute( "ALTER TABLE FutureVisionSpaceData DROP COLUMN version" );

		FV_INFO_MSG( "Converting tables FutureVisionSpaces, FutureVisionSpaceData and "
				"FutureVisionGameTime tables to use InnoDB engine\n" );
		db.Execute( "ALTER TABLE FutureVisionSpaces ENGINE=InnoDB" );
		db.Execute( "ALTER TABLE FutureVisionSpaceData ENGINE=InnoDB" );
		db.Execute( "ALTER TABLE FutureVisionGameTime ENGINE=InnoDB" );

		FV_INFO_MSG( "Adding index to id column of FutureVisionSpaceData\n" );
		db.Execute( "ALTER TABLE FutureVisionSpaceData ADD INDEX (id)" );

		FV_INFO_MSG( "\tUpdating version number\n" );
		std::stringstream	ss;
		ss << "UPDATE FutureVisionInfo SET version=" << DBMGR_CURRENT_VERSION;
		db.Execute( ss.str() );
	}

	void upgradeDatabase1_9Snapshot( MySql& db, const FvEntityDefs& entityDefs )
	{
		MySqlTransaction transaction(db);

		TypeMappings propMappings;
		CreateEntityPropMappings( propMappings, entityDefs, TABLE_NAME_PREFIX );

		SimpleTableCollector entityTableCollector;
		for ( FvEntityTypeID e = 0; e < entityDefs.GetNumEntityTypes(); ++e )
		{
			if (!entityDefs.IsValidEntityType( e ))
				continue;

			PropertyMappings& properties = propMappings[e];
			const FvEntityDescription& entDes = entityDefs.GetEntityDescription(e);

			MySqlEntityMapping entityMapping( entDes, properties );

			VisitSubTablesRecursively( entityMapping, entityTableCollector );
		}
		const SimpleTableCollector::NewTableDataMap& entityTables =
				entityTableCollector.GetTables();

		StrSet	tableNames;
		TableMetaData::GetEntityTables( tableNames, db );

		for (StrSet::const_iterator pTblName = tableNames.begin();
				pTblName != tableNames.end(); ++pTblName )
		{
			FV_INFO_MSG( "Adding \"NOT NULL\" specification to columns in "
					"table %s\n", pTblName->c_str() );

			TableMetaData::NameToColInfoMap columns;
			GetTableColumns( columns, db, *pTblName );

			SimpleTableCollector::NewTableDataMap::const_iterator itColumnsDef =
					entityTables.find( *pTblName );
			const TableMetaData::NameToColInfoMap* pColumnsDef =
					(itColumnsDef != entityTables.end()) ?
							&(itColumnsDef->second) : NULL;
			if (!pColumnsDef)
			{
				FV_WARNING_MSG( "upgradeDatabase1_9Snapshot: Cannot find matching "
						"entity definition for table %s. Default values for "
						"columns won't be set correctly\n", pTblName->c_str() );
			}

			std::stringstream	ss;
			ss << "ALTER TABLE " << *pTblName;
			for ( TableMetaData::NameToColInfoMap::const_iterator pColInfo =
					columns.begin(); pColInfo != columns.end(); ++pColInfo )
			{
				if (pColInfo->first == ID_COLUMN_NAME_STR)
					continue;

				const TableMetaData::ColumnInfo* pDefColInfo = NULL;
				if (pColumnsDef)
				{
					TableMetaData::NameToColInfoMap::const_iterator itDefColInfo
							= pColumnsDef->find( pColInfo->first );
					if (itDefColInfo != pColumnsDef->end())
						pDefColInfo = &itDefColInfo->second;
				}

				if (pDefColInfo)
				{
					FvString	defaultValue =
						pDefColInfo->m_kColumnType.GetDefaultValueAsString( db );
					if (!defaultValue.empty())
					{
						std::stringstream	setNull;
						setNull << "UPDATE " << *pTblName << " SET "
							<< pColInfo->first << '=' << defaultValue
							<< " WHERE " << pColInfo->first << " IS NULL";
						transaction.Execute( setNull.str() );
					}
				}
				else if (pColumnsDef)
				{
					FV_WARNING_MSG( "upgradeDatabase1_9Snapshot: Cannot find "
							"default value for column %s.%s. Existing NULL "
							"values will be set to default value of MySQL "
							"type (not FutureVision type)\n",
							pTblName->c_str(), pColInfo->first.c_str() );
				}

				if (pColInfo != columns.begin())
					ss << ',';
				ss << " MODIFY COLUMN " << pColInfo->first << ' ';
				if (pDefColInfo)
				{
					ss << pDefColInfo->m_kColumnType.GetAsString( db,
							pDefColInfo->m_kIndexType );
				}
				else
				{
					ss << pColInfo->second.m_kColumnType.GetAsString( db,
							pColInfo->second.m_kIndexType );
				}
			}

			transaction.Execute( ss.str() );
		}

		FV_INFO_MSG( "\tRemoving FutureVisionTableMetadata table\n" );
		transaction.Execute( "DROP TABLE FutureVisionTableMetadata" );

		transaction.Commit();
	}

	void upgradeDatabase1_8( MySql& db )
	{
		MySqlTransaction transaction(db);

		FV_INFO_MSG( "Upgrading database tables from 1.8 to 1.9\n" );

		FV_INFO_MSG( "\tAdding snapshotTime column to FutureVisionInfo\n" );
		transaction.Execute( "ALTER TABLE FutureVisionInfo ADD COLUMN "
				"(snapshotTime TIMESTAMP NULL)" );

		transaction.Commit();
	}

	void upgradeDatabase1_7( MySql& db, const FvEntityDefs& entityDefs )
	{
		const FvString& defaultNameProperty =
				entityDefs.GetDefaultNameProperty();
		if (defaultNameProperty.empty())
			throw std::runtime_error( "Upgrade failed because "
				"DBManager/nameProperty is not set. DBManager/nameProperty must be set "
				"to the name property that was used to create this database." );

		FV_INFO_MSG( "Upgrading database tables from 1.7 to 1.8\n" );

		MySqlTransaction transaction(db);


		FV_INFO_MSG( "\tAdding idx column to FutureVisionTableMetadata\n" );
		transaction.Execute( "ALTER TABLE FutureVisionTableMetadata ADD COLUMN idx "
				"INT NOT NULL" );
		transaction.Execute( "UPDATE FutureVisionTableMetadata SET idx=1 WHERE "
				"col='id'" );
		transaction.Execute( "UPDATE FutureVisionTableMetadata SET idx=3 WHERE "
				"col='parentID'" );
		if ( entityDefs.GetNumEntityTypes() > 0 )
		{
			std::stringstream stmtStrm;
			stmtStrm << " UPDATE FutureVisionTableMetadata SET idx=2 WHERE "
					"col='sm_" << defaultNameProperty << "' AND FIELD(tbl";

			for ( FvEntityTypeID typeID = 0; typeID < entityDefs.GetNumEntityTypes();
					++typeID )
			{
				const FvEntityDescription& entDes =
					entityDefs.GetEntityDescription( typeID );
				stmtStrm << ", '"TABLE_NAME_PREFIX"_" << entDes.Name() << "'";
			}
			stmtStrm << ") > 0";

			transaction.Execute( stmtStrm.str() );
		}

		FV_INFO_MSG( "\tRecreating indexes using new index names\n" );
		for ( FvEntityTypeID typeID = 0; typeID < entityDefs.GetNumEntityTypes();
				++typeID )
		{
			const FvEntityDescription& entDes =
					entityDefs.GetEntityDescription( typeID );
			const FvDataDescription* pDataDes =
					entDes.FindProperty( defaultNameProperty );
			if (pDataDes != NULL)
			{
				FvString tblName( TABLE_NAME_PREFIX"_" );
				tblName += entDes.Name();

				FV_DEBUG_MSG( "Recreating index for table %s\n", tblName.c_str() );

				try
				{
					transaction.Execute( "ALTER TABLE " + tblName +
										" DROP INDEX nameIndex" );
				}
				catch (std::exception& e )
				{
					FV_ERROR_MSG( "upgradeDatabase: %s\n", e.what() );
				}
				TableMetaData::ColumnInfo colInfo;
				colInfo.m_kColumnType.m_eFieldType =
						(pDataDes->DatabaseLength() < 1<<16) ?
								MYSQL_TYPE_VAR_STRING : MYSQL_TYPE_BLOB;
				colInfo.m_kIndexType = IMySqlColumnMapping::IndexTypeName;
				CreateEntityTableIndex( transaction, tblName,
						"sm_" + defaultNameProperty, colInfo );
			}
		}

		transaction.Commit();
	}

	void upgradeDatabase( MySql& db, FvUInt32 version,
			const FvEntityDefs& entityDefs )
	{
		if (version == DBMGR_VERSION_1_7)
		{
			upgradeDatabase1_7( db, entityDefs );
			version = DBMGR_VERSION_1_8;
		}

		if (version == DBMGR_VERSION_1_8)
		{
			upgradeDatabase1_8( db );
			version = DBMGR_VERSION_1_9_SNAPSHOT;
		}

		if (version == DBMGR_VERSION_1_9_SNAPSHOT)
		{
			upgradeDatabase1_9Snapshot( db, entityDefs );
			version = DBMGR_VERSION_1_9_NON_NULL;
		}

		FV_ASSERT( version = DBMGR_VERSION_1_9_NON_NULL );
		upgradeDatabase1_9NonNull( db );
	}

}

bool visitPropertyMappings( const FvEntityDefs& entityDefs,
		TypeMappings& propertyMappings, TableInspector& visitor )
{
	TypesCollector typesCollector( visitor.connection() );

	for ( FvEntityTypeID ent = 0; ent < entityDefs.GetNumEntityTypes(); ++ent )
	{
		if (!entityDefs.IsValidEntityType( ent ))
			continue;

		PropertyMappings& properties = propertyMappings[ent];
		const FvEntityDescription& entDes = entityDefs.GetEntityDescription(ent);

		MySqlEntityMapping entityMapping( entDes, properties );
		VisitSubTablesRecursively( entityMapping, visitor );

		typesCollector.addType( ent, entDes.Name() );

		if (properties.size() == 0)
			FV_INFO_MSG( "%s does not have persistent properties.\n",
					  entDes.Name().c_str() );
	}

	if (visitor.deleteUnvisitedTables())
	{
		typesCollector.deleteUnwantedTypes();
	}

	return visitor.isSynced();
}

bool InitEntityTables( MySql& con, const FvEntityDefs& entityDefs,
						FvUInt32 version, bool shouldSyncTablesToDefs )
{
	TypeMappings types;
	CreateEntityPropMappings( types, entityDefs, TABLE_NAME_PREFIX );

	if (version != DBMGR_CURRENT_VERSION)
		upgradeDatabase( con, version, entityDefs );

	FV_INFO_MSG( "\tsyncTablesToDefs = %s\n",
				(shouldSyncTablesToDefs) ? "True" : "False" );

	bool isSynced;
	if (shouldSyncTablesToDefs)
	{
		TableInitialiser tableInitialiser( con );
		isSynced = visitPropertyMappings( entityDefs, types, tableInitialiser );
	}
	else
	{
		TableValidator tableValidator( con );
		isSynced = visitPropertyMappings( entityDefs, types, tableValidator );
	}

	return isSynced;
}

FvString BuildCommaSeparatedQuestionMarks( int num )
{
	if (num <= 0)
		return FvString();

	FvString list;
	list.reserve( (num * 2) - 1 );
	list += '?';

	for ( int i=1; i<num; i++ )
	{
		list += ",?";
	}
	return list;
}

namespace
{
	FvString createInsertStatement( const FvString& tbl,
			const PropertyMappings& properties )
	{
		FvString stmt = "INSERT INTO " + tbl + " (";
		CommaSepColNamesBuilder colNames( properties );
		stmt += colNames.GetResult();
		int numColumns = colNames.GetCount();
		stmt += ") VALUES (";

		stmt += BuildCommaSeparatedQuestionMarks( numColumns );
		stmt += ')';

		return stmt;
	}

	FvString createUpdateStatement( const FvString& tbl,
			const PropertyMappings& properties )
	{
		FvString stmt = "UPDATE " + tbl + " SET ";
		CommaSepColNamesBuilderWithSuffix colNames( properties, "=?" );
		stmt += colNames.GetResult();
		if (colNames.GetCount() == 0)
			return FvString();
		stmt += " WHERE id=?";

		return stmt;
	}

	FvString createSelectStatement( const FvString& tbl,
			const PropertyMappings& properties, const FvString& where, bool getID )
	{
		FvString stmt = "SELECT ";
		if (getID) stmt += "id,";
		CommaSepColNamesBuilder colNames( properties );
		stmt += colNames.GetResult();
		if (getID && (colNames.GetCount() == 0))
			stmt.resize(stmt.length() - 1);	// remove comma
		stmt += " FROM " + tbl;

		if (where.length())
			stmt += " WHERE " + where;

		return stmt;
	}

	FvString createDeleteStatement( const FvString& tbl,
			const FvString& where )
	{
		FvString stmt = "DELETE FROM " + tbl;

		if (where.length())
			stmt += " WHERE " + where;

		return stmt;
	}
}

MySqlEntityMapping::MySqlEntityMapping( const FvEntityDescription& entityDesc,
		PropertyMappings& properties, const FvString& tableNamePrefix ) :
	m_kEntityDesc( entityDesc ),
	m_kTableName( tableNamePrefix + "_" + entityDesc.Name() ),
	m_kProperties( properties )
{
	//! add by Uman, 20100625, 转成小写,与mysql一致,防止数据库名对比时出错
	std::transform(
		m_kTableName.begin(),
		m_kTableName.end(),
		m_kTableName.begin(),
		tolower);
	//! add end
}

FvEntityTypeID MySqlEntityMapping::GetTypeID() const
{
	return m_kEntityDesc.Index();
}

bool MySqlEntityMapping::VisitColumnsWith( IMySqlColumnMapping::IVisitor& visitor )
{
	for ( PropertyMappings::iterator iter = m_kProperties.begin();
			iter != m_kProperties.end(); ++iter )
	{
		if (!(*iter)->VisitParentColumns( visitor ))
			return false;
	}

	return true;
}

bool MySqlEntityMapping::VisitIDColumnWith(	IMySqlIDColumnMapping::IVisitor& visitor )
{
	MySqlIDColumnMappingAdapter idColumn( m_iBoundDBID );
	return visitor.OnVisitIDColumn( idColumn );
}

bool MySqlEntityMapping::VisitSubTablesWith( IMySqlTableMapping::IVisitor& visitor )
{
	for ( PropertyMappings::iterator iter = m_kProperties.begin();
			iter != m_kProperties.end(); ++iter )
	{
		if (!(*iter)->VisitTables( visitor ))
			return false;
	}

	return true;
}



MySqlEntityTypeMapping::MySqlEntityTypeMapping( MySql& con,
		const FvString& tableNamePrefix, const FvEntityDescription& desc,
		PropertyMappings& properties, const FvString& nameProperty ) :
	MySqlEntityMapping( desc, properties, tableNamePrefix ),
	m_kInsertSTMT( con, createInsertStatement( MySqlEntityMapping::GetTableName(),
											properties ) ),
	m_spUpdateSTMT( NULL ),
	m_spSelectNamedSTMT( NULL ),
	m_spSelectNamedForIDSTMT( NULL ),
	m_spSelectIDForNameSTMT( NULL ),
	m_kSelectIDForIDSTMT( con, "SELECT id FROM " +
						MySqlEntityMapping::GetTableName() + " WHERE id=?" ),
	m_spSelectIDSTMT( NULL ),
	m_kDeleteIDSTMT( con, createDeleteStatement(
						MySqlEntityMapping::GetTableName(), "id=?" ) ),
	propsNameMap_(),
	m_pkNameProp(0)
{
	MySqlBindings b;

	const FvString&	tableName = this->GetTableName();
	if (properties.size() > 0)
	{
		for ( PropertyMappings::iterator prop = properties.begin();
			  prop != properties.end(); ++prop )
		{
			(*prop)->PrepareSQL( con );
		}

		ColumnsBindingsBuilder propertyBindings( properties );

		for ( PropertyMappings::const_iterator i = properties.begin();
			i != properties.end(); ++i )
		{
			PropertyMappingPtr pMapping = *i;
			propsNameMap_[pMapping->PropName()] = pMapping.GetObject();
		}

		m_pcFixedCellProps[ CellPositionIdx ] = GetPropMapByName( "position" );
		m_pcFixedCellProps[ CellDirectionIdx ] = GetPropMapByName( "direction" );
		m_pcFixedCellProps[ CellSpaceIDIdx ] = GetPropMapByName( "spaceID" );
		m_pcFixedCellProps[ CellSpaceType ] = GetPropMapByName( "spaceType" );

		m_pcFixedMetaProps[ GameTimeIdx ] =
			GetPropMapByName( GAME_TIME_COLUMN_NAME );
		m_pcFixedMetaProps[ TimestampIdx ] =
			GetPropMapByName( TIMESTAMP_COLUMN_NAME );

		if (!nameProperty.empty())
		{
			PropertyMapping* pNameProp = GetPropMapByName( nameProperty );
			if (pNameProp)
				m_pkNameProp = static_cast< StringLikeMapping* >( pNameProp );
		}

		FvString updateStmtStr = createUpdateStatement( tableName, properties );
		if (!updateStmtStr.empty())
		{
			m_spUpdateSTMT.reset( new MySqlStatement( con, updateStmtStr ) );
			m_spSelectIDSTMT.reset( new MySqlStatement( con,
					createSelectStatement( tableName, properties, "id=?", false ) ) );
		}

		b.clear();
		b << propertyBindings.getBindings();
		m_kInsertSTMT.BindParams( b );

		if (m_spSelectIDSTMT.get())
			m_spSelectIDSTMT->BindResult( b );

		if (m_spUpdateSTMT.get())
		{
			b << this->GetDBIDBuf();
			m_spUpdateSTMT->BindParams( b );
		}

		b.clear();
		b << this->GetDBIDBuf();

		if (m_spSelectIDSTMT.get())
			m_spSelectIDSTMT->BindParams( b );

		if (m_pkNameProp)
		{
			m_spSelectNamedSTMT.reset( new MySqlStatement( con,
				createSelectStatement( tableName, properties, "sm_"+nameProperty+"=?", true ) ) );

			m_spSelectNamedForIDSTMT.reset( new MySqlStatement( con,
				"SELECT id FROM " + tableName + " WHERE sm_" + nameProperty + "=?" ) );

			m_spSelectIDForNameSTMT.reset( new MySqlStatement( con,
				"SELECT sm_" + nameProperty + " FROM " + tableName + " WHERE id=?" ) );

			b.clear();
			m_pkNameProp->AddSelfToBindings(b);

			m_spSelectNamedSTMT->BindParams( b );
			m_spSelectNamedForIDSTMT->BindParams( b );
			m_spSelectIDForNameSTMT->BindResult( b );

			b.clear();
			b << this->GetDBIDBuf();

			m_spSelectIDForNameSTMT->BindParams( b );
			m_spSelectNamedForIDSTMT->BindResult( b );

			b << propertyBindings.getBindings();
			m_spSelectNamedSTMT->BindResult( b );
		}
	}
	else
	{
		for ( int i = 0; i < NumFixedCellProps; ++i )
			m_pcFixedCellProps[i] = 0;
		for ( int i = 0; i < NumFixedMetaProps; ++i )
			m_pcFixedMetaProps[i] = 0;
	}

	b.clear();
	b << this->GetDBIDBuf();
	m_kSelectIDForIDSTMT.BindParams( b );
	m_kSelectIDForIDSTMT.BindResult( b );

	m_kDeleteIDSTMT.BindParams( b );

	std::stringstream strmStmt;
	strmStmt << "SELECT typeID FROM FutureVisionEntityTypes WHERE FutureVisionID=";
	strmStmt << this->GetTypeID();
	MySqlStatement stmtGetID( con, strmStmt.str() );
	b.clear();
	b << m_iMappedType;
	stmtGetID.BindResult( b );
	MySqlTransaction t( con );
	t.Execute( stmtGetID );
	stmtGetID.Fetch();
	t.Commit();
}

bool MySqlEntityTypeMapping::CheckExists( MySqlTransaction& transaction,
	FvDatabaseID dbID )
{
	this->SetDBID( dbID );
	transaction.Execute( m_kSelectIDForIDSTMT );

	return m_kSelectIDForIDSTMT.ResultRows() > 0;
}

FvDatabaseID MySqlEntityTypeMapping::GetDBIDBy( MySqlTransaction& transaction,
	const FvString& name )
{
	if (m_pkNameProp)
	{
		m_pkNameProp->setString( name );
		transaction.Execute( *m_spSelectNamedForIDSTMT );

		if (m_spSelectNamedForIDSTMT->ResultRows())
		{
			m_spSelectNamedForIDSTMT->Fetch();
			return this->GetDBID();
		}
	}

	return 0;
}

bool MySqlEntityTypeMapping::GetName( MySqlTransaction& transaction,
	FvDatabaseID dbID, FvString& name )
{
	if (m_pkNameProp)
	{
		this->SetDBID( dbID );
		transaction.Execute( *m_spSelectIDForNameSTMT );

		if (m_spSelectIDForNameSTMT->ResultRows())
		{
			m_spSelectIDForNameSTMT->Fetch();
			m_pkNameProp->getString(name);
			return true;
		}
	}

	return false;
}

bool MySqlEntityTypeMapping::GetPropsByID( MySqlTransaction& transaction,
										   FvDatabaseID dbID, FvString& name )
{
	bool isOK = true;
	if (m_spSelectIDSTMT.get())
	{
		this->SetDBID( dbID );
		isOK = this->GetPropsImpl( transaction, *m_spSelectIDSTMT );
		if (isOK && m_pkNameProp)
			m_pkNameProp->getString(name);
	}

	return isOK;
}

FvDatabaseID MySqlEntityTypeMapping::GetPropsByName( MySqlTransaction& transaction,
	const FvString& name )
{
	if (m_pkNameProp)
	{
		m_pkNameProp->setString( name );
		if (this->GetPropsImpl( transaction, *m_spSelectNamedSTMT.get() ))
			return this->GetDBID();
	}

	return 0;
}

bool MySqlEntityTypeMapping::GetPropsImpl( MySqlTransaction& transaction,
	MySqlStatement& stmt )
{
	transaction.Execute( stmt );

	bool hasData = stmt.ResultRows();
	if (hasData)
	{
		stmt.Fetch();

		PropertyMappings& properties = this->GetPropertyMappings();
		for ( PropertyMappings::iterator i = properties.begin();
			i != properties.end(); ++i )
		{
			(*i)->GetTableData( transaction, this->GetDBID() );
		}
	}

	return hasData;
}

bool MySqlEntityTypeMapping::DeleteWithID( MySqlTransaction & t, FvDatabaseID id )
{
	this->SetDBID( id );
	t.Execute( m_kDeleteIDSTMT );
	if (t.AffectedRows() > 0)
	{
		FV_ASSERT( t.AffectedRows() == 1 );
		PropertyMappings& properties = this->GetPropertyMappings();
		for ( PropertyMappings::iterator i = properties.begin();
				i != properties.end(); ++i )
		{
			(*i)->DeleteChildren( t, id );
		}
		return true;
	}

	return false;
}


class MySqlBindStreamReader : public FvIDataDescriptionVisitor
{
	MySqlEntityTypeMapping &m_kEntityTypeMap;
	FvBinaryIStream &m_kStream;

public:
	MySqlBindStreamReader( MySqlEntityTypeMapping& entityTypeMap,
		   FvBinaryIStream & stream ) :
		m_kEntityTypeMap( entityTypeMap ),
		m_kStream( stream )
	{}

	virtual bool Visit( const FvDataDescription& propDesc );
};


bool MySqlBindStreamReader::Visit( const FvDataDescription& propDesc )
{
	PropertyMapping* pPropMap =
		m_kEntityTypeMap.GetPropMapByName(propDesc.Name());

	if (pPropMap)
	{
		pPropMap->StreamToBound( m_kStream );
	}
	else
	{
		FV_WARNING_MSG( "MySqlBindStreamReader::Visit: Ignoring value for "
					"property %s\n", propDesc.Name().c_str() );
		propDesc.CreateFromStream( m_kStream, false );
	}

	return true;
}


class MySqlBindStreamWriter : public FvIDataDescriptionVisitor
{
	MySqlEntityTypeMapping& entityTypeMap_;
	FvBinaryOStream & stream_;

public:
	MySqlBindStreamWriter( MySqlEntityTypeMapping& entityTypeMap,
		   FvBinaryOStream & stream ) :
		entityTypeMap_( entityTypeMap ),
		stream_( stream )
	{}

	virtual bool Visit( const FvDataDescription& propDesc );
};

bool MySqlBindStreamWriter::Visit( const FvDataDescription& propDesc )
{
	PropertyMapping* pPropMap =
		entityTypeMap_.GetPropMapByName( propDesc.Name() );
	if (pPropMap)
	{
		pPropMap->BoundToStream( stream_ );
	}
	else
	{
		FV_WARNING_MSG( "MySqlBindStreamWriter::writeProp: Making up default "
					"value for property %s\n", propDesc.Name().c_str() );
		FvObjPtr pDefaultVal = propDesc.pInitialValue();
		propDesc.AddToStream( pDefaultVal.GetObject(), stream_, false );
	}

	return true;
}

void MySqlEntityTypeMapping::StreamToBound( FvBinaryIStream& strm )
{
	this->StreamEntityPropsToBound( strm );
	this->StreamMetaPropsToBound( strm );
}

void MySqlEntityTypeMapping::StreamEntityPropsToBound( FvBinaryIStream& strm )
{
	//MySqlBindStreamReader visitor( *this, strm );
	const FvEntityDescription&	desc = this->GetEntityDescription();
	//desc.Visit( FvEntityDescription::FV_BASE_DATA | FvEntityDescription::FV_CELL_DATA |
	//		FvEntityDescription::FV_ONLY_PERSISTENT_DATA, visitor );
	//! modify by Uman, 20101120, 配合新的dll脚本对属性流化的顺序,改为:按定义顺序的base存档属性,按定义顺序的cell存档属性
	FvUInt32 uiPropCnt = desc.PropertyCount();
	FvDataDescription* pkProp = NULL;
	PropertyMapping* pkPropMap = NULL;
	for(FvUInt32 i=0; i<uiPropCnt; ++i)
	{
		pkProp = desc.Property(i);
		if(pkProp->IsBaseData() && pkProp->IsPersistent())
		{
			pkPropMap = GetPropMapByName(pkProp->Name());
			if(pkPropMap)
			{
				pkPropMap->StreamToBound(strm);
			}
			else
			{
				FV_WARNING_MSG( "MySqlBindStreamReader::Visit: Ignoring value for "
					"property %s\n", pkProp->Name().c_str() );
				FV_ASSERT(0);
			}
		}
	}
	for(FvUInt32 i=0; i<uiPropCnt; ++i)
	{
		pkProp = desc.Property(i);
		if(pkProp->IsCellData() && pkProp->IsPersistent())
		{
			pkPropMap = GetPropMapByName(pkProp->Name());
			if(pkPropMap)
			{
				pkPropMap->StreamToBound(strm);
			}
			else
			{
				FV_WARNING_MSG( "MySqlBindStreamReader::Visit: Ignoring value for "
					"property %s\n", pkProp->Name().c_str() );
				FV_ASSERT(0);
			}
		}
	}

	if (desc.HasCellScript())
	{
		for ( int i = 0; i < NumFixedCellProps; ++i )
		{
			m_pcFixedCellProps[i]->StreamToBound( strm );
		}
	}
}

void MySqlEntityTypeMapping::StreamMetaPropsToBound( FvBinaryIStream& strm )
{
	for ( int i = 0; i < NumFixedMetaProps; ++i )
	{
		m_pcFixedMetaProps[i]->DefaultToBound();
	}
}

void MySqlEntityTypeMapping::BoundToStream( FvBinaryOStream& strm,
	const FvString* pPasswordOverride  )
{
	if (pPasswordOverride)
	{
		PropertyMapping* pPropMap = this->GetPropMapByName( "password" );
		if (dynamic_cast<StringMapping*>(pPropMap) ||
			dynamic_cast<BlobMapping*>(pPropMap) )
		{
			dynamic_cast<StringLikeMapping*>(pPropMap)->setValue(
					*pPasswordOverride );
		}
	}

	//MySqlBindStreamWriter 		visitor( *this, strm );
	const FvEntityDescription&	desc = this->GetEntityDescription();
	//desc.Visit( FvEntityDescription::FV_CELL_DATA | FvEntityDescription::FV_BASE_DATA |
	//		FvEntityDescription::FV_ONLY_PERSISTENT_DATA, visitor );
	//! modify by Uman, 20101120, 配合新的dll脚本对属性流化的顺序,改为:按定义顺序的base存档属性,按定义顺序的cell存档属性
	FvUInt32 uiPropCnt = desc.PropertyCount();
	FvDataDescription* pkProp = NULL;
	PropertyMapping* pkPropMap = NULL;
	for(FvUInt32 i=0; i<uiPropCnt; ++i)
	{
		pkProp = desc.Property(i);
		if(pkProp->IsBaseData() && pkProp->IsPersistent())
		{
			pkPropMap = GetPropMapByName(pkProp->Name());
			if(pkPropMap)
			{
				pkPropMap->BoundToStream(strm);
			}
			else
			{
				FV_WARNING_MSG( "MySqlBindStreamWriter::writeProp: Making up default "
					"value for property %s\n", pkProp->Name().c_str() );
				FV_ASSERT(0);
			}
		}
	}
	for(FvUInt32 i=0; i<uiPropCnt; ++i)
	{
		pkProp = desc.Property(i);
		if(pkProp->IsCellData() && pkProp->IsPersistent())
		{
			pkPropMap = GetPropMapByName(pkProp->Name());
			if(pkPropMap)
			{
				pkPropMap->BoundToStream(strm);
			}
			else
			{
				FV_WARNING_MSG( "MySqlBindStreamWriter::writeProp: Making up default "
					"value for property %s\n", pkProp->Name().c_str() );
				FV_ASSERT(0);
			}
		}
	}

	if (desc.HasCellScript())
	{
		for ( int i = 0; i < NumFixedCellProps; ++i )
		{
			m_pcFixedCellProps[i]->BoundToStream( strm );
		}
	}

	//! remove by Uman, 20100519, 好像真的用不上
	//// xdonlee
	//for ( int i = 0; i < NumFixedMetaProps; ++i )
	//{
	//	m_pcFixedMetaProps[i]->BoundToStream( strm );
	//}
}

FvDatabaseID MySqlEntityTypeMapping::InsertNew( MySqlTransaction& transaction )
{
	transaction.Execute( m_kInsertSTMT );
	FvDatabaseID dbID = transaction.InsertID();

	PropertyMappings& properties = this->GetPropertyMappings();
	for ( PropertyMappings::iterator i = properties.begin();
		i != properties.end(); ++i )
	{
		(*i)->UpdateTable( transaction, dbID );
	}

	return dbID;
}

bool MySqlEntityTypeMapping::Update( MySqlTransaction& transaction )
{
	bool isOK = true;
	if (m_spUpdateSTMT.get())
	{
		transaction.Execute( *m_spUpdateSTMT );

		const char* infoStr = transaction.Info();
		if ( (infoStr) && (atol( infoStr + 14 ) == 1) )
		{
			PropertyMappings& properties = this->GetPropertyMappings();
			for ( PropertyMappings::iterator i = properties.begin();
				i != properties.end(); ++i )
			{
				(*i)->UpdateTable( transaction, this->GetDBID() );
			}
		}
		else
		{
			isOK = false;
		}
	}

	return isOK;
}


MySqlTypeMapping::MySqlTypeMapping( MySql& con, const FvEntityDefs& entityDefs,
		const char * tableNamePrefix ) :
	m_kMappings(),
	m_kStatementAddLogOn( con, "INSERT INTO FutureVisionLogOns (databaseID, typeID, objectID, ip, port, salt) "
						"VALUES (?,?,?,?,?,?) ON DUPLICATE KEY UPDATE "
						"objectID=VALUES(objectID), ip=VALUES(ip), port=VALUES(port), salt=VALUES(salt)" ),
	m_kStatementRemoveLogOn( con, "DELETE FROM FutureVisionLogOns WHERE databaseID=? AND typeID=?" ),
	m_kStatementGetLogOn( con, "SELECT objectID, ip, port, salt FROM FutureVisionLogOns "
				"WHERE databaseID=? and typeID=?" ),
	m_kStatementSetLogOnMapping( con, "REPLACE INTO FutureVisionLogOnMapping (logOnName, password, typeID, recordName) "
				"VALUES (?,?,?,?)" ),
	m_kStatementGetLogOnMapping( con, "SELECT m.password, t.FutureVisionID, m.recordName "
				"FROM FutureVisionLogOnMapping m, FutureVisionEntityTypes t "
				"WHERE m.logOnName=? and m.typeID=t.typeID" ),
	m_kBoundLogOnName(BWMySQLMaxLogOnNameLen),
	m_kBoundPassword(BWMySQLMaxLogOnPasswordLen),
	m_kBoundRecordName(BWMySQLMaxNamePropertyLen)
{
	CreateEntityMappings( m_kMappings, entityDefs, tableNamePrefix, con );

	MySqlBindings b;

	b.clear();
	b << m_uiBoundDatabaseID << m_iBoundTypeID;
	m_kStatementRemoveLogOn.BindParams( b );
	m_kStatementGetLogOn.BindParams( b );

	b << m_kBoundOptEntityID << m_kBoundAddress << m_kBoundPort << m_kBoundSalt;
	m_kStatementAddLogOn.BindParams( b );

	b.clear();
	b << m_kBoundOptEntityID << m_kBoundAddress << m_kBoundPort << m_kBoundSalt;
	m_kStatementGetLogOn.BindResult( b );

	b.clear();
	this->AddLogonMappingBindings( b );
	m_kStatementSetLogOnMapping.BindParams( b );

	b.clear();
	b << m_kBoundPassword << m_iBoundTypeID << m_kBoundRecordName;
	m_kStatementGetLogOnMapping.BindResult( b );
	b.clear();
	b << m_kBoundLogOnName;
	m_kStatementGetLogOnMapping.BindParams( b );
}

MySqlTypeMapping::~MySqlTypeMapping()
{
	this->ClearMappings();
}

void MySqlTypeMapping::ClearMappings()
{
	for ( MySqlEntityTypeMappings::iterator i = m_kMappings.begin();
			i < m_kMappings.end(); ++i )
	{
		delete *i;
	}
	m_kMappings.clear();
}

bool MySqlTypeMapping::HasNameProp( FvEntityTypeID typeID ) const
{
	return m_kMappings[typeID]->HasNameProp();
}

FvDatabaseID MySqlTypeMapping::getEntityDbID( MySqlTransaction& transaction,
	FvEntityTypeID typeID, const FvString& name )
{
	return m_kMappings[typeID]->GetDBIDBy( transaction, name );
}

bool MySqlTypeMapping::GetEntityName( MySqlTransaction& transaction,
	FvEntityTypeID typeID, FvDatabaseID dbID, FvString& name )
{
	return m_kMappings[typeID]->GetName( transaction, dbID, name );
}

bool MySqlTypeMapping::CheckEntityExists( MySqlTransaction& transaction,
	FvEntityTypeID typeID, FvDatabaseID dbID )
{
	return m_kMappings[typeID]->CheckExists( transaction, dbID );
}

bool MySqlTypeMapping::GetEntityToBound( MySqlTransaction& transaction,
	FvEntityDBKey& ekey )
{
	if (ekey.m_iDBID)
	{
		return m_kMappings[ekey.m_uiTypeID]->GetPropsByID( transaction,
			ekey.m_iDBID, ekey.m_kName );
	}
	else
	{
		ekey.m_iDBID = m_kMappings[ekey.m_uiTypeID]->GetPropsByName( transaction,
			ekey.m_kName );
		return ekey.m_iDBID != 0;
	}
}

void MySqlTypeMapping::BoundToStream( FvEntityTypeID typeID,
	FvBinaryOStream& entityDataStrm, const FvString* pPasswordOverride )
{
	m_kMappings[typeID]->BoundToStream( entityDataStrm, pPasswordOverride );
}

bool MySqlTypeMapping::DeleteEntityWithID(
		MySqlTransaction& t, FvEntityTypeID typeID, FvDatabaseID dbID )
{
	return m_kMappings[typeID]->DeleteWithID( t, dbID );
}

void MySqlTypeMapping::LogOnMappingToBound( const FvString& logOnName,
	const FvString& password, FvEntityTypeID typeID,
	const FvString& recordName )
{
	m_kBoundLogOnName.SetString( logOnName );
	m_kBoundPassword.SetString( password );
	m_iBoundTypeID = m_kMappings[typeID]->GetDatabaseTypeID();
	m_kBoundRecordName.SetString( recordName );
}

void MySqlTypeMapping::SetLogOnMapping( MySqlTransaction& transaction  )
{
	transaction.Execute( m_kStatementSetLogOnMapping );
}

bool MySqlTypeMapping::GetLogOnMapping( MySqlTransaction& t, const FvString& logOnName,
		FvString& password, FvEntityTypeID& typeID, FvString& recordName )
{
	m_kBoundLogOnName.SetString( logOnName );
	t.Execute( m_kStatementGetLogOnMapping );
	if (m_kStatementGetLogOnMapping.Fetch())
	{
		if (m_kBoundPassword.IsNull())
			password.clear();
		else
			password = m_kBoundPassword.GetString();
		typeID = m_iBoundTypeID;
		recordName = m_kBoundRecordName.GetString();
		return true;
	}
	else
	{
		return false;
	}
}

bool MySqlTypeMapping::GetLogOnRecord( MySqlTransaction& t, FvEntityTypeID typeID,
		FvDatabaseID dbID, FvEntityMailBoxRef& ref )
{
	m_iBoundTypeID = m_kMappings[typeID]->GetDatabaseTypeID();
	m_uiBoundDatabaseID = dbID;
	t.Execute( m_kStatementGetLogOn );

	if (m_kStatementGetLogOn.Fetch())
	{
		ref.m_iID = *m_kBoundOptEntityID.Get();
		ref.m_kAddr.m_uiIP = htonl( *m_kBoundAddress.Get() );
		ref.m_kAddr.m_uiPort = htons( *m_kBoundPort.Get() );
		ref.m_kAddr.m_uiSalt = *m_kBoundSalt.Get();
		return true;
	}

	return false;
}

void MySqlTypeMapping::StreamToBound( FvEntityTypeID typeID, FvDatabaseID dbID,
									  FvBinaryIStream& entityDataStrm )
{
	MySqlEntityTypeMapping& mapping = *m_kMappings[typeID];
	mapping.StreamToBound( entityDataStrm );
	mapping.SetDBID( dbID );
}

void MySqlTypeMapping::LogOnRecordToBound( FvEntityTypeID typeID, FvDatabaseID dbID,
		const FvEntityMailBoxRef& baseRef )
{
	m_iBoundTypeID = m_kMappings[typeID]->GetDatabaseTypeID();
	m_uiBoundDatabaseID = dbID;
	this->BaseRefToBound( baseRef );
}

void MySqlTypeMapping::BaseRefToBound( const FvEntityMailBoxRef& baseRef )
{
	m_kBoundOptEntityID.Set( baseRef.m_iID );
	m_kBoundAddress.Set( ntohl( baseRef.m_kAddr.m_uiIP ) );
	m_kBoundPort.Set( ntohs( baseRef.m_kAddr.m_uiPort ) );
	m_kBoundSalt.Set( baseRef.m_kAddr.m_uiSalt );
}

FvDatabaseID MySqlTypeMapping::NewEntity( MySqlTransaction& transaction,
									    FvEntityTypeID typeID )
{
	return m_kMappings[typeID]->InsertNew( transaction );
}

bool MySqlTypeMapping::UpdateEntity( MySqlTransaction& transaction,
									 FvEntityTypeID typeID )
{
	return m_kMappings[typeID]->Update( transaction );
}

void MySqlTypeMapping::AddLogOnRecord( MySqlTransaction& transaction,
									   FvEntityTypeID typeID, FvDatabaseID dbID )
{
	m_iBoundTypeID = m_kMappings[typeID]->GetDatabaseTypeID();
	m_uiBoundDatabaseID = dbID;
	transaction.Execute( m_kStatementAddLogOn );
}

void MySqlTypeMapping::RemoveLogOnRecord( MySqlTransaction& t,
		FvEntityTypeID typeID, FvDatabaseID dbID )
{
	m_iBoundTypeID = m_kMappings[typeID]->GetDatabaseTypeID();
	m_uiBoundDatabaseID = dbID;
	t.Execute( m_kStatementRemoveLogOn );
}


void MySqlTypeMapping::AddLogonMappingBindings( MySqlBindings& bindings )
{
	bindings << m_kBoundLogOnName << m_kBoundPassword << m_iBoundTypeID
			<< m_kBoundRecordName;
}

void MySqlTypeMapping::AddLogonRecordBindings( MySqlBindings& bindings )
{
	bindings << m_uiBoundDatabaseID << m_iBoundTypeID << m_kBoundOptEntityID
			<< m_kBoundAddress << m_kBoundPort << m_kBoundSalt;
}

void CreateEntityPropMappings( TypeMappings& types,
							const FvEntityDefs& entityDefs,
							const FvString& tableNamePrefix )
{
	for ( FvEntityTypeID ent = 0; ent < entityDefs.GetNumEntityTypes(); ++ent )
	{
		types.push_back( PropertyMappings() );

		if (!entityDefs.IsValidEntityType( ent ))
			continue;

		PropertyMappings& properties = types.back();
		const FvEntityDescription& entDes = entityDefs.GetEntityDescription(ent);

		const FvString& nameProperty = entityDefs.GetNameProperty(ent);
		Namer namer( entDes.Name(), tableNamePrefix );
		for ( unsigned int i = 0; i < entDes.PropertyCount(); ++i )
		{
			FvDataDescription * dataDes = entDes.Property( i );
			if (dataDes->IsPersistent())
			{
				FvDataType * dataType = dataDes->DataType();

				bool isNameProperty = !nameProperty.empty() &&
							(nameProperty == dataDes->Name());
				PropertyMappingPtr prop =
					createPropertyMapping( namer, dataDes->Name(),
							*dataType, dataDes->DatabaseLength(),
							getDefaultSection( *dataDes ), isNameProperty );
				if (prop.Exists())
					properties.push_back( prop );
			}
		}

		if (entDes.HasCellScript())
		{
			FvVector3 defaultVec(0,0,0);

			FvXMLSectionPtr pDefaultValue = new FvXMLSection( "position" );
			pDefaultValue->SetVector3(defaultVec);

			properties.push_back(
				new VectorMapping<FvVector3,3>( namer, "position", pDefaultValue ) );

			pDefaultValue = new FvXMLSection( "direction" );
			pDefaultValue->SetVector3(defaultVec);
			properties.push_back(
				new VectorMapping<FvVector3,3>( namer, "direction", pDefaultValue ) );

			pDefaultValue = new FvXMLSection( "spaceID" );
			pDefaultValue->SetInt( 0 );
			properties.push_back(
				new NumMapping<FvInt32>( namer, "spaceID", pDefaultValue ) );

			pDefaultValue = new FvXMLSection( "spaceType" );
			pDefaultValue->SetInt( 0 );
			properties.push_back(
				new NumMapping<FvUInt16>( namer, "spaceType", pDefaultValue ) );
		}

		FvXMLSectionPtr pDefaultValue = new FvXMLSection( GAME_TIME_COLUMN_NAME );
		pDefaultValue->SetInt( 0 );
		properties.push_back(
			new NumMapping<FvTimeStamp>( GAME_TIME_COLUMN_NAME, pDefaultValue ) );

		properties.push_back( new TimestampMapping() );
	}
}

void CreateEntityMappings( MySqlEntityTypeMappings& entityMappings,
	TypeMappings& propMappings, const FvEntityDefs& entityDefs,
	const FvString& tableNamePrefix, MySql& connection )
{
	for ( FvEntityTypeID typeID = 0; typeID < entityDefs.GetNumEntityTypes(); ++typeID )
	{
		if (entityDefs.IsValidEntityType( typeID ))
		{
			const FvEntityDescription& entDes =
				entityDefs.GetEntityDescription( typeID );
			entityMappings.push_back( new MySqlEntityTypeMapping( connection,
									tableNamePrefix,entDes,
									propMappings[typeID],
									entityDefs.GetNameProperty( typeID ) ) );
		}
		else
		{
			entityMappings.push_back( NULL );
		}
	}
}

void CreateEntityMappings( MySqlEntityTypeMappings& entityMappings,
	const FvEntityDefs& entityDefs, const FvString& tableNamePrefix,
	MySql& connection )
{
	TypeMappings propMappings;
	CreateEntityPropMappings( propMappings, entityDefs, tableNamePrefix );

	CreateEntityMappings( entityMappings, propMappings, entityDefs,
		tableNamePrefix, connection );
}
