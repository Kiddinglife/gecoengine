#include "FvZoneView.h"

#include <FvLine.h>
#include <FvZone.h>
#include <FvZoneSpace.h>

#include "FvWaypoint.h"

FV_DECLARE_DEBUG_COMPONENT2( "FvNavigator", 0 )

FvZoneView::FvZoneView()
{
	this->Clear();
}

FvZoneView::~FvZoneView()
{
	this->Clear();
}

void FvZoneView::Clear()
{
	m_bMinMaxValid = false;
	m_iNextID = 1;
	m_kPolygons.clear();
}

static const int s_iNavPolySetEltSize = 16;
static const int s_iNavPolyEltSize = 12;
static const int s_iNavPolyEdgeEltSize = 12;


bool FvZoneView::Load( FvZone *pkZone )
{
	if (pkZone == NULL) 
	{
		FV_WARNING_MSG( "FvZoneView::Load: pkZone == 0\n" );
		return false;
	}

	FvXMLSectionPtr pSection;
	FvXMLSectionIterator it;

	this->Clear();

	Ogre::DataStreamPtr spDataSection;
	try
	{
		spDataSection = Ogre::ResourceGroupManager::getSingleton().openResource(
			pkZone->ResourceID(),
			pkZone->Mapping()->Group());
	}catch(...){}

	if(spDataSection.isNull())
	{
		FV_WARNING_MSG( "FvZoneView::Load: spDataSection == 0\n" );
		return false;
	}

	pSection = FvXMLSection::OpenSection( spDataSection );

	if(!pSection)
	{
		FV_WARNING_MSG( "FvZoneView::Load: pSection == 0\n" );
		return false;
	}

	std::vector<FvXMLSectionPtr> wpSets;
	pSection->OpenSections( "waypointSet", wpSets );
	for (unsigned int i = 0; i < wpSets.size(); i++)
	{
		int set = wpSets[i]->AsInt();
		if (int(m_kSets.size()) <= set) m_kSets.resize( set+1 );
		m_kSets[set].m_fGirth = wpSets[i]->ReadFloat( "girth" );
		for (FvXMLSectionIterator it = wpSets[i]->Begin();
			it != wpSets[i]->End();
			it++)
		{
			FvXMLSectionPtr pDS = *it;
			if (pDS->SectionName() == "waypoint")
				this->ParseNavPoly( pDS, set );
		}
	}

	std::vector<FvXMLSectionPtr> npSets;
	pSection->OpenSections( "navPolySet", npSets );
	for (unsigned int i = 0; i < npSets.size(); i++)
	{
		int set = npSets[i]->AsInt();
		if (int(m_kSets.size()) <= set) m_kSets.resize( set+1 );
		m_kSets[set].m_fGirth = npSets[i]->ReadFloat( "girth" );
		for (FvXMLSectionIterator it = npSets[i]->Begin();
			it != npSets[i]->End();
			it++)
		{
			FvXMLSectionPtr pDS = *it;
			if (pDS->SectionName() == "navPoly")
				this->ParseNavPoly( pDS, set, pkZone );
		}
	}

	Ogre::DataStreamPtr spNavmesh;
	char *pcNavmesh = NULL;
	size_t stNavmeshSize = 0;
	try
	{
		spNavmesh = Ogre::ResourceGroupManager::getSingleton().openResource(
			pkZone->Mapping()->Path() + pkZone->Identifier() + ".nav",
			pkZone->Mapping()->Group());
	}catch(...){}

	if(!spNavmesh.isNull() && spNavmesh->size())
	{
		stNavmeshSize = spNavmesh->size();
		pcNavmesh = new char[stNavmeshSize];
		spNavmesh->read(pcNavmesh,stNavmeshSize);
	}

	if (pcNavmesh && stNavmeshSize)
	{
		FvMatrix tr = FvMatrix::IDENTITY;

		const char * dataBeg = pcNavmesh;
		const char * dataEnd = dataBeg + stNavmeshSize;
		const char * dataPtr = dataBeg;

		int setIdx = 0;
		while (dataPtr < dataEnd)
		{
			int aVersion = *((int*&)dataPtr)++;
			float aGirth = *((float*&)dataPtr)++;
			int navPolyElts = *((int*&)dataPtr)++;
			int navPolyEdges = *((int*&)dataPtr)++;

			m_kSets.resize( setIdx+2 );
			m_kSets[setIdx+1].m_fGirth = aGirth;

			int polyOffset = m_iNextID-1;

			const char * edgePtr = dataPtr + navPolyElts * s_iNavPolyEltSize;
			for (int p = 0; p < navPolyElts; p++)
			{
				PolygonDef polyDef;
				polyDef.m_fMinHeight = *((float*&)dataPtr)++;
				polyDef.m_fMaxHeight = *((float*&)dataPtr)++;
				polyDef.m_iSet = setIdx+1;

				int vertexCount = *((int*&)dataPtr)++;

				FvVector3 inLocalFirst( 0, 0, 0 );
				for (int e = 0; e < vertexCount; e++)
				{
					FvVector3 inLocal( 0, 0, polyDef.m_fMaxHeight );
					inLocal.x =	*((float*&)edgePtr)++;
					inLocal.y = *((float*&)edgePtr)++;
					int adj = *((int*&)edgePtr)++;

					VertexDef vertexDef;
					FvVector3 inGlobal = tr.ApplyPoint( inLocal );
					vertexDef.m_kPosition.x = inGlobal.x;
					vertexDef.m_kPosition.y = inGlobal.y;

					if (unsigned int(adj) < 65535)
					{
						vertexDef.m_iAdjacentID = adj+1 + polyOffset;
						vertexDef.m_bAdjToAnotherZone = false;
					}
					else if (adj == 65535)
					{
						vertexDef.m_iAdjacentID = 0;
						vertexDef.m_bAdjToAnotherZone = true;
					}
					else
					{
						vertexDef.m_iAdjacentID = -~adj;
						vertexDef.m_bAdjToAnotherZone = false;
					}

					polyDef.m_kVertices.push_back( vertexDef );
					if (e == 0) inLocalFirst = inLocal;

					if(!m_bMinMaxValid)
					{
						m_kGridMin.x = vertexDef.m_kPosition.x;
						m_kGridMin.y = vertexDef.m_kPosition.y;
						m_kGridMin.z = -1000.0f;
						m_kGridMax.x = vertexDef.m_kPosition.x;
						m_kGridMax.y = vertexDef.m_kPosition.y;
						m_kGridMax.z = 1000.0f;
						m_bMinMaxValid = true;
					}
					else
					{
						if(vertexDef.m_kPosition.x < m_kGridMin.x)
							m_kGridMin.x = vertexDef.m_kPosition.x;
						if(vertexDef.m_kPosition.y < m_kGridMin.y)
							m_kGridMin.y = vertexDef.m_kPosition.y;
						if(vertexDef.m_kPosition.x > m_kGridMax.x)
							m_kGridMax.x = vertexDef.m_kPosition.x;
						if(vertexDef.m_kPosition.y > m_kGridMax.y)
							m_kGridMax.y = vertexDef.m_kPosition.y;
					}
				}

				inLocalFirst.z = polyDef.m_fMinHeight;
				polyDef.m_fMinHeight = tr.ApplyPoint( inLocalFirst ).z;
				inLocalFirst.z = polyDef.m_fMaxHeight;
				polyDef.m_fMaxHeight = tr.ApplyPoint( inLocalFirst ).z;

				m_kPolygons.push_back( polyDef );
				m_iNextID++;
			}

			setIdx++;

			dataPtr = edgePtr;
		}
	}

	return true;
}

