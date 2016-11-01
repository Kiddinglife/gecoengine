#pragma once

#include "FvLogicCommonDefine.h"

#include <FvKernel.h>
#include <FvVector3.h>


class FV_LOGIC_COMMON_API FvAngleCursor
{
public:
	FvAngleCursor(void);
	~FvAngleCursor(void);

	bool Update(const float fDeltaTime, const float fNewDir);
	float Direction()const;

	void SetDirection(const float fNewDir);
	void SetRotSpd(const float fSpd);
	void SetRotDirection(const float fDeltaAngle);

private:

	float m_fCurDiretion;
	float m_fRotSpd;
	float m_fRotDirection;

};
