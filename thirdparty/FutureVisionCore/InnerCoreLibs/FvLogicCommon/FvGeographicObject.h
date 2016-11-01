//{future header message}
#ifndef __FvGeographicObject_H__
#define __FvGeographicObject_H__

#include "FvLogicCommonDefine.h"

#include <FvVector3.h>
#include <FvDirection3.h>

class FV_LOGIC_COMMON_API FvGeographicObject
{
public:
	static const FvVector3 FRONT;
	static const FvVector3 VERTICAL;

	static float GetDirection(const float fX, const float fZ);
	static void GetRotate(const float fAngle, float& fX, float& fY);
	static float GetDistance(const FvVector3& kPos1, const FvVector3& kPos2);
	static float GetDistance2(const FvVector3& kPos1, const FvVector3& kPos2);

	virtual float Yaw()const{return 0.0f;}
	virtual FvDirection3 GetDirection()const{return FvDirection3();}
	virtual float BodyRadius()const{return 0.0f;}
	virtual const FvVector3& Position()const{return FvVector3::ZERO;}
	virtual void SetYaw(const float fDir){}

public:

	//+-----------------------------------------------------------------------------
	//! 地理信息 >> 

	float Aim(const float fX, const float fY);
	float Aim(const FvGeographicObject& kObj);
	void TurnAngle(const float fDeltaAngle);

	float GetAngleFromLocation(const float fX, const float fY)const;
	float GetAngleFromDirection(const float fDeltaX, const float fDeltaY)const;
	bool IsInFront(const float fX, const float fY)const;

	float GetDistance(const FvVector3& kPos)const;
	//float GetDistance2(const FvVector3& kPos)const;
	float GetDistance(const float fX, const float fY)const;
	//float GetDistance2(const float fX, const float fY)const;
	float GetDistance(const FvGeographicObject& kObj)const;
	//float GetDistance2(const FvGeographicObject& kObj)const;

	FvVector3 GetIntersectionPosByDist(const float fToX, const float fToY, const float fDistance)const;	//! fDistance = 0, reach this, fDistance < 0 , penetrate this
	static FvVector3 GetIntersectionPosByDist(const FvVector3& kRootPos, const float fToX, const float fToY, const float fDistance);	//! fDistance = 0, reach kRootPos, fDistance< 0 , penetrate this
	FvVector3 GetIntersectionPosByPerc(const float fToX, const float fToY, const float fPerc)const;	//! fPerc = 0  Reach this, fPerc = 1 Reach From, fPerc < 0 behind self
	
	FvVector3 GetRoundPos(const float fAngle, const float fDistance)const;
	


protected:
	FvGeographicObject();
	~FvGeographicObject(void);


private:

	//+-----------------------------------------------------
};


//! 地效飞行模拟(Wing in ground)
class FV_LOGIC_COMMON_API GroundWinger
{
public:
	enum Type
	{
		POINT,
		LINE,
		FACE,
	};

	static FvVector3 Wing(const Type eType, const FvVector3& kNormal, const FvVector3& kFront);
};


#endif//__FvGeographicObject_H__