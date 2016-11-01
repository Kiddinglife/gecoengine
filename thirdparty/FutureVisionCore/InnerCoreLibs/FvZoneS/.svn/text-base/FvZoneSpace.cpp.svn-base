#include "FvZoneSpace.h"
#include "FvZone.h"
#include "FvZoneManager.h"
#include "FvGridTraversal.h"
#include "FvZoneObstacle.h"
#include "FvZoneSerializer.h"

#include <FvWorldTriangle.h>
#include <FvBoundingBox.h>
#include <FvWatcher.h>
#include <FvDebug.h>

#include <OgreResourceGroupManager.h>

#ifndef FV_EDITOR_ENABLED
extern bool g_bLoadingState;
#endif

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

template <class X> class FvSweepShape
{

};

template <class X>
float ZoneSpaceCollide(  const FvZoneSpace::ColumnGrid &kCurrentFocus,
						 const FvSweepShape<X> &kStart,
						 const FvVector3 &kEnd,
						 FvCollisionCallback &kCallback )
{
	FvZoneObstacle::NextMark();

	FvBoundingBox kShapeBox;
	kStart.BoundingBox( kShapeBox );

	FvVector3 kShapeRange = kShapeBox.MaxBounds() - kShapeBox.MinBounds();
	float fShapeRad = kShapeRange.Length() * 0.5f;

	FvVector3 kBSource = kShapeBox.MinBounds();
	FvVector3 kBExtent = kBSource + (kEnd - kStart.Leader());

	FvVector3 kCSource = kBSource + kShapeRange * 0.5f;
	FvVector3 kCExtent = kCSource + (kEnd - kStart.Leader());

	FvSpaceGridTraversal kSGT( kBSource, kBExtent, kShapeRange, FV_GRID_RESOLUTION );

	const FvZoneObstacle *pkObstacle;

	FvCollisionState kCollisionState( kCallback );

	do
	{
		bool InSpan = kCurrentFocus.InSpan( kSGT.m_iSX, kSGT.m_iSY );
		const FvZoneSpace::Column *pkCol = kCurrentFocus( kSGT.m_iSX, kSGT.m_iSY );

		if (InSpan && pkCol != NULL)
		{
			const FvObstacleTree &kTree = pkCol->Obstacles();

			FvObstacleTreeTraversal kHTT = kTree.Traverse(
				kCSource + kSGT.m_kDir * (kSGT.m_fCellSTravel - fShapeRad),
				kCSource + kSGT.m_kDir * (kSGT.m_fCellETravel + fShapeRad),
				fShapeRad );

			while ((pkObstacle =
				static_cast< const FvZoneObstacle *>( kHTT.Next() )) != NULL)
			{
				if (pkObstacle->Mark()) continue;
				if (pkObstacle->Zone() == NULL) continue;

				FvVector3 kSTr, kETr;
				const FvMatrix &kTrInv = pkObstacle->m_kTransformInverse;
				kTrInv.ApplyPoint( kSTr, kCSource );
				kTrInv.ApplyPoint( kETr, kCExtent );

				int iBax = 0;
				float fBAbs = fabsf( kETr[0] - kSTr[0] );
				float fAAbs;
				fAAbs = fabsf( kETr[1] - kSTr[1] );
				if (fAAbs > fBAbs) { fBAbs = fAAbs; iBax = 1; }
				fAAbs = fabsf( kETr[2] - kSTr[2] );
				if (fAAbs > fBAbs) { fBAbs = fAAbs; iBax = 2; }

				float fSTrba = kSTr[iBax], dTrba = kETr[iBax] - kSTr[iBax];

				if ( !pkObstacle->ClipAgainstBB( kSTr, kETr,
					kStart.TransformRangeToRadius( kTrInv, kShapeRange ) + 0.01f ))
					continue;

				kCollisionState.m_fSTravel = (kSTr[iBax] - fSTrba) / dTrba * kSGT.m_fFullDist;
				kCollisionState.m_fETravel = (kETr[iBax] - fSTrba) / dTrba * kSGT.m_fFullDist;

				if (kCollisionState.m_bOnlyLess && kCollisionState.m_fSTravel > kCollisionState.m_fDist) continue;
				if (kCollisionState.m_bOnlyMore && kCollisionState.m_fETravel < kCollisionState.m_fDist) continue;

				X kShape;
				FvVector3 kShapeEnd;
				kStart.Transform( kShape, kShapeEnd, kTrInv,
					kCollisionState.m_fSTravel, kCollisionState.m_fETravel, kSGT.m_kDir, kSTr, kETr );
				if (pkObstacle->Collide( kShape, kShapeEnd, kCollisionState ))
					return kCollisionState.m_fDist;
			}
		}
	}
	while ((!kCollisionState.m_bOnlyLess || (kCollisionState.m_fDist + fShapeRad > kSGT.m_fCellETravel)) && kSGT.Next());

	return kCollisionState.m_fDist;
}

