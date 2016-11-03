

template <class T, void (T::*OnActive)(), void (T::*OnDeactive)()>
T* FvPriorityList<T, OnActive, OnDeactive>::GetTop()const
{
	if(IsEmpty())
	{
		return NULL;
	}
	else
	{
		List::Iterator iter = m_kList.GetHead();
		T* pkNode = iter->Content();
		FV_ASSERT(pkNode);
		return pkNode;
	}
}
template <class T, void (T::*OnActive)(), void (T::*OnDeactive)()>
bool FvPriorityList<T, OnActive, OnDeactive>::Attach(T& kNode)
{
	if(m_kList.IsEmpty())
	{
		m_kList.PushBack(kNode);
		(kNode.*OnActive)();
		_OnUpdated(NULL);
		return true;
	}
	else
	{
		List::Iterator iter = m_kList.GetHead();
		T* pkOldTop =iter->Content();
		while (!m_kList.IsEnd(iter))
		{
			T* pkNode = iter->Content();
			FV_ASSERT(pkNode);
			if(kNode.Weight() > pkNode->Weight())
			{
				break;
			}
			List::Next(iter);
		}
		m_kList.PushBefore(*iter, kNode);
		if(&kNode == m_kList.GetHead())
		{
			(*pkOldTop.*OnDeactive)();
			(kNode.*OnActive)();
			_OnUpdated(pkOldTop);
			return true;
		}
		else
		{
			return false;
		}
	}
}

template <class T, void (T::*OnActive)(), void (T::*OnDeactive)()>
bool FvPriorityList<T, OnActive, OnDeactive>::Remove(T& kNode)
{
	if(kNode.IsAttach() == false)
	{
		return false;
	}
	if(m_kList.IsEmpty())
	{
		return false;
	}
	if(&kNode == m_kList.GetHead())
	{
		kNode.FvDoubleLinkNode1<T*>::Detach();
		(kNode.*OnDeactive)();
		if(!m_kList.IsEmpty())
		{
			T* pNewTop = m_kList.GetHead()->Content();
			FV_ASSERT(pNewTop);
			(*pNewTop.*OnActive)();
		}
		_OnUpdated(&kNode);
		return true;
	}
	else
	{
		kNode.FvDoubleLinkNode1<T*>::Detach();
		return false;
	}
}