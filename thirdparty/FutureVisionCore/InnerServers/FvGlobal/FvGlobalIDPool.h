//{future header message}
#ifndef __FvGlobalIDPool_H__
#define __FvGlobalIDPool_H__

#include <queue>
#include <FvBinaryStream.h>


class FvGlobalIDPool
{
public:
	FvGlobalIDPool(FvInt32 iInitNum);
	~FvGlobalIDPool();

	FvInt32	GetID();
	void	PutID(FvInt32 iID);
	bool	GetIDsToStream(int iNum, FvBinaryOStream& kStream);
	void	PutIDsFromStream(FvBinaryIStream& kStream);

protected:
	FvInt32		m_iInitNum;
	FvInt32		m_iIDIdx;
	typedef std::queue<FvInt32> IDQueue;
	IDQueue		m_kFreeIDs;
};


#endif//__FvGlobalIDPool_H__
