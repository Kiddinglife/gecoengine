//{future header message}
#ifndef __FvBoundingBox_H__
#define __FvBoundingBox_H__

#include "FvMatrix.h"

class FV_MATH_API FvBoundingBox
{
public:
	FvBoundingBox();
	FvBoundingBox(const FvVector3& kMin, const FvVector3& kMax);

	bool operator == (const FvBoundingBox& kBoundingBox) const;
	bool operator != (const FvBoundingBox& kBoundingBox) const;

	const FvVector3& MinBounds() const;
	const FvVector3& MaxBounds() const;
	void SetBounds(const FvVector3& kMin, const FvVector3& kMax);

	float Width() const;
	float Height() const;
	float Depth() const;

	void AddZBounds(float fZ);
	void AddBounds(const FvVector3& kVec);
	void AddBounds(const FvBoundingBox& kBoundingBox);
	void ExpandSymmetrically(float dx, float dy, float dz);
	void ExpandSymmetrically(const FvVector3& kVec);
	void CalculateOutcode(const FvMatrix& kMat) const;

	FvOutcode Outcode() const;
	void Outcode(FvOutcode eOutcode);
	FvOutcode CombinedOutcode() const;
	void CombinedOutcode(FvOutcode eOutcode);

	void TransformBy(const FvMatrix& kTransform);

	bool Intersects(const FvBoundingBox& kBox) const;
	bool Intersects(const FvVector3& kVec) const;
	bool Intersects(const FvVector3& kVec, float fBias) const;
	bool IntersectsRay(const FvVector3& kOrigin, const FvVector3& kDir) const;
	bool IntersectsLine(const FvVector3& kOrigin, const FvVector3& kDest) const;

	bool Clip(FvVector3& kSource, FvVector3& kExtent, float fBloat = 0.f) const;

	bool Include(const FvBoundingBox& kBox) const;

	float Distance(const FvVector3& kPoint) const;
	FvVector3 Centre() const;
	FvVector3 HalfSize() const;
	bool InsideOut() const;

	static const FvBoundingBox ms_kInsideOut;

protected:
	FvVector3 m_kMin;
	FvVector3 m_kMax;	

	mutable DWORD m_dwOutcode;
	mutable DWORD m_dwCombinedOutcode;
};

#endif /* __FvBoundingBox_H__ */
