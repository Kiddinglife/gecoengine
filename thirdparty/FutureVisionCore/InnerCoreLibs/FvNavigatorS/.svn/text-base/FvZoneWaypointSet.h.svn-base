#ifndef __FvZoneWaypointSet_H__
#define __FvZoneWaypointSet_H__

#include <map>
#include <vector>

#include <FvDebug.h>
#include <FvZone.h>
#include <FvZoneItem.h>
#include <FvXMLSection.h>
#include <FvZoneBoundary.h>
#include <FvZoneSerializerXMLImpl.h>

template <class C> class FvDependentArray
{
public:
	typedef C value_type;
	typedef value_type * iterator;
	typedef const value_type * const_iterator;
	typedef typename std::vector<C>::size_type size_type;

	FvDependentArray() :
		m_pkData( 0 )
	{
		_size() = 0;
	}

	FvDependentArray( iterator beg, iterator end ) :
		m_pkData( beg )
	{
		_size() = end - beg;
	}

	FvDependentArray( const FvDependentArray & other ) :
		m_pkData( other.m_pkData )
	{
		_size() = other._size();
	}

	FvDependentArray & operator=( const FvDependentArray & other )
	{
		new (this) FvDependentArray( other );
		return *this;
	}

	~FvDependentArray()
		{ }

	size_type size() const	{ return _size(); }

	const value_type & operator[]( int i ) const { return m_pkData[i]; }

	value_type & operator[]( int i ){ return m_pkData[i]; }

	const value_type & front() const{ return *m_pkData; }

	value_type & front()			{ return *m_pkData; }

	const value_type & back() const { return m_pkData[_size()-1]; }

	value_type & back()				{ return m_pkData[_size()-1]; }

	const_iterator begin() const	{ return m_pkData; }

	const_iterator end() const		{ return m_pkData+_size(); }

	iterator begin()				{ return m_pkData; }

	iterator end()					{ return m_pkData+_size(); }

private:
	value_type *m_pkData;

	FvUInt16 _size() const	{ return *(FvUInt16*)(this+1); }
	FvUInt16 & _size()		{ return *(FvUInt16*)(this+1); }
};

class FvZoneWaypoint
{
public:
	bool Contains( const FvVector3 &kPoint ) const;
	bool ContainsProjection( const FvVector3 &kPoint ) const;
	float DistanceSquared( const FvZone *pkZone, const FvVector3 &kPoint ) const;
	void Clip( const FvZone *pkZone, FvVector3 &kLPoint ) const;

	float m_fMinHeight;

	float m_fMaxHeight;

	struct Edge
	{
		FvVector2 m_kStart;

		FvUInt32 m_uiNeighbour;

		int NeighbouringWaypoint() const
		{
			return m_uiNeighbour < 32768 ? int(m_uiNeighbour) : -1;
		}

		bool AdjacentToZone() const
		{
			return m_uiNeighbour >= 32768 && m_uiNeighbour <= 65535;
		}

		FvUInt32 NeighbouringVista() const
		{
			return int(m_uiNeighbour) < 0 ? ~m_uiNeighbour : 0;
		}
	};

	void Print()
	{
		FV_DEBUG_MSG( "MinHeight: %g\tMaxHeight: %g\tEdgeNum:%zu\n",
			m_fMinHeight, m_fMaxHeight, m_kEdges.size()  );

		for (FvUInt16 i = 0; i < m_kEdges.size(); ++i)
		{
			FV_DEBUG_MSG( "\t%d (%g, %g) %d - %s\n", i,
				m_kEdges[ i ].m_kStart.x, m_kEdges[ i ].m_kStart.y, m_kEdges[ i ].NeighbouringWaypoint(),
				m_kEdges[ i ].AdjacentToZone() ? "zone" : "no zone" );
		}
	}

	typedef FvDependentArray<Edge> Edges;

	Edges m_kEdges;

	FvUInt16 m_uiEdgeCount;

	mutable FvUInt16 m_uiMark;

	static FvUInt16 ms_uiNextMark;
};


typedef std::vector<FvZoneWaypoint> FvZoneWaypoints;


typedef FvUInt32 FvWaypointEdgeIndex;

class FvZoneWaypointSetData : public FvSafeReferenceCount
{
public:
	FvZoneWaypointSetData();
	virtual ~FvZoneWaypointSetData();

	bool LoadFromXML( FvXMLSectionPtr spSection, const FvString &kSectionName);
	void Transform( const FvMatrix &kMat );

	int Find( const FvVector3 &kLPoint, bool bIgnoreHeight = false );
	int Find( const FvZone *pkZone, const FvVector3 &kLPoint, float &fBestDistanceSquared );

	int GetAbsoluteEdgeIndex( const FvZoneWaypoint::Edge &kEdge ) const;

private:
	float m_fGirth;
	FvZoneWaypoints m_kWaypoints;
	FvString m_kSource;
	FvZoneWaypoint::Edge *m_pkEdges;

	friend class FvZoneWaypointSet;
	friend class FvZoneNavPolySet;
};


typedef FvSmartPointer<FvZoneWaypointSetData> FvZoneWaypointSetDataPtr;


class	FvZoneWaypointSet;
typedef FvSmartPointer<FvZoneWaypointSet> FvZoneWaypointSetPtr;
typedef std::vector<FvZoneWaypointSetPtr> FvZoneWaypointSets;

typedef std::map<FvZoneWaypointSetPtr, FvZoneBoundary::Portal *>
	FvZoneWaypointConns;

