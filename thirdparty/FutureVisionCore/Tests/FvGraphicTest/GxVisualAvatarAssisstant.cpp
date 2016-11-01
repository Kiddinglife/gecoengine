#include "GxVisualAvatar.h"

//+----------------------------------------------------------------------------------------------------
GxAnimController::GxAnimController()
:m_rpAvatar(NULL)
{}

GxAnimController::~GxAnimController()
{}

void GxAnimController::SetAct(const char* pcAnim, const FvAnimatedCharacter::PlayType eType, float fStart, bool bBlend)
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
void GxAnimController::CloseActGroup(const char* pcGroup)
{
	if(m_rpAvatar)
	{
		FvAnimatedCharacter* pkAnims = m_rpAvatar->GetCharacter();
		if(pkAnims)
		{
			pkAnims->RemoveLevel(pcGroup);
		}
	}
}
void GxAnimController::SetScale(const char* pcGroup, const float fScale)
{
	if(m_rpAvatar)
	{
		FvAnimatedCharacter* pkAnims = m_rpAvatar->GetCharacter();
		if(pkAnims)
		{
			pkAnims->SetScale(pcGroup, fScale);
		}
	}
}
void GxAnimController::SetActTime(const char* pcAnim, const float fTimePos)
{
	if(m_rpAvatar)
	{
		FvAnimatedCharacter* pkAnims = m_rpAvatar->GetCharacter();
		if(pkAnims)
		{
			FvAnimationState* pkState = pkAnims->GetAnimationState(pkAnims->GetAnimation(pcAnim));
			if(pkState) pkState->SetTimePosition(fTimePos);
		}
	}
}
void GxAnimController::Init(FvAvatarAttachment& kAvatar)
{
	m_rpAvatar = &kAvatar;
}
void GxAnimController::Fini()
{
	m_rpAvatar = NULL;
}


//----------------------------------------------------------------------------
GxAvatarNode::GxAvatarNode()
:m_pkOldNode(NULL)
,m_eState(LINK_1)
,m_bLoaded(false)
{
}
GxAvatarNode::~GxAvatarNode()
{
	FV_SAFE_DELETE(m_pkOldNode);
}
void GxAvatarNode::AttachOldNode(GxAvatarNode* pkOldNode)
{
	if(pkOldNode == NULL)
	{
		return;
	}
	if(pkOldNode->m_pkOldNode)
	{
		m_pkOldNode = pkOldNode->m_pkOldNode;
		pkOldNode->m_pkOldNode = NULL;
		pkOldNode->PutOff();
		delete pkOldNode;
	}
	else
	{
		m_pkOldNode = pkOldNode;
	}
}
bool GxAvatarNode::LoadFinishAll()
{
	if(m_pkOldNode)
	{
		m_pkOldNode->PutOff();
		FV_SAFE_DELETE(m_pkOldNode);
	}
	m_bLoaded = true;
	m_kCallbackList.Invoke(0, *this);
	return false;
}
//
////+---------------------------------------------------------------------------------------------------------------------------------------------
////+----------------------------------------------------------------------------------------------------
//
//FvOwnPtr<FvRenderStateRes> GxAvatarRenderStateRes::ms_opRenderStateRes;

GxAvatarRenderState::GxAvatarRenderState()
{
	//m_opState = new  FvRenderState(ms_opRenderStateRes);;
}
GxAvatarRenderState::~GxAvatarRenderState()
{
	m_opState = NULL;
}

void GxAvatarRenderState::Update(const float fDeltaTime, FvAnimatedCharacter& kAvatar)
{
	if(m_opState)
	{
		m_opState->AddTime(fDeltaTime);
		m_opState->Update(&kAvatar, 10.0f);
	}
}
FvRenderState& GxAvatarRenderState::State()
{
	FV_ASSERT(!m_opState.IsNULL())
		return *m_opState;
}
/////////////////////////////////////////////////////////////

FvRefListNameSpace<GxDeriveProvider1>::List1 GxDeriveProvider1::S_UpdateList;


GxDeriveProvider1::GxDeriveProvider1(void)
:m_kOffset(0,0,0)
{
	m_spDest = new FvMotor::TrackBall();
}

GxDeriveProvider1::~GxDeriveProvider1(void)
{
}

void GxDeriveProvider1::SetSrc(const FvSmartPointer<FvMotor::TrackBall>& spRef, const FvVector3& kOffset)
{
	if(spRef)
	{
		m_spSrc = spRef;
		m_kOffset = kOffset;
		S_UpdateList.AttachBack(*this);
		Update();
	}
}
void GxDeriveProvider1::Update()
{
	FV_ASSERT_ERROR(m_spSrc);
	FV_ASSERT_ERROR(m_spDest);
	m_spDest->SetValue0(m_spSrc->GetData0() + m_kOffset);
	m_spDest->SetValue1(m_spSrc->GetData1());
}