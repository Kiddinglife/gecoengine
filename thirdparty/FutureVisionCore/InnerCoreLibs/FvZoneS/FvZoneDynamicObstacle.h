//{future header message}
#ifndef __FvZoneDynamicObstacle_H__
#define __FvZoneDynamicObstacle_H__

#include <FvGameObjectModel.h>

#include "FvZoneDefine.h"
#include "FvZoneEmbodiment.h"

class FvModelObstacle;

class FvEmbodimentListener
{
public:
	virtual void OnLoadComplete(const FvZoneEmbodiment *pkEmbodiment) = 0;
};

class FV_ZONE_API FvZoneDynamicObstacle : public FvZoneDynamicEmbodiment
{
public:
	FvZoneDynamicObstacle( const FvString& kName,
		const FvMatrix& kMatrix,
		FvEmbodimentListener* pkListener );
	virtual ~FvZoneDynamicObstacle();

	virtual void Tick( float fTime ){}
	virtual void Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor );
	virtual void Toss( FvZone *pkZone );

	virtual void EnterSpace( FvZoneSpacePtr spSpace, bool bTransient );
	virtual void LeaveSpace( bool bTransient );
	virtual void Move( float fTime );

	virtual const FvVector3 &GetPosition() const;
	virtual void BoundingBox( FvBoundingBox &kBB ) const;

	FvModelObstacle *pObstacle() const { return m_pkModelObstacle; }

	virtual void VisitPick(Ogre::RenderQueue *pkQueue, const Ogre::Vector4& kColor);

private:
	FvZoneDynamicObstacle( const FvZoneDynamicObstacle& );
	FvZoneDynamicObstacle& operator=( const FvZoneDynamicObstacle& );

	static void DeleteModel(FvZoneDynamicObstacle *pkModel);
	static bool StillValid(FvZoneDynamicObstacle *pkModel);

	static void DoLoadResources( void *pkSelf );
	static void OnLoadResourcesComplete( void *pkSelf );

	bool m_bEnterSpace;
	bool m_bLastTransient;
	FvZoneSpacePtr m_spLastSpace;
	FvModelObstacle *m_pkModelObstacle;
	FvEmbodimentListener* m_pkListener;

	static FvSimpleMutex ms_kDeletionLock;
	typedef std::vector< FvZoneDynamicObstacle* > Models;
	static Models ms_kModels;
};

typedef FvSmartPointer<FvZoneDynamicObstacle> FvZoneDynamicObstaclePtr;

class FV_ZONE_API FvZoneStaticObstacle : public FvZoneStaticEmbodiment
{
public:
	FvZoneStaticObstacle( const FvString& kName,
		const FvMatrix& kMatrix,
		FvEmbodimentListener* pkListener );
	virtual ~FvZoneStaticObstacle();

	virtual void Tick( float fTime ){}
	virtual void Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor );
	virtual void Toss( FvZone *pkZone );
	virtual void Lend( FvZone *pkLender );

	virtual void EnterSpace( FvZoneSpacePtr spSpace, bool bTransient );
	virtual void LeaveSpace( bool bTransient );

	virtual const FvVector3 &GetPosition() const;
	virtual void BoundingBox( FvBoundingBox &kBB ) const;

	FvModelObstacle *pObstacle() const { return m_pkModelObstacle; }

	virtual void VisitPick(Ogre::RenderQueue *pkQueue, const Ogre::Vector4& kColor);

private:
	FvZoneStaticObstacle( const FvZoneStaticObstacle& );
	FvZoneStaticObstacle& operator=( const FvZoneStaticObstacle& );

	static void DeleteModel(FvZoneStaticObstacle *pkModel);
	static bool StillValid(FvZoneStaticObstacle *pkModel);

	static void DoLoadResources( void *pkSelf );
	static void OnLoadResourcesComplete( void *pkSelf );

	bool m_bEnterSpace;
	bool m_bLastTransient;
	FvZoneSpacePtr m_spLastSpace;
	FvModelObstacle *m_pkModelObstacle;
	FvEmbodimentListener* m_pkListener;

	static FvSimpleMutex ms_kDeletionLock;
	typedef std::vector< FvZoneStaticObstacle* > Models;
	static Models ms_kModels;
};

class FV_ZONE_API FvModelObstacle
{
public:
	FvModelObstacle( const FvString& kName,
		const FvMatrix& kMatrix );
	~FvModelObstacle();

	void EnterWorld( FvZoneItem *pkItem );
	void LeaveWorld();

	const FvVector3 &GetPosition() const;
	const FvMatrix &WorldTransform() const { return m_kWorldTransform; }
	void WorldTransform(const FvMatrix& kMatrix);

	const FvBoundingBox &BoundingBox();

	FvZoneObstacle *Obstacle() const { return m_spObstacle.Get(); }

	void Loaded(bool bLoaded)	{ m_bLoaded = bLoaded; }
	bool Loaded() const	{ return m_bLoaded; }

	void Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor );
	void VisitPick(Ogre::RenderQueue *pkQueue, const Ogre::Vector4& kColor);

	FvGameObjectModelPtr GetModel() { return m_spModel; }

private:

	void Load();

	bool m_bIsRender;
	FvString m_kResourceName;
	FvMatrix m_kWorldTransform;
	FvGameObjectModelPtr m_spModel;
	FvZoneObstaclePtr m_spObstacle;
	FvBoundingBox m_kBoundingBox;	

	bool m_bLoaded;

	friend class FvZoneStaticObstacle;
	friend class FvZoneDynamicObstacle;
};

#endif // __FvZoneDynamicObstacle_H__