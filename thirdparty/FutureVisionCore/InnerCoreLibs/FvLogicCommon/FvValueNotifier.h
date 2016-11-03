//{future header message}
#ifndef __FvValueNotifier_H__
#define  __FvValueNotifier_H__


#include "FvLogicCommonDefine.h"
#include "FvMemoryNode.h"
#include "FvNotifier.h"
#include <FvRefList.h>

class FV_LOGIC_COMMON_API FvValueNotifierInterface
{
	FV_MEM_TRACKER(FvValueNotifierInterface);
public:
	enum State
	{
		NONE,
		NO_MATCH,
		MATCH,
	};

	State GetState()const{return m_eState;}
	void Notify();
	
protected:

	FvValueNotifierInterface():m_eState(NONE){FV_MEM_TRACK;}
	~FvValueNotifierInterface(){}

	void _SetState(const State eState){m_eState = eState;}

	virtual void _OnMatch()=0;
	virtual void _OnNoMatch()=0;

private:
	State m_eState;
};

//+----------------------------------------------------------------------------------------------------
class FV_LOGIC_COMMON_API FvValueRangeNotifier: public FvValueNotifierInterface, private FvRefNode1<FvValueRangeNotifier*>
{
public:
	typedef FvRefListNameSpace<FvValueRangeNotifier>::List1 List;
	static void OnValueUpdated(const FvInt32& oldValue, const FvInt32& newValue, List& kList);

	FvValueRangeNotifier(void);
	~FvValueRangeNotifier(void){}

	void OnModified(const FvInt32 iOldValue, const FvInt32 iNewValue);
	bool IsMatch(const FvInt32 iValue)const{return (m_iMin <= iValue && iValue <= m_iMax);}

	void Init(const FvInt32 iCurrentValue, const FvInt32 iMin, const FvInt32 iMax);
	void AttachTo(List& kList){kList.AttachBack(*this);}
	void Detach();
	bool IsAttach()const{return FvRefNode1::IsAttach();}

	FvInt32 MaxValue()const{return m_iMax;}
	FvInt32 MinValue()const{return m_iMin;}
	FvInt32 CurValue()const{return m_iValue;}
	
private:
	FvInt32 m_iMax;
	FvInt32 m_iMin;
	FvInt32 m_iValue;
};

//+----------------------------------------------------------------------------------------------------
class FV_LOGIC_COMMON_API FvValueMaskNotifier: public FvValueNotifierInterface, private FvRefNode1<FvValueMaskNotifier*>
{
public:
	typedef FvRefListNameSpace<FvValueMaskNotifier>::List1 List;
	static void OnValueUpdated(const FvInt32& oldValue, const FvInt32& newValue, List& kList);

	void Init(const FvUInt32 uiCurState,  const FvUInt32 uiMatchMask);
	void OnModified(const FvUInt32 oldValue, const FvUInt32 newValue);

	void Detach();
	void AttachTo(List& kList){kList.AttachBack(*this);}
	bool IsAttach()const{ return FvRefNode1::IsAttach();}

	virtual bool IsMatch(const FvUInt32 uiMask)const = 0;

protected:

	FvValueMaskNotifier():m_uiMatchMask(0){}
	~FvValueMaskNotifier(){}

	FvUInt32 m_uiMatchMask;
};

//+----------------------------------------------------------------------------------------------------
class FV_LOGIC_COMMON_API FvValueMaskAnyNotifier: public FvValueMaskNotifier
{
public:
	FvValueMaskAnyNotifier(void){}
	~FvValueMaskAnyNotifier(void){}	

private:
	virtual bool IsMatch(const FvUInt32 uiMask)const;
};

//+----------------------------------------------------------------------------------------------------
class FV_LOGIC_COMMON_API FvValueMaskAllNotifier: public FvValueMaskNotifier
{

public:
	FvValueMaskAllNotifier(void){}
	~FvValueMaskAllNotifier(void){}

private:
	virtual bool IsMatch(const FvUInt32 uiMask)const;
};




#endif //__FvValueNotifier_H__
