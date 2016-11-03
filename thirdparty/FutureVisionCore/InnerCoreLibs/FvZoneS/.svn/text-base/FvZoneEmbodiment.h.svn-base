//{future header message}
#ifndef __FvZoneEmbodiment_H__
#define __FvZoneEmbodiment_H__

#include "FvZoneItem.h"
#include "FvZoneSpace.h"
#include "FvSmartPointer.h"

#include <OgreSceneNode.h>

class FvZoneEmbodiment;
typedef FvSmartPointer<FvZoneEmbodiment> FvZoneEmbodimentPtr;

using namespace Ogre;

class FV_ZONE_API FvZoneEmbodiment : public FvZoneItem
{
public:
	explicit FvZoneEmbodiment( WantFlags eWantFlags = WANTS_NOTHING );
	FvZoneEmbodiment( SceneNode *pkNode, WantFlags eWantFlags = WANTS_NOTHING );
	virtual ~FvZoneEmbodiment();

	virtual void Toss( FvZone *pkZone );

	virtual void EnterSpace( FvZoneSpacePtr spSpace, bool bTransient = false ) = 0;
	virtual void LeaveSpace( bool bTransient = false ) = 0;

	virtual void Move( float fTime ) = 0;

	virtual const FvVector3 &GetPosition() const = 0;
	virtual void BoundingBox( FvBoundingBox &kBound ) const = 0;

	virtual void VisitPick(Ogre::RenderQueue *pkQueue, const Ogre::Vector4& kColor){}

private:

	FvZoneEmbodiment( const FvZoneEmbodiment& );
	FvZoneEmbodiment& operator=( const FvZoneEmbodiment& );
};

class FV_ZONE_API FvZoneEmbodiments : public std::vector<FvZoneEmbodimentPtr> {};

class FV_ZONE_API FvZoneDynamicEmbodiment : public FvZoneEmbodiment
{
public:
	FvZoneDynamicEmbodiment( WantFlags eWantFlags = WANTS_NOTHING );
	virtual ~FvZoneDynamicEmbodiment();

	virtual void Nest( FvZoneSpace *pkSpace );

	virtual void EnterSpace( FvZoneSpacePtr spSpace, bool bTransient );
	virtual void LeaveSpace( bool bTransient );
	virtual void Move( float fTime );

	void Sync();

protected:

	FvZoneSpacePtr m_spSpace;
	FvVector3 m_kLastTranslation;
};

class FV_ZONE_API FvZoneStaticEmbodiment : public FvZoneEmbodiment
{
public:
	FvZoneStaticEmbodiment( WantFlags eWantFlags = WANTS_NOTHING );
	virtual ~FvZoneStaticEmbodiment();

	virtual void Nest( FvZoneSpace *pkSpace );

	virtual void EnterSpace( FvZoneSpacePtr spSpace, bool bTransient);
	virtual void LeaveSpace( bool bTransient );
	virtual void Move( float fTime ) { }

protected:

	FvZoneSpacePtr m_spSpace;
};

#endif // __FvZoneEmbodiment_H__