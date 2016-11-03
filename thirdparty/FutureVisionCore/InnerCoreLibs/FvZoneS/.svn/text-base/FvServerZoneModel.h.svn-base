//{future header message}
#ifndef __FvServerZoneModel_H__
#define __FvServerZoneModel_H__

#include "FvZoneItem.h"
#include "FvZoneSerializerXMLImpl.h"

#include <FvModel.h>
#include <FvSmartPointer.h>

class FvModel;

class FvServerZoneModel : public FvZoneItem
{
	FV_DECLARE_ZONE_ITEM_XML( FvServerZoneModel )	
	FV_DECLARE_ZONE_ITEM_XML_ALIAS( FvServerZoneModel, tree )
	FV_DECLARE_ZONE_ITEM_XML_ALIAS( FvServerZoneModel, shell )

public:
	FvServerZoneModel();
	~FvServerZoneModel();
	FvBoundingBox LocalBB() const;

protected:

	virtual void Toss( FvZone *pkZone );
	bool Load( FvXMLSectionPtr spSection, FvZone *pkZone );

	virtual const char *Label() const;

protected:
	FvModelPtr m_spModel;
	FvString m_kLabel;
	FvMatrix m_kTransform;
};

#endif // __FvServerZoneModel_H__