#ifndef __FVZONEANIMATIONMODELOBSTACLE_H__
#define __FVZONEANIMATIONMODELOBSTACLE_H__
#include "FvZoneItem.h"
#include "FvZone.h"

class FvZoneObstacle;
class FvAnimationModel;

class FvZoneAnimationModelObstacle : public FvZoneCache
{
public:
	FvZoneAnimationModelObstacle( FvZone &kZone );
	~FvZoneAnimationModelObstacle();

	virtual int Focus();

	void AddObstacle( FvZoneObstacle *pkObstacle );
	void DelObstacles( FvZoneItemPtr spItem );

	static Instance<FvZoneAnimationModelObstacle> ms_kInstance;

	void AddAnimationModel( FvSmartPointer<FvAnimationModel> spAnimModel, 
		const FvMatrix &kWorld, FvZoneItemPtr spItem,
		bool bEditorProxy = false );

private:
	std::vector< FvSmartPointer< FvZoneObstacle > > m_kObstacles;

protected:
	virtual int AddToSpace( FvZoneObstacle &kCSO );
	virtual void DelFromSpace( FvZoneObstacle &kCSO );

	FvZone *m_pkZone;
};
#endif //__FVZONEANIMATIONMODELOBSTACLE_H__