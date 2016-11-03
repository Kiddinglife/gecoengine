#include "FvZoneLight.h"
#include "FvCamera.h"

#include <FvDebug.h>

#include <OgreRoot.h>
#include <OgreSceneNode.h>
#include "FvZoneVisitor.h"

FvUInt32 FvZoneLight::ms_uiLightIdentify = 0;

int iZoneLightToken;

FvZoneLight::FvZoneLight( FvZoneItem::WantFlags eWantFlags ) : 
FvZoneItem( eWantFlags ),
m_pkLightNode(NULL),
m_pkLight(NULL),
m_bIsRender(false)
{ 
	ms_uiLightIdentify++;
	char pcIdentify[9];
	sprintf_s(pcIdentify,9,"%I32x",FvInt32(ms_uiLightIdentify));
	m_kLightNodeName = "FvLightNode_";
	m_kLightNodeName += pcIdentify;
	m_kLightName = "FvLight_";
	m_kLightName += pcIdentify;

	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);
	m_pkLightNode = pkSceneManager->createSceneNode(m_kLightNodeName);
}

FvZoneLight::~FvZoneLight()
{
	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);
	if(m_pkLightNode)
		pkSceneManager->destroySceneNode(m_pkLightNode);
}

void FvZoneLight::Toss( FvZone *pkZone )
{
	//FV_GUARD;
	if (m_pkZone != NULL)
	{
		FvZoneLightCache &kLightCache = FvZoneLightCache::ms_kInstance( *m_pkZone );
		this->DelFromContainer( kLightCache.OwnLights() );
		kLightCache.DirtySeep();
	}

	this->FvZoneItem::Toss( pkZone );

	if (m_pkZone != NULL)
	{
		FvZoneLightCache &kLightCache = FvZoneLightCache::ms_kInstance( *m_pkZone );

		this->UpdateLight( m_pkZone->Transform() );

		this->AddToCache( kLightCache );

		kLightCache.DirtySeep();
	}
}

void FvZoneLight::CreateLight()
{
	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);

	if(m_pkLight == NULL)
		m_pkLight = pkSceneManager->createLight(m_kLightName);

	FV_ASSERT(m_pkLightNode)
	
	if(m_pkLight)
		m_pkLightNode->attachObject(m_pkLight);
}

void FvZoneLight::AddToCache( FvZoneLightCache &kCache ) const
{
	this->AddToContainer(kCache.OwnLights());
}

void FvZoneLight::AddToContainer( FvLightContainer &kContainer ) const
{
	kContainer.insert(m_pkLight);
}

void FvZoneLight::DelFromContainer( FvLightContainer &kContainer ) const
{
	kContainer.erase(m_pkLight);
}

void FvZoneLight::UpdateLight( const FvMatrix &kWorld ) const
{
	if(m_pkLightNode)
	{
		Ogre::Vector3 kPosition,kScale;
		FvQuaternion kRotation;
		FvMatrixDecomposeWorld4f((float*)&kRotation,(float*)&kScale,(float*)&kPosition,
			(float*)&kWorld);
		m_pkLightNode->setPosition(kPosition);
		m_pkLightNode->setScale(kScale);
		m_pkLightNode->setOrientation(Ogre::Quaternion(kRotation.w,kRotation.x,kRotation.y,kRotation.z));
	}
}

FV_IMPLEMENT_ZONE_ITEM_XML( FvZoneDirectionalLight, directionalLight, 0 )

FvZoneDirectionalLight::FvZoneDirectionalLight()
{			

}

FvZoneDirectionalLight::~FvZoneDirectionalLight()
{

}

