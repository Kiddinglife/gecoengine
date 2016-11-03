//{future header message}
#ifndef __FvZoneVLO_H__
#define __FvZoneVLO_H__

#include <FvVector2.h>
#include <FvVector3.h>
#include <FvStringMap.h>
#include <FvXMLSection.h>
#include <FvBoundingBox.h>

#include "FvZoneItem.h"

#include "FvZoneSerializerXMLImpl.h"

class FV_ZONE_API FvVLOFactory
{
public:
	typedef bool (*Creator)( FvZone *pkZone, FvXMLSectionPtr spSection, FvString kUID );

	FvVLOFactory( const FvString &kSection,
		int iPriority = 0,
		Creator pfCreator = NULL );

	virtual bool Create( FvZone *pkZone, FvXMLSectionPtr spSection, FvString kUID ) const;

	int Priority() const { return m_iPriority; }

private:
	int m_iPriority;
	Creator m_pfCreator;
};

class FvZoneVLO;

class FvVeryLargeObject;
typedef FvSmartPointer<FvVeryLargeObject> FvVeryLargeObjectPtr;
class FvVeryLargeObject : public FvSafeReferenceCount
{
public:
	typedef FvStringHashMap<FvVeryLargeObjectPtr> UniqueObjectList;
	typedef std::list<FvZoneVLO*> ZoneItemList;

	FvVeryLargeObject();
	FvVeryLargeObject( FvString kUID, FvString kType );
	~FvVeryLargeObject();

	void SetUID( FvString kUID );

#ifdef FV_EDITOR_ENABLED	
	ZoneItemList ZoneItems() const;

	virtual void Cleanup() {}
	virtual void SaveFile(FvZone *pkZone=NULL) {}
	virtual void Save() {}
	virtual void DrawRed(bool val) {}
	virtual void Highlight(bool val) {}
	virtual void EDDelete( FvZoneVLO *pkInstigator );	
	virtual const char *EDClassName() { return "VLO"; }
	virtual const FvMatrix & EDTransform() { return FvMatrix::IDENTITY; }
	virtual bool EDEdit( class FvZoneItemEditor & editor, const FvZoneItemPtr pItem ) { return false; }
	virtual bool EDShouldDraw();
	virtual FvString type() { return m_kType; }

	virtual bool IsObjectCreated() const;

	virtual bool VisibleInside() const { return true; }

	virtual bool VisibleOutside() const { return true; }

	static FvString GenerateUID();

	static void UpdateSelectionMark() { ms_uiCurrentSelectionMark++; }
	static FvUInt32 SelectionMark() { return ms_uiCurrentSelectionMark; }
	virtual bool EDCheckMark(FvUInt32 uiMark)
	{
		if (uiMark == m_uiSelectionMark)
			return false;
		else
		{
			m_uiSelectionMark = uiMark;
			return true;
		}
	}

	static void DeleteUnused();
	static void SaveAll();

#endif // FV_EDITOR_ENABLED

	virtual void ObjectCreated();

	bool ShouldRebuild() { return m_bRebuild; }
	void ShouldRebuild( bool rebuild ) { m_bRebuild = rebuild; }

	virtual void Dirty() {}
	virtual void Visit( FvZoneSpace *pkSpace, FvCamera *pkCamera, FvZoneVisitor *pkVisitor ) {}
	virtual void Lend( FvZone *pkZone ) {}
	virtual void Unlend( FvZone *pkZone ) {}
	virtual void UpdateLocalVars( const FvMatrix & m ) {}
	virtual void UpdateWorldVars( const FvMatrix & m ) {}	
	virtual const FvMatrix &Origin() { return FvMatrix::IDENTITY; }
	virtual const FvMatrix &LocalTransform( ) { return FvMatrix::IDENTITY; }	
	virtual const FvMatrix &LocalTransform( FvZone *pkZone ) { return FvMatrix::IDENTITY; }
	virtual void Sway( const FvVector3 &kSrc, const FvVector3 &kDest, const float fDiameter ) {}

