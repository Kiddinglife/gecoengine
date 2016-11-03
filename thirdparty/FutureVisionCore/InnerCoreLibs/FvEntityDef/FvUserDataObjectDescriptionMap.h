//{future header message}
#ifndef __FvUserDataObjectDescriptionMap_H__
#define __FvUserDataObjectDescriptionMap_H__

#include <FvXMLSection.h>

#include "FvEntityDefDefines.h"
#include "FvUserDataObjectDescription.h"

#include <map>

class FvMD5;


class FV_ENTITYDEF_API FvUserDataObjectDescriptionMap
{
public:
	FvUserDataObjectDescriptionMap();
	virtual ~FvUserDataObjectDescriptionMap();

	bool 	Parse( FvXMLSectionPtr pSection );
	FvInt32	Size() const;
	const FvUserDataObjectDescription& UdoDescription( FvString UserDataObjectName ) const;
	void Clear();
	bool IsUserDataObject( const FvString& name ) const;
	typedef std::map<FvString, FvUserDataObjectDescription> DescriptionMap;
	DescriptionMap::const_iterator Begin() const { return m_kMap.begin(); }
	DescriptionMap::const_iterator End() const{ return m_kMap.end(); }
private:
	DescriptionMap 		m_kMap;
};

#endif // __FvUserDataObjectDescriptionMap_H__
