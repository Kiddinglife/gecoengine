
#include "FvTarget.h"

#include "FvGameUnitAppearance.h"


FvTarget::FvTarget(void)
:m_uiMask(FOCUS_NONE)
,m_kPos(0,0,0)
{}

FvTarget::~FvTarget(void)
{
}

FvGameUnitAppearance* FvTarget::Obj()const
{
	if(m_uiMask == FOCUS_OBJ)
	{
		return m_rpObj;
	}
	return NULL;
}
void FvTarget::Set(FvGameUnitAppearance& kObj)
{
	m_uiMask = FOCUS_OBJ; 
	m_rpObj = &kObj; 
}
const FvVector3* FvTarget::Pos()const
{
	if(m_uiMask == FOCUS_POS)
	{
		return &m_kPos;
	}
	return NULL;
}
void FvTarget::Set(const FvVector3& pos)
{
	m_uiMask = FOCUS_POS; 
	m_kPos = pos;
}
void FvTarget::Clear()
{
	m_uiMask = FOCUS_NONE;
	m_rpObj = NULL;
	m_kPos = FvVector3::ZERO;
}