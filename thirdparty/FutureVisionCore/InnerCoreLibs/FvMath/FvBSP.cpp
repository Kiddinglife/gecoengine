//{future header message}
#include "FvBSP.h"
#include "FvVectorNoDestructor.h"

const int FvBSP::MAX_SIZE = 10;
const float FvBSP::TOLERANCE = 0.01f;

//----------------------------------------------------------------------------
template <class T>
static T Min(T a, T b, T c)
{
	return a < b ?
		(a < c ? a : c) :
		(b < c ? b : c);
}

//----------------------------------------------------------------------------
template <class T>
static T Max(T a, T b, T c)
{
	return a > b ?
		(a > c ? a : c) :
		(b > c ? b : c);
}
//----------------------------------------------------------------------------
struct FvBSPStackNode
{
	FvBSPStackNode();
	FvBSPStackNode(const FvBSP* pkBSP, int eBack, float sDist, float eDist) :
		m_pkNode(pkBSP), m_iBack(eBack), m_fSDist(sDist), m_fEDist(eDist)
	{

	}

	const FvBSP* m_pkNode;
	int m_iBack;
	float m_fSDist;
	float m_fEDist;
};
//----------------------------------------------------------------------------
class FvNullCollisionVisitor : public FvCollisionVisitor
{
public:
	virtual bool Visit(const FvWorldTriangle& kHitTriangle, float fDist)
	{
		return true;
	}
};

static FvNullCollisionVisitor s_kNullCollisionVisitor;
//----------------------------------------------------------------------------
class FvBSPAllocator
{
public:
	FvBSPAllocator(char* pkMem) : m_pkNodeMemory(pkMem)
	{

	}

	FvBSP* NewBSP()
	{
		FvBSP* pkBSP;

		if(m_pkNodeMemory)
		{
			pkBSP = new (m_pkNodeMemory) FvBSP();
			m_pkNodeMemory += sizeof(FvBSP);
		}
		else
		{
			pkBSP = new FvBSP();
		}

		return pkBSP;
	}

	void Destroy(FvBSP* pkBSP)
	{
		FvBSP* pkCur = pkBSP;

		while(pkCur)
		{
			if(pkCur->m_pkBack)
				Destroy(pkCur->m_pkBack);

			FvBSP* pkNext = pkCur->m_pkFront;

			if(m_pkNodeMemory)
				pkCur->~FvBSP();
			else
				delete pkCur;
			pkCur = pkNext;
		}
	}
private:
	char* m_pkNodeMemory;
};
//----------------------------------------------------------------------------
class FvBSPConstructor
{
private:
	struct Element
	{
		FvBSP* m_pkBSP;
		FvWorldTriangleSet m_kTriangles;
		FvWorldPolygonSet m_kPolygons;
	};

public:
	FvBSPConstructor(FvBSPAllocator& kAllocator) : m_kAllocator(kAllocator)
	{

	}

	FvBSP* Construct(FvWorldTriangleSet& kTriangles)
	{
		FvWorldPolygonSet kPolygons(kTriangles.size());
		FvBSP* pkResult = AddToPending(kTriangles, kPolygons);

		while(!m_kStack.empty())
		{
			Element& kElement = m_kStack.back();
			FvWorldPolygonSet kPolygons;
			kPolygons.swap(kElement.m_kPolygons);
			FvWorldTriangleSet kTriangles;
			kTriangles.swap(kElement.m_kTriangles);
			FvBSP * pkCurBSP = kElement.m_pkBSP;
			m_kStack.pop_back();
			pkCurBSP->Partition(kTriangles, kPolygons, *this);
		}

		return pkResult;
	}

	FvBSP* AddToPending(FvWorldTriangleSet& kTriangles,
		FvWorldPolygonSet& kPolygons)
	{
		m_kStack.push_back(Element());
		Element& kElement = m_kStack.back();
		FvBSP* pkBSP = m_kAllocator.NewBSP();
		kElement.m_pkBSP = pkBSP;
		kElement.m_kTriangles.swap(kTriangles);
		kElement.m_kPolygons.swap(kPolygons);
		return pkBSP;
	}

private:
	typedef std::vector<Element> Stack;
	Stack m_kStack;
	FvBSPAllocator m_kAllocator;
};

