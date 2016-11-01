//{future header message}
#include "FvWorldTriangle.h"
#include "FvPlane.h"

#define FV_ISECT(VV0,VV1,VV2,D0,D1,D2,isect0,afIsect1)						\
	isect0=VV0+(VV1-VV0)*D0/(D0-D1);										\
	afIsect1=VV0+(VV2-VV0)*D0/(D0-D2);

#define FV_COMPUTE_INTERVALS(VV0,VV1,VV2,D0,D1,D2,D0D1,D0D2,isect0,afIsect1)	\
	if(D0D1>0.0f)															\
	{																		\
		FV_ISECT(VV2,VV0,VV1,D2,D0,D1,isect0,afIsect1);						\
	}																		\
	else if(D0D2>0.0f)														\
	{																		\
		FV_ISECT(VV1,VV0,VV2,D1,D0,D2,isect0,afIsect1);						\
	}																		\
	else if(D1*D2>0.0f || D0!=0.0f)											\
	{																		\
		FV_ISECT(VV0,VV1,VV2,D0,D1,D2,isect0,afIsect1);						\
	}																		\
	else if(D1!=0.0f)														\
	{																		\
		FV_ISECT(VV1,VV0,VV2,D1,D0,D2,isect0,afIsect1);						\
	}																		\
	else if(D2!=0.0f)														\
	{																		\
		FV_ISECT(VV2,VV0,VV1,D2,D0,D1,isect0,afIsect1);						\
	}																		\
	else																	\
	{																		\
		return false;														\
	}

#define FV_TRIANGLE_EPSILON (0.000001f)

