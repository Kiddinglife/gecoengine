
template<class TData, class TAction>
TAction* FvPriorityActionList<TData, TAction>::GetActive()const
{
	List::Iterator iter = m_kList.GetHead();
	while (!m_kList.IsEnd(iter))
	{
		TAction* pkIterAction = iter->Content();
		FV_ASSERT(pkIterAction);
		List::Next(iter);
		if(pkIterAction->IsActive())
		{
			return pkIterAction;
		}
	}
	return NULL;
}

template<class TData, class TAction>
void FvPriorityActionList<TData, TAction>::Attach(TAction& kAction)
{
	List::Iterator iter = m_kList.GetHead();
	while (!m_kList.IsEnd(iter))
	{
		TAction* pkIterAction = iter->Content();
		FV_ASSERT(pkIterAction);
		if(kAction.Weight() > pkIterAction->Weight())
		{
			break;
		}
		List::Next(iter);
	}
	m_kList.PushBefore(*iter, kAction);
}

template<class TData, class TAction>
void FvPriorityActionList<TData, TAction>::Reset(TData& kData)
{
	TAction* rpOldActiveAction = GetActive();
	List::Iterator iter = m_kList.GetHead();
	while (!m_kList.IsEnd(iter))
	{
		TAction* pkIterAction = iter->Content();
		FV_ASSERT(pkIterAction);
		List::Next(iter);
		if(pkIterAction->IsReady(kData))
		{
			if(pkIterAction != rpOldActiveAction)
			{
				if(rpOldActiveAction)
				{
					rpOldActiveAction->OnEnd();
				}
				FV_ASSERT(pkIterAction->IsActive() == false);
				pkIterAction->OnStart();
				FV_ASSERT(pkIterAction->IsActive());
			}
			break;
		}
	}
}