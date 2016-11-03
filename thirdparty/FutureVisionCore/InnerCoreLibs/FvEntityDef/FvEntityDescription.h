//{future header message}
#ifndef __FvEntityDescription_H__
#define __FvEntityDescription_H__

#include <vector>
#include <float.h>

#include "FvEntityDefDefines.h"
#include "FvDataDescription.h"
#include "FvMethodDescription.h"
#include "FvBaseUserDataObjectDescription.h"

#include <FvNetTypes.h>
#include <FvXMLSection.h>
#include <FvMemoryStream.h>
#include <FvVector3.h>
#include <FvDirection3.h>


const float VOLATILE_ALWAYS = FLT_MAX;


class FV_ENTITYDEF_API FvVolatileInfo
{
public:
	FvVolatileInfo();
	bool Parse( FvXMLSectionPtr pSection );

	bool ShouldSendPosition() const	{ return m_fPositionPriority > 0.f; }
	FvInt32 DirType( float priority ) const;

	bool IsLessVolatileThan( const FvVolatileInfo & info ) const;
	bool IsValid() const;
	bool HasVolatile( float priority ) const;

	float	m_fPositionPriority;
	float	m_fYawPriority;
	float	m_fPitchPriority;
	float	m_fRollPriority;

private:
	float AsPriority( FvXMLSectionPtr pSection ) const;
};

bool operator==( const FvVolatileInfo & info1, const FvVolatileInfo & info2 );
bool operator!=( const FvVolatileInfo & info1, const FvVolatileInfo & info2 );

FV_IOSTREAM_IMP_BY_MEMCPY(FV_INLINE, FvVolatileInfo)


class FvMD5;


class FV_ENTITYDEF_API FvDataLoDLevel
{
public:
	FvDataLoDLevel();

	float Low() const				{ return m_fLow; }
	float High() const				{ return m_fHigh; }

	float Start() const				{ return m_fStart; }
	float Hyst() const				{ return m_fHyst; }

	void Set( const FvString & label, float start, float hyst )
	{
		m_kLabel = label;
		m_fStart = start;
		m_fHyst = hyst;
	}

	const FvString & Label() const	{ return m_kLabel; }

	void Finalise( FvDataLoDLevel * pPrev, bool isLast );

	FvInt32 Index() const			{ return m_iIndex; }
	void Index( FvInt32 i )			{ m_iIndex = i; }

	enum
	{
		OUTER_LEVEL = -2,
		NO_LEVEL = -1
	};

private:
	float m_fLow;
	float m_fHigh;
	float m_fStart;
	float m_fHyst;
	FvString m_kLabel;

	FvInt32 m_iIndex;
};


class FV_ENTITYDEF_API FvDataLoDLevels
{
public:
	FvDataLoDLevels();
	bool AddLevels( FvXMLSectionPtr pSection );

	FvInt32 Size() const;
	const FvDataLoDLevel & GetLevel( FvInt32 i ) const	{ return m_akLevel[i]; }

	FvDataLoDLevel *  Find( const FvString & label );
	bool FindLevel( FvInt32 & level, FvXMLSectionPtr pSection ) const;

	bool NeedsMoreDetail( FvInt32 level, float priority ) const;
	bool NeedsLessDetail( FvInt32 level, float priority ) const;

private:
	FvDataLoDLevel m_akLevel[ FV_MAX_DATA_LOD_LEVELS + 1 ];

	FvInt32 m_iSize;
};


class FvAddToStreamVisitor;
class FvEntityAttributes;

class FV_ENTITYDEF_API FvIDataDescriptionVisitor
{
public:
	virtual ~FvIDataDescriptionVisitor() {};

	virtual bool Visit( const FvDataDescription& propDesc ) = 0;
};


class FV_ENTITYDEF_API FvEntityDescription: public FvBaseUserDataObjectDescription
{
public:
	typedef std::vector< FvMethodDescription >	FvMethodList;

