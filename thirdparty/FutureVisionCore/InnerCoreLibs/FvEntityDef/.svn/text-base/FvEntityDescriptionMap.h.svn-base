//{future header message}
#ifndef __FvEntityDescriptionMap_H__
#define __FvEntityDescriptionMap_H__

#include "FvEntityDefDefines.h"
#include "FvEntityDescription.h"
#include <FvXMLSection.h>

#include <map>
#include <vector>

class FvMD5;

class FV_ENTITYDEF_API FvEntityDescriptionMap
{
public:
	FvEntityDescriptionMap();
	~FvEntityDescriptionMap();

	bool 	Parse( FvXMLSectionPtr pFatherSection, FvXMLSectionPtr pSection );
	bool	NameToIndex( const FvString& name, FvEntityTypeID& index ) const;
	FvInt32	Size() const;

	const FvEntityDescription&	EntityDescription( FvEntityTypeID index ) const;

	void AddToMD5( FvMD5 & md5 ) const;
	void AddPersistentPropertiesToMD5( FvMD5 & md5 ) const;

	void Clear();
	bool IsEntity( const FvString& name ) const;
	typedef std::map<FvString, FvUInt32> DescriptionMap;
	DescriptionMap::const_iterator Begin() const { return m_kMap.begin(); }
	DescriptionMap::const_iterator End() const{ return m_kMap.end(); }

	void Dump(FvXMLSectionPtr pSection);

private:
	bool CheckCount( const char * description,
		FvUInt32 (FvEntityDescription::*fn)() const,
		FvInt32 maxEfficient, FvInt32 maxAllowed ) const;

	typedef std::vector<FvEntityDescription> DescriptionVector;


	DescriptionVector 	m_kVector;
	DescriptionMap 		m_kMap;
};

#endif // __FvEntityDescriptionMap_H__
