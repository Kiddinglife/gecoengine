//{future header message}
#ifndef __FvZoneItem_H__
#define __FvZoneItem_H__

#include <FvBaseTypes.h>
#include <FvSmartPointer.h>

#include "FvZoneDefine.h"

class FvZone;
class FvZoneSpace;
class FvVector3;
class FvZoneVisitor;
class FvBoundingBox;
class FvZoneSerializer;
class FvCamera;

class FvZoneItemBase
{
public:
	enum WantFlags
	{
		WANTS_NOTHING		= 0,
		WANTS_VISIT			= 1 <<  0,
		WANTS_TICK			= 1 <<  1,
		WANTS_SWAY			= 1 <<  2,
		WANTS_NEST			= 1 <<  3,

		FORCE_32_BIT		= 1 << 31
	};
	static const FvUInt32 USER_FLAG_SHIFT  =  8;
	static const FvUInt32 CHUNK_FLAG_SHIFT = 24;

	enum
	{
		TYPE_DEPTH_ONLY		= ( 1 << 0 ),
	};

	explicit FvZoneItemBase( WantFlags eWantFlags = WANTS_NOTHING ) :
	m_iCount( 0 ), m_eWantFlags( eWantFlags ), m_uiCameraMark(0), m_pkZone( NULL ), m_bTrueDynamic(false) {}
	FvZoneItemBase( const FvZoneItemBase &kOther ) :
	m_iCount( 0 ), m_eWantFlags( kOther.m_eWantFlags ), m_uiCameraMark(0), m_pkZone( kOther.m_pkZone ), m_bTrueDynamic(false) {}
	virtual ~FvZoneItemBase() {}

	virtual void Toss( FvZone *pkZone ) { this->Zone(pkZone ); }
	virtual void Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor ) { }
	virtual void Tick( float /*fTime*/ ) { }
	virtual void Sway( const FvVector3 & /*kSrc*/, const FvVector3 & /*kDst*/, const float /*fRadius*/ ) { }
	virtual void Lend( FvZone * /*pkLender*/ ) { }
	virtual void Nest( FvZoneSpace * /*pkSpace*/ ) { }

	virtual const char *Label() const				{ return ""; }
	virtual FvUInt32 TypeFlags() const				{ return 0; }

	virtual void IncRef() const		{ ++m_iCount; }
	virtual void DecRef() const		{ if(--m_iCount == 0) delete this; }
	virtual int RefCount() const	{ return m_iCount; }

	FvZone *Zone() const			{ return m_pkZone; }
	void Zone( FvZone *pkZone )	{ m_pkZone = pkZone; }

	bool WantsVisit() const { return !!(m_eWantFlags & WANTS_VISIT); }
	bool WantsTick() const { return !!(m_eWantFlags & WANTS_TICK); }
	bool WantsSway() const { return !!(m_eWantFlags & WANTS_SWAY); }
	bool WantsNest() const { return !!(m_eWantFlags & WANTS_NEST); }

	FvUInt32 VisitMark() const { return m_uiVisitMark; }
	void VisitMark( FvUInt32 uiMask )
	{ m_uiVisitMark = uiMask; }

	FvUInt32& CameraMark() { return m_uiCameraMark; }

	FvUInt8 UserFlags() const { return FvUInt8(m_eWantFlags >> USER_FLAG_SHIFT); }

	virtual bool AddZBounds( FvBoundingBox &kBB ) const	{	return false;	}

	virtual void SyncInit() {}

	void SetTrueDynamic(bool bEnable) { m_bTrueDynamic = bEnable; }

	bool GetTrueDynamic() { return m_bTrueDynamic; }

#if FV_UMBRA_ENABLE
	UmbraObjectProxyPtr pUmbraObject() const { return m_spUmbraObject; }
#endif
private:
	FvZoneItemBase & operator=( const FvZoneItemBase &kOther );	// undefined

	mutable int m_iCount;
	WantFlags m_eWantFlags;
	FvUInt32 m_uiVisitMark;
	FvUInt32 m_uiCameraMark;

protected:
	FvZone *m_pkZone;
	bool m_bTrueDynamic;

#if FV_UMBRA_ENABLE
	UmbraModelProxyPtr	m_spUmbraModel;
	UmbraObjectProxyPtr	m_spUmbraObject;
#endif

	void LendByBoundingBox( FvZone *pkLender, const FvBoundingBox &kWorldBB );

	void LendByBoundingBoxShell( FvZone *pkLender, const FvBoundingBox &kWorldBB );

public: static	FvUInt32	ms_uiInstanceCount;
public: static	FvUInt32	ms_uiInstanceCountPeak;
};


#ifdef FV_EDITOR_ENABLED
#include "FvEditorZoneItem.h"

#else // FV_EDITOR_ENABLED
class FvClientZoneItem : public FvZoneItemBase
{
public:
	explicit FvClientZoneItem( WantFlags eWantFlags = WANTS_NOTHING ) : 
	FvZoneItemBase( eWantFlags ) { }
};

typedef FvClientZoneItem FvSpecialZoneItem;
#endif // FV_EDITOR_ENABLED

class FV_ZONE_API FvZoneItem : public FvSpecialZoneItem
{
public:
	explicit FvZoneItem( WantFlags eWantFlags = WANTS_NOTHING ) : FvSpecialZoneItem( eWantFlags ) { }
	virtual void SyncInit() {}
};

typedef FvSmartPointer<FvZoneItem> FvZoneItemPtr;

#endif // __FvZoneItem_H__