
//{future header message}
#ifndef __FvLogicGameData_H__
#define  __FvLogicGameData_H__

#include "FvLogicServerDefine.h"
#include <FvDynamicInstance.h>
#include <map>



struct FvStateCondition;

class FV_LOGIC_SERVER_API FvLogicGameData: public FvDynamicInstance<FvLogicGameData>
{
public:

	const FvStateCondition* GetStateCondition(const FvUInt32 uiKey)const;
	void AddCondition(const FvUInt32& uiKey, FvStateCondition& kCondition);

private:

	FvLogicGameData();
	~FvLogicGameData(){}
	template <class> friend class FvDynamicInstance;

	std::map<FvUInt32, const FvStateCondition*> m_kStateConditionList;
};


#endif //__FvLogicGameData_H__