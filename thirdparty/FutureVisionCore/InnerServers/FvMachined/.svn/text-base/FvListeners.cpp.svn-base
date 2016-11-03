#include "FvListeners.h"
#include "FvMachined.h"
#include <FvSysLog.h>
#include <FvMemoryStream.h>


FvListeners::FvListeners( FvMachined &machined ) : m_kMachined( machined ) {}

void FvListeners::HandleNotify( const FvNetProcessMessage &pm, in_addr addr )
{
	InterfaceListenerMsg kMsg(addr.s_addr, pm.m_uiPort, pm.m_uiUID, pm.m_kName);
	int iMsgSize = (int)sizeof(InterfaceListenerMsg);

	Members::iterator iter = m_kMembers.begin();

	while (iter != m_kMembers.end())
	{
		FvNetListenerMessage &lm = iter->m_kLM;

		if (lm.m_uiCategory == pm.m_uiCategory &&
			(lm.m_uiUID == lm.ANY_UID || lm.m_uiUID == pm.m_uiUID) &&
			(lm.m_kName == pm.m_kName || lm.m_kName.size() == 0))
		{
			int preSize = lm.m_kPreAddr.size();
			int postSize = lm.m_kPostAddr.size();
			int msglen = preSize + iMsgSize + postSize;
			char *data = new char[ msglen ];

			memcpy( data, lm.m_kPreAddr.c_str(), preSize );
			memcpy( data + preSize, &kMsg, iMsgSize );
			memcpy( data + preSize + iMsgSize, lm.m_kPostAddr.c_str(), postSize );

			m_kMachined.EP().SendTo( data, msglen, lm.m_uiPort, iter->m_uiAddr );
			delete [] data;
		}

		++iter;
	}
}

void FvListeners::CheckListeners()
{
	for (Members::iterator it = m_kMembers.begin(); it != m_kMembers.end(); )
	{
		if (!CheckProcess( it->m_kLM.m_uiPID ))
		{
			FvSysLog( FV_SYSLOG_INFO, "Dropping dead listener (for %s's) with pid %d",
				it->m_kLM.m_kName.c_str(), it->m_kLM.m_uiPID );
			it = m_kMembers.erase(it);
		}
		else
		{
			++it;
		}
	}
}
