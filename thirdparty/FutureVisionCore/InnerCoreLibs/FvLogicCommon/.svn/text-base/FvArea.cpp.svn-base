#include "FvArea.h"

#include <FvAngle.h>
#include <FvDebug.h>
#include "FvLogBus.h"




FvRotRect::FvRotRect()
:m_fLen(0)
,m_fHalfWidth(0)
,m_kDir(0, 0, 0)
{
	
}
bool FvRotRect::In(const FvVector3& center, const FvVector3& pos)const
{
	const FvVector3 delta = pos - center;
	const float len = delta.DotProduct(m_kDir);
	if(len < 0.0f || len > m_fLen)
	{
		return false;
	}
	const FvVector3 prjLen =  m_kDir*len;
	const FvVector3 prjWidth = delta - prjLen;
	const float halfWidth2 = prjWidth.LengthSquared();
	if(halfWidth2 > m_fHalfWidth*m_fHalfWidth)
	{
		return false;
	}
	return true;
}
bool FvRotRect::In(const FvVector3& center, const FvVector3& pos, const float fRange)const
{
	const FvVector3 delta = pos - center;
	const float len = delta.DotProduct(m_kDir);
	if(len < 0.0f || len > (m_fLen + fRange))
	{
		return false;
	}
	const FvVector3 prjLen =  m_kDir*len;
	const FvVector3 prjWidth = delta - prjLen;
	const float halfWidth2 = prjWidth.LengthSquared();
	if(halfWidth2 > (m_fHalfWidth + fRange)*(m_fHalfWidth + fRange))
	{
		return false;
	}
	return true;
}
void FvRotRect::SetDirection(const FvVector3& dir)
{
	m_kDir = dir;
	m_kDir.Normalise();
}



FvArea::FvArea(void)
:m_kCenter(0.0f, 0.0f, 0.0f)
,m_fRange(0)
{
	FV_MEM_TRACK;
}

FvArea::~FvArea(void)
{
}
float FvArea::Range()const
{
	return m_fRange;
}
const FvVector3& FvArea::Center()const
{
	return m_kCenter;
}
void FvArea::SetCenter(const FvVector3& pos)
{
	_SetCenter(pos.x, pos.y, pos.z);
}
void FvArea::SetCenter(const float x, const float y, const float z)
{
	_SetCenter(x, y, z);
}
void FvArea::_SetCenter(const float x, const float y, const float z)
{
	m_kCenter.x = x;
	m_kCenter.y = y;
	m_kCenter.z = z;
}

//+------------------------------------------------------------------------------------------------------------
FvRoundArea::FvRoundArea()
:m_fRadius2(0.0f)
{

}
void FvRoundArea::Init(const float fRadius)
{
	m_fRadius2 = fRadius*fRadius;
	m_fRange = fRadius;
}
bool FvRoundArea::_InArea(const float x, const float y, const float z)const
{
	return FvMath2DTool::Distance2(m_kCenter.x, m_kCenter.y, x, y) < m_fRadius2;
}
bool FvRoundArea::_InArea(const float x, const float y, const float z, const float fRange)const
{
	return FvMath2DTool::Distance2(m_kCenter.x, m_kCenter.y, x, y) < m_fRadius2 + fRange*fRange;
}
//+------------------------------------------------------------------------------------------------------------

