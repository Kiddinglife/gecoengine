
template <class T>
bool FvTupleNode<T>::CopyFrom(const FvTupleBaseNode& kOther)
{
	if(kOther.Type() == Type())
	{
		m_kValue = static_cast<const FvTupleNode&>(kOther).m_kValue;
		return true;
	}
	else
	{
		return false;
	}
}
template <class T>
bool FvBaseTuple::GetValue(const FvUInt32 uiIdx, T& kValue)const
{
	const FvTupleBaseNode* pkNode = _GetNode(uiIdx);
	if(pkNode == NULL)
	{
		return false;
	}
	if (pkNode->Type() == FvTupleNode<T>::TYPE())
	{
		const FvTupleNode<T>* pkTNode = static_cast<const FvTupleNode<T>*>(pkNode);
		kValue = pkTNode->m_kValue;
		return true;
	}
	else
	{
		return false;
	}
}
template <class T>
const T& FvBaseTuple::Value(const FvUInt32 uiIdx, const T& kDefaultValue)const
{
	const FvTupleBaseNode* pkNode = _GetNode(uiIdx);
	if(pkNode == NULL)
	{
		return kDefaultValue;
	}
	if (pkNode->Type() == FvTupleNode<T>::TYPE())
	{
		const FvTupleNode<T>* pkTNode = static_cast<const FvTupleNode<T>*>(pkNode);
		return pkTNode->m_kValue;
	}
	else
	{
		return kDefaultValue;
	}
}
template <class T>
bool FvBaseTuple::SetValue(const FvUInt32 uiIdx, const T& kValue)
{
	FvTupleBaseNode* pkNode = _GetNode(uiIdx);
	if(pkNode == NULL)
	{
		return false;
	}
	if (pkNode->Type() == FvTupleNode<T>::TYPE())
	{
		FvTupleNode<T>* pkTNode = static_cast<FvTupleNode<T>*>(pkNode);
		pkTNode->m_kValue = kValue;
		return true;
	}
	else
	{
		return false;
	}
}


//+---------------------------------------------------------------------------------

template<class T>
void FvDynamicTuple::Register()
{
	_List& kList = S_List();
	const FvUInt32 uiType = FvTupleNode<T>::TYPE();
	FV_ASSERT(kList[uiType] == CreateNULL);
	kList[uiType] = Create<T>;
}

template<class T>
void FvDynamicTuple::PushBack(const T& kT)
{
	FvTupleNode<T>* pkNode = new FvTupleNode<T>();
	pkNode->m_kValue = kT;
	m_kList.push_back(pkNode);
}