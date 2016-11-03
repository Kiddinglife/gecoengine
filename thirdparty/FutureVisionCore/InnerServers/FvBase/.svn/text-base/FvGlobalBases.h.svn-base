//{future header message}
#ifndef __FvGlobalBases_H__
#define __FvGlobalBases_H__


#include "FvBaseDefines.h"
#include <FvBaseTypes.h>
#include <map>

class FvEntity;
class FvMailBox;
class RegisterGlobalBaseCallBack;


class FvGlobalBases
{
public:
	FvGlobalBases();
	~FvGlobalBases();

	FvMailBox*			Find(const FvString& kLabel) const;

	void				RegisterRequest(const char* pcLabel, FvEntity* pkBase, RegisterGlobalBaseCallBack* pkCallback);
	bool				Deregister(const char* pcLabel, FvEntity* pkBase);

	void				Add(const FvString& kLabel, FvMailBox& kBaseMB);
	void				Remove(const FvString& kLabel);

	void				OnBaseDestroyed(FvEntity* pkBase);

	typedef std::map<FvString, FvMailBox> Maps;
	Maps&				GetGlobalBaseMap() { return m_kMap; }

private:
	Maps		m_kMap;
};



#endif // __FvGlobalBases_H__