bool FvZoneDirectionalLight::Load( FvXMLSectionPtr spSection )
{
	//FV_GUARD;
	FvZoneLight::CreateLight();
	if(m_pkLight == NULL)
		return false;
	
	m_pkLight->setType(Ogre::Light::LT_DIRECTIONAL);
	FvVector3 kColour = spSection->ReadVector3( "colour" ) / 255.f;
	float fMultiplier = spSection->ReadFloat( "multiplier" );
	m_pkLight->setDiffuseColour( 
		Ogre::ColourValue( kColour[0], kColour[1], kColour[2], Ogre::Light::LT_DIRECTIONAL/2.0f
		) * fMultiplier );
	FvVector3 kDirection( spSection->ReadVector3( "direction" ) );
	m_pkLight->setDirection( *(Ogre::Vector3*)&kDirection );
	m_bDynamicLight = spSection->ReadBool( "dynamic", true );
	m_bSpecularLight = spSection->ReadBool( "specular", true );

	return true;
}

FV_IMPLEMENT_ZONE_ITEM_XML( FvZoneOmniLight, omniLight, 0 )

FvZoneOmniLight::FvZoneOmniLight( FvZoneItem::WantFlags eWantFlags ) :
FvZoneLight( eWantFlags )
{
}

FvZoneOmniLight::~FvZoneOmniLight()
{

}

bool FvZoneOmniLight::Load( FvXMLSectionPtr spSection )
{
	//FV_GUARD;	
	FvZoneLight::CreateLight();
	if(m_pkLight == NULL)
		return false;

	m_pkLight->setType(Ogre::Light::LT_POINT);
	FvVector3 kColour = spSection->ReadVector3( "colour" ) / 255.f;
	float fMultiplier = spSection->ReadFloat( "multiplier" );
	m_pkLight->setDiffuseColour( 
		Ogre::ColourValue( kColour[0], kColour[1], kColour[2], Ogre::Light::LT_POINT/2.0f 
		) * fMultiplier );

	*(FvVector3*)&(m_kParams.m_kDiffuse) = kColour;
	m_kParams.m_kDiffuse.w = fMultiplier;
	m_kParams.m_kSpecular = m_kParams.m_kDiffuse;

	*(FvVector3*)&(m_kParams.m_kPositionWS) = spSection->ReadVector3( "position" );
	m_pkLight->setPosition( *(Ogre::Vector3*)&(spSection->ReadVector3( "position" )) );

	
	float fInnerRadius = spSection->ReadFloat( "innerRadius" );
	float fOuterRadius = spSection->ReadFloat( "outerRadius" );
	float fLinear,fQuadratic;
	const float fZero = 0.1f;
	if(!FvAlmostZero(fInnerRadius))
	{
		float fDown = (fOuterRadius *(fOuterRadius - fInnerRadius) * fZero);
		fQuadratic = ( 1 - fZero )/fDown;
		fLinear = (fInnerRadius *(fZero - 1))/fDown;
	}
	else
	{
		fQuadratic = 0.0f;
		fLinear = (1 - fZero)/(fOuterRadius * fZero);
	}
	m_pkLight->setAttenuation( fOuterRadius, 1.0f, fLinear, fQuadratic );
	m_kParams.m_kAttenuation.x = fOuterRadius;
	m_kParams.m_kAttenuation.y = 1.0f;
	m_kParams.m_kAttenuation.z = fLinear;
	m_kParams.m_kAttenuation.w = fQuadratic;

	m_bDynamicLight = spSection->ReadBool( "dynamic", true );
	m_bSpecularLight = spSection->ReadBool( "specular", true );

	return true;
}
//----------------------------------------------------------------------------
void FvZoneOmniLight::Visit(FvCamera *pkCamera, FvZoneVisitor *pkVisitor)
{
	if(pkVisitor->EnableVisitLight() && (FvZone::ms_iRenderZone == 0))
	{
		if((VisitMark() == FvZone::ms_uiNextMark) && m_bIsRender)
		{
			return;
		}
		else
		{
			m_bIsRender = false;
		}

		Ogre::Sphere sphere(m_pkLight->getDerivedPosition(), m_pkLight->getAttenuationRange());
		bool bIsVisible = pkCamera->isVisible(sphere);

		if(bIsVisible)
		{
			*(Ogre::Vector3*)&(m_kParams.m_kPositionWS) = m_pkLight->getDerivedPosition();
			float fSphereCullRadius = sphere.getRadius() * 1.2 + pkCamera->getNearClipDistance();

			if((pkCamera->getRealPosition() - sphere.getCenter()).squaredLength() < (fSphereCullRadius * fSphereCullRadius))
			{
				pkVisitor->VisitPointLight(&m_kParams, true, m_bSpecularLight);
			}
			else
			{
				pkVisitor->VisitPointLight(&m_kParams, false, m_bSpecularLight);
			}

			m_bIsRender = true;
		}
	}
}
//----------------------------------------------------------------------------
FV_IMPLEMENT_ZONE_ITEM_XML( FvZoneSpotLight, spotLight, 0 )