	class PropertyEventStamps
	{
	public:
		void Init( const FvEntityDescription & entityDescription );
		void Init( const FvEntityDescription & entityDescription, FvEventNumber lastEventNumber );

		void Set( const FvDataDescription & dataDescription, FvEventNumber eventNumber );
		FvEventNumber Get( const FvDataDescription & dataDescription ) const;

		void AddToStream( FvBinaryOStream & stream ) const;
		void RemoveFromStream( FvBinaryIStream & stream );

	private:
		typedef std::vector< FvEventNumber > Stamps;
		Stamps m_kEventStamps;
	};

	class FvMethods
	{
	public:
		bool Init( FvXMLSectionPtr pMethods, FvMethodDescription::eComponent component, const char * interfaceName );
		void CheckExposedForSubSlots();
		void Supersede();

		FvUInt32 Size() const;
		FvUInt32 ExposedSize() const { return (FvUInt32)m_kExposedMethods.size(); }

		FvMethodDescription * InternalMethod( FvUInt32 index ) const;
		FvMethodDescription * ExposedMethod( FvUInt8 topIndex, FvBinaryIStream & data ) const;
		FvMethodDescription * Find( const FvString & name ) const;

		FvMethodList & InternalMethods()	{ return m_kInternalMethods; }
		const FvMethodList & InternalMethods() const	{ return m_kInternalMethods; }

		void Dump(FvXMLSectionPtr pSection);

#if FV_ENABLE_WATCHERS
		static FvWatcherPtr pWatcher();
#endif
	private:
		typedef std::map< FvString, FvUInt32 > Map;
		typedef FvMethodList List;

		Map		m_kMap;
		List	m_kInternalMethods;

		std::vector< FvUInt32 >	m_kExposedMethods;
	};

public:
	FvEntityDescription();
	~FvEntityDescription();

	bool	Parse( const FvString & name, FvXMLSectionPtr pSection = NULL, bool isFinal = true );
	void	Supersede( FvMethodDescription::eComponent component );

	enum DataDomain
	{
		FV_BASE_DATA   = 0x1,
		FV_CLIENT_DATA = 0x2,
		FV_CELL_DATA   = 0x4,
		FV_EXACT_MATCH = 0x8,
		FV_ONLY_OTHER_CLIENT_DATA = 0x10,
		FV_ONLY_PERSISTENT_DATA = 0x20
	};

	bool	AddSectionToStream( FvXMLSectionPtr pSection, FvBinaryOStream & stream, FvInt32 dataDomains ) const;
	bool	AddSectionToDictionary( FvXMLSectionPtr pSection, FvEntityAttributes* pDict, FvInt32 dataDomains ) const;
	bool	AddDictionaryToStream( const FvEntityAttributes* pDict, FvBinaryOStream & stream, FvInt32 dataDomains ) const;
	bool	AddAttributesToStream( const FvEntityAttributes* pDict, FvBinaryOStream & stream, FvInt32 dataDomains, FvInt32 * pDataSizes = NULL, FvInt32 numDataSizes = 0 ) const;

	bool	ReadStreamToDict( FvBinaryIStream & stream, FvInt32 dataDomains, FvEntityAttributes* pDict ) const;
	bool	ReadStreamToSection( FvBinaryIStream & stream, FvInt32 dataDomains, FvXMLSectionPtr pSection ) const;

	bool	Visit( FvInt32 dataDomains, FvIDataDescriptionVisitor & visitor ) const;

	FvEntityTypeID			Index() const;
	void					Index( FvEntityTypeID index );

	FvEntityTypeID			ClientIndex() const;
	void					ClientIndex( FvEntityTypeID index );
	const FvString&			ClientName() const;
	void					SetParent( const FvEntityDescription & parent );

	bool					HasCellScript() const { return m_bHasCellScript; }
	bool					HasBaseScript() const { return m_bHasBaseScript; }
	bool					HasGlobalScript() const { return m_bHasGlobalScript; }
	bool					HasClientScript() const { return m_bHasClientScript; }//! TODO: 加标签 和 判断是否有对应域的属性或方法
	bool					IsClientOnlyType() const { return !m_bHasCellScript && !m_bHasBaseScript && !m_bHasGlobalScript; }
	bool					IsClientType() const { return m_kName == m_kClientName; }

