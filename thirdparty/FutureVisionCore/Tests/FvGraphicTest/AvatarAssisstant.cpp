#include "AvatarAssisstant.h"
//#include "ScriptEntityDefine.h"
#include "ScriptGameData.h"
//
//#include "ScriptItemDefine.h"
#include "ScriptGameData.h"
//#include "ScriptDataHelper.h"

//#include <GxDeriveProvider.h>
#include "GxVisualBuilder.h"
//#include <GxSoundListenTracker.h>

#include <FvGameTimer.h>
#include <FvScriptConstValue.h>
//#include <FvLogicDebug.h>
#include <FvZoneManager.h>
#include "FvZoneTest.h"

FvUInt64 ClientAvatarNode::GenKey()
{
	static FvUInt64 S_IDX = 1;
	++S_IDX;
	return S_IDX;
}
////----------------------------------------------------------------------------
ClientAvatarNode::ClientAvatarNode()
:m_rpInfo(NULL)
,m_rpBody(NULL)
,m_opRibbonTrail(NULL)

#pragma warning(push)
#pragma warning(disable: 4355)
,m_kTickNode(*this)
#pragma warning(pop)
,TEST_FLAG(true)
{
}
ClientAvatarNode::~ClientAvatarNode()
{
	FV_SAFE_DELETE(m_opRibbonTrail);
}
bool ClientAvatarNode::LoadFinishAll()
{
	GxAvatarNode::LoadFinishAll();
	return TEST_FLAG;
}
void Replace(std::string& str, char replaced, const char* replace)
{
	const char* pStar = strchr(str.c_str(), replaced);
	if(pStar)
	{
		int iPos = pStar - str.c_str();
		FvString result = str.substr(0, iPos) + replace + str.substr(iPos+1, str.length()-iPos-1);
		str = result;
	}
}

