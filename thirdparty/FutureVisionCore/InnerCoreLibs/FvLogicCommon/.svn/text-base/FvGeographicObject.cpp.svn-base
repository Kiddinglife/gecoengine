#include "FvGeographicObject.h"

#include <FvMath.h>
#include <FvAngle.h>
#include <FvDebug.h>


const FvVector3 FvGeographicObject::FRONT(FvMath2DTool::FRONT_X, FvMath2DTool::FRONT_Y, 0);
const FvVector3 FvGeographicObject::VERTICAL(0, 0, 1);

//+--------------------------------------------------------------------------------------------------------
float FvGeographicObject::GetDirection(const float fX, const float fZ)
{
	return FvMath2DTool::GetRotateAngle(FvMath2DTool::FRONT_X, FvMath2DTool::FRONT_Y, fX, fZ);
}
void FvGeographicObject::GetRotate(const float fAngle, float& fX, float& fY)
{
	FvMath2DTool::Rotate(FvMath2DTool::FRONT_X, FvMath2DTool::FRONT_Y, fAngle, fX, fY);
}
float FvGeographicObject::GetDistance(const FvVector3& kPos1, const FvVector3& kPos2)
{
	return FvMath2DTool::Distance(kPos1.x, kPos1.y, kPos2.x, kPos2.y);
}
float FvGeographicObject::GetDistance2(const FvVector3& kPos1, const FvVector3& kPos2)
{
	return FvMath2DTool::Distance2(kPos1.x, kPos1.y, kPos2.x, kPos2.y);	
	}

FvGeographicObject::FvGeographicObject()
{
}

FvGeographicObject::~FvGeographicObject(void)
{
}
//+-----------------------------------------------------------------------------------------------------------------------------------------------
float FvGeographicObject::Aim(const float x, const float y)
{
	const float fAngle = GetAngleFromLocation(x, y);
	TurnAngle(fAngle);
	return fAngle;
}
float FvGeographicObject::Aim(const FvGeographicObject& qObj)
{
	if(&qObj != this)
	{
		return Aim(qObj.Position().x, qObj.Position().y);
	}
	else
	{
		return 0.0f;
	}
}
void FvGeographicObject::TurnAngle(const float fDeltaAngle)
{
	SetYaw(Yaw()+fDeltaAngle);
}
//+-----------------------------------------------------------------------------------------------------------------------------------------------
float FvGeographicObject::GetAngleFromLocation(const float fX, const float fY)const
{
	const float fDeltaX = fX - Position().x;
	const float fDeltaY = fY - Position().y;
	return GetAngleFromDirection(fDeltaX, fDeltaY);
}
float FvGeographicObject::GetAngleFromDirection(const float fDeltaX, const float fDeltaY)const 
{
	if(fDeltaX == 0.0f && fDeltaY == 0.0f)
	{
		return 0.0f;
	}
	float fDeltaAngle = GetDirection(fDeltaX, fDeltaY) - Yaw();
	FvAngle::Normalise(fDeltaAngle);
	return fDeltaAngle;
}
bool FvGeographicObject::IsInFront(const float fX, const float fY)const
{
	const float fAngle = GetAngleFromLocation(fX, fY);
	if(-FvMath2DTool::HALF_PI < fAngle && fAngle < FvMath2DTool::HALF_PI )
	{
		return true;
	}
	else
	{
		return false;
	}
}

