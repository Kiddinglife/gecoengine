


template<class T>
void FvValueInherit<T>::Update(const T& oldValue, const T& newValue)
{
	if(IsAttach())
	{
		const T oldInheritValue = m_InheritValue;
		m_SrcValue = newValue;
		m_InheritValue = T(m_fPerc*m_SrcValue);
		_OnUpdated(oldInheritValue, m_InheritValue);
	}
}

template<class T>
void FvValueInherit<T>::Init(const float fPerc, const T& srcValue, const bool bUpdate)
{
	if(IsAttach())
	{
		return;
	}
	m_SrcValue = srcValue;
	m_fPerc = fPerc;
	m_InheritValue = T(m_fPerc*m_SrcValue);
	_OnAttach(bUpdate);
}

template<class T>
void FvValueInherit<T>::SetPerc(const float fPerc)
{	
	m_fPerc = fPerc;
	if(IsAttach())
	{
		_OnDetach(true);
		m_InheritValue = T(m_fPerc*m_SrcValue);
		_OnAttach(true);
	}
}

template<class T>
void FvValueInherit<T>::Detach(const bool bUpdate)
{
	if(IsAttach())
	{
		FvRefNode1::Detach();
		_OnDetach(bUpdate);
		m_fPerc = 0.0f;
	}
}


template<class T>
void FvValueInheritList<T>::OnUpdate(const T& kOld, const T& kNew)
{
	m_kList.EndIterator();
	m_kList.BeginIterator();
	while (!m_kList.IsEnd())
	{
		FvValueInherit<T>* pkInherit = m_kList.GetIterator()->Content();
		m_kList.Next();
		FV_ASSERT(pkInherit);
		pkInherit->Update(kOld, kNew);
	}
}
template<class T>
void FvValueInheritList<T>::AttachBack(FvValueInherit<T>& kNode)
{
	if(!kNode.IsAttach())
	{
		m_kList.AttachBack(kNode);
	}
}
template<class T>
void FvValueInheritList<T>::DetachAll(const bool bUpdate)
{
	m_kList.EndIterator();
	m_kList.BeginIterator();
	while (!m_kList.IsEnd())
	{
		FvValueInherit<T>* pkInherit = m_kList.GetIterator()->Content();
		m_kList.Next();
		FV_ASSERT(pkInherit);
		pkInherit->Detach(bUpdate);
	}
}