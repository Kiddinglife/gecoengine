//{future header message}
#ifndef __FvGridTraversal_H__
#define __FvGridTraversal_H__

class FvSpaceGridTraversal
{
public:
	FvSpaceGridTraversal( const FvVector3 &kSource, const FvVector3 &kExtent,
		const FvVector3 &kRange, float fGridResF ) :
		m_kExtent( kExtent ),
		m_fGridResF( fGridResF ),
		m_uiFirstAlt( ms_kAltCells.size() )
	{
		m_iSX = int(kSource.x / m_fGridResF);	if (kSource.x < 0.f) m_iSX--;
		m_iSY = int(kSource.y / m_fGridResF);	if (kSource.y < 0.f) m_iSY--;

		m_iEX = int(m_kExtent.x / m_fGridResF);	if (m_kExtent.x < 0.f) m_iEX--;
		m_iEY = int(m_kExtent.y / m_fGridResF);	if (m_kExtent.y < 0.f) m_iEY--;

		m_kDir = m_kExtent - kSource;
		m_iXSense = m_kDir.x < 0 ? -1 : m_kDir.x > 0 ? 1 : 0;
		m_iYSense = m_kDir.y < 0 ? -1 : m_kDir.y > 0 ? 1 : 0;

		m_fXRange = kRange.x;
		m_fYRange = kRange.y;
		m_fXSeep = m_fGridResF - kRange.x;
		m_fYSeep = m_fGridResF - kRange.y;

		m_fFullDist = m_kDir.Length();
		m_kDir /= m_fFullDist;

		m_kLand = kSource;
		m_fCellETravelPrivate = 0.f;
		m_iNX = m_iSX;
		m_iNY = m_iSY;
		m_bAllOver = false;

		m_bOnRight = m_kLand.x - m_iNX * m_fGridResF > m_fXSeep;
		m_bOnAbove = m_kLand.y - m_iNY * m_fGridResF > m_fYSeep;
		m_pfNearSTravel[0][0] = 0.f, m_pfNearSTravel[0][1] = 0.f;
		m_pfNearSTravel[1][0] = 0.f, m_pfNearSTravel[1][1] = 0.f;
		OffNear( m_iSX, m_iSY );
		m_pcNearCount[0][0] = 0, m_pcNearCount[0][1] = 0;
		m_pcNearCount[1][0] = 0, m_pcNearCount[1][1] = 0;
		IncNear( 0, 0, 0.f );
		IncNear( 0, m_bOnAbove, 0.f );
		IncNear( m_bOnRight, 0, 0.f );
		IncNear( m_bOnRight, m_bOnAbove, 0.f );

		this->CalcNext();

		FV_VERIFY( this->PopAltCell() );
	}

	~FvSpaceGridTraversal()
	{
		ms_kAltCells.resize( m_uiFirstAlt );
	}

	bool Next()
	{
		if (this->PopAltCell()) return true;

		if (m_bAllOver) return false;

		m_iSX = m_iNX;
		m_iSY = m_iNY;

		this->CalcNext();

		FV_VERIFY( this->PopAltCell() );

		return true;
	}

	FvVector3 m_kExtent;
	FvVector3 m_kDir;

	float m_fGridResF;

	float m_fFullDist;
	bool m_bAllOver;

	int	m_iXSense;
	int	m_iYSense;

	int	m_iSX;
	int	m_iSY;
	int	m_iEX;
	int	m_iEY;

	float m_fCellSTravel;
	float m_fCellETravel;

private:
	FvVector3 m_kCast;
	FvVector3 m_kLand;

	float m_fCellETravelPrivate;

	float m_fXRange;
	float m_fYRange;
	float m_fXSeep;
	float m_fYSeep;

	int	m_iNX;
	int	m_iNY;

	int	m_iGX;
	int	m_iGY;

	int	m_iBX;
	int	m_iBY;

	bool m_bOnRight;
	bool m_bOnAbove;