//----------------------------------------------------------------------------
FvBSP::FvBSP() : 
m_pkFront(NULL), 
m_pkBack(NULL), 
m_kPlane(),
m_bPartitioned(false)
{

}
//----------------------------------------------------------------------------
FvBSP::~FvBSP()
{

}
//----------------------------------------------------------------------------
void FvBSP::Partition(FvWorldTriangleSet& kTriangles,
					  FvWorldPolygonSet& kPolygons, FvBSPConstructor& kConstructor)
{
	FV_ASSERT(kTriangles.size() == kPolygons.size());

	if(((int)kTriangles.size()) <= MAX_SIZE)
	{
		m_kTriangles.swap(kTriangles);
		m_kPlane.Init(FvVector3(0.0f, 0.0f, 0.0f),
			FvVector3(1.0f, 0.0f, 0.0f));
		return;
	}

	int iBestFront = kTriangles.size();
	int iBestBack = kTriangles.size();
	int iBestBoth = kTriangles.size();
	int iBestOn = kTriangles.size();

	const int MAX_TESTS = 15;

	for(FvUInt32 i = 0;
		i < FvUInt32(MAX_TESTS) && i < kTriangles.size(); i++)
	{
		FvPlane kOld = m_kPlane;

		int iRandIndex = (int(kTriangles.size()) < MAX_TESTS)
			? i : FvRand() % kTriangles.size();

		FV_ASSERT(0 <= iRandIndex && iRandIndex <
			int(kTriangles.size()));

		m_kPlane.Init(kTriangles[iRandIndex]->Point0(),
			kTriangles[iRandIndex]->Point1(),
			kTriangles[iRandIndex]->Point2());

		int iTempFront = 0;
		int iTempBack = 0;
		int iTempBoth = 0;
		int iTempOn = 0;

		for(FvUInt32 j = 0; j < kTriangles.size() &&
			iTempBoth < iBestBoth; j++)
		{
			const FvWorldTriangle* pkTriangle = kTriangles[j];
			const FvWorldPolygon& kPoly = kPolygons[j];

			FvBSPSide eSide =
				kPoly.m_kData.empty() ?	WhichSide(pkTriangle) :
				WhichSide(kPoly);

			switch(eSide)
			{
			case FV_BSP_BOTH:
				iTempBoth++;
				break;
			case FV_BSP_FRONT:
				iTempFront++;
				break;
			case FV_BSP_BACK:
				iTempBack++;
				break;
			case FV_BSP_ON:
				iTempOn++;
				break;
			}
		}

		if(iTempBoth < iBestBoth)
		{
			iBestBoth  = iTempBoth;
			iBestFront = iTempFront;
			iBestBack  = iTempBack;
			iBestOn    = iTempOn;
		}
		else
		{
			m_kPlane = kOld;
		}
	}

	m_bPartitioned = true;

	FvWorldTriangleSet kFrontSet;
	FvWorldPolygonSet kFrontPolys;

	FvWorldTriangleSet kBackSet;
	FvWorldPolygonSet kBackPolys;

	kFrontSet.reserve(iBestFront + iBestBoth);
	kFrontPolys.reserve(iBestFront + iBestBoth);
	kBackSet.reserve(iBestBack + iBestBoth);
	kBackPolys.reserve(iBestBack + iBestBoth);
	m_kTriangles.reserve(iBestOn);

	for(FvUInt32 iIndex = 0; iIndex < kTriangles.size(); iIndex++)
	{
		const FvWorldTriangle* pkTriangle = kTriangles[iIndex];
		const FvWorldPolygon& kPoly = kPolygons[iIndex];

		FvBSPSide eSide =
			kPoly.m_kData.empty() ? WhichSide(pkTriangle) : WhichSide(kPoly);

		switch(eSide)
		{
		case FV_BSP_ON:
			m_kTriangles.push_back(pkTriangle);
			break;

		case FV_BSP_FRONT:
			kFrontSet.push_back(pkTriangle);
			kFrontPolys.push_back(kPoly);
			break;

		case FV_BSP_BACK:
			kBackSet.push_back(pkTriangle);
			kBackPolys.push_back(kPoly);
			break;

		case FV_BSP_BOTH:
			kFrontSet.push_back(pkTriangle);
			kFrontPolys.push_back(FvWorldPolygon());

			kBackSet.push_back(pkTriangle);
			kBackPolys.push_back(FvWorldPolygon());

			if(!kPoly.m_kData.empty())
			{
				kPoly.Split(m_kPlane, kFrontPolys.back(),
					kBackPolys.back());
			}
			else
			{
				FvWorldPolygon tempPoly(3);
				tempPoly.m_kData[0] = pkTriangle->Point0();
				tempPoly.m_kData[1] = pkTriangle->Point1();
				tempPoly.m_kData[2] = pkTriangle->Point2();

				tempPoly.Split(m_kPlane, kFrontPolys.back(),
					kBackPolys.back());
			}
			break;
		}
	}

	if(!kFrontSet.empty())
	{
		m_pkFront = kConstructor.AddToPending(kFrontSet, kFrontPolys);
	}

	if(!kBackSet.empty())
	{
		m_pkBack = kConstructor.AddToPending(kBackSet, kBackPolys);
	}
}
//----------------------------------------------------------------------------
FvBSPSide FvBSP::WhichSide(const FvWorldPolygon& kPoly) const
{
	FvBSPSide eSide = FV_BSP_ON;

	std::vector<FvVector3>::const_iterator iter = kPoly.m_kData.begin();

	while(iter != kPoly.m_kData.end())
	{
		float fDist = m_kPlane.DistanceTo(*iter);

		if(fDist > TOLERANCE)
		{
			eSide = (FvBSPSide)(eSide | 0x1);
		}
		else if(fDist < -TOLERANCE)
		{
			eSide = (FvBSPSide)(eSide | 0x2);
		}

		iter++;
	}

	return eSide;
}
//----------------------------------------------------------------------------
FvBSPSide FvBSP::WhichSide(const FvWorldTriangle* pkTriangle) const
{
	FvBSPSide eSide = FV_BSP_ON;

	float fDist0 = m_kPlane.DistanceTo(pkTriangle->Point0());
	float fDist1 = m_kPlane.DistanceTo(pkTriangle->Point1());
	float fDist2 = m_kPlane.DistanceTo(pkTriangle->Point2());

	if(fDist0 > TOLERANCE || fDist1 > TOLERANCE || fDist2 > TOLERANCE)
	{
		eSide = (FvBSPSide)(eSide | 0x1);
	}

	if(fDist0 < -TOLERANCE || fDist1 < -TOLERANCE || fDist2 < -TOLERANCE)
	{
		eSide = (FvBSPSide)(eSide | 0x2);
	}

	return eSide;
}
//----------------------------------------------------------------------------
bool FvBSP::Intersects(const FvWorldTriangle& kTriangle,
					   const FvWorldTriangle** ppkHitTriangle) const
{
	bool bIntersects = false;

	if (!m_bPartitioned)
	{
		bIntersects = IntersectsThisNode(kTriangle,
			ppkHitTriangle);
	}
	else
	{
		float d0 = m_kPlane.DistanceTo(kTriangle.Point0());
		float d1 = m_kPlane.DistanceTo(kTriangle.Point1());
		float d2 = m_kPlane.DistanceTo(kTriangle.Point2());

		float fMin = ::Min(d0, d1, d2);
		float fMax = ::Max(d0, d1, d2);

		if(d0 < 0.f)
		{
			if(m_pkBack && fMin < TOLERANCE)
			{
				bIntersects = m_pkBack->Intersects(kTriangle,
					ppkHitTriangle);
			}

			if(fMin < TOLERANCE && fMax > -TOLERANCE && !bIntersects)
			{
				bIntersects = IntersectsThisNode(kTriangle,
					ppkHitTriangle);
			}

			if(m_pkFront && fMax > -TOLERANCE && !bIntersects)
			{
				bIntersects = m_pkFront->Intersects(kTriangle,
					ppkHitTriangle);
			}
		}
		else
		{
			if(m_pkFront && fMax > -TOLERANCE)
			{
				bIntersects = m_pkFront->Intersects(kTriangle,
					ppkHitTriangle);
			}

			if(fMin < TOLERANCE && fMax > -TOLERANCE && !bIntersects)
			{
				bIntersects = IntersectsThisNode(kTriangle,
					ppkHitTriangle);
			}

			if(m_pkBack && fMin < TOLERANCE && !bIntersects)
			{
				bIntersects = m_pkBack->Intersects(kTriangle,
					ppkHitTriangle);
			}
		}
	}

	return bIntersects;
}
//----------------------------------------------------------------------------
bool FvBSP::IntersectsThisNode(const FvWorldTriangle& kTriangle,
							   const FvWorldTriangle** ppkHitTriangle) const
{
	bool bIntersects = false;


	FvWorldTriangleSet::const_iterator iter = m_kTriangles.begin();

	while(iter != m_kTriangles.end() && !bIntersects)
	{
		if((*iter)->CollisionFlags() != FV_TRIANGLE_NOT_IN_BSP)
		{
			if((*iter)->Intersects(kTriangle))
			{
				bIntersects = true;

				if(ppkHitTriangle != NULL)
				{
					*ppkHitTriangle = (*iter);
				}
			}
		}

		iter++;
	}

	return bIntersects;
}
//----------------------------------------------------------------------------
bool FvBSP::Intersects( const FvVector3& kStart, const FvVector3& kEnd,
					   float& fDist, const FvWorldTriangle** ppkHitTriangle,
					   FvCollisionVisitor * pkVisitor) const
{
	const FvWorldTriangle* pkHitTriangle = NULL;

	if(ppkHitTriangle == NULL)
		ppkHitTriangle = &pkHitTriangle;

	if(pkVisitor == NULL)
		pkVisitor = &s_kNullCollisionVisitor;

	float fOrigDist = fDist;
	const FvWorldTriangle* pkOrigHT = *ppkHitTriangle;

	FvVector3 kDelta = kEnd - kStart;
	float fTolerancePct = TOLERANCE / kDelta.Length();

	static FvVectorNoDestructor<FvBSPStackNode> kStack;
	kStack.clear();
	kStack.push_back(FvBSPStackNode(this, -1, 0, 1));

	while(!kStack.empty())
	{
		FvBSPStackNode kCur = kStack.back();
		kStack.pop_back();

		float sDist = kCur.m_fSDist;
		float eDist = kCur.m_fEDist;

		float iDist = 0.f;
		const FvPlane& kPlane = kCur.m_pkNode->m_kPlane;

		int sBack, eBack;

		if(!kCur.m_pkNode->m_bPartitioned)
		{
			sBack = -2;
			eBack = -2;
		}
		else if(kCur.m_iBack == -1)
		{
			float sOut = kPlane.DistanceTo(kStart + kDelta *
				(sDist - fTolerancePct));
			float eOut = kPlane.DistanceTo(kStart + kDelta *
				(eDist + fTolerancePct));
			sBack = int(sOut < 0.f);
			eBack = int(eOut < 0.f);

			FvBSP* pkStartSide = (&kCur.m_pkNode->m_pkFront)[sBack];

			if(sBack == eBack)
			{
				if(FvFabsf(sOut) < TOLERANCE ||
					FvFabsf(eOut) < TOLERANCE)
				{
					kStack.push_back(FvBSPStackNode(
						kCur.m_pkNode, -2, sDist, eDist));
				}
				if (pkStartSide != NULL)
				{
					kStack.push_back(FvBSPStackNode(
						pkStartSide, -1, sDist, eDist));
				}
				continue;
			}

			iDist = kPlane.IntersectRayHalf(
				kStart, kPlane.Normal().DotProduct(kDelta));

			if(pkStartSide != NULL)
			{
				kStack.push_back(FvBSPStackNode(
					kCur.m_pkNode, eBack, sDist, eDist));

				kStack.push_back(FvBSPStackNode(
					pkStartSide, -1, sDist, iDist));

				continue;
			}
		}
		else
		{
			sBack = kCur.m_iBack;
			eBack = kCur.m_iBack;

			iDist = kPlane.IntersectRayHalf(
				kStart, kPlane.Normal().DotProduct(kDelta));
		}

		if(kCur.m_pkNode->IntersectsThisNode(
			kStart, kEnd, fDist, ppkHitTriangle, pkVisitor) &&
			fDist <= (kCur.m_fEDist + fTolerancePct))
		{
			return true;
		}

		fDist = fOrigDist;
		*ppkHitTriangle = pkOrigHT;

		if(eBack >= 0)
		{
			FvBSP* pEndSide = (&kCur.m_pkNode->m_pkFront)[eBack];
			if (pEndSide != NULL) kStack.push_back(FvBSPStackNode(
				pEndSide, -1, iDist, eDist));
		}
	}

	return false;
}
//----------------------------------------------------------------------------
bool FvBSP::Intersects(const FvWorldTriangle& kTriangle,
					   const FvVector3& kTranslation, FvCollisionVisitor* pkVisitor) const
{
	if(!m_bPartitioned)
	{
		return IntersectsThisNode(kTriangle, kTranslation, pkVisitor);
	}

	float dA0 = m_kPlane.DistanceTo(kTriangle.Point0());
	float dA1 = m_kPlane.DistanceTo(kTriangle.Point1());
	float dA2 = m_kPlane.DistanceTo(kTriangle.Point2());
	float dB0 = m_kPlane.DistanceTo(kTriangle.Point0() + kTranslation);
	float dB1 = m_kPlane.DistanceTo(kTriangle.Point1() + kTranslation);
	float dB2 = m_kPlane.DistanceTo(kTriangle.Point2() + kTranslation);

	float fMin = min(Min(dA0, dA1, dA2), Min(dB0, dB1, dB2));
	float fMax = max(Max(dA0, dA1, dA2), Max(dB0, dB1, dB2));

	if(fMin < TOLERANCE && fMax > -TOLERANCE)
	{
		if(IntersectsThisNode(kTriangle, kTranslation, pkVisitor))
			return true;
	}

	if(m_pkBack && fMin < TOLERANCE)
	{
		if(m_pkBack->Intersects(kTriangle, kTranslation, pkVisitor))
			return true;
	}

	if(m_pkFront && fMax > -TOLERANCE)
	{
		if(m_pkFront->Intersects(kTriangle, kTranslation, pkVisitor))
			return true;
	}

	return false;
}
//----------------------------------------------------------------------------
class FvBSPFile
{
public:
	FvBSPFile(char *pkData,size_t stSize) :
	  m_pkData(pkData),
	  m_iSize(stSize)
	  {
	  }

