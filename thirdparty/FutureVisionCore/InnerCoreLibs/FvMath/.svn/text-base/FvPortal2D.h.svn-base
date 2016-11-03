//{future header message}
#ifndef __FvPortal2D_H__
#define __FvPortal2D_H__

#include "FvVector2.h"
#include <FvVectorNoDestructor.h>

class FV_MATH_API FvPortal2D
{
public:
	typedef FvVectorNoDestructor<FvVector2> FvVector2Vec;

	FvPortal2D();
	FvPortal2D(const FvPortal2D& kOther);
	~FvPortal2D();

	FvPortal2D& operator = (const FvPortal2D& kOther);

	void SetDefaults();
	void AddPoint(const FvVector2& kVec);
	void ErasePoints();

	bool Combine(FvPortal2D* pkPortal0, FvPortal2D* pkPortal1);

	bool Contains(const FvVector2& kVec) const;
	const FvVector2Vec& Points() const;

	unsigned int Refs();
	void Refs(unsigned int uiRefs);

protected:
	FvVector2Vec m_kPoints;
	unsigned int m_uiRefs;
};

#endif /* __FvPortal2D_H__ */