template <>
class FvSweepShape<FvVector3>
{
public:
	FvSweepShape( const FvVector3 & pt ) : m_kPT( pt ) {}

	const FvVector3 &Leader() const
	{
		return m_kPT;
	}

	void BoundingBox( FvBoundingBox & bb ) const
	{
		bb.SetBounds( m_kPT, m_kPT );
	}

	void Transform( FvVector3 & shape, FvVector3 & end,
		const FvMatrix &, float, float, const FvVector3 &,
		const FvVector3 & bbCenterAtSDistTransformed,
		const FvVector3 & bbCenterAtEDistTransformed ) const
	{
		shape = bbCenterAtSDistTransformed;
		end = bbCenterAtEDistTransformed;
	}

	inline static const float TransformRangeToRadius( const FvMatrix & /*trInv*/,
		const FvVector3 & /*shapeRange*/ )
	{
		return 0.f;
	}

private:
	FvVector3 m_kPT;
};

float FvZoneSpace::Collide( const FvVector3 &kStart, const FvVector3 &kEnd,
						  FvCollisionCallback &kCallback ) const
{
	//FV_GUARD;
	FV_ASSERT_DEV( -100000.f < kStart.x && kStart.x < 100000.f &&
		-100000.f < kStart.y && kStart.y < 100000.f );
	FV_ASSERT_DEV( -100000.f < kEnd.x && kEnd.x < 100000.f &&
		-100000.f < kEnd.y && kEnd.y < 100000.f );

	return ZoneSpaceCollide(
		m_kCurrentFocus, FvSweepShape<FvVector3>(kStart), kEnd, kCallback );
}

template <> class FvSweepShape<FvWorldTriangle>
{
public:
	FvSweepShape( const FvWorldTriangle &kWT ) : m_kWorldTriangle( kWT ) {}

	const FvVector3 &Leader() const
	{
		return m_kWorldTriangle.Point0();
	}

	void BoundingBox( FvBoundingBox &kBB ) const
	{
		kBB.SetBounds( m_kWorldTriangle.Point0(), m_kWorldTriangle.Point0() );
		kBB.AddBounds( m_kWorldTriangle.Point1() );
		kBB.AddBounds( m_kWorldTriangle.Point2() );
	}

	void Transform( FvWorldTriangle &kShape, FvVector3 &kEnd,
		const FvMatrix &kTR, float fSDist, float fEDist, const FvVector3 &kDir,
		const FvVector3&, const FvVector3& ) const
	{
		FvVector3 kOff = kDir * fSDist;
		kShape = FvWorldTriangle(
			kTR.ApplyPoint( m_kWorldTriangle.Point0() + kOff ),
			kTR.ApplyPoint( m_kWorldTriangle.Point1() + kOff ),
			kTR.ApplyPoint( m_kWorldTriangle.Point2() + kOff ) );
		kEnd = kTR.ApplyPoint( m_kWorldTriangle.Point0() + kDir * fEDist );
	}

	inline static const float TransformRangeToRadius( const FvMatrix &kTrInv,
		const FvVector3 &kShapeRange )
	{
		const FvVector3 kClipRange = kTrInv.ApplyVector( kShapeRange );
		return kClipRange.Length() * 0.5f;
	}

private:
	FvWorldTriangle m_kWorldTriangle;
};

float FvZoneSpace::Collide( const FvWorldTriangle &kStart,
						  const FvVector3 &kEnd, FvCollisionCallback &kCallback ) const
{
	//FV_GUARD;
	return ZoneSpaceCollide(
		m_kCurrentFocus, FvSweepShape<FvWorldTriangle>(kStart), kEnd, kCallback );
}

FvVectorNoDestructor<FvSpaceGridTraversal::CellSpec> FvSpaceGridTraversal::ms_kAltCells;

//extern FV_THREADLOCAL(bool) g_bVersionPointSafe;

FvZoneDirMapping::FvZoneDirMapping( FvZoneSpacePtr spSpace, FvMatrix &kMatrix,
								 const FvString &kGroup, const FvString &kPath, FvXMLSectionPtr spSettings ) :
m_spSpace( spSpace ),
m_kMapper( kMatrix ),
m_kGroup( kGroup ),
m_kPath( kPath ),
m_kName( kPath.substr( 1 + FV_MIN(
	  kPath.find_last_of( '/' ), kPath.find_last_of( '\\' ) ) ) ),
