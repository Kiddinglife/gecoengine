#ifndef __SpellVisualStruct_H__
#define __SpellVisualStruct_H__


#include <FvKernel.h>
#include <FvSpellStruct.h>
#include <FvVector3.h>
#include <FvBaseTypes.h>

#include "GxVisualStruct.h"


struct VisualItem
{
	static const FvUInt32 MISC_VALUE_MAX = 10;

	VisualItem()
	{
		memset(this, 0, sizeof(VisualItem));
	}


	FvInt32 m_iIdx;

	char* m_pcName;
	char* m_pcBook;
	char* m_pcTipDescription;
	char* m_pcIcon_1;
	char* m_pcIcon_2;

	FvInt32 m_iBody_0;///<VisualBody::ID>
	FvInt32 m_iBody_1;///<VisualBody::ID>
	FvInt32 m_iBody_2;///<VisualBody::ID>
	FvInt32 m_iBody_3;///<VisualBody::ID>
	FvInt32 m_iBody_4;///<VisualBody::ID>

	FvInt32 m_iUseType;
	FvInt32 m_iDisplayType;

	FvInt32 m_iLinkMapID;
	float m_fLinkX;
	float m_fLinkY;

	FvUInt32 m_uiBagSortType;

	FvInt32 m_iReserve[16];

	MiscDataArray<MISC_VALUE_MAX> m_kMiscValues;

private:
	_DONT_COPY_(VisualItem);
};

struct VisualBody
{
	static const FvUInt32 MAX_PART = 18;
	static const FvUInt32 MAX_LINK = 2;
	static const FvUInt32 RIBBON_TAIL_CNT = 2;

	VisualBody():m_iId(0){}

	FvUInt32 GetPartSize()const
	{
		FvUInt32 uiCnt = 0;
		for (FvUInt32 uiIdx = 0; uiIdx < MAX_PART; ++uiIdx)
		{
			if(m_kPartDisplay[uiIdx].IsEmpty() == false)
			{
				++uiCnt;
			}
		}
		return uiCnt;
	}
	FvUInt32 GetWeaponSize()const
	{
		FvUInt32 uiCnt = 0;
		for (FvUInt32 uiIdx = 0; uiIdx < MAX_LINK; ++uiIdx)
		{
			if(m_kLinkDisplay[uiIdx].IsEmpty() == false)
			{
				++uiCnt;
			}
		}
		return uiCnt;
	}

	FvInt32 m_iId;
	GxVisualData::BodyPart m_kPartDisplay[MAX_PART];
	GxVisualData::BodyLink m_kLinkDisplay[MAX_LINK];
	GxVisualData::RibbonTail m_kRibbonTail[RIBBON_TAIL_CNT];
};


struct VisualWeaponType
{
	VisualWeaponType():m_pcHoldPos(NULL), m_pcBankPos(NULL), m_uiAnimType(0){}
	const char* m_pcHoldPos;
	const char* m_pcBankPos;
	FvInt32 m_uiAnimType;
private:
	_DONT_COPY_(VisualWeaponType);
};

struct NPCVisualInfo
{
	static const FvUInt32 MAX_WEAPON = 2;

	FvInt32 m_iIdx;
	char* m_pcName;
	char* m_pcSubName;
	char* m_pcDiscript;
	FvInt32 m_iVisualEffectScale;
	FvInt32 m_iOpacity;
	FvInt32 m_iBodyProperty;
	GxVisualData::NPCBody m_kBody;
	FvInt32 m_iBodyScale;
	FvInt32 m_iNPCOption;
	FvInt32 m_iReserve_1;
	FvInt32 m_iReserve_2;
	FvInt32 m_iReserve_3;
	FvInt32 m_iWalkReferenceSpeed;
	FvInt32 m_iRunReferenceSpeed;
	FvInt32 m_iWalkRunBorderSpeed;
	FvInt32 m_iRunSpeedSup;
	FvInt32 m_iReserve_4;
	FvInt32 m_iReserve_5;
	FvInt32 m_iReserve_6;
	FvInt32 m_iReserve_7;
	char* m_pcAnimSet;
	FvInt32 m_iTangencyType;
	FvInt32 m_iBodyItemID;
	FvInt32 m_iHeight;
	FvInt32 m_iRotateType;
	FvInt32 m_iLinkMapID;
	float m_fLinkX;
	float m_fLinkY;
	GxVisualData::NPCWeapon m_kWeapon[MAX_WEAPON];

	FvInt32 uiSound;  

