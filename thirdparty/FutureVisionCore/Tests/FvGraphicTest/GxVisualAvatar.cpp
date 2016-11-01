#include "GxVisualAvatar.h"

//#include "GxDeriveProvider.h"
#include "GxVisualBuilder.h"
//#include "GxSoundListenTracker.h"
//#include "GxVisualDurationEffect_Ex.h"

#include <FvGameTimer.h>

std::map<FvUInt32, GxDeriveProviderInfo_1> GxVisualAvatar::ms_DeriveTrackBallOffsetList_1;
std::map<FvUInt32, GxDeriveProviderInfo_2> GxVisualAvatar::ms_DeriveTrackBallOffsetList_2;

GxVisualAvatar::GxVisualAvatar()
:m_uiStep(0)
,m_rpAvatar(NULL)
#pragma warning(push)
#pragma warning(disable: 4355)
,m_kCB_OnAvatarNodeLoaded(*this)
//,m_kSpellVisualProc(*this)
#pragma warning(pop)
{
	m_kAnimTimeFlow.Init(FvGameTimer::Timer(), GxVisualData::SpellProc::MAX_ANIM, *this);
}
GxVisualAvatar::~GxVisualAvatar()
{
}
void GxVisualAvatar::SetAvatarAttachment(FvAvatarAttachment& pkAvatarAttachment)
{
	m_rpAvatar = &pkAvatarAttachment;
}
void GxVisualAvatar::SetSpace(const FvZoneSpacePtr& spSpace)
{
	m_spSpace = spSpace;	
}
//+------------------------------------------------------------------------------------------------------------------------------------------------------------
void GxVisualAvatar::AddTrackPart(const FvUInt32 uiIdx, const GxDeriveProviderInfo_1& kInfo)
{
	ms_DeriveTrackBallOffsetList_1[uiIdx] = kInfo;
}
void GxVisualAvatar::AddTrackPart(const FvUInt32 uiIdx, const GxDeriveProviderInfo_2& kInfo)
{
	ms_DeriveTrackBallOffsetList_2[uiIdx] = kInfo;
}
bool GxVisualAvatar::GetTrackPartName(const FvUInt32 uiPart, const GxDeriveProviderInfo_1*& kInfo)
{
	std::map<FvUInt32, GxDeriveProviderInfo_1>::const_iterator iter = ms_DeriveTrackBallOffsetList_1.find(uiPart);
	if(iter != ms_DeriveTrackBallOffsetList_1.end())
	{
		kInfo = &(*iter).second;
		return true;
	}
	return false;
}
bool GxVisualAvatar::GetTrackPartName(const FvUInt32 uiPart, const GxDeriveProviderInfo_2*& kInfo)
{
	std::map<FvUInt32, GxDeriveProviderInfo_2>::const_iterator iter = ms_DeriveTrackBallOffsetList_2.find(uiPart);
	if(iter != ms_DeriveTrackBallOffsetList_2.end())
	{
		kInfo = &(*iter).second;
		return true;
	}
	return false;
}
FvMotor::TrackBallPtr GxVisualAvatar::GetTrackBall(const FvUInt32 uiPart)const
{
	const GxDeriveProviderInfo_1* pkInfo1 = NULL;
	if(GetTrackPartName(uiPart, pkInfo1))
	{
		FV_ASSERT(pkInfo1);
		return GetTrackBall(pkInfo1->m_kRefName.c_str(), pkInfo1->m_kOffest);
	}
	const GxDeriveProviderInfo_2* pkInfo2 = NULL;
	if(GetTrackPartName(uiPart, pkInfo2))
	{
		FV_ASSERT(pkInfo2);
		const FvMotor::TrackBallPtr pkRef1 = GetTrackBall(pkInfo2->m_kRefName_1.c_str());
		const FvMotor::TrackBallPtr pkRef2 = GetTrackBall(pkInfo2->m_kRefName_2.c_str());
		//GxDeriveProvider2* pkDeriveProvider = new GxDeriveProvider2();
		//pkDeriveProvider->SetSrc(pkRef1, pkRef2, pkInfo2->m_fWeight);
		//return pkDeriveProvider;
	}
	return m_spTrackBall;
}

