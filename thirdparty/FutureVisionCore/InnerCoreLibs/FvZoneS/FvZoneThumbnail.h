//{future header message}
#ifndef __FvZoneThumbnail_H__
#define __FvZoneThumbnail_H__

#include "FvZoneDefine.h"
#include "FvZone.h"

#include <OgreTexture.h>

class FV_ZONE_API FvZoneThumbnail : public FvZoneCache
{
public:
	FvZoneThumbnail( FvZone &kZone );
	~FvZoneThumbnail();

	bool Load( FvZoneSerializer & );

	static void Touch( FvZone &kZone );

	Ogre::TexturePtr &Thumbnail() { return m_spThumbnail; }

	static Instance<FvZoneThumbnail> ms_kInstance;

private:
	FvZone *m_pkZone;
	Ogre::TexturePtr m_spThumbnail;
};

#endif // __FvZoneThumbnail_H__