	char* m_pcIdleAnim;
	char* m_pcIdleAnim_0;
	FvInt32 m_iIdleProbability_0;
	char* m_pcIdleAnim_1;
	FvInt32 m_iIdleProbability_1;
	char* m_pcIdleAnim_2;
	FvInt32 m_iIdleProbability_2;
	char* m_pcIdleAnim_3;
	FvInt32 m_iIdleProbability_3;
};
//
//struct VisualColor
//{
//	struct AnimList
//	{
//		bool IsEmpty()const {return (m_pcStart == NULL || m_pcStart[0] == 0) 
//			|| (m_pcContinue == NULL || m_pcContinue[0] == 0)
//			|| (m_pcEnd == NULL || m_pcEnd[0] == 0);
//		}
//		FvInt32 m_iType;///<ClientAvatarRenderType::Idx>
//		char* m_pcStart;
//		char* m_pcContinue;
//		char* m_pcEnd;
//	};
//	FvInt32 m_iIdx;
//	AnimList m_kAnim0;
//	AnimList m_kAnim1;
//	FvInt32 m_iDefaultDuration;
//	FvInt32 m_iReserve_0;
//	FvInt32 m_iReserve_1;
//	FvInt32 m_iReserve_2;
//	FvInt32 m_iReserve_3;
//	FvInt32 m_iReserve_4;
//	FvInt32 m_iReserve_5;
//	FvInt32 m_iReserve_6;
//};



struct VisualQuest
{
	FvUInt32 m_uiID;
	char* m_pcTitle;
	char* m_pcObjectives;
	char* m_pcDetails;
	char* m_pcRequestItemsText;
	char* m_pcEndText;
	FvInt32 m_iType;
	FvInt32 m_iReserve_0;
	FvInt32 m_iReserve_1;
	FvInt32 m_iReserve_2;
	FvInt32 m_iReserve_3;
	FvInt32 m_iReserve_4;
	FvInt32 m_iReserve_5;
	FvInt32 m_iReserve_6;
	FvInt32 m_iReserve_7;
	FvInt32 m_iReserve_8;

};


struct VisualCellObject
{
	static const FvUInt32 MISC_VALUE_MAX = 5;

	FvUInt32 m_uiID;
	char* m_pcName;
	char* m_pcIcon;
	char* m_pcMesh;
	char* m_pcMaterial;
	char* m_pcCastBarCaption;
	FvInt32 m_iScale;

	FvInt32 m_iReserve_0;
	FvInt32 m_iReserve_1;
	FvInt32 m_iReserve_2;
	FvInt32 m_iReserve_3;
	FvInt32 m_iReserve_4;
	FvInt32 m_iReserve_5;
	FvInt32 m_iReserve_6;
	FvInt32 m_iReserve_7;
	FvInt32 m_iReserve_8;
	FvInt32 m_iReserve_9;

	MiscDataArray<MISC_VALUE_MAX> m_kMiscValues;
};


struct ScriptUIHint
{
	FvInt32 m_iIdx;
	char* m_pcText;
};


struct ScriptMessage
{
	FvInt32 m_iIdx;
	FvInt32 m_iType;
	FvInt32 m_iFlag;
	char* m_pcEnum;
	char* m_pcText;
	char* m_pcProc0;
	char* m_pcProc1;
	char* m_pcProc2;
	char* m_pcProc3;
	char* m_pcProc4;
	char* m_pcProc5;
	char* m_pcProc6;
	char* m_pcProc7;
};

struct VisualEffectIdx
{
	FvInt32 m_iIdx;
	char* m_pcName;
	FvInt32 m_iEffect;///<GxVisualHitEffect>
};

struct GameObjectDisplayInfo
{
	FvUInt32 m_uiDisplayID;
	char* m_pcName;
	char* m_pcIcon;
	char* m_pcDescription;

	char* m_pcModelResource0;
	GxVisualData::AttachExpress m_kEffect0_0;
	GxVisualData::AttachExpress m_kEffect0_1;
	GxVisualData::Audio m_kAudio_0;

	char* m_pcModelResource1;
	GxVisualData::AttachExpress m_kEffect1_0;
	GxVisualData::AttachExpress m_kEffect1_1;
	GxVisualData::Audio m_kAudio_1;

	char* m_pcModelResource2;
	GxVisualData::AttachExpress m_kEffect2_0;
	GxVisualData::AttachExpress m_kEffect2_1;
	GxVisualData::Audio m_kAudio_2;

	char* m_pcModelResource3;
	GxVisualData::AttachExpress m_kEffect3_0;
	GxVisualData::AttachExpress m_kEffect3_1;
	GxVisualData::Audio m_kAudio_3;

	FvInt32 m_iLinkMapID;
	float m_fLinkX;
	float m_fLinkY;
};

#endif //__SpellVisualStruct_H__

