//{future header message}
#ifndef __FvZoneBoundary_H__
#define __FvZoneBoundary_H__

#include <FvBaseTypes.h>
#include <FvPlane.h>
#include <FvVector2.h>
#include <FvMatrix.h>
#include <FvBoundingBox.h>
#include <FvSmartPointer.h>

#include <vector>

#ifndef FV_SERVER
#include <Ogre.h>
#endif // !FV_SERVER

class FvZone;
class FvZoneSpace;
class FvPortal2D;
class FvZoneDirMapping;
class FvZoneVisitor;

#if FV_UMBRA_ENABLE
namespace Umbra
{
	class Model;
	class PhysicalPortal;
}
#endif

class FvPrivPortal
{
public:
	
	FvPrivPortal() : m_bHasPlaneEquation( false ), m_iFlags( 0 ) { }

	~FvPrivPortal() { }

	const FvPlane &GetPlaneEquation( void ) const { return m_kPlaneEquation; }

	void AddPoints( const std::vector<FvVector3> &kPoints );

	void Flag( const FvString& kFlag );

	const FvVector3 &GetPoint( FvUInt32 uiIDX ) const	{ return m_kPoints[uiIDX]; }

	FvUInt32 GetNPoints() const { return m_kPoints.size(); }

	void Transform( const class FvMatrix &kTransform );

	bool IsHeaven() const { return !!(m_iFlags & (1<<1)); }

	bool IsEarth() const { return !!(m_iFlags & (1<<2)); }

	bool IsInvasive() const	{ return !!(m_iFlags & (1<<3)); }

private:
	FvPlane m_kPlaneEquation;
	bool m_bHasPlaneEquation;

	int m_iFlags;

	std::vector< FvVector3 > m_kPoints;
};

class FvPortal2DRef
{
public:
	FvPortal2DRef( bool bValid = true );
	FvPortal2DRef( const FvPortal2DRef &kOther );
	FvPortal2DRef & operator =( const FvPortal2DRef &kOther );
	~FvPortal2DRef();

	bool Valid() { return (FvUIntPtr)m_pkVal != (FvUIntPtr)-1; }

	const FvPortal2D *Val() const { return m_pkVal; }

	FvPortal2D *operator->() { return m_pkVal; }

	FvPortal2D &operator*()	{ return *m_pkVal; }

private:
	FvPortal2D *m_pkVal;

	friend class FvPortal2DStore;
};

typedef FvSmartPointer<struct FvZoneBoundary> FvZoneBoundaryPtr;

struct FvZoneBoundary : public FvReferenceCount
{
	FvZoneBoundary();
	~FvZoneBoundary();

	static void Fini();

	const FvPlane &Plane() const		{ return m_kPlane; }

	typedef std::vector<FvVector2>	V2Vector;
	typedef std::vector<FvVector3>	V3Vector;

	struct Portal
#if (!defined(FV_SERVER) && defined(FV_ENABLE_DRAW_PORTALS))
		: public Ogre::SimpleRenderable
#endif // !FV_SERVER && !FV_ENABLE_DRAW_PORTALS
	{
		Portal(FvZoneBoundary* pkBoundary);
		~Portal();

		bool ResolveExtern( FvZone *pkOwnZone );
		void ObjectSpacePoint( int iIDX, FvVector3 &kRet );
		FvVector3 ObjectSpacePoint( int iIDX );
		void ObjectSpacePoint( int iIDX, FvVector4 &kRet );
		void Transform( const FvMatrix &kTransform );

		bool m_bInternal;
		bool m_bPermissive;
		FvZone *m_pkZone;
		FvZoneBoundary* m_pkBoundary;
		V2Vector m_kPoints;
		V3Vector m_kWPoints;
		FvVector3 m_kWNormal;
		FvVector3 m_kUAxis;	
		FvVector3 m_kVAxis;	
		FvVector3 m_kOrigin;		
		FvVector3 m_kLCentre;	
		FvVector3 m_kCentre;		
		FvPlane m_kPlane;		
		FvString  m_kLabel;

		enum
		{
			NOTHING = 0,
			HEAVEN = 1,
			EARTH = 2,
			INVASIVE = 3,
			EXTERN = 4,
			LAST_SPECIAL = 15
		};

		bool HasZone() const
		{ return FvUIntPtr(m_pkZone) > FvUIntPtr(LAST_SPECIAL); }

		bool IsHeaven() const
		{ return FvUIntPtr(m_pkZone) == FvUIntPtr(HEAVEN); }

		bool IsEarth() const
		{ return FvUIntPtr(m_pkZone) == FvUIntPtr(EARTH); }

		bool IsInvasive() const
		{ return FvUIntPtr(m_pkZone) == FvUIntPtr(INVASIVE); }

		bool IsExtern() const
		{ return FvUIntPtr(m_pkZone) == FvUIntPtr(EXTERN); }

#if (!defined(FV_SERVER) && defined(FV_ENABLE_DRAW_PORTALS))
		void Visit( FvZoneVisitor *pkVisitor,
			const FvMatrix &kTransform, float fInset = 0.f );

		void getWorldTransforms( Ogre::Matrix4* xform ) const{*xform = Ogre::Matrix4::IDENTITY;}
		Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const { return 0; }
		Ogre::Real getBoundingRadius(void) const { return 0; }
#endif // !FV_SERVER && !FV_ENABLE_DRAW_PORTALS

		bool Inside( const FvVector3 &kPoint ) const;
		FvUInt32 Outcode( const FvVector3 &kPoint ) const;

#if FV_UMBRA_ENABLE
		void CreateUmbraPortal( FvZone *pkOwner );
		Umbra::Model* CreateUmbraPortalModel();
		void ReleaseUmbraPortal();

		Umbra::PhysicalPortal *m_pkUmbraPortal;
#endif
		static bool ms_bDrawPortals;
	};

	typedef std::vector<Portal*> Portals;

	void BindPortal( FvUInt32 uiUnboundIndex );
	void LoosePortal( FvUInt32 uiBoundIndex );

	void AddInvasivePortal( Portal *pkPortal );
	void SplitInvasivePortal( FvZone *pkZone, unsigned int uiIDX );

	FvPlane m_kPlane;

	Portals	m_kBoundPortals;
	Portals	m_kUnboundPortals;
};

typedef std::vector<FvZoneBoundaryPtr> FvZoneBoundaries;

#endif // __FvZoneBoundary_H__