	  ~FvBSPFile()
	  {
		  if (m_iSize != 0)
		  {
			  FV_ERROR_MSG( "FvBSPFile::~FvBSPFile: Size is not 0. (%d)\n", m_iSize );
		  }
	  }

	  int Read( void *pkDest, int iElementSize, int iNumElements )
	  {
		  const int iReadSize = iElementSize * iNumElements;
		  m_iSize -= iReadSize;

		  if (m_iSize >= 0)
		  {
			  memcpy(pkDest, m_pkData, iReadSize);
			  m_pkData += iReadSize;
			  return iReadSize;
		  }
		  else
		  {
			  return 0;
		  }
	  }

	  int Size() const
	  {
		  return m_iSize;
	  }

	  int Close()
	  {
		  m_iSize = 0;
		  return 0;
	  }

private:
	const char *m_pkData;
	int m_iSize;
};

namespace
{

	inline bool IsValidPlane( const FvPlane &kPlane )
	{
		float fNormLen = kPlane.Normal().Length();

		return 0.9f < fNormLen && fNormLen < 1.1f;
	}

} 

static const FvUInt8 FV_BSP_IS_PARTITIONED	= 0x01;
static const FvUInt8 FV_BSP_HAS_FRONT		= 0x02;
static const FvUInt8 FV_BSP_HAS_BACK		= 0x04;
static const FvUInt8 FV_BSP_MAGIC			= 0xa0;
static const FvUInt8 FV_BSP_MAGIC_MASK		= 0xf8;