m_bCondemned( false ),
m_bSingleDir( false )
{
	//FV_GUARD;
	if (!spSettings)
	{
		spSettings = this->OpenSettings( kGroup, kPath );
	}

	m_kInvMapper.Invert( m_kMapper );
	char pcAddrStr[32];
#ifndef _WIN32
	sprintf_s( pcAddrStr, sizeof( pcAddrStr ), "@%p", this );
#else
	sprintf_s( pcAddrStr, sizeof( pcAddrStr ), "@0x%p", this );
#endif

	m_kName += pcAddrStr;	

	if (spSettings)
	{
		FvXMLSectionPtr kBoundsSec = spSettings->OpenSection("bounds");
		m_iMinLGridX = kBoundsSec->ReadInt( "minX" );
		m_iMinLGridY = kBoundsSec->ReadInt( "minY" );
		m_iMaxLGridX = kBoundsSec->ReadInt( "maxX" );
		m_iMaxLGridY = kBoundsSec->ReadInt( "maxY" );
		FvBoundingBox sbb;
		sbb.SetBounds(	FvVector3( float(m_iMinLGridX), 
								   float(m_iMinLGridY) , 0 ) * FV_GRID_RESOLUTION,
						FvVector3( float(m_iMaxLGridX+1),
								   float(m_iMaxLGridY+1) , 0 ) * FV_GRID_RESOLUTION );

		sbb.TransformBy( m_kMapper );

		m_iMinGridX = int(floorf( sbb.MinBounds().x / FV_GRID_RESOLUTION + 0.5f ));
		m_iMaxGridX = int(floorf( sbb.MaxBounds().x / FV_GRID_RESOLUTION + 0.5f ) - 1);
		m_iMinGridY = int(floorf( sbb.MinBounds().y / FV_GRID_RESOLUTION + 0.5f));
		m_iMaxGridY = int(floorf( sbb.MaxBounds().y / FV_GRID_RESOLUTION + 0.5f) - 1);

		m_bSingleDir = spSettings->ReadBool( "singleDir", false );
	}
	else
	{
		m_spSpace = NULL;	
	}
}

FvZoneDirMapping::~FvZoneDirMapping()
{
	//FV_GUARD;
	if (m_spSpace)
	{
		FV_DEBUG_MSG( "FvZoneDirMapping::~FvZoneDirMapping: "
			"Deleted %s from space %u\n", m_kPath.c_str(), m_spSpace->ID() );
	}

	FV_ASSERT_DEV( this->Condemned() );
	m_spSpace = NULL;
}

FvXMLSectionPtr FvZoneDirMapping::OpenSettings( const FvString &kGroup, const FvString &kPath )
{
	//FV_GUARD;
	FvString kStr = kPath;// + FV_SPACE_SETTING_FILE_NAME;
	kStr.append("space.config");
	Ogre::DataStreamPtr spDataStream = Ogre::ResourceGroupManager::getSingleton().
		openResource(kStr,kGroup,true);

	if(spDataStream.isNull())
		return NULL;

	return FvXMLSection::OpenSection(spDataStream);
}

FvString FvZoneDirMapping::OutsideZoneIdentifier( const FvVector3 & kLocalPoint,
													bool bCheckBounds ) const
{
	//FV_GUARD;
	int iGridX = int(floorf( kLocalPoint.x / FV_GRID_RESOLUTION ));
	int iGridY = int(floorf( kLocalPoint.y / FV_GRID_RESOLUTION ));

	if (bCheckBounds)
	{
		if (iGridX < m_iMinLGridX || iGridX > m_iMaxLGridX ||
			iGridY < m_iMinLGridY || iGridY > m_iMaxLGridY)
			return FvString();
	}

	return FvZoneSerializer::OutsideZoneIdentifier( iGridX, iGridY, m_bSingleDir );
}

FvString FvZoneDirMapping::OutsideZoneIdentifier( int iX, int iY,
													bool bCheckBounds ) const
{
	//FV_GUARD;
	if (bCheckBounds)
	{
		if (iX < m_iMinLGridX || iX > m_iMaxLGridX ||
			iY < m_iMinLGridY || iY > m_iMaxLGridY)
			return FvString();
	}

	return FvZoneSerializer::OutsideZoneIdentifier( iX, iY, m_bSingleDir );
}

