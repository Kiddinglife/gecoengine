#pragma once

#include "GxScriptDefine.h"
#include "GxVisualStruct.h"

#include <FvTickTask.h>
#include <FvAvatarAttachment.h>
#include <FvCallback.h>
#include <FvRenderState.h>
#include <FvInitSingleton.h>
#include <FvOwnPtr.h>

class  GxAnimController
{
public:
	GxAnimController();
	~GxAnimController();

	void SetAct(const char* pcAnim, const FvAnimatedCharacter::PlayType eType, float fStart = 0, bool bBlend = true);
	void CloseActGroup(const char* pcGroup);
	void SetScale(const char* pcGroup, const float fScale);
	void SetActTime(const char* pcAnim, const float fTimePos);

	void Init(FvAvatarAttachment& kAvatar);
	void Fini();

private:

	FvAvatarAttachment* m_rpAvatar;
};

//+----------------------------------------------------------------------------------------------------
class  GxAvatarNode : public FvAnimatedCharacterNode
{
	FV_MEM_TRACKER(GxAvatarNode);
	FV_NOT_COPY_COMPARE(GxAvatarNode);
public:
	GxAvatarNode();
	virtual ~GxAvatarNode();

	enum _LinkState
	{
		LINK_1,
		LINK_2,
	};

	void AttachOldNode(GxAvatarNode* pkOldNode);
	virtual void SetLinkState(const _LinkState uiState) = 0;

	virtual bool LoadFinishAll();
	bool IsLoaded()const{return m_bLoaded;}

	FvCallbackList0<GxAvatarNode>& CallbackList(){return m_kCallbackList;}

protected:

	GxAvatarNode* m_pkOldNode;

	FvAnimatedCharacter::ProviderPtr m_spTrackBall;

	_LinkState m_eState;
	bool m_bLoaded;

	FvCallbackList0<GxAvatarNode> m_kCallbackList;

};


class  GxAvatarRenderStateRes: public FvInitSingleton<GxAvatarRenderStateRes>
{
public:
	GxAvatarRenderStateRes()
	{
		Ogre::DataStreamPtr spConfigStream =
			Ogre::ResourceGroupManager::getSingleton().openResource(
			"Environments/RenderState.xml", "General", true);
		FvXMLSectionPtr spRenderState = FvXMLSection::OpenSection(spConfigStream);

		ms_opRenderStateRes = new FvRenderStateRes(spRenderState);
	}
	~GxAvatarRenderStateRes()
	{
		ms_opRenderStateRes = NULL;
	}
	static FvOwnPtr<FvRenderStateRes> ms_opRenderStateRes;
};

class  GxAvatarRenderState
{
public:
	GxAvatarRenderState();
	~GxAvatarRenderState();

	void Update(const float fDeltaTime, FvAnimatedCharacter& kAvatar);

	FvRenderState& State();

private:

	FvOwnPtr<FvRenderState> m_opState;
};