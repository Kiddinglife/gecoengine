#ifndef __FvWaypoint_H__
#define __FvWaypoint_H__

#include <string>
#include <vector>
#include <FvVector2.h>
#include <FvXMLSection.h>
#include <FvBinaryStream.h>

#include "FvNavigatorDefine.h"

const int FV_ZONE_ADJACENT_CONSTANT = 30000;

typedef unsigned long FvWaypointID;

typedef FvString FvZoneID;

class FvWaypointSet;

class FV_NAVIGATOR_API FvWaypoint
{
public:
	FvWaypoint();

	void ReadFromSection(FvXMLSectionPtr, const FvZoneID &kZoneID);
	void WriteToSection(FvXMLSectionPtr, const FvZoneID &kZoneID);

	void ReadFromStream(FvBinaryIStream &kStream);
	void WriteToStream(FvBinaryOStream &kStream);

	int VertexCount() const;

	const FvVector2 &VertexPosition(int iIndex) const;
	FvUInt32 EdgeFlags(int iIndex) const;
	FvWaypointID AdjacentID(int iIndex) const;
	const FvZoneID &AdjacentZoneID(int iIndex) const;

	FvWaypoint *AdjacentWaypoint(int iIndex) const;
	void AdjacentWaypoint(int Iindex, FvWaypoint *pkWaypoint);
	FvWaypointSet *AdjacentWaypointSet(int index) const;
	void AdjacentWaypointSet(int iIndex, FvWaypointSet *pkWSet);

	bool ContainsPoint(float fX, float fY, float fZ) const;
	bool IsAdjacentToZone(const FvZoneID &kZoneID);
	bool IsAdjacentToSet(const FvWaypointSet *pkWSet);

	FvWaypointID GetID() const;
	const FvVector3 &Centre() const;
	float Height() const;

	void Transform( const FvMatrix &kMatrix, bool bHeightToo );

	bool FindClosestPoint(const FvVector3 &kDST3,
							FvVector3 &kIntersection);
private:

	void CalculateCentre();

	struct Vertex
	{
		FvVector2 m_kPosition;
		FvWaypointID m_uiAdjacentID;
		FvZoneID m_kAdjacentZoneID;
		FvWaypoint *m_pkAdjacentWaypoint;
		FvWaypointSet *m_pkAdjacentWaypointSet;
		float m_fDistanceToAdjacent;
	};

	FvWaypointID m_uiID;
	float m_fHeight;
	std::vector<Vertex> m_kVertexVector;	
	FvVector3 m_kCentre;
};

#endif
