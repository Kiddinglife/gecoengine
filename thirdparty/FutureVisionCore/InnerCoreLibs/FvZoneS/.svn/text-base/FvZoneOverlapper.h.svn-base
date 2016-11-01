//{future header message}
#ifndef __FvZoneOverlapper_H__
#define __FvZoneOverlapper_H__

#include "FvZoneItem.h"
#include "FvZone.h"
#include "FvZoneSerializerXMLImpl.h"

class FvZoneOverlappers;

class FV_ZONE_API FvZoneOverlapper : public FvZoneItem
{
	FV_DECLARE_ZONE_ITEM_XML( FvZoneOverlapper )
public:
	FvZoneOverlapper();
	~FvZoneOverlapper();

	bool Load( FvXMLSectionPtr spSection, FvZone *pkZone, FvString *kErrorString = NULL );
	virtual void Toss( FvZone *pkZone );
	void Bind( bool bLooseNotBind );

	const FvBoundingBox &BoundingBox() const { return m_pkOverlapper->BoundingBox(); }

	bool Focussed() const { return m_pkOverlapper->Focussed(); }

	FvZone *Overlapper() const { return m_pkOverlapper; }

	void AlsoInAdd( FvZoneOverlappers *pkAlsoIn );
	void AlsoInDel( FvZoneOverlappers *pkAlsoIn );

private:
	FvZoneOverlapper( const FvZoneOverlapper& );
	FvZoneOverlapper& operator=( const FvZoneOverlapper& );

	FvString m_kOverlapperID;
	FvZone *m_pkOverlapper;
	std::vector<FvZoneOverlappers*>	m_kAlsoIn;
};

typedef FvSmartPointer<FvZoneOverlapper> FvZoneOverlapperPtr;

class FV_ZONE_API FvZoneOverlappers : public FvZoneCache
{
public:
	FvZoneOverlappers( FvZone &kZone );
	virtual ~FvZoneOverlappers();

	virtual void Bind( bool bLooseNotBind );
	static void Touch( FvZone &kZone );

	bool Empty() const						{ return m_kOverlappers.empty(); }
	bool Complete() const					{ return m_bComplete; }

	void Add( FvZoneOverlapperPtr spOverlapper, bool bForeign = false );
	void Del( FvZoneOverlapperPtr spOverlapper, bool bForeign = false );

	typedef std::vector<FvZoneOverlapperPtr> ZoneOverlappers;
	const ZoneOverlappers &Overlappers() const	{ return m_kOverlappers; }
	ZoneOverlappers &Overlappers() { return m_kOverlappers; }

	static Instance<FvZoneOverlappers> ms_kInstance;

private:
	void Share();
	void CopyFrom( FvZoneOverlappers &kOther );
	void CheckIfComplete( bool bCheckNeighbours );

	FvZone &m_kZone;
	ZoneOverlappers	m_kOverlappers;
	ZoneOverlappers m_kForeign;
	bool m_bBound;
	bool m_bHalfBound;
	bool m_bComplete;
};

#endif // __FvZoneOverlapper_H__