class HexLookup
{
public:
	HexLookup()
	{
		for( int i = 0; i < 256; ++i )
		{
			m_auiHexLookup[ i ] = 0;
			m_auiHexFactor[ i ] = 0;
		}

		for (unsigned char i='0'; i<='9'; i++)
		{
			m_auiHexLookup[i] = (FvUInt16)(i-'0');
			m_auiHexFactor[i] = 1;
		}

		for (unsigned char i='a'; i<='f'; i++)
		{
			m_auiHexLookup[i] = (FvUInt16)(i-'a' + 10);
			m_auiHexFactor[i] = 1;
		}

		for (unsigned char i='A'; i<='F'; i++)
		{
			m_auiHexLookup[i] = (FvUInt16)(i-'A' + 10);
			m_auiHexFactor[i] = 1;
		}
	};

	bool FromHex( const unsigned char* f, FvInt16& value )
	{
		value = (m_auiHexLookup[f[3]] +
			(m_auiHexLookup[f[2]]<<4) +
			(m_auiHexLookup[f[1]]<<8) +
			(m_auiHexLookup[f[0]]<<12));
		FvInt8 factor = m_auiHexFactor[f[0]] & m_auiHexFactor[f[1]] & m_auiHexFactor[f[2]] & m_auiHexFactor[f[3]];
		return (!!factor);
	}

	FvUInt16 m_auiHexLookup[256];
	FvUInt8 m_auiHexFactor[256];
};

/*static*/ bool FvZoneDirMapping::GridFromZoneName( const FvString &kZoneName, FvInt16 &uiX, FvInt16 &uiY )
{
	//FV_GUARD;
	static HexLookup hexLookup;

	const unsigned char* f = (const unsigned char*)kZoneName.c_str();
	while ( *f ) f++;

	if(*(f-1) == 'o')
	{
		return (hexLookup.FromHex(f-9,uiX) && hexLookup.FromHex(f-5,uiY));
	}

	return false;
}

FvZoneSpace::LoadMappingTask::LoadMappingTask(
		FvZoneSpacePtr spZoneSpace,
		FvZoneSpaceEntryID kMappingID,
		float *pfMatrix,
		const FvString& kGroup, 
		const FvString& kPath ) :
	m_spZoneSpace( spZoneSpace ),
	m_kMappingID( kMappingID ),
	m_kMatrix( *(FvMatrix *)pfMatrix ),
	m_kPath( kPath ),
	m_kGroup( kGroup ),
	m_spSettings( NULL )
{
}

void FvZoneSpace::LoadMappingTask::DoBackgroundTask( FvBGTaskManager& kMGR )
{
	//FV_GUARD;
	m_spSettings = FvZoneDirMapping::OpenSettings( m_kGroup, m_kPath );

	kMGR.AddMainThreadTask( this );
}

void FvZoneSpace::LoadMappingTask::DoMainThreadTask( FvBGTaskManager& kMGR )
{
	//FV_GUARD;
	if (m_spZoneSpace->ValidatePendingTask( this ))
	{
		if (m_spSettings)
		{
			m_spZoneSpace->AddMapping( m_kMappingID, m_kMatrix, m_kGroup, m_kPath, m_spSettings );
		}
		else
		{
			FV_ERROR_MSG( "FvZoneSpace::AddMappingAsync: "
				"No space.settings file for '%s'\n", m_kPath.c_str() );
		}
	}
}

FvZoneSpace::FvZoneSpace( FvZoneSpaceID uiID ) :
FvConfigZoneSpace( uiID ),
m_fClosestUnloadedZone( 0.f ),
m_iExtraMinX(0),
m_iExtraMaxX(0),
m_iExtraMinY(0),
m_iExtraMaxY(0),
m_stExtraPixelWidth(0),
m_stExtraPixelHeight(0),
m_stExtraPixelSize(0),
m_pkExtraData(NULL)
{
	//FV_GUARD;
	FvZoneManager::Instance().AddSpace( this );
}

FvZoneSpace::~FvZoneSpace()
{
	//FV_GUARD;
	FvZoneManager::Instance().DelSpace( this );
	FV_SAFE_DELETE(m_pkExtraData)
}

void FvZoneSpace::AddMappingAsync( FvZoneSpaceEntryID kMappingID, float *pfMatrix,
					 const FvString &kGroup, const FvString &kPath )
{
	//FV_GUARD;
	FvBackgroundTaskPtr spTask =
		new LoadMappingTask( this, kMappingID, pfMatrix, kGroup, kPath );

	m_kBackgroundTasks.insert( spTask.GetObject() );

	FvBGTaskManager::Instance().AddBackgroundTask( spTask );
}

