



//! 发出回调
template <class From>
void FvAsynCaller<From>::Invoke(const FvUInt32 uiEvent, From& kFrom)
{
	if(uiEvent < m_CallbackRefMap.size())
	{
		FV_ASSERT(m_CallbackRefMap[uiEvent]);
		if(m_CallbackRefMap[uiEvent])
		{
			m_CallbackRefMap[uiEvent]->Invoke(uiEvent, kFrom);
		}
	}
	else
	{
		FvLogBus::Warning("FvFastCaller::Invoke越界");
	}
}

template <class From>
void FvAsynCaller<From>::SetEventSize(const FvUInt32 uiEventTotal)
{
	Clear();
	m_CallbackRefMap.resize(uiEventTotal);
	for(FvUInt32 uiIdx = 0; uiIdx < uiEventTotal; ++uiIdx)
	{
		m_CallbackRefMap[uiIdx] = new _List();
	}
}

template <class From>
void FvAsynCaller<From>::Clear()
{
	for(FvUInt32 uiIdx = 0; uiIdx < m_CallbackRefMap.size(); ++uiIdx)
	{
		FV_ASSERT(m_CallbackRefMap[uiIdx]);
		if(m_CallbackRefMap[uiIdx])
		{
			delete m_CallbackRefMap[uiIdx];
		}
	}
	m_CallbackRefMap.clear();
}