FvMotor::TrackBallPtr GxVisualAvatar::GetTrackBall(const char* pcPart)const
{
	if(GetAvatar())
	{
		return (const FvMotor::TrackBallPtr&)GetAvatar()->ReferenceProvider(pcPart);
	}
	else
	{
		return m_spTrackBall;
	}
}
FvMotor::TrackBallPtr GxVisualAvatar::GetTrackBall(const char* pcPart, const FvVector3& kOffset)const
{
	const FvMotor::TrackBallPtr& spProvider = GetTrackBall(pcPart);
	if(kOffset.Length() < 0.0001f)
	{
		return spProvider;
	}
	else
	{
		//GxDeriveProvider1* pkDeriveProvider = new GxDeriveProvider1();
		//pkDeriveProvider->SetSrc(spProvider, kOffset);
		return spProvider;
	}
}
//+------------------------------------------------------------------------------------------------------------------------------------------------------------
void GxVisualAvatar::MakeExpress(const FvVector3& kTarget, const GxVisualData::TravelExpress& kInfo, const float fDuration)
{
	MakeExpress(new FvMotor::TrackBall(kTarget, FvVector4::ZERO_POINT), kInfo, fDuration);
}

void GxVisualAvatar::MakeExpress(const FvMotor::TrackBallPtr& spTrack, const GxVisualData::TravelExpress& kInfo, const float fDuration)
{
	const FvMotor::TrackBallPtr spFrom = GetTrackBall(kInfo.m_iStartPart);
	if(spFrom)
	{
		FvTravelExpressInfo kInstanceInfo;
		kInstanceInfo.m_fGravity = kInfo.m_iGravity*0.01f;
		kInstanceInfo.m_spTo = spTrack;
		kInstanceInfo.m_fDuration = fDuration;
		kInstanceInfo.m_fScale = kInfo.m_iScale*0.01f;
		kInstanceInfo.m_kFromPos = spFrom->GetData0();
		kInstanceInfo.m_fReserveTime = kInfo.m_iReserveTime*0.01f;
		MakeExpress(kInstanceInfo, kInfo.m_pcRes);
	}
}
FvNodeExpress* GxVisualAvatar::MakeExpress(const FvTravelExpressInfo& kInfo, const char* pcRes)
{
	FV_ASSERT(pcRes);
	if(kInfo.m_fGravity > 0.1f)
	{
		FvTravelNodeExpress<FvGravityTrackMotor>* pkGravityExpress = FvExpressManagerInstance::Instance().MakeExpress<FvTravelNodeExpress<FvGravityTrackMotor>>();
		FV_ASSERT(pkGravityExpress);
		if(pkGravityExpress)
		{
			//! 
			pkGravityExpress->Motor().SetTransltate(kInfo.m_kFromPos);
			pkGravityExpress->Motor().SetGravity(kInfo.m_fGravity);
			//! 
			pkGravityExpress->SetReserveTime(kInfo.m_fReserveTime);
			//!
			AttachExpressAppearance(*pkGravityExpress, pcRes, kInfo.m_fScale);
			pkGravityExpress->Start(kInfo.m_spTo, Space(), kInfo.m_fDuration);
			return pkGravityExpress;
		}
	}
	else
	{
		FvTravelNodeExpress<FvTrackMotor>* pkTrackExpress = FvExpressManagerInstance::Instance().MakeExpress<FvTravelNodeExpress<FvTrackMotor>>();
		FV_ASSERT(pkTrackExpress);
		if(pkTrackExpress)
		{
			//! 
			pkTrackExpress->Motor().SetTransltate(kInfo.m_kFromPos);
			//! 
			pkTrackExpress->SetReserveTime(kInfo.m_fReserveTime);
			//
			AttachExpressAppearance(*pkTrackExpress, pcRes, kInfo.m_fScale);
			pkTrackExpress->Start(kInfo.m_spTo, Space(), kInfo.m_fDuration);
			return pkTrackExpress;
		}
	}
	return NULL;
}
void GxVisualAvatar::AttachExpressAppearance(FvNodeExpress& kExpress, const char* pcRes, const float fScale /* = 1.0f */)
{
	static FvUInt32 S_CNT = 0;
	++S_CNT;
	char pcName[64 + 1] = {0};
	sprintf_s(pcName, 64, "·ÉÐÐÎï%d", S_CNT);
	FvParticleAttachment *pkParticle = new FvParticleAttachment(pcRes);
	FvSmartPointer<FvZoneAttachment> pkAppearance = new FvZoneAttachment(pkParticle);
	kExpress.AttachAppearance(pkAppearance);
	if(fScale > 0.1f)
	{
		FvVector3 kScale(1.0f, 1.0f, 1.0f);
		kScale = fScale*kScale;
		pkAppearance->SetScale(kScale);
	}
}
//+------------------------------------------------------------------------------------------------------------------------------------------------------------
void GxVisualAvatar::Display(const GxVisualData::AttachExpress& kExpress)
{
	Display(kExpress, m_kAttachExpressList);
}
void GxVisualAvatar::Display(const GxVisualData::AttachExpress& kExpress, FvExpressOwnList& kList)
{
	GxVisualBuilder::Create(kExpress, m_spSpace, GetTrackBall(kExpress.m_uiAttachPos), kList);		
}
void GxVisualAvatar::Display(const GxVisualHitEffect& kHitEffect, FvExpressOwnList& kList)
{
	for (FvUInt32 uiIdx = 0; uiIdx < GxVisualHitEffect::MAX_PART; ++uiIdx)
	{
		const GxVisualData::AttachExpress& kExpressInfo = kHitEffect.m_kAttachExpress[uiIdx];
		if(!kExpressInfo.IsEmpty())
		{
			Display(kExpressInfo, kList);
		}
	}
}
void GxVisualAvatar::Display(const GxVisualHitEffect& kHitEffect)
{
	Display(kHitEffect, m_kAttachExpressList);
}
//void GxVisualAvatar::PlayAudio(const GxVisualHitEffect& kHitEffect, const FvSmartPointer<FvProvider1<FvVector3>>& spPosProvider, const FvSmartPointer<FvProvider1<FvDirection3>>& spDirProvider)
//{
//	//PlayAudio(kHitEffect, m_kAttachAudioList, spPosProvider, spDirProvider);
//}
//void GxVisualAvatar::PlayAudio(const GxVisualHitEffect& kHitEffect, GxDurationSoundOwnList& kList, const FvSmartPointer<FvProvider1<FvVector3>>& spPosProvider, const FvSmartPointer<FvProvider1<FvDirection3>>& spDirProvider)
//{
//	if(GxSoundListenerTracker::Instance().IsListened(spPosProvider->GetData0()) == false)
//	{
//		return;
//	}
//	for (FvUInt32 uiIdx = 0; uiIdx < GxVisualHitEffect::MAX_SOUND; ++uiIdx)
//	{
//		const GxVisualData::Audio& kAudioInfo = kHitEffect.m_kAudio[uiIdx];
//		if(!kAudioInfo.IsEmpty())
//		{
//			kList.Add(kAudioInfo, spPosProvider, spDirProvider);
//		}
//	}
//}
//void GxVisualAvatar::EndAllAudio()
//{
//	m_kAttachAudioList.End();
//}
//+------------------------------------------------------------------------------------------------------------------------------------------------------------
//void GxVisualAvatar::PlayAudio(const GxVisualData::SpellProc& kInfo, const FvSmartPointer<FvProvider1<FvVector3>>& spPosProvider, const FvSmartPointer<FvProvider1<FvDirection3>>& spDirProvider)
//{
//	if(GxSoundListenerTracker::Instance().IsListened(spPosProvider->GetData0()) == false)
//	{
//		return;
//	}
//	for (FvUInt32 uiIdx = 0; uiIdx < GxVisualData::SpellProc::MAX_SOUND; ++uiIdx)
//	{
//		const GxVisualData::Audio& kAudioInfo = kInfo.m_kAudio[uiIdx];
//		if(!kAudioInfo.IsEmpty())
//		{
//			m_kCastAudioList.Add(kAudioInfo, spPosProvider, spDirProvider);
//		}
//	}
//}
void GxVisualAvatar::StartCast(const GxVisualData::SpellProc& kInfo)
{
	EndCast();
	m_rpVisualInfo = &kInfo;
	FvUInt32 uiTime = 0;
	for(FvUInt32 uiIdx = 0; uiIdx < GxVisualData::SpellProc::MAX_ANIM; ++uiIdx)
	{
		const GxVisualData::Anim & kAnim = kInfo.m_kAnims[uiIdx];
		if(kAnim.IsEmpty() == false)
		{
			m_kAnimTimeFlow.SetTimeLapse(uiIdx, uiTime, &GxVisualAvatar::_OnStep);
			uiTime += kAnim.m_iDuration;
		}
	}
	for (FvUInt32 uiIdx = 0; uiIdx < GxVisualData::SpellProc::MAX_PART; ++uiIdx)
	{
		const GxVisualData::AttachExpress& kExpressInfo = kInfo.m_kAttachExpress[uiIdx];
		if(!kExpressInfo.IsEmpty())
		{
			GxVisualBuilder::Create(kExpressInfo, m_spSpace, GetTrackBall(kExpressInfo.m_uiAttachPos), m_kCastExpressList);	
		}
	}
}
void GxVisualAvatar::EndCast()
{
	m_kCastExpressList.End();
	m_rpVisualInfo = NULL;
	m_uiStep = 0;
	m_kAnimTimeFlow.Stop();
	//m_kCastAudioList.End();
}

