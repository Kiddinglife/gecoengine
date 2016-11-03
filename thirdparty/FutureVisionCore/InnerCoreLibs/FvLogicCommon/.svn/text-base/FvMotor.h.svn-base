//{future header message}
#ifndef __FvMotor_H__
#define __FvMotor_H__


#include "FvLogicCommonDefine.h"

#include <FvProvider.h>
#include <FvVector3.h>
#include <FvVector4.h>
#include "FvMemoryNode.h"

class FV_LOGIC_COMMON_API FvMotor
{
	FV_MEM_TRACKER(FvMotor);
	FV_NOT_COPY_COMPARE(FvMotor);
public:
	typedef FvProvider2<FvVector3, FvVector4> TrackBall;
	typedef FvSmartPointer<TrackBall> TrackBallPtr;
	//+-------------------------------------------------------------------
	FvMotor(void);
	virtual ~FvMotor(void);
	//+-------------------------------------------------------------------

	virtual void SetTransltate(const FvVector3& pos);
	void SetTarget(const TrackBallPtr& pTrackBall);
	void SetEndForceUpdatePos(const bool bUpdate){m_bEndForceUpdatePos = bUpdate;}

	//+-------------------------------------------------------------------
	const FvVector3& GetTranslate()const{return m_Translate;}
	const FvVector4& GetDirection()const{return m_Dir;}
	const FvVector3& Velocity()const{return m_Vel;}
	const TrackBallPtr& GetTrackBall()const{return m_spTrackBall;}

	virtual void Start(const float durTime);
	bool Update(const float deltaTime);//! 如果不发生移动, 就返回False
	bool Update(const float deltaTime, FvVector3& kMoved);//! 如果不发生移动, 就返回False
	bool IsEnd()const;

	float GetRoll()const{return m_Dir.w;}

protected:
	virtual void _VelocityModify(const float deltaTime, const FvVector3& kTargetPos) = 0; //! 根据需求, 可以变成模板来提高效率, 但是不知道Motor类型会怎么样
	void _End();
	//+-------------------------------------------------------------------
	TrackBallPtr m_spTrackBall;//! 跟踪目标
	float m_fDurTime;//! 持续时间
	float m_fSpeed;
	float m_fDistance;
	bool m_bEndForceUpdatePos;

	FvVector4 m_Dir;
	FvVector3 m_Translate;
	FvVector3 m_Rotate;
	FvVector3 m_Vel;
};

//+-----------------------------------------------------------------------------------------------------------------------


class FV_LOGIC_COMMON_API FvTrackMotor: public FvMotor
{
public:
	//+-------------------------------------------------------------------
	FvTrackMotor(void){}
	virtual ~FvTrackMotor(void){}
	//+-------------------------------------------------------------------
protected:
	virtual void _VelocityModify(const float deltaTime, const FvVector3& kTargetPos);//! 如果不发生移动, 就返回False

};



//+-----------------------------------------------------------------------------------------------------------------------


class FV_LOGIC_COMMON_API FvGravityTrackMotor: public FvMotor
{
public:
	//+-------------------------------------------------------------------
	FvGravityTrackMotor(void);
	virtual ~FvGravityTrackMotor(void){}
	virtual void Start(const float durTime);
	void SetGravity(const float gravity);
	//+-------------------------------------------------------------------
protected:
	virtual void _VelocityModify(const float deltaTime, const FvVector3& kTargetPos);//! 如果不发生移动, 就返回False
	static const float CONST_GRAVITY;
	float m_GravityAcc;

};

class FV_LOGIC_COMMON_API FvRotateTrackMotor: public FvMotor
{
public:
	//+-------------------------------------------------------------------
	FvRotateTrackMotor(void);
	virtual ~FvRotateTrackMotor(void){}

	virtual void Start(const float durTime);

	void SetDir(const FvVector3& kDir);
	void SetRotateSpd(const float fMinRotSpd, const float fMaxRotSpd);

	//+-------------------------------------------------------------------
protected:
	virtual void _VelocityModify(const float deltaTime, const FvVector3& kTargetPos);

	float m_fMinRotSpd;
	float m_fMaxRotSpd;
};

class FV_LOGIC_COMMON_API FvGravityRotateTrackMotor: public FvGravityTrackMotor
{
public:
	//+-------------------------------------------------------------------
	FvGravityRotateTrackMotor(void);
	virtual ~FvGravityRotateTrackMotor(void){}

	void SetRotateSpd(const float fMinRotSpd, const float fMaxRotSpd);
	void SetDir(const FvVector3& kDir);
	//+-------------------------------------------------------------------
protected:
	virtual void _VelocityModify(const float deltaTime, const FvVector3& kTargetPos);//! 如果不发生移动, 就返回False
	float m_fMinRotSpd;
	float m_fMaxRotSpd;
};

namespace Motor_Demo
{
	static void Test()
	{
		FvRotateTrackMotor kMotor;
		FvVector3 kTargetPos(0,0,0);
		FvVector3 kSrcPos(50, 50, 0);
		FvVector3 kDir(1,0,0)/* = kTargetPos - kSrcPos*/;
		kDir.Normalise();
		FvMotor::TrackBallPtr spTarget = new FvMotor::TrackBall();
		spTarget->SetValue0(kTargetPos);
		kMotor.SetDir(kDir);
		kMotor.SetTransltate(kSrcPos);
		kMotor.SetTarget(spTarget);
		kMotor.SetRotateSpd(0.50f, 20.0f);
		kMotor.Start(50.0f);
		float fTime = 0.0f;
		while (kMotor.Update(0.03f))
		{
			fTime += 0.03f;
			const FvVector3& kPos = kMotor.GetTranslate();
			const FvVector4& kDir = kMotor.GetDirection();
			const FvVector3& kVel = kMotor.Velocity();
			FvLogBus::CritOk("<Time%f>Pos(%f, %f, %f) Dir(%f, %f, %f) Vel(%f, %f, %f) Roll(%f)", fTime,
				kPos.x, kPos.y, kPos.z, 			
				kDir.x, kDir.y, kDir.z, 
				kVel.x, kVel.y, kVel.z, kMotor.GetRoll());

			//kTargetPos.y += 0.03f;
			//spTarget->SetValue0(kTargetPos);
		}
	}
}

#endif//__FvMotor_H__