#include "GxVisualBuilder.h"

#include <FvLogBus.h>
#include <FvExpressManagerInstance.h>
#include <FvZoneManager.h>
#include <FvAttachExpress.h>
#include <FvParticleAttachment.h>

void GxVisualBuilder::Create(const GxVisualData::AttachExpress& kInfo, const FvZoneSpacePtr& spSpace, const FvMotor::TrackBallPtr& spProvider, FvExpressOwnList& kOwner, const float fDelayTime /* = 0.0f */)
{
	FV_ASSERT_WARNING(kInfo.IsEmpty() == false && "请外面保证持续时间不为零");
	if(!spSpace || !kInfo.m_pcRes)
	{
		FvLogBus::Warning("VisualAttachExpress::Create Space为空");
		return;
	}
	static char pcName[1024 + 1] = {0};
	static FvUInt32 S_KEY = 0;
	++S_KEY;
	sprintf_s(pcName, 1024, "FvVisualBuilder_1_%d", S_KEY);
	FvParticleAttachment *pkParticle = new FvParticleAttachment(kInfo.m_pcRes);
	FvSmartPointer<FvZoneAttachment> pkAppearance = new FvZoneAttachment(pkParticle);

	typedef FvAttachNodeExpress Express;
	Express* pkExpress = FvExpressManagerInstance::Instance().MakeExpress<FvAttachNodeExpress>();
	if(pkExpress)
	{
		pkExpress->AttachAppearance(pkAppearance);
		if(kInfo.m_iScale)
		{
			FvVector3 kScale(1.0f, 1.0f, 1.0f);
			kScale = kInfo.m_iScale*0.01f*kScale;
			pkAppearance->SetScale(kScale);
		}
		pkExpress->SetAttachType(kInfo.m_uiAttachType);
		pkExpress->AttachToSpace(spProvider, spSpace);
		if(kInfo.m_iDuration == 0)
		{
			pkExpress->Start();
		}
		else
		{
			pkExpress->Start(kInfo.m_iDelayTime*0.01f, kInfo.m_iDuration*0.01f);
		}
		kOwner.Add(*pkExpress);
	}
}
void GxVisualBuilder::Create(const GxVisualData::AttachExpress& kInfo, const FvZoneSpacePtr& spSpace, const FvMotor::TrackBallPtr& spProvider, const float fDelayTime/* = 0.0f*/)
{
	if(!spSpace || !kInfo.m_pcRes)
	{
		FvLogBus::Warning("VisualAttachExpress::Create Space为空");
		return;
	}
	static char pcName[1024 + 1] = {0};
	static FvUInt32 S_KEY = 0;
	++S_KEY;
	sprintf_s(pcName, 1024, "FvVisualBuilder_2_%d", S_KEY);
	FvParticleAttachment *pkParticle = new FvParticleAttachment(kInfo.m_pcRes);
	FvSmartPointer<FvZoneAttachment> pkAppearance = new FvZoneAttachment(pkParticle);

	typedef FvAttachNodeExpress Express;
	Express* pkExpress = FvExpressManagerInstance::Instance().MakeExpress<FvAttachNodeExpress>();
	if(pkExpress)
	{
		if(kInfo.m_iScale)
		{
			FvVector3 kScale(1.0f, 1.0f, 1.0f);
			kScale = kInfo.m_iScale*0.01f*kScale;
			pkAppearance->SetScale(kScale);
		}
		pkExpress->AttachAppearance(pkAppearance);
		pkExpress->SetAttachType(kInfo.m_uiAttachType);
		pkExpress->AttachToSpace(spProvider, spSpace);
		if(kInfo.m_iDuration == 0)
		{
			pkExpress->Start();
		}
		else
		{
			pkExpress->Start(kInfo.m_iDelayTime*0.01f, kInfo.m_iDuration*0.01f);
		}
	}
}