//{future header message}
#ifndef __FvAnimatedCharacter_H__
#define __FvAnimatedCharacter_H__

#include "FvAnimatedEntity.h"
#include "FvAnimatedCharacterResource.h"
#include <FvRefList.h>
#include <FvDoubleLink.h>
#include <FvSmartPointer.h>
#include <FvDebug.h>
#include <FvBGTaskManager.h>
#include <FvProvider.h>
#include <FvQuaternion.h>

namespace ParticleUniverse
{
	class ParticleSystem;
}

#define FV_MAX_BINDING (4)

class FvAnimatedCharacterBase;

class FV_MODEL_API FvAnimationState
{
public:

	class Callback
	{
	public:
		virtual void KeyEventCome(FvAnimationState* pkAnimationState,
			const char* pcMessage) = 0;
	protected:
		virtual ~Callback() {};
	};

	enum AnimationStateType
	{
		PARTITION,
		EXTRA,
		UNKNOWN
	};

	FvAnimationState(FvAnimatedCharacterBase* pkParent, FvUInt32 u32Index);

	virtual ~FvAnimationState();

	bool GetLoop();

	float GetLength();

	bool GetManualTime();

	void SetTimePosition(float fTimePos);

	float GetTimePosition();

	void SetTimeScale(float fTimePos);

	float GetTimeScale();

	AnimationStateType GetType();

	FvAnimatedCharacterBase* GetParent();

	void SetCallback(Callback* pkCallback);

	Callback* GetCallback();

	FvUInt32 GetIndex();

	const char* GetName();

	const FvAnimatedCharacterResource::KeySet& GetKeySet();

	void SetContinuous(bool bEnable);

	bool GetContinuous();

protected:
	friend class FvAnimatedCharacterBase;
	friend class FvAnimatedCharacter;

	void EnableBlendMask(const char* pcName);

	void DisableBlendMask();

	void SetLoop(bool bLoop);

	void SetManualTime(bool bManual);

	bool UpdateTimePosition(float fTimePos);

	void SetWeightArray(float* pfArray);

	float* GetWeightArray() const;

	void SetExtraData(unsigned int* puiExtraData);

	unsigned int GetExtraData() const;

	bool AddTime(float fDeltaTime, bool bCallback);

	void SetEnable(bool bEnable);

	void SetBlendMask(FvUInt32 u32Binding, float* pfBlendMask);

	const float* GetBlendMask(FvUInt32 u32Binding) const;

	const FvUInt32 m_u32Index;
	FvAnimatedCharacterBase* m_pkParent;
	Ogre::AnimationState** m_ppkAnimationStates;
	bool m_bManualTime;
	AnimationStateType m_eType;
	float m_fTimePosition;
	float m_fTimeScale;
	Callback* m_pkCallBack;
	FvUInt32 m_u32KeyPointer;
	bool m_bContinuous;


};

class FvPartitionAnimationState : public FvAnimationState
{
public:
	enum BlendState
	{
		BLEND_NONE,
		BLEND_ANIMATION_ENABLE,
		BLEND_ANIMATION_DISABLE
	};
	struct PartitionData
	{
		static PartitionData& GetFromPartitionNode(FvDoubleLinkNode1<PartitionData*>* pkPartitionNode);
		static PartitionData& GetFromDisableNode(FvDoubleLinkNode1<PartitionData*>* pkDisableNode);

		FvPartitionAnimationState* m_pkParent;
		FvDoubleLinkNode1<PartitionData*> m_kPartitionNode;
		FvDoubleLinkNode1<PartitionData*> m_kDisableNode;
		float m_fBlendWeight;
	};

	FvPartitionAnimationState(FvAnimatedCharacterBase* pkParent, FvUInt32 u32Index);

	virtual ~FvPartitionAnimationState();

protected:
	void SetPriority(FvUInt32* pu32Priority);

	void UpdateFromBonePartition();

	void AfterPartitionMaskUpdate();

	void Play(bool bBlend);

	void Stop(bool bBlend);

	friend class FvAnimatedCharacterBase;
	friend class FvAnimatedCharacter;

