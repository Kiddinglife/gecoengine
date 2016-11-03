//{future header message}
#ifndef __FvWaypointView_H__
#define __FvWaypointView_H__

#include <FvVector2.h>
#include <FvVector3.h>
#include <FvVector4.h>

#include <FvZone.h>

class FvXMLSection;
typedef FvSmartPointer<FvXMLSection> FvXMLSectionPtr;

class FvIWaypointView
{
public:

	struct BSPNodeInfo
	{
		std::vector<FvVector2> m_kBoundary;
		float m_fMinHeight;
		float m_fMaxHeight;
		bool m_bInternal;
		bool m_bWaypoint;
	};

	virtual FvUInt32 GridX() const = 0;
	virtual FvUInt32 GridY() const = 0;
	virtual FvUInt8 GridMask(int iX, int iY) const = 0;
	virtual FvVector3 GridMin() const = 0;
	virtual FvVector3 GridMax() const = 0;
	virtual float GridResolution() const = 0;
	virtual int GridPollCount() const	{ return 0; }
	virtual bool GridPoll( int iCursor, FvVector3 &kPT, FvVector4 &kAHGTS ) const { return false; }

	virtual int GetBSPNodeCount() const	{ return 0; }
	virtual int GetBSPNode( int iCursor, int iDepth, BSPNodeInfo &kBNInfo ) const { return -1; }

	virtual int GetPolygonCount() const = 0;
	virtual float GetGirth(int iSet) const = 0;
	virtual int GetVertexCount(int iPolygon) const = 0;
	virtual float GetMinHeight(int iPolygon) const = 0;
	virtual float GetMaxHeight(int iPolygon) const = 0;
	virtual int GetSet(int iPolygon) const = 0;
	virtual void GetVertex(int iPolygon, int iVertex, 
		FvVector2 &kVec, int &iAdjacency, bool &bAdjToAnotherZone) const = 0;
	virtual void SetAdjacency( int iPolygon, int iVertex, int iNewAdj ) { }

	virtual int FindWaypoint( const FvVector3&, float fGirth ) const = 0;

	void WriteToSection( FvXMLSectionPtr spSection, FvZone *pkZone,
						float fDefaultGirth, bool bRemoveOld = true ) const;
	void SaveOut( FvZone *pkZone, float fDefaultGirth,
						bool bRemoveAllOld = true );
	//BinaryPtr AsBinary( const FvMatrix &kTransformToLocal,
	//					float fDefaultGirth );

	bool EquivGirth( int iPolygon, float fGirth ) const;

	class VertexRef
	{
	public:
		VertexRef( FvIWaypointView &view, int pindex, int vindex ) :
			m_pkView( &view ), m_iPIndex( pindex ), m_iVIndex( vindex )
		{
			this->GetVertex();
		}

		const FvVector2 &Pos() const { return m_kPos; }
		const int AdjNavPoly() const { return m_iAdjNavPoly; }
		const bool AdjToAnotherZone() const	{ return m_bAdjToAnotherZone; }

		void AdjNavPoly( int newAdjNavPoly )
		{
			m_pkView->SetAdjacency( m_iPIndex, m_iVIndex, newAdjNavPoly );
			this->GetVertex();
		}

	private:
		void GetVertex()
		{
			m_pkView->GetVertex( m_iPIndex, m_iVIndex, m_kPos, m_iAdjNavPoly, m_bAdjToAnotherZone );
			m_kPos *= m_pkView->GridResolution();
			FvVector3 gm( m_pkView->GridMin() );
			m_kPos.x += gm.x;
			m_kPos.y += gm.y;
		}

		FvIWaypointView *m_pkView;
		int	m_iPIndex;
		int	m_iVIndex;

		FvVector2 m_kPos;
		int m_iAdjNavPoly;
		bool m_bAdjToAnotherZone;
	};

	class PolygonRef
	{
	public:
		PolygonRef( FvIWaypointView &kView, int iIndex ) :
			m_pkView( &kView ), m_iIndex( iIndex ) { }
		
		float MinHeight() const
			{ return m_pkView->GetMinHeight( m_iIndex ); }
		float MaxHeight() const
			{ return m_pkView->GetMaxHeight( m_iIndex ); }
		int NavPolySetIndex() const
			{ return m_pkView->GetSet( m_iIndex ); }

		unsigned int Size() const
			{ return m_pkView->GetVertexCount( m_iIndex ); }
		VertexRef operator[]( int iIndex ) const
			{ return VertexRef( *m_pkView, m_iIndex, iIndex ); }

	private:
		FvIWaypointView *m_pkView;
		int m_iIndex;
	};
};

#endif // __FvWaypointView_H__