void FvZoneView::ParseNavPoly( FvXMLSectionPtr spSection, int iSet, FvZone *pkZone )
{
	FvXMLSectionIterator it;
	PolygonDef polyDef;
	int id;
	float height;

	id = spSection->AsInt();
	height = spSection->ReadFloat("height");
	polyDef.m_fMinHeight = spSection->ReadFloat("minHeight",height);
	polyDef.m_fMaxHeight = spSection->ReadFloat("maxHeight",height);
	polyDef.m_iSet = iSet;

	if (pkZone)
	{
		FvVector3 vMin( pkZone->Centre().x, pkZone->Centre().y, polyDef.m_fMinHeight );
		FvVector3 vMax( pkZone->Centre().x, pkZone->Centre().y, polyDef.m_fMaxHeight );

		FvVector3 vMinT = pkZone->Transform().ApplyPoint( vMin );
		FvVector3 vMaxT = pkZone->Transform().ApplyPoint( vMax );

		polyDef.m_fMinHeight = vMinT.z;
		polyDef.m_fMaxHeight = vMaxT.z;
	}

	while (m_iNextID <= id)
	{
		PolygonDef blankPoly;
		blankPoly.m_fMinHeight = 0.0f;
		blankPoly.m_fMaxHeight = 0.0f;
		blankPoly.m_iSet = 0;
		m_kPolygons.push_back(blankPoly);
		m_iNextID++;
	}

	for(it = spSection->Begin(); it != spSection->End(); it++)
	{
		if((*it)->SectionName() == "vertex")
		{
			FvXMLSectionPtr pVertex = *it;
			VertexDef vertexDef;
			FvVector3 vinfo = pVertex->AsVector3();
			vertexDef.m_kPosition.x = vinfo.x;
			vertexDef.m_kPosition.y = vinfo.y;
			vertexDef.m_iAdjacentID = int( vinfo.z );

			if (pkZone)
			{
				FvVector3 v( vinfo.x, vinfo.y, polyDef.m_fMaxHeight );
				FvVector3 vT = pkZone->Transform().ApplyPoint( v );

				vertexDef.m_kPosition.x = vT.x;
				vertexDef.m_kPosition.y = vT.y;
			}

			std::string notFound( "__NOT_FOUND__" );
			std::string kAdjacentZone = pVertex->ReadString("adjacentZone",notFound);
			if ( kAdjacentZone != notFound ) 
			{
				if (vertexDef.m_iAdjacentID != 0) 
				{
					FV_WARNING_MSG( "FvZoneView::parseNavPoly: adjacentZone tag"
						" found but edge adjacency not 0.\n" );
				}
				vertexDef.m_iAdjacentID = FV_ZONE_ADJACENT_CONSTANT;
			}

			vertexDef.m_bAdjToAnotherZone = false;
			if (vertexDef.m_iAdjacentID == FV_ZONE_ADJACENT_CONSTANT) 
			{
				vertexDef.m_iAdjacentID = 0;
				vertexDef.m_bAdjToAnotherZone = true;
			}

			polyDef.m_kVertices.push_back( vertexDef );

			if(!m_bMinMaxValid)
			{
				m_kGridMin.x = vertexDef.m_kPosition.x;
				m_kGridMin.y = vertexDef.m_kPosition.y;
				m_kGridMin.z = -1000.0f;
				m_kGridMax.x = vertexDef.m_kPosition.x;
				m_kGridMax.y = vertexDef.m_kPosition.y;
				m_kGridMax.z = 1000.0f;
				m_bMinMaxValid = true;
			}
			else
			{
				if(vertexDef.m_kPosition.x < m_kGridMin.x)
					m_kGridMin.x = vertexDef.m_kPosition.x;
				if(vertexDef.m_kPosition.y < m_kGridMin.y)
					m_kGridMin.y = vertexDef.m_kPosition.y;
				if(vertexDef.m_kPosition.x > m_kGridMax.x)
					m_kGridMax.x = vertexDef.m_kPosition.x;
				if(vertexDef.m_kPosition.y > m_kGridMax.y)
					m_kGridMax.y = vertexDef.m_kPosition.y;
			}
		}
	}

	m_kPolygons[id-1] = polyDef;
}

