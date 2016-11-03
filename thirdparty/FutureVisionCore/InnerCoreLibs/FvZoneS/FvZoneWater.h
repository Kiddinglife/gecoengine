//{future header message}
#ifndef __FvZoneWater_H__
#define __FvZoneWater_H__

#include "FvWater.h"
#include "FvZoneVLO.h"

#if FV_UMBRA_ENABLE
	class FvZoneMirror;
	typedef FvSmartPointer<FvZoneMirror> FvZoneMirrorPtr;
#endif

class FvZoneWater : public FvVeryLargeObject
{
public:
	FvZoneWater( );
	FvZoneWater( FvString kUID );
	~FvZoneWater();

	bool Load( FvXMLSectionPtr spSection, FvZone *pkZone );

	virtual void Visit( FvZoneSpace *pkSpace, FvCamera *pkCamera, FvZoneVisitor *pkVisitor );
#if FV_UMBRA_ENABLE
	virtual void Lend( FvZone *pkZone );
	virtual void Unlend( FvZone *pkZone );
#endif //FV_UMBRA_ENABLE
	virtual void Sway( const FvVector3 &kSrc, const FvVector3 &kDest, const float fDiameter );
	static void SimpleDraw( bool bState );

#ifdef EDITOR_ENABLED
	virtual void Dirty();
#endif //EDITOR_ENABLED

	virtual FvBoundingBox ZoneBB( FvZone *pkZone );

	virtual void SyncInit(FvZoneVLO *pkVLO);
protected:
	FvWater *m_pkWater;
	FvZone *m_pkZone;
#if FV_UMBRA_ENABLE
	FvZoneMirrorPtr	m_spMirrorA;
	FvZoneMirrorPtr	m_spMirrorB;
#endif // FV_UMBRA_ENABLE

	FvWater::WaterState m_kConfig;
private:

	static bool Create( FvZone *pkZone, FvXMLSectionPtr spSection, FvString kUID );
	static FvVLOFactory	ms_kFactory;
	static bool ms_bSimpleDraw;
	virtual bool AddZBounds( FvBoundingBox &kBB ) const;
};


#endif // __FvZoneWater_H__