const char* ClientAvatarNode::GetResName(const char* pcRes, const FvUInt32 uiGender)
{
	static std::string kRes;
	kRes.clear();
	kRes.append(pcRes);
	if(uiGender != 2)
	{
		Replace(kRes, '?', "Male");
		Replace(kRes, '*', "M");
	}
	else
	{
		Replace(kRes, '?', "Female");
		Replace(kRes, '*', "F");
	}
	return kRes.c_str();
}
void ClientAvatarNode::SetLinkState(const _LinkState uiState)
{
	m_eState = uiState;
	FV_ASSERT_WARNING(m_rpBody);
	FV_CHECK_RETURN(m_rpBody);
	const VisualBody& kInfo = *m_rpBody;
	static char S_STR[1024 + 1] = {0};
	for (FvUInt32 uiIdx = 0, uiActiveIdx = 0; uiIdx < VisualBody::MAX_LINK; ++uiIdx)
	{
		const GxVisualData::BodyLink& kLinkInfo = kInfo.m_kLinkDisplay[uiIdx];
		if(kLinkInfo.IsEmpty())
		{
			continue;
		}
		LinkEntityData& pkLinkEntityData = m_kLinkEntityDatas[uiActiveIdx];
		sprintf_s(S_STR, 1024, "ClientAvatarNode::SetLinkState%d", ClientAvatarNode::GenKey());
		m_kNames[uiIdx].append(S_STR);
		pkLinkEntityData.m_apcNames[0] = m_kNames[uiIdx].c_str();
		pkLinkEntityData.m_apcMeshes[0] = kLinkInfo.m_pcMesh;
		pkLinkEntityData.m_apcMaterials[0] = kLinkInfo.m_pcMaterial;
		if(uiState == LINK_1)
		{
			pkLinkEntityData.m_pcNameOfLink = kLinkInfo.m_LinkPos_1;
		}
		else
		{
			pkLinkEntityData.m_pcNameOfLink = kLinkInfo.m_LinkPos_2;
		}
	}
	RefreshLink();
}
void ClientAvatarNode::Load(const VisualItem& kInfo, FvAnimatedCharacter& kAvatar, const FvUInt32 uiGender, const _LinkState uiState /* = LINK_1 */)
{
	FV_ASSERT(m_rpInfo == NULL && "不能初始化两次");
	m_rpInfo = &kInfo;
	m_rpBody = ScriptGameData::GetData<VisualBody>(kInfo.m_iBody_0);
	if(m_rpBody == NULL)
	{
		FvLogBus::Warning("没有对应的Body资源");
		return;
	}
	Load(*m_rpBody, kAvatar, uiState);
}
void ClientAvatarNode::Load(const VisualBody& kInfo, FvAnimatedCharacter& kAvatar, const _LinkState uiState /* = LINK_1 */)
{
	m_rpBody = &kInfo;
	static char S_STR[1024 + 1] = {0};
	m_kPartEntityDatas.resize(m_rpBody->GetPartSize());
	m_kLinkEntityDatas.resize(m_rpBody->GetWeaponSize());
	m_kNames.resize(VisualBody::MAX_PART + VisualBody::MAX_LINK, "");
	m_kMeshs.resize(VisualBody::MAX_PART + VisualBody::MAX_LINK, "");
	m_kMaterials.resize(VisualBody::MAX_PART + VisualBody::MAX_LINK, "");
	for (FvUInt32 uiIdx = 0, uiActiveIdx = 0; uiIdx < VisualBody::MAX_PART; ++uiIdx)
	{
		const GxVisualData::BodyPart& kPartInfo = m_rpBody->m_kPartDisplay[uiIdx];
		if(kPartInfo.IsEmpty())
		{
			continue;
		}
		PartEntityData& pkPartEntityData = m_kPartEntityDatas[uiActiveIdx];
		sprintf_s(S_STR, 1024, "ClientAvatarNode::Load%d", ClientAvatarNode::GenKey());
		m_kNames[uiIdx].append(S_STR);
		pkPartEntityData.m_apcNames[0] = m_kNames[uiIdx].c_str();
		m_kMeshs[uiIdx].append(kPartInfo.m_pcMesh);
		pkPartEntityData.m_apcMeshes[0] = m_kMeshs[uiIdx].c_str();
		m_kMaterials[uiIdx].append(kPartInfo.m_pcMaterial);
		pkPartEntityData.m_apcMaterials[0] = m_kMaterials[uiIdx].c_str();
		pkPartEntityData.m_u32Priority = kPartInfo.m_iDisplayerPriority;
		for (FvUInt32 uiIdx = 0; uiIdx < VisualBody::MAX_PART; ++uiIdx)
		{
			if(kPartInfo.m_pcDisplayerPartArray[uiIdx])
			{
				FvInt32 iValue = kPartInfo.m_pcDisplayerPartArray[uiIdx] - 'a';
				FV_ASSERT(iValue >= 0);
				pkPartEntityData.m_kParts.push_back(iValue);
			}
			else
			{
				break;
			}
		}
		++uiActiveIdx;
	}
	for (FvUInt32 uiIdx = 0, uiActiveIdx = 0; uiIdx < VisualBody::MAX_LINK; ++uiIdx)
	{
		FvUInt32 uiLinkIdx = VisualBody::MAX_PART + uiIdx;
		const GxVisualData::BodyLink& kLinkInfo = m_rpBody->m_kLinkDisplay[uiIdx];
		if(kLinkInfo.IsEmpty())
		{
			continue;
		}
		LinkEntityData& pkLinkEntityData = m_kLinkEntityDatas[uiActiveIdx];
		sprintf_s(S_STR, 1024, "ClientAvatarNode::Load%d", ClientAvatarNode::GenKey());
		m_kNames[uiLinkIdx].append(S_STR);
		pkLinkEntityData.m_apcNames[0] = m_kNames[uiLinkIdx].c_str();
		pkLinkEntityData.m_apcMeshes[0] = kLinkInfo.m_pcMesh;
		pkLinkEntityData.m_apcMaterials[0] = kLinkInfo.m_pcMaterial;
		if(uiState == LINK_1)
			pkLinkEntityData.m_pcNameOfLink = kLinkInfo.m_LinkPos_1;
		else
			pkLinkEntityData.m_pcNameOfLink = kLinkInfo.m_LinkPos_2;

	}
}
void ClientAvatarNode::Load(FvAnimatedCharacter& kAvatar, const char* pcName, const char* pcMesh, const char* pcMaterial)
{
	m_kLinkEntityDatas.resize(1);
	m_kNames.clear();
	m_kNames.push_back(pcName);
	m_kLinkEntityDatas[0].m_apcNames[0] = m_kNames[0].c_str();
	m_kMeshs.clear();
	m_kMeshs.push_back(pcMesh);
	m_kLinkEntityDatas[0].m_apcMeshes[0] = m_kMeshs[0].c_str();
	m_kMaterials.clear();
	m_kMaterials.push_back(pcMaterial);
	m_kLinkEntityDatas[0].m_apcMaterials[0] = m_kMaterials[0].c_str();
	m_kLinkEntityDatas[0].m_pcNameOfLink = "";
}

