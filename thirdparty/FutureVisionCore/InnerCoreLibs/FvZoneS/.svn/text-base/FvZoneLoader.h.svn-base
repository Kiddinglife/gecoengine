//{future header message}
#ifndef __FvZoneLoader_H__
#define __FvZoneLoader_H__

#include <FvBaseTypes.h>
#include "FvZoneDefine.h"

class FvZone;
class FvZoneSpace;
class FvVector3;

class FV_ZONE_API FvZoneLoader
{
public:
	static void Load( FvZone *pkZone, int iPriority = 0 );
	static void LoadNow( FvZone *pkZone );
	static void FindSeed( FvZoneSpace *pkSpace, const FvVector3 &kWhere,
		FvZone *&rpkZone );
};

#endif // __FvZoneLoader_H__