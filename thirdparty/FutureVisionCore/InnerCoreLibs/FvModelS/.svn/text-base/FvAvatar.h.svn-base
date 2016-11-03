//{future header message}
#ifndef __FvAvatarSkeleton_H__
#define __FvAvatarSkeleton_H__

#include <FvBaseTypes.h>
#include <FvSmartPointer.h>
#include <FvStringMap.h>
#include <FvDebug.h>
#include <list>
#include <map>

#include "FvModelDefines.h"
#include "FvAvatarResource.h"

#include <FvRefList.h>
#include <FvProvider.h>
#include <FvVector3.h>
#include <FvQuaternion.h>
#include <OgreVector4.h>
//
//namespace Ogre
//{
//	class SceneNode;
//	class SceneManager;
//	class Entity;
//	class Skeleton;
//	class AnimationState;
//	class AnimationStateSet;
//	class TagPoint;
//	class RenderQueue;
//	struct VisibleObjectsBoundsInfo;
//};
//
//using namespace Ogre;
//class FvAvatar;
//struct FvAvatarLinkNode;
//
//class FV_MODEL_API FvAvatarSkeleton
//{
//public:
//	FvAvatarSkeleton(FvAvatarResource::SkeletonInfo* pkSkeletonInfo,
//		const char* pcName, bool bShow = false);
//	~FvAvatarSkeleton();
//
//	void AddEntity(Entity* pkEntity);
//
//	void RemoveEntity(Entity* pkEntity);
//
//	void SyncEntity(Entity* pkEntity);
//
//	void AttachNode(SceneNode* pkNode);
//
//	void DetachNode(SceneNode* pkNode);
//
//	SceneNode* GetSceneNode();
//
//	AnimationState* GetAnimationState(const char* pcAnimationName);
//
//	AnimationStateSet* GetAllAnimationStates();
//
//	void CreateTagPoint(TagPoint** ppkOut, const char* pcBoneName);
//
//	void FreeTagPoint(TagPoint* pkTag);
//
//	float* GetPartition(FvUInt32 u32Partition);
//
//	FvUInt32 GetBoneNumber();
//
//protected:
//	friend class FvAvatar;
//	FvAvatarResource::SkeletonInfo* m_pkSkeletonInfo;
//	SceneNode* m_pkNode;
//	Entity* m_pkMainEntity;
//
//	typedef std::map<Ogre::SceneNode*,FvAvatarLinkNode*> LinkMap;
//	LinkMap m_kLinkNodes;
//};
//
//class FV_MODEL_API FvAvatarSkeletonAnimationState
//{
//public:
//	enum State
//	{
//		NONE_PARTITION,
//		SINGLE_PARTITION,
//		MORPH_PARTITION
//	};
//
//	FvAvatarSkeletonAnimationState(FvAvatarSkeleton* pkParent,
//		const char* pcName);
//	~FvAvatarSkeletonAnimationState();
//
//	void ChangePartition(FvInt32 i32Partition);
//
//	void MorphPartition(float t, FvInt32 x, FvInt32 y);
//
//	void NonePartition();
//
//	State GetState();
//
//	FvInt32 GetActivePartition();
//
//	const char* GetName();
//
//	void SetLoop(bool bLoop);
//
//	void SetEnabled(bool bEnabled);
//
//	void SetWeight(float fWeight);
//
//	void SetTimePosition(float fTimePos);
//
//	bool IsLoop();
//
//	bool IsEnabled();
//
//	bool IsEnd();
//
//	float GetWeight();
//
//	float GetTimePosition();
//
//	float GetLength();
//
//	void AddTime(float fOffset);
//
//protected:
//	AnimationState* m_pkAnimationState;
//	State m_eState;
//	FvInt32 m_i32ActivePartition;
//	FvAvatarSkeleton* m_pkParent;
//	void* m_pvBlendMask;
//};
//
//class FV_MODEL_API FvAvatarAnimationState
//{
//public:
//	struct KeyEvent;
//
//	typedef std::list<KeyEvent> KeyEventList;
//	typedef FvStringMap<KeyEvent> KeyEventMap;
//
//	struct KeyEvent
//	{
//		FvString m_strName;
//		float m_fTimePos;
//		bool m_bNeedToRemove;
//	};
//
//	enum EventType
//	{
//		ANI_START			= 0x1,
//		ANI_END				= 0x2,
//		ANI_ENABLE			= 0x4,
//		ANI_DISABLE			= 0x8,
//		ANI_KEY_EVENT		= 0x10
//	};
//
//	class AnimationStateCallback
//	{
//	public:
//		virtual ~AnimationStateCallback() {};
//
//		virtual void EventCome(FvAvatarAnimationState* pkAnimationState,
//			EventType eType, const char* pcMessage) = 0;
//	};
//
//	enum BlendState
//	{
//		BLEND_NONE,
//		BLEND_ANIMATION_ENABLE,
//		BLEND_ANIMATION_DISABLE
//	};
//
//	FvAvatarAnimationState(FvAvatarSkeleton** ppkSkeletons, FvUInt32 u32Number,
//		const char* pcName);
//	~FvAvatarAnimationState();
//
//	void Active(FvUInt32 u32Binding);
//
//	void ChangePartition(FvInt32 i32Partition, float fBlendTime);
//
//	void SetLoop(bool bLoop);
//
//	void SetManualTime(bool bManual);
//
//	bool SetEnabled(FvInt32 i32Enable, float fBlendTime);
//
//	void SetTimePosition(float fTimePos);
//
//	void SetTimeScale(float fTimeScale);
//
//	void SetWeight(float fWeight);
//
//	bool IsLoop();
//
//	bool IsManualTime();
//
//	FvInt32 GetEnabled();
//
//	float GetTimePosition();
//
//	float GetTimeScale();
//
//	float GetWeight();
//
//	float GetLength();
//
//	FvInt32 GetActivePartition();
//
//	const char* GetName();
//
//	void AddTime(float fDeltaTime, bool bIsCache = false);
//
//	bool AddTimeDisable(float fDeltaTime);
//
//	void SetCallback(AnimationStateCallback* pkCallback);
//
//	void RegisterCallback(EventType eType,
//		const char* pcName, float fTimePos);
//
//	void UnregisterCallback(EventType eType, const char* pcName);
//
//private:
//	void Update();
//
//	void AddTimeCache(float fDeltaTime);
//
//	void AddTimeBlend(float fDeltaTime);
//
//	void AddTimePosition(float fDeltaTime);
//
//	void ProcessCallbacks(float fLastTimePos, float fTimePos);
//
//	void DisabledAll();
//
//	FvAvatarSkeletonAnimationState** m_ppkStates;
//	FvUInt32 m_u32BindingNumber;
//	FvUInt32 m_u32ActiveBinding;
//	FvAvatarSkeletonAnimationState::State m_eState;
//	FvInt32 m_i32ActivePartition;
//	float m_fTimePosition;
//	float m_fTimeScale;
//	float m_fLength;
//	float m_fWeight;
//	float m_fTimeCache;
//	float m_fTimeCachePosition;
//
//	KeyEventList m_kKeyEventList;
//	KeyEventList::iterator m_itNext;
//	FvUInt32 m_u32EventMask;
//	AnimationStateCallback* m_pkCallback;
//
//	FvInt32 m_i32Enable;
//	BlendState m_eBlendState;
//	float m_fBlendTime;
//	float m_fInvBlendTime;
//	float m_fPassBlendTime;
//	FvInt32 m_i32TargetPartition;
//	float m_fBlendPartitionTime;
//	float m_fInvBlendPartitionTime;
//	float m_fPassBlendPartitionTime;
//
//	bool m_bEnableMorphPartition;
//	bool m_bDirtyEventList;
//	bool m_bManualTime;
//	bool m_bCacheTime;
//};
//
//class FV_MODEL_API FvAvatar : public FvReferenceCount
//{
//public:
//	class AnimationStateCallback;
//
//	struct AnimationState
//	{
//		FvAvatarAnimationState* m_pkState;
//		FvInt32 m_i32Index;
//		AnimationStateCallback* m_pkCallback;
//	};
//
//	class AvatarCallback
//	{
//	public:
//		virtual void EventCome(FvAvatar* pkAvatar,
//			FvInt32 i32Animation,
//			FvAvatarAnimationState::EventType eType,
//			const char* pcMessage) = 0;
//	};
//
//	class AnimationStateCallback :
//		public FvAvatarAnimationState::AnimationStateCallback
//	{
//	public:
//		AnimationStateCallback(FvAvatar* pkAvatar);
//
//		~AnimationStateCallback();
//
//		virtual void EventCome(FvAvatarAnimationState* pkAnimationState,
//			FvAvatarAnimationState::EventType eType, const char* pcMessage);
//
//		void SetCallback(AvatarCallback* pkCallback);
//
//	private:
//		FvAvatar* m_pkParent;
//		AvatarCallback* m_pkCallback;
//	};
//
//	class AvatarNodePrivateData;
//
//	class FV_MODEL_API AvatarNode : private FvRefNode2<AvatarNode*>
//	{
//	public:
//		AvatarNode();
//		virtual ~AvatarNode();
//
//		struct PartEntityData
//		{
//			PartEntityData()
//			{
//				FvZeroMemory(m_apcNames,sizeof(m_apcNames));
//				FvZeroMemory(m_apcMeshes,sizeof(m_apcMeshes));
//				FvZeroMemory(m_apcMaterials,sizeof(m_apcMaterials));
//			}
//			const char* m_apcNames[FV_AVATAR_MAX_BINDING_NUMBER];
//			const char* m_apcMeshes[FV_AVATAR_MAX_BINDING_NUMBER];
//			const char* m_apcMaterials[FV_AVATAR_MAX_BINDING_NUMBER];
//			FvUInt32 m_u32Priority;
//			std::vector<FvUInt32> m_kParts;
//		};
//
//		struct LinkEntityData
//		{
//			LinkEntityData()
//			{
//				FvZeroMemory(m_apcNames,sizeof(m_apcNames));
//				FvZeroMemory(m_apcMeshes,sizeof(m_apcMeshes));
//				FvZeroMemory(m_apcMaterials,sizeof(m_apcMaterials));
//				m_pcNameOfLink = NULL;
//			}
//			const char* m_apcNames[FV_AVATAR_MAX_BINDING_NUMBER];
//			const char* m_apcMeshes[FV_AVATAR_MAX_BINDING_NUMBER];
//			const char* m_apcMaterials[FV_AVATAR_MAX_BINDING_NUMBER];
//			const char* m_pcNameOfLink;
//		};
//
//		struct LinkEffectData
//		{
//			LinkEffectData()
//			{
//				FvZeroMemory(m_apcNames,sizeof(m_apcNames));
//				FvZeroMemory(m_apcEffects,sizeof(m_apcEffects));
//				m_pcNameOfLink = NULL;
//			}
//			const char* m_apcNames[FV_AVATAR_MAX_BINDING_NUMBER];
//			const char* m_apcEffects[FV_AVATAR_MAX_BINDING_NUMBER];
//			Ogre::Quaternion m_kQuaternion;
//			Ogre::Vector3 m_kPosition;
//			const char* m_pcNameOfLink;
//		};
//
//		enum RenderState
//		{
//			NORMAL = 0,
//			COLOR_EFFECT = 1,
//			FADE_OUT = 2,
//			BLEND = 3,
//			MESH_UI = 4,
//			EDGE_HIGHLIGHT = 5,
//			EDGE_HIGHLIGHT_COLOR_EFFECT = 6,
//			EDGE_HIGHLIGHT_BLEND = 7
//		};
//
//		virtual void LoadFinishBinding(FvUInt32 u32Entity, FvUInt32 u32Binding, bool bIsPart) {}
//
//		virtual void LoadFinishEntity(FvUInt32 u32Entity, bool bIsPart) {}
//
//		virtual bool LoadFinishAll() { return true; }
//
//		bool IsOnAvatar(FvAvatar* pkAvatar);
//
//		void RefreshMaterials();
//
//		void RefreshLink();
//
//		void PutOn(FvAvatar* pkParent);
//
//		void PutOff();
//
//		FvAvatar* GetParent();
//
//		void SetRenderState(RenderState eState);
//
//		RenderState GetRenderState();
//
//		void SetAddColor(float r, float g, float b);
//
//		void SetMulColor(float r, float g, float b);
//
//		void SetBlendAlpha(float a, float x = 0.1f, float y = 0.1f, float fRefraction = 0.7f);
//
//		void SetFadeOutAlpha(float a);
//
//		void SetEdgeHighLight(float r, float g, float b, float a);
//
//		void SetEdgeHighLightWidth(float fWidth);
//
//		void UpdateRenderQueue(Ogre::RenderQueue* pkQueue, const Ogre::Vector4& kID);
//
//		void SetUpdateStateWithParent(bool bEnable);
//
//		bool GetUpdateStateWithParent();
//
//		std::vector<PartEntityData> m_kPartEntityDatas;
//		std::vector<LinkEntityData> m_kLinkEntityDatas;
//		std::vector<LinkEffectData> m_kLinkEffectDatas;
//
//	private:
//		friend class FvAvatar; 
//		AvatarNodePrivateData* m_pkData;
//		bool m_bUpdateStateWithParent;
//	};
//
//	class Provider : public FvProvider2<Ogre::Vector3, Ogre::Quaternion>
//	{
//	public:
//		const FvVector3& GetVector3()
//		{
//			return *((FvVector3*)m_pkPointer0);
//		}
//
//		FvQuaternion GetQuaternion()
//		{
//			return FvQuaternion(GetData1().x, GetData1().y,
//				GetData1().z, GetData1().w);
//		}
//	};
//
//	typedef FvSmartPointer<Provider> ProviderPtr;
//
//	class FV_MODEL_API FastProvider : public FvReferenceCount
//	{
//	public:
//		FvVector3 GetVector3();
//		FvQuaternion GetQuaternion();
//
//	protected:
//		FastProvider(FvAvatar* pkAvatar);
//		Ogre::Bone* m_apkBones[FV_AVATAR_MAX_BINDING_NUMBER];
//		bool m_bLocal;
//		FvAvatar* m_pkAvatar;
//
//		friend class FvAvatar;
//	};
//
//	typedef FvSmartPointer<FastProvider> FastProviderPtr;
//
//	enum
//	{
//		DO_NOT_USE		= -1,
//		POS_0			= 0,
//		POS_1			= 1,
//		POS_2			= 2,
//		POS_3			= 3,
//		POS_4			= 4,
//		POS_5			= 5,
//		POS_6			= 6,
//		POS_7			= 7,
//		POS_8			= 8,
//		POS_9			= 9,
//		POS_10			= 10,
//		POS_11			= 11,
//		POS_12			= 12,
//		POS_13			= 13,
//		POS_14			= 14,
//		POS_15			= 15
//	};
//
//	typedef std::list<AnimationState*> AnimationStateList;
//
//	static void SetGlobalDefaultBlendTime(float fBlendTime);
//	static float GetGlobalDefaultBlendTime();
//	static void Init();
//	static void Term();
//
//	FvAvatar(const FvAvatarResourcePtr& rspAvatarResource, const char* pcName);
//	virtual ~FvAvatar();
//
//	const char* GetName();
//
//	SceneNode* GetSceneNode();
//
//	void SetAnimationUsedNumber(FvUInt32 u32Number);
//
//	void SetPartition(FvUInt32 u32Pos, FvInt32 i32Partition);
//
//	void SetAnimation(FvUInt32 u32Pos, FvInt32 i32Animation,
//		bool bEnableBlend = true);
//
//	void SetAnimations(FvUInt32 u32Number, ...);
//
//	void _AddAnimation(FvInt32 i32Animation, FvInt32 i32Partition);
//
//	void _DelAnimation(FvInt32 i32Animation);
//
//	void _ChangeAnimation(FvInt32 i32Old, FvInt32 i32New);
//
//	void _ChangePartition(FvInt32 i32Animation, FvInt32 i32Partition);
//
//	void AddTime(float fDeltaTime);
//
//	FvInt32 GetPartition(const char* pcPartitionName);
//
//	FvInt32 GetAnimation(const char* pcAnimationName);
//
//	FvInt32 GetPartition(FvUInt32 u32Pos);
//
//	FvInt32 GetAnimation(FvUInt32 u32Pos);
//
//	FvInt32 GetEnable(FvInt32 i32Animation);
//
//	const char* GetAnimationName(FvInt32 i32Animation);
//
//	void SetCacheTime(bool bIsCache);
//
//	bool IsCacheTime();
//
//	void Active(FvUInt32 u32Binding);
//
//	FvUInt32 GetBindingNumber();
//
//	FvUInt32 GetActiveBinding();
//
//	FvUInt32 GetAnimationNumber();
//
//	void SetWeight(FvUInt32 u32Animation, float fWeight);
//
//	void SetTimeScale(FvUInt32 u32Animation, float fTimeScale);
//
//	void SetLoop(FvUInt32 u32Animation, bool bLoop);
//
//	void SetTimePosition(FvUInt32 u32Pos, float fTimePos);
//
//	float GetTimePosition(FvUInt32 u32Pos);
//
//	void SetManualTime(FvUInt32 u32Animation, bool bIsManualTime);
//
//	float GetLength(FvUInt32 u32Animation);
//
//	bool IsManualTime(FvUInt32 u32Pos);
//
//	bool IsLoop(FvUInt32 u32Animation);
//
//	void SetDefaultBlendTime(float fTime);
//
//	float GetDefaultBlendTime();
//
//	void EnableCallback(FvUInt32 u32Animation);
//
//	void DisableCallback(FvUInt32 u32Animation);
//
//	void RegisterCallback(FvUInt32 u32Animation,
//		FvAvatarAnimationState::EventType eType, const char* pcName);
//
//	void UnregisterCallback(FvUInt32 u32Animation,
//		FvAvatarAnimationState::EventType eType, const char* pcName);
//
//	void SetCallback(FvUInt32 u32Animation,
//		AvatarCallback* pkCallback);
//
//	void SetCallback(AvatarCallback* pkCallback);
//
//	void PutOn(AvatarNode* pkNode);
//
//	void PutOff(AvatarNode* pkNode);
//
//	void PutOnCached();
//
//	const ProviderPtr& ReferenceProvider(const char* pcName);
//
//	void ReleaseProvider(const char* pcName);
//
//	FastProviderPtr GetFastProvider(const char* pcName, bool bLocal = false);
//
//	void SetRenderState(AvatarNode::RenderState eState);
//
//	void SetAddColor(float r, float g, float b);
//
//	void SetMulColor(float r, float g, float b);
//
//	void SetBlendAlpha(float a, float x = 0.1f, float y = 0.1f, float fRefraction = 0.7f);
//
//	void SetFadeOutAlpha(float a);
//
//	void SetEdgeHighLight(float r, float g, float b, float a);
//
//	void SetEdgeHighLightWidth(float fWidth);
//
//	void FindVisibleLinkObjects(Ogre::Camera *pkCamera, Ogre::RenderQueue *pkQueue, 
//		Ogre::VisibleObjectsBoundsInfo *pkVisibleBounds, 
//		bool bIncludeChildren = true, bool bDisplayNodes = false, bool bOnlyShadowCasters = false);
//
//	void VisitPick(Ogre::RenderQueue *pkQueue, const Ogre::Vector4& kColor);
//
//	const FvAvatarResourcePtr& GetAvatarResource();
//
//	void GetBoundingBox(FvBoundingBox &kBB);
//
//	void GetBoundingBox(FvBoundingBox &kBB, const FvVector3& kScale);
//
//	bool HasBoundingBox();
//
//	void SetScale(const FvVector3& kScale);
//
//protected:
//
//	void DisableAnimation(AnimationState* pkState, float fBlendTime);
//	
//	float GetPartitionBlendTime(FvInt32 i32Partition0, FvInt32 i32Partition1);
//
//	float GetAnimationBlendTime(FvInt32 i32Animation0, FvInt32 i32Animation1);
//
//	void SetPartitionReal(FvUInt32 u32Pos);
//
//	void Link(AvatarNodePrivateData* pkData);
//
//	void Cut(AvatarNodePrivateData* pkData);
//
//	void UpdateBoundingBox();
//
//	friend class AnimationStateCallback;
//	static float ms_fDefaultBlendTime;
//
//	const FvString m_kName;
//	FvAvatarResourcePtr m_spAvatarResource;
//	FvAvatarSkeleton** m_ppkAvatarSkeletons;
//	FvUInt32 m_u32BindingNumber;
//	FvUInt32 m_u32ActiveBinding;
//	SceneNode* m_pkAvatarRootNode;
//	AnimationState* m_pkAnimationStates;
//	FvUInt32 m_u32ActiveAnimationNumber;
//
//	struct
//	{
//		FvInt32 m_i32Partition;
//		AnimationState* m_pkAnimationState;
//		bool m_bIsDirtyPartition;
//	} m_akAnimationSet[FV_AVATAR_ANI_SET_SIZE];
//	FvUInt32 m_u32AnimationUsed;
//
//	AnimationStateList m_kDisabledBlendingAnimationStates;
//	float m_fCacheDeltaTime;
//
//	class PVList : public FvRefList<void*>
//	{
//	public:
//		PVList() {}
//
//		PVList(const PVList& kNode) {}
//
//		void operator = (const PVList& kNode) {}
//
//	};
//
//	std::vector<PVList> m_kAvatarGlobalParts;
//
//	FvRefList<AvatarNode*> m_kAvatarNodeList;
//	FvRefList<AvatarNode*> m_kAvatarNodeCacheList;
//
//	struct BindingProvider
//	{
//		BindingProvider();
//		~BindingProvider();
//
//		void UpdateBinding(FvUInt32 u32Binding, SceneNode* pkParent, bool bClear);
//
//		Ogre::Bone* m_apkBones[FV_AVATAR_MAX_BINDING_NUMBER];
//		Ogre::Vector3 m_kPosition;
//		Ogre::Quaternion m_kRotation;
//		ProviderPtr m_spProvider;
//	};
//
//	typedef FvStringMap<BindingProvider*> BindingProviderMap;
//
//	BindingProviderMap m_kProviderMap;
//	ProviderPtr m_spRootProvider;
//
//	typedef std::vector<FastProviderPtr> FastProviders;
//
//	FastProviders m_kFastProviders;
//
//	float m_fDefaultBlendTime;
//	AvatarCallback* m_pkCallback;
//	bool m_bCacheTime;
//
//	struct BoundingBoxParams
//	{
//		ProviderPtr m_spPoint;
//		FvBoundingBox m_kBoundingBox;
//	};
//
//	BoundingBoxParams* m_pkBoundingBoxParams;
//
//	FvVector3 m_kScale;
//
//};
//
//typedef FvSmartPointer<FvAvatar> FvAvatarPtr;

#endif // __FvAvatarSkeleton_H__