	const FvVolatileInfo &	VolatileInfo() const;

	FvUInt32				ClientServerPropertyCount() const;
	FvDataDescription*		ClientServerProperty( FvUInt32 n ) const;

	FvUInt32				ClientMethodCount() const;
	FvMethodDescription*	ClientMethod( FvUInt8 n, FvBinaryIStream & data ) const;

	FvUInt32				ExposedBaseMethodCount() const { return this->Base().ExposedSize(); }
	FvUInt32				ExposedCellMethodCount() const { return this->Cell().ExposedSize(); }

	FvUInt32				NumEventStampedProperties() const;

	FvMethodDescription*	FindClientMethod( const FvString& name ) const;

	bool					CompressMethodId(int methodId, int& baseId, int& extId) const;
	bool					DecompressMethodId(int messageId, int& methodId, FvBinaryIStream& data) const;

#ifdef FV_SERVER
	const FvDataLoDLevels &	LodLevels() const { return m_kLodLevels; }
#endif

	const FvMethods &		Cell() const	{ return m_kCell; }
	const FvMethods &		Base() const	{ return m_kBase; }
	const FvMethods &		Client() const	{ return m_kClient; }
	const FvMethods &		Global() const	{ return m_kGlobal; }

	void					AddToMD5( FvMD5 & md5 ) const;
	void					AddPersistentPropertiesToMD5( FvMD5 & md5 ) const;

	void					Dump(FvXMLSectionPtr pSection);

#if FV_ENABLE_WATCHERS
	static FvWatcherPtr pWatcher();
#endif
protected:
	virtual bool			ParseInterface( FvXMLSectionPtr pSection, const char * interfaceName );
	virtual bool			ParseImplements( FvXMLSectionPtr pInterfaces );
	virtual	bool			ParseProperties( FvXMLSectionPtr pProperties );

	virtual const FvString	GetDefsDir() const;
	virtual const FvString	GetShortDefsDir() const;
	virtual const FvString	GetClientDir() const;
	virtual const FvString	GetCellDir() const;
	virtual const FvString	GetBaseDir() const;

private:
	bool					ParseClientMethods( FvXMLSectionPtr pMethods, const char * interfaceName );
	bool					ParseCellMethods( FvXMLSectionPtr pMethods, const char * interfaceName );
	bool					ParseBaseMethods( FvXMLSectionPtr pMethods, const char * interfaceName );
	bool					ParseGlobalMethods( FvXMLSectionPtr pMethods, const char * interfaceName );

	bool					AddToStream( const FvAddToStreamVisitor & visitor,
										FvBinaryOStream & stream,
										FvInt32 dataTypes,
										FvInt32 * pDataSizes = NULL,
		   								FvInt32 numDataSizes = 0 ) const;

	//! TODO: 以下两个静态函数只会在AddToStream和Visit中使用,但效率太低,可以将其展开
	static bool				ShouldConsiderData( FvInt32 pass, const FvDataDescription * pDD, FvInt32 dataDomains );
	static bool				ShouldSkipPass( FvInt32 pass, FvInt32 dataDomains );

	typedef std::vector< FvUInt32 >	PropertyIndices;

	FvEntityTypeID		m_uiIndex;
	FvEntityTypeID		m_uiClientIndex;
	FvString			m_kClientName;
	bool				m_bHasCellScript;
	bool				m_bHasBaseScript;
	bool				m_bHasClientScript;
	bool				m_bHasGlobalScript;
	FvVolatileInfo 		m_kVolatileInfo;

	PropertyIndices		m_kClientServerProperties;

	FvMethods			m_kCell;
	FvMethods			m_kBase;
	FvMethods			m_kClient;
	FvMethods			m_kGlobal;

