//{future header message}
#ifndef __FvMySQLTypeMapping_H__
#define __FvMySQLTypeMapping_H__

#include "FvMySQLWrapper.h"
#include "FvMySQLTable.h"
#include "FvIDatabase.h"

#include <FvSmartPointer.h>
#include <vector>
#include <set>

class FvEntityDescription;
class MySqlEntityTypeMapping;
class StringLikeMapping;
class TableIndices;
typedef std::map<FvString, FvString> StrStrMap;
typedef std::set<FvString> StrSet;
typedef std::vector<MySqlEntityTypeMapping*> MySqlEntityTypeMappings;

static const FvUInt32	DBMGR_VERSION_1_7		= 1;		// 1.7
static const FvUInt32	DBMGR_VERSION_1_8		= 2;		// 1.8
static const FvUInt32	DBMGR_VERSION_1_9_SNAPSHOT = 3;		// 1.9 snapshot
static const FvUInt32	DBMGR_VERSION_1_9_NON_NULL = 4;		// 1.9 non-null columns
static const FvUInt32	DBMGR_CURRENT_VERSION	= 5;
static const FvUInt32	DBMGR_OLDEST_SUPPORTED_VERSION	= 1;

bool InitEntityTables( MySql& con, const FvEntityDefs& entityDefs,
		FvUInt32 version, bool shouldSyncTablesToDefs );

namespace TableMetaData
{
	struct ColumnInfo
	{
		IMySqlColumnMapping::Type m_kColumnType;
		IMySqlColumnMapping::IndexType m_kIndexType;

		ColumnInfo( const MYSQL_FIELD& field, const TableIndices& indices );
		ColumnInfo();

		bool operator==( const ColumnInfo& other ) const;
		bool operator!=( const ColumnInfo& other ) const
		{	return !this->operator==( other );	}

	private:
		static IMySqlColumnMapping::IndexType DeriveIndexType(
				const MYSQL_FIELD& field, const TableIndices& indices );
	};

	typedef std::map< FvString, ColumnInfo > NameToColInfoMap;

	struct UpdatedColumnInfo : public ColumnInfo
	{
		IMySqlColumnMapping::IndexType	oldIndexType;

		UpdatedColumnInfo( const ColumnInfo& newCol, const ColumnInfo& oldCol )
			:  ColumnInfo( newCol ),
			oldIndexType( oldCol.m_kIndexType )
		{}
	};
	typedef std::map< FvString, UpdatedColumnInfo > NameToUpdatedColInfoMap;

	void GetEntityTables( StrSet& tables, MySql& connection );
	void GetTableColumns( TableMetaData::NameToColInfoMap& columns,
			MySql& connection, const FvString& tableName );
};

class FutureVisionMetaData
{
	MySql &m_kConnection;
	MySqlStatement m_kSTMTGetEntityTypeID;
	MySqlStatement m_kSTMTSetEntityTypeID;
	MySqlStatement m_kSTMTAddEntityType;
	MySqlStatement m_kSTMTRemoveEntityType;

	MySqlBuffer m_kBufferTypeName;
	int m_iBufferInteger;
public:
	FutureVisionMetaData( MySql& connection );

	MySql &Connection()		{ return m_kConnection; }

	FvEntityTypeID GetEntityTypeID( const FvString& entityName );
	void SetEntityTypeID( const FvString& entityName, FvEntityTypeID typeID );
	void AddEntityType( const FvString& entityName, FvEntityTypeID typeID );
	void RemoveEntityType( const FvString& entityName );
};

class SimpleTableCollector : public IMySqlTableMapping::IVisitor
{
public:
	typedef std::map< FvString, TableMetaData::NameToColInfoMap >
			NewTableDataMap;
	NewTableDataMap	m_kTables;

	virtual ~SimpleTableCollector()	{}

	const NewTableDataMap &GetTables() const
	{
		return m_kTables;
	}

