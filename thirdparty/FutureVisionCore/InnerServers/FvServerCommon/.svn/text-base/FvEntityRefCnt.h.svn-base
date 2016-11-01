//{future header message}
#ifndef __FvEntityRefCnt_H__
#define __FvEntityRefCnt_H__

#include "FvServerCommon.h"
#include <FvBinaryStream.h>


class FV_SERVERCOMMON_API FvEntityRefCnt
{
public:
	FvEntityRefCnt():m_uiSysRefCnt(0),m_uiUsrRefCnt(0){}

	bool	CanDestroy() const { return m_uiSysRefCnt == 0 && m_uiUsrRefCnt == 0x8000; }

	bool	IsSetDestroy() const { return m_uiUsrRefCnt & 0x8000; }
	void	SetDestroy() { m_uiUsrRefCnt = m_uiUsrRefCnt | 0x8000; }

	void	AddSysRefCnt() { FV_ASSERT(m_uiSysRefCnt<0xFFFF); ++m_uiSysRefCnt; }
	void	DelSysRefCnt() { FV_ASSERT(0<m_uiSysRefCnt); --m_uiSysRefCnt; }

	void	AddUsrRefCnt() { FV_ASSERT((m_uiUsrRefCnt & 0x7FFF)<0x7FFF); ++m_uiUsrRefCnt; }
	void	DelUsrRefCnt() { FV_ASSERT(0<(m_uiUsrRefCnt & 0x7FFF)); --m_uiUsrRefCnt; }

protected:
	FvUInt16	m_uiSysRefCnt;
	FvUInt16	m_uiUsrRefCnt;	//! 最高位表示是否Destroy
};

FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvEntityRefCnt)


#endif//__FvEntityRefCnt_H__