FvZoneDirMapping * FvZoneSpace::AddMapping( FvZoneSpaceEntryID kMappingID,
										 float *pfMatrix, 
										 const FvString &kGroup, 
										 const FvString &kPath,
										 FvXMLSectionPtr spSettings)
{
	//FV_GUARD;
	FvMatrix kMatrix = *(FvMatrix*)pfMatrix;	

	FvZoneDirMapping *pkMapping =
		new FvZoneDirMapping( this, kMatrix, kGroup, kPath, spSettings );

	if (!pkMapping->pSpace())
	{
		FV_ERROR_MSG( "FvZoneSpace::AddMapping: "
			"No space settings file found in %s\n", kPath.c_str() );
		pkMapping->Condemn();
		delete pkMapping;
		return NULL;
	}

	const FvVector3 &kXA = kMatrix.ApplyToUnitAxisVector( 0 );
	const FvVector3 &kTR = kMatrix.ApplyToOrigin();
	FV_DEBUG_MSG( "FvZoneSpace::AddMapping: "
		"Adding %s at (%f,%f,%f) xaxis (%f,%f,%f) to space %u\n",
		kPath.c_str(), kTR.x, kTR.y, kTR.z, kXA.x, kXA.y, kXA.z, this->ID() );

	pkMapping->IncRef();
	m_kMappings.insert( std::make_pair( kMappingID, pkMapping ) );


	FvZoneMap::iterator it;
	for (it = m_kCurrentZones.begin(); it != m_kCurrentZones.end(); it++)
	{
		for (unsigned int i = 0; i < it->second.size(); i++)
		{
			FvZone * pkZone = it->second[i];
			if (!pkZone->Online()) continue;

			pkZone->ResolveExterns();
		}
	}

	this->RecalcGridBounds();

	if (!spSettings)
	{
		spSettings = FvZoneDirMapping::OpenSettings( kGroup, kPath );
	}

	m_spTerrainSettings = new FvTerrainSettings;
	FvXMLSectionPtr spTerrainSettingsData = spSettings->OpenSection("terrain") ;

	m_spTerrainSettings->Init(spTerrainSettingsData);

	FvXMLSectionPtr spExtraData = spSettings->OpenSection("extraData");
	if(m_pkExtraData == NULL && spExtraData)
	{
		FvString kExtraDataFile = spExtraData->ReadString("file");
		Ogre::DataStreamPtr spDataStream;
		try
		{
			spDataStream = Ogre::ResourceGroupManager::getSingleton().
				openResource(kPath + kExtraDataFile,kGroup,true);
		}
		catch (...){}

		m_iExtraMinX = spExtraData->ReadInt("minX");
		m_iExtraMaxX = spExtraData->ReadInt("maxX");
		m_iExtraMinY = spExtraData->ReadInt("minY");
		m_iExtraMaxY = spExtraData->ReadInt("maxY");
		m_stExtraPixelWidth = (m_iExtraMaxX - m_iExtraMinX) + 1;
		m_stExtraPixelHeight = (m_iExtraMaxY - m_iExtraMinY) + 1;
		m_stExtraPixelSize = spExtraData->ReadInt("pixelSize");
		size_t stDataSize = m_stExtraPixelWidth * m_stExtraPixelHeight * m_stExtraPixelSize;

		if(!spDataStream.isNull() && 
			spDataStream->size() == stDataSize)
		{
			m_pkExtraData = new char[stDataSize];
			spDataStream->read(m_pkExtraData,stDataSize);
		}
		else
		{
			FV_ERROR_MSG("FvZoneSpace::AddMapping extra data file %s format error!",
				kExtraDataFile.c_str());
		}
	}

	return pkMapping;
}

FvZoneDirMapping * FvZoneSpace::GetMapping( FvZoneSpaceEntryID mappingID )
{
	//FV_GUARD;
	ZoneDirMappings::iterator found = m_kMappings.find( mappingID );
	if (found == m_kMappings.end())
		return NULL;

	return found->second;
}

//PROFILER_DECLARE( ChunkSpace_delMapping1, "ChunkSpace_delMapping1" );
//PROFILER_DECLARE( ChunkSpace_delMapping2, "ChunkSpace_delMapping2" );

#if PROFILE_D3D_RESOURCE_RELEASE
extern void DumpD3DResourceReleaseResults();
extern bool g_bDoProfileD3DResourceRelease;
#endif

