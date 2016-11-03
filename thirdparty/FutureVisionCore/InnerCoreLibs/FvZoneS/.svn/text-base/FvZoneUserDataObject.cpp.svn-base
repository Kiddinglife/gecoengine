#include "FvZoneUserDataObject.h"
#include "FvBaseZoneSpace.h"


FV_DECLARE_DEBUG_COMPONENT2( "ZoneUserDataObject", 0 )

int iZoneUserDataObjectToken;

FvZoneUserDataObject::FvZoneUserDataObject()
:FvZoneItem(WANTS_NOTHING)
,m_uiType(0xFFFF)
{

}

FvZoneUserDataObject::~FvZoneUserDataObject()
{
	if(m_kUDO.IsReady())
		m_kUDO.DestroyUDO();
}

FvZoneItemXMLFactory::Result FvZoneUserDataObject::Create(FvZone* pkZone, FvXMLSectionPtr spSection)
{
	int iDomain = spSection->ReadInt("Domain");
#ifdef FV_SERVER
	if((iDomain & FvObjUDO::CELL) == 0)
	{
	    return FvZoneItemXMLFactory::SucceededWithoutItem();
	}
#else
	#ifndef FV_EDITOR_ENABLED
	if((iDomain & FvObjUDO::CLIENT) == 0)
	{
	    return FvZoneItemXMLFactory::SucceededWithoutItem();
	}
	#endif
#endif

	FvZoneUserDataObject* pkItem = new FvZoneUserDataObject();

	FvString kErrorString;
	if(pkItem->Load(spSection, pkZone, &kErrorString))
	{
		pkZone->AddStaticItem(pkItem);
		return FvZoneItemXMLFactory::Result(pkItem);
	}

	delete pkItem;
	return FvZoneItemXMLFactory::Result(NULL, kErrorString);
}
FvZoneItemXMLFactory FvZoneUserDataObject::ms_kFactory("UserDataObject", 0, FvZoneUserDataObject::Create);


bool FvZoneUserDataObject::Load(FvXMLSectionPtr spSection, FvZone* pkZone, FvString* pkErrorString)
{
	FvString kType = spSection->ReadString("type");
	m_uiType = FvObjUDO::NameToType(kType);
	if(m_uiType == 0xFFFF)
	{
		FV_ERROR_MSG("%s: No such UserDataObject type '%s'\n", __FUNCTION__, kType.c_str());
		return false;
	}
	FvString kID = spSection->ReadString("guid");
	if(kID.empty())
	{
		FV_ERROR_MSG("%s: UserDataObject has no GUID", __FUNCTION__);
		return false;
	}
	m_kInitData.kGuid = FvUniqueID(kID);
	FvXMLSectionPtr spTransform = spSection->OpenSection("transform");
	if(spTransform)
	{
		FvMatrix kMatrix = spSection->ReadMatrix34("transform", FvMatrix::IDENTITY);

		m_kInitData.kPosition.Set(kMatrix._41 + pkZone->X()*FV_GRID_RESOLUTION, kMatrix._42 + pkZone->Y()*FV_GRID_RESOLUTION, kMatrix._43);
		m_kInitData.kDirection.SetYaw(kMatrix.Roll());
		m_kInitData.kDirection.SetPitch(kMatrix.Pitch());
		m_kInitData.kDirection.SetRoll(kMatrix.Yaw());

		//kMatrix.PostMultiply(pkZone->Transform());
		//m_kInitData.kPosition = kMatrix.ApplyToOrigin();
		//m_kInitData.kDirection = FvDirection3(kMatrix.Yaw(), kMatrix.Pitch(), kMatrix.Roll());
	}
	else
	{
		FV_ERROR_MSG("%s: UserDataObject (of type '%s') has no position\n", __FUNCTION__, kType.c_str());
		return false;
	}

	FvXMLSectionPtr spProp = spSection->OpenSection("properties");
	if(!spProp)
	{
		FV_ERROR_MSG("%s: UserDataObject has no 'properties' section\n", __FUNCTION__);
		return false;
	}

	m_kInitData.spPropertiesDS = FvXMLSection::CreateSection("properties");
	m_kInitData.spPropertiesDS->CopySections(spProp);
	return true;
}

void FvZoneUserDataObject::Bind()
{
	if(m_kUDO.IsReady())
		return;

	m_kUDO.InitUDO(m_uiType, m_kInitData);
}

void FvZoneUserDataObject::Toss(FvZone* pkZone)
{
	if(m_uiType == 0xFFFF)
		return;

	if(pkZone == m_pkZone) return;

	if(m_pkZone != NULL)
	{
		FvZoneUserDataObjectCache::ms_kInstance(*m_pkZone).Del(this);
	}

	FvZoneItem::Toss(pkZone);

	if(m_pkZone != NULL)
	{
		FvZoneUserDataObjectCache::ms_kInstance(*m_pkZone).Add(this);
	}
}


FvZoneUserDataObjectCache::FvZoneUserDataObjectCache(FvZone& kZone)
{
}

FvZoneUserDataObjectCache::~FvZoneUserDataObjectCache()
{
}

void FvZoneUserDataObjectCache::Bind(bool bLooseNotBind)
{
	for(FvZoneUserDataObjects::iterator it = m_kUserDataObjects.begin();
		it != m_kUserDataObjects.end();
		++it)
	{
		(*it)->Bind();
	}
}

void FvZoneUserDataObjectCache::Add(FvZoneUserDataObject* pkUserDataObject)
{
	m_kUserDataObjects.push_back(pkUserDataObject);
}

void FvZoneUserDataObjectCache::Del(FvZoneUserDataObject* pkUserDataObject)
{
	FvZoneUserDataObjects::iterator found =
		std::find(m_kUserDataObjects.begin(), m_kUserDataObjects.end(), pkUserDataObject);

	if(found != m_kUserDataObjects.end())
		m_kUserDataObjects.erase(found);
}

FvZoneCache::Instance<FvZoneUserDataObjectCache> FvZoneUserDataObjectCache::ms_kInstance;
