#include "FvZoneWater.h"

#include "FvZone.h"
#include "FvZoneObstacle.h"
#include "FvZoneSpace.h"
#include "FvZoneManager.h"

#if FV_UMBRA_ENABLE
#include "FvZoneTerrain.h"
#include "FvZoneUmbra.h"
#include <umbraModel.hpp>
#include <umbraObject.hpp>
#include "umbra_proxies.hpp"
#endif // FV_UMBRA_ENABLE

int iZoneWaterToken;

FvZoneWater::FvZoneWater( FvString uid ) :
	FvVeryLargeObject( uid, "water" ),
	m_pkWater( NULL ),
	m_pkZone( NULL )
{
}

FvZoneWater::FvZoneWater( ) :
	m_pkWater( NULL )	
{
	m_kUID = "";
	m_kType = "water";
}

FvZoneWater::~FvZoneWater()
{
	if (m_pkWater != NULL)
	{
		FvWater::DeleteWater(m_pkWater);
		m_pkWater = NULL;
	}
}

bool FvZoneWater::Load( FvXMLSectionPtr spSection, FvZone *pkZone )
{
	// FV_GUARD;
	if (pkZone==NULL)
		return false;

	if (m_pkWater != NULL)
		ShouldRebuild(true);

	FvXMLSectionPtr spReqSec;

	spReqSec = spSection->OpenSection( "position" );
	if (!spReqSec)
		m_kConfig.m_kPosition = pkZone->BoundingBox().Centre();
	else
		m_kConfig.m_kPosition = spReqSec->AsVector3();

	float fLORI = spSection->ReadFloat( "orientation", 0.f );
	FvVector3 kOriVec( sinf( fLORI ), cosf( fLORI ), 0.f );
	m_kConfig.m_fOrientation = atan2f( kOriVec.x, kOriVec.y );

	spReqSec = spSection->OpenSection( "size" );
	if (!spReqSec) return false;

	FvVector3 kSizeV3 = spReqSec->AsVector3();
	m_kConfig.m_kSize = FvVector2( kSizeV3.x, kSizeV3.y );

	m_kConfig.m_fFresnelConstant = spSection->ReadFloat( "fresnelConstant", 0.3f );
	m_kConfig.m_fFresnelExponent = spSection->ReadFloat( "fresnelExponent", 5.f );

	m_kConfig.m_kReflectionTint = spSection->ReadVector4( "reflectionTint", FvVector4(1,1,1,1) );
	m_kConfig.m_fReflectionScale = spSection->ReadFloat( "reflectionStrength", 0.04f );

	m_kConfig.m_kRefractionTint = spSection->ReadVector4( "refractionTint", FvVector4(1,1,1,1) );
	m_kConfig.m_fRefractionScale = spSection->ReadFloat( "refractionStrength", 0.04f );

	m_kConfig.m_fTessellation = spSection->ReadFloat( "tessellation", 10.f );
	m_kConfig.m_fConsistency = spSection->ReadFloat( "consistency", 0.95f );

	m_kConfig.m_fTextureTessellation = spSection->ReadFloat( "textureTessellation", m_kConfig.m_fTessellation);
	

	float oldX = spSection->ReadFloat( "scrollSpeedX", -1.f );	
	float oldY = spSection->ReadFloat( "scrollSpeedY", 1.f );

	m_kConfig.m_kScrollSpeed1 = spSection->ReadVector2( "scrollSpeed1", FvVector2(oldX,0.5f) );	
	m_kConfig.m_kScrollSpeed2 = spSection->ReadVector2( "scrollSpeed2", FvVector2(oldY,0.f) );	
	m_kConfig.m_kWaveScale = spSection->ReadVector2( "waveScale", FvVector2(1.f,0.75f) );

	m_kConfig.m_fWindVelocity = spSection->ReadFloat( "windVelocity", 0.02f );

	m_kConfig.m_fSunPower = spSection->ReadFloat( "sunPower", 32.f );
	m_kConfig.m_fSunScale = spSection->ReadFloat( "sunScale", 1.0f );

	m_kConfig.m_kWaveTexture = spSection->ReadString( "waveTexture", "system/maps/waves2.dds" );

	m_kConfig.m_fSimCellSize = spSection->ReadFloat( "cellsize", 100.f );
	m_kConfig.m_fSmoothness = spSection->ReadFloat( "smoothness", 0.f );

	m_kConfig.m_kFoamTexture = spSection->ReadString( "foamTexture", "system/maps/water_foam2.dds" );	
	
	m_kConfig.m_kReflectionTexture = spSection->ReadString( "reflectionTexture", "system/maps/cloudyhillscubemap2.dds" );

	m_kConfig.m_kDeepColour = spSection->ReadVector4( "deepColour", FvVector4(0.f,0.20f,0.33f,1.f) );

	m_kConfig.m_fDepth = spSection->ReadFloat( "depth", 10.f );
	m_kConfig.m_fFadeDepth = spSection->ReadFloat( "fadeDepth", 0.f );

	m_kConfig.m_fFoamIntersection = spSection->ReadFloat( "foamIntersection", 0.25f );
	m_kConfig.m_fFoamMultiplier = spSection->ReadFloat( "foamMultiplier", 0.75f );
	m_kConfig.m_fFoamTiling = spSection->ReadFloat( "foamTiling", 1.f );	

	m_kConfig.m_bUseEdgeAlpha = spSection->ReadBool( "useEdgeAlpha", true );
	
	m_kConfig.m_bUseCubeMap = spSection->ReadBool( "useCubeMap", false );	
	
	m_kConfig.m_bUseSimulation = spSection->ReadBool( "useSimulation", true );

	m_kConfig.m_uiVisibility = spSection->ReadInt( "visibility", FvWater::ALWAYS_VISIBLE );
	if ( m_kConfig.m_uiVisibility != FvWater::ALWAYS_VISIBLE &&
		m_kConfig.m_uiVisibility != FvWater::INSIDE_ONLY &&
		m_kConfig.m_uiVisibility != FvWater::OUTSIDE_ONLY )
	{
		m_kConfig.m_uiVisibility = FvWater::ALWAYS_VISIBLE;
	}


	m_kConfig.m_kTransparencyTable = pkZone->Mapping()->Path() + m_kUID + ".odata";

	m_pkZone = pkZone;
	return true;
}
#if FV_UMBRA_ENABLE
class FvZoneMirror : public FvReferenceCount
{
public:
	FvZoneMirror(const std::vector<FvVector3>& vertices, const std::vector<FvUInt32>& triangles, Chunk* pChunk) :
		m_pkZone(pChunk),
		m_pkPortal(NULL)
	{
		// FV_GUARD;
		Umbra::MeshModel* model = Umbra::MeshModel::create((Umbra::FvVector3*)&vertices[0], (Umbra::Vector3i*)&triangles[0], vertices.size(), triangles.size()/3);
		model->autoRelease();
		model->set(Umbra::Model::BACKFACE_CULLABLE, true);

		Umbra::VirtualPortal* umbraPortalA = Umbra::VirtualPortal::create(model, NULL);
		Umbra::VirtualPortal* umbraPortalB = Umbra::VirtualPortal::create(model, umbraPortalA);

		umbraPortalA->setCell(pChunk->getUmbraCell());
		umbraPortalB->setCell(pChunk->getUmbraCell());

		umbraPortalA->setTargetPortal(umbraPortalB);
		umbraPortalB->set(Umbra::Object::ENABLED, false);

		umbraPortalA->set(Umbra::Object::INFORM_PORTAL_ENTER, true);
		umbraPortalA->set(Umbra::Object::INFORM_PORTAL_EXIT, true);

		umbraPortalA->setStencilModel(model);
		umbraPortalA->set(Umbra::Object::FLOATING_PORTAL, true);

		m_pkPortal = new UmbraPortal(vertices, triangles, m_pkZone);
		m_pkPortal->reflectionPortal_ = true;

		umbraPortalA->setUserPointer(m_pkPortal);

		FvVector3 p = vertices[triangles[0]];
		FvVector3 da = (vertices[triangles[1]]-vertices[triangles[0]]);
		FvVector3 db = (vertices[triangles[2]]-vertices[triangles[0]]);
		FvVector3 normal;
		normal.crossProduct(da, db);

		da.normalise();
		db.normalise();
		normal.normalise();

		FvMatrix warp;
		warp.setIdentity();
		warp.setTranslate(p);
		warp[0] = da;
		warp[1] = db;
		warp[2] = normal;

		umbraPortalA->setWarpMatrix((Umbra::Matrix4x4&)warp);
		
		warp[2] = -warp[2];
		umbraPortalB->setWarpMatrix((Umbra::Matrix4x4&)warp);
		
		m_spUmbraPortalA = UmbraObjectProxy::get( umbraPortalA );
		m_spUmbraPortalB = UmbraObjectProxy::get( umbraPortalB );
	}