	float* m_pfWeights;
	FvUInt32* m_pu32Priority;
	PartitionData* m_pkPartitionState;
	FvUInt32 m_u32PartitionMask;
	FvRefNode2<FvPartitionAnimationState*> m_kNode;
	bool m_bIsOnPartition;
};

class FvExtraAnimationState : public FvAnimationState
{
public:
	FvExtraAnimationState(FvAnimatedCharacterBase* pkParent, FvUInt32 u32Index);

protected:

	void UpdateNonePartition();

	void AddBlend(float fDeltaTime);

	bool IsPlay();

	void Play(bool bBlend);

	void Stop(bool bBlend);

	friend class FvAnimatedCharacterBase;
	friend class FvAnimatedCharacter;

	float m_fWeight;
	float m_fBlendTime;
	float m_fInvBlendTime;
	float m_fPassedBlendTime;
	FvRefNode2<FvExtraAnimationState*> m_kNode;


	static FvUInt32 ms_u32ExtraMask;
};

class FvAnimatedCharacterNode;

class FvAnimatedCharacterNodePrivateData
{
public:
	enum RenderState
	{
		NORMAL = 0,
		COLOR_EFFECT = 1,
		FADE_OUT = 2,
		BLEND = 3,
		MESH_UI = 4,
		EDGE_HIGHLIGHT = 5,
		EDGE_HIGHLIGHT_COLOR_EFFECT = 6,
		EDGE_HIGHLIGHT_BLEND = 7
	};

	class Loader : public FvBackgroundTask
	{
	public:
		Loader(FvAnimatedCharacterNodePrivateData* pkParent);

		virtual ~Loader();

		virtual void DoBackgroundTask(FvBGTaskManager &kMgr);

		virtual void DoMainThreadTask(FvBGTaskManager &kMgr);

		FvAnimatedCharacterNodePrivateData* m_pkParent;

		struct PartEntityBinding
		{
			FvString m_kName;
			Ogre::NameValuePairList m_kParams;
			FvComponentEntity* m_pkEntity;
		};

		typedef std::vector<PartEntityBinding> PartEntity;

		struct LinkEntityBinding
		{
			FvString m_kName;
			FvString m_kMeshName;
			FvString m_kMaterialName;
			Ogre::Entity* m_pkEntity;
		};

		typedef std::vector<LinkEntityBinding> LinkEntity;

		struct LinkEffectBinding
		{
			FvString m_kName;
			FvString m_kEffect;
		};

		typedef std::vector<LinkEffectBinding> LinkEffect;
	
		std::vector<PartEntity> m_kPartEntityDatas;
		std::vector<LinkEntity> m_kLinkEntityDatas;
		std::vector<LinkEffect> m_kLinkEffectDatas;

	};

	typedef FvSmartPointer<Loader> LoaderPtr;

	struct PartEntity
	{
		PartEntity();

		~PartEntity();

		FvUInt32 m_u32Priority;
		FvComponentEntity** m_ppkEntities;
		FvRefNode2<PartEntity*>* m_pkLocalParts;
		FvUInt32* m_pu32LocalParts;
		FvUInt32 m_u32LocalPartNumber;

		void Add(FvUInt32 u32Flag);

		void Del(FvUInt32 u32Flag);

		void Update(FvUInt32 u32BindingNumber);

	private:
		FvUInt32 m_u32DisplayFlags;
	};

	struct LinkEntity
	{
		LinkEntity();

		~LinkEntity();

		Ogre::Entity** m_ppkEntities;
		FvString m_kTagName;
	};

	struct LinkEffect
	{
		LinkEffect();

		~LinkEffect();

		ParticleUniverse::ParticleSystem** m_ppkEffects;
		FvString m_kTagName;
	};

	FvAnimatedCharacterNodePrivateData(FvAnimatedCharacterBase* pkParent, FvAnimatedCharacterNode* pkNode);

	~FvAnimatedCharacterNodePrivateData();

	void FinishLoad();

	void SetGroup(FvUInt32 u32Group);

	void SetCustomParams(FvUInt32 u32ID, const Ogre::Vector4& kParams);

	void SetCustomParams(FvUInt32 u32ID, FvUInt32 u32Pos, float fParam);

	void UpdatePickColor(const Ogre::Vector4& kID);

