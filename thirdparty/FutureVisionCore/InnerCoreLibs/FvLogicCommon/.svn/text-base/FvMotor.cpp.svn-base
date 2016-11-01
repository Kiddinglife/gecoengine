#include "FvMotor.h"
#include <FvMath.h>
#include <FvQuaternion.h>

FvMotor::FvMotor(void)
:m_spTrackBall(NULL)
,m_fDurTime(0.0f)
,m_fSpeed(0.0f)
,m_fDistance(0.0f)
,m_Dir(0,0,0,0)
,m_Translate(0,0,0)
,m_Vel(0,0,0)
,m_bEndForceUpdatePos(true)
{
	FV_MEM_TRACK;
}

FvMotor::~FvMotor(void)
{
	m_spTrackBall = NULL;
}

void FvMotor::Start(const float durTime)
{
	m_fDurTime = FvMathTool::Max(durTime, 0.01f);
	m_Vel = FvVector3::ZERO;
}
bool FvMotor::IsEnd()const
{
	return (m_fDurTime <= 0.0f);
}
void FvMotor::_End()
{
	m_fSpeed = 0.0f;
	m_fDurTime = 0.0f;
	m_fDistance = 0.0f;
}
bool FvMotor::Update(const float deltaTime)
{
	FvVector3 kMoved(0,0,0);
	return Update(deltaTime, kMoved);
}
bool FvMotor::Update(const float deltaTime, FvVector3& kMoved)
{
	FV_ASSERT(deltaTime > 0.0f);//! 交给外面管理器控制
	FV_ASSERT(m_spTrackBall);
	FV_ASSERT(m_fDurTime >= 0.0f);

	const FvVector3& kTargetPos = m_spTrackBall->GetData0();
	const FvVector3 kDiff = kTargetPos - m_Translate;
	m_fDistance = kDiff.Length();
	m_fSpeed = m_fDistance/m_fDurTime;
	if(m_fSpeed > 0.1f)
	{
		const FvVector3 kOldVel = m_Vel;
		_VelocityModify(deltaTime, kTargetPos);
		kMoved = m_Vel*deltaTime;//(kOldVel + m_Vel)*deltaTime*0.5f;
		m_Translate += kMoved;
	}
	else
	{
		kMoved = FvVector3::ZERO;
	}
	//FvLogBus::CritOk("FvMotor::Update Translate[%f, %f, %f], Moved[%f, %f, %f]", 
	//	m_Translate.x, m_Translate.y, m_Translate.z,
	//	kMoved.x, kMoved.y, kMoved.z);
	m_fDurTime -= deltaTime;
	if(m_fDurTime <= 0.0f)
	{
		if(m_bEndForceUpdatePos)
		{
			kMoved = kTargetPos - m_Translate;
			m_Translate = kTargetPos;
		}
		_End();
		return false;
	}
	else
	{
		return true;
	}
}
void FvMotor::SetTarget(const FvMotor::TrackBallPtr& pTrackBall)
{
	m_spTrackBall = pTrackBall;
}
void FvMotor::SetTransltate(const FvVector3& pos)
{
	m_Translate = pos;
}

//+-----------------------------------------------------------------------------------------------------------------------
void FvTrackMotor::_VelocityModify(const float deltaTime, const FvVector3& kTargetPos)
{
	FV_ASSERT(deltaTime > 0.0f);
	FV_ASSERT(m_fSpeed > 0.0f);
	FvVector3 kDir = kTargetPos - m_Translate;
	kDir.Normalise();
	m_Vel = kDir*m_fSpeed;
	m_Dir.Set(kDir.x, kDir.y, kDir.z, 0.0f);
}


const float FvGravityTrackMotor::CONST_GRAVITY = 9.8f;

