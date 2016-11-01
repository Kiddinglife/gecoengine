#ifndef __FvAdjacentZoneSet_H__
#define __FvAdjacentZoneSet_H__

#include "FvWaypoint.h"
#include "FvNavigatorDefine.h"

#include <FvPlane.h>
#include <FvVector3.h>
#include <vector>

class FV_NAVIGATOR_API FvAdjacentZoneSet
{
public:
	FvAdjacentZoneSet();

	//bool read(DataSectionPtr pChunkDir, const FvZoneID &kZoneID);

	const FvZoneID &StartZone() const;  

	bool HasZone(const FvZoneID &kZoneID) const;
	void AddZone(const FvZoneID &kZoneID);
	void AddPlane(const FvPlane &kPlane);

	bool Test(const FvVector3 &kPosition, FvZoneID &kZoneID) const;

private:
	bool ReadZone(FvXMLSectionPtr spAdjZone);
	bool TestZone(const FvVector3 &kPosition, int iIndex) const;

	struct ZoneDef
	{
		FvZoneID m_kZoneID;
		std::vector<FvPlane> m_kPlanes;
	};

	std::vector<ZoneDef> m_kZones;
	FvZoneID m_kStartZone;
};

#endif // __FvAdjacentZoneSet_H__
