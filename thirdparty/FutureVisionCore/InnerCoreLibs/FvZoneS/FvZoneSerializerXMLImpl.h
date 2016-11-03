//{future header message}
#ifndef __FvZoneSerializerImpl_H__
#define __FvZoneSerializerImpl_H__

#include <FvBaseTypes.h>
#include <FvMatrix.h>
#include <FvVector3.h>
#include <FvStringMap.h>
#include <FvXMLSection.h>

#include <OgreDataStream.h>

#include "FvZoneBoundary.h"
#include "FvZoneItem.h"
#include "FvZoneSerializer.h"

class FvZone;

class FV_ZONE_API FvZoneItemXMLFactory
{
public:

	class Result
	{
		bool m_bSucceeded;
		bool m_bOnePerZone;
		FvZoneItemPtr m_spItem;
		FvString m_kErrorString;
	protected:

		Result() : m_bSucceeded( true )
		{}
	public:

		explicit Result( FvZoneItemPtr spItem,
			const FvString &kErrorString = "",
			bool bOnePerChunk = false ) :
		m_bSucceeded( spItem ),
			m_bOnePerZone( bOnePerChunk ),
			m_spItem( spItem ),
			m_kErrorString(kErrorString )
		{}

		operator bool() const
		{
			return m_bSucceeded;
		}

		FvZoneItemPtr operator->()
		{
			return m_spItem;
		}
		FvZoneItemPtr Item()
		{
			return m_spItem;
		}
		bool OnePerChunk() const 
		{ 
			return m_bOnePerZone; 
		}
		const FvString &ErrorString() const
		{
			return m_kErrorString;
		}
	};

	class SucceededWithoutItem : public Result
	{};

	typedef Result (*Creator)( FvZone *pkZone, FvXMLSectionPtr spSection );

	FvZoneItemXMLFactory( const FvString &kSection,
		int iPriority = 0,
		Creator pfCreator = NULL );
	virtual ~FvZoneItemXMLFactory() {};

	virtual Result Create( FvZone *pkZone, FvXMLSectionPtr spSection ) const;

	int Priority() const							{ return m_iPriority; }

private:
	int	m_iPriority;
	Creator m_pfCreator;
};

class FV_ZONE_API FvZoneSerializerXMLImpl : public FvZoneSerializerImpl
{
public:
	FvZoneSerializerXMLImpl();
	~FvZoneSerializerXMLImpl();

	bool ExportZone(const FvZone *pkZone,const FvString& kFileName);
	bool ImportZone(Ogre::DataStreamPtr& kStream, FvZone *pkDest);

	static void RegisterFactory( const FvString &kSection,
		const FvZoneItemXMLFactory &kFactory );

	static void Fini();

protected:

	FvZoneItemXMLFactory::Result LoadItem( FvXMLSectionPtr spSection,FvZone *pkDest );

	bool LoadInclude( FvXMLSectionPtr spSection, const FvMatrix & flatten, 
		FvString* errorStr,FvZone *pkDest);
	bool ReadBoundaries( FvXMLSectionPtr spSection, FvZone *pkDest );
	FvXMLSectionPtr CreateBoundary( FvXMLSectionPtr spBoundarySection, 
		const FvVector3 &kNormal, float fD );
	void CreatePortal( FvXMLSectionPtr spBoundary, const FvString &kToZone,
		const FvVector3 &kUAxis, const FvVector3 &kPT1, const FvVector3 &kPT2,
		const FvVector3 &kPT3, const FvVector3 &kPT4 );
	void CreateBoundaries( FvXMLSectionPtr spSection, FvZone *pkZone,
		std::vector<FvXMLSectionPtr> &kBSects);

	void ReadBoundary( FvXMLSectionPtr spSection,
		FvZoneBoundary *pkBoundary,FvZone *pkDest );

	void ReadBoundaryPortal( FvXMLSectionPtr spSection,
		FvZoneBoundary::Portal *pkPortal,FvZoneBoundary *pkBoundary,FvZone *pkDest );

	typedef FvStringHashMap<const FvZoneItemXMLFactory*> Factories;
	static Factories * ms_pkFactories;
};

#define FV_DECLARE_ZONE_ITEM_XML( CLASS )															\
	static FvZoneItemXMLFactory::Result Create( FvZone *pkZone, FvXMLSectionPtr spSection );		\
	static FvZoneItemXMLFactory ms_kFactory;														

#define FV_DECLARE_ZONE_ITEM_XML_ALIAS( CLASS, ALIAS )												\
	static FvZoneItemXMLFactory ms_k##ALIAS##Factory;

#define FV_IMPLEMENT_ZONE_ITEM_XML( CLASS, LABEL, PRIORITY )										\
	FvZoneItemXMLFactory CLASS::ms_kFactory( #LABEL, PRIORITY, CLASS::Create );						\
																									\
	FvZoneItemXMLFactory::Result CLASS::Create( FvZone *pkZone, FvXMLSectionPtr spSection )			\
{																									\
	CLASS *pkItem = new CLASS();																	\
	\
	FvString kErrorString; 																			\
	if (pkItem->Load FV_IMPLEMENT_ZONE_ITEM_XML_ARGS)												\
	{																								\
		pkZone->AddStaticItem( pkItem );															\
		return FvZoneItemXMLFactory::Result( pkItem );												\
	}																								\
																									\
	delete pkItem;																					\
	return FvZoneItemXMLFactory::Result( NULL, kErrorString );										\
}

#define FV_IMPLEMENT_ZONE_ITEM_XML_ARGS (spSection)

#define FV_IMPLEMENT_ZONE_ITEM_XML_ALIAS( CLASS, ALIAS, PRIORITY )									\
	FvZoneItemXMLFactory CLASS::ms_k##ALIAS##Factory( #ALIAS, PRIORITY, CLASS::Create );

#endif // __FvZoneSerializerImpl_H__