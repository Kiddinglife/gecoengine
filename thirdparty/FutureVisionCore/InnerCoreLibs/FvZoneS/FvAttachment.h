//{future header message}
#ifndef __FvAttachment_H__
#define __FvAttachment_H__

#include "FvZoneDefine.h"
#include "FvCamera.h"
#include <FvSmartPointer.h>
#include <FvQuaternion.h>

class FvQuaternion;
class FvCamera;
class FvZoneVisitor;

#define FV_ATTACHMENT_TYPE_NAME(NAME)\
	static const std::string& TYPE_NAME(){static std::string S_NAME(NAME); return S_NAME;}\
	virtual const std::string& TypeName()const{return TYPE_NAME();}

class FV_ZONE_API FvAttachment : public FvReferenceCount
{
public:
	FV_ATTACHMENT_TYPE_NAME("FvAttachment");

	~FvAttachment(){}

	virtual void Tick( float fTime ) = 0;
	virtual bool Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor ) { return true; }
	virtual void VisitPick(Ogre::RenderQueue *pkQueue, const Ogre::Vector4& kColor) {}

	virtual const FvVector3 &GetPosition() const = 0;
	virtual const FvQuaternion &GetQuaternion() const = 0;
	virtual const FvVector3 &GetScale() const = 0;
	virtual void SetPosition(const FvVector3 &kPosition) = 0;
	virtual void SetQuaternion(const FvQuaternion &kQuaternion) = 0;
	virtual void SetScale(const FvVector3 &kScale) = 0;

	virtual void BoundingBoxAcc( FvBoundingBox &kBB, bool bSkinny = false )
	{ kBB.AddBounds( FvVector3::ZERO ); }

	virtual void Tossed( FvZone *pkZone ) {}

	virtual bool Attach(){return true;}
	virtual void Detach(){}

	virtual void EnterWorld(){}
	virtual void LeaveWorld(){}

	virtual void Move( float fTime ) {}

	bool IsAttached() const { return m_bAttached; }
	bool IsInWorld() const { return m_bInWorld; }


protected:

	bool m_bAttached;
	bool m_bInWorld;
};

typedef FvSmartPointer<FvAttachment> FvAttachmentPtr;

#endif // __FvAttachment_H__