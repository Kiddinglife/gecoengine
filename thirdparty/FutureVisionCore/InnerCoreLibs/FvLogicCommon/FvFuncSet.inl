
//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

template<class ReturnType, class Param0, class Param1, class Param2>
void FvFuncVector< ReturnType, Param0, Param1, Param2>::SetSize(const FvUInt32 uiSize)
{
	if(Size() == 0)
	{
		m_kFuncList.resize(uiSize, NULL);
	}
	else
	{
		FvLogBus::Error("FvFuncVector::SetSize 多次");
	}
}
template<class ReturnType, class Param0, class Param1, class Param2>
void FvFuncVector< ReturnType, Param0, Param1, Param2>::Add(const FvUInt32 uiIdx, Func pFunc)
{
	if(uiIdx >= m_kFuncList.size())
	{
		FvLogBus::Error("FvFuncVector::Add 越界");
		return;
	}
	if(m_kFuncList[uiIdx] != NULL)
	{
		FvLogBus::Warning("FvFuncVector::Add 多次");
		return;
	}
	m_kFuncList[uiIdx] = pFunc;
}

//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


template<class ReturnType, class Param0, class Param1>
void FvFuncVector< ReturnType, Param0, Param1>::SetSize(const FvUInt32 uiSize)
{
	if(Size() == 0)
	{
		m_kFuncList.resize(uiSize, NULL);
	}
	else
	{
		FvLogBus::Error("FvFuncVector::SetSize 多次");
	}
}
template<class ReturnType, class Param0, class Param1>
void FvFuncVector< ReturnType, Param0, Param1>::Add(const FvUInt32 uiIdx, Func pFunc)
{
	if(uiIdx >= m_kFuncList.size())
	{
		FvLogBus::Error("FvFuncVector::Add 越界");
		return;
	}
	if(m_kFuncList[uiIdx] != NULL)
	{
		FvLogBus::Warning("FvFuncVector::Add 多次");
		return;
	}
	m_kFuncList[uiIdx] = pFunc;
}

//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

template<class ReturnType, class Param0>
void FvFuncVector< ReturnType, Param0>::SetSize(const FvUInt32 uiSize)
{
	if(Size() == 0)
	{
		m_kFuncList.resize(uiSize, NULL);
	}
	else
	{
		FvLogBus::Error("FvFuncVector::SetSize 多次");
	}
}
template<class ReturnType, class Param0>
void FvFuncVector< ReturnType, Param0>::Add(const FvUInt32 uiIdx, Func pFunc)
{
	if(uiIdx >= m_kFuncList.size())
	{
		FvLogBus::Error("FvFuncVector::Add 越界");
		return;
	}
	if(m_kFuncList[uiIdx] != NULL)
	{
		FvLogBus::Warning("FvFuncVector::Add 多次");
		return;
	}
	m_kFuncList[uiIdx] = pFunc;
}
//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

template<class ReturnType>
void FvFuncVector< ReturnType>::SetSize(const FvUInt32 uiSize)
{
	if(Size() == 0)
	{
		m_kFuncList.resize(uiSize, NULL);
	}
	else
	{
		FvLogBus::Error("FvFuncVector::SetSize 多次");
	}
}
template<class ReturnType>
void FvFuncVector< ReturnType>::Add(const FvUInt32 uiIdx, Func pFunc)
{
	if(uiIdx >= m_kFuncList.size())
	{
		FvLogBus::Error("FvFuncVector::Add 越界");
		return;
	}
	if(m_kFuncList[uiIdx] != NULL)
	{
		FvLogBus::Warning("FvFuncVector::Add 多次");
		return;
	}
	m_kFuncList[uiIdx] = pFunc;
}
//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------