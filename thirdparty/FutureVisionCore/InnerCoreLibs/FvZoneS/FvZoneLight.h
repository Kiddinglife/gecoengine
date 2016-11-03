//{future header message}
#ifndef __FvZoneLight_H__
#define __FvZoneLight_H__

#include "FvZone.h"
#include "FvZoneItem.h"
#include "FvZoneSerializerXMLImpl.h"

#include <OgreWireBoundingBox.h>

class FvZoneLightCache;
namespace Ogre
{
	class SceneNode;
}

typedef std::set<Ogre::Light*> FvLightContainer;

class FV_ZONE_API FvZoneLight : public FvZoneItem
{
public:
	explicit FvZoneLight( WantFlags eWantFlags = WANTS_NOTHING );
	virtual ~FvZoneLight();
	virtual void Toss( FvZone *pkZone );

protected:

	void CreateLight();

	virtual void AddToCache( FvZoneLightCache &kCache ) const;
	virtual void AddToContainer( FvLightContainer &kContainer ) const;
	virtual void DelFromContainer( FvLightContainer &kContainer ) const;

	virtual void UpdateLight( const FvMatrix &kWorld ) const;

	Ogre::SceneNode *m_pkLightNode;
	Ogre::Light *m_pkLight;
	FvString m_kLightName;
	FvString m_kLightNodeName;
	static FvUInt32 ms_uiLightIdentify;
	bool m_bDynamicLight;
	bool m_bSpecularLight;
	bool m_bIsRender;
};

class FvZoneDirectionalLight : public FvZoneLight
{
	FV_DECLARE_ZONE_ITEM_XML( FvZoneDirectionalLight )
public:
	FvZoneDirectionalLight();
	~FvZoneDirectionalLight();

	bool Load( FvXMLSectionPtr spSection );
};

class FvZoneOmniLight : public FvZoneLight
{
	FV_DECLARE_ZONE_ITEM_XML( FvZoneOmniLight )

public:
	FvZoneOmniLight( WantFlags eWantFlags = WANTS_VISIT );
	~FvZoneOmniLight();

	bool Load( FvXMLSectionPtr spSection );

	virtual void Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor);

	struct Params
	{
		FvVector4 m_kPositionWS;
		FvVector4 m_kPositionVS;
		FvVector4 m_kAttenuation;
		FvVector4 m_kDiffuse;
		FvVector4 m_kSpecular;
	};

protected:
	Params m_kParams;
	
};

class FvZoneSpotLight : public FvZoneLight
{
	FV_DECLARE_ZONE_ITEM_XML( FvZoneSpotLight )
public:
	FvZoneSpotLight();
	~FvZoneSpotLight();

	bool Load( FvXMLSectionPtr spSection );
};

class FvZoneAmbientLight : public FvZoneLight
{
	FV_DECLARE_ZONE_ITEM_XML( FvZoneAmbientLight )
public:
	FvZoneAmbientLight();
	~FvZoneAmbientLight();

	bool Load( FvXMLSectionPtr spSection );

	virtual void Toss( FvZone *pkZone );

protected:
	void AddToCache( FvZoneLightCache &kCache ) const;
	void AddToContainer( FvLightContainer &kContainer ) const{}
	void DelFromContainer( FvLightContainer &kContainer ) const{}

	Ogre::ColourValue m_kColour;
};

class FvZoneLightCache : public FvZoneCache
{
public:
	FvZoneLightCache( FvZone &kZone );
	~FvZoneLightCache();

	virtual void Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor );
	virtual void Bind( bool bLooseNotBind );

	static void Touch( FvZone &kZone );

	FvLightContainer &OwnLights() { return m_kOwnLights; }
	FvLightContainer &AllLights() { return m_kAllLights; }
	Ogre::ColourValue &AmbientColor() { return m_kAmbientColor; }

	bool IsInAllLights(Ogre::Light *pkLight);

	static Instance<FvZoneLightCache> ms_kInstance;

	void DirtySeep();

private:
	void Dirty();
	void CollectLights();
	void CollectLightsForZone( FvLightContainer &kContainer, FvZone *pkZone );
	void CollectNeighbouringZones( std::set<FvZone*> &kRet );

	FvZone &m_kZone;

	FvLightContainer m_kOwnLights;
	FvLightContainer m_kAllLights;	
	Ogre::ColourValue m_kAmbientColor;

	bool m_bLightContainerDirty;
	bool m_bHeavenSeen;
};

#endif // __FvZoneLight_H__