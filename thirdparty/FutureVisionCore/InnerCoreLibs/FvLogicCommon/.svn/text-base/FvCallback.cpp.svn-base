#include "FvCallback.h"



FvAsynCallbackInterface::List FvAsynCallbackInterface::S_LIST;
FvAsynCallbackInterface::FvAsynCallbackInterface()
{
	
}
void FvAsynCallbackInterface::UpdateAsynCallbacks()
{
	List& kList = S_List();
	kList.EndIterator();
	kList.BeginIterator();
	while (!kList.IsEnd())
	{
		FvAsynCallbackInterface* pkCallback = kList.GetIterator()->Content();
		FV_ASSERT(pkCallback);
		kList.Next();
		pkCallback->_AsynExec();
	}
	kList.Clear();
}

//+-----------------------------------------------------------------------------------------------------------------

FvFrameEndCallbackInterface::List FvFrameEndCallbackInterface::S_LIST;
FvFrameEndCallbackInterface::FvFrameEndCallbackInterface()
{
	FV_MEM_TRACK;
}
FvFrameEndCallbackInterface::~FvFrameEndCallbackInterface()
{}

void FvFrameEndCallbackInterface::UpdateCallbacks()
{
	List& kList = S_List();
	kList.EndIterator();
	kList.BeginIterator();
	while (!kList.IsEnd())
	{
		FvFrameEndCallbackInterface* pkCallback = kList.GetIterator()->Content();
		FV_ASSERT(pkCallback);
		kList.Next();
		pkCallback->_Exec();
	}
	kList.Clear();
}
void FvFrameEndCallbackInterface::Detach()
{
	FvRefNode1<FvFrameEndCallbackInterface*>::Detach();
}
void FvFrameEndCallbackInterface::_Attach()
{
	S_List().AttachBackSafeIterator(*this);	
}
