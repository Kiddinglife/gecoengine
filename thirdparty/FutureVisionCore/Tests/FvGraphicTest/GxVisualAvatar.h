#pragma once

#include "GxScriptDefine.h"
#include "GxVisualAvatarAssisstant.h"

#include <FvTickTask.h>
#include <FvAvatarAttachment.h>
#include <FvZoneSpace.h>
#include <FvCallback.h>
#include <FvRotateCursor.h>
#include <FvExpress.h>
#include <FvMotor.h>
#include <FvTimeFlow.h>
#include <FvTravelExpress.h>
class  GxDeriveProvider1: public FvRefNode1<GxDeriveProvider1*>
{
public:
	GxDeriveProvider1();
	~GxDeriveProvider1(void);

	static FvRefListNameSpace<GxDeriveProvider1>::List1 S_UpdateList;

	void Update();
	void SetSrc(const FvSmartPointer<FvMotor::TrackBall>& spRef, const FvVector3& kOffset);
	const FvSmartPointer<FvMotor::TrackBall>& Dest(){return m_spDest;}

private:

	FvSmartPointer<FvMotor::TrackBall> m_spDest;
	FvSmartPointer<FvMotor::TrackBall> m_spSrc;
	FvVector3 m_kOffset;
};

class  GxVisualAvatar
{
	FV_MEM_TRACKER(GxVisualAvatar);
	FV_NOT_COPY_COMPARE(GxVisualAvatar);
public:
	static void AttachExpressAppearance(FvNodeExpress& kExpress, const char* pcRes, const float fScale = 1.0f);

	static void AddTrackPart(const FvUInt32 uiIdx, const GxDeriveProviderInfo_1& kInfo);
	static void AddTrackPart(const FvUInt32 uiIdx, const GxDeriveProviderInfo_2& kInfo);
	static bool GetTrackPartName(const FvUInt32 uiPart, const GxDeriveProviderInfo_1*& kInfo);
	static bool GetTrackPartName(const FvUInt32 uiPart, const GxDeriveProviderInfo_2*& kInfo);

	GxVisualAvatar();
	~GxVisualAvatar();

	void SetAvatarAttachment(FvAvatarAttachment& pkAvatarAttachment);
	void SetSpace(const FvZoneSpacePtr& spSpace);

	void StartCast(const GxVisualData::SpellProc& kInfo);
	void EndCast();
	void PlayAudio(const GxVisualData::SpellProc& kInfo, const FvSmartPointer<FvProvider1<FvVector3>>& spPosProvider, const FvSmartPointer<FvProvider1<FvDirection3>>& spDirProvider);

	void Display(const GxVisualData::AttachExpress& kExpress);
	void Display(const GxVisualData::AttachExpress& kExpress, FvExpressOwnList& kList);

	void Display(const GxVisualHitEffect& kHitEffect);
	void Display(const GxVisualHitEffect& kHitEffect, FvExpressOwnList& kList);

	//void PlayAudio(const GxVisualHitEffect& kHitEffect, const FvSmartPointer<FvProvider1<FvVector3>>& spPosProvider, const FvSmartPointer<FvProvider1<FvDirection3>>& spDirProvider);
	//void PlayAudio(const GxVisualHitEffect& kHitEffect, GxDurationSoundOwnList& kList, const FvSmartPointer<FvProvider1<FvVector3>>& spPosProvider, const FvSmartPointer<FvProvider1<FvDirection3>>& spDirProvider);
	void EndAllAudio();

	void CameraAction(const FvInt32 m_iCameraAction);///<ÔÝÊ±´æ·Å>

	void MakeExpress(const FvVector3& kTarget, const GxVisualData::TravelExpress& kInfo, const float fDuration);
	void MakeExpress(const FvMotor::TrackBallPtr& spTrack, const GxVisualData::TravelExpress& kInfo, const float fDuration);
	FvNodeExpress* MakeExpress(const FvTravelExpressInfo& kInfo, const char* pcRes);

	const FvZoneSpacePtr& Space()const{ return m_spSpace;}
	FvAnimatedCharacter* GetAvatar() const;
	FvAvatarAttachment* GetAvatarAttachMent() const{return m_rpAvatar;}
	FvMotor::TrackBallPtr GetTrackBall(const char* pcPart, const FvVector3& kOffset)const;
	FvMotor::TrackBallPtr GetTrackBall(const char* pcPart)const;
	FvMotor::TrackBallPtr GetTrackBall(const FvUInt32 uiPart)const;

	void SetAnim(const char* pcAnim, const FvAnimatedCharacter::PlayType eType, float fStart = 0, bool bBlend = true);

	GxAvatarRenderState& GetRenderState(){return m_kAvatarRenderState;}
	GxAnimController& BodyAnimController(){return m_kBodyAnimController;}
	void ClearAllEffectController();

	virtual void SetLinkState(const GxAvatarNode::_LinkState eState) = 0;

	const FvSmartPointer<FvProvider1<FvVector3>>& PosProvider()const{return m_spPosProvider;}
	const FvSmartPointer<FvProvider1<FvDirection3>>& DirProvider()const{return m_spDirProvider;}
	const FvMotor::TrackBallPtr& TrackBall()const{return m_spTrackBall;}
	void SetProvider(const FvSmartPointer<FvProvider1<FvVector3>>& spProvider){m_spPosProvider = spProvider;}
	void SetProvider(const FvSmartPointer<FvProvider1<FvDirection3>>& spProvider){m_spDirProvider = spProvider;}
	void SetProvider(const FvMotor::TrackBallPtr& spProvider){m_spTrackBall = spProvider;}

protected:

	void _OnStep(const FvUInt32, FvTimeEventNodeInterface&);
	FvTimeFlow<GxVisualAvatar> m_kAnimTimeFlow;

	_CALLBACK_FRIEND_CLASS_;
	void OnAvatarNodeLoaded(const FvUInt32, GxAvatarNode&);
	FvCallback0<GxAvatarNode, GxVisualAvatar, &OnAvatarNodeLoaded> m_kCB_OnAvatarNodeLoaded;

	FvAvatarAttachment* m_rpAvatar;
	FvZoneSpacePtr m_spSpace;

	FvExpressOwnList m_kAttachExpressList;
	//GxDurationSoundOwnList m_kAttachAudioList;

	GxAvatarRenderState m_kAvatarRenderState;
	//GxDurationActionLayer m_kDurationActionLayer;
	GxAnimController m_kBodyAnimController;
	//GxSpellVisualProc m_kSpellVisualProc;

	//std::map<FvUInt32, GxVisualDurationEffectController*> m_kEffectControllerList;

	///<Cast>
	FvUInt32 m_uiStep;
	const GxVisualData::SpellProc* m_rpVisualInfo;
	FvExpressOwnList m_kCastExpressList;
	//GxDurationSoundOwnList m_kCastAudioList;

	FvMotor::TrackBallPtr m_spTrackBall;
	FvSmartPointer<FvProvider1<FvVector3>> m_spPosProvider;
	FvSmartPointer<FvProvider1<FvDirection3>> m_spDirProvider;

	static std::map<FvUInt32, GxDeriveProviderInfo_1> ms_DeriveTrackBallOffsetList_1;
	static std::map<FvUInt32, GxDeriveProviderInfo_2> ms_DeriveTrackBallOffsetList_2;
};
