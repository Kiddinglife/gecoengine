//{future header message}
#ifndef __FvZoneTreeObstacle_H__
#define __FvZoneTreeObstacle_H__

#include "FvZoneItem.h"
#include "FvZone.h"
#include "FvTree.h"

class FvZoneObstacle;
class FvTree;

class FvZoneTreeObstacle : public FvZoneCache
{
public:
	FvZoneTreeObstacle( FvZone &kZone );
	~FvZoneTreeObstacle();

	virtual int Focus();

	void AddObstacle( FvZoneObstacle *pkObstacle );
	void DelObstacles( FvZoneItemPtr spItem );

	static Instance<FvZoneTreeObstacle> ms_kInstance;

	void AddTree(const FvTreePtr& spTree, 
		const FvMatrix &kWorld, FvZoneItemPtr spItem,
		bool bEditorProxy = false );

private:
	std::vector< FvSmartPointer< FvZoneObstacle > > m_kObstacles;

protected:
	virtual int AddToSpace( FvZoneObstacle &kCSO );
	virtual void DelFromSpace( FvZoneObstacle &kCSO );

	FvZone *m_pkZone;
};

#endif // __FvZoneTreeObstacle_H__
