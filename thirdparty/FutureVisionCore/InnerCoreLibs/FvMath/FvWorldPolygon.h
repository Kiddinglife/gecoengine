//{future header message}
#ifndef __FvWorldPolygon_H__
#define __FvWorldPolygon_H__

#include <vector>
#include "FvPlane.h"

class FV_MATH_API FvWorldPolygon
{
public:
	FvWorldPolygon();
	FvWorldPolygon(size_t stSize);

	void Split(const FvPlane& kPlane,
		FvWorldPolygon& kFrontPoly,
		FvWorldPolygon& kBackPoly) const;

	void Chop(const FvPlane& kPlane);

	std::vector<FvVector3> m_kData;
};

typedef std::vector<FvWorldPolygon> FvWorldPolygonSet;

#endif /* __FvWorldPolygon_H__ */