	~FvZoneMirror()
	{
		// FV_GUARD;
		m_spUmbraPortalA = NULL;
		m_spUmbraPortalB = NULL;
		if (m_pkPortal)
			delete m_pkPortal;
	}

private:

	FvZone *m_pkZone;
	UmbraObjectProxyPtr	m_spUmbraPortalA;
	UmbraObjectProxyPtr	m_spUmbraPortalB;
	UmbraPortal *m_pkPortal;
};
#endif

void FvZoneWater::SyncInit(FvZoneVLO *pkVLO)
{
	// FV_GUARD;
#if FV_UMBRA_ENABLE
	if (pkVLO && pkVLO->Zone())
	{
		FvVector2 xy = m_kConfig.m_kSize * 0.5f;

		std::vector<FvVector3> v(4);

		v[0].Set( -xy.x, -xy.y, 0.f );
		v[1].Set( -xy.x,  xy.y, 0.f );
		v[2].Set(  xy.x,  xy.y, 0.f );
		v[3].Set(  xy.x, -xy.y, 0.f );

		FvMatrix m;
		m.SetRotateY( m_kConfig.m_fOrientation );
		m.PostTranslateBy( m_kConfig.m_kPosition );

		for (int i = 0; i < 4; i++)
			v[i] = m.ApplyPoint(v[i]);

		std::vector<FvUInt32> tris(3*2);
		tris[0] = 0;
		tris[1] = 1;
		tris[2] = 2;

		tris[3] = 0;
		tris[4] = 2;	
		tris[5] = 3;

		m_spMirrorA = new FvZoneMirror(v, tris, pkVLO->Zone() );


		tris[0] = 0;
		tris[1] = 2;
		tris[2] = 1;

		tris[3] = 0;
		tris[4] = 3;	
		tris[5] = 2;

		m_spMirrorB = new FvZoneMirror(v, tris, pkVLO->Zone());
	}
	else
	{
		m_spMirrorA = NULL;
		m_spMirrorB = NULL;
	}
#endif
}

