//{future header message}
#ifndef __FvListeners_H__
#define __FvListeners_H__

#include <FvNetMachineGuard.h>

class FvMachined;

class FvListeners
{
private:
	class Member
	{
	public:
		Member( const FvNetListenerMessage & lm, u_int32_t addr ) :
			m_kLM( lm ),
			m_uiAddr( addr ) {}
		FvNetListenerMessage m_kLM;
		u_int32_t m_uiAddr;
	};

public:
	FvListeners( FvMachined &machined );
	void Add( const FvNetListenerMessage & lm, u_int32_t addr )
	{
		m_kMembers.push_back( Member( lm, addr ) );
	}

	void HandleNotify( const FvNetProcessMessage & pm, in_addr addr );
	void CheckListeners();

private:
	typedef std::vector< Member > Members;
	Members m_kMembers;
	FvMachined &m_kMachined;
};

#endif // __FvListeners_H__