//+----------------------------------------------------------------------------------------------------
void ClientAvatarNode::StartRibbonTrail(FvAvatarAttachment& kAvatarAttachment, FvAnimatedCharacter& kAvatar)
{
	if(m_opRibbonTrail == NULL )
	{
		return;
	}
	const GxVisualData::RibbonTail& kRibbonTailInfo = m_rpBody->m_kRibbonTail[0];
	if(!(kRibbonTailInfo.m_pcTrackPart && kRibbonTailInfo.m_pcTrackPart[0]))
	{
		return;
	}
	FvGameTimer::SimpleTaskList().AttachBack(m_kTickNode);
	kAvatarAttachment.AttachBack(*m_opRibbonTrail);
	m_spTrackBall = kAvatar.ReferenceProvider(kRibbonTailInfo.m_pcTrackPart);
}
void ClientAvatarNode::EndRibbonTrail()
{
	m_kTickNode.Detach();
	if(m_opRibbonTrail)
	{
		m_opRibbonTrail->Reset();
		m_opRibbonTrail->Detach();
	}
}
void ClientAvatarNode::InitRibbonTrail()
{
	FV_ASSERT_WARNING(m_rpBody);
	if(m_rpBody && m_rpBody->m_kRibbonTail[0].m_iPosCnt > 1)
	{
		const GxVisualData::RibbonTail& kRibbonTailInfo = m_rpBody->m_kRibbonTail[0];
		if(m_opRibbonTrail == NULL)
		{
			m_opRibbonTrail = new FvRibbonTrail(30,kRibbonTailInfo.m_iPosCnt - 1,0.2f);
		}
		m_opRibbonTrail->SetMaterialName(m_rpBody->m_kRibbonTail[0].m_pcNormalRes, m_rpBody->m_kRibbonTail[0].m_pcHDRRes);
	}
}
void ClientAvatarNode::_OnRibbonTrailTick(const float fDeltaTime)
{
	FV_ASSERT_WARNING(m_opRibbonTrail && m_spTrackBall);
	if(m_spTrackBall == NULL)
	{
		return;
	}
	FvVector3 akElement[GxVisualData::RibbonTail::MAX_TRACK_CNT];
	const GxVisualData::RibbonTail& kRibbonTailInfo = m_rpBody->m_kRibbonTail[0];
	for (FvUInt32 uiIdx = 0; uiIdx < kRibbonTailInfo.m_iPosCnt; ++uiIdx)
	{
		const Ogre::Vector3 kPos(kRibbonTailInfo.m_iTrackPos[uiIdx].x*0.01f, kRibbonTailInfo.m_iTrackPos[uiIdx].y*0.01f, kRibbonTailInfo.m_iTrackPos[uiIdx].z*0.01f);
		akElement[uiIdx] = *(FvVector3*)&(m_spTrackBall->GetData1() * kPos + m_spTrackBall->GetData0());
	}
	FvVector3* apkElement[GxVisualData::RibbonTail::MAX_TRACK_CNT] = 
	{	&akElement[0], 
		&akElement[1], 
		&akElement[2], 
		&akElement[3], 
		&akElement[4], 
		&akElement[5], 
		&akElement[6], 
		&akElement[7], 
		&akElement[8], 
		&akElement[9]
	};

	m_opRibbonTrail->AddNewElement(apkElement);
}

//+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



