//{future header message}
#ifndef __FvDBEntityDefs_H__
#define __FvDBEntityDefs_H__

#include <FvEntityDescriptionMap.h>
#include <FvMD5.h>
#include <FvNetTypes.h>

const FvEntityTypeID INVALID_TYPEID = std::numeric_limits<FvUInt16>::max();

class FvEntityDefs
{
	FvEntityDescriptionMap m_kEntityDescriptionMap;
	std::vector<bool> m_kEntityPasswordBits;
	FvMD5::Digest m_kMD5Digest;
	FvMD5::Digest m_kPersistentPropertiesMD5Digest;
	FvEntityTypeID m_uiDefaultTypeID;
	std::vector< FvString >	m_kNameProperties;
	FvString m_kDefaultNameProperty;

public:
	FvEntityDefs() :
		m_kEntityDescriptionMap(), m_kEntityPasswordBits(), m_kMD5Digest(),
		m_kPersistentPropertiesMD5Digest(),
		m_uiDefaultTypeID( FV_INVALID_ENTITY_TYPE_ID ), m_kNameProperties(),
		m_kDefaultNameProperty()
	{}

	bool Init(FvXMLSectionPtr pFatherEntitiesSection,
		FvXMLSectionPtr pEntitiesSection,
		const FvString& defaultTypeName,
		const FvString& defaultNameProperty );

	bool XMLToMD5();

	const FvMD5::Digest &GetDigest() const 		{	return m_kMD5Digest;	}
	const FvMD5::Digest &GetPersistentPropertiesDigest() const
	{	return m_kPersistentPropertiesMD5Digest;	}
	const FvString &GetNameProperty( FvEntityTypeID index ) const
	{	return m_kNameProperties[ index ];	}
	const FvString &GetDefaultNameProperty() const
	{ 	return m_kDefaultNameProperty;	}
	const FvEntityTypeID GetDefaultType() const	{	return m_uiDefaultTypeID; 	}
	const FvString &GetDefaultTypeName() const;

	bool EntityTypeHasPassword( FvEntityTypeID typeID ) const
	{
		return m_kEntityPasswordBits[typeID];
	}
	bool IsValidEntityType( FvEntityTypeID typeID ) const
	{
		return (typeID < m_kEntityDescriptionMap.Size()) &&
				!m_kEntityDescriptionMap.EntityDescription( typeID ).
					IsClientOnlyType();
	}
	FvEntityTypeID GetEntityType( const FvString& typeName ) const
	{
		FvEntityTypeID typeID = INVALID_TYPEID;
		m_kEntityDescriptionMap.NameToIndex( typeName, typeID );
		return typeID;
	}
	size_t GetNumEntityTypes() const
	{
		return m_kEntityDescriptionMap.Size();
	}
	const FvEntityDescription &GetEntityDescription( FvEntityTypeID typeID ) const
	{
		return m_kEntityDescriptionMap.EntityDescription( typeID );
	}
	FvString GetPropertyType( FvEntityTypeID typeID,
		const FvString& propName ) const;

	void DebugDump( int detailLevel ) const;
};

#endif // __FvDBEntityDefs_H__
