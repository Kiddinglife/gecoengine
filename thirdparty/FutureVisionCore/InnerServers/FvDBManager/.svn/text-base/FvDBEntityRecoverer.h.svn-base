//{future header message}
#ifndef __FvDBEntityRecoverer_H__
#define __FvDBEntityRecoverer_H__

#include <FvNetTypes.h>

class FvDBEntityRecoverer
{
public:
	FvDBEntityRecoverer();

	void Reserve( int numEntities );

	void Start();

	void Abort();

	void AddEntity( FvEntityTypeID entityTypeID, FvDatabaseID dbID );

	void OnRecoverEntityComplete( bool isOK );

private:

	void CheckFinished();
	bool SendNext();

	bool AllSent() const	{ return m_iNumSent >= int(m_kEntities.size()); }

	typedef std::vector< std::pair< FvEntityTypeID, FvDatabaseID > > Entities;
	Entities m_kEntities;
	int m_iNumOutstanding;
	int m_iNumSent;
	bool m_bHasErrors;
};

#endif // __FvDBEntityRecoverer_H__