typedef std::map<FvWaypointEdgeIndex, FvZoneWaypointSetPtr>
	FvZoneWaypointEdgeLabels;

class FvZoneWaypointSet : public FvZoneItem
{
	FV_DECLARE_ZONE_ITEM_XML( FvZoneWaypointSet )
public:
	FvZoneWaypointSet();
	~FvZoneWaypointSet();

	bool Load( FvZone * pkZone, FvXMLSectionPtr spSection,
		const char *pcSectionName, bool bInWorldCoords );

	virtual void Toss( FvZone * pkZone );

	void Bind();

	int Find( const FvVector3 & kLPoint, bool bIgnoreHeight = false )
	{
		return m_spData->Find( kLPoint, bIgnoreHeight );
	}

	int Find( const FvVector3 &kLPoint, float &fBestDistanceSquared )
	{ 
		return m_spData->Find( Zone(), kLPoint, fBestDistanceSquared ); 
	}

	float Girth() const
	{
		return m_spData->m_fGirth;
	}

	int WaypointCount() const
	{
		return m_spData->m_kWaypoints.size();
	}

	FvZoneWaypoint &Waypoint( int iIndex )
	{
		return m_spData->m_kWaypoints[iIndex];
	}

	FvZoneWaypointConns::iterator ConnectionsBegin()
	{
		return m_kConnections.begin();
	}

	FvZoneWaypointConns::const_iterator ConnectionsBegin() const
	{
		return m_kConnections.begin();
	}

	FvZoneWaypointConns::iterator ConnectionsEnd()
	{
		return m_kConnections.end();
	}

	FvZoneWaypointConns::const_iterator ConnectionsEnd() const
	{
		return m_kConnections.end();
	}

	FvZoneBoundary::Portal *ConnectionPortal( FvZoneWaypointSetPtr spWaypointSet )
	{
		return m_kConnections[spWaypointSet];
	}

	FvZoneWaypointSetPtr ConnectionWaypoint( const FvZoneWaypoint::Edge &kEdge )
	{
		return m_kEdgeLabels[m_spData->GetAbsoluteEdgeIndex( kEdge )];
	}

	void AddBacklink( FvZoneWaypointSetPtr pWaypointSet );

	void RemoveBacklink( FvZoneWaypointSetPtr pWaypointSet );

	void Print()
	{
		FV_DEBUG_MSG( "FvZoneWayPointSet: 0x%p - %s\tWayPointCount: %d\n", this, m_pkZone->Identifier().c_str(), WaypointCount() );

		for (int i = 0; i < WaypointCount(); ++i)
			Waypoint( i ).Print();

		for (FvZoneWaypointConns::iterator iter = ConnectionsBegin();
			iter != ConnectionsEnd(); ++iter)
		{
			FV_DEBUG_MSG( "**** connecting to 0x%p %s", iter->first.GetObject(), iter->first->m_pkZone->Identifier().c_str() );
		}
	}

private:
	void DeleteConnection( FvZoneWaypointSetPtr pSet );

	void RemoveOurConnections();
	void RemoveOthersConnections();

	void Connect(
		FvZoneWaypointSetPtr spWaypointSet,
		FvZoneBoundary::Portal *pkPortal,
		FvZoneWaypoint::Edge &kEdge );

protected:
	FvZoneWaypointSetDataPtr m_spData;
	FvZoneWaypointConns m_kConnections;
	FvZoneWaypointEdgeLabels m_kEdgeLabels;
	FvZoneWaypointSets m_kBacklinks;
};

class FvZoneNavigator : public FvZoneCache
{
public:
	FvZoneNavigator( FvZone & kZone );
	~FvZoneNavigator();

	virtual void Bind( bool bLooseNotBind );

	struct Result
	{
		Result() : m_spSet( NULL ), m_iWaypoint( -1 ) { }

		FvZoneWaypointSetPtr m_spSet;
		int m_iWaypoint;
		bool m_bExactMatch;
	};

	bool Find( const FvVector3 &kLPoint, float fGirth,
				Result &kRes, bool bIgnoreHeight = false );

	bool IsEmpty();
	bool HasNavPolySet( float fGirth );

	void Add( FvZoneWaypointSet *pkSet );
	void Del( FvZoneWaypointSet *pkSet );

	static Instance<FvZoneNavigator> ms_kInstance;
	static bool ms_bUseGirthGrids;

private:
	FvZone &m_kZone;

	FvZoneWaypointSets m_kWPSets;

	struct GGElement
	{
		FvZoneWaypointSet *m_pkSet;
		int m_iWaypoint;

		GGElement( FvZoneWaypointSet *pkSet, int iWaypoint )
			: m_pkSet(pkSet), m_iWaypoint(iWaypoint)
		{}
	};

	class GGList : public std::vector<GGElement>
	{
	public:
		bool Find( const FvVector3 &kLPoint, Result &kRes,
							bool bIgnoreHeight = false );
		void Find( const FvZone *pkZone, const FvVector3 &kLPoint,
			float &fBestDistanceSquared, Result &kRes );
	};

	struct GirthGrid
	{
		float m_fGirth;
		GGList *m_pkGrid;
	};
	typedef std::vector<GirthGrid> GirthGrids;

	GirthGrids m_kGirthGrids;
	FvVector2 m_kGGOrigin;
	float m_fGGResolution;

	static const unsigned int GG_SIZE = 12;
};

#endif // __FvZoneWaypointSet_H__
