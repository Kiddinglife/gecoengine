//{future header message}
#ifndef __FvZoneModelObstacle_H__
#define __FvZoneModelObstacle_H__

#include "FvZoneItem.h"
#include "FvZone.h"

class FvZoneObstacle;
class FvModel;

class FvZoneModelObstacle : public FvZoneCache
{
public:
	FvZoneModelObstacle( FvZone &kZone );
	~FvZoneModelObstacle();

	virtual int Focus();

	void AddObstacle( FvZoneObstacle *pkObstacle );
	void DelObstacles( FvZoneItemPtr spItem );

	static Instance<FvZoneModelObstacle> ms_kInstance;

	void AddModel( FvSmartPointer<FvModel> spModel, 
		const FvMatrix &kWorld, FvZoneItemPtr spItem,
		bool bEditorProxy = false );

private:
	std::vector< FvSmartPointer< FvZoneObstacle > > m_kObstacles;

protected:
	virtual int AddToSpace( FvZoneObstacle &kCSO );
	virtual void DelFromSpace( FvZoneObstacle &kCSO );

	FvZone *m_pkZone;
};

#endif // __FvZoneModelObstacle_H__