FvUInt32 FvZoneView::GridX() const
{
	return FvUInt32( (m_kGridMax.x - m_kGridMin.x) / this->GridResolution() );
}

FvUInt32 FvZoneView::GridY() const
{
	return FvUInt32( (m_kGridMax.y - m_kGridMin.y) / this->GridResolution() );
}

FvUInt8 FvZoneView::GridMask(int iX, int iY) const
{
	return 0;
}

FvVector3 FvZoneView::GridMin() const
{
	return m_kGridMin;
}

FvVector3 FvZoneView::GridMax() const
{
	return m_kGridMax;
}

float FvZoneView::GridResolution() const
{
	return 0.5f;
}

int FvZoneView::GetPolygonCount() const
{
	return m_kPolygons.size();
}

int FvZoneView::GetBSPNodeCount() const
{
	return 0;
}

float FvZoneView::GetGirth( int set ) const
{
	return m_kSets[set].m_fGirth;
}

int FvZoneView::GetVertexCount(int iPolygon) const
{
	return m_kPolygons[iPolygon].m_kVertices.size();
}

float FvZoneView::GetMinHeight(int iPolygon) const
{
	return m_kPolygons[iPolygon].m_fMinHeight;
}

float FvZoneView::GetMaxHeight(int iPolygon) const
{
	return m_kPolygons[iPolygon].m_fMaxHeight;
}

int FvZoneView::GetSet(int iPolygon) const
{
	return m_kPolygons[iPolygon].m_iSet;
}

void FvZoneView::GetVertex(int iPolygon, int iVertex, 
	FvVector2 &kVec, int &iAdjacency, bool &bAdjToAnotherZone) const
{
	const VertexDef & vertexDef = m_kPolygons[iPolygon].m_kVertices[iVertex];

	kVec.x = (vertexDef.m_kPosition.x - m_kGridMin.x) / this->GridResolution();
	kVec.y = (vertexDef.m_kPosition.y - m_kGridMin.y) / this->GridResolution();
	iAdjacency = vertexDef.m_iAdjacentID;
	bAdjToAnotherZone = vertexDef.m_bAdjToAnotherZone;
}

void FvZoneView::SetAdjacency( int iPolygon, int iVertex, int iNewAdj )
{
	VertexDef & vertexDef = m_kPolygons[iPolygon].m_kVertices[iVertex];
	vertexDef.m_iAdjacentID = iNewAdj;
}

int FvZoneView::FindWaypoint( const FvVector3 &kInput, float fGirth ) const
{
	static FvVector3 lastPt( 0.f, 0.f, 0.f );
	static FvUInt32 nextToSkip = 0;
	if (lastPt != kInput)	{ lastPt = kInput; nextToSkip = 0; }
	int toSkip = nextToSkip++;

	FvVector2 pt(kInput.x, kInput.y);

	unsigned int i, j;
	for (i = 0; i < m_kPolygons.size(); i++)
	{
		const PolygonDef & p = m_kPolygons[i];
		if (!p.m_kVertices.size()) continue;
		if (!this->EquivGirth( i, fGirth )) continue;

		FvVector2 av, bv;
		bv = p.m_kVertices.back().m_kPosition;
		for (j = 0; j < p.m_kVertices.size(); j++)
		{
			av = bv;
			bv = p.m_kVertices[j].m_kPosition;

			FvLine leq( av, bv );
			if (leq.IsInFrontOf( pt )) break;
		}

		if (j == p.m_kVertices.size() && toSkip--==0) return i;
	}

	nextToSkip = 0;
	return -1;
}
