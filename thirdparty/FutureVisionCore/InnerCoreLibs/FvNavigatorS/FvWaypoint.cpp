#include <float.h>
#include "FvWaypoint.h"

#include <FvLine.h>
#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT2("FvNavigator", 0);

FvWaypoint::FvWaypoint() :
	m_uiID(0),
	m_fHeight(0.0f)
{
}

void FvWaypoint::ReadFromSection(FvXMLSectionPtr spSection, const FvZoneID &kZoneID)
{
	FvXMLSectionIterator it;

	m_uiID = spSection->AsLong();
	m_fHeight = spSection->ReadFloat("height");
	m_kVertexVector.clear();

	for(it = spSection->Begin(); it != spSection->End(); it++)
	{
		FvXMLSectionPtr pDS = *it;
		if(pDS->SectionName() == "vertex")
		{
			Vertex vertex;
			FvVector3 vPos = pDS->AsVector3();
			vertex.m_kPosition[0] = vPos[0];
			vertex.m_kPosition[1] = vPos[1];
			vertex.m_uiAdjacentID = FvWaypointID(int(vPos[2]));
			vertex.m_kAdjacentZoneID = pDS->ReadString("adjacentZone");
			vertex.m_pkAdjacentWaypoint = NULL;
			vertex.m_pkAdjacentWaypointSet = NULL;
			vertex.m_fDistanceToAdjacent = 0.0f;

			FvString & acid = vertex.m_kAdjacentZoneID;
			if (acid.empty())
			{
				if (int(vertex.m_uiAdjacentID) > 0)
				{
					acid = kZoneID;
				}
			}
			else
			{
				for (unsigned int i = 0; i < acid.size() - 1; i++)
					acid[i] = toupper(acid[i]);
			}

			m_kVertexVector.push_back(vertex);
		}
	}

	this->CalculateCentre();
}

void FvWaypoint::WriteToSection(FvXMLSectionPtr spSection, const FvZoneID &kZoneID)
{
	std::vector<Vertex>::iterator it;
	spSection->SetLong(m_uiID);
	spSection->WriteFloat("height", m_fHeight);

	for(it = m_kVertexVector.begin(); it != m_kVertexVector.end(); it++)
	{
		FvXMLSectionPtr pVertex = spSection->OpenSection("vertex", true);
		pVertex->SetVector3( FvVector3(
			it->m_kPosition[0], it->m_kPosition[1], float( it->m_uiAdjacentID ) ) );
		if (!it->m_kAdjacentZoneID.empty() && it->m_kAdjacentZoneID != kZoneID)
			pVertex->WriteString("adjacentZone", it->m_kAdjacentZoneID);
	}
}

void FvWaypoint::ReadFromStream(FvBinaryIStream &kStream)
{
	unsigned int i, count;
	Vertex v;

	m_kVertexVector.clear();
	kStream >> m_uiID >> m_fHeight >> m_kCentre >> count;

	for(i = 0; i < count; i++)
	{
		kStream >> v.m_kPosition >> v.m_uiAdjacentID >> v.m_kAdjacentZoneID;
		v.m_pkAdjacentWaypoint = NULL;
		v.m_pkAdjacentWaypointSet = NULL;
		v.m_fDistanceToAdjacent = 0.0f;
		m_kVertexVector.push_back(v);
	}
}

void FvWaypoint::WriteToStream(FvBinaryOStream &kStream)
{
	kStream << m_uiID << m_fHeight << m_kCentre << m_kVertexVector.size();

	for(unsigned int i = 0; i < m_kVertexVector.size(); i++)
	{
		kStream << m_kVertexVector[i].m_kPosition;
		kStream << m_kVertexVector[i].m_uiAdjacentID;
		kStream << m_kVertexVector[i].m_kAdjacentZoneID;
	}
}

int FvWaypoint::VertexCount() const
{
	return m_kVertexVector.size();
}

const FvVector2& FvWaypoint::VertexPosition(int iIndex) const
{
	return m_kVertexVector[iIndex].m_kPosition;
}

FvUInt32 FvWaypoint::EdgeFlags(int iIndex) const
{
	int aid = int( m_kVertexVector[iIndex].m_uiAdjacentID );
	return aid < 0 ? FvUInt32( -aid ) : 0;
}

FvWaypointID FvWaypoint::AdjacentID(int iIndex) const
{
	int aid = int( m_kVertexVector[iIndex].m_uiAdjacentID );
	return aid > 0 ? FvWaypointID( aid ) : 0;
}

const FvZoneID& FvWaypoint::AdjacentZoneID(int iIndex) const
{
	return m_kVertexVector[iIndex].m_kAdjacentZoneID;
}

FvWaypoint* FvWaypoint::AdjacentWaypoint(int iIndex) const
{
	return m_kVertexVector[iIndex].m_pkAdjacentWaypoint;
}