//----------------------------------------------------------------------------
template <class T>
static void Sort(T& a, T& b)
{
	if(a > b)
	{
		T temp = a;
		a = b;
		b = temp;
	}
}
//----------------------------------------------------------------------------
static bool Inside(const FvVector2& kVecA, const FvVector2& kVecB)
{
	return (kVecA.x * kVecB.y - kVecA.y * kVecB.x) >= 0;
}
//----------------------------------------------------------------------------
FvWorldTriangle::FvWorldTriangle(const FvVector3& kVec0,
	const FvVector3& kVec1, const FvVector3& kVec2, Flags usFlags) :
	m_usFlags(usFlags), m_usPadding(0)
{
	m_akPoint[0] = kVec0;
	m_akPoint[1] = kVec1;
	m_akPoint[2] = kVec2;
}
//----------------------------------------------------------------------------
FvWorldTriangle::FvWorldTriangle() : m_usFlags(0), m_usPadding(0)
{
	m_akPoint[0] = FvVector3::ZERO;
	m_akPoint[1] = FvVector3::ZERO;
	m_akPoint[2] = FvVector3::ZERO;
}
//----------------------------------------------------------------------------
bool FvWorldTriangle::Intersects(const FvWorldTriangle& kTriangle) const
{
	const FvWorldTriangle& kTriA = *this;
	const FvWorldTriangle& kTriB = kTriangle;

	FvPlane kPlaneA(kTriA.Point0(), kTriA.Point1(), kTriA.Point2(), false);

	float fDistToB0 = kPlaneA.DistanceTo(kTriB.Point0());
	float fDistToB1 = kPlaneA.DistanceTo(kTriB.Point1());
	float fDistToB2 = kPlaneA.DistanceTo(kTriB.Point2());

	float fDB0DB1 = fDistToB0 * fDistToB1;
	float fDB0DB2 = fDistToB0 * fDistToB2;

	if(fDB0DB1 > 0.0f && fDB0DB2 > 0.0f)
		return false;

	FvPlane kPlaneB(kTriB.Point0(), kTriB.Point1(), kTriB.Point2(), false);

	float fDistToA0 = kPlaneB.DistanceTo(kTriA.Point0());
	float fDistToA1 = kPlaneB.DistanceTo(kTriA.Point1());
	float fDistToA2 = kPlaneB.DistanceTo(kTriA.Point2());

	float fDA0DA1 = fDistToA0 * fDistToA1;
	float fDA0DA2 = fDistToA0 * fDistToA2;

	if(fDA0DA1 > 0.0f && fDA0DA2 > 0.0f)
		return false;

	FvVector3 kCrossDir(kPlaneA.Normal().CrossProduct(kPlaneB.Normal()));

	float fMax = FvFabsf(kCrossDir.x);
	int iIndex = FV_X_AXIS;

	float fTemp = FvFabsf(kCrossDir.y);

	if(fTemp > fMax)
	{
		fMax = fTemp;
		iIndex = FV_Y_AXIS;
	}

	fTemp = FvFabsf(kCrossDir.z);

	if(fTemp > fMax)
	{
		iIndex = FV_Z_AXIS;
	}

	FvVector3 kProjA(
		kTriA.Point0()[iIndex],
		kTriA.Point1()[iIndex],
		kTriA.Point2()[iIndex]);

	FvVector3 kProjB(
		kTriB.Point0()[iIndex],
		kTriB.Point1()[iIndex],
		kTriB.Point2()[iIndex]);

	float afIsect1[2], afIsect2[2];

	FV_COMPUTE_INTERVALS(kProjA.x, kProjA.y, kProjA.z,
		fDistToA0, fDistToA1,fDistToA2,
		fDA0DA1, fDA0DA2,
		afIsect1[0], afIsect1[1]);

	FV_COMPUTE_INTERVALS(kProjB.x, kProjB.y, kProjB.z,
		fDistToB0, fDistToB1,fDistToB2,
		fDB0DB1, fDB0DB2,
		afIsect2[0], afIsect2[1]);

	Sort(afIsect1[0],afIsect1[1]);
	Sort(afIsect2[0],afIsect2[1]);

	return(afIsect1[1] >= afIsect2[0] && afIsect2[1] >= afIsect1[0]);
}
//----------------------------------------------------------------------------
bool FvWorldTriangle::Intersects(const FvVector3& kStart,
	const FvVector3& kDir, float& fLength) const
{
	const FvVector3 kEdge0(Point1() - Point0());
	const FvVector3 kEdge1(Point2() - Point0());

	const FvVector3 p(kDir.CrossProduct(kEdge1));

	float fDet = kEdge0.DotProduct(p);

	if(FvAlmostZero(fDet, FV_TRIANGLE_EPSILON))
	{
		return false;
	}

	float fInvDet = 1.0f / fDet;

	const FvVector3 t(kStart - Point0());

	float u = t.DotProduct(p) * fInvDet;

	if(u < 0.f || 1.f < u)
	{
		return false;
	}

	const FvVector3 q(t.CrossProduct(kEdge0));

	float v = kDir.DotProduct(q) * fInvDet;

	if(v < 0.f || 1.f < u + v)
	{
		return false;
	}

	float fDistanceToPlane = kEdge1.DotProduct(q) * fInvDet;

	if(0.f < fDistanceToPlane && fDistanceToPlane < fLength)
	{
		fLength = fDistanceToPlane;
		return true;
	}
	else
	{
		return false;
	}
}
//----------------------------------------------------------------------------
bool FvWorldTriangle::Intersects(const FvWorldTriangle& kTriangle,
	const FvVector3& kOffset) const
{
	FvPlane kPlane(m_akPoint[0], m_akPoint[1], m_akPoint[2], false);

	float fNormalDotOffset = kPlane.Normal().DotProduct(kOffset);
	if(FvAlmostZero(fNormalDotOffset, 0.0001f))
	{ 
		return false;
	}

	float fInvNormalDotOffset = 1.0f / fNormalDotOffset;

	float vd0 = kPlane.IntersectRayHalfNoCheck(kTriangle.m_akPoint[0],
		fInvNormalDotOffset);
	float vd1 = kPlane.IntersectRayHalfNoCheck(kTriangle.m_akPoint[1],
		fInvNormalDotOffset);
	float vd2 = kPlane.IntersectRayHalfNoCheck(kTriangle.m_akPoint[2],
		fInvNormalDotOffset);

	if(vd0 <  0.0f && vd1 <  0.0f && vd2 <  0.0f)
	{
		return false;
	}
	if(vd0 >= 1.0f && vd1 >= 1.0f && vd2 >= 1.0f)
	{
		return false;
	}

	const FvVector3& kNormal = kPlane.Normal();

	float fMax = FvFabsf(kNormal.x);
	int iIndexU = FV_Y_AXIS;
	int iIndexV = FV_Z_AXIS;

	float fTemp = FvFabsf(kNormal.y);
	if(fTemp > fMax)
	{
		fMax = fTemp; iIndexU = FV_X_AXIS;
		iIndexV = FV_Z_AXIS;
	}

	if(FvFabsf(kNormal.z) > fMax)
	{
		iIndexU = FV_X_AXIS;
		iIndexV = FV_Y_AXIS;
	}

	FvVector3 kTriB3D[3] =
	{
		kTriangle.m_akPoint[0] + kOffset * vd0,
		kTriangle.m_akPoint[1] + kOffset * vd1,
		kTriangle.m_akPoint[2] + kOffset * vd2
	};

	FvVector2 kTriA[3] =
	{
		FvVector2(m_akPoint[0][iIndexU], m_akPoint[0][iIndexV]),
		FvVector2(m_akPoint[1][iIndexU], m_akPoint[1][iIndexV]),
		FvVector2(m_akPoint[2][iIndexU], m_akPoint[2][iIndexV])
	};

	FvVector2 kTriB[3] =
	{
		FvVector2(kTriB3D[0][iIndexU], kTriB3D[0][iIndexV]),
		FvVector2(kTriB3D[1][iIndexU], kTriB3D[1][iIndexV]),
		FvVector2(kTriB3D[2][iIndexU], kTriB3D[2][iIndexV])
	};

	bool bSignA = Inside(kTriA[1] - kTriA[0], kTriA[2] - kTriA[0]);
	bool bSignB = Inside(kTriB[1] - kTriB[0], kTriB[2] - kTriB[0]);
				
	FvVector2 kSegmentA, kSegmentB;

	for(int i1 = 0, i2 = 2; i1 <= 2; i2 = i1, i1++)
	{
		kSegmentA = kTriA[i1] - kTriA[i2];
		if(Inside(kSegmentA, kTriB[0] - kTriA[i2]) != bSignA &&
			Inside(kSegmentA, kTriB[1] - kTriA[i2]) != bSignA &&
			Inside(kSegmentA, kTriB[2] - kTriA[i2]) != bSignA)
		{
			return false;
		}

		kSegmentB = kTriB[i1] - kTriB[i2];
		if(Inside(kSegmentB, kTriA[0] - kTriB[i2]) != bSignB &&
			Inside(kSegmentB, kTriA[1] - kTriB[i2]) != bSignB &&
			Inside(kSegmentB, kTriA[2] - kTriB[i2]) != bSignB)
		{
			return false;
		}
	}
	
	return true;
}
//----------------------------------------------------------------------------
FvVector2 FvWorldTriangle::Project(const FvVector3& kOnTriangle) const
{
	FvVector2 vs(m_akPoint[1][0] - m_akPoint[0][0],
		m_akPoint[1][2]	- m_akPoint[0][2]);
	FvVector2 vt(m_akPoint[2][0] - m_akPoint[0][0],
		m_akPoint[2][2]	- m_akPoint[0][2]);
	FvVector2 vp(kOnTriangle[0] - m_akPoint[0][0],
		kOnTriangle[2] - m_akPoint[0][2]);

	float sXt = vs[0]*vt[1] - vs[1]*vt[0];
	float ls = (vp[0]*vt[1] - vp[1]*vt[0]) / sXt;
	float lt = (vp[1]*vs[0] - vp[0]*vs[1]) / sXt;

	return FvVector2(ls, lt);
}
//----------------------------------------------------------------------------
const FvVector3& FvWorldTriangle::Point0() const
{
	return Point(0);
}
//----------------------------------------------------------------------------
const FvVector3& FvWorldTriangle::Point1() const
{
	return Point(1);
}
//----------------------------------------------------------------------------
const FvVector3& FvWorldTriangle::Point2() const
{
	return Point(2);
}
//----------------------------------------------------------------------------
const FvVector3& FvWorldTriangle::Point(FvUInt32 uiIndex) const
{
	return m_akPoint[uiIndex];
}
//----------------------------------------------------------------------------
FvVector3 FvWorldTriangle::Normal() const
{
	return (m_akPoint[2] - m_akPoint[0]).CrossProduct(
		m_akPoint[2] - m_akPoint[1]);
}
//----------------------------------------------------------------------------
void FvWorldTriangle::Bounce(FvVector3& kVec, float fElasticity) const
{
	FvVector3 kNormal = Normal();
	kNormal.Normalise();

	float fProj = kNormal.DotProduct(kVec);
	kVec -= (1 + fElasticity) * fProj * kNormal;
}
//----------------------------------------------------------------------------
FvWorldTriangle::Flags FvWorldTriangle::PackFlags(int iCollisionFlags,
	int iMaterialKind)
{
	return iCollisionFlags | (
		(iMaterialKind << FV_TRIANGLE_MATERIALKIND_SHIFT) & 
		FV_TRIANGLE_MATERIALKIND_MASK);
}
//----------------------------------------------------------------------------
int FvWorldTriangle::CollisionFlags(Flags usFlags)
{
	return (usFlags & FV_TRIANGLE_COLLISIONFLAG_MASK);
}
//----------------------------------------------------------------------------
int FvWorldTriangle::MaterialKind(Flags usFlags)
{
	return (usFlags & FV_TRIANGLE_MATERIALKIND_MASK) >>
		FV_TRIANGLE_MATERIALKIND_SHIFT;
}
//----------------------------------------------------------------------------
FvWorldTriangle::Flags FvWorldTriangle::GetFlags() const
{
	return m_usFlags;
}
//----------------------------------------------------------------------------
void FvWorldTriangle::SetFlags(Flags usNewFlags)
{
	m_usFlags = usNewFlags;
}
//----------------------------------------------------------------------------
bool FvWorldTriangle::IsTransparent() const
{
	return (m_usFlags & FV_TRIANGLE_TRANSPARENT) != 0;
}
//----------------------------------------------------------------------------
bool FvWorldTriangle::IsBlended() const
{
	return (m_usFlags & FV_TRIANGLE_BLENDED) != 0;
}
//----------------------------------------------------------------------------
FvUInt32 FvWorldTriangle::CollisionFlags() const
{
	return CollisionFlags(m_usFlags);
}
//----------------------------------------------------------------------------
FvUInt32 FvWorldTriangle::MaterialKind() const
{
	return MaterialKind(m_usFlags);
}
//----------------------------------------------------------------------------
bool FvWorldTriangle::operator == (const FvWorldTriangle& kTriangle) const
{
	return m_akPoint[0] == kTriangle.m_akPoint[0] &&
		m_akPoint[1] == kTriangle.m_akPoint[1] &&
		m_akPoint[2] == kTriangle.m_akPoint[2] &&
		m_usFlags == kTriangle.m_usFlags &&
		m_usPadding == kTriangle.m_usPadding;
}
//----------------------------------------------------------------------------
