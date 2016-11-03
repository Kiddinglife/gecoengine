//{future header message}
#ifndef __FvUnitAuraChannel_H__
#define  __FvUnitAuraChannel_H__

#include "FvLogicServerDefine.h"

#include <FvDoubleLink.h>
#include <map>


class FvUnitAura;


class FV_LOGIC_SERVER_API FvUnitAuraChannel
{
public:
	typedef FvDoubleLink<FvUnitAura*, FvDoubleLinkNode2> List;

	void Attach(FvDoubleLinkNode2<FvUnitAura*>& kNode, const bool bUpdate);
	void Detach(FvDoubleLinkNode2<FvUnitAura*>& kNode, const bool bUpdate);

	FvUnitAura* GetTop()const;
	bool IsEmpty()const{return m_kList.IsEmpty();}
	const List& AuraList()const{return m_kList;}

private:

	List m_kList;
};

class FV_LOGIC_SERVER_API FvUnitAuraChannelList
{
public:
	FvUnitAuraChannelList();
	~FvUnitAuraChannelList();

	void Attach(const FvUInt32 uiChannel, FvDoubleLinkNode2<FvUnitAura*>& kNode, const bool bUpdate);
	void Detach(const FvUInt32 uiChannel, FvDoubleLinkNode2<FvUnitAura*>& kNode, const bool bUpdate);

	bool IsTop(const FvUnitAura& kAura, const FvUInt32 uiChannel)const;

	FvUnitAuraChannel* GetChannel(const FvUInt32 uiChannel)const;

private:
	std::map<FvUInt32, FvUnitAuraChannel*> m_kUnitAuraChannelList;
};



#endif //__FvUnitAuraChannel_H__