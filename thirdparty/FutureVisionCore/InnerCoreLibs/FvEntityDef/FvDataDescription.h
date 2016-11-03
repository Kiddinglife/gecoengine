//{future header message}
#ifndef __FvDataDescription_H__
#define __FvDataDescription_H__

#include "FvEntityDefDefines.h"
#include "FvMemberDescription.h"
#include "FvPropertyOwnerBase.h"
#include <FvXMLSection.h>
#include <set>


class FvBinaryOStream;
class FvBinaryIStream;
class FvMD5;


enum FvEntityDataFlags
{
	FV_DATA_GHOSTED_OLD			= 0x01,		///< Synchronised to ghost entities.
	FV_DATA_OTHER_CLIENT_OLD	= 0x02,		///< Sent to other clients.
	FV_DATA_OWN_CLIENT_OLD		= 0x04,		///< Sent to own client.
	FV_DATA_BASE_OLD			= 0x08,		///< Sent to the base.
	FV_DATA_CLIENT_ONLY_OLD		= 0x10,		///< Static client-side data only.
	FV_DATA_PERSISTENT_OLD		= 0x20,		///< Saved to the database.
	FV_DATA_EDITOR_ONLY_OLD		= 0x40,		///< Only read and written by editor.
	FV_DATA_ID_OLD				= 0x80,		///< Is an indexed column in the database.
};

#define FV_DATA_DISTRIBUTION_FLAGS (FV_DATA_GHOSTED_OLD | FV_DATA_OTHER_CLIENT_OLD | \
								FV_DATA_OWN_CLIENT_OLD | FV_DATA_BASE_OLD | 		\
								FV_DATA_CLIENT_ONLY_OLD | FV_DATA_EDITOR_ONLY_OLD)

#define FV_DEFAULT_DATABASE_LENGTH 65535

#ifdef FV_EDITOR_ENABLED
class GeneralProperty;
class EditorChunkEntity;
class FvZoneItem;
#endif


class FvDataType;
typedef FvSmartPointer<FvDataType> FvDataTypePtr;

class FV_ENTITYDEF_API FvMetaDataType
{
public:
	FvMetaDataType(const char* name, const FvUInt32 id):m_pcName(name),m_uiID(id) {}
	virtual ~FvMetaDataType() {}

	static FvMetaDataType* Find( const FvString & name );
	static FvMetaDataType* Find( FvUInt32 id );
	static void Fini();
	static void AddAlias( const FvString& orig, const FvString& alias );

	const char* Name() const { return m_pcName; }
	FvUInt32 ID() const { return m_uiID; }
	virtual FvDataTypePtr GetType(FvXMLSectionPtr pSection) = 0;

protected:
	static void AddMetaType( FvMetaDataType * pMetaType );
	static void DelMetaType( FvMetaDataType * pMetaType );

private:
	typedef std::map< FvString, FvMetaDataType* > FvMetaDataTypesByName;
	typedef std::map< FvUInt32, FvMetaDataType* > FvMetaDataTypesByID;
	static FvMetaDataTypesByName*	ms_pkMetaDataTypesByName;
	static FvMetaDataTypesByID*		ms_pkMetaDataTypesByID;
	const char* m_pcName;
	const FvUInt32 m_uiID;
};


class FvPropertyOwnerBase;
class FvObj;
typedef FvSmartPointer<FvObj> FvObjPtr;
enum FvDataTypeID_Old;

class FV_ENTITYDEF_API FvDataType : public FvReferenceCount
{
public:
	FvDataType(FvMetaDataType* pMetaDataType)
	:m_pkMetaDataType(pMetaDataType) {}
	virtual ~FvDataType();

	FvMetaDataType * pMetaDataType() { return m_pkMetaDataType; }
	const FvMetaDataType * pMetaDataType() const { return m_pkMetaDataType; }
	bool IsSameType(FvObjPtr pValue);
	FvUInt32 TypeID() const { return m_pkMetaDataType->ID(); }
	virtual FvString TypeName() const { return m_pkMetaDataType->Name(); }
	virtual void AddToMD5(FvMD5& md5) const = 0;
	virtual bool operator<(const FvDataType& other) const { return m_pkMetaDataType<other.m_pkMetaDataType; }