	virtual FvBoundingBox ZoneBB( FvZone *pkZone ) { return FvBoundingBox::ms_kInsideOut; };

	FvString GetUID() const { return m_kUID; }

	void AddItem( FvZoneVLO *pkItem );
	void RemoveItem( FvZoneVLO *pkItem );
	FvBoundingBox &BoundingBox() { return m_kBB; }	
	FvZoneVLO *ContainsZone( const FvZone *pkZone ) const;
#ifdef FV_EDITOR_ENABLED
	void Section( const FvXMLSectionPtr spVLOSection )
	{ 
		m_spDataSection = spVLOSection;
	}	
	FvXMLSectionPtr Section() { return m_spDataSection; }

	static FvVeryLargeObjectPtr GetObject( const FvString &kUID )	
	{
		FvString kUIDTemp = kUID;
		std::transform(
				kUIDTemp.begin(), kUIDTemp.end(), kUIDTemp.begin(), tolower );
		return ms_kUniqueObjects[kUIDTemp];
	}
#else

	static FvVeryLargeObjectPtr GetObject( const FvString &kUID )	
	{
		return ms_kUniqueObjects[kUID];
	}
#endif // FV_EDITOR_ENABLED
	
	virtual void SyncInit(FvZoneVLO* pVLO) {}

	static UniqueObjectList ms_kUniqueObjects;
protected:
	friend class EditorChunkVLO;

	FvString m_kZonePath;
	FvBoundingBox m_kBB;
	FvString m_kUID;
	FvString m_kType;
	ZoneItemList m_kItemList;
#ifdef FV_EDITOR_ENABLED
	FvXMLSectionPtr m_spDataSection;
	bool m_bListModified;
	bool m_bObjectCreated;
#endif // FV_EDITOR_ENABLED

private:
	bool m_bRebuild;
#ifdef FV_EDITOR_ENABLED
	FvUInt32 m_uiSelectionMark;	
	static FvUInt32 ms_uiCurrentSelectionMark;
#endif // FV_EDITOR_ENABLED
};

class FV_ZONE_API FvZoneVLO : public FvZoneItem
{
public:
	explicit FvZoneVLO( WantFlags eWantFlags = WANTS_NOTHING );
	~FvZoneVLO();	

	virtual void Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor );		
	virtual void ObjectCreated() { }
	virtual void Lend( FvZone *pkZone );
	virtual void Toss( FvZone *pkZone );
	virtual void RemoveCollisionScene() {}	
	virtual void UpdateTransform( FvZone *pkZone ) {}
	virtual void Sway( const FvVector3 &kSrc, const FvVector3 &kDest, const float fDiameter );

	bool Load( FvXMLSectionPtr spSection, FvZone *pkZone );

#ifdef FV_EDITOR_ENABLED
	bool Root() const { return m_bCreationRoot; }
	void Root(bool bVal) { m_bCreationRoot = bVal; }
	bool CreateVLO( FvXMLSectionPtr spSection, FvZone *pkZone );
	bool CreateLegacyVLO( FvXMLSectionPtr spSection, FvZone *pkZone, FvString &kType );	
	bool CloneVLO( FvXMLSectionPtr spSection, FvZone *pkZone, FvVeryLargeObjectPtr spSource );
	bool BuildVLOSection( FvXMLSectionPtr spObjectSection, FvZone *pkZone, FvString &kType, FvString &kUID ); 
#endif // FV_EDITOR_ENABLED

	static bool LoadItem( FvZone *pkZone, FvXMLSectionPtr spSection );

	FvVeryLargeObjectPtr Object() const{ return m_spObject; }
	static void RegisterFactory( const FvString &kSection,
				const FvVLOFactory &kFactory );

	static void Fini();
	virtual void SyncInit();
	
protected:
	FvVeryLargeObjectPtr m_spObject;
private:
	typedef FvStringHashMap<const FvVLOFactory*> Factories;

	bool m_bDirty;
	bool m_bCreationRoot;
	static Factories *ms_pkFactories;

	FV_DECLARE_ZONE_ITEM_XML( FvZoneVLO )
};

#endif // __FvZoneVLO_H__
