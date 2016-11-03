#include "FvAreaAura.h"

FvAreaAura::FvAreaAura(void)
{
	m_kWorldAttachNode.m_Content = this;
	m_kSpellFlowAttachNode.m_Content = this;
}

FvAreaAura::~FvAreaAura(void)
{
}
void FvAreaAura::AttachToWorld(FvRefList<FvAreaAura*>& kList)
{
	kList.AttachBack(m_kWorldAttachNode);
}
void FvAreaAura::AttachToSpellFlow(FvRefList<FvAreaAura*>& kList)
{
	kList.AttachBack(m_kSpellFlowAttachNode);
}