float FvGeographicObject::GetDistance(const float fX, const float fY)const
{
	const FvVector3& kSelfPos = Position();
	const float fDiff = FvMath2DTool::Distance(fX, fY, kSelfPos.x, kSelfPos.y);
	return FvMathTool::Max(0.0f, fDiff - BodyRadius());
}
//float FvGeographicObject::GetDistance2(const float fX, const float fY)const
//{
//	const FvVector3& kSelfPos = Position();
//	const float fDiff2 = FvMath2DTool::Distance2(fX, fY, kSelfPos.x, kSelfPos.y);
//	const float fRadius = BodyRadius();
//	return FvMathTool::Max(0.0f, fDiff2 - fRadius*fRadius);
//}
float FvGeographicObject::GetDistance(const FvVector3& kPos)const
{
	const float fDiff = FvGeographicObject::GetDistance(kPos, Position());
	return FvMathTool::Max(0.0f, fDiff - BodyRadius());
}
//float FvGeographicObject::GetDistance2(const FvVector3& kPos)const
//{
//	const float fDiff2 = FvGeographicObject::GetDistance2(kPos.x, Position());
//	const float fRadius = BodyRadius();
//	return FvMathTool::Max(0.0f, fDiff2 - fRadius*fRadius);
//}
float FvGeographicObject::GetDistance(const FvGeographicObject& kObj)const
{
	const float fDiff = FvGeographicObject::GetDistance(Position(), kObj.Position());
	return FvMathTool::Max(0.0f, fDiff - BodyRadius() - kObj.BodyRadius());
}
//float FvGeographicObject::GetDistance2(const FvGeographicObject& kObj)const
//{
//	const float fDiff2 = FvGeographicObject::GetDistance2(Position(), kObj.Position());
//	const float fSelfRadius = BodyRadius();
//	const float fOtherRadius = kObj.BodyRadius();
//	return FvMathTool::Max(0.0f, fDiff2 - fSelfRadius*fSelfRadius - fOtherRadius*fOtherRadius);
//}
FvVector3 FvGeographicObject::GetIntersectionPosByDist(const float fToX, const float fToY, const float fDistance)const
{
	return GetIntersectionPosByDist(Position(), fToX, fToY, fDistance);
}
FvVector3 FvGeographicObject::GetIntersectionPosByDist(const FvVector3& kRootPos, const float fToX, const float fToY, const float fDistance)
{
	const float fSpan = FvMath2DTool::Distance(kRootPos.x, kRootPos.y, fToX, fToY);
	if(fSpan != 0.0f)
	{
		const float fPerc = fDistance/fSpan;
		const float fDeltaX = fToX - kRootPos.x;
		const float fDeltaY = fToY - kRootPos.y;
		FvVector3 kDest(kRootPos.x + fDeltaX*fPerc, kRootPos.y + fDeltaY*fPerc, kRootPos.z);
		const float fDiff = FvMath2DTool::Distance(kRootPos.x, kRootPos.y, kDest.x, kDest.y);
		FV_ASSERT(fabs(fDiff - fabs(fDistance)) < 0.1f);
		return FvVector3(kRootPos.x + fDeltaX*fPerc, kRootPos.y + fDeltaY*fPerc, kRootPos.z);
	}
	else
	{
		return kRootPos;
	}
}
FvVector3 FvGeographicObject::GetIntersectionPosByPerc(const float fToX, const float fToY, const float fPerc)const
{
	const float fDeltaX = fToX - Position().x;
	const float fDeltaY = fToY - Position().y;
	return FvVector3(Position().x + fDeltaX*fPerc, Position().y + fDeltaY*fPerc, Position().z);
}
FvVector3 FvGeographicObject::GetRoundPos(const float fAngle, const float fDistance)const
{
	float x = 0.0f;
	float y = 0.0f;
	GetRotate(Yaw()+fAngle, x, y);
	x *= fDistance;
	y *= fDistance;
	return FvVector3(x, y, 0) + Position();
}
//! << 地理信息



//! 地效飞行模拟(Wing in ground)
FvVector3 GroundWinger::Wing(const Type eType, const FvVector3& kNormal, const FvVector3& kFront)
{
	switch(eType)
	{
	case POINT:
		return FvVector3::UNIT_Z;
	case LINE:
		{
			FvVector3 kNormal_ = kNormal;
			const float fZ = sqrt(1 - kNormal_.z*kNormal_.z);
			kNormal_.z = 0;
			kNormal_.Normalise();

			const float fFall = fabs(kFront.DotProduct(kNormal_)*fZ);
			FvVector3 kNewNormal = kNormal_*fFall;
			kNewNormal.z = sqrt(1 - fFall*fFall);
			return kNewNormal;
		}
		break;
	case FACE:
		return kNormal;
	default:
		return kNormal;
	}
}
