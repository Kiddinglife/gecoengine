//{future header message}
#ifndef __FvZoneTree_H__
#define __FvZoneTree_H__

#include <FvTree.h>

#include "FvZoneItem.h"
#include "FvZoneSerializerXMLImpl.h"

class FV_ZONE_API FvZoneTree : public FvZoneItem
{
	FV_DECLARE_ZONE_ITEM_XML( FvZoneTree )
public:
	FvZoneTree();
	~FvZoneTree();

	bool Load( FvXMLSectionPtr spSection, FvZone *pkZone = NULL );

	virtual void Toss( FvZone *pkZone );

	virtual void Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor );
	virtual void Tick( float fTime );
	virtual void Lend( FvZone *pkLender );

	virtual const char *Label() const;

	bool GetReflectionVis() const { return m_bReflectionVisible; }
	bool SetReflectionVis( const bool& reflVis ) { m_bReflectionVisible=reflVis; return true; }
	FvBoundingBox LocalBB() const;

protected:

	FvTreePtr m_spTree;

	float m_fAnimRateMultiplier;

	FvMatrix m_kTransform;

	FvString m_kLabel;

	bool m_bCastShadows;
	bool m_bReflectionVisible;
	bool m_bIsRender;

#if FV_UMBRA_ENABLE
	bool m_bUumbraOccluder;
	FvString m_kUmbraModelName;
#endif // FV_UMBRA_ENABLE

	virtual bool AddZBounds( FvBoundingBox &kBB ) const;

};

#endif // __FvZoneTree_H__
