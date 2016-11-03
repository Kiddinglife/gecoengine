//{future header message}
#ifndef __FvUserDataObject_H__
#define __FvUserDataObject_H__


#include "FvEntityDefDefines.h"
#include "FvObj.h"
#include <FvSmartPointer.h>
#include <FvXMLSection.h>
#include <FvUniqueID.h>
#include <map>
#include <FvNetMsgTypes.h>


class FvUserDataObjectType;
typedef FvSmartPointer<FvUserDataObjectType>		FvUserDataObjectTypePtr;
class FvUserDataObject;
typedef FvSmartPointer<FvUserDataObject>			FvUserDataObjectPtr;
struct FvUserDataObjectInitData
{
	FvUniqueID		kGuid;
	FvPosition3D	kPosition;
	FvDirection3	kDirection;
	FvXMLSectionPtr	spPropertiesDS;
};

class FV_ENTITYDEF_API FvUserDataObject : public FvObj
{
	FVOBJ_RTTI_DEFINE
public:
	FvUserDataObject(FvUserDataObjectTypePtr spUserDataObjectType);
	FvUserDataObject(FvDataType* dataType, bool bDefault=true);
	~FvUserDataObject();

	virtual FvObjPtr Copy() { return new FvUserDataObject(NULL); }
	virtual int	GetSize();
	bool operator<( const FvUserDataObject & other ) const;

	static bool IsValidPosition(const FvCoord& kC)
	{
		const float MAX_ENTITY_POS = 1000000000.f;
		return (-MAX_ENTITY_POS < kC.m_fX && kC.m_fX < MAX_ENTITY_POS &&
			-MAX_ENTITY_POS < kC.m_fY && kC.m_fY < MAX_ENTITY_POS);
	}

	static FvUserDataObject* Get(const FvUniqueID& kGuid);
	static FvUserDataObject* Build(const FvUserDataObjectInitData& kInitData, FvUserDataObjectTypePtr spType);
	static FvUserDataObject* CreateRef(const FvString& kGuid);
	static FvUserDataObject* CreateRef(const FvUniqueID& kGuid);
	static void CreateRefType();

	void	Init(const FvUserDataObjectInitData& kInitData );
	bool	IsReady() const { return m_bPropsLoaded; }
	void	UnBuild();

	const FvUniqueID&			Guid() const		{ return m_kGuid; }
	const FvPosition3D&			Position() const	{ return m_kGlobalPosition; }
	const FvDirection3&			Direction() const	{ return m_kGlobalDirection; }
	const FvUserDataObjectType& GetType() const		{ return *m_spUserDataObjectType.GetObject(); }

	void	ResetType(FvUserDataObjectTypePtr spNewType);

	FvObj*	GetAttribute(const char* pcAttrName);
	bool	SetAttribute(const char* pcAttrName, FvObj* pkValue);


protected:
	FvUserDataObject(const FvUserDataObject& src);
	virtual	void	Attach(FvPropertyOwnerBase* owner) { FV_ASSERT(!m_pkOwner); }
	virtual	void	Detach() { FV_ASSERT(!m_pkOwner); }

private:
	FvUserDataObjectTypePtr	m_spUserDataObjectType;

	FvUniqueID				m_kGuid;
	FvPosition3D			m_kGlobalPosition;
	FvDirection3			m_kGlobalDirection;
	bool					m_bPropsLoaded;

	typedef std::vector<FvObjPtr> Attributs;
	Attributs				m_kAttributs;

	typedef std::map<FvUniqueID,FvUserDataObject*> UdoMap;
	static UdoMap			ms_kCreated;
};

#endif//__FvUserDataObject_H__

