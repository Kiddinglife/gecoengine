//{future header message}
#ifndef __FvZone_H__
#define __FvZone_H__

#include <FvBaseTypes.h>
#include <FvBoundingBox.h>
#include <FvMatrix.h>
#include <FvStringMap.h>
#include <FvVectorNoDestructor.h>

#include "FvZoneDefine.h"
#include "FvZoneBoundary.h"
#include "FvZoneItem.h"
#include <FvRefList.h>

#ifndef FV_SERVER
#include <OgreAxisAlignedBox.h>
#include <OgreWireBoundingBox.h>
#endif // !FV_SERVER

class FvOcclusionQueryAABB;
class FvPortal2D;
class FvCamera;
class FvZoneCache;
class FvZoneSpace;
class FvZoneDirMapping;
class FvZoneSerializer;

#if FV_UMBRA_ENABLE
namespace Umbra
{
	class Cell;
};
#endif

#ifdef FV_EDITOR_ENABLED
class FvEditorZoneCache;
#endif

class FV_ZONE_API FvZone : public FvRefNode1<FvZone*>
{
public:
	static void Init();
	static void Fini();

	FvZone( const FvString & identifier, FvZoneDirMapping * pMapping );
	~FvZone();

	void SetMainBoundaryID(FvInt32 i32MainBoundary);
	FvInt32 GetMainBoundaryID();
	FvUInt32 GetJointBoundaryNumber();
	FvZoneBoundary* GetMainBoundary();

	void Ratify();

	bool Load();
	void Eject();

	void Bind( bool bForm );
	void BindPortals( bool bForm );
	void Loose( bool bCut );

	void Focus();
	void Smudge();

	void ResolveExterns( FvZoneDirMapping *pkDeadMapping = NULL );

	void UpdateBoundingBoxes( FvZoneItemPtr spItem );

	void AddStaticItem( FvZoneItemPtr spItem );
	void DelStaticItem( FvZoneItemPtr spItem );

	void AddDynamicItem( FvZoneItemPtr spItem );
	bool ModDynamicItem( FvZoneItemPtr spItem,
		const FvVector3 &kOldPos, const FvVector3 &kNewPos, const float fDiameter = 1.f,
		bool bUseDynamicLending = false);
	void DelDynamicItem( FvZoneItemPtr pItem, bool bUseDynamicLending=true );

	void JogForeignItems();

	bool AddLoanItem( FvZoneItemPtr pItem );
	bool DelLoanItem( FvZoneItemPtr pItem, bool bCanFail=false );
	bool IsLoanItem( FvZoneItemPtr pItem ) const;

#ifndef FV_SERVER
	void Visit(FvCamera *pkCamera, FvZoneVisitor *pkVisitor);
	void VisitTerrain(FvCamera *pkCamera, FvZoneVisitor *pkVisitor);

	void VisitLents(FvCamera *pkCamera, FvZoneVisitor *pkVisitor);

	void Tick( float fTime );
	void VisitIndex(FvUInt32 uiIndex) { m_uiVisitIndex = uiIndex; }
	FvUInt32 VisitIndex() { return m_uiVisitIndex; }
#endif // FV_SERVER

	FvZone *FindClosestUnloadedZoneTo( const FvVector3 & kPoint,
		float * pfDist );

	bool Contains( const FvVector3 &kPoint, const float fRadius = 0.f ) const;
	bool Owns( const FvVector3 &kPoint );

	float Volume() const;
	const FvVector3 &Centre() const { return m_kCentre; }

	const FvString & Identifier() const	{ return m_kIdentifier; }
	FvInt16	X() const			{ return m_iX; }
	FvInt16 Y() const			{ return m_iY; }
	FvZoneDirMapping *Mapping() const		{ return m_pkMapping; }
	FvZoneSpace *Space() const		{ return m_pkSpace; }
	FvString BinFileName() const;

	bool IsOutsideZone() const { return m_IsOutsideZone; }
	bool IsOutsideLight() const { return m_bUseOutsideLight; }

	bool HasInternalZones()	const { return m_bHasInternalZones; }
	void HasInternalZones( bool v ) { m_bHasInternalZones = v; }

	bool Ratified()	const	{ return m_bRatified; }
	bool Loading()	const	{ return m_bLoading; }
	bool Loaded()	const	{ return m_bLoaded; }
	bool Online()	const	{ return m_bOnline; }

	bool Focussed()	const	{ return m_iFocusCount > 0; }

	FvMatrix &Transform() { return m_kTransform; }
	FvMatrix &TransformInverse() { return m_kTransformInverse; }
	void Transform( const FvMatrix & kTransform );
	void TransformTransiently( const FvMatrix & kTransform );

	FvUInt32 VisitMark() const { return m_uiVisitMark; }
	void VisitMark( FvUInt32 uiMask ) { m_uiVisitMark = uiMask; }

	FvUInt32 ReflectionMark() const		{ return m_uiReflectionMark; }
	void ReflectionMark( FvUInt32 m )	{ m_uiReflectionMark = m; }

	FvUInt32 TraverseMark() const { return m_uiTraverseMark; }
	void TraverseMark( FvUInt32 m )	{ m_uiTraverseMark = m; }

