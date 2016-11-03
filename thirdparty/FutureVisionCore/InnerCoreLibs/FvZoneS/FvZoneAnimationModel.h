#ifndef __FVZONEANIMATIONMODEL_H__
#define __FVZONEANIMATIONMODEL_H__

#include <FvAnimationModel.h>

#include "FvZoneItem.h"
#include "FvZoneSerializerXMLImpl.h"

class FV_ZONE_API FvZoneAnimationModel : public FvZoneItem
{
	FV_DECLARE_ZONE_ITEM_XML( FvZoneAnimationModel )
	FV_DECLARE_ZONE_ITEM_XML_ALIAS( FvZoneAnimationModel, shell )
public:
	FvZoneAnimationModel();
	~FvZoneAnimationModel();

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

	

	FvAnimationModelPtr m_spAnimModel;

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

	bool IsShellModel( const FvXMLSectionPtr spSection ) const;
};

#include "FvZoneAnimationModel.inl"
#endif //__FVZONEANIMATIONMODEL_H__