	FvAnimatedCharacterBase* m_pkParent;
	FvAnimatedCharacterNode* m_pkSelf;
	PartEntity* m_pkPartEntities;
	FvUInt32 m_u32NumPartEntity;
	LinkEntity* m_pkLinkEntities;
	FvUInt32 m_u32NumLinkEntity;
	LinkEffect* m_pkLinkEffects;
	FvUInt32 m_u32NumLinkEffect;

	LoaderPtr m_spLoader;

	RenderState m_eRenderState;
	bool m_bUpdateStateWithParent;

private:
	void InitPartEntity();

	void TermPartEntity();

	void InitLinkEntity();

	void TermLinkEntity();

	void InitLinkEffect();

	void TermLinkEffect();

	void InitLoader();

	void TermLoader();

};

class FV_MODEL_API FvAnimatedCharacterNode : private FvRefNode1<FvAnimatedCharacterNode*>
{
public:
	struct FV_MODEL_API PartEntityData
	{
		PartEntityData();
		
		const char* m_apcNames[FV_MAX_BINDING];
		const char* m_apcMeshes[FV_MAX_BINDING];
		const char* m_apcMaterials[FV_MAX_BINDING];
		FvUInt32 m_u32Priority;
		std::vector<FvUInt32> m_kParts;
	};

	struct FV_MODEL_API LinkEntityData
	{
		LinkEntityData();
		
		const char* m_apcNames[FV_MAX_BINDING];
		const char* m_apcMeshes[FV_MAX_BINDING];
		const char* m_apcMaterials[FV_MAX_BINDING];
		const char* m_pcNameOfLink;
	};

	struct LinkEffectData
	{
		LinkEffectData();
		
		const char* m_apcNames[FV_MAX_BINDING];
		const char* m_apcEffects[FV_MAX_BINDING];
		const char* m_pcNameOfLink;
	};

	typedef FvAnimatedCharacterNodePrivateData::RenderState RenderState;

	FvAnimatedCharacterNode();

	virtual ~FvAnimatedCharacterNode();

	virtual bool LoadFinishAll() { return true; }

	std::vector<PartEntityData> m_kPartEntityDatas;
	std::vector<LinkEntityData> m_kLinkEntityDatas;
	std::vector<LinkEffectData> m_kLinkEffectDatas;

	bool IsOnAvatar(FvAnimatedCharacterBase* pkCharacter);

	void PutOn(FvAnimatedCharacterBase* pkCharacter);

	void PutOff();

	FvAnimatedCharacterBase* GetParent();

	void RefreshLink();

	void SetRenderState(RenderState eState);

	RenderState GetRenderState();

	void SetUpdateStateWithParent(bool bEnable);

	bool GetUpdateStateWithParent();

	void SetAddColor(float r, float g, float b);

	void SetMulColor(float r, float g, float b);

	void SetBlendAlpha(float a, float x = 0.1f, float y = 0.1f, float fRefraction = 0.7f);

	void SetFadeOutAlpha(float a);

	void SetEdgeHighLight(float r, float g, float b, float a);

	void SetEdgeHighLightWidth(float fWidth);

	void UpdatePickColor(const Ogre::Vector4& kID);

private:
	friend class FvAnimatedCharacterBase;

	FvAnimatedCharacterNodePrivateData* m_pkData;

};

class FV_MODEL_API FvAnimatedCharacterBase : public FvReferenceCount
{
public:
	enum
	{
		NONE_ANIMATION = 0xFFFFFFFF
	};

	class Provider : public FvProvider2<Ogre::Vector3, Ogre::Quaternion>
	{
	public:
		const FvVector3& GetVector3()
		{
			return *((FvVector3*)m_pkPointer0);
		}

		FvQuaternion GetQuaternion()
		{
			return FvQuaternion(GetData1().x, GetData1().y,
				GetData1().z, GetData1().w);
		}
	};

	class Callback : public FvRefNode1<Callback*>
	{
	public:
		virtual void CallbackBeforeRender() = 0;
	};

	typedef FvSmartPointer<Provider> ProviderPtr;

	FvAnimatedCharacterBase(const FvAnimatedCharacterResourcePtr& rspResource, const char* pcName);
	virtual ~FvAnimatedCharacterBase();

	const char* GetName();