void FvZoneSpace::DelMapping( FvZoneSpaceEntryID mappingID )
{
	//FV_GUARD;	
#ifndef FV_SERVER
	//ScopedSyncMode scopedSyncMode;
#endif//FV_SERVER

#if PROFILE_D3D_RESOURCE_RELEASE
	g_bDoProfileD3DResourceRelease = true;
#endif

	ZoneDirMappings::iterator found = m_kMappings.find( mappingID );
	if (found == m_kMappings.end()) return;

	FvZoneDirMapping * pMapping = found->second;
	//m_kMappingsLock.grab();
	m_kMappings.erase( found );
	//m_kMappingsLock.give();
	pMapping->Condemn();

	FV_DEBUG_MSG( "FvZoneSpace::DelMapping: Comdemned %s in space %u\n",
		pMapping->Path().c_str(), this->ID() );

	std::vector<FvZone*> kComdemnedZones;
	FvZoneMap::iterator it;

	for (it = m_kCurrentZones.begin(); it != m_kCurrentZones.end(); it++)
	{
		for (unsigned int i = 0; i < it->second.size(); i++)
		{
			FvZone * pkZone = it->second[i];

			if (pkZone->Mapping() != pMapping) continue;
			if (pkZone->Loading()) continue;	

			{
				//PROFILER_SCOPED( ChunkSpace_delMapping1 );
				if (pkZone->Online()) pkZone->Loose( false );
			}

			{
				//PROFILER_SCOPED( ChunkSpace_delMapping2 );
				if (pkZone->Loaded()) pkZone->Eject();
			}

			kComdemnedZones.push_back( pkZone );
		}
	}

	for (it = m_kCurrentZones.begin(); it != m_kCurrentZones.end(); it++)
	{
		for (unsigned int i = 0; i < it->second.size(); i++)
		{
			FvZone * pkZone = it->second[i];
			if (pkZone->Mapping() == pMapping) continue;

			if (!pkZone->Online()) continue;

			pkZone->ResolveExterns( pMapping );
		}
	}

	for (unsigned int i = 0; i < kComdemnedZones.size(); i++)
		delete kComdemnedZones[i];

	this->RecalcGridBounds();
	pMapping->DecRef();	

#if PROFILE_D3D_RESOURCE_RELEASE
	g_bDoProfileD3DResourceRelease = false;
	DumpD3DResourceReleaseResults();
#endif
}

void FvZoneSpace::Clear()
{
	//FV_GUARD;
	FV_DEBUG_MSG( "FvZoneSpace::Clear: Clearing space %u\n", this->ID() );

	FV_ASSERT_DEV( this->RefCount() != 0 );
	FvZoneSpacePtr pThis = this;

	this->FvConfigZoneSpace::Clear();

	while (!m_kMappings.empty())
		this->DelMapping( m_kMappings.begin()->first );

	m_kBackgroundTasks.clear();

	if (this == FvZoneManager::Instance().CameraSpace())
	{
		FvZoneManager::Instance().Camera( FvMatrix::IDENTITY, NULL );
	}

	m_fClosestUnloadedZone = 0.0f;
}

FvZone * FvZoneSpace::FindZoneFromPoint( const FvVector3 & kPoint )
{
	//FV_GUARD;

	FvVector3 kPT = kPoint;
	kPT.z += 0.0001f;
	Column * pCol = this->pColumn( kPT, false );
	return (pCol != NULL) ? pCol->FindZone( kPT ) : NULL;
}

FvZoneSpace::Column * FvZoneSpace::pColumn( const FvVector3 & kPoint, bool bCanCreate )
{
	//FV_GUARD;
	int iX = PointToGrid( kPoint.x );
	int iY = PointToGrid( kPoint.y );

	if (!m_kCurrentFocus.InSpan( iX, iY ))
	{
		return NULL;
	}

	Column * & rpCol = m_kCurrentFocus( iX, iY );

	if (!rpCol && bCanCreate)
	{
		rpCol = new Column( iX, iY );
	}

	return rpCol;
}

FvZoneSpace::Column * FvZoneSpace::pColumn(int iX, int iY)
{
	if (!m_kCurrentFocus.InSpan( iX, iY ))
	{
		return NULL;
	}

	Column * & rpCol = m_kCurrentFocus( iX, iY );

	return rpCol;
}
void FvZoneSpace::DumpDebug() const
{
	//FV_GUARD;
	const ColumnGrid & kFocus = m_kCurrentFocus;

	const int XBegin = kFocus.XBegin();
	const int XEnd = kFocus.XEnd();
	const int YBegin = kFocus.YBegin();
	const int YEnd = kFocus.YEnd();

	FV_DEBUG_MSG( "----- Total Size -----\n" );

	int total = 0;

	for (int y = YBegin; y < YEnd; y++)
	{
		for (int x = XBegin; x < XEnd; x++)
		{
			const Column * pCol = kFocus( x, y );

			int totalSize = pCol ? pCol->Size() : 0;
			total += totalSize;
			FvDPrintfNormal( "%8d ", totalSize );
		}

		FvDPrintfNormal( "\n" );
	}
	FV_DEBUG_MSG( "Total = %d\n", total );
	total = 0;

	FV_DEBUG_MSG( "----- Obstacle Size -----\n" );

	for (int y = YBegin; y < YEnd; y++)
	{
		for (int x = XBegin; x < XEnd; x++)
		{
			const Column * pCol = kFocus( x, y );

			int obstacleSize =
				pCol ? pCol->Obstacles().Size() : 0;
			total += obstacleSize;
			FvDPrintfNormal( "%8d ", obstacleSize );
		}

		FvDPrintfNormal( "\n" );
	}
	FV_DEBUG_MSG( "Total = %d\n", total );
}