FvBoundingBox FvZoneWater::ZoneBB( FvZone *pkZone )
{
	// FV_GUARD;
	FvBoundingBox kBB = FvBoundingBox::ms_kInsideOut;
	FvBoundingBox kCBB = pkZone->BoundingBox();

	FvVector3 kSize( m_kConfig.m_kSize.x * 0.5f, m_kConfig.m_kSize.y * 0.5f, 0 );
	FvBoundingBox kWBB( -kSize, kSize );

	FvMatrix kMat;
	kMat.SetRotateZ( m_kConfig.m_fOrientation );
	kMat.PostTranslateBy( m_kConfig.m_kPosition );

	kWBB.TransformBy( kMat );
    
	if (kWBB.Intersects( kCBB ))
	{
		kBB.SetBounds( 
			FvVector3(	std::max( kWBB.MinBounds().x, kCBB.MinBounds().x ),
						std::max( kWBB.MinBounds().y, kCBB.MinBounds().y ),
						std::max( kWBB.MinBounds().z, kCBB.MinBounds().z ) ),
			FvVector3(	std::min( kWBB.MaxBounds().x, kCBB.MaxBounds().x ),
						std::min( kWBB.MaxBounds().y, kCBB.MaxBounds().y ),
						std::min( kWBB.MaxBounds().z, kCBB.MaxBounds().z ) ) );
		kBB.TransformBy( pkZone->TransformInverse() );
	}

	return kBB;
}


