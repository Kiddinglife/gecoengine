
#include <vld.h>
#include <FvAttachExpress.h>
#include <FvTravelExpress.h>
#include <FvExpressManager.h>
#include <FvMotor.h>
#include <FvDestroyManager.h>
#include <FvGameTimer.h>


namespace Express_Demo
{
	void Test()
	{
		FvGameTimer::Init();
		AttachExpress_Demo::TestListener attachExpressListener;
		TravelExpress_Demo::TestListener travelExpressListener;
		FvExpressManager expressManager;

		FvReferencePtr<FvAttachNodeExpress> rpAttachExpress = AttachExpress_Demo::Create(attachExpressListener, expressManager);
		FvReferencePtr<FvTravelNodeExpress<FvTrackMotor>> rpTravelExpress = TravelExpress_Demo::Create(travelExpressListener, expressManager);

		//! Ä£ÄâApplication
		FvUInt64 uiAccumulateTime = 0;
		while (expressManager.ExpressCount() != 0)
		{
			uiAccumulateTime += 1000;
			expressManager.Update(1.0f);
			FvGameTimer::Update(uiAccumulateTime);
		}

		FV_ASSERT(rpAttachExpress);
		FV_ASSERT(rpTravelExpress);

		FvDestroyManager::UpdateDeleteList();
	}
}

void main()
{
	Express_Demo::Test();

}