FvGravityTrackMotor::FvGravityTrackMotor()
:m_GravityAcc(CONST_GRAVITY)
{

}
void FvGravityTrackMotor::Start(const float durTime)
{
	FV_ASSERT(m_spTrackBall);
	if(m_spTrackBall == NULL)
	{
		m_spTrackBall = new FvProvider2<FvVector3, FvVector4>();
	}
	const float durTimeMod = FvMathTool::Max(0.01f, durTime);
	FvMotor::Start(durTimeMod);
	const FvVector3& targetPos =  m_spTrackBall->GetData0();
	const float distanceH = FvMath2DTool::Distance(targetPos.x, targetPos.y, m_Translate.x, m_Translate.y);
	const float distanceV = targetPos.z - m_Translate.z;
	const float time = distanceV/m_GravityAcc/durTimeMod;
	const float preDeltaTime = durTimeMod*0.5f + time;//! 上行时间
	const float aftDeltaTime = durTimeMod*0.5f - time;//! 下行时间0
	m_Vel.z = preDeltaTime*m_GravityAcc;//! v = g*t
}
void FvGravityTrackMotor::_VelocityModify(const float deltaTime, const FvVector3& kTargetPos)
{
	FV_ASSERT(deltaTime > 0.0f);
	FV_ASSERT(m_fSpeed > 0.0f);

	const float distanceH = FvMath2DTool::Distance(kTargetPos.x, kTargetPos.y, m_Translate.x, m_Translate.y);
	const float distanceV = kTargetPos.z - m_Translate.z;
	const float time = distanceH/m_fSpeed;
	m_GravityAcc = -2.0f*(distanceV/(time*time) - m_Vel.z/time);//! 重力加速度修正
	m_GravityAcc = FvMathTool::Clamp(m_GravityAcc, -CONST_GRAVITY, 2.0f*CONST_GRAVITY);
	FvVector3 kDir = kTargetPos - m_Translate;
	kDir.z = 0.0f;
	kDir.Normalise();
	m_Vel.z -= m_GravityAcc*deltaTime;
	m_Vel.x = m_Dir.x*m_fSpeed;
	m_Vel.y = m_Dir.y*m_fSpeed;
	kDir = m_Vel;
	kDir.Normalise();
	m_Dir.Set(kDir.x, kDir.y, kDir.z, 0.0f);
}
void FvGravityTrackMotor::SetGravity(const float gravity)
{
	m_GravityAcc = FvMathTool::Clamp(gravity, 1.0f, CONST_GRAVITY);
}



FvRotateTrackMotor::FvRotateTrackMotor(void)
:m_fMinRotSpd(1.0f)
,m_fMaxRotSpd(20.0f)
{

}
void FvRotateTrackMotor::Start(const float durTime)
{
	FvMotor::Start(durTime);
	FV_ASSERT(m_spTrackBall);
}
void FvRotateTrackMotor::SetDir(const FvVector3& kDir)
{
	m_Dir.Set(kDir.x, kDir.y, kDir.z, 0.0f);
}
void FvRotateTrackMotor::SetRotateSpd(const float fMinRotSpd, const float fMaxRotSpd)
{
	m_fMinRotSpd = FvMathTool::Max(0.01f, fMinRotSpd);
	m_fMaxRotSpd = FvMathTool::Max(20.0f, fMaxRotSpd);
}

void FvRotateTrackMotor::_VelocityModify(const float deltaTime, const FvVector3& kTargetPos)
{
	FV_ASSERT(deltaTime > 0.0f);
	FV_ASSERT(m_fSpeed > 0.0f);
	FV_ASSERT(m_fDurTime > 0.0f);

	const FvVector3 kDiff = m_spTrackBall->GetData0() - m_Translate;
	const float fCosValue = FvVector3(m_Dir.x, m_Dir.y, m_Dir.z).DotProduct(kDiff);
	const float fDeltaAngle = (fCosValue >= 1.0f) ? 0.0f: acos(fCosValue);
	const float fRotTimePercMax = 2.5f;//40%
	const float fRotSpd = FvMathTool::Min<float>(fabs(fDeltaAngle/m_fDurTime)*fRotTimePercMax, m_fMaxRotSpd);//FvMathTool::Clamp(fabs(fDeltaAngle/m_fDurTime)*fRotTimePercMax, m_fMinRotSpd, m_fMaxRotSpd);
	const float fMaxRot = deltaTime*fRotSpd;
	FvVector3 kDir = kDiff;
	kDir.Normalise();
	if(fabs(fDeltaAngle) < fMaxRot)
	{
		if(fDeltaAngle != 0.0f)
		{
			FvLogBus::CritOk("FvRotateTrackMotor::_VelocityModify fMaxRot[%f] fDeltaAngle[%f] fRotSpd[%f]", fMaxRot, fDeltaAngle, fRotSpd);
		}
		m_Dir.Set(kDir.x, kDir.y, kDir.z, 0.0f);
	}
	else
	{
		FvVector3 kAxis = FvVector3::UNIT_Z;
		if(fCosValue >= 1.0f)
		{
			kAxis = -FvVector3::UNIT_Z;
		}
		else if(fCosValue <= -1.0f)
		{
			kAxis = FvVector3::UNIT_Z;
		}
		else
		{
			kAxis = FvVector3(m_Dir.x, m_Dir.y, m_Dir.z).CrossProduct(kDir);
		}

		const float fSign = -FvMathTool::Sign(kAxis.z);
		kAxis.Normalise();
		FvQuaternion kQuat;
		kQuat.FromAngleAxis(-fMaxRot, kAxis);
		kDir = kQuat*kDir;
		kDir.Normalise();
		const float fAcc = m_fSpeed*fRotSpd;
		const float fGrav = 9.8f;
		const float fRoll = fSign*atan2(fAcc , fGrav);
		m_Dir.Set(kDir.x, kDir.y, kDir.z, fRoll);
		//FvLogBus::CritOk("FvRotateTrackMotor::_VelocityModify fRotSpd[%f] fAcc[%f] RollAngle[%f]", fRotSpd, fAcc, m_fRollAngle);
	}
	m_Vel = kDir*m_fSpeed;
}

