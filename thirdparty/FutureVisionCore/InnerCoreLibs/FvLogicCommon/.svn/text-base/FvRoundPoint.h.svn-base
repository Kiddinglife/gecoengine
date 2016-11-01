
#ifndef __RoundPoint_H__
#define __RoundPoint_H__

#include "FvLogicCommonDefine.h"

#include <FvVector3.h>
#include <FvProvider.h>
#include "FvRefList.h"

class FV_LOGIC_COMMON_API FvRoundPoint: private FvRefNode1<FvRoundPoint*>
{
public:
	FvRoundPoint();
	~FvRoundPoint();

	void Init(const float fRandomRange, const float fFixRange, const FvSmartPointer<FvProvider1<FvVector3>>& spFrom);
	bool IsInRange(const FvVector3& kCenter)const;
	const FvVector3& Position()const{return m_kCrossPoint;}

private:
	
	float m_fRandomRange;
	float m_fFixRange;
	FvVector3 m_kCrossPoint;
	FvSmartPointer<FvProvider1<FvVector3>> m_spFrom; 
	friend class FvRoundPointList;
};

class FV_LOGIC_COMMON_API FvRoundPointList
{
public:
	typedef FvRefList<FvRoundPoint*, FvRefNode1> List;

	FvRoundPointList();

	void Tick();
	void SetPercRange(const float fPercInf, const float fPercSup);
	void Attach(FvRoundPoint& kPoint);
	void SetCenter(const FvSmartPointer<FvProvider1<FvVector3>>& spCenter);

private:
	static void _Update(List& kList, const FvVector3& kCenter);
	FvSmartPointer<FvProvider1<FvVector3>> m_spCenter;

	FvInt32 m_iMinDistPerc;
	FvInt32 m_iMaxDistPerc;
	static const FvUInt32 S_DIRECTION_TOTAL = 16;
	List m_kPosList[S_DIRECTION_TOTAL];
	static const float S_DIRECTIONS[S_DIRECTION_TOTAL];
};

namespace RoundPoint_Demo
{
	static void Test()
	{
		FvRoundPointList kList;
		FvSmartPointer<FvProvider1<FvVector3>> spCenter = new FvProvider1<FvVector3>();
		spCenter->SetValue0(FvVector3(0, 0, 0));
		kList.SetCenter(spCenter);

		FvRoundPoint kPoint1;
		FvSmartPointer<FvProvider1<FvVector3>> spPointFrom1 = new FvProvider1<FvVector3>();
		spPointFrom1->SetValue0(FvVector3(15, 0, 0));
		kPoint1.Init(10.0f, 5.0f, spPointFrom1);
		kList.Attach(kPoint1);

		FvRoundPoint kPoint2;
		FvSmartPointer<FvProvider1<FvVector3>> spPointFrom2 = new FvProvider1<FvVector3>();
		spPointFrom2->SetValue0(FvVector3(15, 0, 0));
		kPoint2.Init(10.0f, 5.0f, spPointFrom2);
		kList.Attach(kPoint2);

		spCenter->SetValue0(FvVector3(-10, 0, 0));

		FvRoundPoint kPoint3;
		FvSmartPointer<FvProvider1<FvVector3>> spPointFrom3 = new FvProvider1<FvVector3>();
		spPointFrom3->SetValue0(FvVector3(15, 0, 0));
		kPoint3.Init(10.0f, 5.0f, spPointFrom3);
		kList.Attach(kPoint3);
	}
}

#endif //__RoundPoint_H__