	SimpleTableCollector& operator+=( const SimpleTableCollector& rhs );

	virtual bool OnVisitTable( IMySqlTableMapping& table );
};

void CreateEntityTableIndex( MySql& connection,
	const FvString& tblName, const FvString& colName,
	const TableMetaData::ColumnInfo& colInfo );

class PropertyMapping : public FvReferenceCount
{
public:
	PropertyMapping( const FvString& propName ) :
		m_kPropName(propName)
	{}
	virtual ~PropertyMapping() {}

	virtual void PrepareSQL( MySql& ) = 0;

	const FvString &PropName() const	{ return m_kPropName; }
	virtual void StreamToBound( FvBinaryIStream & strm ) = 0;
	virtual void BoundToStream( FvBinaryOStream & strm ) const = 0;
	virtual void DefaultToStream( FvBinaryOStream & strm ) const = 0;
	virtual void DefaultToBound() = 0;

	virtual bool HasTable() const = 0;
	virtual void UpdateTable( MySqlTransaction& transaction,
		FvDatabaseID parentID ) = 0;
	virtual void GetTableData( MySqlTransaction& transaction,
		FvDatabaseID parentID ) = 0;

	virtual bool VisitParentColumns( IMySqlColumnMapping::IVisitor& visitor ) = 0;
	virtual bool VisitTables( IMySqlTableMapping::IVisitor& visitor ) = 0;

	struct ISequenceBuffer
	{
		virtual ~ISequenceBuffer() {};

		virtual void StreamToBuffer( int numElems,
										FvBinaryIStream& strm ) = 0;
		virtual void BufferToStream( FvBinaryOStream& strm, int idx ) const = 0;
		virtual void BufferToBound( PropertyMapping& binding,
									int idx ) const = 0;
		virtual void BoundToBuffer( const PropertyMapping& binding ) = 0;
		virtual int	GetNumElems() const = 0;
		virtual void Reset() = 0;
	};
	virtual ISequenceBuffer *CreateSequenceBuffer() const = 0;

	virtual void DeleteChildren( MySqlTransaction&, FvDatabaseID parentID ) = 0;

private:
	FvString m_kPropName;
};

typedef FvSmartPointer<PropertyMapping> PropertyMappingPtr;
typedef std::vector<PropertyMappingPtr> PropertyMappings;
typedef std::vector<PropertyMappings> TypeMappings;

class CommaSepColNamesBuilder : public IMySqlColumnMapping::IVisitor,
								public IMySqlIDColumnMapping::IVisitor
{
protected:
	std::stringstream m_kCommaSepColumnNames;
	int m_iCount;

	CommaSepColNamesBuilder() : m_iCount( 0 ) {}

public:
	CommaSepColNamesBuilder( PropertyMapping& property ) : m_iCount( 0 )
	{
		property.VisitParentColumns( *this );
	}

	CommaSepColNamesBuilder( const PropertyMappings& properties ) : m_iCount( 0 )
	{
		for ( PropertyMappings::const_iterator it = properties.begin();
				it != properties.end(); ++it )
		{
			(*it)->VisitParentColumns( *this );
		}
	}

	CommaSepColNamesBuilder( IMySqlTableMapping& table, bool visitIDCol ) :
		m_iCount( 0 )
	{
		if (visitIDCol)
			table.VisitIDColumnWith( *this );
		table.VisitColumnsWith( *this );
	}

	FvString GetResult() const	{ return m_kCommaSepColumnNames.str(); }
	int GetCount() const	{ return m_iCount; }

	bool OnVisitColumn( IMySqlColumnMapping& column )
	{
		if (column.HasBinding())
		{
			if (m_iCount > 0)
				m_kCommaSepColumnNames << ',';
			m_kCommaSepColumnNames << column.GetColumnName();

			++m_iCount;
		}

		return true;
	}

	bool OnVisitIDColumn( IMySqlIDColumnMapping& column )
	{
		return this->OnVisitColumn( column );
	}
};

