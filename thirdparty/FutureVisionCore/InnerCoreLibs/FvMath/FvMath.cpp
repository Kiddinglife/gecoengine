#include "FvMath.h"


const float FvMath2DTool::PI = FV_MATH_PI_F;
const float FvMath2DTool::HALF_PI = PI*0.5f;
const float PI_2 = FV_MATH_PI_F*2.0f;

//! 正前方的向量[0.0f, -1.0f]
const float FvMath2DTool::FRONT_X = 0.0f;
const float FvMath2DTool::FRONT_Y = -1.0f;

float FvMath2DTool::GetAngle(const float fX, const float fY)
{
	const float angle =  atan2f(fX, -fY);
	return angle;
}
void FvMath2DTool::CaculateAngle(const float fAngle, float& fX, float& fY)
{
	fY = -cos(fAngle);
	fX = sin(fAngle);
}
float FvMath2DTool::GetRotateAngle(const float fSrcX, const float fSrcy, const float fDesX, const float fDesY)
{
	//if(abs(fSrcX) <= 0.001f && abs(fSrcy) <= 0.001f 
	//	|| abs(fDesX) <= 0.001f && abs(fDesY) <= 0.001f)
	//{
	//	return 0.0f;
	//}
	const float fDesAngle = GetAngle(fDesX, fDesY);
	const float fSrcAngle =  GetAngle(fSrcX, fSrcy);
	float fRotateAngle = fDesAngle - fSrcAngle;
	// 映射到(-M_PI, M_PI)区间上来
	if(fRotateAngle > PI)
		fRotateAngle -= PI_2;
	else if (fRotateAngle < -PI)
		fRotateAngle += PI_2;
	return fRotateAngle;
}

void FvMath2DTool::Rotate(const float fSrcX, const float fSrcy, const float fRotateAngle, float& fDesX, float& fDesY)
{
	//! 逆时针旋转
	const float fSin = sin(fRotateAngle);
	const float fCon = cos(fRotateAngle);
	//! 顺时针旋转
	//const float fSin = sin(-fRotateAngle);
	//const float fCon = cos(-fRotateAngle);
	fDesX = fCon*fSrcX - fSin*fSrcy;
	fDesY = fSin*fSrcX + fCon*fSrcy;
}

void FvMath2DTool::Rotate(float& fX, float& fY, const float fRotateAngle)
{
	const float fSin = sin(fRotateAngle);
	const float fCon = cos(fRotateAngle);
	//! 顺时针旋转
	//const float fSin = sin(-fRotateAngle);
	//const float fCon = cos(-fRotateAngle);
	const float fDesX = fCon*fX - fSin*fY;
	const float fDesY = fSin*fX + fCon*fY;
	fX = fDesX;
	fY = fDesY;
}

void FvMath2DTool::RotateRight90(float& x, float& y)
{
	const float temp = x;
	x = y;
	y = -temp;
}
void FvMath2DTool::RotateLeft90(float& x, float& y)
{
	const float temp = x;
	x = -y;
	y = temp;
}
int FvMath2DTool::GetSide(const float fromX, const float fromY, const float toX, const float toY, const float x, const float y)
{
	const float s = (fromX - x)*(toY - y) - (fromY - y)*(toX - x);
	if(s == 0)
	{
		return 0;
	}
	else if(s < 0)//! 右侧
	{
		return -1;
	}
	else
	{
		return 1;
	}
}