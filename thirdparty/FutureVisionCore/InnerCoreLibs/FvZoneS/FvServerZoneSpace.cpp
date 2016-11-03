#include "FvServerZoneSpace.h"
#include "FvZone.h"

FV_DECLARE_DEBUG_COMPONENT2( "Zone", 0 )

FvServerZoneSpace::FvServerZoneSpace( FvZoneSpaceID id ) :
	FvBaseZoneSpace( id )
{
}


void FvServerZoneSpace::SeeZone( FvZone *pkZone )
{
	const FvVector3 &kCen = pkZone->Centre();
	int iNX = int(kCen.x / FV_GRID_RESOLUTION);	if (kCen.x < 0.f) iNX--;
	int iNY = int(kCen.y / FV_GRID_RESOLUTION);	if (kCen.y < 0.f) iNY--;

	for ( int x = iNX - 1; x <= iNX + 1; ++x )
	{
		for ( int y = iNY - 1; y <= iNY + 1; ++y )
		{
			FvServerZoneSpace::Column * pCol = m_kCurrentFocus( x, y );		
			if (pCol != NULL) pCol->OpenAndSee( pkZone );
		}
	}
}

void FvServerZoneSpace::Focus()
{
	while ( m_kBlurred.size() > 0 )
	{
		FvZone *pkZone = *(m_kBlurred.begin());
		m_kBlurred.erase( m_kBlurred.begin() );

		const FvVector3 &kCen = pkZone->Centre();
		int iNX = int(kCen.x / FV_GRID_RESOLUTION);	if (kCen.x < 0.f) iNX--;
		int iNY = int(kCen.y / FV_GRID_RESOLUTION);	if (kCen.y < 0.f) iNY--;

		for ( int x = iNX - 1; x <= iNX + 1; ++x ) 
		{
			for ( int y = iNY - 1; y <= iNY + 1; ++y )
			{
				Column * pCol = m_kCurrentFocus( x, y );
				if ( pCol != NULL ) {
					pCol->ShutIfSeen( pkZone );
				}
			}
		}

		pkZone->Focus();

		for ( int x = iNX - 1; x <= iNX + 1; ++x )
		{
			for ( int y = iNY - 1; y <= iNY + 1; ++y )
			{
				Column * pCol = m_kCurrentFocus( x, y );

				if (pCol != NULL) pCol->OpenAndSee( pkZone );
			}
		}
	}
}

void FvServerZoneSpace::RecalcGridBounds()
{
	m_kCurrentFocus.Rect( m_iMinGridX, m_iMinGridY,
		m_iMaxGridX-m_iMinGridX+1, m_iMaxGridY-m_iMinGridY+1 );
}


void FvServerZoneSpace::ColumnGrid::Rect(
	int xOriginNew, int zOriginNew, int xSizeNew, int zSizeNew )
{
	std::vector< Column * > gridNew( xSizeNew * zSizeNew );

	for (int x = m_iXOrigin; x < m_iXOrigin+m_iXSize; x++)
	{
		bool xInSpan = (x >= xOriginNew && x < xOriginNew+xSizeNew);
		for (int z = m_iYOrigin; z < m_iYOrigin+m_iYSize; z++)
		{
			bool zInSpan = (z >= zOriginNew && z < zOriginNew+zSizeNew);

			Column *& col = m_kGrid[ (x-m_iXOrigin) + m_iXSize*(z-m_iYOrigin) ];

			if (xInSpan && zInSpan)
				gridNew[ (x-xOriginNew) + xSizeNew*(z-zOriginNew) ] = col;
			else
				delete col;

			col = NULL;
		}
	}

	m_iXOrigin = xOriginNew;
	m_iYOrigin = zOriginNew;
	m_iXSize = xSizeNew;
	m_iYSize = zSizeNew;

	m_kGrid.swap( gridNew );
}