	static FvUInt32 NextMark()			 { return ms_uiNextMark++; }
	static FvUInt32 NextRenderMark()	 { return ms_uiRenderMark++; }
	static FvUInt32 NextVisibilityMark() { return ms_uiNextVisibilityMark++; }

	float PathSum() const		{ return m_fPathSum; }
	void PathSum( float fSum )	{ m_fPathSum = fSum; }

	const FvBoundingBox &LocalBB() const		{ return m_kLocalBB; }
	const FvBoundingBox &BoundingBox() const	{ return m_kBoundingBox; }

#ifndef FV_SERVER
	const FvBoundingBox &VisibilityBox();
	void AddZBoundsToVisibilityBox(float fMinZ, float fMaxZ);
#endif // FV_SERVER

	void LocalBB( const FvBoundingBox &kBB ) { m_kLocalBB = kBB; }
	void BoundingBox( const FvBoundingBox &kBB ) { m_kBoundingBox = kBB; }

	FvZoneBoundaries &Bounds() { return m_kBounds; }
	FvZoneBoundaries &Joints() { return m_kJoints; }
	bool CanSeeHeaven();

	const FvString & Label() const { return m_kLabel; }

	FvString ResourceID() const;

	static bool FindBetterPortal( FvZoneBoundary::Portal *pkCurr, float fWithinRange,
		FvZoneBoundary::Portal *pkTest, const FvVector3 &kVec );

	class piterator
	{
	public:
		void operator++(int)
		{
			m_kPIt++;
			this->Scan();
		}

		bool operator==( const piterator &kOther ) const
		{
			return m_kBIt == kOther.m_kBIt &&
				(m_kBIt == m_kSource.end() || m_kPIt == kOther.m_kPIt);
		}

		bool operator!=( const piterator & other ) const
		{
			return !this->operator==( other );
		}

		FvZoneBoundary::Portal & operator*()
		{
			return **m_kPIt;
		}

		FvZoneBoundary::Portal * operator->()
		{
			return *m_kPIt;
		}


	private:
		piterator( FvZoneBoundaries &kSource, bool end ) :
		   m_kSource( kSource )
		   {
			   if (!end)
			   {
				   m_kBIt = m_kSource.begin();
				   if (m_kBIt != m_kSource.end())
				   {
					   m_kPIt = (*m_kBIt)->m_kBoundPortals.begin();
					   this->Scan();
				   }
			   }
			   else
			   {
				   m_kBIt = m_kSource.end();
			   }
		   }

		   void Scan()
		   {
			   while (m_kBIt != m_kSource.end() && m_kPIt == (*m_kBIt)->m_kBoundPortals.end())
			   {
				   m_kBIt++;
				   if (m_kBIt != m_kSource.end())
					   m_kPIt = (*m_kBIt)->m_kBoundPortals.begin();
			   }
		   }

		   FvZoneBoundaries::iterator m_kBIt;
		   FvZoneBoundary::Portals::iterator m_kPIt;
		   FvZoneBoundaries	& m_kSource;

		   friend class FvZone;
	};

	piterator PBegin()
	{
		return piterator( m_kJoints, false );
	}

	piterator PEnd()
	{
		return piterator( m_kJoints, true );
	}

	typedef void (*TouchFunction)( FvZone &kZone );
	static int RegisterCache( TouchFunction tf );

	FvZoneCache *&Cache( int iID )					{ return m_ppCaches[iID]; }

	FvZone *FringeNext()							{ return m_pkFringeNext; }
	FvZone *FringePrev()							{ return m_pkFringePrev; }
	void FringeNext( FvZone * pkZone )				{ m_pkFringeNext = pkZone; }
	void FringePrev( FvZone * pkZone )				{ m_pkFringePrev = pkZone; }

	int SizeStaticItems() const;

	void Loading( bool bLoading ) { m_bLoading = bLoading; }

	bool Removable() const { return m_bRemovable; }
	void Removable( bool bRemovelbe ) { m_bRemovable = bRemovelbe; }

#if FV_UMBRA_ENABLE
	Umbra::Cell* GetUmbraCell() const;
#endif


#ifdef FV_EDITOR_ENABLED
	friend class FvEditorZoneCache;
	static bool ms_bHideIndoorZones;
#endif

	typedef std::vector<FvZone*> (*OverlapperFinderFun)( FvZone* );
	static OverlapperFinderFun OverlapperFinder()
	{
		return ms_kOverlapperFinder;
	}
	static void OverlapperFinder( OverlapperFinderFun kFinder )
	{
		ms_kOverlapperFinder = kFinder;
	}

	static FvUInt32 ms_uiNextMark;
	static FvUInt32 ms_uiRenderMark;
	static FvUInt32 ms_uiCurrentCamera;
	static FvUInt32 ms_uiCurrentCameraID;
	static FvInt32  ms_iRenderZone;

	void AddLentVisitItem(FvZoneItem* pkItem);
	void ClearLentVisitItems();

private:
	friend class FvZoneAmbientLight;

	static OverlapperFinderFun ms_kOverlapperFinder;
	void SyncInit();