	virtual void SetDefaultValue( FvXMLSectionPtr pSection ) = 0;
	virtual FvObjPtr pDefaultValue() const = 0;
	virtual FvXMLSectionPtr pDefaultSection() const;

	virtual void AddToStream( FvObjPtr pValue, FvBinaryOStream & stream, bool isPersistentOnly ) const = 0;
	virtual FvObjPtr CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const = 0;
	virtual void AddToSection( FvObjPtr pValue, FvXMLSectionPtr pSection ) const = 0;
	virtual FvObjPtr CreateFromSection( FvXMLSectionPtr pSection ) const = 0;

	virtual void ReloadScript() {}
	virtual void ClearScript() {}

	virtual bool AllowNone() const { return false; }

	////! 移除待定
	//virtual FvObjPtr Attach( FvObjPtr pObject, FvPropertyOwnerBase * pOwner, FvInt32 ownerRef );
	//virtual void Detach( FvObjPtr pObject );
	//virtual FvPropertyOwnerBase * AsOwner( FvObjPtr pObject );

	static void ClearStaticsForReload();
	//static void ReloadAllScript()
	//{
	//	FvDataType::CallOnEach( &FvDataType::ReloadScript );
	//}
	//static void ClearAllScript()
	//{
	//	FvDataType::CallOnEach( &FvDataType::ClearScript );
	//}
	//static void CallOnEach( void (FvDataType::*f)() );
	////! end

	static FvDataTypePtr BuildDataType( FvXMLSectionPtr pSection );
	static void DumpDataType( FvXMLSectionPtr pSection );
	static void DumpAliases( FvXMLSectionPtr pSection );

#ifdef FV_EDITOR_ENABLED
	virtual GeneralProperty * CreateEditorProperty( const FvString& name,
		FvZoneItem* chunkItem, FvInt32 editorEntityPropertyId )
		{ return NULL; }

	static FvXMLSectionPtr FindAliasWidget( const FvString& name )
	{
		AliasWidgets::iterator i = ms_AliasWidgets.find( name );
		if ( i != ms_AliasWidgets.end() )
			return (*i).second;
		else
			return NULL;
	}

	static void Fini()
	{
		ms_AliasWidgets.clear();
	}
#endif

protected:
	FvMetaDataType*	m_pkMetaDataType;

private:
	static FvDataTypePtr FindOrAddType( FvDataTypePtr pDT );
	static bool InitAliases();

	struct SingletonPtr
	{
		explicit SingletonPtr( FvDataType * pInst ) : m_pkInst( pInst ) {}

		FvDataType * m_pkInst;

		bool operator<( const SingletonPtr & me ) const { return (*m_pkInst) < (*me.m_pkInst); }
	};
	typedef std::set<SingletonPtr> SingletonMap;
	static SingletonMap*ms_pkSingletonMap;
	typedef std::map< FvString, FvDataTypePtr >	Aliases;
	static Aliases		ms_kAliases;
#ifdef FV_EDITOR_ENABLED
	typedef std::map< FvString, FvXMLSectionPtr >	AliasWidgets;
	static AliasWidgets ms_AliasWidgets;
#endif // FV_EDITOR_ENABLED
};

class FV_ENTITYDEF_API FvDataDescription : public FvMemberDescription
{
public:
	FvDataDescription();
	FvDataDescription( const FvDataDescription& description );
	~FvDataDescription();

	enum PARSE_OPTIONS
	{
		PARSE_DEFAULT,			// Parses all known sections.
		PARSE_IGNORE_FLAGS = 1	// Ignores the 'Flags' section.
	};

	bool		Parse( FvXMLSectionPtr pSection, const FvString & parentName, PARSE_OPTIONS options = PARSE_DEFAULT );

