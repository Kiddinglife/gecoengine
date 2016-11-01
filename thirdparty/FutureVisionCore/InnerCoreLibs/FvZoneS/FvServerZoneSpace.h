//{future header message}
#ifndef __FvServerZoneSpace_H__
#define __FvServerZoneSpace_H__

#include "FvBaseZoneSpace.h"

class FvServerZoneSpace : public FvBaseZoneSpace
{
public:
	FvServerZoneSpace( FvZoneSpaceID id );

	void AddHomelessItem( FvZoneItem * /*pItem*/ ) {};

	void Focus();

	class ColumnGrid
	{
	public:
		ColumnGrid() :
			m_iXOrigin( 0 ),
			m_iYOrigin( 0 ),
			m_iXSize( 0 ),
			m_iYSize( 0 )
		{
		}

		void Rect( int iXOrigin, int iYOrigin, int iXSize, int iYSize );

		Column *& operator()( int iX, int iY )
		{
			static Column *pNullCol = NULL;
			return this->InSpan( iX, iY ) ?
				m_kGrid[ (iX - m_iXOrigin) + m_iXSize * (iY - m_iYOrigin) ] :
				pNullCol;
		}

		const Column * operator()( int iX, int iY ) const
		{
			return this->InSpan( iX, iY ) ?
				m_kGrid[ (iX - m_iXOrigin) + m_iXSize * (iY - m_iYOrigin) ] :
				NULL;
		}

		bool InSpan( int iX, int iY ) const
		{
			return m_iXOrigin <= iX && iX < m_iXOrigin + m_iXSize &&
					m_iYOrigin <= iY && iY < m_iYOrigin + m_iYSize;
		}

		const int XBegin() const	{	return m_iXOrigin; }
		const int XEnd() const		{	return m_iXOrigin + m_iXSize; }
		const int YBegin() const	{	return m_iYOrigin; }
		const int YEnd() const		{	return m_iYOrigin + m_iYSize; }

	private:
		std::vector< Column * > m_kGrid;

		int m_iXOrigin;
		int m_iYOrigin;

		int m_iXSize;
		int m_iYSize;
	};

	void SeeZone( FvZone * );

protected:

	void RecalcGridBounds();

	ColumnGrid m_kCurrentFocus;
};

#endif // __FvServerZoneSpace_H__