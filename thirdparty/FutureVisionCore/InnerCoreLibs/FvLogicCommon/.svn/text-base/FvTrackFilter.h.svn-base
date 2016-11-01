
#ifndef __FvTrackFilter_H__
#define __FvTrackFilter_H__

#include "FvLogicCommonDefine.h"


#include <FvKernel.h>
#include <FvVector3.h>
#include <FvAngle.h>

#include <list>

class FV_LOGIC_COMMON_API FvSmoothTime
{
public:

	FvSmoothTime();

	void SetIgnoreRange(const float fMinRange, const float fMaxRange);

	void Start(const double fTime);
	float Tick(const float fDeltaTime);
	void Drive(const double fTime);
	double Time()const{return m_fLocal;}

private:

	double m_fLocal;
	double m_fDrive;
	
	float m_fIgnoreMinRange;
	float m_fIgnoreMaxRange;
};

struct FvPositionRotateFilterNode
{
	FvPositionRotateFilterNode():m_kPosition(0,0,0), m_kYaw(0), m_fMovSpd(0.0f), m_fExecTime(0.0)
	{}
	FvPositionRotateFilterNode(const FvVector3& kPos, const float fAngle, const float fMovSpd, const double fExecTime)
		:m_kPosition(kPos), m_kYaw(fAngle), m_fMovSpd(fMovSpd), m_fExecTime(fExecTime)
	{}

	FvVector3 m_kPosition;
	FvAngle m_kYaw;
	float m_fMovSpd;
	double m_fExecTime;
};

struct FvMoveActFilterNode
{
	FvUInt32 m_uiMoveFlag;
	double m_fExecTime;
};

class FV_LOGIC_COMMON_API FvPositionRotateFilter
{
public:
	FvPositionRotateFilter(){}
	~FvPositionRotateFilter(){Clear();}
	
	typedef FvPositionRotateFilterNode Node;

	const std::list<Node>& List()const{return m_kList;}
	void Add(const FvVector3& kPos, const float fYaw, const float fMovSpd, const double fExecTime);
	void Add(const Node& kNode){m_kList.push_back(kNode);}

	bool Tick(const double fOldTime, const double fNewTime, FvVector3& kPos, float& fYaw);
	void Clear(){m_kList.clear();}

private:

	static bool _Update(double& fCurTime, double& fDeltaTime, FvVector3& kPos, float& fYaw, const Node& kNode);

	std::list<Node> m_kList;
};

class FV_LOGIC_COMMON_API FvSmothTimePositionRotateFilter
{
public:
	FvSmothTimePositionRotateFilter(void){}
	~FvSmothTimePositionRotateFilter(void){}

	FvPositionRotateFilter& PositionRotateFilter(){return m_kPositionRotateFilter;}
	FvSmoothTime& GetTime(){return m_kSmoothTime;}

	bool Tick(const float fDeltaTime, FvVector3& kPos, float& fYaw);

private:

	FvPositionRotateFilter m_kPositionRotateFilter;
	FvSmoothTime m_kSmoothTime;
};

namespace PositionRotateFilter_Demo
{
	static void Test()
	{
		//FvPositionRotateFilter kFilter;
		//kFilter.Add(FvVector3(0, 0, 0), 0.0f, 6.0f, 0.1f);
		//kFilter.Add(FvVector3(0, 1, 0), 0.0f, 6.0f, 0.2f);
		//kFilter.Add(FvVector3(1, 1, 0), 0.0f, 6.0f, 0.3f);
		//kFilter.Add(FvVector3(1, 0, 0), 0.0f, 6.0f, 0.4f);

		//FvVector3 kPos;
		//float fYaw = 0.0f;
		//kFilter.Start(0.1f);
	
		//while(1)
		//{
		//	const bool bUpdated = kFilter.Tick(0.04f, kPos, fYaw);
		//	if(bUpdated == false)
		//	{
		//		return;
		//	}
		//}
		//kFilter.Tick(0.04f, kPos, fYaw);
		//kFilter.Tick(0.04f, kPos, fYaw);
		//kFilter.Tick(0.04f, kPos, fYaw);


		FvSmoothTime m_kTimeSmoothing;
		m_kTimeSmoothing.SetIgnoreRange(1.0f, 3.0f);
		m_kTimeSmoothing.Start(0.0);
		m_kTimeSmoothing.Drive(0.0);

		float fTime = 0.0f;
		while(fTime < 2.0f)
		{
			m_kTimeSmoothing.Tick(0.1f);
			fTime += 0.1f;
		}
		m_kTimeSmoothing.Drive(3.0);
		while(fTime < 4.0f)
		{
			m_kTimeSmoothing.Tick(0.1f);
			fTime += 0.1f;
		}
		m_kTimeSmoothing.Drive(4.0);
		while(fTime < 5.0f)
		{
			m_kTimeSmoothing.Tick(0.1f);
			fTime += 0.1f;
		}
	}
}


#endif //__FvTrackFilter_H__