FvBoundingBox FvZoneSpace::GridBounds() const
{
	return FvBoundingBox(
		FvVector3( GridToPoint( MinGridX() ), GridToPoint( MinGridY() ), FV_MIN_ZONE_HEIGHT ),
		FvVector3( GridToPoint( MaxGridX() + 1 ), GridToPoint( MaxGridY() + 1 ), FV_MAX_ZONE_HEIGHT )
		);
}

FvZone * FvZoneSpace::GuessZone( const FvVector3 &kPoint, bool bLookInside )
{
	//FV_GUARD;	

	FvZoneDirMapping * pBestMapping = NULL;
	FvString kBestZoneIdentifier;

	FvSimpleMutexHolder kSMH( m_kMappingsLock );

	if (m_kMappings.empty()) return NULL;

	for (ZoneDirMappings::iterator kIt = m_kMappings.begin();
		kIt != m_kMappings.end();
		kIt++)
	{
		FvZoneDirMapping *pkMapping = kIt->second;
		FvVector3 kLPoint = pkMapping->InvMapper().ApplyPoint( kPoint );
		int iGridMX = int(floorf( kLPoint.x / FV_GRID_RESOLUTION ));
		int iGridMY = int(floorf( kLPoint.y / FV_GRID_RESOLUTION ));
		int iGrido = bLookInside ? 1 : 0;

		FvString kZoneIdentifier;

		for (int iGridX = iGridMX-iGrido; iGridX <= iGridMX+iGrido; iGridX++)
			for (int iGridY = iGridMY-iGrido; iGridY <= iGridMY+iGrido; iGridY++)
			{	
				FvString kGridZoneIdentifier =
					pkMapping->OutsideZoneIdentifier( kLPoint );
				if (kGridZoneIdentifier.empty()) continue;
		
				Ogre::DataStreamPtr spDataStream = Ogre::ResourceGroupManager::getSingleton().
					openResource(pkMapping->Path() + kGridZoneIdentifier + ".zone",pkMapping->Group(),true);
				FvXMLSectionPtr spOutsideDS = FvXMLSection::OpenSection(spDataStream);
				if (!spOutsideDS)
				{
					continue;
				}

				if (iGridX == iGridMX && iGridY == iGridMY && kZoneIdentifier.empty())
					kZoneIdentifier = kGridZoneIdentifier;

				if (bLookInside)
				{
					std::vector<FvString> kOverlappers;
					spOutsideDS->ReadStrings( "overlapper", kOverlappers );
					for (unsigned int i = 0; i < kOverlappers.size(); i++)
					{
						Ogre::DataStreamPtr spDataStream = Ogre::ResourceGroupManager::getSingleton().
							openResource(pkMapping->Path() + kOverlappers[i] + ".zone",pkMapping->Group(),true);
						FvXMLSectionPtr spBBSect = FvXMLSection::OpenSection(spDataStream);
						spBBSect = spBBSect->OpenSection("boundingBox");
						if (!spBBSect) continue;

						FvBoundingBox kBB;
						kBB.SetBounds(	spBBSect->ReadVector3( "min" ),
							spBBSect->ReadVector3( "max" ) );
					
						if (kBB.Intersects( kLPoint ))
						{
							kZoneIdentifier = kOverlappers[i];
							pBestMapping = NULL;	
							break;
						}
					}
				}

			}	

			if (pBestMapping == NULL && !kZoneIdentifier.empty())
			{
				pBestMapping = pkMapping;
				kBestZoneIdentifier = kZoneIdentifier;

				if (!bLookInside) break;	
			}
	}

	if (pBestMapping != NULL)
	{
		FvZone * pkZone = new FvZone( kBestZoneIdentifier, pBestMapping );
		pBestMapping->IncRef();
		return pkZone;
	}

	return NULL;
}

