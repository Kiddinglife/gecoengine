#pragma once

#include <FvMiscData.h>
#include <FvVector3.h>

#define _DONT_COPY_(CLASS) \
	FV_DECLARE_COPY(CLASS);\
	FV_DECLARE_COMPARE(CLASS);\
	FV_DECLARE_COPY_CONSTRUCTOR(CLASS);

template <FvUInt32 CNT>
class MiscStrArray
{
public:
	~MiscStrArray(){}

	bool GetName(const FvUInt32 uiIdx, std::string& kName)const
	{
		if(uiIdx >= CNT)
		{
			return false;
		}
		const char* pcData = m_strData[uiIdx];
		if(pcData && pcData[0])
		{
			kName = pcData;
			return true;
		}
		else
		{
			return false;
		}
	}
	const char* Name(const FvUInt32 uiIdx, const char* strDefault = "")const
	{
		if(uiIdx >= CNT)
		{
			return strDefault;
		}
		const char* pcData = m_strData[uiIdx];
		if(pcData && pcData[0])
		{
			return pcData;
		}
		else
		{
			return strDefault;
		}
	}
	void Print(std::string& kOutput)const
	{
	}
private:
	char* m_strData[CNT];
};



namespace GxVisualData
{
	struct Audio
	{
		Audio():m_iDuration(0), m_iScale(0), m_pcRes(NULL){}
		bool IsEmpty()const{ return (m_iDuration == 0 || m_pcRes[0] == 0); }

		FvInt32 m_iDelayTime;
		FvInt32 m_iDuration;
		FvInt32 m_iScale;
		char* m_pcRes;
	private:
		_DONT_COPY_(Audio);
	};

	struct Anim
	{
		Anim():m_iDuration(0), m_iScale(0), m_pcAnim(NULL){}
		bool IsEmpty()const{ return (m_iDuration == 0 || m_pcAnim[0] == 0); }

		FvInt32 m_iDuration;
		FvInt32 m_iScale;
		char* m_pcAnim;
		FvInt32 m_iType;///<AnimType>
	private:
		_DONT_COPY_(Anim);
	};
	struct AvatarColor
	{
		bool IsEmpty()const 
		{
			return (m_pcStart == NULL || m_pcStart[0] == 0) 
			|| (m_pcContinue == NULL || m_pcContinue[0] == 0)
			|| (m_pcEnd == NULL || m_pcEnd[0] == 0);
		}
		FvInt32 m_iType;///<ClientAvatarRenderType::Idx>
		char* m_pcStart;
		char* m_pcContinue;
		char* m_pcEnd;
	};

	struct AttachExpress
	{
		AttachExpress():m_iDelayTime(0), m_iDuration(0), m_iScale(0), m_pcRes(NULL), m_uiAttachPos(0), m_uiAttachType(0){}
		bool IsEmpty()const{ return (m_pcRes[0] == 0); }

		FvInt32 m_iDelayTime;
		FvInt32 m_iDuration;
		FvInt32 m_iScale;
		char* m_pcRes;
		FvInt32 m_uiAttachPos;
		FvInt32 m_uiAttachType;///<ExpressAttachType>
	private:
		_DONT_COPY_(AttachExpress);
	};


	struct TravelExpress
	{
		TravelExpress():m_iTrackPart(0), m_iScale(0), m_iGravity(0), m_pcRes(NULL){}
		bool IsEmpty()const{ return (m_pcRes[0] == 0); }

		FvInt32 m_iStartPart;
		FvInt32 m_iTrackPart;
		FvInt32 m_iScale;
		FvInt32 m_iGravity;
		FvInt32 m_iReserveTime;///! 残留时间
		FvInt32 m_iReserve_1;
		FvInt32 m_iReserve_2;
		FvInt32 m_iReserve_3;
		char* m_pcRes;
		Audio m_kAudio;
	private:
		_DONT_COPY_(TravelExpress);
	};

	struct RibbonTail
	{
		static const FvUInt32 MAX_TRACK_CNT = 10;
		RibbonTail():m_pcTrackPart(NULL), m_iPosCnt(0), m_iType(0), m_pcNormalRes(NULL), m_pcHDRRes(NULL){}
		struct Vector32
		{
			FvInt32 x, y, z; 
		};