	FvUInt32			m_uiNumEventStampedProperties;

#ifdef FV_SERVER
	FvDataLoDLevels		m_kLodLevels;
#endif

#ifdef FV_EDITOR_ENABLED
	FvString			m_kEditorModel;
#endif

	bool	m_bHasClientProp;
	bool	m_bHasBaseProp;
	bool	m_bHasCellProp;
};


class FV_ENTITYDEF_API FvPropertyEventCallBack
{
public:
	virtual void HandlePropertyEvent(FvDataDescription* pkDataDes, FvObj* pkOldVal, FvObj* pkNewVal, FvUInt8 uiMessageID, FvBinaryIStream& kMsg) = 0;
	bool IsNeedAddToStream() { return m_bNeedAddToStream; }

	FvPropertyEventCallBack(bool bNeedAddToStream):m_bNeedAddToStream(bNeedAddToStream) {}

protected:
	bool	m_bNeedAddToStream;
};


class FV_ENTITYDEF_API FvEntityAttributes : public FvPropertyOwnerBase
{
public:
	FvEntityAttributes(const FvEntityDescription& entityDesc);
	~FvEntityAttributes();

	FvVector3&	GetPos() { return m_kPos; }
	const FvVector3&GetPos() const { return m_kPos; }
	FvDirection3&GetDir() { return m_kDir; }
	const FvDirection3&GetDir() const { return m_kDir; }
	FvInt32		GetSpaceID() const { return m_iSpaceID; }
	FvUInt16	GetSpaceType() const { return m_uiSpaceType; }
	FvObjPtr	GetAttribut(const FvString& name) const;
	FvObjPtr	GetAttribut(const char* name) const;
	FvObjPtr	GetAttribut(FvUInt32 idx) const;
	void		SetPos(FvVector3& kPos) { m_kPos=kPos; }
	void		SetDir(FvDirection3& kDir) { m_kDir=kDir; }
	void		SetSpaceID(FvInt32 iSpaceID) { m_iSpaceID=iSpaceID; }
	void		SetSpaceType(FvUInt16 uiSpaceType) { m_uiSpaceType=uiSpaceType; }
	bool		SetAttribut(const FvString& name, FvObjPtr pObj);
	bool		SetAttribut(FvUInt32 idx, FvObjPtr pObj);

	bool		ReadStreamToPosDir(FvBinaryIStream& stream);
	void		AddPosDirToStream(FvBinaryOStream& stream) const;

	const FvEntityDescription& GetEntityDes() const { return m_kEntityDesc; }
	void		SetPropertyCallBack(FvPropertyEventCallBack* pkCallBack) { m_pkPropertyCallBack=pkCallBack; }
	void		OnPropertyEvent(FvUInt8 messageID, FvBinaryIStream& stream,
					FvUInt32& topLevelIdx, FvUInt32& pathDepth,
					FvPropertyOwnerBase::ChangeVal& oldVal, FvPropertyOwnerBase::ChangeVal& newVal);

	FvEntityAttributes* Copy() const;
	void		Dump( FvXMLSectionPtr pSection );
	void		DumpSizeAndCnt();

protected:
	virtual void PropertyChanged(FvPropertyOperator op, FvUInt32 opIdx,
		const ChangeVal& oldVal, const ChangeVal& newVal, ChangePath& path);
	virtual FvInt32 PropertyDivisions();
	virtual FvPropertyOwnerBase* PropertyVassal(FvUInt32 ref);
	virtual void PropertyRenovate(FvPropertyOperator op, FvUInt32 opIdx,
		FvBinaryIStream& data, ChangeVal& oldValue, ChangeVal& newValue);

protected:
	typedef std::vector<FvObjPtr> Attributs;
	Attributs					m_kAttributs;
	const FvEntityDescription&	m_kEntityDesc;
	FvPropertyEventCallBack*	m_pkPropertyCallBack;
	FvVector3					m_kPos;
	FvDirection3				m_kDir;
	FvInt32						m_iSpaceID;
	FvUInt16					m_uiSpaceType;
};


#include "FvEntityDescription.inl"


#endif // __FvEntityDescription_H__