	void AttachToNode(Ogre::SceneNode* pkNode);

	void AttachCharacter(const char* pcTagPoint,
		FvVector3& kScale, FvAnimatedCharacterBase* pkCharacter);

	void DetachCharacter(FvAnimatedCharacterBase* pkCharacter);

	void Active(FvUInt32 u32Binding);

	FvUInt32 Active();

	FvUInt32 GetBindingNumber();

	FvUInt32 GetAnimation(const char* pcAnimationName);

	const char* GetAnimationName(FvUInt32 u32Animation);

	FvAnimationState* GetAnimationState(FvUInt32 u32Animation);

	void PutOn(FvAnimatedCharacterNode& kNode);

	void PutOff(FvAnimatedCharacterNode& kNode);

	void RefreshLink(FvAnimatedCharacterNode& kNode);

	void LinkCacheNodes();

	void SetRenderState(FvAnimatedCharacterNodePrivateData::RenderState eState);

	void SetAddColor(float r, float g, float b);

	void SetMulColor(float r, float g, float b);

	void SetBlendAlpha(float a, float x = 0.1f, float y = 0.1f, float fRefraction = 0.7f);

	void SetFadeOutAlpha(float a);

	void SetEdgeHighLight(float r, float g, float b, float a);

	void SetEdgeHighLightWidth(float fWidth);

	void VisitPick(Ogre::RenderQueue *pkQueue, const Ogre::Vector4& kColor);

	const ProviderPtr& ReferenceProvider(const char* pcName);

	void ReleaseProvider(const char* pcName);

	const ProviderPtr& ReferenceLocalProvider(const char* pcName);

	void ReleaseLocalProvider(const char* pcName);

	void AddCallback(Callback& kCallback);

	void UpdataTagPoints();

	void AddToRenderQueue(Ogre::RenderQueue* pkQueue);

	static void Tick();

	FvAnimatedCharacterResourcePtr GetAnimatedResource() { return m_spResource; }

protected:

	void LinkCacheNodesImpl();

	FvUInt32 GetAnimationReal(const char* pcAnimationName);

	void EnablePartitionAnimaiton(FvUInt32 u32Animation, FvUInt32* pu32Priority, bool bLoop = false, bool bManual = false, bool bContinuous = false);

	void EnableExtraAnimaiton(FvUInt32 u32Animation, bool bLoop = false, bool bManual = false, bool bContinuous = false);

	void DisableAnimation(FvUInt32 u32Animation);

	void PlayAnimation(FvUInt32 u32Animation, bool bIsBlend = true);

	void StopAnimation(FvUInt32 u32Animation, bool bIsBlend = true);

	float GetBlendTime(FvUInt32 u32From, FvUInt32 u32To);

	void AddTime(float fDeltaTime);

	void EnableAnimation(FvUInt32 u32Animation, FvAnimationState::AnimationStateType eType);

	void AddBlendTime(float fDeltaTime);
	void AddPositionTime(float fDeltaTime);

	void Cache(FvAnimatedCharacterNodePrivateData& kData);

	void Link(FvAnimatedCharacterNodePrivateData& kData);

	void Cut(FvAnimatedCharacterNodePrivateData& kData);

	friend class FvAnimatedEntity;
	friend class FvAnimationState;
	friend class FvPartitionAnimationState;
	friend class FvExtraAnimationState;
	friend class FvAnimatedCharacterNodePrivateData;

	static float ms_fDefaultBlendTime;

	const FvString m_kName;
	FvAnimatedCharacterResourcePtr m_spResource;
	const FvUInt32 m_u32BindingNumber;
	FvAnimatedEntity** m_ppkAnimatedEntities;
	FvUInt8** m_ppu8BoneSwitchTables;
	FvUInt32 m_u32ActiveBinding;
	const FvUInt32 m_u32PartitionNumber;	

	struct PartitionStack
	{
		FvDoubleLink<FvPartitionAnimationState::PartitionData*, FvDoubleLinkNode1> m_kPartitionAnimations;
		FvDoubleLink<FvPartitionAnimationState::PartitionData*, FvDoubleLinkNode1> m_kDisableAnimations;

		float m_fBlendTime;
		float m_fInvBlendTime;
		float m_fPassedBlendTime;
	};