FvZoneSpotLight::FvZoneSpotLight()
{

}

FvZoneSpotLight::~FvZoneSpotLight()
{

}

bool FvZoneSpotLight::Load( FvXMLSectionPtr spSection )
{
	//FV_GUARD;	
	FvZoneLight::CreateLight();
	if(m_pkLight == NULL)
		return false;

	m_pkLight->setType(Ogre::Light::LT_SPOTLIGHT);
	FvVector3 kColour = spSection->ReadVector3( "colour" ) / 255.f;
	float fMultiplier = spSection->ReadFloat( "multiplier" );
	m_pkLight->setDiffuseColour( Ogre::ColourValue( kColour[0], kColour[1], kColour[2], 
		1 ) * fMultiplier );

	m_pkLight->setPosition( Ogre::Vector3( spSection->ReadVector3( "position" ) ) );
	m_pkLight->setDirection( Ogre::Vector3( spSection->ReadVector3( "direction" ) ) );
	m_pkLight->setSpotlightInnerAngle( Ogre::Radian(spSection->ReadFloat( "innerRadius" )) );
	m_pkLight->setSpotlightOuterAngle( Ogre::Radian(spSection->ReadFloat( "outerRadius" )) );
	//m_pkLight->cosConeAngle( spSection->ReadFloat( "cosConeAngle" ) );

	m_bDynamicLight = spSection->ReadBool( "dynamic", true );
	m_bSpecularLight = spSection->ReadBool( "specular", true );

	return true;
}

FV_IMPLEMENT_ZONE_ITEM_XML( FvZoneAmbientLight, ambientLight, 0 )

FvZoneAmbientLight::FvZoneAmbientLight():
m_kColour(0.0f,0.0f,0.0f,1.0f)
{
}

FvZoneAmbientLight::~FvZoneAmbientLight()
{
}

bool FvZoneAmbientLight::Load( FvXMLSectionPtr spSection )
{
	//FV_GUARD;	
	FvVector3 kColour = spSection->ReadVector3( "colour" ) / 255.f;
	float fMultiplier = spSection->ReadFloat( "multiplier", 1.f );
	//kColour = kColour * fMultiplier;
	m_kColour = Ogre::ColourValue( kColour[0], kColour[1], kColour[2], fMultiplier );

	return true;
}

void FvZoneAmbientLight::AddToCache( FvZoneLightCache &kCache ) const
{
	kCache.AmbientColor() = m_kColour;
}

void FvZoneAmbientLight::Toss( FvZone *pkZone )
{
	FvZoneLight::Toss(pkZone);

	if(m_pkZone && (!m_pkZone->IsOutsideZone()))
	{
		m_pkZone->m_bUseOutsideLight = false;
	}	
}


struct FvLightsSorting
{
	bool operator()(const Ogre::Light *pkLight1, const Ogre::Light *pkLight2) const
	{
		if (pkLight1 == pkLight2)
			return false;

		if (pkLight1->getCastShadows() != pkLight2->getCastShadows())
			return pkLight1->getCastShadows();

		if(pkLight1->getType() != pkLight2->getType())
		{
			int iLight1,iLight2;
			if(pkLight1->getType() == Ogre::Light::LT_POINT) iLight1 = 0;
			if(pkLight1->getType() == Ogre::Light::LT_SPOTLIGHT) iLight1 = 1;
			if(pkLight1->getType() == Ogre::Light::LT_DIRECTIONAL) iLight1 = 2;
			if(pkLight2->getType() == Ogre::Light::LT_POINT) iLight2 = 0;
			if(pkLight2->getType() == Ogre::Light::LT_SPOTLIGHT) iLight2 = 1;
			if(pkLight2->getType() == Ogre::Light::LT_DIRECTIONAL) iLight2 = 2;
			return iLight2 < iLight1;
		}

		return false;
	}
};