void GxVisualAvatar::_OnStep(const FvUInt32, FvTimeEventNodeInterface&)
{
	FV_ASSERT(m_rpVisualInfo);
	FV_ASSERT(m_uiStep < GxVisualData::SpellProc::MAX_ANIM);
	const GxVisualData::Anim & kAnim = m_rpVisualInfo->m_kAnims[m_uiStep];
	SetAnim(kAnim.m_pcAnim, FvAnimatedCharacter::BREAK);
	++m_uiStep;
	FV_ASSERT(m_uiStep <= GxVisualData::SpellProc::MAX_ANIM);
}
//+------------------------------------------------------------------------------------------------------------------------------------------------------------
FvAnimatedCharacter* GxVisualAvatar::GetAvatar()const
{
	if(m_rpAvatar)
	{
		return m_rpAvatar->GetCharacter();
	}
	else
		return NULL;
}
void GxVisualAvatar::SetAnim(const char* pcAnim, const FvAnimatedCharacter::PlayType eType, float fStart, bool bBlend)
{
	if(m_rpAvatar)
	{
		FvAnimatedCharacter* pkAnims = m_rpAvatar->GetCharacter();
		if(pkAnims)
		{
			pkAnims->PlayAnimation(pcAnim, eType, fStart, bBlend);
		}
	}
}
//+------------------------------------------------------------------------------------------------------------------------------------------------------------
void GxVisualAvatar::OnAvatarNodeLoaded(const FvUInt32, GxAvatarNode&)
{
	if(GetAvatar())
	{
		GetAvatar()->LinkCacheNodes();
	}
}
//+------------------------------------------------------------------------------------------------------------------------------------------------------------
//GxVisualDurationEffectController* GxVisualAvatar::GetEffectController(const FvUInt32 uiType)
//{
//	std::map<FvUInt32, GxVisualDurationEffectController*>::const_iterator iter = m_kEffectControllerList.find(uiType);
//	if(iter == m_kEffectControllerList.end())
//	{
//		GxVisualDurationEffectController* pkController = new GxVisualDurationEffectController();
//		m_kEffectControllerList[uiType] = pkController;
//		return pkController;
//	}
//	else
//	{
//		GxVisualDurationEffectController* pkController = (*iter).second;
//		return pkController;
//	}
//}
//void GxVisualAvatar::ClearAllEffectController()
//{
//	for(std::map<FvUInt32, GxVisualDurationEffectController*>::iterator iter = m_kEffectControllerList.begin(); iter != m_kEffectControllerList.end(); ++iter)
//	{
//		GxVisualDurationEffectController* pkController = (*iter).second;
//		pkController->Clear(*this);
//		delete pkController;
//	}
//	m_kEffectControllerList.clear();
//}
////+------------------------------------------------------------------------------------------------------------------------------------------------------------
//void GxVisualAvatar::CameraAction(const FvInt32 m_iCameraAction)
//{
//	GxVisualDurationEffect_CameraShake::Exec(*this, m_iCameraAction);
//}
