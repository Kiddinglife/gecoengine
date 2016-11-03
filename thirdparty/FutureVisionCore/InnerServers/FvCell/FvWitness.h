//{future header message}
#ifndef __FvWitness_H__
#define __FvWitness_H__

#include "FvCellDefines.h"
#include "FvRealEntity.h"
//#include "FvEntityCache.h"


class FV_CELL_API FvWitness
{
public:
	FvWitness(FvRealEntity& owner, FvBinaryIStream& data, CreateRealInfo createRealInfo, bool hasChangedSpace=false);
	~FvWitness();

	FvRealEntity&		Real()			{ return m_kReal; }
	const FvRealEntity& Real() const	{ return m_kReal; }
	FvEntity&			Entity()		{ return m_kEntity; }
	const FvEntity&		Entity() const	{ return m_kEntity; }

	void				OpenAoIAfterTeleportLocally();
	void				CloseAoIBeforeTeleportLocally();
	void				Offload(FvBinaryOStream& kStream);
	void				CheckEventSync();

private:
	FvRealEntity&		m_kReal;
	FvEntity&			m_kEntity;

};


#include "FvWitness.inl"


#endif // __FvWitness_H__
