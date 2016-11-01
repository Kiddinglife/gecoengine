#include "FvZoneSerializer.h"
#include "FvZoneSerializerXMLImpl.h"

#include <FvWatcher.h>
#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

FvZoneSerializer::FvZoneSerializer() :
m_pkListener(NULL)
{
	m_kImplementations.insert(FvZoneSerializerImplMap::value_type("xml",
		new FvZoneSerializerXMLImpl));
}

FvZoneSerializer::~FvZoneSerializer()
{
	for(FvZoneSerializerImplMap::iterator kIt = m_kImplementations.begin();
		kIt != m_kImplementations.end(); ++kIt)
	{
		delete kIt->second;
	}
	m_kImplementations.clear();
}

bool FvZoneSerializer::ExportZone(const FvZone *pkZone,const FvString& kFileName,
				const FvString &kImpl)
{
	FvZoneSerializerImplMap::iterator kIt = m_kImplementations.find(kImpl);
	if(kIt == m_kImplementations.end())
	{
		FV_ERROR_MSG("Conn't find serializer implementation %s",kImpl.c_str());
		return false;
	}

	return kIt->second->ExportZone(pkZone,kFileName);
}

bool FvZoneSerializer::ImportZone(Ogre::DataStreamPtr& kStream, FvZone *pkDest,
				const FvString &kImpl)
{
	FvZoneSerializerImplMap::iterator kIt = m_kImplementations.find(kImpl);
	if(kIt == m_kImplementations.end())
	{
		FV_ERROR_MSG("Conn't find serializer implementation %s",kImpl.c_str());
		return false;
	}

	return kIt->second->ImportZone(kStream,pkDest);
}

FvString FvZoneSerializer::OutsideZoneIdentifier( int iGridX, int iGridY, bool bSingleDir )
{
	char pcZoneIdentifierCStr[32];
	FvString kGridZoneIdentifier;

	FvUInt16 uiGridxs = FvUInt16(iGridX), uiGridys = FvUInt16(iGridY);
	if (!bSingleDir)
	{
		if (FvUInt16(uiGridxs + 4096) >= 8192 || FvUInt16(uiGridys + 4096) >= 8192)
		{
			sprintf_s( pcZoneIdentifierCStr, sizeof(pcZoneIdentifierCStr), "%01xxxx%01xxxx/sep/",
				int(uiGridxs >> 12), int(uiGridys >> 12) );
			kGridZoneIdentifier = pcZoneIdentifierCStr;
		}
		if (FvUInt16(uiGridxs + 256) >= 512 || FvUInt16(uiGridys + 256) >= 512)
		{
			sprintf_s( pcZoneIdentifierCStr, sizeof(pcZoneIdentifierCStr), "%02xxx%02xxx/sep/",
				int(uiGridxs >> 8), int(uiGridys >> 8) );
			kGridZoneIdentifier += pcZoneIdentifierCStr;
		}
		if (FvUInt16(uiGridxs + 16) >= 32 || FvUInt16(uiGridys + 16) >= 32)
		{
			sprintf_s( pcZoneIdentifierCStr, sizeof(pcZoneIdentifierCStr), "%03xx%03xx/sep/",
				int(uiGridxs >> 4), int(uiGridys >> 4) );
			kGridZoneIdentifier += pcZoneIdentifierCStr;
		}
	}
	sprintf_s( pcZoneIdentifierCStr, sizeof(pcZoneIdentifierCStr), "%04x%04xo", int(uiGridxs), int(uiGridys) );
	kGridZoneIdentifier += pcZoneIdentifierCStr;

	return kGridZoneIdentifier;
}