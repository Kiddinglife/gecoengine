//{future header message}
#ifndef __FvDirection3_H__
#define __FvDirection3_H__

#include "FvMath.h"

class FV_MATH_API FvDirection3
{
public:
	FvDirection3();
	FvDirection3(const float fYaw, const float fPitch, const float fRoll);

	float Yaw()const;
	float Pitch()const;
	float Roll()const;
	void SetYaw(const float fYaw);
	void SetPitch(const float fPitch);
	void SetRoll(const float fRoll);

	bool IsEqual(const FvDirection3& kOther)const;
	static bool IsEqual(const FvDirection3& kOne, const FvDirection3& kOther);

private:

	float m_fYaw;
	float m_fPitch;	
	float m_fRoll;	
};


#endif //__FvDirection3_H__