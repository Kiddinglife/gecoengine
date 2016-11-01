//{future header message}
#ifndef __FvZoneView_H__
#define __FvZoneView_H__

#include <vector>
#include <string>

#include "FvWaypointView.h"
#include "FvNavigatorDefine.h"

class FV_NAVIGATOR_API FvZoneView : public FvIWaypointView
{
public:
	FvZoneView();
	~FvZoneView();

	void Clear();
	bool Load( FvZone *pkZone );

	FvUInt32 GridX() const;
	FvUInt32 GridY() const;
	FvUInt8 GridMask(int iX, int iY) const;
	FvVector3 GridMin() const;
	FvVector3 GridMax() const;
	float GridResolution() const;

	int GetPolygonCount() const;
	int GetBSPNodeCount() const;
	float GetGirth( int iSet ) const;
	int GetVertexCount( int iPolygon ) const;
	float GetMinHeight( int iPolygon ) const;
	float GetMaxHeight( int iPolygon ) const;
	int GetSet( int iPolygon ) const;
	void GetVertex( int iPolygon, int iVertex, 
		FvVector2 &kVec, int &iAdjacency, bool &bAdjToAnotherZone ) const;
	void SetAdjacency( int iPolygon, int iVertex, int iNewAdj );

	int FindWaypoint( const FvVector3&, float fGirth ) const;

private:
	FvVector3 m_kGridMin;
	FvVector3 m_kGridMax;

	struct VertexDef
	{
		FvVector2 m_kPosition;
		int m_iAdjacentID;
		bool m_bAdjToAnotherZone;
	};

	struct PolygonDef
	{
		float m_fMinHeight;
		float m_fMaxHeight;
		std::vector<VertexDef> m_kVertices;
		int m_iSet;
	};

	struct SetDef
	{
		SetDef() : m_fGirth( 0.f ) { }
		float m_fGirth;
	};

	std::vector<PolygonDef> m_kPolygons;
	std::vector<SetDef> m_kSets;
	int m_iNextID;
	bool m_bMinMaxValid;

	void ParseNavPoly( FvXMLSectionPtr spSect, int iSet, FvZone *pkZone = NULL );
};

#endif // __FvZoneView_H__
