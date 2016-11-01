#include "FvAdjacentZoneSet.h"

#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvNavigator", 0 )

FvAdjacentZoneSet::FvAdjacentZoneSet()
{
}

//bool FvAdjacentZoneSet::read(DataSectionPtr pChunkDir, const FvZoneID &kZoneID)
//{
//	DataSection::iterator kZoneIter;
//	DataSection::iterator boundaryIter;
//	DataSectionPtr pkZone, pBoundary, pPortal, pAdjChunk;
//	FvString name;
//	
//	pkZone = pChunkDir->openSection(kZoneID + ".chunk");
//
//	if(!pkZone)
//	{
//		FV_ERROR_MSG("Failed to open chunk %s\n", kZoneID.c_str());
//		return false;
//	}
//
//	m_kStartZone = kZoneID;
//	this->AddZone(kZoneID);
//	this->ReadZone(pkZone);
//
//	for(kZoneIter = pkZone->begin();
//		kZoneIter != pkZone->end();
//		kZoneIter++)
//	{
//		pBoundary = *kZoneIter;
//
//		if(pBoundary->sectionName() != "boundary")
//			continue;
//
//		for(boundaryIter = pBoundary->begin();
//			boundaryIter != pBoundary->end();
//			boundaryIter++)
//		{
//			pPortal = *boundaryIter;
//
//			if(pPortal->sectionName() != "portal")
//				continue;
//
//			FvString name = pPortal->readString("chunk");
//
//			if (name == "heaven" || name == "earth" || name == "")
//				continue;
//
//			pAdjChunk = pChunkDir->openSection(name + ".chunk");
//
//			if (!pAdjChunk)
//			{
//				FV_ERROR_MSG("Failed to open adjacent chunk %s\n", name.c_str());
//			}
//			else if(!this->HasZone(name))
//			{
//				this->AddZone(name);
//				this->ReadZone(pAdjChunk);
//			}
//		}
//	}
//
//	return true;
//}

const FvZoneID& FvAdjacentZoneSet::StartZone() const
{
	return m_kStartZone;
}

bool FvAdjacentZoneSet::HasZone(const FvZoneID &kZoneID) const
{
	unsigned int i;

	for(i = 0; i < m_kZones.size(); i++)
	{
		if(m_kZones[i].m_kZoneID == kZoneID)
			return true;
	}

	return false;
}

void FvAdjacentZoneSet::AddZone(const FvZoneID &kZoneID)
{
	ZoneDef kZoneDef;
	kZoneDef.m_kZoneID = kZoneID;
	m_kZones.push_back(kZoneDef);
}

void FvAdjacentZoneSet::AddPlane(const FvPlane &kPlane)
{
	m_kZones.back().m_kPlanes.push_back(kPlane);
}

bool FvAdjacentZoneSet::Test(const FvVector3 &kPosition, FvZoneID &kZoneID) const
{	
	for (unsigned int i = 0; i < m_kZones.size(); i++)
	{
		if (m_kZones[i].m_kZoneID[8] == 'i' && TestZone(kPosition, i))
		{
			kZoneID = m_kZones[i].m_kZoneID;
			return true;
		}
	}

	for (unsigned int i = 0; i < m_kZones.size(); i++)
	{
		if (m_kZones[i].m_kZoneID[8] != 'i' && TestZone(kPosition, i))
		{
			kZoneID = m_kZones[i].m_kZoneID;
			return true;
		}
	}

	return false;
}

bool FvAdjacentZoneSet::ReadZone(FvXMLSectionPtr spAdjZone)
{
	FvXMLSectionIterator kZoneIter;
	FvXMLSectionPtr pBoundary;
	FvVector3 normal;
	FvMatrix transform;
	float d;

	for(kZoneIter = spAdjZone->Begin();
		kZoneIter != spAdjZone->End();
		kZoneIter++)
	{
		pBoundary = *kZoneIter;

		transform = spAdjZone->ReadMatrix34("transform");

		if(pBoundary->SectionName() != "boundary")
			continue;

		if(pBoundary->ReadBool("portal/internal"))
		{
			continue;
		}

		if(pBoundary->ReadString("portal/zone") == "heaven" ||
		   pBoundary->ReadString("portal/zone") == "earth" ||
		   pBoundary->ReadString("portal/zone") == "extern")
		{
			continue;
		}

		normal = pBoundary->ReadVector3("normal");
		d = pBoundary->ReadFloat("d");

		FvVector3 ndtr = transform.ApplyPoint(normal * d);
		FvVector3 ntr = transform.ApplyVector(normal);
		FvPlane plane(ntr, ntr.DotProduct(ndtr));

		this->AddPlane(plane);
	}

	return true;
}

bool FvAdjacentZoneSet::TestZone(const FvVector3 &kPosition, int iIndex) const
{
	const ZoneDef &kZoneDef = m_kZones[iIndex];

	for(unsigned int p = 0; p < kZoneDef.m_kPlanes.size(); p++)
	{
		if(!kZoneDef.m_kPlanes[p].IsInFrontOf(kPosition))
		{
			return false;
		}
	}

	return true;
}
