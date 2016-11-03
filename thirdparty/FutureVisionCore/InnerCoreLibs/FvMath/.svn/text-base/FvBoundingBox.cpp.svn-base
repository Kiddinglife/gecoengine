//{future source message}
#include "FvBoundingBox.h"
#include <float.h>
#include "FvBaseTypes.h"

#include <FvDebug.h>

const FvBoundingBox FvBoundingBox::ms_kInsideOut;
//----------------------------------------------------------------------------
FvBoundingBox::FvBoundingBox() : m_kMin(FLT_MAX, FLT_MAX, FLT_MAX),
	m_kMax(-FLT_MAX, -FLT_MAX, -FLT_MAX), m_dwOutcode(FV_OUTCODE_NULL),
	m_dwCombinedOutcode(FV_OUTCODE_NULL)
{
	
}
//----------------------------------------------------------------------------
FvBoundingBox::FvBoundingBox(const FvVector3& kMin, const FvVector3& kMax) :
	m_kMin(kMin), m_kMax(kMax), m_dwOutcode(FV_OUTCODE_NULL),
	m_dwCombinedOutcode(FV_OUTCODE_NULL)
{

}
//----------------------------------------------------------------------------
bool FvBoundingBox::operator == (const FvBoundingBox& kBoundingBox) const
{
	return (m_kMin == kBoundingBox.m_kMin) &&
		(m_kMax == kBoundingBox.m_kMax);
}
//----------------------------------------------------------------------------
bool FvBoundingBox::operator != (const FvBoundingBox& kBoundingBox) const
{
	return (m_kMin != kBoundingBox.m_kMin) ||
		(m_kMax != kBoundingBox.m_kMax);
}
//----------------------------------------------------------------------------
const FvVector3& FvBoundingBox::MinBounds() const
{
	FV_ASSERT(!InsideOut());
	return m_kMin;
}
//----------------------------------------------------------------------------
const FvVector3& FvBoundingBox::MaxBounds() const
{
	FV_ASSERT(!InsideOut());
	return m_kMax;
}
//----------------------------------------------------------------------------
void FvBoundingBox::SetBounds(const FvVector3& kMin, const FvVector3& kMax)
{
	m_kMin = kMin;
	m_kMax = kMax;
}
//----------------------------------------------------------------------------
float FvBoundingBox::Width() const
{
	if(InsideOut())
		return 0.0f;
	else
		return m_kMax.x - m_kMin.x;
}
//----------------------------------------------------------------------------
float FvBoundingBox::Height() const
{
	if(InsideOut())
		return 0.0f;
	else
		return m_kMax.y - m_kMin.y;
}
//----------------------------------------------------------------------------
float FvBoundingBox::Depth() const
{
	if(InsideOut())
		return 0.0f;
	else
		return m_kMax.z - m_kMin.z;
}
//----------------------------------------------------------------------------
void FvBoundingBox::AddZBounds(float fZ)
{
	if(m_kMin[2] > fZ)
		m_kMin[2] = fZ;
	if(m_kMax[2] < fZ)
		m_kMax[2] = fZ;
}
//----------------------------------------------------------------------------
void FvBoundingBox::AddBounds(const FvVector3& kVec)
{
	if(InsideOut())
	{
		m_kMin = kVec;
		m_kMax = kVec;
		return;
	}

	if(m_kMin[0] > kVec[0])
		m_kMin[0] = kVec[0];

	if(m_kMin[1] > kVec[1])
		m_kMin[1] = kVec[1];

	if(m_kMin[2] > kVec[2])
		m_kMin[2] = kVec[2];

	if(m_kMax[0] < kVec[0])
		m_kMax[0] = kVec[0];

	if(m_kMax[1] < kVec[1])
		m_kMax[1] = kVec[1];

	if(m_kMax[2] < kVec[2])
		m_kMax[2] = kVec[2];
}
//----------------------------------------------------------------------------
void FvBoundingBox::AddBounds(const FvBoundingBox& kBoundingBox)
{
	if(kBoundingBox.InsideOut())
		return;

	if(InsideOut())
	{
		m_kMin = kBoundingBox.m_kMin;
		m_kMax = kBoundingBox.m_kMax;
		return;
	}

	if (m_kMin[0] > kBoundingBox.m_kMin[0])
		m_kMin[0] = kBoundingBox.m_kMin[0];

	if (m_kMin[1] > kBoundingBox.m_kMin[1])
		m_kMin[1] = kBoundingBox.m_kMin[1];

	if (m_kMin[2] > kBoundingBox.m_kMin[2])
		m_kMin[2] = kBoundingBox.m_kMin[2];

	if (m_kMax[0] < kBoundingBox.m_kMax[0])
		m_kMax[0] = kBoundingBox.m_kMax[0];

	if (m_kMax[1] < kBoundingBox.m_kMax[1])
		m_kMax[1] = kBoundingBox.m_kMax[1];

	if (m_kMax[2] < kBoundingBox.m_kMax[2])
		m_kMax[2] = kBoundingBox.m_kMax[2];
}
//----------------------------------------------------------------------------
void FvBoundingBox::ExpandSymmetrically(float dx, float dy, float dz)
{
	if (!InsideOut())
	{
		m_kMin.x -= dx; m_kMin.y -= dy; m_kMin.z -= dz;
		m_kMax.x += dx; m_kMax.y += dy; m_kMax.z += dz;
	}
}
//----------------------------------------------------------------------------
void FvBoundingBox::ExpandSymmetrically(const FvVector3& kVec)
{
	ExpandSymmetrically(kVec.x, kVec.y, kVec.z);
}
//----------------------------------------------------------------------------
void FvBoundingBox::CalculateOutcode(const FvMatrix& kMat) const
{
	m_dwOutcode = FV_OUTCODE_NULL;
	m_dwCombinedOutcode = FV_OUTCODE_MASK;

	FvVector4 vx[2];
	FvVector4 vy[2];
	FvVector4 vz[2];

	vx[0] = kMat.Row(0);
	vx[1] = vx[0];
	vx[0] *= m_kMin[0];
	vx[1] *= m_kMax[0];

	vy[0] = kMat.Row(1);
	vy[1] = vy[0];
	vy[0] *= m_kMin[1];
	vy[1] *= m_kMax[1];

	vz[0] = kMat.Row(2);
	vz[1] = vz[0];
	vz[0] *= m_kMin[2];
	vz[1] *= m_kMax[2];

	const FvVector4& vw = kMat.Row(3);

	for(FvUInt32 i(0); i < 8; i++)
	{
		FvVector4 v = vw;
		v += vx[i & 1];
		v += vy[(i >> 1) & 1];
		v += vz[(i >> 2) & 1];

		FV_OUT_CODE eOutcode = v.CalculateOutcode();

		m_dwCombinedOutcode &= eOutcode;

		m_dwOutcode |= eOutcode;
	}
}
//----------------------------------------------------------------------------
FvOutcode FvBoundingBox::Outcode() const
{
	return (FvOutcode)m_dwOutcode;
}
//----------------------------------------------------------------------------
void FvBoundingBox::Outcode(FvOutcode eOutcode)
{
	m_dwOutcode = eOutcode;
}
//----------------------------------------------------------------------------
FvOutcode FvBoundingBox::CombinedOutcode() const
{
	return (FvOutcode)m_dwCombinedOutcode;
}
//----------------------------------------------------------------------------
void FvBoundingBox::CombinedOutcode(FvOutcode eOutcode)
{
	m_dwCombinedOutcode = eOutcode;
}
//----------------------------------------------------------------------------
void FvBoundingBox::TransformBy( const FvMatrix& kTransform )
{
	FV_ASSERT(!InsideOut());

	const FvVector3 kSize = MaxBounds() - MinBounds();

	m_kMin = kTransform.ApplyPoint(m_kMin);
	m_kMax = m_kMin;

	for(int iAxis = FV_X_AXIS; iAxis <= FV_Z_AXIS; iAxis++)
	{
		const FvVector3 kTransformedAxis =
			kSize[iAxis] * kTransform.ApplyToUnitAxisVector(iAxis);

		for(int iResultDirection = FV_X_COORD;
			iResultDirection <= FV_Z_COORD;
			iResultDirection++)
		{
			if (kTransformedAxis[iResultDirection] > 0)
			{
				m_kMax[iResultDirection] +=
					kTransformedAxis[iResultDirection];
			}
			else
			{
				m_kMin[iResultDirection] +=
					kTransformedAxis[iResultDirection];
			}
		}
	}
}
//----------------------------------------------------------------------------
bool IntervalsIntersect(float min1, float max1, float min2, float max2)
{
	return min2 <= max1 && min1 <= max2;
}
//----------------------------------------------------------------------------
bool FvBoundingBox::Intersects(const FvBoundingBox& kBox) const
{
	const FvVector3& min1 = m_kMin;
	const FvVector3& max1 = m_kMax;

	const FvVector3& min2 = kBox.m_kMin;
	const FvVector3& max2 = kBox.m_kMax;

	return(
		IntervalsIntersect(min1[0], max1[0], min2[0], max2[0]) &&
		IntervalsIntersect(min1[1], max1[1], min2[1], max2[1]) &&
		IntervalsIntersect(min1[2], max1[2], min2[2], max2[2]));
}
//----------------------------------------------------------------------------
bool FvBoundingBox::Intersects(const FvVector3& kVec) const
{
	return (kVec[0] >= m_kMin[0]) && (kVec[1] >= m_kMin[1]) &&
		(kVec[2] >= m_kMin[2]) && (kVec[0] < m_kMax[0]) &&
		(kVec[1] < m_kMax[1]) && (kVec[2] < m_kMax[2]);
}
//----------------------------------------------------------------------------
bool FvBoundingBox::Intersects(const FvVector3& kVec, float fBias) const
{
	return (kVec[0] >= (m_kMin[0] - fBias)) &&
		(kVec[1] >= (m_kMin[1] - fBias)) &&
		(kVec[2] >= (m_kMin[2] - fBias)) &&
		(kVec[0] < (m_kMax[0] + fBias)) &&
		(kVec[1] < (m_kMax[1] + fBias)) && (kVec[2] < (m_kMax[2] + fBias));
}
//----------------------------------------------------------------------------
bool FvBoundingBox::IntersectsRay(const FvVector3& kOrigin,
	const FvVector3& kDir) const
{
	FvVector3 akBounds[2] = {MinBounds(), MaxBounds()};
	FvVector3 kPointOnPlane;

	for (int iBound = 0; iBound < 2; iBound++)
	{
		for (int iAxis = FV_X_AXIS; iAxis <= FV_Z_AXIS; iAxis++)
		{
			if (kDir[iAxis] != 0.f)
			{
				float t = (akBounds[iBound][iAxis] - kOrigin[iAxis]) /
					kDir[iAxis];

				if (t > 0.f)
				{
					kPointOnPlane = kOrigin + t * kDir;

					int iNextAxis = (iAxis + 1) % 3;
					int iPrevAxis = (iAxis + 2) % 3;

					if (m_kMin[iNextAxis] < kPointOnPlane[iNextAxis] &&
						kPointOnPlane[iNextAxis] < m_kMax[iNextAxis] &&
						m_kMin[iPrevAxis] < kPointOnPlane[iPrevAxis] &&
						kPointOnPlane[iPrevAxis] < m_kMax[iPrevAxis])
					{
						return true;
					}

				}
			}
		}
	}

	return false;
}
//----------------------------------------------------------------------------
bool FvBoundingBox::IntersectsLine(const FvVector3& kOrigin,
	const FvVector3& kDest) const
{
	const FvVector3 kDirection = kDest - kOrigin;
	FvVector3 akBounds[2] = {MinBounds(), MaxBounds()};
	FvVector3 kPointOnPlane;

	for (int iBound = 0; iBound < 2; iBound++)
	{
		for (int iAxis = FV_X_AXIS; iAxis <= FV_Z_AXIS; iAxis++)
		{
			if (kDirection[iAxis] != 0.0f)
			{
				float t = (akBounds[iBound][iAxis] - kOrigin[iAxis]) /
					kDirection[iAxis];

				if (t > 0.0f && t < 1.0f)
				{
					kPointOnPlane = kOrigin + t * kDirection;

					int iNextAxis = (iAxis + 1) % 3;
					int iPrevAxis = (iAxis + 2) % 3;

					if (m_kMin[iNextAxis] < kPointOnPlane[iNextAxis] &&
						kPointOnPlane[iNextAxis] < m_kMax[iNextAxis] &&
						m_kMin[iPrevAxis] < kPointOnPlane[iPrevAxis] &&
						kPointOnPlane[iPrevAxis] < m_kMax[iPrevAxis])
					{
						return true;
					}

				}
			}
		}
	}

	return false;
}
//----------------------------------------------------------------------------
bool FvBoundingBox::Clip(FvVector3& kSource, FvVector3& kExtent,
	float fBloat) const
{
	FvVector3	mt;
	FvVector3	Mt;

	float sMin = 0.0f;
	float eMax = 1.0f;

	FvVector3 kDelta = kExtent - kSource;

	if (kDelta.x != 0.f)
	{
		mt.x = (m_kMin.x - fBloat - kSource.x) / kDelta.x;
		Mt.x = (m_kMax.x + fBloat - kSource.x) / kDelta.x;

		if (kDelta.x > 0.f)
		{
			if (mt.x > sMin) sMin = mt.x;
			if (Mt.x < eMax) eMax = Mt.x;
		}
		else
		{
			if (Mt.x > sMin) sMin = Mt.x;
			if (mt.x < eMax) eMax = mt.x;
		}
	}
	else if (kSource.x < m_kMin.x - fBloat || kSource.x >= m_kMax.x + fBloat)
		return false;

	if (kDelta.y != 0.f)
	{
		mt.y = (m_kMin.y - fBloat - kSource.y) / kDelta.y;
		Mt.y = (m_kMax.y + fBloat - kSource.y) / kDelta.y;
		if (kDelta.y > 0.f)
		{
			if (mt.y > sMin) sMin = mt.y;
			if (Mt.y < eMax) eMax = Mt.y;
		}
		else
		{
			if (Mt.y > sMin) sMin = Mt.y;
			if (mt.y < eMax) eMax = mt.y;
		}
	}
	else if (kSource.y < m_kMin.y - fBloat || kSource.y >= m_kMax.y + fBloat)
		return false;

	if (kDelta.z != 0.f)
	{
		mt.z = (m_kMin.z - fBloat - kSource.z) / kDelta.z;
		Mt.z = (m_kMax.z + fBloat - kSource.z) / kDelta.z;
		if (kDelta.z > 0.f)
		{
			if (mt.z > sMin) sMin = mt.z;
			if (Mt.z < eMax) eMax = Mt.z;
		}
		else
		{
			if (Mt.z > sMin) sMin = Mt.z;
			if (mt.z < eMax) eMax = mt.z;
		}
	}
	else if (kSource.z < m_kMin.z - fBloat || kSource.z >= m_kMax.z + fBloat)
		return false;

	if (sMin >= eMax) return false;

	kExtent = kSource + eMax * kDelta;
	kSource = kSource + sMin * kDelta;

	return true;
}
//----------------------------------------------------------------------------
float FvBoundingBox::Distance(const FvVector3& kPoint) const
{
	FvVector3 p(0,0,0);

	if (kPoint.x > m_kMax.x)
		p.x = kPoint.x - m_kMax.x;
	else if (kPoint.x < m_kMin.x)
		p.x = kPoint.x - m_kMin.x;

	if (kPoint.y > m_kMax.y)
		p.y = kPoint.y - m_kMax.y;
	else if (kPoint.y < m_kMin.y)
		p.y = kPoint.y - m_kMin.y;

	if (kPoint.z > m_kMax.z)
		p.z = kPoint.z - m_kMax.z;
	else if (kPoint.z < m_kMin.z)
		p.z = kPoint.z - m_kMin.z;

	return p.Length();
}
//----------------------------------------------------------------------------
FvVector3 FvBoundingBox::Centre() const
{
	FV_ASSERT(!InsideOut());

	return (m_kMin + m_kMax) * 0.5f;
}
//----------------------------------------------------------------------------
FvVector3 FvBoundingBox::HalfSize() const
{
	FV_ASSERT(!InsideOut());

	return (m_kMax - m_kMin) * 0.5f;
}
//----------------------------------------------------------------------------
bool FvBoundingBox::InsideOut() const
{
	return m_kMin.x > m_kMax.x || m_kMin.y > m_kMax.y || m_kMin.z > m_kMax.z;
}
//----------------------------------------------------------------------------
bool FvBoundingBox::Include( const FvBoundingBox& kBox ) const
{
	return (kBox.m_kMin.x > m_kMin.x) && (kBox.m_kMin.y > m_kMin.y) && (kBox.m_kMin.z > m_kMin.z)
		&& (kBox.m_kMax.x < m_kMax.x) && (kBox.m_kMax.y < m_kMax.y) && (kBox.m_kMax.z < m_kMax.z);
}
//----------------------------------------------------------------------------
