#include "FvDynamicInstance.h"


FvRefListNameSpace<FvBaseDynamicInstance>::List1 FvBaseDynamicInstance::ms_InstanceList;

FvBaseDynamicInstance::FvBaseDynamicInstance()
{
	ms_InstanceList.AttachBack(*this);
}
FvBaseDynamicInstance::~FvBaseDynamicInstance()
{
	Detach();
}