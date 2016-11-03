//{future header message}
#ifndef __FvClientZoneSpace_H__
#define __FvClientZoneSpace_H__

#include "FvBaseZoneSpace.h"
#include "FvZoneDefine.h"

#if FV_UMBRA_ENABLE
#include "UmbraCell.hpp"
#endif

class FvZone;
class FvZoneItem;
typedef FvSmartPointer<FvZoneItem> FvZoneItemPtr;
class FvBoundingBox;
class FvHullTree;
class FvHullBorder;
class FvHullContents;
class FvBSP;
class FvWorldTriangle;
class FvOutsideLighting;

class FvZoneObstacle;

template < class T, int ISPAN = 63 >
class FvFocusGrid
{
public:
	FvFocusGrid();
	~FvFocusGrid();

	enum { SPANH = ISPAN/2 };
	enum { SPAN = ISPAN };
	enum { SPANX = ISPAN+1 };

	void Origin( int iCX, int iCY );

	int OriginX() const	{ return m_iCX; }
	int OriginY() const	{ return m_iCY; }

	T * & operator()( int iX, int iY )
	{ return m_pkGrid[ iY & SPAN ][ iX & SPAN ]; }

	const T * operator()( int iX, int iY ) const
	{ return m_pkGrid[ iY & SPAN ][ iX & SPAN ]; }

	bool InSpan( int iX, int iY ) const
	{
		return	iX >= m_iCX - SPANH && iX <= m_iCX + SPANH &&
			iY >= m_iCY - SPANH && iY <= m_iCY + SPANH;
	}

	const int XBegin() const	{	return m_iCX - SPANH; }
	const int XEnd() const		{	return m_iCX + SPANH + 1; }
	const int YBegin() const	{	return m_iCY - SPANH; }
	const int YEnd() const		{	return m_iCY + SPANH + 1; }

private:
	void EraseEntry( int iX, int iY );

	int	m_iCX;
	int	m_iCY;
	T *m_pkGrid[SPANX][SPANX];
};

class FV_ZONE_API FvClientZoneSpace : public FvBaseZoneSpace
{
public:
	FvClientZoneSpace( FvZoneSpaceID id );
	~FvClientZoneSpace();

	void BlurSpace();
	void Clear();

	void AddHomelessItem( FvZoneItemPtr spItem );
	void DelHomelessItem( FvZoneItemPtr spItem );

	void Focus( const FvVector3 &kPoint );

	void Tick( float fTime );

	const FvZoneMap &Zones() const		{ return m_kCurrentZones; }
	FvZoneMap &Zones()					{ return m_kCurrentZones; }
	FvGridZoneMap &GridZones()			{ return m_kGridZones; }

	bool Ticking() const				{ return m_bTicking; }

	class Column : public FvBaseZoneSpace::Column
	{
	public:
		Column( int iX, int iY );
		~Column();

		unsigned int NumHoldings() const
		{ return m_kHoldings.size() + m_kHeldObstacles.size(); }

		void Soft( bool v )		{ m_bSoft = v; }

	private:

		bool m_bSoft;

		void AddObstacleInternal( const FvZoneObstacle &kObstacle );
	};

	typedef FvFocusGrid<Column> ColumnGrid;

	const FvMatrix &Common() const			{ return m_kCommon; }
	const FvMatrix &CommonInverse() const	{ return m_kCommonInverse; }

	void TransformSpaceToCommon( FvVector3 & pos, FvVector3 & dir );
	void TransformCommonToSpace( FvVector3 & pos, FvVector3 & dir );

	bool IsMapped() const;

#if FV_UMBRA_ENABLE
	Umbra::Cell* UmbraCell();
	Umbra::Cell* UmbraInsideCell();
#endif

protected:
	bool m_bTicking;

	ColumnGrid m_kCurrentFocus;

	std::vector<FvZoneItemPtr> m_kHomeless;

	FvMatrix m_kCommon;
	FvMatrix m_kCommonInverse;
	bool m_bIsMapped;

#if FV_UMBRA_ENABLE
	Umbra::Cell *m_pkUmbraCell;
	Umbra::Cell *m_pkUmbraInsideCell;
#endif

};

#endif // __FvClientZoneSpace_H__