void FvZoneSpace::RecalcGridBounds()
{
	//FV_GUARD;
	if (m_kMappings.empty())
	{
		m_iMinGridX = 0;
		m_iMinGridY = 0;
		m_iMaxGridX = 0;
		m_iMaxGridY = 0;
	}
	else
	{
		m_iMinGridX = 1000000000;
		m_iMinGridY = 1000000000;
		m_iMaxGridX = -1000000000;
		m_iMaxGridY = -1000000000;

		for (ZoneDirMappings::iterator it = m_kMappings.begin();
			it != m_kMappings.end();
			it++)
		{
			m_iMinGridX = FV_MIN( m_iMinGridX, it->second->MinGridX() );
			m_iMinGridY = FV_MIN( m_iMinGridY, it->second->MinGridY() );
			m_iMaxGridX = FV_MAX( m_iMaxGridX, it->second->MaxGridX() );
			m_iMaxGridY = FV_MAX( m_iMaxGridY, it->second->MaxGridY() );
		}
	}

	this->FvConfigZoneSpace::RecalcGridBounds();
}

void FvZoneSpace::Emulate( FvZoneSpacePtr pRightfulSpace )
{
	//FV_GUARD;
	FV_ASSERT_DEV( m_kMappings.size() == 1 );
	FV_ASSERT_DEV( pRightfulSpace->m_kMappings.size() == 1 );

	FvZoneDirMapping * pOwnMapping = m_kMappings.begin()->second;

	FV_ASSERT_DEV( pOwnMapping->Path() ==
		pRightfulSpace->m_kMappings.begin()->second->Path() );

	FvZoneManager & cm = FvZoneManager::Instance();
	cm.DelSpace( this );

	this->FvBaseZoneSpace::Emulate( pRightfulSpace );

	m_kMappings.clear();
	m_kMappings.insert( std::make_pair(
		pRightfulSpace->m_kMappings.begin()->first, pOwnMapping ) );

	this->RecalcGridBounds();

	cm.DelSpace( &*cm.Space( this->ID(), false ) );
	cm.AddSpace( this );
}

void FvZoneSpace::EjectLoadedZoneBeforeBinding( FvZone * pkZone )
{
	//FV_GUARD;
	FV_IF_NOT_ASSERT_DEV( pkZone->Loaded() && !pkZone->Online() )
	{
		return;
	}

	FV_DEBUG_MSG( "FvZoneSpace::EjectLoadedZoneBeforeBinding: %s\n",
		pkZone->Identifier().c_str() );

	bool ejectChunk = true;

	for (FvZoneBoundaries::iterator bit = pkZone->Joints().begin();
		bit != pkZone->Joints().end();
		bit++)
	{
		FvZoneBoundary::Portals::iterator pit;
		for (pit = (*bit)->m_kUnboundPortals.begin();
			pit != (*bit)->m_kUnboundPortals.end();
			pit++)
		{
			if ((*pit)->HasZone())
			{
				FvZoneDirMapping * pMapping = (*pit)->m_pkZone->Mapping();

				FV_IF_NOT_ASSERT_DEV( !(*pit)->m_pkZone->Ratified() )
				{
					ejectChunk = false;
					continue;
				}
				delete (*pit)->m_pkZone;
				(*pit)->m_pkZone = NULL;

				if (pMapping != pkZone->Mapping())
					pMapping->DecRef();
			}
		}
	}

	if( ejectChunk )
		pkZone->Eject();
}

void FvZoneSpace::IgnoreZone( FvZone * pkZone )
{
	//FV_GUARD;
	if (!pkZone->Online())
	{
		FV_ERROR_MSG( "FvZoneSpace::IgnoreZone: "
			"Attempted to ignore offline chunk '%s'\n",
			pkZone->Identifier().c_str() );
		return;
	}

	const FvVector3 & cen = pkZone->Centre();
	int nx = int(cen.x / FV_GRID_RESOLUTION);	if (cen.x < 0.f) nx--;
	int ny = int(cen.y / FV_GRID_RESOLUTION);	if (cen.y < 0.f) ny--;

	for (int x = nx-1; x <= nx+1; x++) for (int y = ny-1; y <= ny+1; y++)
	{
		if ( m_kCurrentFocus.InSpan( x, y ) )
		{
			Column *& rpCol = m_kCurrentFocus( x, y );
			if (rpCol != NULL)
			{
				delete rpCol;
				rpCol = NULL;
			}
		}
	}

	this->RemoveFromBlurred( pkZone );

}

void FvZoneSpace::NoticeZone( FvZone * pkZone )
{
	//FV_GUARD;
	this->BlurredZone( pkZone );
}

bool FvZoneSpace::ValidatePendingTask( FvBackgroundTask* pkTask )
{
	//FV_GUARD;
	return m_kBackgroundTasks.erase( pkTask ) != 0;
}

//TerrainSettingsPtr FvZoneSpace::pTerrainSettings() const
//{
//	return m_spTerrainSettings;
//}