class CommaSepColNamesBuilderWithSuffix : public CommaSepColNamesBuilder
{
	typedef ColumnVisitorArgPasser< CommaSepColNamesBuilderWithSuffix,
			const FvString > SuffixPasser;

public:
	CommaSepColNamesBuilderWithSuffix( const PropertyMappings& properties,
			const FvString& suffix = FvString() )
	{
		SuffixPasser proxyVisitor( *this, suffix );
		for ( PropertyMappings::const_iterator it = properties.begin();
				it != properties.end(); ++it )
		{
			(*it)->VisitParentColumns( proxyVisitor );
		}
	}

	CommaSepColNamesBuilderWithSuffix( PropertyMapping& property,
			const FvString& suffix = FvString() )
	{
		SuffixPasser proxyVisitor( *this, suffix );
		property.VisitParentColumns( proxyVisitor );
	}

	bool OnVisitColumn( IMySqlColumnMapping& column,
			const FvString& suffix )
	{
		bool shouldContinue = CommaSepColNamesBuilder::OnVisitColumn( column );
		if (column.HasBinding())
		{
			m_kCommaSepColumnNames << suffix;
		}

		return shouldContinue;
	}
};

class ColumnsBindingsBuilder : public IMySqlColumnMapping::IVisitor,
								public IMySqlIDColumnMapping::IVisitor
{
	MySqlBindings	bindings_;

public:
	ColumnsBindingsBuilder( PropertyMapping& property )
	{
		property.VisitParentColumns( *this );
	}

	ColumnsBindingsBuilder( const PropertyMappings& properties )
	{
		for ( PropertyMappings::const_iterator it = properties.begin();
				it != properties.end(); ++it )
		{
			(*it)->VisitParentColumns( *this );
		}
	}

	ColumnsBindingsBuilder( IMySqlTableMapping& table )
	{
		table.VisitIDColumnWith( *this );
		table.VisitColumnsWith( *this );
	}

	MySqlBindings& getBindings() { return bindings_; }

	virtual bool OnVisitColumn( IMySqlColumnMapping& column )
	{
		if (column.HasBinding())
		{
			column.AddSelfToBindings( bindings_ );
		}
		return true;
	}

	bool OnVisitIDColumn( IMySqlIDColumnMapping& column )
	{
		return this->OnVisitColumn( column );
	}
};

FvString BuildCommaSeparatedQuestionMarks( int num );

void CreateEntityPropMappings( TypeMappings& types,
							const FvEntityDefs& entityDefs,
							const FvString& tableNamePrefix );
void CreateEntityMappings( MySqlEntityTypeMappings& entityMappings,
	TypeMappings& propMappings, const FvEntityDefs& entityDefs,
	const FvString& tableNamePrefix, MySql& connection );
void CreateEntityMappings( MySqlEntityTypeMappings& entityMappings,
	const FvEntityDefs& entityDefs, const FvString& tableNamePrefix,
	MySql& connection );


class MySqlEntityMapping : public IMySqlTableMapping
{
	const FvEntityDescription &m_kEntityDesc;
	FvString m_kTableName;
	PropertyMappings m_kProperties;

	FvDatabaseID m_iBoundDBID;

public:
	MySqlEntityMapping( const FvEntityDescription& entityDesc,
			PropertyMappings& properties,
			const FvString& tableNamePrefix = TABLE_NAME_PREFIX );
	virtual ~MySqlEntityMapping() {};

	const FvEntityDescription &GetEntityDescription() const { return m_kEntityDesc; }
	const PropertyMappings &GetPropertyMappings() const	{ return m_kProperties; }
	PropertyMappings &GetPropertyMappings() { return m_kProperties; }

	FvDatabaseID GetDBID() const		{ return m_iBoundDBID; }
	FvDatabaseID &GetDBIDBuf()		{ return m_iBoundDBID; }
	void SetDBID( FvDatabaseID dbID )	{ m_iBoundDBID = dbID; }