bool FvZoneWater::AddZBounds( FvBoundingBox &kBB ) const
{
	// FV_GUARD;
	kBB.AddZBounds( m_kConfig.m_kPosition.z );

	return true;
}

void FvZoneWater::Visit( FvZoneSpace *pkSpace, FvCamera *pkCamera, FvZoneVisitor *pkVisitor )
{
	// FV_GUARD;
	if(pkVisitor->HasFlag(FvZoneVisitor::VISIT_FLAG_SHADOW_CASTER) ||
		pkVisitor->HasFlag(FvZoneVisitor::VISIT_FLAG_WATER_REFLECTION) ||
		pkVisitor->HasFlag(FvZoneVisitor::VISIT_FLAG_PICK))
		return;

	if (m_pkWater == NULL)
	{
		m_pkWater = new FvWater( m_kConfig, new FvZoneEnvironmentTerrainCollider( /*pSpace*/ ) );	

		this->ObjectCreated();
	}
	else if ( ShouldRebuild() )
	{
		m_pkWater->Rebuild( m_kConfig );
		ShouldRebuild(false);
		this->ObjectCreated();
	}

	if ( !ms_bSimpleDraw )
	{
		FvWaters::AddToDrawList( m_pkWater );
	}
}

#if FV_UMBRA_ENABLE
void FvZoneWater::Lend( FvZone *pkZone )
{
	// FV_GUARD;
	if (!m_pkWater)
	{
		m_pkWater = new FvWater( m_kConfig, new FvZoneEnvironmentTerrainCollider( /*pSpace*/ ) );
		this->ObjectCreated();
	}

	if (pkZone != NULL)
	{
		FvZoneTerrain *pkZoneTerrain =
			FvZoneTerrainCache::Instance( *pkZone ).ZoneTerrain();

		if (pkZoneTerrain != NULL)
			m_pkWater->AddTerrainItem( pkZoneTerrain );
	}
}

void FvZoneWater::Unlend( FvZone *pkZone )
{
	// FV_GUARD;
	if (m_pkWater)
	{
		if (pkZone != NULL)
		{
			FvZoneTerrain *pkZoneTerrain =
				FvZoneTerrainCache::Instance( *pkZone ).ZoneTerrain();

			if (pkZoneTerrain != NULL)
				m_pkWater->EraseTerrainItem( pkZoneTerrain );
		}
	}
}

#endif // FV_UMBRA_ENABLE

void FvZoneWater::Sway( const FvVector3 &kSrc, const FvVector3 &kDest, const float fDiameter )
{
	// FV_GUARD;
	if (m_pkWater != NULL)
	{
		m_pkWater->AddMovement( kSrc, kDest, fDiameter );
	}
}


#ifdef EDITOR_ENABLED
void FvZoneWater::Dirty()
{
	// FV_GUARD;
	if (m_pkWater != NULL)
		ShouldRebuild(true);
}
#endif //EDITOR_ENABLED

bool FvZoneWater::Create( FvZone *pkZone, FvXMLSectionPtr spSection, FvString kUID )
{
	// FV_GUARD;
	FvZoneWater *pkItem = new FvZoneWater( kUID );	
	if (pkItem->Load( spSection, pkZone ))
		return true;
	delete pkItem;
	return false;
}


void FvZoneWater::SimpleDraw( bool bState )
{
	FvZoneWater::ms_bSimpleDraw = bState;
}


FvVLOFactory FvZoneWater::ms_kFactory( "water", 0, FvZoneWater::Create );
bool FvZoneWater::ms_bSimpleDraw = false;