//----------------------------------------------------------------------------
bool FvBSP::Load(const FvBSPTree &kTree, FvBSPFile &kBSPFile,
			   FvBSPAllocator &kAllocator)
{
	FvUInt8 uiFlags;
	if (!kBSPFile.Read( &uiFlags, 1, 1 ))
	{
		return false;
	}

	if ((uiFlags & FV_BSP_MAGIC_MASK) != FV_BSP_MAGIC)
	{
		FV_ERROR_MSG("FvBSP::Load: Bad magic mask 0x%x.\n", (int)uiFlags);
		return false;
	}

	m_bPartitioned = (uiFlags & FV_BSP_IS_PARTITIONED) ? 1 : 0;

	if (!kBSPFile.Read( &m_kPlane, sizeof( m_kPlane ), 1 ))
	{
		return false;
	}

	FvUInt16 uiNumTris;

	if (!kBSPFile.Read( &uiNumTris, sizeof( FvUInt16 ), 1 ))
	{
		return false;
	}

	if (!kTree.LoadTrianglesForNode( kBSPFile, *this, uiNumTris ))
	{
		return false;
	}

	if (m_bPartitioned && !IsValidPlane( m_kPlane ))
	{
		FV_ERROR_MSG( "FvBSP::Load: Bad plane equation: n = (%f, %f, %f). d = %f\n",
			m_kPlane.Normal().x, m_kPlane.Normal().y, m_kPlane.Normal().z,
			m_kPlane.Distance() );
		return false;
	}

	if (uiFlags & FV_BSP_HAS_FRONT)
	{
		m_pkFront = kAllocator.NewBSP();
		if (!m_pkFront->Load( kTree, kBSPFile, kAllocator ))
		{
			kAllocator.Destroy( m_pkFront );
			m_pkFront = NULL;
			return false;
		}
	}

	if (uiFlags & FV_BSP_HAS_BACK)
	{
		m_pkBack = kAllocator.NewBSP();
		if (!m_pkBack->Load( kTree, kBSPFile, kAllocator ))
		{
			kAllocator.Destroy( m_pkBack );
			m_pkBack = NULL;
			return false;
		}
	}

	return true;
}
//----------------------------------------------------------------------------
void FvBSP::GetNumNodes(int& iNumNodes, int& iMaxTriangles) const
{
	iNumNodes++;
	if(m_kTriangles.size() > size_t(iMaxTriangles))
	{
		iMaxTriangles = m_kTriangles.size();
	}

	if(m_pkFront)
	{
		m_pkFront->GetNumNodes(iNumNodes, iMaxTriangles);
	}

	if(m_pkBack)
	{
		m_pkBack->GetNumNodes(iNumNodes, iMaxTriangles);
	}
}
//----------------------------------------------------------------------------
void FvBSP::Dump( int iDepth )
{
	if (m_kTriangles.size() > 20)
	{
		FvString kSpaces(iDepth,' ');
		FV_INFO_MSG( "%ssize = %d. n = (%f, %f, %f). d = %f len = %f\n",
			kSpaces.c_str(), (int)m_kTriangles.size(),
			m_kPlane.Normal().x,
			m_kPlane.Normal().y,
			m_kPlane.Normal().z,
			m_kPlane.Distance(),
			m_kPlane.Normal().Length() );
	}

	if (m_pkFront != NULL)
	{
		m_pkFront->Dump(iDepth + 1);
	}

	if (m_pkBack != NULL)
	{
		m_pkBack->Dump(iDepth + 1);
	}
}
//----------------------------------------------------------------------------
FvUInt32 FvBSP::Size() const
{
	FvUInt32 uiSizeBSP = sizeof(FvBSP);
	uiSizeBSP += m_kTriangles.capacity() * sizeof(m_kTriangles.front());
	return uiSizeBSP;
}
//----------------------------------------------------------------------------
bool FvBSP::IntersectsThisNode(const FvVector3& kStart,
							   const FvVector3& kEnd, float& fDist,
							   const FvWorldTriangle** ppkHitTriangle,
							   FvCollisionVisitor* pkVisitor) const
{
	bool bIntersects = false;

	FvWorldTriangleSet::const_iterator iter = m_kTriangles.begin();
	const FvVector3 kDirection(kEnd - kStart);

	while(iter != m_kTriangles.end())
	{
		if((*iter)->CollisionFlags() != FV_TRIANGLE_NOT_IN_BSP)
		{
			float fOriginalDist = fDist;

			if((*iter)->Intersects(kStart, kDirection, fDist) &&
				(!pkVisitor || pkVisitor->Visit(**iter, fDist)))
			{
				bIntersects |= true;

				if(ppkHitTriangle != NULL)
				{
					*ppkHitTriangle = (*iter);
				}
			}
			else
			{
				fDist = fOriginalDist;
			}
		}

		iter++;
	}

	return bIntersects;
}
//----------------------------------------------------------------------------
bool FvBSP::IntersectsThisNode(const FvWorldTriangle& kTriangle,
							   const FvVector3& kTranslation, FvCollisionVisitor* pkVisitor) const
{
	for(FvWorldTriangleSet::const_iterator iter = m_kTriangles.begin();
		iter != m_kTriangles.end(); iter++)
	{
		if((*iter)->CollisionFlags() != FV_TRIANGLE_NOT_IN_BSP)
		{
			if((*iter)->Intersects(kTriangle, kTranslation))
			{
				if(pkVisitor == NULL || pkVisitor->Visit(**iter, 0.0f))
					return true;
			}
		}
	}

	return false;
}