ClientGameUnitAvatar::ClientGameUnitAvatar()
:m_opDefaultBody(NULL)

#pragma warning(push)
#pragma warning(disable: 4355)
//,m_kPlayerRotateCursor(*this)
#pragma warning(pop)
{
	
}
ClientGameUnitAvatar::~ClientGameUnitAvatar()
{
}
//+------------------------------------------------------------------------------------------------------------------------------------------------------------
void ClientGameUnitAvatar::SetPartSize(const FvUInt32 uiSize)
{
	m_PartList.resize(uiSize, NULL);
}
void ClientGameUnitAvatar::SetLinkState(const GxAvatarNode::_LinkState eState)
{
	m_eLinkState = eState;
	for (std::vector<ClientAvatarNode*>::iterator iter = m_PartList.begin(); iter != m_PartList.end(); ++iter)
	{
		ClientAvatarNode* pkNode = (*iter);
		if(pkNode)
		{
			pkNode->SetLinkState(eState);
		}
	}
	for (std::list<ClientAvatarNode*>::iterator iter = m_BodyVisualList.begin(); iter != m_BodyVisualList.end(); ++iter)
	{
		ClientAvatarNode* pkNode = (*iter);
		if(pkNode)
		{
			pkNode->SetLinkState(eState);
		}
	}
}
ClientAvatarNode* ClientGameUnitAvatar::CreatePartNode(const FvUInt32 uiPart)
{
	if(m_PartList.size() <= uiPart)
	{
		FvLogBus::Warning("FvAvatarAssisstant::CreatePartNode 异常 PartSize[%d], Part[%d]", m_PartList.size(), uiPart);
		return NULL;
	}
	ClientAvatarNode* pNEwPart = new ClientAvatarNode();
	if(pNEwPart)
	{
		if(m_PartList[uiPart])
		{
			pNEwPart->AttachOldNode(m_PartList[uiPart]);
		}		
		m_PartList[uiPart] = pNEwPart;
	}
	return pNEwPart;
}
//+------------------------------------------------------------------------------------------------------------------------------------------------------------
void ClientGameUnitAvatar::StartRibbonTail()
{
	if(m_rpAvatar && GetAvatar())
	{
		for (std::vector<ClientAvatarNode*>::const_iterator iter = m_PartList.begin(); iter != m_PartList.end(); ++iter)
		{
			ClientAvatarNode* pkNode = (*iter);
			if(pkNode)
			{
				pkNode->StartRibbonTrail(*m_rpAvatar, *GetAvatar());
			}
		}
	}
}
void ClientGameUnitAvatar::EndRibbonTail()
{
	for (std::vector<ClientAvatarNode*>::const_iterator iter = m_PartList.begin(); iter != m_PartList.end(); ++iter)
	{
		ClientAvatarNode* pkNode = (*iter);
		if(pkNode)
		{
			pkNode->EndRibbonTrail();
		}
	}
}
//+------------------------------------------------------------------------------------------------------------------------------------------------------------
ClientAvatarNode* ClientGameUnitAvatar::_DisplayItemVisual(FvAnimatedCharacter& kAvatar, const VisualItem& kInfo, const FvUInt32 uiGender)
{
	ClientAvatarNode* pkNode = new ClientAvatarNode();
	pkNode->Load(kInfo, kAvatar, uiGender, m_eLinkState);
	pkNode->TEST_FLAG = false;
	kAvatar.PutOn(*pkNode);
	if(m_rpAvatar && GetAvatar())
	{
		pkNode->InitRibbonTrail();
	}
	return pkNode;
}	
ClientAvatarNode* ClientGameUnitAvatar::_DisplayBodyVisual(FvAnimatedCharacter& kAvatar, const VisualBody& kInfo)
{
	ClientAvatarNode* pkNode = new ClientAvatarNode();
	pkNode->Load(kInfo, kAvatar, m_eLinkState);
	pkNode->TEST_FLAG = false;
	kAvatar.PutOn(*pkNode);
	if(m_rpAvatar && GetAvatar())
	{
		pkNode->InitRibbonTrail();
	}
	
	return pkNode;
}
void ClientGameUnitAvatar::ClearAllParts()
{
	for(std::vector<ClientAvatarNode*>::iterator iter = m_PartList.begin(); iter != m_PartList.end(); ++iter)
	{
		ClientAvatarNode* pkPartNode = (*iter);
		if(pkPartNode)
		{
			delete pkPartNode;
		}
	}
	m_PartList.clear();
	for(std::list<ClientAvatarNode*>::iterator iter = m_BodyVisualList.begin(); iter != m_BodyVisualList.end(); ++iter)
	{
		ClientAvatarNode* pkPartNode = (*iter);
		if(pkPartNode)
		{
			delete pkPartNode;
		}
	}
	m_BodyVisualList.clear();
}
//+------------------------------------------------------------------------------------------------------------------------------------------------------------
void ClientGameUnitAvatar::SetBody(const FvUInt32 uiBodyID, const FvUInt32 uiGender)
{
	const VisualItem* pkItem = ScriptGameData::GetData<VisualItem>(uiBodyID);
	if(GetAvatar() && pkItem)
	{
		m_opDefaultBody = _DisplayItemVisual(*GetAvatar(), *pkItem, uiGender);
	}
}
void ClientGameUnitAvatar::SetBody(const FvUInt32 uiBodyID)
{
	const VisualBody* pkBody = ScriptGameData::GetData<VisualBody>(uiBodyID);
	if(GetAvatar() && pkBody)
	{
		m_opDefaultBody = _DisplayBodyVisual(*GetAvatar(), *pkBody);
	}
}
void ClientGameUnitAvatar::AddBody(const FvUInt32 uiBodyID)
{
	const VisualBody* pkBody = ScriptGameData::GetData<VisualBody>(uiBodyID);
	if(GetAvatar() && pkBody)
	{
		ClientAvatarNode* pkNode = _DisplayBodyVisual(*GetAvatar(), *pkBody);
		if(pkNode)
		{
			m_BodyVisualList.push_back(pkNode);
		}
	}
}
void ClientGameUnitAvatar::AddItem(const FvUInt32 uiBodyID, const FvUInt32 uiGender)
{
	const VisualItem* pkItem = ScriptGameData::GetData<VisualItem>(uiBodyID);
	if(GetAvatar() && pkItem)
	{
		ClientAvatarNode* pkNode = _DisplayItemVisual(*GetAvatar(), *pkItem, uiGender);
		if(pkNode)
		{
			m_BodyVisualList.push_back(pkNode);
		}
	}
}