	FvEntityTypeID GetTypeID() const;

	virtual const FvString& GetTableName() const
	{	return m_kTableName;	}
	virtual bool VisitColumnsWith( IMySqlColumnMapping::IVisitor& visitor );
	virtual bool VisitIDColumnWith( IMySqlIDColumnMapping::IVisitor& visitor );
	virtual bool VisitSubTablesWith( IMySqlTableMapping::IVisitor& visitor );
	virtual IRowBuffer* GetRowBuffer() { return NULL; }	// no row buffer.
};


class MySqlEntityTypeMapping : public MySqlEntityMapping
{
public:

	MySqlEntityTypeMapping( MySql& conneciton,
		const FvString& tableNamePrefix, const FvEntityDescription& desc,
		PropertyMappings& properties, const FvString& nameProperty );

	FvDatabaseID GetDBIDBy( MySqlTransaction& transaction,
		const FvString& name );
	bool GetName( MySqlTransaction& transaction, FvDatabaseID dbID,
		FvString& name );
	bool CheckExists( MySqlTransaction& transaction, FvDatabaseID dbID );
	bool DeleteWithID( MySqlTransaction& t, FvDatabaseID id );

	void StreamToBound( FvBinaryIStream& strm );
	void StreamEntityPropsToBound( FvBinaryIStream& strm );
	void StreamMetaPropsToBound( FvBinaryIStream& strm );
	FvDatabaseID InsertNew( MySqlTransaction& transaction );
	bool Update( MySqlTransaction& transaction );

	bool GetPropsByID( MySqlTransaction& transaction, FvDatabaseID dbID,
		FvString& name );
	FvDatabaseID GetPropsByName( MySqlTransaction& transaction,
		const FvString& name );
	void BoundToStream( FvBinaryOStream& strm, const FvString* pPasswordOverride );

	int GetDatabaseTypeID() const	{ return m_iMappedType;	}
	bool HasNameProp() const		{ return m_pkNameProp != 0;	}

	PropertyMapping *GetPropMapByName( const FvString& name )
	{
		std::map< FvString, PropertyMapping*>::iterator propMapIter =
			propsNameMap_.find(name);
		return ( propMapIter != propsNameMap_.end() )
					? propMapIter->second : 0;
	}

	const PropertyMapping *GetPropMapByName( const FvString& name ) const
	{
		return const_cast<MySqlEntityTypeMapping*>(this)->GetPropMapByName( name );
	}

private:
	MySqlStatement m_kInsertSTMT;
	std::auto_ptr<MySqlStatement> m_spUpdateSTMT;
	std::auto_ptr<MySqlStatement> m_spSelectNamedSTMT;
	std::auto_ptr<MySqlStatement> m_spSelectNamedForIDSTMT;
	std::auto_ptr<MySqlStatement> m_spSelectIDForNameSTMT;
	MySqlStatement m_kSelectIDForIDSTMT;
	std::auto_ptr<MySqlStatement> m_spSelectIDSTMT;
	MySqlStatement m_kDeleteIDSTMT;
	std::map< FvString, PropertyMapping* > propsNameMap_;

	enum FixedCellProp
	{
		CellPositionIdx,
		CellDirectionIdx,
		CellSpaceIDIdx,
		CellSpaceType,
		NumFixedCellProps
	};
	enum FixedMetaProp
	{
		GameTimeIdx,
		TimestampIdx,
		NumFixedMetaProps
	};
	PropertyMapping *m_pcFixedCellProps[ NumFixedCellProps ];
	PropertyMapping *m_pcFixedMetaProps[ NumFixedMetaProps ];
	StringLikeMapping *m_pkNameProp;

	int m_iMappedType;

	bool GetPropsImpl( MySqlTransaction& transaction, MySqlStatement& stmt );
};

