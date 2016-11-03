#include "FvZoneThumbnail.h"
#include "FvZoneSpace.h"

int iZoneThumbnailToken;

FvZoneThumbnail::FvZoneThumbnail( FvZone &kZone ) :
m_pkZone( &kZone )
{

}

FvZoneThumbnail::~FvZoneThumbnail()
{
	if(!m_spThumbnail.isNull())
	{
		Ogre::TextureManager::getSingleton().remove(m_spThumbnail->getHandle());
		m_spThumbnail.setNull();
	}
}

bool FvZoneThumbnail::Load( FvZoneSerializer & )
{
	try
	{
		m_spThumbnail = Ogre::TextureManager::getSingleton().load(
			m_pkZone->Mapping()->Path() + m_pkZone->Identifier() + ".dds",
			m_pkZone->Mapping()->Group(),Ogre::TEX_TYPE_2D,0);
	}catch(...){}

	return !m_spThumbnail.isNull();
}

void FvZoneThumbnail::Touch( FvZone &kZone )
{
	//FV_GUARD;
	FvZoneThumbnail::ms_kInstance( kZone );
}

FvZoneCache::Instance<FvZoneThumbnail> FvZoneThumbnail::ms_kInstance;