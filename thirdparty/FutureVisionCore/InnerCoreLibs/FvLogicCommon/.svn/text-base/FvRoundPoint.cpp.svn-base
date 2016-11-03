#include "FvRoundPoint.h"

#include "FvGeographicObject.h"
#include <FvMath.h>
#include "FvGameRandom.h"



//float GetRoundRoundCrossPos(const FvVector3& kCenter0, const float fR0, const FvVector3& kCenter1, const float fR1, FvVector3& kCrossPos0, FvVector3& kCrossPos0)
//{
//	//令L=√[(a-x)^2+(b-y)^2] 
//	//K1=(b-y)/(a-x) 
//	//K2=-1/K1 
//	//X0=x+(a-x)(R^2-S^2+L^2)/(2L^2) 
//	//Y0=y+K1(X0-x) 
//	//R2=R^2-(X0-x)^2-(Y0-y)^2 
//	//则要求点C(Xc,Yc),D(Xd,Yd)的坐标为 
//	//Xc=X0-√[R2/(1+K^2)] 
//	//Yc=Y0+K2(Xc-X0) 
//	//Xd=X0+√[R2/(1+K^2)] 
//	//Yd=Y0+K2(Xd-X0)。 
//	const float fDX = kCenter0.x - kCenter1.x;
//	const float fDY = kCenter0.y - kCenter1.y;
//	const float fL2 = fDX*fDX + fDY*fDY;
//	float fK = 1.0f;
//	if(fDY == 0.0f)
//	{
//		fK = -fDX/fDY;
//	}
//	else
//	{
//		fK = -fDX/fDY;
//	}
//	//X0=x+(a-x)(R^2-S^2+L^2)/(2L^2) 
//	const float fXTemp = kCenter1.x + 0.5f*fDX*(fR0*fR0 - fR1*fR1 + fL2)/fL2;
//	//y+K1(X0-x) 
//	const float fYTemp = kCenter1.y + 0.5f*fDX*(fR0*fR0 - fR1*fR1 + fL2)/fL2;
//
//}

FvRoundPoint::FvRoundPoint()
:m_fRandomRange(0.0f)
,m_fFixRange(0.0f)
,m_kCrossPoint(0,0,0)
{
}
FvRoundPoint::~FvRoundPoint()
{

}

void FvRoundPoint::Init(const float fRandomRange, const float fFixRange, const FvSmartPointer<FvProvider1<FvVector3>>& spFrom)
{
	m_fRandomRange = fRandomRange;
	m_fFixRange = fFixRange;
	m_spFrom = spFrom;
}
bool FvRoundPoint::IsInRange(const FvVector3& kCenter)const
{
	const float fRange2 = FvDistance2(kCenter, m_kCrossPoint);
	const float fRangeMax = m_fFixRange + m_fRandomRange;
	return (fRange2 < fRangeMax*fRangeMax);
}

//+----------------------------------------------------------------------------------------------------

const float FvRoundPointList::S_DIRECTIONS[S_DIRECTION_TOTAL]
=
{
	0.0f, 
	FvMath2DTool::PI*0.125f, 
	FvMath2DTool::PI*0.25f, 
	FvMath2DTool::PI*0.375f, 
	FvMath2DTool::PI*0.5f, 
	FvMath2DTool::PI*0.625f, 
	FvMath2DTool::PI*0.75f, 
	FvMath2DTool::PI*0.875f, 
	FvMath2DTool::PI, 
	FvMath2DTool::PI*(-0.875f), 
	FvMath2DTool::PI*(-0.75f), 
	FvMath2DTool::PI*(-0.625f),
	FvMath2DTool::PI*(-0.5f), 
	FvMath2DTool::PI*(-0.375f), 
	FvMath2DTool::PI*(-0.25f), 
	FvMath2DTool::PI*(-0.125f)
};

FvRoundPointList::FvRoundPointList()
:m_iMinDistPerc(40)
,m_iMaxDistPerc(80)
{
}
void FvRoundPointList::SetPercRange(const float fPercInf, const float fPercSup)
{
	m_iMinDistPerc = FvMathTool::Clamp(0, FvInt32(fPercInf*100), 100);
	m_iMaxDistPerc = FvMathTool::Clamp(m_iMinDistPerc, FvInt32(fPercSup*100), 100);
}
void FvRoundPointList::_Update(List& kList, const FvVector3& kCenter)
{
	List::iterator iter = kList.GetHead();
	while (!kList.IsEnd(iter))
	{
		FvRoundPoint* pkPoint = static_cast<FvRoundPoint*>(iter);
		List::Next(iter);
		FV_ASSERT(pkPoint);
		if(!pkPoint->IsInRange(kCenter))
		{
			pkPoint->Detach();
		}
	}
}
void FvRoundPointList::Tick()
{
	if(m_spCenter == NULL)
	{
		return;
	}
	const FvVector3& kCenter = m_spCenter->GetData0();
	for (FvUInt32 uiIdx = 0; uiIdx < S_DIRECTION_TOTAL; ++uiIdx)
	{
		_Update(m_kPosList[uiIdx], kCenter);
	}
}
void FvRoundPointList::Attach(FvRoundPoint& kPoint)
{
	kPoint.Detach();
	if(m_spCenter == NULL || kPoint.m_spFrom == NULL)
	{
		return;
	}
	Tick();
	const FvVector3& kCenter = m_spCenter->GetData0();
	const FvVector3& kFrom = kPoint.m_spFrom->GetData0();
	const float MAX_WEIGHT = 100000.0f;
	float uiMinWeight = MAX_WEIGHT;
	FvUInt32 uiDir = 0;
	FvVector3 kMinCrossPoint(0,0,0);
	for (FvUInt32 uiIdx = 0; uiIdx < S_DIRECTION_TOTAL; ++uiIdx)
	{
		FvVector3 kDeltaPos(0,0,0);
		FvGeographicObject::GetRotate(S_DIRECTIONS[uiIdx], kDeltaPos.x, kDeltaPos.y);
		float fDistance = kPoint.m_fFixRange + kPoint.m_fRandomRange*FvGameRandom::GetValue(m_iMinDistPerc, m_iMaxDistPerc)*0.01f;
		kDeltaPos.x *= fDistance;
		kDeltaPos.y *= fDistance;
		const FvVector3 kCrossPos = kDeltaPos + kCenter;
		const float fDist2 = FvDistance2(kFrom, kCrossPos);
		const float fWeight = FvMathTool::Min(m_kPosList[uiIdx].Size()*1000 + fDist2, MAX_WEIGHT);
		if(uiMinWeight >= fWeight)
		{
			uiMinWeight = fWeight;
			uiDir = uiIdx;
			kMinCrossPoint = kCrossPos;
		}
	}
	m_kPosList[uiDir].AttachBack(kPoint);
	kPoint.m_kCrossPoint = kMinCrossPoint;
}
void FvRoundPointList::SetCenter(const FvSmartPointer<FvProvider1<FvVector3>>& spCenter)
{
	m_spCenter = spCenter;
}





//+----------------------------------------------------------------------------------------------------