	bool		IsCorrectType( FvObjPtr pNewValue );
	FvDataType*	DataType();
	const FvDataType*DataType() const;

	void		AddToStream( FvObjPtr pNewValue, FvBinaryOStream & stream, bool isPersistentOnly ) const;
	FvObjPtr	CreateFromStream( FvBinaryIStream & stream, bool isPersistentOnly ) const;
	void		AddToSection( FvObjPtr pNewValue, FvXMLSectionPtr pSection ) const;
	FvObjPtr	CreateFromSection( FvXMLSectionPtr pSection ) const;
	void		AddToMD5( FvMD5 & md5 ) const;

	const FvString&	Name() const;
	FvObjPtr	pInitialValue() const;
	FvXMLSectionPtr pDefaultSection() const;



	bool		IsGhostedData() const;
	bool		IsOtherClientData() const;
	bool		IsOwnClientData() const;
	bool		IsCellData() const;
	bool		IsBaseData() const;
	bool		IsClientOnlyData() const;
	bool		IsClientServerData() const;
	bool		IsPersistent() const;
	bool		IsIdentifier() const;
	bool		IsEditorOnly() const;
	bool		IsOfType( FvEntityDataFlags flags );
	const char* GetDataFlagsAsStr() const;

	FvInt32		Index() const;
	void		Index( FvInt32 index );

	FvInt32		LocalIndex() const;
	void		LocalIndex( FvInt32 i );

	FvInt32		EventStampIndex() const;
	void		EventStampIndex( FvInt32 index );

	FvInt32		ClientServerFullIndex() const;
	void		ClientServerFullIndex( FvInt32 i );

	FvInt32		DetailLevel() const;
	void		DetailLevel( FvInt32 level );

	FvInt32		DatabaseLength() const;

	void		Dump(FvXMLSectionPtr pSection);

#ifdef FV_EDITOR_ENABLED
	bool		Editable() const;
	void		Editable( bool v );

	void		Widget( FvXMLSectionPtr pSection );
	FvXMLSectionPtr Widget();
#endif

#ifdef FV_ENABLE_WATCHERS
	static FvWatcherPtr pWatcher();
#endif

private:
	FvString		m_kName;
	FvDataTypePtr	m_spDataType;
	FvUInt32		m_uiDataFlags;
	FvObjPtr		m_spInitialValue;

	FvInt32			m_iIndex;
	FvInt32			m_iLocalIndex;		// Index into local prop value vector.	//! 好像没有用
	FvInt32			m_iEventStampIndex;	// Index into time-stamp vector.
	FvInt32			m_iClientServerFullIndex;

	FvInt32			m_iDetailLevel;

	FvInt32			m_iDatabaseLength;

#ifdef FV_EDITOR_ENABLED
	bool			m_bEditable;
	FvXMLSectionPtr m_spWidgetSection;
#endif

};

////! 移除待定
//class FV_ENTITYDEF_API PropertyOwner : public FvPropertyOwnerBase
//{
//protected:
//	PropertyOwner() {}
//};
//
//template <class C> class PropertyOwnerLink : public FvPropertyOwnerBase
//{
//public:
//	PropertyOwnerLink( C & self ) : m_Self( self ) { }
//
//	virtual void PropertyChanged( FvObjPtr val, const FvDataType & type,
//			ChangePath path )
//		{ m_Self.PropertyChanged( val, type, path ); }
//
//	virtual FvInt32 PropertyDivisions()
//		{ return m_Self.PropertyDivisions(); }
//	virtual FvPropertyOwnerBase * PropertyVassal( FvUInt32 ref )
//		{ return m_Self.PropertyVassal( ref ) ; }
//	virtual FvObjPtr PropertyRenovate( FvInt32 ref, FvBinaryIStream & data,
//			FvObjPtr & pValue, FvDataType *& pType )
//		{ return m_Self.PropertyRenovate( ref, data, pValue, pType ); }
//
//private:
//	C & m_Self;
//};
////! end


#include "FvDataDescription.inl"


#endif // __FvDataDescription_H__

