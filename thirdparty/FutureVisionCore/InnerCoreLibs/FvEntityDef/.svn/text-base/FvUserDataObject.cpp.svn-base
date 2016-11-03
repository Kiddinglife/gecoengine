#include "FvUserDataObject.h"
#include "FvUserDataObjectLinkDataType.h"
#include "FvUserDataObjectType.h"


FVOBJ_RTTI_IMPLEMENT(FvUserDataObject, FVOBJ_RTTI_USERDATAOBJ)

FvUserDataObject::UdoMap FvUserDataObject::ms_kCreated;

namespace
{
	FvUserDataObjectTypePtr s_spBaseType = NULL;
}

void DestroyUserDataRefType()
{
	s_spBaseType = NULL;
}


FvUserDataObject::FvUserDataObject(FvUserDataObjectTypePtr spUserDataObjectType)
:FvObj(FvUserDataObjectLinkDataType::pInstance())
,m_spUserDataObjectType(spUserDataObjectType)
,m_bPropsLoaded(false)
{

}

FvUserDataObject::FvUserDataObject(FvDataType* dataType, bool bDefault)
:FvObj(FvUserDataObjectLinkDataType::pInstance())
,m_bPropsLoaded(false)
{
	CreateRefType();

	m_spUserDataObjectType = s_spBaseType;
}

FvUserDataObject::~FvUserDataObject()
{
	UdoMap::iterator it = ms_kCreated.find( m_kGuid );
	if ( it != ms_kCreated.end() )
		ms_kCreated.erase( it );
}

int FvUserDataObject::GetSize()
{
	AddObjCnt(ms_uiRTTI);
	int iSize = (int)sizeof(FvUserDataObject);
	int iCnt = (int)m_kAttributs.size();
	for(int i=0; i<iCnt; ++i)
	{
		iSize += m_kAttributs[i]->GetSize();
	}
	return iSize;
}

bool FvUserDataObject::operator<( const FvUserDataObject & other ) const
{
	return m_kGuid < other.m_kGuid;
}

FvUserDataObject* FvUserDataObject::Get(const FvUniqueID& kGuid)
{
	UdoMap::iterator it = ms_kCreated.find( kGuid );
	if ( it != ms_kCreated.end() )
		return (*it).second;

	return NULL;
}

FvUserDataObject* FvUserDataObject::Build(const FvUserDataObjectInitData& kInitData, FvUserDataObjectTypePtr spType)
{
	FvUserDataObject* kObj = FvUserDataObject::Get( kInitData.kGuid );
	if(kObj && kObj->IsReady())
	{
		FV_CRITICAL_MSG( "%s: "
			"object %s has already been built.\n",
			__FUNCTION__,
			kInitData.kGuid.ToString().c_str() );
		return NULL;
	}

	if(kObj == NULL)
	{
		kObj = spType->NewUserDataObject(kInitData);
	}
	else if(spType)
	{
		kObj->ResetType(spType);
		kObj->Init(kInitData);
	}

	return kObj;
}

FvUserDataObject* FvUserDataObject::CreateRef(const FvString& kGuid)
{
	if(kGuid.empty())
		return NULL;

	return FvUserDataObject::CreateRef(FvUniqueID(kGuid));
}

FvUserDataObject* FvUserDataObject::CreateRef(const FvUniqueID& kGuid)
{
	FvUserDataObject* pkUDO = FvUserDataObject::Get(kGuid);
	if(pkUDO == NULL)
	{
		CreateRefType();

		if(s_spBaseType != NULL)
		{
			FvUserDataObjectInitData kInitData;
			kInitData.kGuid = kGuid;
			pkUDO = s_spBaseType->NewUserDataObject(kInitData);
		}
	}

	return pkUDO;
}

void FvUserDataObject::CreateRefType()
{
	static bool bFirstTime = true;

	if(bFirstTime)
	{
		bFirstTime = false;

		s_spBaseType = new FvUserDataObjectType(FvUserDataObjectDescription());
	}
}

void FvUserDataObject::Init(const FvUserDataObjectInitData& kInitData)
{
	FV_IF_NOT_ASSERT_DEV(!this->IsReady())
	{
		return;
	}

	FV_IF_NOT_ASSERT_DEV(!kInitData.kGuid.ToString().empty())
	{
		return;
	}

	ms_kCreated[kInitData.kGuid] = this;

	if(kInitData.spPropertiesDS == NULL)
	{
		m_kGuid = kInitData.kGuid;
		return;
	}

	if(!IsValidPosition(kInitData.kPosition))
	{
		FV_ERROR_MSG("FvUserDataObject::SetPositionAndDirection: "
			"(%f,%f,%f) is not a valid position for entity %s\n",
			kInitData.kPosition.x, kInitData.kPosition.y, kInitData.kPosition.z,
			m_kGuid.ToString().c_str());
		return;
	}

	m_kGuid	= kInitData.kGuid;
	m_kGlobalPosition = kInitData.kPosition;
	m_kGlobalDirection = kInitData.kDirection;

	const FvUserDataObjectDescription& kUserDataDes = m_spUserDataObjectType->Description();
	FvUInt32 uiPropCnt = kUserDataDes.PropertyCount();
	for(FvUInt32 i=0; i<uiPropCnt; ++i)
	{
		FvDataDescription* pkDataDes = kUserDataDes.Property(i);
		FV_ASSERT(pkDataDes);
		FvXMLSectionPtr spChildSection = kInitData.spPropertiesDS->OpenChild(pkDataDes->Name());

		if(spChildSection)
		{
			FvObjPtr spObj = pkDataDes->CreateFromSection(spChildSection);
			m_kAttributs.push_back(spObj);
		}
		else
		{
			FvObjPtr spObj = pkDataDes->pInitialValue()->Copy();
			m_kAttributs.push_back(spObj);
		}
	}

	m_bPropsLoaded = true;
}

void FvUserDataObject::UnBuild()
{
	if(IsReady())
	{
		CreateRefType();

		if(s_spBaseType != NULL)
		{
			m_kAttributs.clear();

			ResetType(s_spBaseType);

			m_bPropsLoaded = false;
		}
	}
}

void FvUserDataObject::ResetType(FvUserDataObjectTypePtr spNewType)
{
	FV_IF_NOT_ASSERT_DEV(spNewType)
	{
		return;
	}

	m_spUserDataObjectType = spNewType;
}

FvObj* FvUserDataObject::GetAttribute(const char* pcAttrName)
{
	if(!IsReady())
		return NULL;

	FvUInt32 uiIdx;
	if(!m_spUserDataObjectType->Description().FindPropertyIndex(pcAttrName, uiIdx))
		return NULL;

	if(uiIdx < (int)m_kAttributs.size())
		return m_kAttributs[uiIdx].Get();
	else
		return NULL;
}

bool FvUserDataObject::SetAttribute(const char* pcAttrName, FvObj* pkValue)
{
	return false;
}