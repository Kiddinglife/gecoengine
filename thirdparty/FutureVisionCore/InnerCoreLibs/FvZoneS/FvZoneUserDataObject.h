#ifndef __FvZoneUserDataObject_H__
#define __FvZoneUserDataObject_H__

#include "FvZoneItem.h"
#include "FvZoneSerializerXMLImpl.h"
#include "FvZone.h"
#include <FvXMLSection.h>
#include <FvDataObj.h>


class FV_ZONE_API FvZoneUserDataObject : public FvZoneItem
{
	FV_DECLARE_ZONE_ITEM_XML( FvZoneUserDataObject )
public:
	FvZoneUserDataObject();
	virtual ~FvZoneUserDataObject();
	bool Load( FvXMLSectionPtr spSection, FvZone *pkZone, FvString *pkErrorString = NULL);
	virtual void Toss( FvZone *pkZone );
	void Bind();

protected:
	FvObjUDO m_kUDO;

private:
	FvUInt16				m_uiType;
	FvObjUDO::FvUDOInitData	m_kInitData;
};

typedef FvSmartPointer<FvZoneUserDataObject> FvZoneUserDataObjectPtr;



class FvZoneUserDataObjectCache : public FvZoneCache
{
public:
	FvZoneUserDataObjectCache( FvZone &kZone );
	~FvZoneUserDataObjectCache();

	virtual void Bind( bool bLooseNotBind );

	void Add( FvZoneUserDataObject *pkUserDataObject );
	void Del( FvZoneUserDataObject *pkUserDataObject );

	static Instance<FvZoneUserDataObjectCache> ms_kInstance;

private:
	typedef std::vector< FvZoneUserDataObject * > FvZoneUserDataObjects;
	FvZoneUserDataObjects m_kUserDataObjects;
};


#endif // __FvZoneUserDataObject_H__