//{future header message}
#ifndef __FvZoneVisitor_H__
#define __FvZoneVisitor_H__

#include <FvBaseTypes.h>

#include <vector>

#include "FvZoneBoundary.h"
#ifndef FV_SERVER
#include "FvZoneLight.h"
#endif // FV_SERVER

class FvZone;
class FvZoneItem;

class FvZoneVisitor
{
public:
	enum VisitFlag
	{
		VISIT_FLAG_NONE						= 0x00,
		VISIT_FLAG_SHADOW_CASTER			= 0x01,
		VISIT_FLAG_WATER_REFLECTION			= 0x02,
		VISIT_FLAG_PICK						= 0x04
	};

	FvZoneVisitor(){}
	virtual ~FvZoneVisitor(){}

	virtual bool HasFlag(VisitFlag eFlag) = 0;

	virtual bool Visit(FvZone *pkZone) = 0;
	virtual bool Visit(FvZoneItem *pkItem) = 0;
	virtual bool Visit(FvZoneBoundary::Portal *pkPortal) = 0;

	virtual bool EnableVisitLight() = 0;

	virtual bool IsOnlyShadowCasters() = 0;

#ifndef FV_SERVER
	virtual bool VisitPointLight(FvZoneOmniLight::Params* pkPointLight, bool bGlobal, bool bSpecular) = 0;
	virtual Ogre::RenderQueue *GetRenderQueue() = 0;
	virtual Ogre::LightList *GetLightList() = 0;
	virtual Ogre::VisibleObjectsBoundsInfo *GetVisibleObjectsBoundsInfo() = 0;
#endif // !FV_SERVER
};

#endif // __FvZoneVisitor_H__
