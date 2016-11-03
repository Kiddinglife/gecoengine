//{future header message}
#ifndef __FvZoneSerializer_H__
#define __FvZoneSerializer_H__

#include <FvBaseTypes.h>

#include <OgreDataStream.h>

#include <map>

#include "FvZoneDefine.h"

class FvZone;
class FvZoneSerializerListener;

class FV_ZONE_API FvZoneSerializerImpl
{
public:
	virtual ~FvZoneSerializerImpl(){}

	virtual bool ExportZone(const FvZone *pkZone,const FvString& kFileName) = 0;
	virtual bool ImportZone(Ogre::DataStreamPtr& kStream, FvZone *pkDest) = 0;
};

class FV_ZONE_API FvZoneSerializer
{
public:
	FvZoneSerializer();
	~FvZoneSerializer();

	bool ExportZone(const FvZone *pkZone,const FvString& kFileName,
		const FvString &kImpl = "xml");
	bool ImportZone(Ogre::DataStreamPtr &kStream, FvZone *pkDest,
		const FvString &kImpl = "xml");

	static FvString OutsideZoneIdentifier( int iGridX, int iGridY, 
		bool bSingleDir = false );

protected:

	typedef std::map<FvString,FvZoneSerializerImpl*> FvZoneSerializerImplMap;
	FvZoneSerializerImplMap m_kImplementations;

	FvZoneSerializerListener *m_pkListener;
};

#endif // __FvZoneSerializer_H__