const FvUInt8 FV_BSP_FILE_VERSION = 0;
const FvUInt32 FV_BSP_FILE_MAGIC = 0x505342;
const FvUInt32 FV_BSP_FILE_TOKEN = FV_BSP_FILE_MAGIC | (FV_BSP_FILE_VERSION << 24);

//----------------------------------------------------------------------------
FvBSPTree::FvBSPTree(FvRealWorldTriangleSet& kTriangles) : m_pkRoot(NULL),
	m_pusIndices(NULL), m_iIndicesSize(0), m_pcNodeMemory(NULL)
{
	m_kTriangles.swap(kTriangles);
	int iSize = m_kTriangles.size();
	FvWorldTriangleSet kTris(iSize);

	for(int i = 0; i < iSize; i++)
	{
		kTris[i] = &m_kTriangles[i];
	}

	FvBSPAllocator kAllocator(m_pcNodeMemory);
	FvBSPConstructor kConstructor(kAllocator);
	m_pkRoot = kConstructor.Construct(kTris);
}
//----------------------------------------------------------------------------
FvBSPTree::FvBSPTree() : m_pkRoot(NULL), m_pusIndices(NULL),
	m_iIndicesSize(0), m_pcNodeMemory(NULL)
{

}
//----------------------------------------------------------------------------
FvBSPTree::~FvBSPTree()
{
	FvBSPAllocator kAllocator(m_pcNodeMemory);

	if(m_pkRoot)
		kAllocator.Destroy(m_pkRoot);
	if(m_pcNodeMemory != NULL)
	{
		delete [] m_pcNodeMemory;
	}
}
//----------------------------------------------------------------------------
bool FvBSPTree::Load(char *pkData,size_t stSize)
{
	FV_IF_NOT_ASSERT_DEV( sizeof( FvWorldTriangle ) == 40 )
	{
		FV_EXIT( "sizeof( FvWorldTriangle ) must be 40!" );
	}

	bool bResult = false;

	if (m_pkRoot == NULL)
	{
		FvBSPFile kBSPFile(pkData,stSize);

		struct
		{
			FvUInt32 m_uiMagic;
			FvInt32 m_iNumTriangles;
			FvInt32 m_iNumNodes;
			FvInt32 m_iMaxTriangles;
		} kHeader;

		if (!kBSPFile.Read( &kHeader, sizeof(kHeader), 1 ))
		{
			kBSPFile.Close();
			return false;
		}

		if (kHeader.m_uiMagic != FV_BSP_FILE_TOKEN)
		{
			if ((kHeader.m_uiMagic & 0xffffff) == FV_BSP_FILE_MAGIC)
			{
				FV_ERROR_MSG( "FvBSPTree::Load: "
					"Bad version. Expected %d. Got %u.\n",
					FV_BSP_FILE_VERSION, kHeader.m_uiMagic >> 24 );
			}
			else
			{
				FV_ERROR_MSG( "FvBSPTree::Load: Bad magic\n" );
			}
			kBSPFile.Close();
			return false;
		}

		m_kTriangles.resize( kHeader.m_iNumTriangles );

		kBSPFile.Read( m_kTriangles.empty() ? NULL : &m_kTriangles.front(),
		sizeof( FvWorldTriangle ), kHeader.m_iNumTriangles );

		m_pusIndices = new FvUInt16[ kHeader.m_iMaxTriangles ];
		m_iIndicesSize = kHeader.m_iMaxTriangles;

		m_pcNodeMemory = new char[ kHeader.m_iNumNodes * sizeof(FvBSP) ];

		FvBSPAllocator kAllocator( m_pcNodeMemory );

		m_pkRoot = kAllocator.NewBSP();
		bResult = m_pkRoot->Load( *this, kBSPFile, kAllocator );

		delete [] m_pusIndices;
		m_pusIndices = NULL;
		m_iIndicesSize = 0;

		while (kBSPFile.Size() > 0 && bResult)
		{
			FV_IF_NOT_ASSERT_DEV( unsigned( kBSPFile.Size() ) >=
				sizeof( UserDataKey ) + sizeof( int ) )
			{
				return false;
			}

			UserDataKey eType;
			kBSPFile.Read(&eType, sizeof(UserDataKey), 1);

			int iSize = 0;
			kBSPFile.Read(&iSize, sizeof(int), 1);

			FV_IF_NOT_ASSERT_DEV(kBSPFile.Size() >= iSize)
			{
				return false;
			}
			UserDataPtr spUserData(new UserData(iSize));
			kBSPFile.Read(spUserData->Data(), sizeof(char), iSize);
			this->SetUserData(eType, spUserData);
		}

		kBSPFile.Close();

		this->GenerateBoundingBox();

		if (!bResult)
		{
			FV_ERROR_MSG( "FvBSPTree::Load: Loading failed.\n" );
		}
	}
	else
	{
		FV_ERROR_MSG( "FvBSPTree::Load: Already been initialised.\n" );
		bResult = false;
	}

	return bResult;
}
//----------------------------------------------------------------------------
bool FvBSPTree::LoadTrianglesForNode(FvBSPFile &kBSPFile,
								   FvBSP &kNode, int iNumTriangles) const
{
	if (iNumTriangles > m_iIndicesSize)
	{
		FV_ERROR_MSG( "FvBSPTree::LoadTrianglesForNode: "
			"Wanted to load %d but can only handle %d\n", iNumTriangles, m_iIndicesSize );
		return false;
	}

	if (iNumTriangles == 0)
		return true;

	if (!kBSPFile.Read(m_pusIndices, sizeof(FvUInt16), iNumTriangles))
	{
		FV_ERROR_MSG( "FvBSPTree::LoadTrianglesForNode: Failed to read %d indices.\n",
			iNumTriangles );
		return false;
	}

	FvWorldTriangleSet &kNodeTris = kNode.m_kTriangles;
	kNodeTris.resize( iNumTriangles );
	const int iMaxSize = m_kTriangles.size();

	for (int i = 0; i < iNumTriangles; i++)
	{
		FvUInt16 uiIndex = m_pusIndices[i];
		if (uiIndex >= iMaxSize)
		{
			FV_ERROR_MSG( "FvBSPTree::LoadTrianglesForNode: "
				"Index too big %d >= %d.\n", uiIndex, iMaxSize );
			return false;
		}

		kNodeTris[i] = &m_kTriangles[m_pusIndices[i]];
	}

	return true;
}
//----------------------------------------------------------------------------
void FvBSPTree::RemapFlags(FvBSPFlagsMap& kFlagsMap)
{
	FvRealWorldTriangleSet::iterator iter = m_kTriangles.begin();

	while(iter != m_kTriangles.end())
	{
		FvWorldTriangle& kTriangle = *iter++;
		FvUInt32 idx = (FvUInt32)kTriangle.GetFlags();
		kTriangle.SetFlags(kFlagsMap[idx % kFlagsMap.size()]);
	}
}
//----------------------------------------------------------------------------
const FvBSP* FvBSPTree::GetRoot() const
{
	return m_pkRoot;
}
//----------------------------------------------------------------------------
FvUInt32 FvBSPTree::Size() const
{
	FvUInt32 sz = sizeof(FvBSPTree);
	sz += m_kTriangles.capacity() * sizeof(m_kTriangles.front());
	return sz;
}
//----------------------------------------------------------------------------
bool FvBSPTree::Empty() const
{
	return m_kTriangles.empty();
}
//----------------------------------------------------------------------------
void FvBSPTree::GenerateBoundingBox()
{
	m_kBoundingBox = FvBoundingBox::ms_kInsideOut;

	FvRealWorldTriangleSet::const_iterator it;
	for(it = m_kTriangles.begin(); it != m_kTriangles.end(); it++)
	{
		const FvWorldTriangle& kTri = *it;

		m_kBoundingBox.AddBounds(kTri.Point0());
		m_kBoundingBox.AddBounds(kTri.Point1());
		m_kBoundingBox.AddBounds(kTri.Point2());
	}
}
//----------------------------------------------------------------------------
const FvBoundingBox& FvBSPTree::GetBoundingBox()
{
	return m_kBoundingBox;
}
//----------------------------------------------------------------------------
const FvRealWorldTriangleSet& FvBSPTree::Triangles() const
{
	return m_kTriangles;
}
//----------------------------------------------------------------------------
FvBSPTree::UserDataPtr FvBSPTree::GetUserData(UserDataKey eType)
{
	UserDataMap::const_iterator kIt = this->m_kUserData.find(eType);
	return kIt != this->m_kUserData.end() ? kIt->second : UserDataPtr(NULL);
}
//----------------------------------------------------------------------------
void FvBSPTree::SetUserData(UserDataKey eType, UserDataPtr spData)
{
	this->m_kUserData.insert(std::make_pair(eType, spData));
}
//----------------------------------------------------------------------------
bool FvBSPTree::CanCollide() const
{
	for(FvRealWorldTriangleSet::const_iterator it = m_kTriangles.begin();
		it != m_kTriangles.end(); it++)
	{
		if((*it).CollisionFlags() != FV_TRIANGLE_NOT_IN_BSP)
		{
			return true;
		}
	}
	return false;
}
//----------------------------------------------------------------------------
