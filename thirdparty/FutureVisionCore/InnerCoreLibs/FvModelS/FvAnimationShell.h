//{future header message}
#ifndef __FvAnimationShell_H__
#define __FvAnimationShell_H__

#include "FvAvatar.h"
#include "FvAnimationShellResource.h"
//
//class FV_MODEL_API FvAnimationShell : public FvReferenceCount, protected FvAvatar::AvatarCallback
//{
//public:
//	enum PlayType
//	{
//		DEFAULT = 0,
//		REMOVE = 1,
//		INSERT = 2,
//		INSERT_BREAK = 3,
//		INSERT_BREAK_SOFT = 4
//	};
//
//	class FV_MODEL_API AnimationHandle : public FvRefNode2<AnimationHandle*>
//	{
//	public:
//		enum CallBackType
//		{
//			HUNG_UP,
//			RESUME,
//			INSERT_START,
//			INSERT_END,
//			INSERT_BREAKED
//		};
//
//		typedef void (*CallBack)(AnimationHandle*,CallBackType);
//
//		AnimationHandle();
//		
//		const FvString& GetName();
//
//		void SetLoop(bool bLoop);
//
//		float GetLength();
//
//		void SetTimePosition(float fTimePos);
//
//		void SetTimePositionDirection(float fTimePos);
//
//		void GotoTheEnd();
//
//		FvUInt32* GetPriorities();
//
//		FvUInt32 GetLevel();
//
//		void SetCallback(CallBack pkCallback);
//
//	protected:
//		~AnimationHandle() {}
//
//		friend class FvAnimationShell;
//		FvString m_kAnimationName;
//		FvAnimationShell* m_pkParent;
//		FvInt32 m_i32Animation;
//		FvAnimationShellResource::Animation m_kResAnimation;
//		bool m_bIsLevelAnimation;
//		float m_fTimePosition;
//		CallBack m_pkCallBack;
//		FvUInt32 m_u32PlayNumber;
//		FvInt32 m_i32Binding;
//	};
//
//	FvAnimationShell(const FvAnimationShellResourcePtr& spResource, const FvAvatarPtr& spAvatar);
//	virtual ~FvAnimationShell();
//
//	//bool UseAnimationHandle(const FvString& kName, AnimationHandle* pkHandle);
//
//	AnimationHandle* GetAnimationHandleByName(const FvString& kName);
//
//	void PlayAnimation(AnimationHandle* pkHandle, PlayType eType = DEFAULT);
//
//	void AddTime(float fDeltaTime);
//
//	FvUInt32 GetMaskedPartitionNumber();
//
//	void SetScale(const FvString& kGroup, float fScale);
//
//	void StopLevel(const FvString& kLevel);
//
//protected:
//	virtual void EventCome(FvAvatar* pkAvatar,
//		FvInt32 i32Animation,
//		FvAvatarAnimationState::EventType eType,
//		const char* pcMessage);
//
//	FvAnimationShellResourcePtr m_spResource;
//	FvAvatarPtr m_spTargetAvatar;
//
//	void Init();
//
//	void RemoveAnimation(FvUInt32 u32Pos);
//
//	struct Level : public FvAvatar::AvatarCallback
//	{
//		Level();
//		~Level();
//
//		virtual void EventCome(FvAvatar* pkAvatar,
//			FvInt32 i32Animation,
//			FvAvatarAnimationState::EventType eType,
//			const char* pcMessage);
//
//		void Init(FvUInt32 u32PartNumber);
//
//		void AddTime(float fDeltaTime, FvAnimationShell* pkParent);
//
//		void Play(FvAnimationShell* pkParent, FvUInt32 i);
//
//		FvUInt32* GetPriorities();
//
//		void PartitionPre(FvAnimationShell* pkParent);
//
//		void PartitionPost(FvAnimationShell* pkParent);
//
//		void PartitionRemove(FvAnimationShell* pkParent);
//
//		void AnimationRemove(FvAnimationShell* pkParent);
//
//		void ClearLevelPartition(FvAnimationShell* pkParent, PlayType eType);
//
//		enum
//		{
//			STATE_OFF,
//			STATE_DEFAULT,
//			STATE_INSERT
//		} m_eState;
//		AnimationHandle* m_pkCurrentAnimation;
//		AnimationHandle* m_pkCurrentLevelAnimation;
//		AnimationHandle* m_pkNextLevelAnimation;
//		float m_fInsertPassTime;
//		std::list<AnimationHandle*> m_kInsertQueue;
//		AnimationHandle* m_pkBreakAnimation;
//		FvUInt32 m_u32AnimationPosition;
//		FvUInt32 m_u32CurrentPartition;
//		FvRefNode2<Level*>* m_pkLevelNodes;
//		bool m_bNeedPlay;
//		bool m_bDirtyPartition;
//		//bool m_bNeedClear;
//		bool m_bNeedRemoveInsert;
//	};
//
//	Level* m_pkLevels;
//	FvRefList<Level*>* m_pkLists;
//
//	FvRefList<AnimationHandle*> m_kExtraAnimationEnable;
//	FvRefList<AnimationHandle*> m_kExtraAnimationDisable;
//
//	FvUInt32 m_au32AnimationSet[FV_AVATAR_ANI_SET_SIZE];
//	FvUInt32 m_u32AnimationUse;
//
//	FvStringMap<AnimationHandle*> m_kHandles;
//};
//
//typedef FvSmartPointer<FvAnimationShell> FvAnimationShellPtr;

#endif // __FvAnimationShell_H__
