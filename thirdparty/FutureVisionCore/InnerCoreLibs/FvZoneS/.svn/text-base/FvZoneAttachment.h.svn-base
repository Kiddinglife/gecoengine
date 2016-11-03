//{future header message}
#ifndef __FvZoneAttachment_H__
#define __FvZoneAttachment_H__

#include <FvQuaternion.h>

#include "FvZoneDefine.h"
#include "FvZoneEmbodiment.h"
#include "FvAttachment.h"

class FV_ZONE_API FvZoneAttachment : public FvZoneDynamicEmbodiment
{
public:
	FvZoneAttachment();
	FvZoneAttachment( FvAttachmentPtr spAttachment );
	~FvZoneAttachment();

	virtual void Tick( float fTime );
	virtual void Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor );
	virtual void Toss( FvZone *pkZone );

	void EnterSpace( FvZoneSpacePtr spSpace, bool bTransient );
	void LeaveSpace( bool bTransient );

	virtual void Move( float fTime );

	virtual void BoundingBox( FvBoundingBox &kBound ) const;
	virtual void DrawBoundingBoxes( const FvBoundingBox &kBound, 
		const FvBoundingBox &kVBound, const FvMatrix &kSpaceTrans ) const {}

	const FvVector3 &GetPosition() const;
	const FvQuaternion &GetQuaternion() const;
	const FvVector3 &GetScale() const;
	void SetPosition( const FvVector3 &kPosition );
	void SetQuaternion( const FvQuaternion &kQuaternion );
	void SetScale( const FvVector3 &kScale );

	FvAttachment *GetAttachment() const;

	virtual bool AddZBounds( FvBoundingBox &kBB ) const;

	virtual void VisitPick(Ogre::RenderQueue *pkQueue, const Ogre::Vector4& kColor);

private:

	bool m_bNeedsSync;
	bool m_bInited;
	bool m_bIsRender;

	FvAttachmentPtr m_spAttachment;
};

#include "FvZoneAttachment.inl"

#endif // __FvZoneAttachment_H__