	void Bind( FvZone * pkZone );
	bool FormPortal( FvZone * pkZone, FvZoneBoundary::Portal &kPortal );
	void Loose( FvZone * pkZone, bool bCut );

	void NotifyCachesOfBind( bool bLooseNotBind );

	FvZoneItem* m_apkLentVisitItems[100];
	FvUInt32 m_u32LentVisitItemPointer;

	static FvRefListNameSpace<FvZone>::List1 ms_kLentZoneList;

	const FvString m_kIdentifier;
	FvInt16 m_iX;
	FvInt16 m_iY;
	FvZoneDirMapping *m_pkMapping;
	FvZoneSpace *m_pkSpace;

	const bool m_IsOutsideZone;
	bool m_bHasInternalZones;
	bool m_bUseOutsideLight;

	FvUInt8 m_uiCurrentGroupID;

	bool m_bRatified;
	bool m_bLoading;
	bool m_bLoaded;
	bool m_bOnline;
	int m_iFocusCount;

	FvMatrix m_kTransform;
	FvMatrix m_kTransformInverse;

	FvBoundingBox m_kLocalBB;

	FvBoundingBox m_kBoundingBox;

#ifndef FV_SERVER
	Ogre::WireBoundingBox m_kWireBox;
	FvBoundingBox m_kVisibilityBox;
	FvBoundingBox m_kVisibilityBoxCache;
	FvUInt32 m_uiVisibilityBoxMark;
	FvOcclusionQueryAABB* m_pkOcclusion;
#endif // FV_SERVER

	static FvUInt32 ms_uiNextVisibilityMark;

	FvVector3 m_kCentre;

	FvZoneBoundaries m_kBounds;	
	FvZoneBoundaries m_kJoints;	

	FvInt32 m_i32MainBoundaryID;

	//@{
	FvUInt32 m_uiVisitIndex;

	FvUInt32 m_uiVisitMark;
	FvUInt32 m_uiTraverseMark;
	FvUInt32 m_uiReflectionMark;
	FvUInt32 m_uiCameraVisitFlag;

	float m_fPathSum;
	//@}

	FvZoneCache **m_ppCaches;

	typedef std::vector<FvZoneItemPtr> Items;
	Items m_kSelfItems;
	Items m_kDynoItems;
	Items m_kSwayItems;

#if FV_UMBRA_ENABLE
	typedef std::map<FvZoneItem*, UmbraObjectProxyPtr> UmbraItems;
#endif // FV_UMBRA_ENABLE

	class Lender : public FvReferenceCount
	{
	public:
		FvZone *m_pkLender;
		Items m_kItems;
#if FV_UMBRA_ENABLE
		UmbraItem m_kUmbraItems;
#endif // FV_UMBRA_ENABLE
	};
	typedef FvSmartPointer<Lender> LenderPtr;
	typedef std::vector<LenderPtr> Lenders;
	Lenders m_kLenders;

	typedef std::vector<FvZone *> Borrowers;
	Borrowers m_kBorrowers;

	FvVectorNoDestructor<Items *> m_kLentItemLists;

	FvString m_kLabel;

	static int ms_iNextCacheID;
	static std::vector<TouchFunction>& TouchType()
	{
		static std::vector<TouchFunction> s_kTouchType;
		return s_kTouchType;
	};

	friend class FvHullZone;

	FvZone *m_pkFringeNext;
	FvZone *m_pkFringePrev;

	bool m_bInTick;
	bool m_bRemovable;

#if FV_UMBRA_ENABLE
	Umbra::Cell *m_pkUmbraCell;
#endif
	//debug

public: static	FvUInt32 ms_uiInstanceCount;
public: static	FvUInt32 ms_uiInstanceCountPeak;

	friend class FvZoneSerializerBINImpl;
	friend class FvZoneSerializerXMLImpl;
	friend class FvRenderManager;
	friend class FvZoneManager;
};

typedef FvZone* FvZonePtr;

class FV_ZONE_API FvZoneCache
{
public:
	virtual ~FvZoneCache() {}

	virtual void Visit(FvCamera *pkCamera, FvZoneVisitor *pkVisitor) {}
	virtual int Focus() { return 0; }		
	virtual void Bind( bool bLooseNotBind ) {}			
	virtual bool Load( FvZoneSerializer & )	
	{ return true; }

	static void Touch( FvZone & ) {}			

	template <class CacheT> class Instance
	{
	public:
		Instance() : m_iID( FvZone::RegisterCache( &CacheT::Touch ) ) {}

		CacheT & operator()( FvZone & kZone ) const
		{
			FvZoneCache * & cc = kZone.Cache( m_iID );
			if (cc == NULL) cc = new CacheT( kZone );
			return static_cast<CacheT &>(*cc);
		}

		bool Exists( FvZone &kZone ) const
		{
			return !!kZone.Cache( m_iID );
		}

		void Clear( FvZone &kZone ) const
		{
			FvZoneCache *&cc = kZone.Cache( m_iID );
			delete cc;
			cc = NULL;
		}

	private:
		int m_iID;
	};
};

void ZonesDrawCullingHUD();

#endif //__FvZone_H__