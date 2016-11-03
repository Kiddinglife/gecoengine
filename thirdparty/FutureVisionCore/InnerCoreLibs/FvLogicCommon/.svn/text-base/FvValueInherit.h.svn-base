//{future header message}
#ifndef __FvValueInherit_H__
#define  __FvValueInherit_H__

#include "FvMemoryNode.h"
#include <FvRefList.h>


template<class T>
class FvValueInherit: private FvRefNode1<FvValueInherit<T>*>
{
public:

	FvValueInherit(void):m_fPerc(0.0f){}
	~FvValueInherit(void){}

	void Update(const T& oldValue, const T& newValue);
	void Init(const float fPerc, const T& srcValue, const bool bUpdate);
	void SetPerc(const float fPerc);
	void Detach(const bool bUpdate);
	const T& GetInheritValue()const{return m_InheritValue;}
	bool IsAttach()const{return FvRefNode1::IsAttach();}

private:

	virtual void _OnUpdated(const T& oldInheritValue, const T& newInheritValue){}
	virtual void _OnAttach(const bool bUpdate){}
	virtual void _OnDetach(const bool bUpdate){}

	T m_InheritValue;
	
private:

	float m_fPerc;
	T m_SrcValue;

	template<class T> friend class FvValueInheritList;
};

template<class T>
class FvValueInheritList
{
public:
	typedef FvRefList<FvValueInherit<T>*, FvRefNode1> List;

	void OnUpdate(const T& kOld, const T& kNew);
	void AttachBack(FvValueInherit<T>& kNode);
	void DetachAll(const bool bUpdate);

private:
	List m_kList;
};

#include "FvValueInherit.inl"
#include <FvKernel.h>

namespace ValueInherit_Demo
{
	static void Test()
	{
		FvInt32 uiSrcValue = 100;
		FvValueInheritList<FvInt32> kList;
		FvValueInherit<FvInt32> kValueInherit;
		kValueInherit.Init(1.0f, uiSrcValue, true);
		kList.AttachBack(kValueInherit);
		kList.OnUpdate(200, uiSrcValue);
		uiSrcValue = 200;
		kList.OnUpdate(300, uiSrcValue);
		uiSrcValue = 300;
		kValueInherit.Detach(true);
	}
}

#endif //__FvValueInherit_H__


