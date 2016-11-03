//{future header message}
#ifndef __FvWorldTriangle_H__
#define __FvWorldTriangle_H__

#include <vector>
#include <FvBaseTypes.h>
#include "FvVector3.h"

enum FV_TRIANGLE_FLAG
{
	FV_TRIANGLE_NOT_IN_BSP				= 0xff,
	FV_TRIANGLE_CAMERANOCOLLIDE			= 1 << 0,
	FV_TRIANGLE_TRANSPARENT				= 1 << 1,
	FV_TRIANGLE_BLENDED					= 1 << 2,
	FV_TRIANGLE_TERRAIN					= 1 << 3,
	FV_TRIANGLE_NOCOLLIDE				= 1 << 4,
	FV_TRIANGLE_DOUBLESIDED				= 1 << 5,
	FV_TRIANGLE_DOOR					= 1 << 6,
	FV_TRIANGLE_UNUSED_7				= 1 << 7,
	FV_TRIANGLE_COLLISIONFLAG_MASK		= 0xff,
	FV_TRIANGLE_MATERIALKIND_MASK		= 0xff << 8,
	FV_TRIANGLE_MATERIALKIND_SHIFT		= 8
};

class FV_MATH_API FvWorldTriangle
{
public:
	typedef FvUInt16 Flags;
	typedef FvUInt16 Padding;

	FvWorldTriangle(const FvVector3& kVec0, const FvVector3& kVec1,
		const FvVector3& kVec2, Flags usFlags = 0);
	FvWorldTriangle();

	bool Intersects(const FvWorldTriangle& kTriangle) const;

	bool Intersects(const FvVector3& kStart, const FvVector3& kDir,
		float& fLength) const;

	bool Intersects(const FvWorldTriangle& kTriangle,
		const FvVector3& kOffset) const;

	FvVector2 Project(const FvVector3& kOnTriangle) const;

	const FvVector3& Point0() const;
	const FvVector3& Point1() const;
	const FvVector3& Point2() const;
	const FvVector3& Point(FvUInt32 uiIndex) const;

	FvVector3 Normal() const;

	void Bounce(FvVector3& kVec, float fElasticity = 1.0f) const;

	static Flags PackFlags(int iCollisionFlags, int iMaterialKind);
	
	static int CollisionFlags(Flags usFlags);
	static int MaterialKind(Flags usFlags);

	Flags GetFlags() const;
	void SetFlags(Flags usNewFlags);

	bool IsTransparent() const;
	bool IsBlended() const;

	FvUInt32 CollisionFlags() const;
	FvUInt32 MaterialKind() const;

	bool operator == (const FvWorldTriangle& kTriangle) const;

private:
	FvVector3 m_akPoint[3];
	Flags m_usFlags;
	Padding m_usPadding;
};

typedef std::vector<const FvWorldTriangle*> FvWorldTriangleSet;
typedef std::vector<FvWorldTriangle> FvRealWorldTriangleSet;

#endif /* __FvWorldTriangle_H__ */
