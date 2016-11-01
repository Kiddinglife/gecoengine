//{future header message}
#include "FvPortal2D.h"
#include "FvPowerDefines.h"

#include <FvDebug.h>

typedef std::vector<char> FvInsideVector;
static FvInsideVector s_kInsideVector;

//----------------------------------------------------------------------------
FvPortal2D::FvPortal2D() : m_uiRefs(0)
{

}
//----------------------------------------------------------------------------
FvPortal2D::FvPortal2D(const FvPortal2D& kOther) :
	m_kPoints(kOther.m_kPoints), m_uiRefs(0)
{

}
//----------------------------------------------------------------------------
FvPortal2D::~FvPortal2D()
{

}
//----------------------------------------------------------------------------
FvPortal2D& FvPortal2D::operator = (const FvPortal2D& kOther)
{
	m_kPoints = kOther.m_kPoints;
	return *this;
}
//----------------------------------------------------------------------------
void FvPortal2D::SetDefaults()
{
	ErasePoints();

	FvVector2 kVec( -1, 1 );
	m_kPoints.push_back(kVec);
	kVec[0] = 1;
	m_kPoints.push_back(kVec);
	kVec[1] = -1;
	m_kPoints.push_back(kVec);
	kVec[0] = -1;
	m_kPoints.push_back(kVec);
}
//----------------------------------------------------------------------------
void FvPortal2D::AddPoint(const FvVector2& kVec)
{
	m_kPoints.push_back(kVec);
}
//----------------------------------------------------------------------------
void FvPortal2D::ErasePoints()
{
	m_kPoints.clear();
}
//----------------------------------------------------------------------------
bool FvPortal2D::Combine(FvPortal2D* pkPortal0, FvPortal2D* pkPortal1)
{
	FV_ASSERT( pkPortal0 );
	FV_ASSERT( pkPortal1 );

	FvInsideVector& kInside = s_kInsideVector;

	m_kPoints.resize(pkPortal1->m_kPoints.size());
	FvMemoryCopy(&m_kPoints.front(), &pkPortal1->m_kPoints.front(),
		sizeof(FvVector2) * pkPortal1->m_kPoints.size());

	pkPortal0->m_kPoints.push_back(pkPortal0->m_kPoints.front());

	for(unsigned int i = 0; i < (pkPortal0->m_kPoints.size() - 1); i++ )
	{
		kInside.clear();
		FvVector2 n2 = pkPortal0->m_kPoints[i+1] - pkPortal0->m_kPoints[i];
		FvVector2 n(n2[1], -n2[0]);
		float d = n.DotProduct(pkPortal0->m_kPoints[i]);

		unsigned int j ;
		for(j = 0; j < m_kPoints.size(); j++)
		{
			char isInside = (n.DotProduct(m_kPoints[j]) >= d) ? 1 : 0;
			kInside.push_back(isInside);
		}
		m_kPoints.push_back(m_kPoints.front());

		int firstInside = kInside[0];
		kInside.push_back(firstInside);

		for(j = 0; j < (m_kPoints.size() - 1); j++ )
		{
			int inside1 = kInside[j];
			int inside2 = kInside[j+1];

			if( inside1 != inside2 )
			{
				FvVector2 v = m_kPoints[j];
				FvVector2 dv = m_kPoints[j+1] - v;
				float t = (d - n.DotProduct(v)) / n.DotProduct(dv);
				dv *= t;
				v += dv;
				j++;
				m_kPoints.push_back(m_kPoints.back());
				for(unsigned int k = m_kPoints.size()-1; k > j; k--)
					m_kPoints[k] = m_kPoints[k-1];
				m_kPoints[j] = v;
				kInside.push_back(kInside.back());
				for (unsigned int k = kInside.size()-1; k > j; k--)
					kInside[k] = kInside[k-1];
				kInside[j] = 1;
			}
		}

		m_kPoints.pop_back();
		kInside.pop_back();

		j = 0;
		for (FvInsideVector::iterator bit = kInside.begin();
			bit != kInside.end(); bit++)
		{
			if(*bit == 0)
				m_kPoints.erase(m_kPoints.begin() + j);
			else
				j++;
		}
	}

	pkPortal0->m_kPoints.pop_back();

	return m_kPoints.size() >= 3;
}
//----------------------------------------------------------------------------
bool FvPortal2D::Contains(const FvVector2& kVec) const
{
	FvVector2 ptA = m_kPoints.back(), ptB;
	for(unsigned int i = 0; i < m_kPoints.size(); i++)
	{
		ptB = m_kPoints[i];
		FvVector2 seg = ptB - ptA;
		FvVector2 nor( seg.y, -seg.x );
		if(nor.DotProduct(kVec) < nor.DotProduct( ptA ))
			return false;

		ptA = ptB;
	}

	return true;
}
//----------------------------------------------------------------------------
const FvPortal2D::FvVector2Vec& FvPortal2D::Points() const
{
	return m_kPoints;
}
//----------------------------------------------------------------------------
unsigned int FvPortal2D::Refs()
{
	return m_uiRefs;
}
//----------------------------------------------------------------------------
void FvPortal2D::Refs(unsigned int uiRefs)
{
	m_uiRefs = uiRefs;
}
//----------------------------------------------------------------------------
