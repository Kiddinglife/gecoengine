//{future header message}
#ifndef __FvAura_H__
#define  __FvAura_H__

#include "FvLogicServerDefine.h"

#include "FvSpellCasterInfo.h"

#include <FvSpellDefine.h>
#include <FvSpellStruct.h>
#include <FvReferencePtr.h>
#include <FvTimeNodeEx.h>
#include <FvMemoryNode.h>


#define FV_AURA_CONSTRUCT(CLASS_NAME, BASE_CLASS_NAME )\
	CLASS_NAME():BASE_CLASS_NAME(){}

#define FV_AURA_DEFINE(CLASS_NAME, TYPE)  CLASS_NAME::Create//! 暂时没法加入CLASS_NAME&TYPE映射


class FvGameUnitAppearance;

class FV_LOGIC_SERVER_API FvAura
{
	FV_MEM_TRACKER(FvAura);
	FV_NOT_COPY_COMPARE(FvAura);

public:
	FV_REFERENCE_HEAD;

	static FvInt32 GetCost(const FvGameUnitAppearance& kObj, const FvIdxValueRange& kCost, const FvRefIdxValue& kRef0Cost, const FvRefIdxValue& kRef1Cost);
	
	static bool CanDispel(FvAura& kAura); 
	static FvUInt32 GetDuration(const FvAuraInfo& kInfo);

	FvUInt64 GetDuration()const;

	void Start();
	void Start(const FvUInt32 uiDelayTime);
	void End();
	void End(const FvUInt32 uiDelayTime);

	virtual void Load(const FvUInt64 uiDuration, const bool bUpdate);
	virtual void Save(const bool bUpdate);

	virtual FvUInt32 GetState()const = 0;
	bool HasState(const FvAuraState::Mask& eMask)const;

	//+-----------------------------------------------------------------------------
	void ModDurTime(const FvInt32 iDeltaTime);
	void SetDurTime(const FvUInt32 uiDuration);

	//+-----------------------------------------------------------------------------
	virtual void Active(const bool bUpdate);
	virtual void Deactive(const bool bUpdate);

	//+-----------------------------------------------------------------------------
	FvAura* FatherAura()const{return m_rpFather;} 
	void SetFatherAura(FvAura& kFather){m_rpFather = &kFather;}

	virtual FvSpellID SpellId()const = 0;
	virtual FvSpellEffectIdx EffectIdx()const = 0;
	virtual void SetCasterValue(const FvAuraValue& kValue) = 0;
	virtual FvInt32 GetCasterValueBase(const FvUInt32 uiIdx)const = 0;
	virtual FvInt32 GetCasterValueMod(const FvUInt32 uiIdx)const = 0;

	void SetInfo(const FvAuraInfo& kInfo){m_rpInfo = &kInfo;}
	const FvAuraInfo& GetInfo()const; 
	bool GetMiscValue(const char* pcName, FvInt32& iValue)const;
	FvInt32 MiscValue(const char* pcName, const FvInt32 iDefaultValue = 0)const;

	bool IsOwn()const;
	void SetOwn(const bool bOwn);

protected:

	FvAura();
	virtual ~FvAura(void);

	//+-----------------------------------------------------------------------------
	//! 需要重载的接口
	virtual void _OnStart();
	virtual void _OnEnd();

	virtual void _OnLoad(const bool bUpdate);
	virtual void _OnSave(const bool bUpdate);

	virtual void _OnActive(const bool bUpdate){}
	virtual void _OnTick(){}
	virtual void _OnDeactive(const bool bUpdate){}

	//+-----------------------------------------------------------------------------
	//! 属性设置应用

	void _AddState(const FvAuraState::Mask& eMask);
	void _DelState(const FvAuraState::Mask& eMask);

	virtual void _SetEndTime(const FvUInt64& uiTime) = 0;
	virtual void _SetState(const FvUInt32& eMask) = 0;
	virtual void _SetCasterID(const FvUInt64& uiCaster) = 0;

private:

	//+----------------------------------------------------------------------------
	//! 时间驱动
	_FRIEND_CLASS_TimeNodeEvent_;
	void _OnStart(FvTimeEventNodeInterface&);
	FvTimeEventNode2<FvAura, &_OnStart> m_kTimeStartEvent;
	////
	void _OnTimeTick(FvTimeEventNodeInterface&);
	void _OnEnd(FvTimeEventNodeInterface&);
	FvTimeEventNode4<FvAura, &_OnTimeTick, &_OnEnd> m_kTimeTickEvent;

	//+-----------------------------------------------------------------------------
	//! 状态控制
	void _Destroy();

private:
	//+-----------------------------------------------------------------------------	
	FvReferencePtr<FvAura> m_rpFather;

	const FvAuraInfo* m_rpInfo;/// 不会为空
	
	friend class FvDeletable;

};

class FV_LOGIC_SERVER_API FvAuraOwnInterface
{
public:
	virtual ~FvAuraOwnInterface(){}
	virtual void End() = 0;
protected:
	FvAuraOwnInterface(){}
};


namespace Aura_Demo
{
	class TestAura: public FvAura
	{
	public:
		TestAura::TestAura():FvAura(){}

		virtual void _AppearActive(){}
		virtual void _AppearTick(){}
		virtual void _AppearDeactive(){}

		virtual void _OnActive(){}
		virtual void _OnTick(){}
		virtual void _OnDeactive(){}

		virtual FvUInt32 GetState()const{return 0;}
		virtual void _SetState(const FvUInt32& eState){}

		virtual FvSpellID SpellId()const{return 0;}
		virtual FvSpellEffectIdx EffectIdx()const{return 0;}
		virtual FvUInt64 MagicKey()const{return 0;}
		virtual void SetCasterValue0(const FvInt32& uiValue){}
		virtual void SetCasterValue1(const FvInt32& uiValue){}
		virtual FvInt32 _GetCasterInfo0()const{return 0;}
		virtual FvInt32 _GetCasterInfo1()const{return 0;}
		virtual void _SetDuration(const FvUInt64& uiTime){}
		virtual void _SetCasterID(const FvUInt64& uiCaster){}
	};

	static void Test()
	{
		//FvFastTimer kTimer;
		//kTimer.Start(0);
		////FvAura::SetTimer(kTimer);
		//FvReferencePtr<TestAura> aura = new TestAura();
		//aura->SetTimeTick(5, 20);
		//aura->Start(10);

		//FvUInt64 uiTime = 10;
		//kTimer.Update(uiTime);
		//while (aura != NULL)
		//{
		//	kTimer.Update(uiTime);
		//	uiTime += 10;
		//}
	}
	
}

#endif //__FvAura_H__
