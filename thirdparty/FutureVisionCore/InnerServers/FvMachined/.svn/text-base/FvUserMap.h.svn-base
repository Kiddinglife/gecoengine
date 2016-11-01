//{future header message}
#ifndef __FvUserMap_H__
#define __FvUserMap_H__

#include <FvNetMachineGuard.h>

class FvUserMap
{
public:
	FvUserMap();

	void Add( const FvNetUserMessage &um );
	FvNetUserMessage* Add( struct passwd *ent );

	bool GetEnv( FvNetUserMessage &um, struct passwd *pEnt = NULL );
	FvNetUserMessage *Fetch( FvUInt16 uid );
	bool SetEnv( const FvNetUserMessage &um );
	void Flush();

	friend class FvMachined;

protected:
	typedef std::map< FvUInt16, FvNetUserMessage > Map;
	Map m_kMap;
	FvNetUserMessage m_kNotFound;

	void QueryUserConfs();
};

#endif // __FvUserMap_H__