		char* m_pcTrackPart;
		FvUInt32 m_iPosCnt;
		Vector32 m_iTrackPos[MAX_TRACK_CNT];
		FvInt32 m_iType;
		char* m_pcNormalRes;
		char* m_pcHDRRes;

	private:
		_DONT_COPY_(RibbonTail);
	};

	struct SpellProc
	{
		static const FvUInt32 MAX_ANIM = 3;
		static const FvUInt32 MAX_PART = 10;
		static const FvUInt32 MAX_SOUND = 3;

		SpellProc():m_iCameraAction(0){}

		Anim m_kAnims[MAX_ANIM];
		AttachExpress m_kAttachExpress[MAX_PART];
		Audio m_kAudio[MAX_SOUND];
		FvInt32 m_iCameraAction;

	private:
		_DONT_COPY_(SpellProc);
	};

	//>----------------------------------------------------------------------------
	struct SpellFailed
	{
		SpellFailed():m_pcMessage(NULL){}
		char* m_pcMessage;
	private:
		_DONT_COPY_(SpellFailed);
	};


	struct BodyPart
	{
		static const FvUInt32 EXPRESS_CNT = 2;
		struct Express
		{
			char* m_pcEffectRes;
			char* m_pcEffectPos;
			FvInt32 m_iScale;
			FvInt32 m_uiAttachType;///<ExpressAttachType>
		};

		BodyPart():m_iDisplayerPriority(0), m_pcDisplayerPartArray(NULL), m_pcMesh(NULL), m_pcMaterial(NULL){}
		bool IsEmpty()const	{return !(m_pcMaterial && m_pcMaterial[0] != 0);}

		FvInt32 m_iDisplayerPriority;//! 
		char* m_pcDisplayerPartArray;//! 显示部位序列
		char* m_pcMesh;
		char* m_pcMaterial;//! 只能有一张贴图		FvInt32 m_iScale;

		Express m_kExpress[EXPRESS_CNT];

	private:
		_DONT_COPY_(BodyPart);
	};
	struct BodyLink
	{
		static const FvUInt32 EXPRESS_CNT = 2;
		struct Express
		{
			char* m_pcEffectRes;
			char* m_pcEffectPos;
			FvInt32 m_iScale;
			FvInt32 m_uiAttachType;///<ExpressAttachType>
		};

		BodyLink():m_LinkPos_1(NULL), m_LinkPos_2(NULL), m_pcMesh(NULL), m_pcMaterial(NULL){}
		bool IsEmpty()const	{return !(m_pcMaterial && m_pcMaterial[0] != 0);}

		char* m_LinkPos_1;//! 
		char* m_LinkPos_2;//! 显示部位序列
		char* m_pcMesh;
		char* m_pcMaterial;//! 只能有一张贴图		FvInt32 m_iScale;

		Express m_kExpress[EXPRESS_CNT];

	private:
		_DONT_COPY_(BodyLink);
	};

	struct NPCBody
	{
		NPCBody():m_pcMesh(NULL), m_pcMaterial(NULL){}
		bool IsEmpty()const	{return !(m_pcMaterial && m_pcMaterial[0] != 0);}

		char* m_pcMesh;
		char* m_pcMaterial;//! 只能有一张贴图
	private:
		_DONT_COPY_(NPCBody);
	};

	struct NPCWeapon
	{
		NPCWeapon():m_iVisual(0), m_iAnimSet(0)	{}
		bool IsEmpty()const	{return (m_iVisual == 0);}

		FvInt32 m_iVisual;///<VisualBody>
		FvInt32 m_iAnimSet;
		FvInt32 m_iReserve_0;
		FvInt32 m_iReserve_1;
		FvInt32 m_iReserve_2;
	private:
		_DONT_COPY_(NPCWeapon);
	};
};



struct GxVisualSpellInfoStruct 
{
	static const FvUInt32 MAX_TRAVEL = 3;
	static const FvUInt32 MAX_PROC = 3;
	static const FvUInt32 MISC_VALUE_MAX = 5;

	GxVisualSpellInfoStruct():m_iIdx(0), m_pcName(NULL), m_pcBook(NULL), m_pcTipDescription(NULL), m_pcIcon_1(NULL), m_pcIcon_2(NULL)
	{}

	FvInt32 m_iIdx;
	char* m_pcName;
	char* m_pcBook;
	char* m_pcTipDescription;
	char* m_pcIcon_1;
	char* m_pcIcon_2;