class MySqlTypeMapping
{
public:
	MySqlTypeMapping( MySql& con, const FvEntityDefs& entityDefs,
		const char * tableNamePrefix = TABLE_NAME_PREFIX );
	~MySqlTypeMapping();

	void ClearMappings();

	MySqlEntityTypeMappings &getEntityMappings() { return m_kMappings; }
	MySqlEntityTypeMapping *GetEntityMapping( FvEntityTypeID typeID )
	{ return m_kMappings[typeID]; }

	bool HasNameProp( FvEntityTypeID typeID ) const;
	FvDatabaseID getEntityDbID( MySqlTransaction& transaction,
		FvEntityTypeID typeID, const FvString& name );
	bool GetEntityName( MySqlTransaction& transaction,
		FvEntityTypeID typeID, FvDatabaseID dbID, FvString& name );
	bool CheckEntityExists( MySqlTransaction& transaction,
		FvEntityTypeID typeID, FvDatabaseID dbID );
	bool DeleteEntityWithID( MySqlTransaction&, FvEntityTypeID, FvDatabaseID );

	void StreamToBound( FvEntityTypeID typeID, FvDatabaseID dbID,
		FvBinaryIStream& entityDataStrm );
	void LogOnRecordToBound( FvEntityTypeID typeID, FvDatabaseID dbID,
			const FvEntityMailBoxRef& baseRef );
	void BaseRefToBound( const FvEntityMailBoxRef& baseRef );
	void LogOnMappingToBound( const FvString& logOnName,
		const FvString& password, FvEntityTypeID typeID,
		const FvString& recordName );
	FvString GetBoundLogOnName() const {	return m_kBoundLogOnName.GetString(); }

	FvDatabaseID NewEntity( MySqlTransaction& transaction, FvEntityTypeID typeID );
	bool UpdateEntity( MySqlTransaction& transaction, FvEntityTypeID typeID );
	void AddLogOnRecord( MySqlTransaction&, FvEntityTypeID, FvDatabaseID );
	void RemoveLogOnRecord( MySqlTransaction&, FvEntityTypeID, FvDatabaseID );
	void SetLogOnMapping( MySqlTransaction& transaction );

	bool GetEntityToBound( MySqlTransaction& transaction, FvEntityDBKey& ekey );

	void BoundToStream( FvEntityTypeID typeID, FvBinaryOStream& entityDataStrm,
		const FvString* pPasswordOverride );

	bool GetLogOnRecord( MySqlTransaction&, FvEntityTypeID, FvDatabaseID,
		FvEntityMailBoxRef& );
	bool GetLogOnMapping( MySqlTransaction&, const FvString& logOnName,
			FvString& password, FvEntityTypeID& typeID, FvString& recordName );

	void AddLogonMappingBindings( MySqlBindings& bindings );
	void AddLogonRecordBindings( MySqlBindings& bindings );

private:
	MySqlEntityTypeMappings	m_kMappings;
	MySqlStatement m_kStatementAddLogOn;
	MySqlStatement m_kStatementRemoveLogOn;
	MySqlStatement m_kStatementGetLogOn;

	MySqlStatement m_kStatementSetLogOnMapping;
	MySqlStatement m_kStatementGetLogOnMapping;

	int m_iBoundTypeID;
	FvDatabaseID m_uiBoundDatabaseID;
	FvEntityID m_iBoundEntityID;
	MySqlValueWithNull<FvEntityID> m_kBoundOptEntityID;
	MySqlValueWithNull<FvUInt32> m_kBoundAddress;
	MySqlValueWithNull<FvUInt16> m_kBoundPort;
	MySqlValueWithNull<FvUInt16> m_kBoundSalt;

	MySqlBuffer m_kBoundLogOnName;
	MySqlBuffer m_kBoundPassword;
	MySqlBuffer m_kBoundRecordName;
};

#endif // __FvMySQLTypeMapping_H__