	char m_pcNearCount[2][2];
	float m_pfNearSTravel[2][2];

	struct CellSpec
	{
		int	m_iX;
		int	m_iY;
		float m_fSTravel;
		float m_fETravel;
	};
	static FvVectorNoDestructor<CellSpec> ms_kAltCells;
	unsigned int m_uiFirstAlt;

	inline void IncNear( int iR, int iA, float fTrav )
	{
		iR ^= m_iBX;
		iA ^= m_iBY;

		if (!m_pcNearCount[iR][iA]++)
		{
			m_pfNearSTravel[iR][iA] = fTrav;
		}
	}

	inline void OffNear( int iX, int iY )
	{
		m_iGX = iX;
		m_iGY = iY;
		m_iBX = iX&1;
		m_iBY = iY&1;
	}

	inline void DecNear( int iR, int iA, float fTrav )
	{
		iR ^= m_iBX;
		iA ^= m_iBY;
		if (!--m_pcNearCount[iR][iA])
		{
			CellSpec kCS = { m_iGX+(iR^m_iBX), m_iGY+(iA^m_iBY), m_pfNearSTravel[iR][iA], fTrav };
			ms_kAltCells.push_back( kCS );
		}
	}

	void CalcNext()
	{
		m_kCast = m_kLand;
		m_kLand = m_kExtent;

		m_fCellETravel = m_fCellETravelPrivate;
		m_fCellSTravel = m_fCellETravel;

		float fTryDist;
		float fTryOther;
		if (m_iXSense != 0)
		{
			float xedge = ( m_iXSense < 0 ? m_iSX : m_iSX + 1 ) * m_fGridResF;
			fTryDist = (xedge - m_kCast.x) / m_kDir.x;
			fTryOther = m_kCast.y + fTryDist * m_kDir.y;
			if (fTryOther >= m_iSY * m_fGridResF &&
				fTryOther <= (m_iSY+1) * m_fGridResF &&
				m_fCellETravel + fTryDist < m_fFullDist)
			{
				m_kLand.x = xedge;
				m_kLand.y = fTryOther;
				m_kLand.z = m_kCast.z + fTryDist * m_kDir.z;
				m_iNX = m_iSX + m_iXSense;
				m_fCellETravel += fTryDist;

				if (m_iXSense < 0)	
				{
					DecNear( m_bOnRight, 0, m_fCellETravel );
					DecNear( m_bOnRight, m_bOnAbove, m_fCellETravel );

					OffNear( m_iNX, m_iNY );
					m_bOnRight = m_fXRange > 0.f;	

					IncNear( 0, 0, m_fCellETravel );
					IncNear( 0, m_bOnAbove, m_fCellETravel );
					DecNear( 1, 0, m_fCellETravel );
					DecNear( 1, m_bOnAbove, m_fCellETravel );

					IncNear( m_bOnRight, 0, m_fCellETravel );
					IncNear( m_bOnRight, m_bOnAbove, m_fCellETravel );
				}
				else			
				{
					IncNear( 1, 0, m_fCellETravel );
					IncNear( 1, m_bOnAbove, m_fCellETravel );
					DecNear( 0, 0, m_fCellETravel );
					DecNear( 0, m_bOnAbove, m_fCellETravel );

					DecNear( m_bOnRight, 0, m_fCellETravel );
					DecNear( m_bOnRight, m_bOnAbove, m_fCellETravel );

					OffNear( m_iNX, m_iNY );
					m_bOnRight = 0;			

					IncNear( m_bOnRight, 0, m_fCellETravel );
					IncNear( m_bOnRight, m_bOnAbove, m_fCellETravel );
				}
			}
		}
		if (m_iYSense != 0)
		{
			float	zedge = ( m_iYSense < 0 ? m_iSY : m_iSY+1 ) * m_fGridResF;
			fTryDist = (zedge - m_kCast.y) / m_kDir.y;
			fTryOther = m_kCast.x + fTryDist * m_kDir.x;
			if (fTryOther >= m_iSX * m_fGridResF &&
				fTryOther <= (m_iSX+1) * m_fGridResF &&
				m_fCellETravel + fTryDist < m_fFullDist)
			{
				m_kLand.x = fTryOther;
				m_kLand.y = zedge;
				m_kLand.z = m_kCast.z + fTryDist * m_kDir.z;
				m_iNY = m_iSY + m_iYSense;
				if (m_iNX == m_iSX) m_fCellETravel += fTryDist;

				if (m_iYSense < 0)	
				{
					DecNear( 0, m_bOnAbove, m_fCellETravel );
					DecNear( m_bOnRight, m_bOnAbove, m_fCellETravel );

					OffNear( m_iNX, m_iNY );
					m_bOnAbove = m_fYRange > 0.f;	

					IncNear( 0, 0, m_fCellETravel );
					IncNear( m_bOnRight, 0, m_fCellETravel );
					DecNear( 0, 1, m_fCellETravel );
					DecNear( m_bOnRight, 1, m_fCellETravel );

					IncNear( 0, m_bOnAbove, m_fCellETravel );
					IncNear( m_bOnRight, m_bOnAbove, m_fCellETravel );
				}
				else			
				{
					IncNear( 0, 1, m_fCellETravel );
					IncNear( m_bOnRight, 1, m_fCellETravel );
					DecNear( 0, 0, m_fCellETravel );
					DecNear( m_bOnRight, 0, m_fCellETravel );

					DecNear( 0, m_bOnAbove, m_fCellETravel );
					DecNear( m_bOnRight, m_bOnAbove, m_fCellETravel );

					OffNear( m_iNX, m_iNY );
					m_bOnAbove = 0;			

					IncNear( 0, m_bOnAbove, m_fCellETravel );
					IncNear( m_bOnRight, m_bOnAbove, m_fCellETravel );
				}
			}
		}

		if (m_iSX == m_iNX)
		{
			bool nnRight = m_kLand.x - m_iNX * m_fGridResF > m_fXSeep;
			if (nnRight != m_bOnRight)
			{
				DecNear( m_bOnRight, 0, m_fCellETravel );
				DecNear( m_bOnRight, m_bOnAbove, m_fCellETravel );
				m_bOnRight = nnRight;
				IncNear( m_bOnRight, 0, m_fCellSTravel );
				IncNear( m_bOnRight, m_bOnAbove, m_fCellSTravel );
			}
		}
		if (m_iSY == m_iNY)
		{
			bool nnAbove = m_kLand.y - m_iNY * m_fGridResF > m_fYSeep;
			if (nnAbove != m_bOnAbove)
			{
				DecNear( 0, m_bOnAbove, m_fCellETravel );
				DecNear( m_bOnRight, m_bOnAbove, m_fCellETravel );
				m_bOnAbove = nnAbove;
				IncNear( 0, m_bOnAbove, m_fCellSTravel );
				IncNear( m_bOnRight, m_bOnAbove, m_fCellSTravel );
			}
		}

		if (m_iNX == m_iSX && m_iNY == m_iSY)
		{
			m_fCellETravel = m_fFullDist;

			DecNear( 0, 0, m_fCellETravel );
			DecNear( 0, m_bOnAbove, m_fCellETravel );
			DecNear( m_bOnRight, 0, m_fCellETravel );
			DecNear( m_bOnRight, m_bOnAbove, m_fCellETravel );

			m_bAllOver = true;
		}

		m_fCellETravelPrivate = m_fCellETravel;
	}

	bool PopAltCell()
	{
		if (ms_kAltCells.size() == m_uiFirstAlt) return false;

		CellSpec kCS = ms_kAltCells.back();
		ms_kAltCells.pop_back();

		m_iSX = kCS.m_iX;
		m_iSY = kCS.m_iY;
		m_fCellSTravel = kCS.m_fSTravel;
		m_fCellETravel = kCS.m_fETravel;
		return true;
	}
};


#endif // __FvGridTraversal_H__