//+------------------------------------------------------------------------------------------------------------
FvSectorArea::FvSectorArea()
:m_fLeftAngle(0.0f)
,m_fRightAngle(0.0f)
,m_fDirLeftAngle(0.0f)
,m_fDirRightAngle(0.0f)
,m_fRadius2(0.0f)
{

}
void FvSectorArea::Init(const float fRadius, const float fLeftAngle, const float fRightAngle)
{
	m_fLeftAngle = fLeftAngle;
	FvAngle::Normalise(m_fLeftAngle);
	m_fRightAngle = fRightAngle;
	FvAngle::Normalise(m_fRightAngle);
	if(m_fLeftAngle > m_fRightAngle)
	{
		m_fRightAngle += FV_MATH_2PI_F;
	}
	m_fRange = fRadius;
	m_fRadius2 = fRadius*fRadius;
	FV_ASSERT(m_fLeftAngle <= m_fRightAngle);
}
void FvSectorArea::SetDir(const float fDir)
{
	m_fDirLeftAngle = m_fLeftAngle + fDir;
	m_fDirRightAngle = m_fRightAngle + fDir;
	if(m_fDirLeftAngle > m_fDirRightAngle)
	{
		m_fDirRightAngle += FV_MATH_2PI_F;
	}
	FV_ASSERT(m_fDirLeftAngle <= m_fDirRightAngle);
}
bool FvSectorArea::_InArea(const float x, const float y, const float z)const
{
	FV_ASSERT(m_fDirLeftAngle <= m_fDirRightAngle);
	const float deltaX = float(x - m_kCenter.x);
	const float deltaY = float(y - m_kCenter.y);
	const float dir = FvMath2DTool::GetAngle(deltaX, deltaY);

	return (FvMath2DTool::Distance2(m_kCenter.x, m_kCenter.y, x, y) < m_fRadius2 ) && ( m_fDirLeftAngle <= dir && dir < m_fDirRightAngle);
}
bool FvSectorArea::_InArea(const float x, const float y, const float z, const float fRange)const
{
	FV_ASSERT(m_fDirLeftAngle <= m_fDirRightAngle);
	const float deltaX = float(x - m_kCenter.x);
	const float deltaY = float(y - m_kCenter.y);
	const float dir = FvMath2DTool::GetAngle(deltaX, deltaY);
	return (FvMath2DTool::Distance2(m_kCenter.x, m_kCenter.y, x, y) < (m_fRadius2 + fRange) ) && ( m_fDirLeftAngle <= dir && dir < m_fDirRightAngle);
}
//+------------------------------------------------------------------------------------------------------------

FvRotRectArea::FvRotRectArea()
{

}

void FvRotRectArea::Init(const float fFront, const float fHalfWidth)
{
	m_kRotRect.m_fLen = fFront;
	m_kRotRect.m_fHalfWidth = fHalfWidth;
	m_fRange = FvMath2DTool::Length(fFront, fHalfWidth);
}
void FvRotRectArea::Rotate(const float angle)
{
	FV_ASSERT(0 && "ипн╢й╣ож");
}
void FvRotRectArea::SetDir(const FvVector3& kDir)
{
	m_kRotRect.SetDirection(kDir);		
}

bool FvRotRectArea::_InArea(const float x, const float y, const float z)const
{
	return m_kRotRect.In(m_kCenter, FvVector3(x, y, z));
}

bool FvRotRectArea::_InArea(const float x, const float y, const float z, const float fRange)const
{
	return m_kRotRect.In(m_kCenter, FvVector3(x, y, z), fRange);
}

//+------------------------------------------------------------------------------------------------------------

const FvArea& FvAreaTool::GetArea(const FvAreaData& kData)
{
	switch (kData.uiType)
	{
	case 	FvAreaData::ROUND:
		m_kRoundArea.SetCenter(kData.m_kCenter);
		m_kRoundArea.Init(kData.m_fValue0);
		return m_kRoundArea;
		break;
	case 	FvAreaData::SECTOR:
		m_kSectorArea.SetCenter(kData.m_kCenter);
		m_kSectorArea.Init(kData.m_fValue0, kData.m_fValue1, kData.m_fValue2);
		return m_kSectorArea;
		break;
	case 	FvAreaData::ROT_RECT:
		m_kRotRectArea.SetCenter(kData.m_kCenter);
		m_kRotRectArea.Rotate(kData.m_fValue0);
		m_kRotRectArea.Init(kData.m_fValue1, kData.m_fValue2);
		return m_kSectorArea;
		break;
	default:
		FvLogBus::Warning("FvAreaData::GetArea:: type error");
		m_kRoundArea.SetCenter(kData.m_kCenter);
		m_kRoundArea.Init(kData.m_fValue0);
		return m_kRoundArea;
	}
}