	GxVisualData::SpellFailed m_kFailMsg;
	GxVisualData::SpellProc m_kProc[MAX_PROC];
	GxVisualData::TravelExpress m_kTravelEpxress[MAX_TRAVEL];
	char* m_pcScreenEffect;
	FvInt32 m_iWeaponState;///<SpellWeaponState::Idx>
	FvInt32 m_iReseve[10];
	MiscDataArray<MISC_VALUE_MAX> m_kMiscValues;

private:
	_DONT_COPY_(GxVisualSpellInfoStruct);
};

struct GxVisualAura
{
	static const FvUInt32 MAX_ANIM = 3;
	static const FvUInt32 MAX_PART = 10;
	static const FvUInt32 MAX_SOUND = 3;
	static const FvUInt32 MISC_VALUE_MAX = 5;
	static const FvUInt32 SCRIPT_DURATION_EFFECT = 10;

	GxVisualAura()
	{
		memset(this, 0, sizeof(GxVisualAura));
	}

	FvInt32 m_iIdx;
	FvInt32 m_iReserve_0;
	FvInt32 m_iWeight;
	FvInt32 m_iReserve_1;
	FvInt32 m_iState;
	FvInt32 m_iDisplayerType;///<AuraDiplayType::Idx>

	FvInt32 m_iReserve_2;
	FvInt32 m_iReserve_3;

	GxVisualData::AttachExpress m_kStartExpress[MAX_PART];
	GxVisualData::AttachExpress m_kDurExpress[MAX_PART];
	GxVisualData::AttachExpress m_kEndExpress[MAX_PART];

	GxVisualData::Audio m_kStartAudio;
	GxVisualData::Audio m_kDurAudio;
	GxVisualData::Audio m_kEndAudio;

	FvInt32 m_iReserve_4;
	FvInt32 m_iScriptDurationEffect[SCRIPT_DURATION_EFFECT];
	FvInt32 m_iCameraAction;

private:
	_DONT_COPY_(GxVisualAura);
};

struct GxVisualHitEffect
{
	static const FvUInt32 MAX_ANIM = 3;
	static const FvUInt32 MAX_PART = 10;
	static const FvUInt32 MAX_SOUND = 3;
	static const FvUInt32 MISC_VALUE_MAX = 5;

	GxVisualHitEffect():m_iIdx(0), m_iCameraAction(0){}

	FvInt32 m_iIdx;
	FvInt32 m_iChanel;
	FvInt32 m_iWeight;
	FvInt32 m_iColor;

	GxVisualData::Anim m_kAnims[MAX_ANIM];
	GxVisualData::AttachExpress m_kAttachExpress[MAX_PART];
	GxVisualData::Audio m_kAudio[MAX_SOUND];

	char* m_pcScreenEffect;
	MiscDataArray<MISC_VALUE_MAX> m_kMiscValues;

	FvInt32 m_iCameraAction;
private:
	_DONT_COPY_(GxVisualHitEffect);
};

struct GxDeriveProviderInfo_1
{
	FvVector3 m_kOffest;
	FvString m_kRefName;
};

struct GxDeriveProviderInfo_2
{
	float m_fWeight;
	FvString m_kRefName_1;
	FvString m_kRefName_2;
};

struct GxVisualDurationEffectInfo
{
	static const FvUInt32 MISC_VALUE_MAX = 10;
	static const FvUInt32 MISC_STR_MAX = 5;
	FvInt32 m_iIdx;
	FvInt32 m_iType;
	FvInt32 m_iDuration;
	FvInt32 m_iReserve_0;
	FvInt32 m_iReserve_1;
	FvInt32 m_iReserve_2;
	FvInt32 m_iReserve_3;
	FvInt32 m_iReserve_4;
	FvInt32 m_iReserve_5;
	FvInt32 m_iReserve_6;
	MiscDataArray<MISC_VALUE_MAX> m_kMiscValues;
	MiscStrArray<MISC_STR_MAX> m_kMisStrValue;
};


struct GxVisualCameraShakeStruct
{
	FvInt32 m_iIdx;
	FvInt32 m_iType;
	FvInt32 m_iDuration;
	FvInt32 m_iReserve_0;
	FvInt32 m_iReserve_1;
	FvInt32 m_iReserve_2;
	FvInt32 m_iShakeX;
	FvInt32 m_iShakeY;
	FvInt32 m_iShakeZ;
};
