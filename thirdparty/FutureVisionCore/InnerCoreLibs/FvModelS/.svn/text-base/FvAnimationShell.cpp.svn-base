//#include "FvAnimationShell.h"
//
////----------------------------------------------------------------------------
//FvAnimationShell::FvAnimationShell(
//	const FvAnimationShellResourcePtr& spResource,
//	const FvAvatarPtr& spAvatar)
//{
//	FV_ASSERT(!spResource.isNull());
//	FV_ASSERT(spAvatar);
//	m_spResource = spResource;
//	m_spTargetAvatar = spAvatar;
//
//	Init();
//}
////----------------------------------------------------------------------------
//FvAnimationShell::~FvAnimationShell()
//{
//	FV_SAFE_DELETE_ARRAY(m_pkLevels);
//	FV_SAFE_DELETE_ARRAY(m_pkLists);
//
//	for(FvStringMap<AnimationHandle*>::iterator it = m_kHandles.begin();
//		it != m_kHandles.end(); ++it)
//	{
//		delete it->second;
//	}
//
//	m_kHandles.clear();
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::Init()
//{
//	m_pkLevels = new Level[m_spResource->m_u32LevelNumber];
//	for(FvUInt32 i(0); i < m_spResource->m_u32LevelNumber; ++i)
//	{
//		m_pkLevels[i].Init(m_spResource->m_u32MaskedPartitionNumber);
//	}
//
//	m_pkLists = new FvRefList<Level*>[m_spResource->m_u32MaskedPartitionNumber];
//
//	m_u32AnimationUse = 0;
//
//	for(FvUInt32 i(0); i < m_spResource->m_u32LoopAnimationNumber; ++i)
//	{
//		FvInt32 i32Animation = m_spTargetAvatar->GetAnimation(m_spResource->m_pkLoopAnimations[i].c_str());
//
//		if(i32Animation >= 0)
//			m_spTargetAvatar->SetLoop(i32Animation, true);
//	}
//
//	for(FvUInt32 i(0); i < m_spResource->m_u32ManualAnimationNumber; ++i)
//	{
//		FvInt32 i32Animation = m_spTargetAvatar->GetAnimation(m_spResource->m_pkManualAnimations[i].c_str());
//		
//		if(i32Animation >= 0)
//			m_spTargetAvatar->SetManualTime(i32Animation, true);
//	}
//}
////----------------------------------------------------------------------------
////bool FvAnimationShell::UseAnimationHandle(const FvString& kName,
////	AnimationHandle* pkHandle)
////{
////	pkHandle->m_kAnimationName = kName;
////	pkHandle->m_pkParent = this;
////	pkHandle->m_i32Animation = m_spTargetAvatar->GetAnimation(kName.c_str());
////	pkHandle->m_kResAnimation = m_spResource->GetAnimation(kName);
////	pkHandle->m_bIsLevelAnimation =
////		pkHandle->m_kResAnimation.m_u32Level < m_spResource->m_u32LevelNumber;
////
////	FvStringMap<FvUInt32>::iterator iter = m_spResource->m_kBindingMap.find(kName);
////	pkHandle->m_i32Binding = iter == m_spResource->m_kBindingMap.end() ? -1 : iter->second;
////
////	return pkHandle->m_i32Animation >= 0;
////}
////----------------------------------------------------------------------------
//FvAnimationShell::AnimationHandle* FvAnimationShell::GetAnimationHandleByName(
//	const FvString& kName)
//{
//	FvStringMap<AnimationHandle*>::iterator it = m_kHandles.find(kName);
//
//	if(it == m_kHandles.end())
//	{
//		FvString kRealName;
//
//		FvStringMap<FvString>::iterator aliasIt = m_spResource->m_kHandleAliasMap.find(kName);
//
//		if(aliasIt == m_spResource->m_kHandleAliasMap.end())
//		{
//			kRealName = kName;
//		}
//		else
//		{
//			kRealName = aliasIt->second;
//		}
//
//		FvInt32 i32Animation = m_spTargetAvatar->GetAnimation(kRealName.c_str());
//
//		if(i32Animation < 0) return NULL;
//
//		AnimationHandle* pkHandle = new AnimationHandle();
//
//		pkHandle->m_kAnimationName = kRealName;
//		pkHandle->m_pkParent = this;
//		pkHandle->m_i32Animation = i32Animation;
//		pkHandle->m_kResAnimation = m_spResource->GetAnimation(kRealName);
//		pkHandle->m_bIsLevelAnimation =
//			pkHandle->m_kResAnimation.m_u32Level < m_spResource->m_u32LevelNumber;
//
//		FvStringMap<FvUInt32>::iterator iter = m_spResource->m_kBindingMap.find(kRealName);
//		pkHandle->m_i32Binding = iter == m_spResource->m_kBindingMap.end() ? -1 : iter->second;
//
//		it = m_kHandles.insert(FvStringMap<AnimationHandle*>::value_type(kName.c_str(), pkHandle));
//	}
//
//	return it->second;
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::PlayAnimation(AnimationHandle* pkHandle,
//	PlayType eType)
//{
//	static const char* apcPlayType[4] =
//	{
//		"DEFAULT",
//		"REMOVE",
//		"INSERT",
//		"INSERT_BREAK"
//	};
//
//#ifdef FV_MODEL_INFO
//	//FV_INFO_MSG("%p->FvAnimationShell::PlayAnimation(%s,%s)\n", this, pkHandle->m_kAnimationName.c_str(), apcPlayType[eType]);
//#endif
//
//	//FV_TRACE_MSG("%p->FvAnimationShell::PlayAnimation(%s,%s)\n", this, pkHandle->m_kAnimationName.c_str(), apcPlayType[eType]);
//
//	FV_ASSERT(pkHandle && pkHandle->m_pkParent == this);
//
//	if(pkHandle->m_i32Animation >= 0)
//	{
//		switch(eType)
//		{
//		case DEFAULT:
//			{
//				if(pkHandle->m_bIsLevelAnimation)
//				{
//					Level* pkLevel = &(m_pkLevels[pkHandle->GetLevel()]);
//					pkLevel->m_pkNextLevelAnimation = pkHandle;
//					//pkLevel->m_bNeedClear = false;
//				}
//				else
//				{
//					m_kExtraAnimationEnable.AttachBack(*pkHandle);
//				}
//			}
//			break;
//		case REMOVE:
//			{
//				if(pkHandle->m_bIsLevelAnimation)
//				{
//					Level* pkLevel = &(m_pkLevels[pkHandle->GetLevel()]);
//
//					if(pkLevel->m_pkCurrentLevelAnimation == pkHandle || pkLevel->m_pkNextLevelAnimation == pkHandle)
//					{
//						pkLevel->m_pkNextLevelAnimation = NULL;
//					}
//
//					if(pkLevel->m_pkCurrentAnimation == pkHandle && pkLevel->m_eState == Level::STATE_INSERT)
//					{
//						pkLevel->m_pkCurrentAnimation->SetTimePosition(0);
//						pkLevel->m_bNeedRemoveInsert = true;
//
//						m_spTargetAvatar->SetCallback(pkLevel->m_pkCurrentAnimation->m_i32Animation, NULL);
//
//						pkLevel->m_kInsertQueue.clear();
//					}
//
//					if(pkLevel->m_pkBreakAnimation == pkHandle)
//					{
//						pkLevel->m_pkBreakAnimation = NULL;
//						pkLevel->m_kInsertQueue.clear();
//					}
//					else
//					{
//						std::list<AnimationHandle*>::iterator iter = pkLevel->m_kInsertQueue.begin();
//						while (iter != pkLevel->m_kInsertQueue.end())
//						{
//							if((*iter) == pkHandle)
//							{
//								break;
//							}
//							++iter;
//						}
//						if(iter != pkLevel->m_kInsertQueue.end())
//						{
//							pkLevel->m_kInsertQueue.erase(iter, pkLevel->m_kInsertQueue.end());
//						}
//					}
//
//				}
//				else
//				{
//					m_kExtraAnimationDisable.AttachBack(*pkHandle);
//				}
//			}
//			break;
//		case INSERT:
//			{
//				if(pkHandle->m_bIsLevelAnimation)
//				{
//					Level* pkLevel = &(m_pkLevels[pkHandle->GetLevel()]);
//					
//					pkLevel->m_kInsertQueue.push_back(pkHandle);
//					//pkLevel->m_bNeedClear = false;
//				}
//				else
//				{
//					FvInt32 i32Enable = m_spTargetAvatar->GetEnable(pkHandle->m_i32Animation);
//
//					if(i32Enable >= 0)
//					{
//						++(pkHandle->m_u32PlayNumber);
//
//						m_spTargetAvatar->EnableCallback(pkHandle->m_i32Animation);
//						m_spTargetAvatar->RegisterCallback(pkHandle->m_i32Animation,
//							FvAvatarAnimationState::ANI_END, NULL);
//						m_spTargetAvatar->SetCallback(pkHandle->m_i32Animation, this);
//					}
//					else
//					{
//						m_spTargetAvatar->EnableCallback(pkHandle->m_i32Animation);
//						m_spTargetAvatar->RegisterCallback(pkHandle->m_i32Animation,
//							FvAvatarAnimationState::ANI_END, NULL);
//						m_spTargetAvatar->SetCallback(pkHandle->m_i32Animation, this);
//
//						m_au32AnimationSet[m_u32AnimationUse] = (FvUInt32)pkHandle;
//						m_spTargetAvatar->SetAnimationUsedNumber(++m_u32AnimationUse);
//
//						FvInt32 i32Partition = m_spTargetAvatar->GetPartition(((FvAnimationShellResource::ExtraAnimation*)(pkHandle->m_kResAnimation.m_pvPointer))->m_kPartition.c_str());
//						m_spTargetAvatar->SetPartition(m_u32AnimationUse - 1, i32Partition);
//						m_spTargetAvatar->SetAnimation(m_u32AnimationUse - 1, pkHandle->m_i32Animation);
//						pkHandle->m_u32PlayNumber = 0;
//
//						if(pkHandle->m_pkCallBack)
//							pkHandle->m_pkCallBack(pkHandle, AnimationHandle::INSERT_START);
//					}
//				}
//			}
//			break;
//		case INSERT_BREAK_SOFT:
//			{
//				if(pkHandle->m_bIsLevelAnimation)
//				{
//					Level* pkLevel = &(m_pkLevels[pkHandle->GetLevel()]);
//
//					if(pkLevel->m_eState == Level::STATE_INSERT && pkLevel->m_pkCurrentAnimation == pkHandle)
//					{
//						pkLevel->m_kInsertQueue.clear();
//					}
//					else
//					{
//						pkLevel->m_pkBreakAnimation = pkHandle;
//						pkLevel->m_kInsertQueue.clear();
//					}
//
//					//pkLevel->m_bNeedClear = false;
//				}
//				else
//				{
//					m_spTargetAvatar->EnableCallback(pkHandle->m_i32Animation);
//					m_spTargetAvatar->RegisterCallback(pkHandle->m_i32Animation,
//						FvAvatarAnimationState::ANI_END, NULL);
//					m_spTargetAvatar->SetCallback(pkHandle->m_i32Animation, this);
//
//					FvInt32 i32Enable = m_spTargetAvatar->GetEnable(pkHandle->m_i32Animation);
//
//					if(i32Enable >= 0)
//					{						
//						pkHandle->m_u32PlayNumber = 0;
//					}
//					else
//					{
//						m_au32AnimationSet[m_u32AnimationUse] = (FvUInt32)pkHandle;
//						m_spTargetAvatar->SetAnimationUsedNumber(++m_u32AnimationUse);
//
//						FvInt32 i32Partition = m_spTargetAvatar->GetPartition(((FvAnimationShellResource::ExtraAnimation*)(pkHandle->m_kResAnimation.m_pvPointer))->m_kPartition.c_str());
//						m_spTargetAvatar->SetPartition(m_u32AnimationUse - 1, i32Partition);
//						m_spTargetAvatar->SetAnimation(m_u32AnimationUse - 1, pkHandle->m_i32Animation);
//						m_spTargetAvatar->SetTimePosition(m_u32AnimationUse - 1, 0);
//						pkHandle->m_u32PlayNumber = 0;
//
//						if(pkHandle->m_pkCallBack)
//							pkHandle->m_pkCallBack(pkHandle, AnimationHandle::INSERT_START);
//					}					
//				}
//			}
//			break;
//		case INSERT_BREAK:
//			{
//				if(pkHandle->m_bIsLevelAnimation)
//				{
//					Level* pkLevel = &(m_pkLevels[pkHandle->GetLevel()]);
//
//					pkLevel->m_pkBreakAnimation = pkHandle;
//					//pkLevel->m_bNeedClear = false;
//					pkLevel->m_kInsertQueue.clear();
//				}
//				else
//				{
//					m_spTargetAvatar->EnableCallback(pkHandle->m_i32Animation);
//					m_spTargetAvatar->RegisterCallback(pkHandle->m_i32Animation,
//						FvAvatarAnimationState::ANI_END, NULL);
//					m_spTargetAvatar->SetCallback(pkHandle->m_i32Animation, this);
//
//					FvInt32 i32Enable = m_spTargetAvatar->GetEnable(pkHandle->m_i32Animation);
//
//					if(i32Enable >= 0)
//					{						
//						m_spTargetAvatar->SetTimePosition(i32Enable, 0);
//						pkHandle->m_u32PlayNumber = 0;
//					}
//					else
//					{
//						m_au32AnimationSet[m_u32AnimationUse] = (FvUInt32)pkHandle;
//						m_spTargetAvatar->SetAnimationUsedNumber(++m_u32AnimationUse);
//
//						FvInt32 i32Partition = m_spTargetAvatar->GetPartition(((FvAnimationShellResource::ExtraAnimation*)(pkHandle->m_kResAnimation.m_pvPointer))->m_kPartition.c_str());
//						m_spTargetAvatar->SetPartition(m_u32AnimationUse - 1, i32Partition);
//						m_spTargetAvatar->SetAnimation(m_u32AnimationUse - 1, pkHandle->m_i32Animation);
//						m_spTargetAvatar->SetTimePosition(m_u32AnimationUse - 1, 0);
//						pkHandle->m_u32PlayNumber = 0;						
//					}
//
//					if(pkHandle->m_pkCallBack)
//						pkHandle->m_pkCallBack(pkHandle, AnimationHandle::INSERT_START);
//				}
//			}
//			break;
//		default:
//			break;
//		}
//	}
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::AddTime(float fDeltaTime)
//{
//	for(FvUInt32 i(0); i < m_spResource->m_u32LevelNumber; ++i)
//	{
//		m_pkLevels[i].AddTime(fDeltaTime, this);
//	}
//
//	for(FvUInt32 i(0); i < m_spResource->m_u32LevelNumber; ++i)
//	{
//		m_pkLevels[i].Play(this, i);
//	}
//
//	for(FvUInt32 i(0); i < m_spResource->m_u32LevelNumber; ++i)
//	{
//		if(m_pkLevels[i].m_eState == Level::STATE_OFF) continue;
//
//		if((m_pkLevels[i].m_u32CurrentPartition) > 0 && (m_pkLevels[i].m_u32CurrentPartition < m_spResource->m_u32AllMask))
//		{
//			FvInt32 i32Animation = m_spTargetAvatar->GetAnimation(m_pkLevels[0].m_u32AnimationPosition);
//
//			if(i32Animation >= 0 && (i32Animation != m_pkLevels[0].m_pkCurrentAnimation->m_i32Animation))
//			{
//				m_spTargetAvatar->SetAnimation(m_pkLevels[0].m_u32AnimationPosition, FvAvatar::DO_NOT_USE);
//			}
//		}
//
//		if(m_pkLevels[i].m_bNeedPlay)
//		{
//			if(i)
//			{
//				FV_ASSERT(m_pkLevels[i].m_u32CurrentPartition == 0);
//
//				m_spTargetAvatar->SetPartition(m_pkLevels[i].m_u32AnimationPosition, m_pkLevels[i].m_u32CurrentPartition);
//				m_spTargetAvatar->SetAnimation(m_pkLevels[i].m_u32AnimationPosition, m_pkLevels[i].m_pkCurrentAnimation->m_i32Animation);
//
//				m_spTargetAvatar->SetTimePosition(m_pkLevels[i].m_u32AnimationPosition, m_pkLevels[i].m_pkCurrentAnimation->m_fTimePosition);
//				m_pkLevels[i].m_pkCurrentAnimation->m_fTimePosition = -1.f;
//			}
//			
//			m_pkLevels[i].m_bNeedPlay = false;
//		}
//	}
//
//	FvRefList<AnimationHandle*>::iterator iter = m_kExtraAnimationEnable.GetHead();
//	while (!m_kExtraAnimationEnable.IsEnd(iter))
//	{
//		AnimationHandle* pkHandle = (*iter).m_Content;
//		FvRefList<AnimationHandle*>::Next(iter);		
//		FV_ASSERT(pkHandle);
//		
//		FvInt32 i32Enable = m_spTargetAvatar->GetEnable(pkHandle->m_i32Animation);
//
//		if(i32Enable >= 0)
//		{
//			pkHandle->m_u32PlayNumber = 0;
//
//			m_spTargetAvatar->EnableCallback(pkHandle->m_i32Animation);
//			m_spTargetAvatar->SetCallback(pkHandle->m_i32Animation, NULL);
//		}
//		else
//		{
//			m_au32AnimationSet[m_u32AnimationUse] = (FvUInt32)pkHandle;
//			m_spTargetAvatar->SetAnimationUsedNumber(++m_u32AnimationUse);
//
//			FvInt32 i32Partition = m_spTargetAvatar->GetPartition(((FvAnimationShellResource::ExtraAnimation*)(pkHandle->m_kResAnimation.m_pvPointer))->m_kPartition.c_str());
//			m_spTargetAvatar->SetPartition(m_u32AnimationUse - 1, i32Partition);
//			m_spTargetAvatar->SetAnimation(m_u32AnimationUse - 1, pkHandle->m_i32Animation);
//
//			if(pkHandle->m_fTimePosition >= 0)
//			{
//				m_spTargetAvatar->SetTimePosition(m_u32AnimationUse - 1, pkHandle->m_fTimePosition);
//				pkHandle->m_fTimePosition = -1;
//			}
//
//			pkHandle->m_u32PlayNumber = 0;
//		}
//	}
//
//	m_kExtraAnimationEnable.Clear();
//
//	iter = m_kExtraAnimationDisable.GetHead();
//	while (!m_kExtraAnimationDisable.IsEnd(iter))
//	{
//		AnimationHandle* pkHandle = (*iter).m_Content;
//		FvRefList<AnimationHandle*>::Next(iter);		
//		FV_ASSERT(pkHandle);
//
//		FvInt32 i32Enable = m_spTargetAvatar->GetEnable(pkHandle->m_i32Animation);
//
//		if(i32Enable >= 0)
//		{
//			pkHandle->m_u32PlayNumber = 0;
//			m_spTargetAvatar->EnableCallback(pkHandle->m_i32Animation);
//			m_spTargetAvatar->SetCallback(pkHandle->m_i32Animation, NULL);
//			RemoveAnimation(i32Enable);
//		}
//	}
//
//	m_kExtraAnimationDisable.Clear();
//
//	m_spTargetAvatar->AddTime(fDeltaTime);
//}
////----------------------------------------------------------------------------
//FvUInt32 FvAnimationShell::GetMaskedPartitionNumber()
//{
//	return m_spResource->m_u32MaskedPartitionNumber;
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::RemoveAnimation(FvUInt32 u32Pos)
//{
//	FV_ASSERT(u32Pos < m_u32AnimationUse);
//	if(u32Pos != m_u32AnimationUse - 1)
//	{
//		m_au32AnimationSet[u32Pos] = m_au32AnimationSet[m_u32AnimationUse - 1];
//		m_spTargetAvatar->SetPartition(u32Pos,
//			m_spTargetAvatar->GetPartition(m_u32AnimationUse - 1));
//		m_spTargetAvatar->SetAnimation(u32Pos,
//			m_spTargetAvatar->GetAnimation(m_u32AnimationUse - 1));
//
//		if(m_au32AnimationSet[u32Pos] < m_spResource->m_u32LevelNumber)
//		{
//			m_pkLevels[m_au32AnimationSet[u32Pos]].m_u32AnimationPosition
//				= u32Pos;
//		}
//	}
//
//	m_spTargetAvatar->SetAnimationUsedNumber(--m_u32AnimationUse);
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::EventCome(FvAvatar* pkAvatar, FvInt32 i32Animation,
//	FvAvatarAnimationState::EventType eType, const char* pcMessage)
//{
//	FvInt32 i32Enable = m_spTargetAvatar->GetEnable(i32Animation);
//	FV_ASSERT(i32Enable >= 0 && i32Enable < (FvInt32)m_u32AnimationUse);
//	AnimationHandle* pkHandle = (AnimationHandle*)(m_au32AnimationSet[i32Enable]);
//
//	if(pkHandle->m_u32PlayNumber)
//	{
//		--(pkHandle->m_u32PlayNumber);
//
//		m_spTargetAvatar->SetTimePosition(i32Enable, 0);
//
//		if(pkHandle->m_pkCallBack)
//		{
//			pkHandle->m_pkCallBack(pkHandle, AnimationHandle::INSERT_END);
//			pkHandle->m_pkCallBack(pkHandle, AnimationHandle::INSERT_START);
//		}
//	}
//	else
//	{
//		m_spTargetAvatar->SetCallback(i32Animation, NULL);
//		RemoveAnimation(i32Enable);
//
//		if(pkHandle->m_pkCallBack)
//			pkHandle->m_pkCallBack(pkHandle, AnimationHandle::INSERT_END);
//	}
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::SetScale(const FvString& kGroup, float fScale)
//{
//	FvStringMap<FvUInt32>::iterator it = m_spResource->m_kGroupMap.find(kGroup);
//
//	if(it != m_spResource->m_kGroupMap.end())
//	{
//		FvUInt32 u32ID = it->second;
//		
//		m_spResource->m_pkGroups[u32ID].m_fScale = fScale;
//
//		std::map<FvUInt32, std::list<FvUInt32>>::iterator iter
//			= m_spResource->m_kGroupGroupMap.find(u32ID);
//
//		if(iter != m_spResource->m_kGroupGroupMap.end())
//		{
//			for(std::list<FvUInt32>::iterator i = iter->second.begin();
//				i != iter->second.end(); ++i)
//			{
//				fScale *= m_spResource->m_pkGroups[*i].m_fScale;
//			}
//		}
//
//		for(FvUInt32 i(0); i < m_spResource->m_pkGroups[u32ID].m_u32AnimationNumber; ++i)
//		{
//			FvInt32 i32Animation = m_spTargetAvatar->GetAnimation(
//				m_spResource->m_pkGroups[u32ID].m_pkAnimationNames[i].c_str());
//			
//			if(i32Animation >= 0)
//			{
//				m_spTargetAvatar->SetTimeScale(i32Animation, fScale);
//			}
//		}
//
//		for(FvUInt32 i(0); i < m_spResource->m_pkGroups[u32ID].m_u32SubGroupNumber; ++i)
//		{
//			SetScale(m_spResource->m_pkGroups[u32ID].m_ppkSubGroups[i]->m_kName,
//				m_spResource->m_pkGroups[u32ID].m_ppkSubGroups[i]->m_fScale);
//		}
//	}
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::StopLevel(const FvString& kLevel)
//{
//	FvStringMap<FvUInt32>::iterator it
//		= m_spResource->m_kLevelMap.find(kLevel);
//
//	if(it != m_spResource->m_kLevelMap.end())
//	{
//		m_pkLevels[it->second].m_pkNextLevelAnimation = NULL;
//
//		m_pkLevels[it->second].m_bNeedRemoveInsert = true;
//
//		if(m_pkLevels[it->second].m_pkCurrentAnimation)
//		{
//			m_spTargetAvatar->EnableCallback(m_pkLevels[it->second].m_pkCurrentAnimation->m_i32Animation);
//			m_spTargetAvatar->SetCallback(m_pkLevels[it->second].m_pkCurrentAnimation->m_i32Animation, NULL);
//		}
//
//		m_pkLevels[it->second].m_kInsertQueue.clear();
//
//		m_pkLevels[it->second].m_pkBreakAnimation = NULL;
//	}
//}
////----------------------------------------------------------------------------
//FvAnimationShell::AnimationHandle::AnimationHandle() : m_pkParent(NULL),
//	m_i32Animation(0), m_fTimePosition(-1), m_pkCallBack(NULL),
//	m_u32PlayNumber(0), m_i32Binding(-1)
//{
//	m_Content = this;
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::AnimationHandle::SetLoop(bool bLoop)
//{
//	FV_ASSERT(m_pkParent && (m_i32Animation >= 0));
//	m_pkParent->m_spTargetAvatar->SetLoop(m_i32Animation, bLoop);
//}
////----------------------------------------------------------------------------
//FvUInt32* FvAnimationShell::AnimationHandle::GetPriorities()
//{
//	if(m_i32Animation >= 0 && m_bIsLevelAnimation)
//	{
//		return ((FvAnimationShellResource::LevelAnimation*)(m_kResAnimation.m_pvPointer))->m_pu32Priorities
//			? ((FvAnimationShellResource::LevelAnimation*)(m_kResAnimation.m_pvPointer))->m_pu32Priorities
//			: m_pkParent->m_spResource->m_pkLevelArray[m_kResAnimation.m_u32Level].m_pu32DefaultPriorities;
//	}
//	else
//	{
//		return NULL;
//	}
//}
////----------------------------------------------------------------------------
//FvUInt32 FvAnimationShell::AnimationHandle::GetLevel()
//{
//	return m_kResAnimation.m_u32Level;
//}
////----------------------------------------------------------------------------
//float FvAnimationShell::AnimationHandle::GetLength()
//{
//	return m_pkParent->m_spTargetAvatar->GetLength(m_i32Animation);
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::AnimationHandle::SetTimePosition(float fTimePos)
//{
//	m_fTimePosition = fTimePos;
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::AnimationHandle::GotoTheEnd()
//{
//	SetTimePosition(GetLength());
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::AnimationHandle::SetCallback(CallBack pkCallback)
//{
//	m_pkCallBack = pkCallback;
//}
////----------------------------------------------------------------------------
//const FvString& FvAnimationShell::AnimationHandle::GetName()
//{
//	return m_kAnimationName;
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::AnimationHandle::SetTimePositionDirection(
//	float fTimePos)
//{
//	FvInt32 i32Pos = m_pkParent->m_spTargetAvatar->GetEnable(m_i32Animation);
//
//	if(i32Pos >= 0)
//	{
//		m_pkParent->m_spTargetAvatar->SetTimePosition(i32Pos, fTimePos);
//	}
//}
////----------------------------------------------------------------------------
//FvAnimationShell::Level::Level() : m_eState(STATE_OFF),
//	m_pkCurrentAnimation(NULL), m_pkCurrentLevelAnimation(NULL),
//	m_pkNextLevelAnimation(NULL), m_fInsertPassTime(0), 
//	m_u32AnimationPosition(0), m_u32CurrentPartition(0), m_pkLevelNodes(NULL),
//	m_bNeedPlay(false),	m_bDirtyPartition(false), m_pkBreakAnimation(NULL),
//	/*m_bNeedClear(false), */m_bNeedRemoveInsert(false)
//{
//	
//}
////----------------------------------------------------------------------------
//FvAnimationShell::Level::~Level()
//{
//	FV_SAFE_DELETE_ARRAY(m_pkLevelNodes);
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::Level::Init(FvUInt32 u32PartNumber)
//{
//	m_pkLevelNodes = new FvRefNode2<Level*>[u32PartNumber];
//
//	for(FvUInt32 i(0); i < u32PartNumber; ++i)
//	{
//		m_pkLevelNodes[i].m_Content = this;
//	}
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::Level::AddTime(float fDeltaTime,
//	FvAnimationShell* pkParent)
//{
//	/*if(m_bNeedClear)
//	{
//		m_pkCurrentLevelAnimation = NULL;
//		m_pkNextLevelAnimation = NULL;
//		m_fInsertPassTime = 0;
//		m_kInsertQueue.clear();
//		m_pkBreakAnimation = NULL;
//
//		if(m_pkCurrentAnimation)
//		{
//			PartitionRemove(pkParent);
//			AnimationRemove(pkParent);
//
//			m_pkCurrentAnimation = NULL;
//		}
//		
//		m_eState = STATE_OFF;
//		
//		m_bNeedClear = false;
//		m_bNeedPlay = false;
//		m_bDirtyPartition = false;
//		m_bNeedRemoveInsert = false;
//		return;
//	}*/
//
//	if(m_eState == STATE_INSERT)
//	{
//		if(m_bNeedRemoveInsert)
//		{
//			m_pkCurrentAnimation = NULL;
//
//			m_bNeedRemoveInsert = false;
//		}
//
//		if(m_pkBreakAnimation)
//		{
//			if(m_pkCurrentAnimation && m_pkCurrentAnimation->m_pkCallBack)
//				m_pkCurrentAnimation->m_pkCallBack(m_pkCurrentAnimation, AnimationHandle::INSERT_BREAKED);
//
//			m_pkBreakAnimation->SetTimePosition(0);
//
//			m_pkCurrentAnimation = m_pkBreakAnimation;
//			m_pkBreakAnimation = NULL;
//
//			if(m_pkCurrentAnimation && m_pkCurrentAnimation->m_pkCallBack)
//				m_pkCurrentAnimation->m_pkCallBack(m_pkCurrentAnimation, AnimationHandle::INSERT_START);
//
//			pkParent->m_spTargetAvatar->EnableCallback(m_pkCurrentAnimation->m_i32Animation);
//			pkParent->m_spTargetAvatar->RegisterCallback(m_pkCurrentAnimation->m_i32Animation,
//				FvAvatarAnimationState::ANI_END, NULL);
//			pkParent->m_spTargetAvatar->SetCallback(m_pkCurrentAnimation->m_i32Animation, this);
//
//			PartitionPre(pkParent);
//			m_bNeedPlay = true;
//		}
//
//		if(m_pkCurrentAnimation)
//		{
//			if(m_pkCurrentLevelAnimation != m_pkNextLevelAnimation)
//			{
//				m_pkCurrentLevelAnimation = m_pkNextLevelAnimation;
//				m_fInsertPassTime = 0;
//			}
//			else
//			{
//				m_fInsertPassTime += fDeltaTime;
//			}
//		}
//		else
//		{
//			if(m_pkCurrentLevelAnimation != m_pkNextLevelAnimation)
//			{
//				m_pkCurrentLevelAnimation = m_pkNextLevelAnimation;
//				m_fInsertPassTime = 0;
//			}
//
//			if(m_kInsertQueue.size())
//			{
//				std::list<AnimationHandle*>::iterator it = m_kInsertQueue.begin();
//
//				m_pkCurrentAnimation = *it;
//
//				m_kInsertQueue.erase(it);
//
//				if(m_pkCurrentAnimation && m_pkCurrentAnimation->m_pkCallBack)
//					m_pkCurrentAnimation->m_pkCallBack(m_pkCurrentAnimation, AnimationHandle::INSERT_START);
//
//				pkParent->m_spTargetAvatar->EnableCallback(m_pkCurrentAnimation->m_i32Animation);
//				pkParent->m_spTargetAvatar->RegisterCallback(m_pkCurrentAnimation->m_i32Animation,
//					FvAvatarAnimationState::ANI_END, NULL);
//				pkParent->m_spTargetAvatar->SetCallback(m_pkCurrentAnimation->m_i32Animation, this);
//
//				PartitionPre(pkParent);
//				m_bNeedPlay = true;
//			}
//			else
//			{
//				if(m_pkCurrentLevelAnimation)
//				{
//					m_eState = STATE_DEFAULT;
//
//					m_pkCurrentAnimation = m_pkCurrentLevelAnimation;
//					m_pkCurrentAnimation->SetTimePosition(m_fInsertPassTime);
//
//					if(m_pkCurrentLevelAnimation && m_pkCurrentLevelAnimation->m_pkCallBack)
//						m_pkCurrentLevelAnimation->m_pkCallBack(m_pkCurrentLevelAnimation, AnimationHandle::RESUME);
//
//					PartitionPre(pkParent);
//					m_bNeedPlay = true;
//				}
//				else
//				{
//					m_eState = STATE_OFF;
//
//					PartitionRemove(pkParent);
//					AnimationRemove(pkParent);
//				}
//			}
//		}
//	}
//	else
//	{
//		m_bNeedRemoveInsert = false;
//
//		if(m_pkBreakAnimation)
//		{
//			if(m_pkCurrentAnimation)
//			{
//				//FV_ASSERT(m_pkCurrentLevelAnimation == m_pkCurrentAnimation);
//				if(m_pkCurrentAnimation->m_pkCallBack)
//					m_pkCurrentAnimation->m_pkCallBack(m_pkCurrentAnimation, AnimationHandle::HUNG_UP);
//
//				m_fInsertPassTime = pkParent->m_spTargetAvatar->GetTimePosition(m_u32CurrentPartition == FvAvatar::DO_NOT_USE
//					? pkParent->m_pkLevels[0].m_u32AnimationPosition : m_u32AnimationPosition);
//
//				if(m_pkCurrentAnimation == m_pkBreakAnimation)
//					m_pkBreakAnimation->SetTimePosition(0);
//			}
//
//			m_pkCurrentAnimation = m_pkBreakAnimation;
//			m_pkBreakAnimation = NULL;
//
//			if(m_pkCurrentAnimation && m_pkCurrentAnimation->m_pkCallBack)
//				m_pkCurrentAnimation->m_pkCallBack(m_pkCurrentAnimation, AnimationHandle::INSERT_START);
//
//			pkParent->m_spTargetAvatar->EnableCallback(m_pkCurrentAnimation->m_i32Animation);
//			pkParent->m_spTargetAvatar->RegisterCallback(m_pkCurrentAnimation->m_i32Animation,
//				FvAvatarAnimationState::ANI_END, NULL);
//			pkParent->m_spTargetAvatar->SetCallback(m_pkCurrentAnimation->m_i32Animation, this);
//
//			if(m_eState == STATE_OFF)
//			{
//				m_eState = STATE_DEFAULT;
//
//				pkParent->m_au32AnimationSet[pkParent->m_u32AnimationUse] = m_pkCurrentAnimation->GetLevel();
//
//				m_u32AnimationPosition = pkParent->m_u32AnimationUse;
//				pkParent->m_spTargetAvatar->SetAnimationUsedNumber(++(pkParent->m_u32AnimationUse));
//			}
//
//			m_eState = STATE_INSERT;
//
//			PartitionPre(pkParent);
//			m_bNeedPlay = true;
//		}
//		else if(m_kInsertQueue.size())
//		{
//			if(m_pkCurrentAnimation)
//			{
//				FV_ASSERT(m_pkCurrentLevelAnimation == m_pkCurrentAnimation);
//				if(m_pkCurrentAnimation->m_pkCallBack)
//					m_pkCurrentAnimation->m_pkCallBack(m_pkCurrentAnimation, AnimationHandle::HUNG_UP);
//
//				m_fInsertPassTime = pkParent->m_spTargetAvatar->GetTimePosition(m_u32CurrentPartition == FvAvatar::DO_NOT_USE
//					? pkParent->m_pkLevels[0].m_u32AnimationPosition : m_u32AnimationPosition);
//			}
//
//			std::list<AnimationHandle*>::iterator it = m_kInsertQueue.begin();
//
//			AnimationHandle* pkInsert = *it;
//
//			m_kInsertQueue.erase(it);
//
//			m_pkCurrentAnimation = pkInsert;
//
//			if(m_pkCurrentAnimation && m_pkCurrentAnimation->m_pkCallBack)
//				m_pkCurrentAnimation->m_pkCallBack(m_pkCurrentAnimation, AnimationHandle::INSERT_START);
//
//			pkParent->m_spTargetAvatar->EnableCallback(m_pkCurrentAnimation->m_i32Animation);
//			pkParent->m_spTargetAvatar->RegisterCallback(m_pkCurrentAnimation->m_i32Animation,
//				FvAvatarAnimationState::ANI_END, NULL);
//			pkParent->m_spTargetAvatar->SetCallback(m_pkCurrentAnimation->m_i32Animation, this);
//
//			if(m_eState == STATE_OFF)
//			{
//				m_eState = STATE_DEFAULT;
//
//				pkParent->m_au32AnimationSet[pkParent->m_u32AnimationUse] = m_pkCurrentAnimation->GetLevel();
//
//				m_u32AnimationPosition = pkParent->m_u32AnimationUse;
//				pkParent->m_spTargetAvatar->SetAnimationUsedNumber(++(pkParent->m_u32AnimationUse));
//			}
//
//			m_eState = STATE_INSERT;
//
//			PartitionPre(pkParent);
//			m_bNeedPlay = true;
//		}
//
//		if(m_pkCurrentLevelAnimation != m_pkNextLevelAnimation)
//		{
//			if(m_eState != STATE_INSERT)
//			{
//				if(m_pkCurrentAnimation && m_pkCurrentAnimation == m_pkCurrentLevelAnimation && m_pkNextLevelAnimation)
//				{
//					if(m_pkCurrentAnimation->m_i32Binding == m_pkNextLevelAnimation->m_i32Binding)
//					{
//						FvInt32 i32Enable = pkParent->m_spTargetAvatar->GetEnable(m_pkCurrentAnimation->m_i32Animation);
//
//						if(i32Enable >= 0)
//						{
//							m_pkNextLevelAnimation->SetTimePosition(pkParent->m_spTargetAvatar->GetTimePosition(i32Enable));
//						}
//					}
//				}
//
//				m_pkCurrentAnimation = m_pkCurrentLevelAnimation = m_pkNextLevelAnimation;
//
//				if(m_pkCurrentAnimation)
//				{
//					if(m_eState == STATE_OFF)
//					{
//						m_eState = STATE_DEFAULT;
//
//						pkParent->m_au32AnimationSet[pkParent->m_u32AnimationUse] = m_pkCurrentAnimation->GetLevel();
//
//						m_u32AnimationPosition = pkParent->m_u32AnimationUse;
//						pkParent->m_spTargetAvatar->SetAnimationUsedNumber(++(pkParent->m_u32AnimationUse));
//					}
//
//					PartitionPre(pkParent);
//					m_bNeedPlay = true;
//				}
//				else
//				{
//					FV_ASSERT(m_eState == STATE_DEFAULT);
//
//					m_eState = STATE_OFF;
//
//					PartitionRemove(pkParent);
//					AnimationRemove(pkParent);
//				}
//			}
//			else
//			{
//				m_pkCurrentLevelAnimation = m_pkNextLevelAnimation;
//				m_fInsertPassTime = 0;
//			}
//		}
//
//	}
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::Level::Play(FvAnimationShell* pkParent, FvUInt32 i)
//{
//	//if(m_pkCurrentAnimation);
//
//	if(m_bNeedPlay)
//	{
//		FV_ASSERT(m_pkCurrentAnimation);
//
//		FvUInt32 u32NewPartition = 0;
//
//		for(FvUInt32 j(0); j < pkParent->GetMaskedPartitionNumber(); ++j)
//		{
//			if(pkParent->m_pkLists[j].GetHead()->m_Content == this)
//			{
//				FV_MASK_ADD(u32NewPartition, FV_MASK(j));
//			}		
//		}
//
//		//if(u32NewPartition == pkParent->m_spResource->m_u32AllMask)
//		//	u32NewPartition = FvAvatar::DO_NOT_USE;
//
//		if(u32NewPartition == pkParent->m_spResource->m_u32AllMask)
//		{
//			u32NewPartition = FvAvatar::DO_NOT_USE;
//			pkParent->m_spTargetAvatar->SetPartition(pkParent->m_pkLevels[0].m_u32AnimationPosition, FvAvatar::DO_NOT_USE);
//			pkParent->m_spTargetAvatar->SetAnimation(pkParent->m_pkLevels[0].m_u32AnimationPosition, m_pkCurrentAnimation->m_i32Animation);
//
//			if(pkParent->m_pkLevels[0].m_u32AnimationPosition != m_u32AnimationPosition)
//				pkParent->m_spTargetAvatar->SetAnimation(m_u32AnimationPosition, FvAvatar::DO_NOT_USE);
//		}
//		else
//		{
//			if(m_u32CurrentPartition == 0 && u32NewPartition != 0 && i == 0)
//			{
//				FvInt32 i32Animation = pkParent->m_spTargetAvatar->GetAnimation(m_u32AnimationPosition);
//
//				if(i32Animation >= 0)
//				{
//					FvString kAnimation = pkParent->m_spTargetAvatar->GetAnimationName(i32Animation);
//
//					FvUInt32 u32Level = pkParent->m_spResource->m_kAnimationMap.find(kAnimation)->second.m_u32Level;
//
//					if(u32Level != 0)
//					{
//						FvUInt32 u32Pos = pkParent->m_pkLevels[u32Level].m_u32AnimationPosition;
//
//						pkParent->m_spTargetAvatar->SetPartition(u32Pos, pkParent->m_spTargetAvatar->GetPartition(m_u32AnimationPosition));
//						pkParent->m_spTargetAvatar->SetAnimation(u32Pos, pkParent->m_spTargetAvatar->GetAnimation(m_u32AnimationPosition));
//					}
//				}				
//			}
//
//			if(u32NewPartition)
//			{
//				//PartitionPost(pkParent);
//				pkParent->m_spTargetAvatar->SetPartition(m_u32AnimationPosition, u32NewPartition);
//				pkParent->m_spTargetAvatar->SetAnimation(m_u32AnimationPosition, m_pkCurrentAnimation->m_i32Animation);
//			}
//		}
//
//		m_u32CurrentPartition = u32NewPartition;
//
//		if(u32NewPartition)
//		{
//			if(m_pkCurrentAnimation->m_fTimePosition >= 0.f)
//			{
//				pkParent->m_spTargetAvatar->SetTimePosition(m_u32CurrentPartition == FvAvatar::DO_NOT_USE
//					? pkParent->m_pkLevels[0].m_u32AnimationPosition : m_u32AnimationPosition, m_pkCurrentAnimation->m_fTimePosition);
//				m_pkCurrentAnimation->m_fTimePosition = -1.f;
//			}
//
//			m_bNeedPlay = false;
//		}
//		m_bDirtyPartition = false;
//	}
//	else if(m_bDirtyPartition)
//	{
//		if(!m_pkCurrentAnimation)
//		{
//			FV_ASSERT(m_eState == STATE_OFF);
//			m_u32CurrentPartition = 0;
//
//			return;
//		}
//
//		FvUInt32 u32NewPartition = 0;
//
//		for(FvUInt32 j(0); j < pkParent->GetMaskedPartitionNumber(); ++j)
//		{
//			if(pkParent->m_pkLists[j].GetHead()->m_Content == this)
//			{
//				FV_MASK_ADD(u32NewPartition, FV_MASK(j));
//			}		
//		}
//
//		if(u32NewPartition != m_u32CurrentPartition)
//		{
//			if(u32NewPartition == pkParent->m_spResource->m_u32AllMask)
//			{
//				u32NewPartition = FvAvatar::DO_NOT_USE;
//
//				pkParent->m_spTargetAvatar->SetPartition(pkParent->m_pkLevels[0].m_u32AnimationPosition, FvAvatar::DO_NOT_USE);
//				pkParent->m_spTargetAvatar->SetAnimation(pkParent->m_pkLevels[0].m_u32AnimationPosition, m_pkCurrentAnimation->m_i32Animation);
//
//				if(pkParent->m_pkLevels[0].m_u32AnimationPosition != m_u32AnimationPosition)
//					pkParent->m_spTargetAvatar->SetAnimation(m_u32AnimationPosition, FvAvatar::DO_NOT_USE);
//			}
//			else
//			{
//				if(u32NewPartition)
//				{
//					if(m_u32CurrentPartition == 0 && u32NewPartition != 0)
//					{
//						FvInt32 i32Animation = pkParent->m_spTargetAvatar->GetAnimation(m_u32AnimationPosition);
//
//						if(i32Animation >= 0)
//						{
//							FvString kAnimation = pkParent->m_spTargetAvatar->GetAnimationName(i32Animation);
//
//							FvUInt32 u32Level = pkParent->m_spResource->m_kAnimationMap.find(kAnimation)->second.m_u32Level;
//
//							if((u32Level != i) && (pkParent->m_pkLevels[u32Level].m_eState != Level::STATE_OFF))
//							{
//								FvUInt32 u32Pos = pkParent->m_pkLevels[u32Level].m_u32AnimationPosition;
//
//								pkParent->m_spTargetAvatar->SetPartition(u32Pos, pkParent->m_spTargetAvatar->GetPartition(m_u32AnimationPosition));
//								pkParent->m_spTargetAvatar->SetAnimation(u32Pos, pkParent->m_spTargetAvatar->GetAnimation(m_u32AnimationPosition));
//							}
//						}						
//
//						pkParent->m_spTargetAvatar->SetPartition(m_u32AnimationPosition, u32NewPartition);
//						pkParent->m_spTargetAvatar->SetAnimation(m_u32AnimationPosition, m_pkCurrentAnimation->m_i32Animation);
//					}
//					else if(m_u32CurrentPartition == FvAvatar::DO_NOT_USE)
//					{
//						pkParent->m_spTargetAvatar->SetPartition(m_u32AnimationPosition, u32NewPartition);
//						pkParent->m_spTargetAvatar->SetAnimation(m_u32AnimationPosition, m_pkCurrentAnimation->m_i32Animation);
//					}
//					else
//					{
//						pkParent->m_spTargetAvatar->SetPartition(m_u32AnimationPosition, u32NewPartition);
//					}
//				}
//				else if(i > 0)
//				{
//					pkParent->m_spTargetAvatar->SetPartition(m_u32AnimationPosition, u32NewPartition);
//					pkParent->m_spTargetAvatar->SetAnimation(m_u32AnimationPosition, m_pkCurrentAnimation->m_i32Animation);
//				}
//			}
//		}
//
//		m_u32CurrentPartition = u32NewPartition;
//
//		//PartitionPost(pkParent);
//		//pkParent->m_spTargetAvatar->SetPartition(m_u32AnimationPosition, m_u32CurrentPartition);
//
//		m_bDirtyPartition = false;
//	}
//}
//
////----------------------------------------------------------------------------
//FvUInt32* FvAnimationShell::Level::GetPriorities()
//{
//	return m_pkCurrentAnimation ? m_pkCurrentAnimation->GetPriorities() : NULL;
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::Level::PartitionPre(FvAnimationShell* pkParent)
//{
//	FV_ASSERT(pkParent);
//
//	for(FvUInt32 i(0); i < pkParent->GetMaskedPartitionNumber(); ++i)
//	{
//		bool bIsHead = pkParent->m_pkLists[i].GetHead() == &(m_pkLevelNodes[i]);
//
//		m_pkLevelNodes[i].Detach();
//
//		if(pkParent->m_pkLists[i].Size())
//		{
//			FvRefList<Level*>::iterator iter = pkParent->m_pkLists[i].GetHead();
//			while(true)
//			{
//				if(iter->m_Content->GetPriorities() ? iter->m_Content->GetPriorities()[i] >= GetPriorities()[i] : true)
//				{
//					if(iter == pkParent->m_pkLists[i].GetHead())
//					{
//						iter->m_Content->m_bDirtyPartition = true;
//					}
//
//					pkParent->m_pkLists[i].AttachBefore(*iter, m_pkLevelNodes[i]);
//					break;
//				}
//				else
//				{
//					if(bIsHead && iter == pkParent->m_pkLists[i].GetHead())
//					{
//						iter->m_Content->m_bDirtyPartition = true;
//					}
//
//					FvRefList<Level*>::Next(iter);
//
//					if(pkParent->m_pkLists[i].IsEnd(iter))
//					{
//						pkParent->m_pkLists[i].AttachBack(m_pkLevelNodes[i]);
//						break;
//					}
//				}
//			}
//		}
//		else
//		{
//			pkParent->m_pkLists[i].AttachBack(m_pkLevelNodes[i]);
//		}
//	}
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::Level::PartitionPost(FvAnimationShell* pkParent)
//{
//	FV_ASSERT(pkParent);
//
//	m_u32CurrentPartition = 0;
//
//	for(FvUInt32 i(0); i < pkParent->GetMaskedPartitionNumber(); ++i)
//	{
//		if(pkParent->m_pkLists[i].GetHead()->m_Content == this)
//		{
//			FV_MASK_ADD(m_u32CurrentPartition, FV_MASK(i));
//		}		
//	}
//
//	if(m_u32CurrentPartition == pkParent->m_spResource->m_u32AllMask)
//		m_u32CurrentPartition = FvAvatar::DO_NOT_USE;
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::Level::PartitionRemove(FvAnimationShell* pkParent)
//{
//	FV_ASSERT(pkParent);
//
//	for(FvUInt32 i(0); i < pkParent->GetMaskedPartitionNumber(); ++i)
//	{
//		bool bHead = &(m_pkLevelNodes[i]) == pkParent->m_pkLists[i].GetHead();
//
//		m_pkLevelNodes[i].Detach();
//
//		if(bHead && pkParent->m_pkLists[i].Size())
//		{
//			pkParent->m_pkLists[i].GetHead()->m_Content->m_bDirtyPartition = true;
//		}
//	}
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::Level::AnimationRemove(FvAnimationShell* pkParent)
//{
//	pkParent->RemoveAnimation(m_u32AnimationPosition);
//}
////----------------------------------------------------------------------------
//void FvAnimationShell::Level::EventCome(FvAvatar* pkAvatar,
//	FvInt32 i32Animation, FvAvatarAnimationState::EventType eType,
//	const char* pcMessage)
//{
//	if(m_pkCurrentAnimation)
//	{
//		FV_ASSERT(m_pkCurrentAnimation->m_i32Animation == i32Animation);
//		if(m_pkCurrentAnimation && m_eState == STATE_INSERT && eType == FvAvatarAnimationState::ANI_END)
//		{
//			if(m_pkCurrentAnimation->m_pkCallBack)
//				m_pkCurrentAnimation->m_pkCallBack(m_pkCurrentAnimation, AnimationHandle::INSERT_END);
//
//			if(!pkAvatar->IsLoop(i32Animation))
//			{
//				m_pkCurrentAnimation->SetTimePosition(0);
//
//				m_pkCurrentAnimation = NULL;
//			}		
//		}
//	}
//}
////----------------------------------------------------------------------------
