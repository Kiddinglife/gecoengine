


FV_INLINE bool FvAuraInfo::IsNotEmpty(const ValueRangeArray& kCondition)
{
	for (FvUInt32 uiIdx = 0 ; uiIdx < CONDITION_MAX; ++uiIdx)
	{
		if(kCondition.m_kDatas[uiIdx].IsEmpty())
		{
			return false;
		}
	}
	return true;
}
FV_INLINE void FvAuraValue::Clear()
{
	for (FvUInt32 uiIdx = 0; uiIdx < FvAuraInfo::VALUE_MAX; ++uiIdx)
	{
		m_kValues[uiIdx].Clear();
	}
}

FV_INLINE bool FvSpellInfo::HasSelect(const FvUInt32 uiSelectIdx)const
{
	for (FvUInt32 uiIdx = 0; uiIdx < EFFECT_MAX; ++uiIdx)
	{
		//const FvAuraInfo& kAuraEffect = m_AuraInfos[uiIdx];
		const FvAuraInfo& kAuraEffect = *(*g_pfnGetAura)(m_AuraIds[uiIdx]);
		if(!kAuraEffect.IsEmpty() && kAuraEffect.m_iSelectChannel == uiSelectIdx)
		{
			return true;
		}
		const FvHitEffectInfo& kHitEffectInfo = m_HitEffectInfos[uiIdx];
		if(!kHitEffectInfo.IsEmpty() && kHitEffectInfo.m_iSelectChannel == uiSelectIdx)
		{
			return true;
		}
	}
	return false;
}
FV_INLINE const FvAuraInfo* FvSpellInfo::GetSelectAura(const FvUInt32 uiSelectIdx, const FvUInt32 uiEffectidx)const
{
	FV_ASSERT(uiEffectidx < EFFECT_MAX);
	//const FvAuraInfo& kAuraEffect = m_AuraInfos[uiEffectidx];
	const FvAuraInfo& kAuraEffect = *(*g_pfnGetAura)(m_AuraIds[uiEffectidx]);
	if(!kAuraEffect.IsEmpty() && kAuraEffect.m_iSelectChannel == uiSelectIdx)
	{
		return &kAuraEffect;
	}
	else
	{
		return NULL;
	}
}
FV_INLINE const FvHitEffectInfo* FvSpellInfo::GetSelectHitEffect(const FvUInt32 uiSelectIdx, const FvUInt32 uiEffectidx)const
{
	FV_ASSERT(uiEffectidx < EFFECT_MAX);
	const FvHitEffectInfo& kHitEffectInfo = m_HitEffectInfos[uiEffectidx];
	if(!kHitEffectInfo.IsEmpty() && kHitEffectInfo.m_iSelectChannel == uiSelectIdx)
	{
		return &kHitEffectInfo;
	}
	else
	{
		return NULL;
	}
}
FV_INLINE const FvAuraInfo* FvSpellInfo::GetAura(const FvUInt32 uiIdx)const
{
	if(uiIdx < EFFECT_MAX)
	{
		//const FvAuraInfo& kInfo = m_AuraInfos[uiIdx];
		const FvAuraInfo& kInfo = *(*g_pfnGetAura)(m_AuraIds[uiIdx]);
		if(!kInfo.IsEmpty())
		{
			return &kInfo;
		}
	}
	return NULL;
}
FV_INLINE const FvHitEffectInfo* FvSpellInfo::GetHitEffect(const FvUInt32 uiIdx)const
{
	if(uiIdx < EFFECT_MAX)
	{
		const FvHitEffectInfo& kInfo = m_HitEffectInfos[uiIdx];
		if(!kInfo.IsEmpty())
		{
			return &kInfo;
		}
	}
	return NULL;
}
