#include "FvAnimatedCharacter.h"
#include <FvDebug.h>
#include <OgreAnimationState.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreSubEntity.h>
#include <FvPowerDefines.h>
#include <OgreAnimation.h>
#include <OgreMeshManager.h>
#include <OgreMaterialManager.h>
#include <FvRenderManager.h>
#include <ParticleUniverseSystemManager.h>
#include <OgreTagPoint.h>

using namespace Ogre;

//----------------------------------------------------------------------------
FvAnimationState::FvAnimationState(FvAnimatedCharacterBase* pkParent,
	FvUInt32 u32Index) : m_u32Index(u32Index), m_pkParent(pkParent),
	m_ppkAnimationStates(new Ogre::AnimationState*[m_pkParent->m_u32BindingNumber]),
	m_bManualTime(false), m_eType(UNKNOWN), m_fTimePosition(0), m_fTimeScale(1.0f),
	m_pkCallBack(NULL), m_u32KeyPointer(0), m_bContinuous(false)
{
	const char* pcName = pkParent->GetAnimationName(m_u32Index);
	for(FvUInt32 i(0); i < m_pkParent->m_u32BindingNumber; ++i)
	{
		m_ppkAnimationStates[i] = m_pkParent->m_ppkAnimatedEntities[i]->getAnimationState(pcName);
		m_ppkAnimationStates[i]->SetTimePosition(&m_fTimePosition);
	}
}
//----------------------------------------------------------------------------
FvAnimationState::~FvAnimationState()
{
	delete [] m_ppkAnimationStates;
}
//----------------------------------------------------------------------------
void FvAnimationState::SetLoop(bool bLoop)
{
	for(FvUInt32 i(0); i < m_pkParent->m_u32BindingNumber; ++i)
	{
		m_ppkAnimationStates[i]->setLoop(bLoop);
	}
}
//----------------------------------------------------------------------------
bool FvAnimationState::GetLoop()
{
	return m_ppkAnimationStates[0]->getLoop();
}
//----------------------------------------------------------------------------
float FvAnimationState::GetLength()
{
	return m_ppkAnimationStates[0]->getLength();
}
//----------------------------------------------------------------------------
void FvAnimationState::SetManualTime(bool bManual)
{
	m_bManualTime = bManual;
}
//----------------------------------------------------------------------------
bool FvAnimationState::GetManualTime()
{
	return m_bManualTime;
}
//----------------------------------------------------------------------------
void FvAnimationState::SetBlendMask(FvUInt32 u32Binding, float* pfBlendMask)
{
	FV_ASSERT(u32Binding < m_pkParent->m_u32BindingNumber);
	m_ppkAnimationStates[u32Binding]->SetBlendMask(pfBlendMask);
}
//----------------------------------------------------------------------------
const float* FvAnimationState::GetBlendMask(FvUInt32 u32Binding) const
{
	FV_ASSERT(u32Binding < m_pkParent->m_u32BindingNumber);
	return m_ppkAnimationStates[u32Binding]->GetBlendMask();
}
//----------------------------------------------------------------------------
void FvAnimationState::SetWeightArray(float* pfArray)
{
	for(FvUInt32 i(0); i < m_pkParent->m_u32BindingNumber; ++i)
	{
		m_ppkAnimationStates[i]->SetWeightArray(pfArray);
	}
}
//----------------------------------------------------------------------------
float* FvAnimationState::GetWeightArray() const
{
	return m_ppkAnimationStates[0]->GetWeightArray();
}
//----------------------------------------------------------------------------
void FvAnimationState::SetExtraData(unsigned int* puiExtraData)
{
	for(FvUInt32 i(0); i < m_pkParent->m_u32BindingNumber; ++i)
	{
		m_ppkAnimationStates[i]->SetExtraData(puiExtraData);
	}
}
//----------------------------------------------------------------------------
unsigned int FvAnimationState::GetExtraData() const
{
	return m_ppkAnimationStates[0]->GetExtraData();
}
//----------------------------------------------------------------------------
bool FvAnimationState::AddTime(float fDeltaTime, bool bCallback)
{
	fDeltaTime *= m_fTimeScale;
	if(!m_bManualTime)
	{
		if(bCallback) return UpdateTimePosition(m_fTimePosition + fDeltaTime);
		else
		{
			SetTimePosition(m_fTimePosition + fDeltaTime);
			return false;
		}
	}
	else
	{
		return false;
	}
}
//----------------------------------------------------------------------------
float FvAnimationState::GetTimePosition()
{
	return m_fTimePosition;
}
//----------------------------------------------------------------------------
void FvAnimationState::SetTimePosition(float fTimePos)
{
	if (fTimePos != m_fTimePosition)
	{
		m_fTimePosition = fTimePos;
		if (GetLoop())
		{
			// Wrap
			m_fTimePosition = fmod(m_fTimePosition, GetLength());
			if(m_fTimePosition < 0)
				m_fTimePosition += GetLength();     
		}
		else
		{
			// Clamp
			if(m_fTimePosition < 0)
				m_fTimePosition = 0;
			else if (m_fTimePosition > GetLength())
				m_fTimePosition = GetLength();
		}

		m_ppkAnimationStates[m_pkParent->m_u32ActiveBinding]->_notifyDirty();
	}
	/*else
	{
		UpdateTimePosition(fTimePos);
	}*/
}
//----------------------------------------------------------------------------
void FvAnimationState::SetTimeScale(float fTimePos)
{
	m_fTimeScale = fTimePos;
}
//----------------------------------------------------------------------------
float FvAnimationState::GetTimeScale()
{
	return m_fTimeScale;
}
//----------------------------------------------------------------------------
bool FvAnimationState::UpdateTimePosition(float fTimePos)
{
	if(fTimePos > m_fTimePosition)
	{
		bool bRes = fTimePos > GetLength();

		float fLastTimePos = m_fTimePosition;

		m_fTimePosition = GetLoop() ? FvFmodf(fTimePos, GetLength()) : std::min(fTimePos, GetLength());

		if(m_pkCallBack)
		{
			FvAnimatedCharacterResource::Animation& kAnimation
				= m_pkParent->m_spResource->m_pkAnimations[m_u32Index];

			if(kAnimation.m_kKeySet.size())
			{
				FvAnimatedCharacterResource::AnimationKey* pkKeySet = &(kAnimation.m_kKeySet[0]);

				if(pkKeySet[m_u32KeyPointer].m_fTimePos <= fLastTimePos)
				{
					do
					{
						if((++m_u32KeyPointer) < kAnimation.m_kKeySet.size())
						{
							if(pkKeySet[m_u32KeyPointer].m_fTimePos <= fLastTimePos)
							{
								continue;
							}
							else
							{
								break;
							}
						}
						else
						{
							m_u32KeyPointer = 0;
							break;
						}
					} while(true);
				}
				else
				{	
					do 
					{
						if(m_u32KeyPointer)
						{
							if(pkKeySet[m_u32KeyPointer-1].m_fTimePos <= fLastTimePos)
							{
								break;
							}
							else
							{
								--m_u32KeyPointer;
								continue;
							}
						}
						else
						{
							break;
						}
					} while(true);
				}

				if(pkKeySet[m_u32KeyPointer].m_fTimePos > fLastTimePos)
				{
					do
					{
						if(pkKeySet[m_u32KeyPointer].m_fTimePos <= fTimePos)
						{
							m_pkCallBack->KeyEventCome(this, pkKeySet[m_u32KeyPointer].m_kName.c_str());
							++m_u32KeyPointer;

							if(m_u32KeyPointer < kAnimation.m_kKeySet.size())
							{
								continue;
							}
							else
							{
								m_u32KeyPointer = 0;
								break;;
							}
						}
						else
						{
							break;
						}
					} while(true);
				}

				
				if(bRes && GetLoop())
				{
					do 
					{
						if(pkKeySet[m_u32KeyPointer].m_fTimePos <= m_fTimePosition)
						{
							m_pkCallBack->KeyEventCome(this, pkKeySet[m_u32KeyPointer].m_kName.c_str());
							++m_u32KeyPointer;

							if(m_u32KeyPointer < kAnimation.m_kKeySet.size())
							{
								continue;
							}
							else
							{
								m_u32KeyPointer = 0;
								break;;
							}
						}
						else
						{
							break;
						}
					} while (true);
				}			
			}
		}
		
		m_ppkAnimationStates[m_pkParent->m_u32ActiveBinding]->_notifyDirty();
		//FV_TRACE_MSG("TimePos: %f\n", m_fTimePosition);

		return bRes && (!GetLoop()) && (!m_bContinuous);
	}
	else
	{
		return false;
	}
}
//----------------------------------------------------------------------------
void FvAnimationState::SetEnable(bool bEnable)
{
	for(FvUInt32 i(0); i < m_pkParent->m_u32BindingNumber; ++i)
	{
		m_ppkAnimationStates[i]->setEnabled(bEnable);
	}
}
//----------------------------------------------------------------------------
FvAnimationState::AnimationStateType FvAnimationState::GetType()
{
	return m_eType;
}
//----------------------------------------------------------------------------
FvAnimatedCharacterBase* FvAnimationState::GetParent()
{
	return m_pkParent;
}
//----------------------------------------------------------------------------
void FvAnimationState::SetCallback(Callback* pkCallback)
{
	m_pkCallBack = pkCallback;
}
//----------------------------------------------------------------------------
FvAnimationState::Callback* FvAnimationState::GetCallback()
{
	return m_pkCallBack;
}
//----------------------------------------------------------------------------
FvUInt32 FvAnimationState::GetIndex()
{
	return m_u32Index;
}
//----------------------------------------------------------------------------
const char* FvAnimationState::GetName()
{
	return m_pkParent->m_spResource->m_pkAnimations[m_u32Index].m_kName.c_str();
}
//----------------------------------------------------------------------------
const FvAnimatedCharacterResource::KeySet& FvAnimationState::GetKeySet()
{
	return m_pkParent->m_spResource->m_pkAnimations[m_u32Index].m_kKeySet;
}
//----------------------------------------------------------------------------
void FvAnimationState::EnableBlendMask(const char* pcName)
{
	FvStringMap<FvUInt32>::iterator it = m_pkParent->m_spResource->m_kBlendMaskIndexes.find(pcName);

	if(it != m_pkParent->m_spResource->m_kBlendMaskIndexes.end())
	{
		//FvUInt32 u32Index = it->second * m_pkParent->m_spResource->
		for(FvUInt32 i(0); i < m_pkParent->m_u32BindingNumber; ++i)
		{
			FvAnimatedCharacterResource::SkeletonInfo& kInfo = m_pkParent->m_spResource->m_kSkeletonInfo[i];
			SetBlendMask(i, &(kInfo.m_pfBlendMaskBuffer[it->second * kInfo.m_u32BoneNumber]));
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimationState::DisableBlendMask()
{
	for(FvUInt32 i(0); i < m_pkParent->m_u32BindingNumber; ++i)
	{
		SetBlendMask(i, NULL);
	}
}
//----------------------------------------------------------------------------
void FvAnimationState::SetContinuous(bool bEnable)
{
	m_bContinuous = bEnable;
}
//----------------------------------------------------------------------------
bool FvAnimationState::GetContinuous()
{
	return m_bContinuous;
}
//----------------------------------------------------------------------------
FvPartitionAnimationState::FvPartitionAnimationState(
	FvAnimatedCharacterBase* pkParent, FvUInt32 u32Index)
	: FvAnimationState(pkParent, u32Index),
	m_u32PartitionMask(0), m_pu32Priority(NULL), m_bIsOnPartition(false)
{
	FV_ASSERT(m_pkParent->m_u32PartitionNumber);
	m_kNode.m_Content = this;
	m_eType = PARTITION;
	m_pfWeights = new float[m_pkParent->m_u32PartitionNumber];
	m_pkPartitionState = new PartitionData[m_pkParent->m_u32PartitionNumber];

	for(FvUInt32 i(0); i < m_pkParent->m_u32PartitionNumber; ++i)
	{
		m_pfWeights[i] = 0.0f;
		m_pkPartitionState[i].m_pkParent = this;
		m_pkPartitionState[i].m_fBlendWeight = 0.0f;
	}

	SetWeightArray(m_pfWeights);
	SetExtraData(&m_u32PartitionMask);
}
//----------------------------------------------------------------------------
FvPartitionAnimationState::~FvPartitionAnimationState()
{
	delete [] m_pfWeights;
	delete [] m_pkPartitionState;
}
//----------------------------------------------------------------------------
void FvPartitionAnimationState::SetPriority(FvUInt32* pu32Priority)
{
	m_pu32Priority = pu32Priority;
}
//----------------------------------------------------------------------------
void FvPartitionAnimationState::AfterPartitionMaskUpdate()
{
	if((!m_u32PartitionMask) && (!m_bIsOnPartition))
	{
		SetEnable(false);
		SetTimePosition(0);
		m_kNode.Detach();
	}
}
//----------------------------------------------------------------------------
void FvPartitionAnimationState::Play(bool bBlend)
{
	if(!m_bIsOnPartition)
	{
		FV_ASSERT(m_pu32Priority);
		SetEnable(true);
		m_pkParent->m_kActivePartitionAnimations.AttachBack(m_kNode);
		m_bIsOnPartition = true;

		FvAnimatedCharacterBase::PartitionStack* pkPartition = m_pkParent->m_pkPartitionAnimations;

		for(FvUInt32 i(0); i < m_pkParent->m_u32PartitionNumber; ++i, ++pkPartition)
		{
			const FvUInt32 u32Mask = FV_MASK(i);
			FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Iterator iter = pkPartition->m_kPartitionAnimations.GetHead();
			if(pkPartition->m_kPartitionAnimations.IsEnd(iter))
			{
				pkPartition->m_kPartitionAnimations.PushFront(m_pkPartitionState[i].m_kPartitionNode);
				FV_MASK_ADD(m_u32PartitionMask, u32Mask);
				m_pkPartitionState[i].m_kDisableNode.Detach();

				float fBlendTimeCopy = m_pkParent->GetBlendTime(FvAnimatedCharacterBase::NONE_ANIMATION, m_u32Index);

				if(bBlend && (fBlendTimeCopy > 0))
				{
					pkPartition->m_fBlendTime = fBlendTimeCopy;
					pkPartition->m_fInvBlendTime = 1.0f / pkPartition->m_fBlendTime;
					pkPartition->m_fPassedBlendTime = m_pfWeights[i] * pkPartition->m_fBlendTime;						

					{
						FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Iterator it = pkPartition->m_kDisableAnimations.GetHead();
						while (!pkPartition->m_kDisableAnimations.IsEnd(it))
						{
							PartitionData& kData = PartitionData::GetFromDisableNode(it);
							FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Next(it);
							kData.m_fBlendWeight = kData.m_pkParent->m_pfWeights[i];
						}
					}
				}
				else
				{
					pkPartition->m_fBlendTime = 0;
					pkPartition->m_fPassedBlendTime = 0;

					m_pfWeights[i] = 1.0f;

					{
						FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Iterator it = pkPartition->m_kDisableAnimations.GetHead();
						while (!pkPartition->m_kDisableAnimations.IsEnd(it))
						{
							PartitionData& kData = PartitionData::GetFromDisableNode(it);
							FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Next(it);
							kData.m_pkParent->m_pfWeights[i] = 0;
							FV_MASK_DEL(kData.m_pkParent->m_u32PartitionMask, u32Mask);
							kData.m_pkParent->AfterPartitionMaskUpdate();
						}
						pkPartition->m_kDisableAnimations.Clear();
					}
				}
			}
			else
			{
				PartitionData& kData = PartitionData::GetFromPartitionNode(iter);
				if(m_pu32Priority[i] <= kData.m_pkParent->m_pu32Priority[i])
				{
					pkPartition->m_kPartitionAnimations.PushFront(m_pkPartitionState[i].m_kPartitionNode);
					FV_MASK_ADD(m_u32PartitionMask, u32Mask);
					m_pkPartitionState[i].m_kDisableNode.Detach();

					float fBlendTimeCopy = m_pkParent->GetBlendTime(kData.m_pkParent->m_u32Index, m_u32Index);

					if(bBlend && (fBlendTimeCopy > 0))
					{
						pkPartition->m_fBlendTime = fBlendTimeCopy;
						pkPartition->m_fInvBlendTime = 1.0f / pkPartition->m_fBlendTime;
						pkPartition->m_fPassedBlendTime = m_pfWeights[i] * pkPartition->m_fBlendTime;

						pkPartition->m_kDisableAnimations.PushBack(kData.m_kDisableNode);

						{
							FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Iterator it = pkPartition->m_kDisableAnimations.GetHead();
							while (!pkPartition->m_kDisableAnimations.IsEnd(it))
							{
								PartitionData& kData = PartitionData::GetFromDisableNode(it);
								FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Next(it);
								kData.m_fBlendWeight = kData.m_pkParent->m_pfWeights[i];
							}
						}
					}
					else
					{
						pkPartition->m_fBlendTime = 0;
						pkPartition->m_fPassedBlendTime = 0;

						m_pfWeights[i] = 1.0f;

						pkPartition->m_kDisableAnimations.PushBack(kData.m_kDisableNode);

						{
							FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Iterator it = pkPartition->m_kDisableAnimations.GetHead();
							while (!pkPartition->m_kDisableAnimations.IsEnd(it))
							{
								PartitionData& kData = PartitionData::GetFromDisableNode(it);
								FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Next(it);
								kData.m_pkParent->m_pfWeights[i] = 0;
								FV_MASK_DEL(kData.m_pkParent->m_u32PartitionMask, u32Mask);
								kData.m_pkParent->AfterPartitionMaskUpdate();
							}
							pkPartition->m_kDisableAnimations.Clear();
						}
					}
				}
				else
				{
					bool bNeedPushBack = true;

					FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Next(iter);
					while (!pkPartition->m_kPartitionAnimations.IsEnd(iter))
					{
						PartitionData& kData = PartitionData::GetFromPartitionNode(iter);
						FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Next(iter);

						if(m_pu32Priority[i] <= kData.m_pkParent->m_pu32Priority[i])
						{
							FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::PushBefore(kData.m_kPartitionNode, m_pkPartitionState[i].m_kPartitionNode);
							bNeedPushBack = false;
							break;
						}
					}

					if(bNeedPushBack)
					{
						pkPartition->m_kPartitionAnimations.PushBack(m_pkPartitionState[i].m_kPartitionNode);
					}
				}
			}
		}
	}
	
}
//----------------------------------------------------------------------------
void FvPartitionAnimationState::Stop(bool bBlend)
{
	if(m_bIsOnPartition)
	{
		m_bIsOnPartition = false;

		FvAnimatedCharacterBase::PartitionStack* pkPartition = m_pkParent->m_pkPartitionAnimations;

		for(FvUInt32 i(0); i < m_pkParent->m_u32PartitionNumber; ++i, ++pkPartition)	
		{
			const FvUInt32 u32Mask = FV_MASK(i);

			FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Iterator iter = pkPartition->m_kPartitionAnimations.GetHead();

			if(&(m_pkPartitionState[i].m_kPartitionNode) == iter)
			{
				m_pkPartitionState[i].m_kPartitionNode.Detach();
				
				iter = pkPartition->m_kPartitionAnimations.GetHead();
				if(pkPartition->m_kPartitionAnimations.IsEnd(iter))
				{
					float fBlendTimeCopy = m_pkParent->GetBlendTime(m_u32Index, FvAnimatedCharacterBase::NONE_ANIMATION);
					if(bBlend && (fBlendTimeCopy > 0))
					{
						pkPartition->m_fBlendTime = fBlendTimeCopy;
						pkPartition->m_fInvBlendTime = 1.0f / pkPartition->m_fBlendTime;
						pkPartition->m_fPassedBlendTime = 0;

						pkPartition->m_kDisableAnimations.PushBack(m_pkPartitionState[i].m_kDisableNode);

						{
							FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Iterator it = pkPartition->m_kDisableAnimations.GetHead();
							while (!pkPartition->m_kDisableAnimations.IsEnd(it))
							{
								PartitionData& kDisableData = PartitionData::GetFromDisableNode(it);
								FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Next(it);
								kDisableData.m_fBlendWeight = kDisableData.m_pkParent->m_pfWeights[i];
							}
						}
					}
					else
					{
						m_pfWeights[i] = 0;
						FV_MASK_DEL(m_u32PartitionMask, u32Mask);

						FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Iterator it = pkPartition->m_kDisableAnimations.GetHead();
						while (!pkPartition->m_kDisableAnimations.IsEnd(it))
						{
							PartitionData& kDisableData = PartitionData::GetFromDisableNode(it);
							FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Next(it);
							kDisableData.m_pkParent->m_pfWeights[i] = 0;
							FV_MASK_DEL(kDisableData.m_pkParent->m_u32PartitionMask, u32Mask);
							kDisableData.m_pkParent->AfterPartitionMaskUpdate();
						}
						pkPartition->m_kDisableAnimations.Clear();
						m_kNode.Detach();
					}
				}
				else
				{
					PartitionData& kData = PartitionData::GetFromPartitionNode(iter);
					FV_MASK_ADD(kData.m_pkParent->m_u32PartitionMask, u32Mask);
					kData.m_kDisableNode.Detach();

					float fBlendTimeCopy = m_pkParent->GetBlendTime(m_u32Index, kData.m_pkParent->m_u32Index);
					if(bBlend && (fBlendTimeCopy > 0))
					{
						pkPartition->m_fBlendTime = fBlendTimeCopy;
						pkPartition->m_fInvBlendTime = 1.0f / pkPartition->m_fBlendTime;
						pkPartition->m_fPassedBlendTime = kData.m_pkParent->m_pfWeights[i] * pkPartition->m_fBlendTime;

						pkPartition->m_kDisableAnimations.PushBack(m_pkPartitionState[i].m_kDisableNode);

						{
							FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Iterator it = pkPartition->m_kDisableAnimations.GetHead();
							while (!pkPartition->m_kDisableAnimations.IsEnd(it))
							{
								PartitionData& kDisableData = PartitionData::GetFromDisableNode(it);
								FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Next(it);
								kDisableData.m_fBlendWeight = kDisableData.m_pkParent->m_pfWeights[i];
							}
						}
					}
					else
					{
						m_pfWeights[i] = 0;
						FV_MASK_DEL(m_u32PartitionMask, u32Mask);
						kData.m_pkParent->m_pfWeights[i] = 1.0;

						FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Iterator it = pkPartition->m_kDisableAnimations.GetHead();
						while (!pkPartition->m_kDisableAnimations.IsEnd(it))
						{
							PartitionData& kDisableData = PartitionData::GetFromDisableNode(it);
							FvDoubleLink<PartitionData*, FvDoubleLinkNode1>::Next(it);
							kDisableData.m_pkParent->m_pfWeights[i] = 0;
							FV_MASK_DEL(kDisableData.m_pkParent->m_u32PartitionMask, u32Mask);
							kDisableData.m_pkParent->AfterPartitionMaskUpdate();
						}
						pkPartition->m_kDisableAnimations.Clear();
						m_kNode.Detach();
					}
				}				
			}
			else
			{
				m_pkPartitionState[i].m_kPartitionNode.Detach();
			}
		}

		AfterPartitionMaskUpdate();
	}
}
//----------------------------------------------------------------------------
void FvPartitionAnimationState::UpdateFromBonePartition()
{
	for(FvUInt32 i(0); i < m_pkParent->m_u32BindingNumber; ++i)
	{
		m_ppkAnimationStates[i]->GetTargetAnimation()->UpdateTrackList(m_pkParent->m_spResource->GetBonesPartTable(i));
	}
}
//----------------------------------------------------------------------------
#define OFFSET(CLASS, VAR) ((FvUInt32)(&(((CLASS*)(NULL))->VAR)))
//----------------------------------------------------------------------------
FvPartitionAnimationState::PartitionData&
FvPartitionAnimationState::PartitionData::GetFromPartitionNode(
	FvDoubleLinkNode1<PartitionData*>* pkPartitionNode)
{
	return *(PartitionData*)(((FvUInt32)pkPartitionNode) - OFFSET(PartitionData, m_kPartitionNode));
}
//----------------------------------------------------------------------------
FvPartitionAnimationState::PartitionData&
FvPartitionAnimationState::PartitionData::GetFromDisableNode(
	FvDoubleLinkNode1<PartitionData*>* pkDisableNode)
{
	return *(PartitionData*)(((FvUInt32)pkDisableNode) - OFFSET(PartitionData, m_kDisableNode));
}
//----------------------------------------------------------------------------
float FvAnimatedCharacterBase::ms_fDefaultBlendTime = 0.2f;
//----------------------------------------------------------------------------
void AddSubMeshToBoneSwitchTable(FvUInt8* pu8BoneSwitchTable,
	const Ogre::SubMesh* pkSubMesh)
{
	if(!(pkSubMesh->useSharedVertices))
	{
		const Mesh::IndexMap& indexMap = pkSubMesh->blendIndexToBoneIndexMap;
		const SkeletonPtr& spSkeleton = pkSubMesh->parent->getSkeleton();

		std::set<unsigned short> kIndexSet;

		for(FvUInt32 i(0); i < indexMap.size(); ++i)
		{
			unsigned short usIndex = indexMap[i];
			while(true)
			{
				kIndexSet.insert(usIndex);

				Bone* pkParent = static_cast<Bone*>(spSkeleton->getBone(usIndex)->getParent());

				if(pkParent)
				{
					usIndex = pkParent->getHandle();
					continue;
				}
				break;
			}
		}

		for(std::set<unsigned short>::iterator it = kIndexSet.begin();
			it != kIndexSet.end(); ++it)
		{
			unsigned short usIndex = *it;

			if(pu8BoneSwitchTable[usIndex] < 0xff)
			{
				++(pu8BoneSwitchTable[usIndex]);
			}
			else
			{
				FV_CRITICAL_MSG("BoneSwitchTable error!\n");
			}
		}
	}
}
//----------------------------------------------------------------------------
void DelMeshFromBoneSwitchTable(FvUInt8* pu8BoneSwitchTable,
	const Ogre::SubMesh* pkSubMesh)
{
	if(!(pkSubMesh->useSharedVertices))
	{
		const Mesh::IndexMap& indexMap = pkSubMesh->blendIndexToBoneIndexMap;
		const SkeletonPtr& spSkeleton = pkSubMesh->parent->getSkeleton();

		std::set<unsigned short> kIndexSet;

		for(FvUInt32 i(0); i < indexMap.size(); ++i)
		{
			unsigned short usIndex = indexMap[i];
			while(true)
			{
				kIndexSet.insert(usIndex);

				Bone* pkParent = static_cast<Bone*>(spSkeleton->getBone(usIndex)->getParent());

				if(pkParent)
				{
					usIndex = pkParent->getHandle();
					continue;
				}
				break;
			}
		}

		for(std::set<unsigned short>::iterator it = kIndexSet.begin();
			it != kIndexSet.end(); ++it)
		{
			unsigned short usIndex = *it;

			if(pu8BoneSwitchTable[usIndex])
			{
				--(pu8BoneSwitchTable[usIndex]);
			}
			else
			{
				FV_CRITICAL_MSG("BoneSwitchTable error!\n");
			}
		}
	}
}
//----------------------------------------------------------------------------
void AddMeshToBoneSwitchTable(FvUInt8* pu8BoneSwitchTable,
	const Ogre::MeshPtr& spMesh)
{
	const Mesh::IndexMap& indexMap = spMesh->sharedBlendIndexToBoneIndexMap;
	const SkeletonPtr& spSkeleton = spMesh->getSkeleton();

	std::set<unsigned short> kIndexSet;

	for(FvUInt32 i(0); i < indexMap.size(); ++i)
	{
		unsigned short usIndex = indexMap[i];
		while(true)
		{
			kIndexSet.insert(usIndex);

			Bone* pkParent = static_cast<Bone*>(spSkeleton->getBone(usIndex)->getParent());

			if(pkParent)
			{
				usIndex = pkParent->getHandle();
				continue;
			}
			break;
		}
	}

	for(std::set<unsigned short>::iterator it = kIndexSet.begin();
		it != kIndexSet.end(); ++it)
	{
		unsigned short usIndex = *it;

		if(pu8BoneSwitchTable[usIndex] < 0xff)
		{
			++(pu8BoneSwitchTable[usIndex]);
		}
		else
		{
			FV_CRITICAL_MSG("BoneSwitchTable error!\n");
		}
	}

	for(FvUInt32 i(0); i < spMesh->getNumSubMeshes(); ++i)
	{
		AddSubMeshToBoneSwitchTable(pu8BoneSwitchTable, spMesh->getSubMesh(i));
	}
}
//----------------------------------------------------------------------------
void DelMeshFromBoneSwitchTable(FvUInt8* pu8BoneSwitchTable,
	const Ogre::MeshPtr& spMesh)
{
	for(FvUInt32 i(0); i < spMesh->getNumSubMeshes(); ++i)
	{
		DelMeshFromBoneSwitchTable(pu8BoneSwitchTable, spMesh->getSubMesh(i));
	}

	const Mesh::IndexMap& indexMap = spMesh->sharedBlendIndexToBoneIndexMap;
	const SkeletonPtr& spSkeleton = spMesh->getSkeleton();

	std::set<unsigned short> kIndexSet;

	for(FvUInt32 i(0); i < indexMap.size(); ++i)
	{
		unsigned short usIndex = indexMap[i];
		while(true)
		{
			kIndexSet.insert(usIndex);

			Bone* pkParent = static_cast<Bone*>(spSkeleton->getBone(usIndex)->getParent());

			if(pkParent)
			{
				usIndex = pkParent->getHandle();
				continue;
			}
			break;
		}
	}

	for(std::set<unsigned short>::iterator it = kIndexSet.begin();
		it != kIndexSet.end(); ++it)
	{
		unsigned short usIndex = *it;

		if(pu8BoneSwitchTable[usIndex])
		{
			--(pu8BoneSwitchTable[usIndex]);
		}
		else
		{
			FV_CRITICAL_MSG("BoneSwitchTable error!\n");
		}
	}
}
//----------------------------------------------------------------------------
FvRefList<FvAnimatedCharacterBase*> FvAnimatedCharacterBase::m_kCacheList;
//----------------------------------------------------------------------------
FvAnimatedCharacterBase::FvAnimatedCharacterBase(
	const FvAnimatedCharacterResourcePtr& rspResource, const char* pcName)
	: m_kName(pcName), m_spResource(rspResource),
	m_u32BindingNumber(rspResource->m_kSkeletonInfo.size()), m_u32ActiveBinding(0),
	m_u32PartitionNumber(rspResource->m_kSkeletonInfo[0].m_pu8PartitionBuffer[0]),
	m_pkGlobalParts(NULL), m_ppkParentTagPoints(NULL)
{
	m_kNodeForCache.m_Content = this;

	FV_ASSERT(m_u32BindingNumber <= FV_MAX_BINDING);
	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);
	FV_ASSERT(m_spResource.getPointer());
	FV_ASSERT(m_spResource->isLoaded());

	char acBuffer[256];

	m_ppkAnimatedEntities = new FvAnimatedEntity*[m_u32BindingNumber];
	m_ppu8BoneSwitchTables = new FvUInt8*[m_u32BindingNumber];
	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
	{
		FvAnimatedCharacterResource::SkeletonInfo& kInfo = m_spResource->m_kSkeletonInfo[i];
		NameValuePairList kParams;
		kParams["mesh"] = kInfo.m_spMainMesh->getName();
		sprintf_s(acBuffer, "%s_%d", pcName, i);
		m_ppkAnimatedEntities[i] = (FvAnimatedEntity*)(pkSceneManager->createMovableObject(acBuffer, "AnimatedEntity", &kParams));
		m_ppkAnimatedEntities[i]->m_pkParent = this;
		
		{
			m_ppkAnimatedEntities[i]->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAX);
			for(FvUInt32 s(0); s < m_ppkAnimatedEntities[i]->getNumSubEntities(); ++s)
			{
				m_ppkAnimatedEntities[i]->getSubEntity(s)->setCustomParameter(0, Ogre::Vector4(0.5f, 0.5f, 0.5f, 0.5f));
			}
		}

		for(FvUInt32 j(0); j < m_ppkAnimatedEntities[i]->getNumSubEntities(); ++j)
		{
			if(j < kInfo.m_kMainMaterials.size())
			{
				m_ppkAnimatedEntities[i]->getSubEntity(j)->setMaterial(kInfo.m_kMainMaterials[j]);
			}
			else
			{
				m_ppkAnimatedEntities[i]->getSubEntity(j)->setMaterialName("BaseWhiteNoLighting");
			}
		}

		m_ppkAnimatedEntities[i]->SetShow(kInfo.m_bShowMainMesh);

		m_ppu8BoneSwitchTables[i] = new FvUInt8[kInfo.m_u32BoneNumber];
		FvZeroMemory(m_ppu8BoneSwitchTables[i], kInfo.m_u32BoneNumber);

 		if(kInfo.m_bShowMainMesh)
 		{
 			AddMeshToBoneSwitchTable(m_ppu8BoneSwitchTables[i], m_ppkAnimatedEntities[i]->getMesh());
 		}

		m_ppkAnimatedEntities[i]->SetBonesSwitchTable(m_ppu8BoneSwitchTables[i]);
	}

	FvAnimatedCharacterResource::BoundingBoxParams* pkAABB = m_spResource->m_pkBoundingBoxParams;

	if(pkAABB)
	{
		for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
		{
			FvAnimatedCharacterResource::SkeletonInfo& kInfo = m_spResource->m_kSkeletonInfo[i];
			FvUInt16 u16Target = kInfo.m_spSkeleton->getBone(pkAABB->m_kTailPointName)->getHandle();
			
			m_ppkAnimatedEntities[i]->SetBoundingBoxTargetBone(u16Target, *(Ogre::Vector3*)&pkAABB->m_kMax, *(Ogre::Vector3*)&pkAABB->m_kMin);
		}
	}

	m_pkPartitionAnimations = new PartitionStack[m_u32PartitionNumber];
	m_ppkAnimationStates = new FvAnimationState*[m_spResource->GetNumAnimation()];
	FvZeroMemoryEx(m_ppkAnimationStates, sizeof(FvAnimationState*), m_spResource->GetNumAnimation());

	if(m_spResource->m_u32PartNumber)
	{
		m_pkGlobalParts = new FvRefList<FvAnimatedCharacterNodePrivateData::PartEntity*>[m_spResource->m_u32PartNumber];
	}
}
//----------------------------------------------------------------------------
FvAnimatedCharacterBase::~FvAnimatedCharacterBase()
{
	for(BindingProviderMap::iterator it = m_kProviderMap.begin();
		it != m_kProviderMap.end(); ++it)
	{
		FV_ASSERT(it->second);
		for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
		{
			m_ppkAnimatedEntities[i]->FreeTagPoint(it->second->m_apkNodes[i]);
		}
		delete it->second;
	}
	m_kProviderMap.clear();

	for(BindingProviderMap::iterator it = m_kLocalProviderMap.begin();
		it != m_kLocalProviderMap.end(); ++it)
	{
		FV_ASSERT(it->second);
		for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
		{
			m_ppkAnimatedEntities[i]->FreeTagPoint(it->second->m_apkNodes[i]);
		}
		delete it->second;
	}
	m_kLocalProviderMap.clear();

	for(FvUInt32 i(0); i < 3; ++i)
	{
		FvRefListNameSpace<FvAnimatedCharacterNode>::List1& kList = (&m_kNodesWaitLoad)[i];

		kList.BeginIterator();
		while(!kList.IsEnd())
		{
			FvRefListNameSpace<FvAnimatedCharacterNode>::List1::iterator iter = kList.GetIterator();
			FvAnimatedCharacterNode* pkNode = static_cast<FvAnimatedCharacterNode*>(iter);
			kList.Next();
			//
			PutOff(*pkNode);
		}
	}

	FV_SAFE_DELETE_ARRAY(m_pkGlobalParts);

	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);

	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
	{
		pkSceneManager->destroyMovableObject(m_ppkAnimatedEntities[i]);
		delete [] m_ppu8BoneSwitchTables[i];
	}

	delete [] m_ppkAnimatedEntities;

	delete [] m_ppu8BoneSwitchTables;

	delete [] m_pkPartitionAnimations;

	for(FvUInt32 i(0); i < m_spResource->GetNumAnimation(); ++i)
	{
		FV_SAFE_DELETE(m_ppkAnimationStates[i]);
	}

	delete [] m_ppkAnimationStates;
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::AttachToNode(Ogre::SceneNode* pkNode)
{
	pkNode->attachObject(m_ppkAnimatedEntities[m_u32ActiveBinding]);
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::AttachCharacter(const char* pcTagPoint,
	FvVector3& kScale, FvAnimatedCharacterBase* pkCharacter)
{
	FV_ASSERT(pkCharacter);
	FV_ASSERT(!pkCharacter->m_ppkParentTagPoints);
	
	SceneNode* pkNode = pkCharacter->m_ppkAnimatedEntities[pkCharacter->m_u32ActiveBinding]->getParentSceneNode();
	if(pkNode)
	{
		pkNode->detachObject(pkCharacter->m_ppkAnimatedEntities[pkCharacter->m_u32ActiveBinding]);
	}

	FV_ASSERT(!pkCharacter->m_ppkParentTagPoints);
	
	pkCharacter->m_ppkParentTagPoints = new Ogre::TagPoint*[FV_MAX_BINDING];
	FvZeroMemoryEx(pkCharacter->m_ppkParentTagPoints, sizeof(Ogre::TagPoint*), FV_MAX_BINDING);

	FvAnimatedCharacterResource::AliasTagPoint::iterator iter = m_spResource->m_kBoneAlias.find(pcTagPoint);
	FV_ASSERT(iter != m_spResource->m_kBoneAlias.end());
	FvAnimatedCharacterResource::Offset& kOffset = iter->second;
	
	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
	{
		pkCharacter->m_ppkParentTagPoints[i] = m_ppkAnimatedEntities[i]->RefTagPoint(pcTagPoint, m_spResource->m_kBoneAlias);

		pkCharacter->m_ppkParentTagPoints[i]->setScale(kScale.x, kScale.y, kScale.z);
		pkCharacter->m_ppkParentTagPoints[i]->setPosition(kOffset.m_kPosition.x * kScale.x,kOffset.m_kPosition.y * kScale.y,kOffset.m_kPosition.z * kScale.z);

		m_ppkAnimatedEntities[i]->LinkObject(pkCharacter->m_ppkAnimatedEntities[((i < pkCharacter->m_u32BindingNumber) ? i : (pkCharacter->m_u32BindingNumber - 1))]);
	}

	pkCharacter->m_u32ActiveBinding = ((m_u32ActiveBinding < pkCharacter->m_u32BindingNumber) ? m_u32ActiveBinding : (pkCharacter->m_u32BindingNumber - 1));
	pkCharacter->m_ppkAnimatedEntities[pkCharacter->m_u32ActiveBinding]->_notifyAttached(pkCharacter->m_ppkParentTagPoints[m_u32ActiveBinding], true);
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::DetachCharacter(
	FvAnimatedCharacterBase* pkCharacter)
{
	FV_ASSERT(pkCharacter);
	FV_ASSERT(pkCharacter->m_ppkParentTagPoints);
	
	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
	{
		m_ppkAnimatedEntities[i]->FreeTagPoint(pkCharacter->m_ppkParentTagPoints[i]);

		m_ppkAnimatedEntities[i]->UnlinkObject(pkCharacter->m_ppkAnimatedEntities[((i < pkCharacter->m_u32BindingNumber) ? i : (pkCharacter->m_u32BindingNumber - 1))]);
	}

	for(FvUInt32 i(0); i < pkCharacter->m_u32BindingNumber; ++i)
	{
		pkCharacter->m_ppkAnimatedEntities[i]->_notifyAttached(NULL);
	}

	FV_SAFE_DELETE_ARRAY(pkCharacter->m_ppkParentTagPoints);
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::Active(FvUInt32 u32Binding)
{
	FvUInt32 u32BindingCopy = u32Binding;
	if(m_u32ActiveBinding != u32Binding)
	{
		if(u32Binding >= m_u32BindingNumber) u32Binding = m_u32BindingNumber - 1;

		if(m_ppkParentTagPoints)
		{
			m_ppkAnimatedEntities[u32Binding]->_notifyAttached(
				m_ppkParentTagPoints[u32BindingCopy]);
		}
		else
		{
			SceneNode* pkNode = m_ppkAnimatedEntities[m_u32ActiveBinding]->getParentSceneNode();
			if(pkNode)
			{
				pkNode->detachObject(m_ppkAnimatedEntities[m_u32ActiveBinding]);
				pkNode->attachObject(m_ppkAnimatedEntities[u32Binding]);
			}
		}

		m_u32ActiveBinding = u32Binding;

		for(BindingProviderMap::iterator it = m_kProviderMap.begin();
			it != m_kProviderMap.end(); ++it)
		{
			FV_ASSERT(it->second);
			it->second->UpdateBinding(m_u32ActiveBinding);
		}

		for(BindingProviderMap::iterator it = m_kLocalProviderMap.begin();
			it != m_kLocalProviderMap.end(); ++it)
		{
			FV_ASSERT(it->second);
			it->second->LocalUpdateBinding(m_u32ActiveBinding);
		}
	}
}
//----------------------------------------------------------------------------
FvUInt32 FvAnimatedCharacterBase::Active()
{
	return m_u32ActiveBinding;
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::AddTime(float fDeltaTime)
{
	AddBlendTime(fDeltaTime);
	AddPositionTime(fDeltaTime);

	m_kExtraAnimationEnableList.BeginIterator();
	while (!m_kExtraAnimationEnableList.IsEnd())
	{
		FvRefList<FvExtraAnimationState*>::iterator iter = m_kExtraAnimationEnableList.GetIterator();
		FvExtraAnimationState* obj = (*iter).m_Content;
		m_kExtraAnimationEnableList.Next();
		
		obj->AddBlend(fDeltaTime);
		obj->AddTime(fDeltaTime, true);
	}

	m_kExtraAnimationDisableList.BeginIterator();
	while (!m_kExtraAnimationDisableList.IsEnd())
	{
		FvRefList<FvExtraAnimationState*>::iterator iter = m_kExtraAnimationDisableList.GetIterator();
		FvExtraAnimationState* obj = (*iter).m_Content;
		m_kExtraAnimationDisableList.Next();

		obj->AddBlend(fDeltaTime);
		obj->AddTime(fDeltaTime, false);
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::AddBlendTime(float fDeltaTime)
{
	PartitionStack* pkPartition = m_pkPartitionAnimations;
	for(FvUInt32 i(0); i < m_u32PartitionNumber; ++i, ++pkPartition)
	{
		if(pkPartition->m_fPassedBlendTime < pkPartition->m_fBlendTime)
		{
			pkPartition->m_fPassedBlendTime += fDeltaTime;

			if(pkPartition->m_fPassedBlendTime < pkPartition->m_fBlendTime)
			{
				float fWeight = pkPartition->m_fPassedBlendTime * pkPartition->m_fInvBlendTime;

				if(pkPartition->m_kPartitionAnimations.IsNotEmpty())
				{
					FvPartitionAnimationState::PartitionData& kData = FvPartitionAnimationState::PartitionData::GetFromPartitionNode(pkPartition->m_kPartitionAnimations.GetHead());
					kData.m_pkParent->m_pfWeights[i] = fWeight;
				}

				fWeight = 1 - fWeight;

				FvDoubleLink<FvPartitionAnimationState::PartitionData*, FvDoubleLinkNode1>::Iterator iter = pkPartition->m_kDisableAnimations.GetHead();
				while (!pkPartition->m_kDisableAnimations.IsEnd(iter))
				{
					FvPartitionAnimationState::PartitionData& kData = FvPartitionAnimationState::PartitionData::GetFromDisableNode(iter);
					FvDoubleLink<FvPartitionAnimationState::PartitionData*, FvDoubleLinkNode1>::Next(iter);
					kData.m_pkParent->m_pfWeights[i] = fWeight * kData.m_fBlendWeight;
				}
			}
			else
			{
				if(pkPartition->m_kPartitionAnimations.IsNotEmpty())
				{
					FvPartitionAnimationState::PartitionData& kData = FvPartitionAnimationState::PartitionData::GetFromPartitionNode(pkPartition->m_kPartitionAnimations.GetHead());
					kData.m_pkParent->m_pfWeights[i] = 1.0f;
				}

				const FvUInt32 u32Mask = FV_MASK(i);

				FvDoubleLink<FvPartitionAnimationState::PartitionData*, FvDoubleLinkNode1>::Iterator iter = pkPartition->m_kDisableAnimations.GetHead();
				while (!pkPartition->m_kDisableAnimations.IsEnd(iter))
				{
					FvPartitionAnimationState::PartitionData& kData = FvPartitionAnimationState::PartitionData::GetFromDisableNode(iter);
					FvDoubleLink<FvPartitionAnimationState::PartitionData*, FvDoubleLinkNode1>::Next(iter);
					kData.m_pkParent->m_pfWeights[i] = 0;
					FV_MASK_DEL(kData.m_pkParent->m_u32PartitionMask, u32Mask);
					kData.m_pkParent->AfterPartitionMaskUpdate();
				}

				pkPartition->m_kDisableAnimations.Clear();
			}
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::AddPositionTime(float fDeltaTime)
{
	//FV_TRACE_MSG("---------Start---------\n");
	m_kActivePartitionAnimations.BeginIterator();
	while (!m_kActivePartitionAnimations.IsEnd())
	{
		FvRefList<FvPartitionAnimationState*>::iterator iter = m_kActivePartitionAnimations.GetIterator();
		FvPartitionAnimationState* pkState = (*iter).m_Content;
		m_kActivePartitionAnimations.Next();
		
		pkState->AddTime(fDeltaTime, pkState->m_bIsOnPartition);

		/*char acBuffer[1024];

		sprintf_s(acBuffer, "%s: %x, %f,%f,%f,%f,%f,%f,%f\n", pkState->m_ppkAnimationStates[0]->getAnimationName().c_str(), pkState->m_u32PartitionMask
			,pkState->m_pfWeights[0],pkState->m_pfWeights[1],pkState->m_pfWeights[2],pkState->m_pfWeights[3],pkState->m_pfWeights[4],pkState->m_pfWeights[5]
			,pkState->m_pfWeights[6]);
		FV_TRACE_MSG(acBuffer);*/
	}
	//FV_TRACE_MSG("----------End----------\n");
}
//----------------------------------------------------------------------------
FvUInt32 FvAnimatedCharacterBase::GetAnimation(const char* pcAnimationName)
{
	FvAnimatedCharacterResource::AliasName::iterator itAlias
		= m_spResource->m_kAnimationAlias.find(pcAnimationName);
	if(itAlias == m_spResource->m_kAnimationAlias.end())
	{
		FvStringMap<FvUInt32>::iterator it
			= m_spResource->m_kAnimationIndexes.find(pcAnimationName);

		if(it != m_spResource->m_kAnimationIndexes.end())
		{
			return it->second;
		}
	}
	else
	{
		FvStringMap<FvUInt32>::iterator it
			= m_spResource->m_kAnimationIndexes.find(itAlias->second);

		if(it != m_spResource->m_kAnimationIndexes.end())
		{
			return it->second;
		}
	}

	return NONE_ANIMATION;
}
//----------------------------------------------------------------------------
FvUInt32 FvAnimatedCharacterBase::GetAnimationReal(const char* pcAnimationName)
{

	FvStringMap<FvUInt32>::iterator it
		= m_spResource->m_kAnimationIndexes.find(pcAnimationName);

	if(it != m_spResource->m_kAnimationIndexes.end())
	{
		return it->second;
	}

	return NONE_ANIMATION;
}
//----------------------------------------------------------------------------
const char* FvAnimatedCharacterBase::GetAnimationName(FvUInt32 u32Animation)
{
	if(u32Animation < m_spResource->m_kAnimationIndexes.size())
	{
		return m_spResource->m_pkAnimations[u32Animation].m_kName.c_str();
	}
	else
	{
		return NULL;
	}
}
//----------------------------------------------------------------------------
FvAnimationState* FvAnimatedCharacterBase::GetAnimationState(
	FvUInt32 u32Animation)
{
	if(u32Animation < m_spResource->m_kAnimationIndexes.size())
	{
		return m_ppkAnimationStates[u32Animation];
	}
	else
	{
		return NULL;
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::EnablePartitionAnimaiton(FvUInt32 u32Animation,
	FvUInt32* pu32Priority, bool bLoop, bool bManual, bool bContinuous)
{
	if(u32Animation < m_spResource->GetNumAnimation())
	{
		FV_ASSERT(pu32Priority);
		EnableAnimation(u32Animation, FvAnimationState::PARTITION);
		((FvPartitionAnimationState*)m_ppkAnimationStates[u32Animation])->SetPriority(pu32Priority);
		((FvPartitionAnimationState*)m_ppkAnimationStates[u32Animation])->UpdateFromBonePartition();

		m_ppkAnimationStates[u32Animation]->SetLoop(bLoop);
		m_ppkAnimationStates[u32Animation]->SetManualTime(bManual);
		m_ppkAnimationStates[u32Animation]->SetContinuous(bContinuous);
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::EnableExtraAnimaiton(FvUInt32 u32Animation,
	bool bLoop, bool bManual, bool bContinuous)
{
	if(u32Animation < m_spResource->GetNumAnimation())
	{
		EnableAnimation(u32Animation, FvAnimationState::EXTRA);
		((FvExtraAnimationState*)m_ppkAnimationStates[u32Animation])->UpdateNonePartition();

		m_ppkAnimationStates[u32Animation]->SetLoop(bLoop);
		m_ppkAnimationStates[u32Animation]->SetManualTime(bManual);
		m_ppkAnimationStates[u32Animation]->SetContinuous(bContinuous);
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::EnableAnimation(FvUInt32 u32Animation,
	FvAnimationState::AnimationStateType eType)
{
	if(m_ppkAnimationStates[u32Animation])
	{
		DisableAnimation(u32Animation);
	}

	switch(eType)
	{
	case FvAnimationState::PARTITION:
		m_ppkAnimationStates[u32Animation] = new FvPartitionAnimationState(
			this, u32Animation);
		break;
	case FvAnimationState::EXTRA:
		m_ppkAnimationStates[u32Animation] = new FvExtraAnimationState(
			this, u32Animation);
		break;
	}	
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::DisableAnimation(FvUInt32 u32Animation)
{
	FV_ASSERT(u32Animation < m_spResource->GetNumAnimation());
	FV_SAFE_DELETE(m_ppkAnimationStates[u32Animation]);
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::PlayAnimation(FvUInt32 u32Animation,
	bool bIsBlend)
{
	FV_ASSERT(u32Animation < m_spResource->GetNumAnimation());
	FvAnimationState* pkState = m_ppkAnimationStates[u32Animation];

	if(pkState)
	{
		switch(pkState->GetType())
		{
		case FvAnimationState::PARTITION:
			((FvPartitionAnimationState*)pkState)->Play(bIsBlend);
			break;
		case FvAnimationState::EXTRA:
			((FvExtraAnimationState*)pkState)->Play(bIsBlend);
			break;
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::StopAnimation(FvUInt32 u32Animation,
	bool bIsBlend)
{
	FV_ASSERT(u32Animation < m_spResource->GetNumAnimation());
	FvAnimationState* pkState = m_ppkAnimationStates[u32Animation];

	if(pkState)
	{
		switch(pkState->GetType())
		{
		case FvAnimationState::PARTITION:
			((FvPartitionAnimationState*)pkState)->Stop(bIsBlend);
			break;
		case FvAnimationState::EXTRA:
			((FvExtraAnimationState*)pkState)->Stop(bIsBlend);
			break;
		}
	}
}
//----------------------------------------------------------------------------
float FvAnimatedCharacterBase::GetBlendTime(FvUInt32 u32From, FvUInt32 u32To)
{
	FvAnimatedCharacterResource::InterBlendMap::iterator it
		= m_spResource->m_kAnimationInterBlendMap.find(std::make_pair(u32From, u32To));
	if(it == m_spResource->m_kAnimationInterBlendMap.end())
	{
		FvAnimatedCharacterResource::BlendMap::iterator iter
			= m_spResource->m_kAnimationBlendMap.find(u32To);
		if(iter == m_spResource->m_kAnimationBlendMap.end())
		{
			iter = m_spResource->m_kAnimationBlendMap.find(u32From);
			if(iter == m_spResource->m_kAnimationBlendMap.end())
			{
				return ms_fDefaultBlendTime;
			}
			else
			{
				return iter->second;
			}
		}
		else
		{
			return iter->second;
		}
	}
	else
	{
		return it->second;
	}
}
//----------------------------------------------------------------------------
const char* FvAnimatedCharacterBase::GetName()
{
	return m_kName.c_str();
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::PutOn(FvAnimatedCharacterNode& kNode)
{
	if(kNode.m_pkData)
	{
		if(kNode.m_pkData->m_pkParent == this)
		{
			return;
		}
		else
		{
			PutOff(kNode);
		}
	}

	FV_ASSERT(kNode.m_pkData == NULL);

	kNode.m_pkData = new FvAnimatedCharacterNodePrivateData(this, &kNode);
	m_kNodesWaitLoad.AttachBack(kNode);
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::PutOff(FvAnimatedCharacterNode& kNode)
{
	if(kNode.m_pkData && (kNode.m_pkData->m_pkParent == this))
	{
		if(kNode.IsAttach(m_kNodesOnCharacter))
		{
			Cut(*kNode.m_pkData);
		}

		kNode.Detach();
		delete kNode.m_pkData;
		kNode.m_pkData = NULL;
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::RefreshLink(FvAnimatedCharacterNode& kNode)
{
	if(kNode.m_pkData && (kNode.m_pkData->m_pkParent == this))
	{
		if(kNode.IsAttach(m_kNodesOnCharacter))
		{
			for(FvUInt32 i(0); i < kNode.m_pkData->m_u32NumLinkEntity; ++i)
			{
				FvAnimatedCharacterNodePrivateData::LinkEntity& kLinkEntity
					= kNode.m_pkData->m_pkLinkEntities[i];

				for(FvUInt32 j(0); j < m_u32BindingNumber; ++j)
				{
					m_ppkAnimatedEntities[j]->Detach(kLinkEntity.m_ppkEntities[j]);
				}

				FvAnimatedCharacterNode::LinkEntityData& kData = kNode.m_kLinkEntityDatas[i];

				kLinkEntity.m_kTagName = kData.m_pcNameOfLink;

				for(FvUInt32 j(0); j < m_u32BindingNumber; ++j)
				{
					m_ppkAnimatedEntities[j]->Attach(kLinkEntity.m_ppkEntities[j], kLinkEntity.m_kTagName.c_str(), m_spResource->m_kBoneAlias);					
				}
			}
		}
		else if(kNode.m_pkData)
		{
			for(FvUInt32 i(0); i < kNode.m_pkData->m_u32NumLinkEntity; ++i)
			{
				FvAnimatedCharacterNodePrivateData::LinkEntity& kLinkEntity
					= kNode.m_pkData->m_pkLinkEntities[i];

				FvAnimatedCharacterNode::LinkEntityData& kData = kNode.m_kLinkEntityDatas[i];

				kLinkEntity.m_kTagName = kData.m_pcNameOfLink;
			}
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::Cache(FvAnimatedCharacterNodePrivateData& kData)
{
	m_kNodesCache.AttachBack(*kData.m_pkSelf);
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::Link(FvAnimatedCharacterNodePrivateData& kData)
{
	FV_ASSERT(kData.m_pkParent == this);

	std::set<FvAnimatedCharacterNodePrivateData::PartEntity*> kEntityNeedUpdate;

	for(FvUInt32 i(0); i < kData.m_u32NumPartEntity; ++i)
	{
		FvAnimatedCharacterNodePrivateData::PartEntity& kPartEntity
			= kData.m_pkPartEntities[i];

		for(FvUInt32 j(0); j < m_u32BindingNumber; ++j)
		{
			kPartEntity.m_ppkEntities[j]->SetParent(m_ppkAnimatedEntities[j]);

			AddMeshToBoneSwitchTable(m_ppu8BoneSwitchTables[j], kPartEntity.m_ppkEntities[j]->getMesh());
		}

		for(FvUInt32 j(0); j < kPartEntity.m_u32LocalPartNumber; ++j)
		{
			const FvUInt32 u32GlobalPart = kPartEntity.m_pu32LocalParts[j];
			FV_ASSERT(u32GlobalPart < m_spResource->m_u32PartNumber);

			FvRefList<FvAnimatedCharacterNodePrivateData::PartEntity*>& kList
				= m_pkGlobalParts[u32GlobalPart];

			bool bAttachFlag = false;

			FvRefList<FvAnimatedCharacterNodePrivateData::PartEntity*>::iterator iter
				= kList.GetHead();

			while(!kList.IsEnd(iter))
			{
				FvAnimatedCharacterNodePrivateData::PartEntity* pkDst
					= iter->m_Content;

				if(kPartEntity.m_u32Priority <= pkDst->m_u32Priority)
				{
					if(iter == kList.GetHead())
					{
						kPartEntity.Add(FV_MASK(j));
						pkDst->Del(FV_MASK(iter - pkDst->m_pkLocalParts));

						kEntityNeedUpdate.insert(&kPartEntity);
						kEntityNeedUpdate.insert(pkDst);
					}

					kList.AttachBefore(*iter, kPartEntity.m_pkLocalParts[j]);
					bAttachFlag = true;
					break;
				}
				else
				{
					FvRefList<FvAnimatedCharacterNodePrivateData::PartEntity*>::Next(iter);
				}
			}

			if(!bAttachFlag)
			{
				kList.AttachBack(kPartEntity.m_pkLocalParts[j]);

				if(kList.Size() == 1)
				{
					kPartEntity.Add(FV_MASK(j));
					kEntityNeedUpdate.insert(&kPartEntity);
				}
			}
		}
	}

	for(std::set<FvAnimatedCharacterNodePrivateData::PartEntity*>::iterator it =
		kEntityNeedUpdate.begin(); it != kEntityNeedUpdate.end(); ++it)
	{
		(*it)->Update(m_u32BindingNumber);
	}

	for(FvUInt32 i(0); i < kData.m_u32NumLinkEntity; ++i)
	{
		FvAnimatedCharacterNodePrivateData::LinkEntity& kLinkEntity
			= kData.m_pkLinkEntities[i];

		for(FvUInt32 j(0); j < m_u32BindingNumber; ++j)
		{
			m_ppkAnimatedEntities[j]->Attach(kLinkEntity.m_ppkEntities[j],
				kLinkEntity.m_kTagName.c_str(), m_spResource->m_kBoneAlias);
		}
	}

	for(FvUInt32 i(0); i < kData.m_u32NumLinkEffect; ++i)
	{
		FvAnimatedCharacterNodePrivateData::LinkEffect& kLinkEffect
			= kData.m_pkLinkEffects[i];

		for(FvUInt32 j(0); j < m_u32BindingNumber; ++j)
		{
			if(kLinkEffect.m_ppkEffects[j])
			{
				m_ppkAnimatedEntities[j]->Attach(kLinkEffect.m_ppkEffects[j],
					kLinkEffect.m_kTagName.c_str(), m_spResource->m_kBoneAlias);

				kLinkEffect.m_ppkEffects[j]->start();
			}
		}
	}

	m_kNodesOnCharacter.AttachBack(*kData.m_pkSelf);
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::Cut(FvAnimatedCharacterNodePrivateData& kData)
{
	FV_ASSERT(kData.m_pkParent == this);

	std::set<FvAnimatedCharacterNodePrivateData::PartEntity*> kEntityNeedUpdate;

	for(FvUInt32 i(0); i < kData.m_u32NumPartEntity; ++i)
	{
		FvAnimatedCharacterNodePrivateData::PartEntity& kPartEntity
			= kData.m_pkPartEntities[i];

		for(FvUInt32 j(0); j < m_u32BindingNumber; ++j)
		{
			DelMeshFromBoneSwitchTable(m_ppu8BoneSwitchTables[j], kPartEntity.m_ppkEntities[j]->getMesh());
		}

		for(FvUInt32 j(0); j < kPartEntity.m_u32LocalPartNumber; ++j)
		{
			const FvUInt32 u32GlobalPart = kPartEntity.m_pu32LocalParts[j];
			FV_ASSERT(u32GlobalPart < m_spResource->m_u32PartNumber);

			FvRefList<FvAnimatedCharacterNodePrivateData::PartEntity*>& kList
				= m_pkGlobalParts[u32GlobalPart];

			if(&kPartEntity.m_pkLocalParts[j] == kList.GetHead())
			{
				FvAnimatedCharacterNodePrivateData::PartEntity* pkDst
					= kPartEntity.m_pkLocalParts[j].GetNextContent(NULL);
				if(pkDst)
				{
					pkDst->Add(FV_MASK(kPartEntity.m_pkLocalParts[j].Next() - pkDst->m_pkLocalParts));

					kEntityNeedUpdate.insert(pkDst);
				}

			}

			kPartEntity.m_pkLocalParts[j].Detach();
		}
	}

	for(std::set<FvAnimatedCharacterNodePrivateData::PartEntity*>::iterator it =
		kEntityNeedUpdate.begin(); it != kEntityNeedUpdate.end(); ++it)
	{
		(*it)->Update(m_u32BindingNumber);
	}

	for(FvUInt32 i(0); i < kData.m_u32NumLinkEntity; ++i)
	{
		FvAnimatedCharacterNodePrivateData::LinkEntity& kLinkEntity
			= kData.m_pkLinkEntities[i];

		for(FvUInt32 j(0); j < m_u32BindingNumber; ++j)
		{
			m_ppkAnimatedEntities[j]->Detach(kLinkEntity.m_ppkEntities[j]);
		}
	}

	for(FvUInt32 i(0); i < kData.m_u32NumLinkEffect; ++i)
	{
		FvAnimatedCharacterNodePrivateData::LinkEffect& kLinkEffect
			= kData.m_pkLinkEffects[i];

		for(FvUInt32 j(0); j < m_u32BindingNumber; ++j)
		{
			if(kLinkEffect.m_ppkEffects[j])
			{
				kLinkEffect.m_ppkEffects[j]->stop();

				m_ppkAnimatedEntities[j]->Detach(kLinkEffect.m_ppkEffects[j]);
			}
		}
	}
}
//----------------------------------------------------------------------------
FvUInt32 FvAnimatedCharacterBase::GetBindingNumber()
{
	return m_u32BindingNumber;
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::LinkCacheNodes()
{
	m_kCacheList.AttachBack(m_kNodeForCache);
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::LinkCacheNodesImpl()
{
	m_kNodesCache.BeginIterator();
	while(!m_kNodesCache.IsEnd())
	{
		FvRefListNameSpace<FvAnimatedCharacterNode>::List1::iterator iter
			= m_kNodesCache.GetIterator();
		FvAnimatedCharacterNode* pkNode = static_cast<FvAnimatedCharacterNode*>(iter);
		m_kNodesCache.Next();
		
		Link(*pkNode->m_pkData);
	}

	m_kNodeForCache.Detach();
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::Tick()
{
	if(m_kCacheList.Size())
	{
		FvAnimatedCharacterBase* pkAnimatedCharacter
			= m_kCacheList.GetHead()->m_Content;
		pkAnimatedCharacter->LinkCacheNodesImpl();
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::SetRenderState(
	FvAnimatedCharacterNodePrivateData::RenderState eState)
{
	FvRefListNameSpace<FvAnimatedCharacterNode>::List1::iterator iter = m_kNodesOnCharacter.GetHead();
	while(!m_kNodesOnCharacter.IsEnd(iter))
	{
		FvAnimatedCharacterNode* pkNode = static_cast<FvAnimatedCharacterNode*>(iter);
		FvRefListNameSpace<FvAnimatedCharacterNode>::List1::Next(iter);
		if(pkNode->GetUpdateStateWithParent())
		{
			pkNode->SetRenderState(eState);
		}
	}

	FvUInt32 u32Group;

	switch(eState)
	{
	case FvAnimatedCharacterNodePrivateData::COLOR_EFFECT:
		u32Group = FvRenderManager::COLOR_EFFECT_GROUP_ID;
		break;
	case FvAnimatedCharacterNodePrivateData::FADE_OUT:
		u32Group = FvRenderManager::FADE_OUT_GROUP_ID;
		break;
	case FvAnimatedCharacterNodePrivateData::BLEND:
		u32Group = FvRenderManager::BLEND_GROUP_ID;
		break;
	case FvAnimatedCharacterNodePrivateData::MESH_UI:
		u32Group = 90;
		break;
	case FvAnimatedCharacterNodePrivateData::EDGE_HIGHLIGHT:
		u32Group = FvRenderManager::EDGE_HIGH_LIGHT;
		break;
	case FvAnimatedCharacterNodePrivateData::EDGE_HIGHLIGHT_COLOR_EFFECT:
		u32Group = FvRenderManager::EDGE_HIGH_LIGHT_COLOR_EFFECT;
		break;
	case FvAnimatedCharacterNodePrivateData::EDGE_HIGHLIGHT_BLEND:
		u32Group = FvRenderManager::EDGE_HIGH_LIGHT_BLEND;
		break;
	default:
		u32Group = Ogre::RENDER_QUEUE_MAX;
		break;
	}

	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
	{
		m_ppkAnimatedEntities[i]->setRenderQueueGroup(u32Group);
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::SetAddColor(float r, float g, float b)
{
	FvRefListNameSpace<FvAnimatedCharacterNode>::List1::iterator iter = m_kNodesOnCharacter.GetHead();
	while(!m_kNodesOnCharacter.IsEnd(iter))
	{
		FvAnimatedCharacterNode* pkNode = static_cast<FvAnimatedCharacterNode*>(iter);
		FvRefListNameSpace<FvAnimatedCharacterNode>::List1::Next(iter);
		if(pkNode->GetUpdateStateWithParent())
		{
			pkNode->SetAddColor(r, g, b);
		}
	}

	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
	{
		for(FvUInt32 j(0); j < m_ppkAnimatedEntities[i]->getNumSubEntities(); ++j)
		{
			m_ppkAnimatedEntities[i]->getSubEntity(j)->setCustomParameter(2, Ogre::Vector4(r, g, b, 1));
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::SetMulColor(float r, float g, float b)
{
	FvRefListNameSpace<FvAnimatedCharacterNode>::List1::iterator iter = m_kNodesOnCharacter.GetHead();
	while(!m_kNodesOnCharacter.IsEnd(iter))
	{
		FvAnimatedCharacterNode* pkNode = static_cast<FvAnimatedCharacterNode*>(iter);
		FvRefListNameSpace<FvAnimatedCharacterNode>::List1::Next(iter);
		if(pkNode->GetUpdateStateWithParent())
		{
			pkNode->SetMulColor(r, g, b);
		}
	}

	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
	{
		for(FvUInt32 j(0); j < m_ppkAnimatedEntities[i]->getNumSubEntities(); ++j)
		{
			m_ppkAnimatedEntities[i]->getSubEntity(j)->setCustomParameter(1, Ogre::Vector4(r - 1, g - 1, b - 1, 1));
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::SetBlendAlpha(
	float a, float x, float y, float fRefraction)
{
	FvRefListNameSpace<FvAnimatedCharacterNode>::List1::iterator iter = m_kNodesOnCharacter.GetHead();
	while(!m_kNodesOnCharacter.IsEnd(iter))
	{
		FvAnimatedCharacterNode* pkNode = static_cast<FvAnimatedCharacterNode*>(iter);
		FvRefListNameSpace<FvAnimatedCharacterNode>::List1::Next(iter);
		if(pkNode->GetUpdateStateWithParent())
		{
			pkNode->SetBlendAlpha(a, x, y, fRefraction);
		}
	}

	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
	{
		for(FvUInt32 j(0); j < m_ppkAnimatedEntities[i]->getNumSubEntities(); ++j)
		{
			m_ppkAnimatedEntities[i]->getSubEntity(j)->setCustomParameter(7, Ogre::Vector4(fRefraction, x, -y, a));
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::SetFadeOutAlpha(float a)
{
	FvRefListNameSpace<FvAnimatedCharacterNode>::List1::iterator iter = m_kNodesOnCharacter.GetHead();
	while(!m_kNodesOnCharacter.IsEnd(iter))
	{
		FvAnimatedCharacterNode* pkNode = static_cast<FvAnimatedCharacterNode*>(iter);
		FvRefListNameSpace<FvAnimatedCharacterNode>::List1::Next(iter);
		if(pkNode->GetUpdateStateWithParent())
		{
			pkNode->SetFadeOutAlpha(a);
		}
	}

	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
	{
		for(FvUInt32 j(0); j < m_ppkAnimatedEntities[i]->getNumSubEntities(); ++j)
		{
			m_ppkAnimatedEntities[i]->getSubEntity(j)->setCustomParameter(3, Ogre::Vector4(a, a, a, a));
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::SetEdgeHighLight(
	float r, float g, float b, float a)
{
	FvRefListNameSpace<FvAnimatedCharacterNode>::List1::iterator iter = m_kNodesOnCharacter.GetHead();
	while(!m_kNodesOnCharacter.IsEnd(iter))
	{
		FvAnimatedCharacterNode* pkNode = static_cast<FvAnimatedCharacterNode*>(iter);
		FvRefListNameSpace<FvAnimatedCharacterNode>::List1::Next(iter);
		if(pkNode->GetUpdateStateWithParent())
		{
			pkNode->SetEdgeHighLight(r, g, b, a);
		}
	}

	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
	{
		for(FvUInt32 j(0); j < m_ppkAnimatedEntities[i]->getNumSubEntities(); ++j)
		{
			m_ppkAnimatedEntities[i]->getSubEntity(j)->setCustomParameter(6, Ogre::Vector4(r, g, b, a));
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::SetEdgeHighLightWidth(float fWidth)
{
	FvRefListNameSpace<FvAnimatedCharacterNode>::List1::iterator iter = m_kNodesOnCharacter.GetHead();
	while(!m_kNodesOnCharacter.IsEnd(iter))
	{
		FvAnimatedCharacterNode* pkNode = static_cast<FvAnimatedCharacterNode*>(iter);
		FvRefListNameSpace<FvAnimatedCharacterNode>::List1::Next(iter);
		if(pkNode->GetUpdateStateWithParent())
		{
			pkNode->SetEdgeHighLightWidth(fWidth);
		}
	}

	for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
	{
		for(FvUInt32 j(0); j < m_ppkAnimatedEntities[i]->getNumSubEntities(); ++j)
		{
			m_ppkAnimatedEntities[i]->getSubEntity(j)->setCustomParameter(8, Ogre::Vector4(fWidth, fWidth, fWidth, fWidth));
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::VisitPick(
	Ogre::RenderQueue *pkQueue, const Ogre::Vector4& kColor)
{
	FvRefListNameSpace<FvAnimatedCharacterNode>::List1::iterator iter = m_kNodesOnCharacter.GetHead();
	while(!m_kNodesOnCharacter.IsEnd(iter))
	{
		FvAnimatedCharacterNode* pkNode = static_cast<FvAnimatedCharacterNode*>(iter);
		FvRefListNameSpace<FvAnimatedCharacterNode>::List1::Next(iter);
		pkNode->UpdatePickColor(kColor);
	}

	for(FvUInt32 j(0); j < m_ppkAnimatedEntities[m_u32ActiveBinding]->getNumSubEntities(); ++j)
	{
		m_ppkAnimatedEntities[m_u32ActiveBinding]->getSubEntity(j)->setCustomParameter(4, kColor);
	}

	m_ppkAnimatedEntities[m_u32ActiveBinding]->_updateRenderQueue(pkQueue);
}
//----------------------------------------------------------------------------
FvUInt32 FvExtraAnimationState::ms_u32ExtraMask = 1;
//----------------------------------------------------------------------------
FvExtraAnimationState::FvExtraAnimationState(
	FvAnimatedCharacterBase* pkParent, FvUInt32 u32Index)
	: FvAnimationState(pkParent, u32Index), m_fWeight(0), m_fBlendTime(0),
	m_fInvBlendTime(0), m_fPassedBlendTime(0)
{
	m_kNode.m_Content = this;
	m_eType = EXTRA;

	SetWeightArray(&m_fWeight);
	SetExtraData(&ms_u32ExtraMask);
}
//----------------------------------------------------------------------------
void FvExtraAnimationState::UpdateNonePartition()
{
	for(FvUInt32 i(0); i < m_pkParent->m_u32BindingNumber; ++i)
	{
		m_ppkAnimationStates[i]->GetTargetAnimation()->UpdateTrackList(NULL);
	}
}
//----------------------------------------------------------------------------
void FvExtraAnimationState::AddBlend(float fDeltaTime)
{
	if(m_fPassedBlendTime < m_fBlendTime)
	{
		m_fPassedBlendTime += fDeltaTime;

		if(m_fPassedBlendTime < m_fBlendTime)
		{
			if(m_kNode.IsAttach(m_pkParent->m_kExtraAnimationDisableList))
			{
				m_fWeight = 1.0f - m_fPassedBlendTime * m_fInvBlendTime;
			}
			else
			{
				m_fWeight = m_fPassedBlendTime * m_fInvBlendTime;
			}
		}
		else
		{
			if(m_kNode.IsAttach(m_pkParent->m_kExtraAnimationDisableList))
			{
				m_fWeight = 0;
				SetEnable(false);
				SetTimePosition(0);
				m_kNode.Detach();
			}
			else
			{
				m_fWeight = 1.0f;
			}
		}
	}
}
//----------------------------------------------------------------------------
bool FvExtraAnimationState::IsPlay()
{
	return m_kNode.IsAttach(m_pkParent->m_kExtraAnimationEnableList);
}
//----------------------------------------------------------------------------
void FvExtraAnimationState::Play(bool bBlend)
{
	if(!(m_kNode.IsAttach(m_pkParent->m_kExtraAnimationEnableList)))
	{
		SetEnable(true);
		if(bBlend)
		{
			m_fBlendTime = m_pkParent->GetBlendTime(FvAnimatedCharacterBase::NONE_ANIMATION, m_u32Index);
			m_fInvBlendTime = 1.0f / m_fBlendTime;
			m_fPassedBlendTime = m_fWeight * m_fBlendTime;
		}
		else
		{
			m_fBlendTime = 0;
			m_fInvBlendTime = 0;
			m_fPassedBlendTime = 0;
			m_fWeight = 1.0f;
		}

		m_pkParent->m_kExtraAnimationEnableList.AttachBack(m_kNode);
	}	
}
//----------------------------------------------------------------------------
void FvExtraAnimationState::Stop(bool bBlend)
{
	if((m_kNode.IsAttach(m_pkParent->m_kExtraAnimationEnableList)))
	{
		if(bBlend)
		{
			m_fBlendTime = m_pkParent->GetBlendTime(m_u32Index, FvAnimatedCharacterBase::NONE_ANIMATION);
			m_fInvBlendTime = 1.0f / m_fBlendTime;
			m_fPassedBlendTime = (1 - m_fWeight) * m_fBlendTime;

			m_pkParent->m_kExtraAnimationDisableList.AttachBack(m_kNode);
		}
		else
		{
			m_fBlendTime = 0;
			m_fInvBlendTime = 0;
			m_fPassedBlendTime = 0;
			m_fWeight = 0.0f;
			m_kNode.Detach();
			SetEnable(false);
			SetTimePosition(0);
		}
	}
}
//----------------------------------------------------------------------------
FvAnimatedCharacter::FvAnimatedCharacter(
	const FvAnimatedCharacterResourcePtr& rspResource, const char* pcName,
	const FvAnimationSetupResourcePtr& rspAniSetup)
	: FvAnimatedCharacterBase(rspResource, pcName), m_spAnimationSetup(rspAniSetup)
{
	m_pkAnimationSetups = new FvAnimationSetupResource::Animation*[rspResource->GetNumAnimation()];
	for(FvUInt32 i(0); i < rspResource->GetNumAnimation(); ++i)
	{
		m_pkAnimationSetups[i] = NULL;
	}

	m_pkLevels = new Level[m_spAnimationSetup->m_kLevelMap.size()];

	
	for(FvStringMap<FvAnimationSetupResource::Animation*>::iterator it = m_spAnimationSetup->m_kAnimationMap.begin();
		it != m_spAnimationSetup->m_kAnimationMap.end(); ++it)
	{
		FvUInt32 u32Index = GetAnimationReal(it->first);
		if(u32Index < rspResource->GetNumAnimation())
		{
			FvAnimationSetupResource::Animation* pkAnimation = it->second;

			m_pkAnimationSetups[u32Index] = pkAnimation;
			if(pkAnimation->m_u32Level < m_spAnimationSetup->m_kLevelMap.size())
			{
				EnablePartitionAnimaiton(u32Index, ((FvAnimationSetupResource::LevelAnimation*)pkAnimation)->m_pu32Priorities, pkAnimation->m_bLoop, pkAnimation->m_bManual, pkAnimation->m_bContinuous);

			}
			else
			{
				EnableExtraAnimaiton(u32Index, pkAnimation->m_bLoop, pkAnimation->m_bManual, pkAnimation->m_bContinuous);

				m_kExtraAnimation[u32Index] = new Extra(u32Index);
			}

			if(pkAnimation->m_kBlendMask != "")
			{
				GetAnimationState(u32Index)->EnableBlendMask(pkAnimation->m_kBlendMask.c_str());
			}
		}
	}
}
//----------------------------------------------------------------------------
FvAnimatedCharacter::~FvAnimatedCharacter()
{
	FV_SAFE_DELETE_ARRAY(m_pkAnimationSetups);
	FV_SAFE_DELETE_ARRAY(m_pkLevels);

	for(std::map<FvUInt32, Extra*>::iterator it = m_kExtraAnimation.begin();
		it != m_kExtraAnimation.end(); ++it)
	{
		delete it->second;
	}
	m_kExtraAnimation.clear();
}
//----------------------------------------------------------------------------
void FvAnimatedCharacter::AddTime(float fDeltaTime)
{
	FvDoubleLink<Level, FvDoubleLinkNode1>::Iterator itLevel = m_kActiveLevels.GetHead();
	while(!m_kActiveLevels.IsEnd(itLevel))
	{
		Level& kLevel = *(Level*)itLevel;
		FvDoubleLink<Level, FvDoubleLinkNode1>::Next(itLevel);

		if(kLevel.m_bNeedRemove || (kLevel.m_u32CurrentAnimation == NONE_ANIMATION))
		{
			kLevel.m_bNeedRemove = false;
			FvUInt32 u32AnimationNeedRemove = kLevel.m_u32CurrentAnimation;
			bool bBlend = true;
			if(kLevel.m_kInsertQueue.size())
			{
				std::list<std::pair<FvUInt32, AnimatioPlay>>::iterator it
					= kLevel.m_kInsertQueue.begin();

				FvAnimationState* pkState = GetAnimationState(it->first);
				FV_ASSERT(pkState);

				if(it->first == u32AnimationNeedRemove)
				{
					u32AnimationNeedRemove = NONE_ANIMATION;
					if(it->second.m_fStart >= 0)
					{
						pkState->SetTimePosition(it->second.m_fStart);
					}
				}
				else if((u32AnimationNeedRemove != NONE_ANIMATION) && ((FvAnimationSetupResource::LevelAnimation*)(m_pkAnimationSetups[it->first]))->m_u32Binding
					&& (((FvAnimationSetupResource::LevelAnimation*)(m_pkAnimationSetups[it->first]))->m_u32Binding == ((FvAnimationSetupResource::LevelAnimation*)(m_pkAnimationSetups[u32AnimationNeedRemove]))->m_u32Binding))
				{
					pkState->SetTimePosition(GetAnimationState(u32AnimationNeedRemove)->GetTimePosition());
				}
				else
				{
					pkState->SetTimePosition(it->second.m_fStart);
				}

				kLevel.m_u32CurrentAnimation = pkState->GetIndex();
				
				((FvPartitionAnimationState*)pkState)->Play(it->second.m_bBlend);
				bBlend = it->second.m_bBlend;

				kLevel.m_kInsertQueue.erase(it);
			}
			else
			{
				kLevel.m_u32CurrentAnimation = NONE_ANIMATION;
				kLevel.Detach();
			}

			if(u32AnimationNeedRemove != NONE_ANIMATION)
			{
				StopAnimation(u32AnimationNeedRemove, bBlend);
			}
		}
	}

	FvDoubleLink<Extra, FvDoubleLinkNode1>::Iterator itExtra = m_kActiveExtraAnimations.GetHead();
	while(!m_kActiveExtraAnimations.IsEnd(itExtra))
	{
		Extra& kExtra =  *(Extra*)itExtra;
		FvDoubleLink<Extra, FvDoubleLinkNode1>::Next(itExtra);

		FvAnimationState* pkState = GetAnimationState(kExtra.m_u32Index);
		FV_ASSERT(pkState);

		if(kExtra.m_bNeedRemove || (!((FvExtraAnimationState*)pkState)->IsPlay()))
		{
			kExtra.m_bNeedRemove = false;
	
			if(kExtra.m_kInsertQueue.size())
			{
				std::list<AnimatioPlay>::iterator it
					= kExtra.m_kInsertQueue.begin();

				pkState->SetTimePosition(it->m_fStart);
				((FvExtraAnimationState*)pkState)->Play(it->m_bBlend);

				kExtra.m_kInsertQueue.erase(it);
			}
			else
			{
				kExtra.Detach();
			}
		}
	}

	AddBlendTime(fDeltaTime);
	
	m_kActivePartitionAnimations.BeginIterator();
	while (!m_kActivePartitionAnimations.IsEnd())
	{
		FvRefList<FvPartitionAnimationState*>::iterator iter = m_kActivePartitionAnimations.GetIterator();
		FvPartitionAnimationState* pkState = (*iter).m_Content;
		m_kActivePartitionAnimations.Next();
		
		bool bRes = pkState->AddTime(fDeltaTime, pkState->m_bIsOnPartition);

		if(bRes)
		{
			FvAnimationSetupResource::Animation* pkAni = m_pkAnimationSetups[pkState->GetIndex()];
			Level& kLevel = m_pkLevels[pkAni->m_u32Level];
			FV_ASSERT(kLevel.m_u32CurrentAnimation == pkState->GetIndex());
			kLevel.m_bNeedRemove = true;
		}
	}

	m_kExtraAnimationEnableList.BeginIterator();
	while (!m_kExtraAnimationEnableList.IsEnd())
	{
		FvRefList<FvExtraAnimationState*>::iterator iter = m_kExtraAnimationEnableList.GetIterator();
		FvExtraAnimationState* obj = (*iter).m_Content;
		m_kExtraAnimationEnableList.Next();

		obj->AddBlend(fDeltaTime);
		bool bRes = obj->AddTime(fDeltaTime, true);

		if(bRes)
		{
			Extra& kExtra = *(m_kExtraAnimation[obj->GetIndex()]);
			kExtra.m_bNeedRemove = true;
		}
	}

	m_kExtraAnimationDisableList.BeginIterator();
	while (!m_kExtraAnimationDisableList.IsEnd())
	{
		FvRefList<FvExtraAnimationState*>::iterator iter = m_kExtraAnimationDisableList.GetIterator();
		FvExtraAnimationState* obj = (*iter).m_Content;
		m_kExtraAnimationDisableList.Next();

		obj->AddBlend(fDeltaTime);
		obj->AddTime(fDeltaTime, false);
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacter::PlayAnimation(const char* pcName, PlayType eType,
	float fStart, bool bBlend)
{
	PlayAnimation(GetAnimation(pcName), eType, fStart, bBlend);
}
//----------------------------------------------------------------------------
void FvAnimatedCharacter::PlayAnimation(FvUInt32 u32Animation,
	PlayType eType, float fStart, bool bBlend)
{
	if(u32Animation >= m_spResource->GetNumAnimation()) return;

	FvAnimationSetupResource::Animation* pkAnimation = m_pkAnimationSetups[u32Animation];
	//FV_ASSERT(pkAnimation);
	if(pkAnimation == NULL)
		return;

	if(pkAnimation->m_u32Level < m_spAnimationSetup->m_kLevelMap.size())
	{
		Level& kLevel = m_pkLevels[pkAnimation->m_u32Level];

		switch(eType)
		{
		case INSERT:
			kLevel.m_kInsertQueue.push_back(std::make_pair(u32Animation, AnimatioPlay(fStart, bBlend)));
			if(!kLevel.IsAttach())
			{
				m_kActiveLevels.PushBack(kLevel);
			}
			break;
		case DEFAULT:
			if(kLevel.m_u32CurrentAnimation == u32Animation)
			{
				fStart = -1;
			}
		case BREAK:
			kLevel.m_kInsertQueue.clear();
			kLevel.m_kInsertQueue.push_back(std::make_pair(u32Animation, AnimatioPlay(fStart, bBlend)));
			if(!kLevel.IsAttach())
			{
				m_kActiveLevels.PushBack(kLevel);
			}
			kLevel.m_bNeedRemove = true;
			break;
		case BREAK_SOFT:
			kLevel.m_kInsertQueue.clear();
			kLevel.m_kInsertQueue.push_back(std::make_pair(u32Animation, AnimatioPlay(fStart, bBlend)));
			if(!kLevel.IsAttach())
			{
				m_kActiveLevels.PushBack(kLevel);
			}
			break;
		case REMOVE:
			if(kLevel.IsAttach())
			{
				if(kLevel.m_u32CurrentAnimation == u32Animation)
				{
					kLevel.m_bNeedRemove = true;
				}

				for(std::list<std::pair<FvUInt32, AnimatioPlay>>::iterator it = kLevel.m_kInsertQueue.begin();
					it != kLevel.m_kInsertQueue.end(); )
				{
					if(it->first == u32Animation)
					{
						kLevel.m_kInsertQueue.erase(it++);
					}
					else
					{
						++it;
					}
				}				
			}
			break;
		}
	}
	else
	{
		std::map<FvUInt32, Extra*>::iterator it = m_kExtraAnimation.find(u32Animation);
		FV_ASSERT(it != m_kExtraAnimation.end());
		Extra& kExtra = *(it->second);

		switch(eType)
		{
		case INSERT:
			kExtra.m_kInsertQueue.push_back(AnimatioPlay(fStart, bBlend));
			if(!kExtra.IsAttach())
			{
				m_kActiveExtraAnimations.PushBack(kExtra);
			}
			break;
		case BREAK:
			kExtra.m_kInsertQueue.clear();
			kExtra.m_kInsertQueue.push_back(AnimatioPlay(fStart, bBlend));
			if(!kExtra.IsAttach())
			{
				m_kActiveExtraAnimations.PushBack(kExtra);
			}
			kExtra.m_bNeedRemove = true;
			break;
		case BREAK_SOFT:
			kExtra.m_kInsertQueue.clear();
			kExtra.m_kInsertQueue.push_back(AnimatioPlay(fStart, bBlend));
			if(!kExtra.IsAttach())
			{
				m_kActiveExtraAnimations.PushBack(kExtra);
			}
			break;
			break;
		case REMOVE:
			if(kExtra.IsAttach())
			{
				kExtra.m_kInsertQueue.clear();
				kExtra.m_bNeedRemove = true;
			}
			break;
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacter::RemoveLevel(const char* pcName)
{
	FvStringMap<FvUInt32>::iterator it = m_spAnimationSetup->m_kLevelMap.find(pcName);
	if(it != m_spAnimationSetup->m_kLevelMap.end())
	{
		Level& kLevel = m_pkLevels[it->second];
		if(kLevel.IsAttach())
		{
			kLevel.m_kInsertQueue.clear();
			kLevel.m_bNeedRemove = true;
		}
	}
}
//----------------------------------------------------------------------------
FvAnimatedCharacter::Level::Level() : m_u32CurrentAnimation(NONE_ANIMATION),
	m_bNeedRemove(false)
{

}
//----------------------------------------------------------------------------
FvAnimatedCharacterNode::PartEntityData::PartEntityData() : m_u32Priority(0)
{
	FvZeroMemory(m_apcNames,sizeof(m_apcNames));
	FvZeroMemory(m_apcMeshes,sizeof(m_apcMeshes));
	FvZeroMemory(m_apcMaterials,sizeof(m_apcMaterials));
}
//----------------------------------------------------------------------------
FvAnimatedCharacterNode::LinkEntityData::LinkEntityData()
{
	FvZeroMemory(m_apcNames,sizeof(m_apcNames));
	FvZeroMemory(m_apcMeshes,sizeof(m_apcMeshes));
	FvZeroMemory(m_apcMaterials,sizeof(m_apcMaterials));
	m_pcNameOfLink = NULL;
}
//----------------------------------------------------------------------------
FvAnimatedCharacterNode::LinkEffectData::LinkEffectData()
{
	FvZeroMemory(m_apcNames,sizeof(m_apcNames));
	FvZeroMemory(m_apcEffects,sizeof(m_apcEffects));
	m_pcNameOfLink = NULL;
}
//----------------------------------------------------------------------------
FvAnimatedCharacterNodePrivateData::PartEntity::PartEntity()
	: m_u32Priority(0), m_ppkEntities(NULL), m_pkLocalParts(NULL),
	m_pu32LocalParts(0), m_u32LocalPartNumber(0), m_u32DisplayFlags(0)
{
}
//----------------------------------------------------------------------------
FvAnimatedCharacterNodePrivateData::PartEntity::~PartEntity()
{
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::PartEntity::Add(FvUInt32 u32Flag)
{
	FV_MASK_ADD(m_u32DisplayFlags, u32Flag);
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::PartEntity::Del(FvUInt32 u32Flag)
{
	FV_MASK_DEL(m_u32DisplayFlags, u32Flag);
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::PartEntity::Update(
	FvUInt32 u32BindingNumber)
{
	for(FvUInt32 i(0); i < u32BindingNumber; ++i)
	{
		FV_ASSERT(m_ppkEntities[i]);
		m_ppkEntities[i]->SetDisplayFlags(m_u32DisplayFlags);
	}
}
//----------------------------------------------------------------------------
FvAnimatedCharacterNodePrivateData::LinkEntity::LinkEntity()
	: m_ppkEntities(NULL)
{

}
//----------------------------------------------------------------------------
FvAnimatedCharacterNodePrivateData::LinkEntity::~LinkEntity()
{

}
//----------------------------------------------------------------------------
FvAnimatedCharacterNodePrivateData::LinkEffect::LinkEffect()
	: m_ppkEffects(NULL)
{

}
//----------------------------------------------------------------------------
FvAnimatedCharacterNodePrivateData::LinkEffect::~LinkEffect()
{

}
//----------------------------------------------------------------------------
FvAnimatedCharacterNode::FvAnimatedCharacterNode() : m_pkData(NULL)
{
}
//----------------------------------------------------------------------------
FvAnimatedCharacterNode::~FvAnimatedCharacterNode()
{
	PutOff();
}
//----------------------------------------------------------------------------
bool FvAnimatedCharacterNode::IsOnAvatar(FvAnimatedCharacterBase* pkCharacter)
{
	if(m_pkData)
	{
		return m_pkData->m_pkParent == pkCharacter;
	}
	else
	{
		return pkCharacter == NULL;
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNode::PutOn(FvAnimatedCharacterBase* pkCharacter)
{
	FV_ASSERT(pkCharacter);

	pkCharacter->PutOn(*this);
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNode::PutOff()
{
	if(m_pkData)
	{
		m_pkData->m_pkParent->PutOff(*this);
	}
}
//----------------------------------------------------------------------------
FvAnimatedCharacterBase* FvAnimatedCharacterNode::GetParent()
{
	return m_pkData ? m_pkData->m_pkParent : NULL;
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNode::RefreshLink()
{
	if(m_pkData)
	{
		m_pkData->m_pkParent->RefreshLink(*this);
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNode::SetRenderState(RenderState eState)
{
	if(m_pkData && (m_pkData->m_eRenderState != eState))
	{
		FvUInt32 u32Group;

		switch(eState)
		{
		case FvAnimatedCharacterNodePrivateData::COLOR_EFFECT:
			u32Group = FvRenderManager::COLOR_EFFECT_GROUP_ID;
			break;
		case FvAnimatedCharacterNodePrivateData::FADE_OUT:
			u32Group = FvRenderManager::FADE_OUT_GROUP_ID;
			break;
		case FvAnimatedCharacterNodePrivateData::BLEND:
			u32Group = FvRenderManager::BLEND_GROUP_ID;
			break;
		case FvAnimatedCharacterNodePrivateData::MESH_UI:
			u32Group = 90;
			break;
		case FvAnimatedCharacterNodePrivateData::EDGE_HIGHLIGHT:
			u32Group = FvRenderManager::EDGE_HIGH_LIGHT;
			break;
		case FvAnimatedCharacterNodePrivateData::EDGE_HIGHLIGHT_COLOR_EFFECT:
			u32Group = FvRenderManager::EDGE_HIGH_LIGHT_COLOR_EFFECT;
			break;
		case FvAnimatedCharacterNodePrivateData::EDGE_HIGHLIGHT_BLEND:
			u32Group = FvRenderManager::EDGE_HIGH_LIGHT_BLEND;
			break;
		default:
			u32Group = Ogre::RENDER_QUEUE_MAX;
			break;
		}
		m_pkData->SetGroup(u32Group);
		m_pkData->m_eRenderState = eState;
	}
}
//----------------------------------------------------------------------------
FvAnimatedCharacterNode::RenderState FvAnimatedCharacterNode::GetRenderState()
{
	return m_pkData ? m_pkData->m_eRenderState : FvAnimatedCharacterNodePrivateData::NORMAL;
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNode::SetUpdateStateWithParent(bool bEnable)
{
	FV_ASSERT(m_pkData);

	m_pkData->m_bUpdateStateWithParent = bEnable;
}
//----------------------------------------------------------------------------
bool FvAnimatedCharacterNode::GetUpdateStateWithParent()
{
	FV_ASSERT(m_pkData);

	return m_pkData->m_bUpdateStateWithParent;
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNode::SetAddColor(float r, float g, float b)
{
	FV_ASSERT(m_pkData);
	m_pkData->SetCustomParams(2, Ogre::Vector4(r, g, b, 1));
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNode::SetMulColor( float r, float g, float b )
{
	FV_ASSERT(m_pkData);
	m_pkData->SetCustomParams(1, Ogre::Vector4(r - 1, g - 1, b - 1, 0));
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNode::SetBlendAlpha(
	float a, float x, float y, float fRefraction)
{
	FV_ASSERT(m_pkData);
	m_pkData->SetCustomParams(7, Ogre::Vector4(fRefraction, x, -y, a));
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNode::SetFadeOutAlpha( float a )
{
	FV_ASSERT(m_pkData);
	m_pkData->SetCustomParams(3, Ogre::Vector4(a, a, a, a));
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNode::SetEdgeHighLight( float r, float g, float b, float a )
{
	FV_ASSERT(m_pkData);
	m_pkData->SetCustomParams(6, Ogre::Vector4(r, g, b, a));
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNode::SetEdgeHighLightWidth( float fWidth )
{
	FV_ASSERT(m_pkData);
	m_pkData->SetCustomParams(8, Ogre::Vector4(fWidth, fWidth, fWidth, fWidth));
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNode::UpdatePickColor(const Ogre::Vector4& kID)
{
	FV_ASSERT(m_pkData);
	m_pkData->UpdatePickColor(kID);
}
//----------------------------------------------------------------------------
FvAnimatedCharacterNodePrivateData::FvAnimatedCharacterNodePrivateData(
	FvAnimatedCharacterBase* pkParent, FvAnimatedCharacterNode* pkNode)
	: m_pkParent(pkParent), m_pkSelf(pkNode), m_eRenderState(NORMAL),
	m_bUpdateStateWithParent(true)
{
	InitPartEntity();

	InitLinkEntity();

	InitLinkEffect();

	InitLoader();
}
//----------------------------------------------------------------------------
FvAnimatedCharacterNodePrivateData::~FvAnimatedCharacterNodePrivateData()
{
	TermLoader();

	TermLinkEffect();

	TermLinkEntity();

	TermPartEntity();
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::InitPartEntity()
{
	m_u32NumPartEntity = m_pkSelf->m_kPartEntityDatas.size();
	m_pkPartEntities = new PartEntity[m_u32NumPartEntity];

	for(FvUInt32 i(0); i < m_u32NumPartEntity; ++i)
	{
		FvAnimatedCharacterNode::PartEntityData& kData
			= m_pkSelf->m_kPartEntityDatas[i];
		PartEntity& kEntity = m_pkPartEntities[i];

		kEntity.m_u32Priority = kData.m_u32Priority;

		kEntity.m_u32LocalPartNumber = kData.m_kParts.size();
		kEntity.m_pkLocalParts = new FvRefNode2<PartEntity*>[kEntity.m_u32LocalPartNumber];
		kEntity.m_pu32LocalParts = new FvUInt32[kEntity.m_u32LocalPartNumber];
		kEntity.m_ppkEntities = new FvComponentEntity*[m_pkParent->GetBindingNumber()];
		FvZeroMemoryEx(kEntity.m_ppkEntities, sizeof(FvComponentEntity*), m_pkParent->GetBindingNumber());

		for(FvUInt32 j(0); j < kEntity.m_u32LocalPartNumber; ++j)
		{
			kEntity.m_pkLocalParts[j].m_Content = m_pkPartEntities + i;
			kEntity.m_pu32LocalParts[j] = kData.m_kParts[j];
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::TermPartEntity()
{
	for(FvUInt32 i(0); i < m_u32NumPartEntity; ++i)
	{
		PartEntity& kEntity = m_pkPartEntities[i];

		for(FvUInt32 j(0); j < m_pkParent->GetBindingNumber(); ++j)
		{
			if(kEntity.m_ppkEntities[j])
			{
				Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
					_getCurrentSceneManager();
				FV_ASSERT(pkSceneManager);
				pkSceneManager->destroyMovableObject(
					kEntity.m_ppkEntities[j]);
			}
		}

		delete [] kEntity.m_ppkEntities;
		delete [] kEntity.m_pu32LocalParts;
		delete [] kEntity.m_pkLocalParts;
	}

	delete [] m_pkPartEntities;
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::InitLinkEntity()
{
	m_u32NumLinkEntity = m_pkSelf->m_kLinkEntityDatas.size();
	m_pkLinkEntities = new LinkEntity[m_u32NumLinkEntity];

	for(FvUInt32 i(0); i < m_u32NumLinkEntity; ++i)
	{
		FvAnimatedCharacterNode::LinkEntityData& kData
			= m_pkSelf->m_kLinkEntityDatas[i];
		LinkEntity& kEntity = m_pkLinkEntities[i];

		kEntity.m_kTagName = kData.m_pcNameOfLink;

		kEntity.m_ppkEntities = new Entity*[m_pkParent->GetBindingNumber()];
		FvZeroMemoryEx(kEntity.m_ppkEntities, sizeof(Entity*), m_pkParent->GetBindingNumber());
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::TermLinkEntity()
{
	for(FvUInt32 i(0); i < m_u32NumLinkEntity; ++i)
	{
		LinkEntity& kEntity = m_pkLinkEntities[i];

		for(FvUInt32 j(0); j < m_pkParent->GetBindingNumber(); ++j)
		{
			if(kEntity.m_ppkEntities[j])
			{
				Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
					_getCurrentSceneManager();
				FV_ASSERT(pkSceneManager);
				pkSceneManager->destroyEntity(kEntity.m_ppkEntities[j]);
			}
		}

		delete [] kEntity.m_ppkEntities;
	}

	delete [] m_pkLinkEntities;
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::InitLinkEffect()
{
	m_u32NumLinkEffect = m_pkSelf->m_kLinkEffectDatas.size();
	m_pkLinkEffects = new LinkEffect[m_u32NumLinkEffect];

	for(FvUInt32 i(0); i < m_u32NumLinkEffect; ++i)
	{
		FvAnimatedCharacterNode::LinkEffectData& kData
			= m_pkSelf->m_kLinkEffectDatas[i];
		LinkEffect& kEffect = m_pkLinkEffects[i];

		kEffect.m_kTagName = kData.m_pcNameOfLink;

		kEffect.m_ppkEffects = new ParticleUniverse::ParticleSystem*[m_pkParent->GetBindingNumber()];
		FvZeroMemoryEx(kEffect.m_ppkEffects, sizeof(ParticleUniverse::ParticleSystem*), m_pkParent->GetBindingNumber());
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::TermLinkEffect()
{
	for(FvUInt32 i(0); i < m_u32NumLinkEffect; ++i)
	{
		LinkEffect& kEffect = m_pkLinkEffects[i];

		for(FvUInt32 j(0); j < m_pkParent->GetBindingNumber(); ++j)
		{
			ParticleUniverse::ParticleSystemManager *pkManager = 
				ParticleUniverse::ParticleSystemManager::getSingletonPtr();
			Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
				_getCurrentSceneManager();
			FV_ASSERT(pkSceneManager && pkManager);
			if(kEffect.m_ppkEffects[j])
			{
				pkManager->destroyParticleSystem(kEffect.m_ppkEffects[j], pkSceneManager);
			}
		}

		delete [] kEffect.m_ppkEffects;
	}

	delete [] m_pkLinkEffects;
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::InitLoader()
{
	m_spLoader = new Loader(this);
	
	FvBGTaskManager::Instance().AddBackgroundTask(m_spLoader, FvBGTaskManager::LOW);
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::TermLoader()
{
	if(m_spLoader)
	{
		m_spLoader->m_pkParent = NULL;
		m_spLoader = NULL;
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::FinishLoad()
{
	bool bLinkNow = m_pkSelf->LoadFinishAll();

	if(bLinkNow)
	{
		m_pkParent->Link(*this);
	}
	else
	{
		m_pkParent->Cache(*this);
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::SetGroup(FvUInt32 u32Group)
{
	for(FvUInt32 i(0); i < m_u32NumPartEntity; ++i)
	{
		for(FvUInt32 j(0); j < m_pkParent->GetBindingNumber(); ++j)
		{
			m_pkPartEntities[i].m_ppkEntities[j]->setRenderQueueGroup(u32Group);
		}
	}

	for(FvUInt32 i(0); i < m_u32NumLinkEntity; ++i)
	{
		for(FvUInt32 j(0); j < m_pkParent->GetBindingNumber(); ++j)
		{
			m_pkLinkEntities[i].m_ppkEntities[j]->setRenderQueueGroup(u32Group);
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::SetCustomParams(FvUInt32 u32ID,
	const Ogre::Vector4& kParams)
{
	for(FvUInt32 i(0); i < m_u32NumPartEntity; ++i)
	{
		for(FvUInt32 j(0); j < m_pkParent->GetBindingNumber(); ++j)
		{
			m_pkPartEntities[i].m_ppkEntities[j]->setCustomParameter(u32ID, kParams);
		}
	}

	for(FvUInt32 i(0); i < m_u32NumLinkEntity; ++i)
	{
		for(FvUInt32 j(0); j < m_pkParent->GetBindingNumber(); ++j)
		{
			for(FvUInt32 k(0);
				k < m_pkLinkEntities[i].m_ppkEntities[j]->getNumSubEntities();
				++k)
			{
				m_pkLinkEntities[i].m_ppkEntities[j]->getSubEntity(k)->setCustomParameter(u32ID, kParams);
			}
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::SetCustomParams(
	FvUInt32 u32ID, FvUInt32 u32Pos, float fParam)
{
	for(FvUInt32 i(0); i < m_u32NumPartEntity; ++i)
	{
		for(FvUInt32 j(0); j < m_pkParent->GetBindingNumber(); ++j)
		{
			Vector4 kParams = m_pkPartEntities[i].m_ppkEntities[j]->getCustomParameter(u32ID);
			kParams[u32Pos] = fParam;
			m_pkPartEntities[i].m_ppkEntities[j]->setCustomParameter(u32ID, kParams);
		}
	}

	for(FvUInt32 i(0); i < m_u32NumLinkEntity; ++i)
	{
		for(FvUInt32 j(0); j < m_pkParent->GetBindingNumber(); ++j)
		{
			for(FvUInt32 k(0);
				k < m_pkLinkEntities[i].m_ppkEntities[j]->getNumSubEntities();
				++k)
			{
				Vector4 kParams = m_pkLinkEntities[i].m_ppkEntities[j]->getSubEntity(k)->getCustomParameter(u32ID);
				kParams[u32Pos] = fParam;
				m_pkLinkEntities[i].m_ppkEntities[j]->getSubEntity(k)->setCustomParameter(u32ID, kParams);
			}
		}
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::UpdatePickColor(
	const Ogre::Vector4& kID)
{
	for(FvUInt32 i(0); i < m_u32NumPartEntity; ++i)
	{
		m_pkPartEntities[i].m_ppkEntities[m_pkParent->m_u32ActiveBinding]->setCustomParameter(4, kID);
	}

	for(FvUInt32 i(0); i < m_u32NumLinkEntity; ++i)
	{
		for(FvUInt32 k(0);
			k < m_pkLinkEntities[i].m_ppkEntities[m_pkParent->m_u32ActiveBinding]->getNumSubEntities();
			++k)
		{
			m_pkLinkEntities[i].m_ppkEntities[m_pkParent->m_u32ActiveBinding]->getSubEntity(k)->setCustomParameter(4, kID);
		}
	}
}
//----------------------------------------------------------------------------
FvAnimatedCharacterNodePrivateData::Loader::Loader(
	FvAnimatedCharacterNodePrivateData* pkParent) : m_pkParent(pkParent)
{
	FV_ASSERT(m_pkParent->m_u32NumPartEntity == m_pkParent->m_pkSelf->m_kPartEntityDatas.size());
	m_kPartEntityDatas.resize(m_pkParent->m_pkSelf->m_kPartEntityDatas.size());

	for(FvUInt32 i(0); i < m_kPartEntityDatas.size(); ++i)
	{
		PartEntity& kEntity = m_kPartEntityDatas[i];
		kEntity.resize(m_pkParent->m_pkParent->GetBindingNumber());

		FvAnimatedCharacterNode::PartEntityData& kData
			= m_pkParent->m_pkSelf->m_kPartEntityDatas[i];

		for(FvUInt32 j(0); j < kEntity.size(); ++j)
		{
			PartEntityBinding& kBinding = kEntity[j];
			kBinding.m_kName = kData.m_apcNames[j];
			kBinding.m_kParams["mesh"] = kData.m_apcMeshes[j];
			kBinding.m_kParams["material"] = kData.m_apcMaterials[j];
			kBinding.m_pkEntity = NULL;
		}
	}

	FV_ASSERT(m_pkParent->m_u32NumLinkEntity == m_pkParent->m_pkSelf->m_kLinkEntityDatas.size());
	m_kLinkEntityDatas.resize(m_pkParent->m_pkSelf->m_kLinkEntityDatas.size());

	for(FvUInt32 i(0); i < m_kLinkEntityDatas.size(); ++i)
	{
		LinkEntity& kEntity = m_kLinkEntityDatas[i];
		kEntity.resize(m_pkParent->m_pkParent->GetBindingNumber());

		FvAnimatedCharacterNode::LinkEntityData& kData
			= m_pkParent->m_pkSelf->m_kLinkEntityDatas[i];

		for(FvUInt32 j(0); j < kEntity.size(); ++j)
		{
			LinkEntityBinding& kBinding = kEntity[j];
			kBinding.m_kName = kData.m_apcNames[j];
			kBinding.m_kMeshName = kData.m_apcMeshes[j];
			kBinding.m_kMaterialName = kData.m_apcMaterials[j];
			kBinding.m_pkEntity = NULL;
		}
	}

	FV_ASSERT(m_pkParent->m_u32NumLinkEffect == m_pkParent->m_pkSelf->m_kLinkEffectDatas.size());
	m_kLinkEffectDatas.resize(m_pkParent->m_pkSelf->m_kLinkEffectDatas.size());

	for(FvUInt32 i(0); i < m_kLinkEffectDatas.size(); ++i)
	{
		LinkEffect& kEffect = m_kLinkEffectDatas[i];
		kEffect.resize(m_pkParent->m_pkParent->GetBindingNumber());

		FvAnimatedCharacterNode::LinkEffectData& kData
			= m_pkParent->m_pkSelf->m_kLinkEffectDatas[i];

		for(FvUInt32 j(0); j < kEffect.size(); ++j)
		{
			LinkEffectBinding& kBinding = kEffect[j];
			kBinding.m_kName = kData.m_apcNames[j];
			kBinding.m_kEffect = kData.m_apcEffects[j];
		}
	}
}
//----------------------------------------------------------------------------
FvAnimatedCharacterNodePrivateData::Loader::~Loader()
{

}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::Loader::DoBackgroundTask(
	FvBGTaskManager &kMgr)
{
	for(FvUInt32 i(0); i < m_kPartEntityDatas.size(); ++i)
	{
		PartEntity& kEntity = m_kPartEntityDatas[i];

		for(FvUInt32 j(0); j < kEntity.size(); ++j)
		{
			PartEntityBinding& kBinding = kEntity[j];

			Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
				_getCurrentSceneManager();
			FV_ASSERT(pkSceneManager);
			
			kBinding.m_pkEntity = (FvComponentEntity*)
				(pkSceneManager->createMovableObject(kBinding.m_kName,
				"ComponentEntity", &kBinding.m_kParams));

			FV_ASSERT(kBinding.m_pkEntity);

			if(kBinding.m_pkEntity)
			{
				kBinding.m_pkEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAX);
				kBinding.m_pkEntity->setCustomParameter(0, Ogre::Vector4(0.5f, 0.5f, 0.5f, 0.5f));
			}
		}
	}

	for(FvUInt32 i(0); i < m_kLinkEntityDatas.size(); ++i)
	{
		LinkEntity& kEntity = m_kLinkEntityDatas[i];

		for(FvUInt32 j(0); j < kEntity.size(); ++j)
		{
			LinkEntityBinding& kBinding = kEntity[j];

			Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
				_getCurrentSceneManager();
			FV_ASSERT(pkSceneManager);

			kBinding.m_pkEntity = pkSceneManager->createEntity(
				kBinding.m_kName, kBinding.m_kMeshName);
			kBinding.m_pkEntity->setMaterialName(kBinding.m_kMaterialName);

			{
				kBinding.m_pkEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAX);

				for(FvUInt32 s(0); s < kBinding.m_pkEntity->getNumSubEntities(); ++s)
				{
					kBinding.m_pkEntity->getSubEntity(s)->setCustomParameter(0, Ogre::Vector4(0.5f, 0.5f, 0.5f, 0.5f));
				}
			}
		}
	}

	kMgr.AddMainThreadTask(this);
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterNodePrivateData::Loader::DoMainThreadTask(
	FvBGTaskManager &kMgr)
{
	if(m_pkParent)
	{
		for(FvUInt32 i(0); i < m_kPartEntityDatas.size(); ++i)
		{
			PartEntity& kEntity = m_kPartEntityDatas[i];

			FvAnimatedCharacterNodePrivateData::PartEntity& kPartEntity
				= m_pkParent->m_pkPartEntities[i];

			for(FvUInt32 j(0); j < kEntity.size(); ++j)
			{
				PartEntityBinding& kBinding = kEntity[j];

				kPartEntity.m_ppkEntities[j] = kBinding.m_pkEntity;
			}
		}

		for(FvUInt32 i(0); i < m_kLinkEntityDatas.size(); ++i)
		{
			LinkEntity& kEntity = m_kLinkEntityDatas[i];

			FvAnimatedCharacterNodePrivateData::LinkEntity& kLinkEntity
				= m_pkParent->m_pkLinkEntities[i];

			for(FvUInt32 j(0); j < kEntity.size(); ++j)
			{
				LinkEntityBinding& kBinding = kEntity[j];

				kLinkEntity.m_ppkEntities[j] = kBinding.m_pkEntity;
			}
		}

		ParticleUniverse::ParticleSystemManager *pkManager = 
			ParticleUniverse::ParticleSystemManager::getSingletonPtr();
		Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
			_getCurrentSceneManager();
		if(pkSceneManager && pkManager)
		{
			for(FvUInt32 i(0); i < m_kLinkEffectDatas.size(); ++i)
			{
				LinkEffect& kEffect = m_kLinkEffectDatas[i];

				FvAnimatedCharacterNodePrivateData::LinkEffect& kLinkEffect
					= m_pkParent->m_pkLinkEffects[i];

				for(FvUInt32 j(0); j < kEffect.size(); ++j)
				{
					LinkEffectBinding& kBinding = kEffect[j];

					if(kBinding.m_kEffect != "")
					{
						kLinkEffect.m_ppkEffects[j] = pkManager->createParticleSystem(
							kBinding.m_kName, kBinding.m_kEffect, pkSceneManager);
						kLinkEffect.m_ppkEffects[j]->setRenderQueueGroup(89);
					}				
				}
			}
		}

		m_pkParent->m_spLoader = NULL;
		m_pkParent->FinishLoad();
	}
	else
	{
		Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
			_getCurrentSceneManager();
		FV_ASSERT(pkSceneManager);

		for(FvUInt32 i(0); i < m_kPartEntityDatas.size(); ++i)
		{
			PartEntity& kEntity = m_kPartEntityDatas[i];

			for(FvUInt32 j(0); j < kEntity.size(); ++j)
			{
				PartEntityBinding& kBinding = kEntity[j];

				pkSceneManager->destroyMovableObject(kBinding.m_pkEntity);
			}
		}

		for(FvUInt32 i(0); i < m_kLinkEntityDatas.size(); ++i)
		{
			LinkEntity& kEntity = m_kLinkEntityDatas[i];

			for(FvUInt32 j(0); j < kEntity.size(); ++j)
			{
				LinkEntityBinding& kBinding = kEntity[j];

				pkSceneManager->destroyEntity(kBinding.m_pkEntity);
			}
		}
	}
}
//----------------------------------------------------------------------------
static FvAnimatedEntityFactory* ms_pkAnimatedFactory = NULL;
static FvComponentEntityFactory* ms_pkComponentFactory = NULL;
//----------------------------------------------------------------------------
void FvAnimatedCharacter::Init()
{
	FvAnimatedCharacterResourceManager::Create("General");
	FvAnimationSetupResourceManager::Create("General");

	ms_pkAnimatedFactory = OGRE_NEW FvAnimatedEntityFactory();
	FV_ASSERT(ms_pkAnimatedFactory);
	Root::getSingleton().addMovableObjectFactory(ms_pkAnimatedFactory);
	ms_pkComponentFactory = OGRE_NEW FvComponentEntityFactory();
	FV_ASSERT(ms_pkComponentFactory);
	Root::getSingleton().addMovableObjectFactory(ms_pkComponentFactory);
}
//----------------------------------------------------------------------------
void FvAnimatedCharacter::Term()
{
	Root* pkRoot = Root::getSingletonPtr();
	if(pkRoot)
	{
		pkRoot->removeMovableObjectFactory(ms_pkAnimatedFactory);
		pkRoot->removeMovableObjectFactory(ms_pkComponentFactory);
	}

	OGRE_DELETE ms_pkAnimatedFactory;
	ms_pkAnimatedFactory = NULL;
	OGRE_DELETE ms_pkComponentFactory;
	ms_pkComponentFactory = NULL;

	FvAnimationSetupResourceManager::Destory();
	FvAnimatedCharacterResourceManager::Destory();
}
//----------------------------------------------------------------------------
void FvAnimatedCharacter::SetScale(const char* pcGroup, float fScale)
{
	FvStringMap<FvUInt32>::iterator it = m_spAnimationSetup->m_kGroupMap.find(pcGroup);

	if(it != m_spAnimationSetup->m_kGroupMap.end())
	{
		FvAnimationSetupResource::Group& kGroup = m_spAnimationSetup->m_pkGroups[it->second];
		for(FvUInt32 i(0); i < kGroup.m_u32AnimationNumber; ++i)
		{
			FvUInt32 u32Anim = GetAnimation(kGroup.m_pkAnimationNames[i].c_str());
			FvAnimationState* pkState = GetAnimationState(u32Anim);
			if(pkState)
			{
				pkState->SetTimeScale(fScale);
			}
		}
	}
}
//----------------------------------------------------------------------------
FvAnimatedCharacterBase::BindingProvider::BindingProvider()
{
	FvZeroMemory(m_apkNodes, sizeof(m_apkNodes));
}
//----------------------------------------------------------------------------
FvAnimatedCharacterBase::BindingProvider::~BindingProvider()
{
	if(m_spProvider)
	{
		m_spProvider->Detach0();
		m_spProvider->Detach1();
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::BindingProvider::UpdateBinding(
	FvUInt32 u32Binding)
{
	Ogre::TagPoint* pkTag = m_apkNodes[u32Binding];
	FV_ASSERT(pkTag);

	if(m_spProvider)
	{
		m_spProvider->Attach0(pkTag->_getDerivedPosition());
		m_spProvider->Attach1(pkTag->_getDerivedOrientation());
	}	
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::BindingProvider::LocalUpdateBinding(
	FvUInt32 u32Binding)
{
	Ogre::TagPoint* pkTag = m_apkNodes[u32Binding];
	FV_ASSERT(pkTag);

	if(m_spProvider)
	{
		m_spProvider->Attach0(pkTag->getParent()->_getDerivedPosition());
		m_spProvider->Attach1(pkTag->getParent()->_getDerivedOrientation());
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::BindingProvider::ForcedUpdate(
	FvUInt32 u32Binding)
{
	Ogre::TagPoint* pkTag = m_apkNodes[u32Binding];
	FV_ASSERT(pkTag);

	pkTag->_update(false, true);
}
//----------------------------------------------------------------------------
const FvAnimatedCharacterBase::ProviderPtr&
	FvAnimatedCharacterBase::ReferenceProvider(const char* pcName)
{
	FV_ASSERT(pcName);
	BindingProviderMap::iterator it = m_kProviderMap.find(pcName);

	if(it == m_kProviderMap.end())
	{
		BindingProvider* pkBindingProvider = new BindingProvider();
		for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
		{
			pkBindingProvider->m_apkNodes[i] = m_ppkAnimatedEntities[i]->RefTagPoint(pcName, m_spResource->m_kBoneAlias);
			FV_ASSERT(pkBindingProvider->m_apkNodes[i]);
		}
		pkBindingProvider->m_spProvider = new Provider();
		pkBindingProvider->UpdateBinding(m_u32ActiveBinding);
		m_kProviderMap.insert(BindingProviderMap::value_type(pcName, pkBindingProvider));
		return pkBindingProvider->m_spProvider;
	}
	else
	{
		return it->second->m_spProvider;
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::ReleaseProvider(const char* pcName)
{
	FV_ASSERT(pcName);
	
	BindingProviderMap::iterator it = m_kProviderMap.find(pcName);
	if(it != m_kProviderMap.end())
	{
		FV_ASSERT(it->second);
		for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
		{
			m_ppkAnimatedEntities[i]->FreeTagPoint(it->second->m_apkNodes[i]);
		}
		delete (it->second);
		m_kProviderMap.erase(it);
	}
}
//----------------------------------------------------------------------------
const FvAnimatedCharacterBase::ProviderPtr&
	FvAnimatedCharacterBase::ReferenceLocalProvider(const char* pcName)
{
	FV_ASSERT(pcName);
	BindingProviderMap::iterator it = m_kLocalProviderMap.find(pcName);

	if(it == m_kLocalProviderMap.end())
	{
		BindingProvider* pkBindingProvider = new BindingProvider();
		for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
		{
			pkBindingProvider->m_apkNodes[i] = m_ppkAnimatedEntities[i]->RefTagPoint(pcName, m_spResource->m_kBoneAlias);
			FV_ASSERT(pkBindingProvider->m_apkNodes[i]);
		}
		pkBindingProvider->m_spProvider = new Provider();
		pkBindingProvider->LocalUpdateBinding(m_u32ActiveBinding);
		m_kLocalProviderMap.insert(BindingProviderMap::value_type(pcName, pkBindingProvider));
		return pkBindingProvider->m_spProvider;
	}
	else
	{
		return it->second->m_spProvider;
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::ReleaseLocalProvider(const char* pcName)
{
	FV_ASSERT(pcName);

	BindingProviderMap::iterator it = m_kLocalProviderMap.find(pcName);
	if(it != m_kLocalProviderMap.end())
	{
		FV_ASSERT(it->second);
		for(FvUInt32 i(0); i < m_u32BindingNumber; ++i)
		{
			m_ppkAnimatedEntities[i]->FreeTagPoint(it->second->m_apkNodes[i]);
		}
		delete (it->second);
		m_kLocalProviderMap.erase(it);
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::AddCallback(Callback& kCallback)
{
	m_kCallbackList.AttachBack(kCallback);
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::UpdataTagPoints()
{
	for(BindingProviderMap::iterator it = m_kProviderMap.begin();
		it != m_kProviderMap.end(); ++it)
	{
		FV_ASSERT(it->second);
		it->second->ForcedUpdate(m_u32ActiveBinding);
	}

	for(BindingProviderMap::iterator it = m_kLocalProviderMap.begin();
		it != m_kLocalProviderMap.end(); ++it)
	{
		FV_ASSERT(it->second);
		it->second->ForcedUpdate(m_u32ActiveBinding);
	}

	m_kCallbackList.BeginIterator();
	while (!m_kCallbackList.IsEnd())
	{
		FvAnimatedCharacterBase::Callback* obj = static_cast<FvAnimatedCharacterBase::Callback*>(m_kCallbackList.GetIterator());
		m_kCallbackList.Next();
		FV_ASSERT(obj);
		///<使用>
		obj->CallbackBeforeRender();
		///</使用>
	}
}
//----------------------------------------------------------------------------
void FvAnimatedCharacterBase::AddToRenderQueue(Ogre::RenderQueue* pkQueue)
{
	m_ppkAnimatedEntities[m_u32ActiveBinding]->_updateRenderQueue(pkQueue);
}
//----------------------------------------------------------------------------