FvZoneLightCache::FvZoneLightCache( FvZone &kZone ) :
m_kZone( kZone ),
m_kAmbientColor(0.0,0.0,0.0,1.0),
m_bLightContainerDirty( true ),
m_bHeavenSeen( false )
{
	//FV_GUARD;
}

FvZoneLightCache::~FvZoneLightCache()
{

}

void FvZoneLightCache::Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor )
{
	//FV_GUARD;

	if (m_bLightContainerDirty)
	{
		this->CollectLights();
		m_bLightContainerDirty = false;
	}
}

void FvZoneLightCache::Bind( bool bLooseNotBind )
{
	//FV_GUARD;
	DirtySeep();
}

void FvZoneLightCache::Touch( FvZone &kZone )
{
	//FV_GUARD;
	FvZoneLightCache::ms_kInstance( kZone );
}

bool FvZoneLightCache::IsInAllLights(Ogre::Light *pkLight)
{
	return m_kAllLights.find(pkLight) != m_kAllLights.end();
}

void FvZoneLightCache::DirtySeep()
{
	//FV_GUARD;
	this->Dirty();

	std::set<FvZone*> kNeighbours;
	this->CollectNeighbouringZones( kNeighbours );

	for (std::set<FvZone*>::iterator kPIt = kNeighbours.begin(); kPIt != kNeighbours.end(); kPIt++)
	{
		FvZoneLightCache::ms_kInstance(**kPIt).Dirty();
	}	
}

void FvZoneLightCache::Dirty()
{
	m_bLightContainerDirty = true;
}

void FvZoneLightCache::CollectLights()
{
	FvLightContainer::iterator kIt = m_kOwnLights.begin();
	for(; kIt != m_kOwnLights.end(); ++kIt)
		m_kAllLights.insert(*kIt);

	m_bHeavenSeen = m_kZone.CanSeeHeaven();
	if (m_bHeavenSeen)
	{
		// set sky outside ambient color,sun light.
	}

	std::set<FvZone*> kNeighbours;
	this->CollectNeighbouringZones( kNeighbours );

	for (std::set<FvZone*>::iterator kNIt = kNeighbours.begin(); kNIt != kNeighbours.end(); kNIt++)
	{
		FvZoneLightCache &kCache = FvZoneLightCache::ms_kInstance(**kNIt);
		kCache.CollectLightsForZone(m_kAllLights,&m_kZone);
	}
}

void FvZoneLightCache::CollectLightsForZone( FvLightContainer &kContainer, FvZone *pkZone )
{
	for(FvLightContainer::iterator kIt = m_kOwnLights.begin();
		kIt != m_kOwnLights.end(); ++kIt)
	{
		if(pkZone->IsOutsideZone() && m_kZone.IsOutsideZone()/* &&
			!this->IsInLightsContainer(kContainer,*kIt)*/)
			kContainer.insert(*kIt);
	}
}

void FvZoneLightCache::CollectNeighbouringZones( std::set<FvZone*> &kRet )
{	
	//FV_GUARD;

	for (FvZone::piterator kPIt = m_kZone.PBegin(); kPIt!= m_kZone.PEnd(); kPIt++)
	{
		if (kPIt->HasZone() && kPIt->m_pkZone->Online())
		{
			FvZone *pkZone = kPIt->m_pkZone;
			kRet.insert( pkZone );

			for (FvZone::piterator kPIt2 = pkZone->PBegin(); kPIt2 != pkZone->PEnd(); kPIt2++)
			{
				if (kPIt2->HasZone() && kPIt2->m_pkZone->Online())
				{
					kRet.insert( kPIt2->m_pkZone );
				}
			}
		}
	}
}

FvZoneCache::Instance<FvZoneLightCache> FvZoneLightCache::ms_kInstance;
