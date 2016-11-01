//{future header message}
#ifndef __FvZoneParticles_H__
#define __FvZoneParticles_H__

#include "FvZoneDefine.h"
#include "FvZoneItem.h"
#include "FvZoneSerializerXMLImpl.h"

#include <FvXMLSection.h>
#include <ParticleUniverseSystemManager.h>

using namespace Ogre;

class FV_ZONE_API FvZoneParticles : public FvZoneItem
{
	FV_DECLARE_ZONE_ITEM_XML( FvZoneParticles )
public:
	FvZoneParticles();
	~FvZoneParticles();

	bool Load( FvXMLSectionPtr spSection );

	virtual void Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor );

	virtual void Toss( FvZone *pkZone );

	bool GetReflectionVis() const;
	void SetReflectionVis( bool bReflVis );

	virtual const FvMatrix &GetMatrix() const;
	virtual void SetMatrix( const FvMatrix &kMatrix );

protected:

	FvString m_kResourceID;
	FvMatrix m_kLocalTransform;
	FvMatrix m_kWorldTransform;
	FvString m_kParticleSystemName;
	static FvUInt32 ms_uiLightIdentify;
	SceneNode *m_pkParticleNode;
	ParticleUniverse::ParticleSystem *m_pkParticleSystem;

private:

	virtual bool AddZBounds( FvBoundingBox &kBound ) const;

	bool m_bReflectionVisible;
};

#include "FvZoneParticles.inl"

#endif // __FvZoneParticles_H__