void FvWaypoint::AdjacentWaypoint(int iIndex, FvWaypoint *pkWaypoint)
{
	m_kVertexVector[iIndex].m_pkAdjacentWaypoint = pkWaypoint;
	m_kVertexVector[iIndex].m_fDistanceToAdjacent =
		(pkWaypoint->Centre() - this->Centre()).Length();
}

FvWaypointSet* FvWaypoint::AdjacentWaypointSet(int iIndex) const
{
	return m_kVertexVector[iIndex].m_pkAdjacentWaypointSet;
}

void FvWaypoint::AdjacentWaypointSet(int iIndex, FvWaypointSet *pkWSet)
{
	m_kVertexVector[iIndex].m_pkAdjacentWaypointSet = pkWSet;
}

bool FvWaypoint::ContainsPoint(float fX, float fY, float fZ) const
{
	if (fY > m_fHeight+2.f) return false;

	float u, v, xd, yd, c;
	unsigned int i, j;

	i = m_kVertexVector.size() - 1;
	for(j = 0; j < m_kVertexVector.size(); j++)
	{
		u = m_kVertexVector[j].m_kPosition[0] - m_kVertexVector[i].m_kPosition[0];
		v = m_kVertexVector[j].m_kPosition[1] - m_kVertexVector[i].m_kPosition[1];

		xd = fX - m_kVertexVector[i].m_kPosition[0];
		yd = fY - m_kVertexVector[i].m_kPosition[1];

		c = xd * v - yd * u;

		if(c < 0)
			return false;

		i = j;
	}

	return true;
}

bool FvWaypoint::IsAdjacentToZone(const FvZoneID &kZoneID)
{
	for(unsigned int i = 0; i < m_kVertexVector.size(); i++)
		if(m_kVertexVector[i].m_kAdjacentZoneID == kZoneID)
			return true;
	return false;
}

bool FvWaypoint::IsAdjacentToSet(const FvWaypointSet *pkWSet)
{
	for(unsigned int i = 0; i < m_kVertexVector.size(); i++)
		if(m_kVertexVector[i].m_pkAdjacentWaypointSet == pkWSet)
			return true;
	return false;
}

void FvWaypoint::CalculateCentre()
{
	FvVector2 newCen( 0.f, 0.f );

	for(unsigned int i = 0; i < m_kVertexVector.size(); i++)
		newCen += m_kVertexVector[i].m_kPosition;

	unsigned int sz = m_kVertexVector.size();
	if (sz != 0) newCen /= float(sz);

	m_kCentre.Set( newCen.x, newCen.y, m_fHeight );
}

FvWaypointID FvWaypoint::GetID() const
{
	return m_uiID;
}

const FvVector3& FvWaypoint::Centre() const
{
	return m_kCentre;
}

float FvWaypoint::Height() const
{
	return m_fHeight;
}

void FvWaypoint::Transform( const FvMatrix &kMatrix, bool bHeightToo )
{
	FvVector3 v;

	if (bHeightToo)
	{
		v.x = 0;
		v.y = 0;
		v.z = m_fHeight;

		v = kMatrix.ApplyPoint( v );
		m_fHeight = v.z;
	}


	for (unsigned int i = 0; i < m_kVertexVector.size(); i++)
	{
		v.x = m_kVertexVector[i].m_kPosition[0];
		v.y = m_kVertexVector[i].m_kPosition[1];
		v.z = 0.f;

		v = kMatrix.ApplyPoint( v );
		m_kVertexVector[i].m_kPosition.Set( v.x, v.y );
	}

	this->CalculateCentre();
}

bool FvWaypoint::FindClosestPoint(const FvVector3 &kDST3, FvVector3 &kIntersection)
{
	FvVector3 cen = this->Centre();
	FvVector2 src( cen.x, cen.y );
	FvVector2 dst( kDST3.x, kDST3.y );
	FvVector2 movementVector = dst - src;
	FvLine movementLine(src, dst, true);

	for (unsigned int i = 0; i < m_kVertexVector.size(); i++)
	{
		FvVector2 p1 = m_kVertexVector[i].m_kPosition;
		FvVector2 p2 = m_kVertexVector[(i + 1) % m_kVertexVector.size()].m_kPosition;

		float cp1 = movementVector.CrossProduct(p1 - src);
		float cp2 = movementVector.CrossProduct(p2 - src);

		if(cp1 > 0.0f && cp2 < 0.0f)
		{
			FvLine edgeLine(p1, p2, true);
			float p = movementLine.Intersect(edgeLine);
			FvVector2 interv2 = movementLine.Param(p);
			kIntersection.Set( interv2.x, interv2.y, cen.z + 0.1f );
			return true;
		}
	}

	return false;
}
