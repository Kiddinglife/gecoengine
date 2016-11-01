#include "FvAreaHitEffect.h"

FvRefList<FvAreaHitEffect*> FvAreaHitEffect::S_WorldList;
void FvAreaHitEffect::ClearWorldList()
{
	S_WorldList.BeginIterator();
	while (!S_WorldList.IsEnd())
	{
		FvRefList<FvAreaHitEffect*>::iterator iter = S_WorldList.GetIterator();
		FvAreaHitEffect* pkEffect = (*iter).Content();
		FV_ASSERT(pkEffect);
		S_WorldList.Next();
		pkEffect->End();
	}
}
FvAreaHitEffect::FvAreaHitEffect(void)
:m_kPos(0,0,0)
{
	m_kAttachToWorld.m_Content = this;
	WorldList().AttachBack(m_kAttachToWorld);
}

FvAreaHitEffect::~FvAreaHitEffect(void)
{
}

void FvAreaHitEffect::SetPosition(const FvVector3& kPos)
{
	m_kPos = kPos;
}
const FvVector3& FvAreaHitEffect::_Position()const
{
	return m_kPos;
}
void FvAreaHitEffect::_OnEnd()
{
	m_kAttachToWorld.Detach();
}