	PartitionStack* m_pkPartitionAnimations;
	FvRefList<FvPartitionAnimationState*> m_kActivePartitionAnimations;

	FvRefList<FvExtraAnimationState*> m_kExtraAnimationEnableList;
	FvRefList<FvExtraAnimationState*> m_kExtraAnimationDisableList;

	FvAnimationState** m_ppkAnimationStates;

	FvRefList<FvAnimatedCharacterNodePrivateData::PartEntity*>* m_pkGlobalParts;

	FvRefListNameSpace<FvAnimatedCharacterNode>::List1 m_kNodesWaitLoad;
	FvRefListNameSpace<FvAnimatedCharacterNode>::List1 m_kNodesCache;
	FvRefListNameSpace<FvAnimatedCharacterNode>::List1 m_kNodesOnCharacter;

	FvRefNode2<FvAnimatedCharacterBase*> m_kNodeForCache;

	Ogre::TagPoint** m_ppkParentTagPoints;

	struct BindingProvider
	{
		BindingProvider();
		~BindingProvider();

		void UpdateBinding(FvUInt32 u32Binding);
		void LocalUpdateBinding(FvUInt32 u32Binding);
		void ForcedUpdate(FvUInt32 u32Binding);

		Ogre::TagPoint* m_apkNodes[FV_MAX_BINDING];
		ProviderPtr m_spProvider;
	};

	typedef FvStringMap<BindingProvider*> BindingProviderMap;
	BindingProviderMap m_kProviderMap;
	BindingProviderMap m_kLocalProviderMap;

	FvRefList<Callback*, FvRefNode1> m_kCallbackList;

	static FvRefList<FvAnimatedCharacterBase*> m_kCacheList;
};

class FV_MODEL_API FvAnimatedCharacter : public FvAnimatedCharacterBase
{
public:
	enum PlayType
	{
		DEFAULT,
		INSERT,
		BREAK,
		BREAK_SOFT,
		REMOVE
	};

	FvAnimatedCharacter(const FvAnimatedCharacterResourcePtr& rspResource, const char* pcName,
		const FvAnimationSetupResourcePtr& rspAniSetup);
	virtual ~FvAnimatedCharacter();

	struct AnimatioPlay
	{
		AnimatioPlay(float fStart, bool bBlend) : m_fStart(fStart), m_bBlend(bBlend) {}
		float m_fStart;
		bool m_bBlend;
	};

	struct Level : public FvDoubleLinkNode1<Level>
	{
		Level();
		FvUInt32 m_u32CurrentAnimation;
		std::list<std::pair<FvUInt32, AnimatioPlay>> m_kInsertQueue;
		bool m_bNeedRemove;
	};

	struct Extra : public FvDoubleLinkNode1<Extra>
	{
		Extra(FvUInt32 u32Index) : m_u32Index(u32Index), m_bNeedRemove(false) {}
		const FvUInt32 m_u32Index;
		std::list<AnimatioPlay> m_kInsertQueue;
		bool m_bNeedRemove;
	};

	static void Init();

	static void Term();

	void AddTime(float fDeltaTime);

	void PlayAnimation(const char* pcName, PlayType eType = INSERT, float fStart = 0, bool bBlend = true);

	void PlayAnimation(FvUInt32 u32Animation, PlayType eType = INSERT, float fStart = 0, bool bBlend = true);

	void RemoveLevel(const char* pcName);

	void SetScale(const char* pcGroup, float fScale);

	FvAnimationSetupResourcePtr GetAnimatedSetup() { return m_spAnimationSetup; }

protected:
	FvAnimationSetupResourcePtr m_spAnimationSetup;
	FvAnimationSetupResource::Animation** m_pkAnimationSetups;
	Level* m_pkLevels;
	FvDoubleLink<Level, FvDoubleLinkNode1> m_kActiveLevels;

	std::map<FvUInt32, Extra*> m_kExtraAnimation;
	FvDoubleLink<Extra, FvDoubleLinkNode1> m_kActiveExtraAnimations;
	
};

typedef FvSmartPointer<FvAnimatedCharacter> FvAnimatedCharacterPtr;

#endif // __FvAnimatedCharacter_H__