void ClientGameUnitAvatar::DisplayEquipOnItem(const FvUInt32 uiSlot, const VisualItem& kInfo, const FvUInt32 uiGender)
{
	FvAnimatedCharacter* pkAvatar = GetAvatar();
	if (pkAvatar == NULL)
	{
		return;
	}
	if(m_PartList[uiSlot] && m_PartList[uiSlot]->Info() == &kInfo)
	{
		return;
	}
	ClientAvatarNode* pkNode = _DisplayItemVisual(*pkAvatar, kInfo, uiGender);
	if(pkNode)
	{
		pkNode->AttachOldNode(m_PartList[uiSlot]);
		m_PartList[uiSlot] = pkNode;
	}	
}
void ClientGameUnitAvatar::DisplayEquipOffItem(const FvUInt32 uiSlot)
{
#pragma comment(qiuyao)
	//FV_ASSERT(uiSlot < EquipSlotDefine::TOTAL);
	ClientAvatarNode* pkNode = m_PartList[uiSlot];
	if(pkNode == NULL || pkNode->Info() == NULL)
	{
		return;
	}
	delete pkNode;
	m_PartList[uiSlot] = NULL;
}
//+------------------------------------------------------------------------------------------------------------------------------------------------------------
void ClientGameUnitAvatar::Clear()
{
	//EndAllAudio();
	EndCast();
	ClearAllParts();
	EndCast();
	//ClearAllEffectController();
	m_rpAvatar = NULL;
	FV_SAFE_DELETE(m_opDefaultBody);
}


void AvatarFinishLoadCallBack::CallBack()
{
	if(m_rpListener)
	{
		m_rpListener->OnAvatarFinishLoad();
	}
}