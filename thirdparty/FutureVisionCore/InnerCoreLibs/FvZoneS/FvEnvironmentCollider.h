//{future header message}
#ifndef __FvEnvironmentCollider_H__
#define __FvEnvironmentCollider_H__

#include <FvVector3.h>
#include <FvSmartPointer.h>

class FvEnvironmentCollider : public FvReferenceCount
{
public:
	virtual float Ground( const FvVector3 &kPos, float fDropDistance, bool bOneSided = false ) = 0;
	virtual float Ground( const FvVector3 &kPos ) = 0;
	virtual float Collide( const FvVector3 &kStart, const FvVector3 &kEnd, class FvWorldTriangle &kResult ) = 0;

	static float NO_GROUND_COLLISION;
};

typedef FvSmartPointer<FvEnvironmentCollider> FvEnvironmentColliderPtr;

#endif // __FvEnvironmentCollider_H__