//+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
FvGravityRotateTrackMotor::FvGravityRotateTrackMotor(void)
:m_fMinRotSpd(1.0f)
,m_fMaxRotSpd(6.0f)
{

}
void FvGravityRotateTrackMotor::SetRotateSpd(const float fMinRotSpd, const float fMaxRotSpd)
{
	m_fMinRotSpd = FvMathTool::Max(0.01f, fMinRotSpd);
	m_fMaxRotSpd = FvMathTool::Max(20.0f, fMaxRotSpd);
}
void FvGravityRotateTrackMotor::SetDir(const FvVector3& kDir)
{
	m_Dir.Set(kDir.x, kDir.y, kDir.z, 0.0f);
}

void FvGravityRotateTrackMotor::_VelocityModify(const float deltaTime, const FvVector3& kTargetPos)
{
	FV_ASSERT(deltaTime > 0.0f);
	FV_ASSERT(m_fSpeed > 0.0f);
	FV_ASSERT(m_fDurTime > 0.0f);

	FvVector3 kDiff = kTargetPos - m_Translate;

	///<计算水平旋转>
	const float fDeltaAngle = FvMath2DTool::GetRotateAngle(m_Dir.x, m_Dir.y, kDiff.x, kDiff.y);
	const float fRotTimePercMax = 5;//20%
	const float fRotSpd = FvMathTool::Min<float>(fabs(fDeltaAngle/m_fDurTime)*fRotTimePercMax, m_fMaxRotSpd);//FvMathTool::Clamp(fabs(fDeltaAngle/m_fDurTime)*fRotTimePercMax, m_fMinRotSpd, m_fMaxRotSpd);
	const float fMaxRot = deltaTime*fRotSpd;

	float fRollAngle = 0.0f;
	if(fabs(fDeltaAngle) < fMaxRot)
	{
		kDiff.Normalise();
	}
	else
	{
		const float fSign = FvMathTool::Sign(fDeltaAngle);
		FvMath2DTool::Rotate(m_Dir.x, m_Dir.y, fMaxRot*FvMathTool::Sign(fDeltaAngle));
		const float fAcc = fRotSpd*m_fSpeed;
		const float fGra = 9.8f;
		fRollAngle = fSign*atan2(fGra, fAcc);
	}

	///<计算重力>
	const float distanceH = FvMath2DTool::Length(kDiff.x, kDiff.y);
	const float distanceV = kTargetPos.z - m_Translate.z;
	const float time = distanceH/m_fSpeed;
	m_GravityAcc = -2.0f*(distanceV/(time*time) - m_Vel.z/time);//! 重力加速度修正
	m_GravityAcc = FvMathTool::Clamp(m_GravityAcc, -CONST_GRAVITY, 2.0f*CONST_GRAVITY);

	m_Vel.z -= m_GravityAcc*deltaTime;
	m_Vel.x = m_Dir.x*m_fSpeed;
	m_Vel.y = m_Dir.y*m_fSpeed;
	FvVector3 kDir = m_Vel;
	kDir.Normalise();
	m_Dir.Set(kDir.x, kDir.y, kDir.z, fRollAngle);
}