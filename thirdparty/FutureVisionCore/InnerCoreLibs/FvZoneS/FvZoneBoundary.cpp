#include "FvZoneBoundary.h"
#include "FvZone.h"
#include "FvZoneSpace.h"
#include "FvZoneVisitor.h"

#include <FvPortal2D.h>
#include <FvWatcher.h>
#include <FvDebug.h>

FV_DECLARE_DEBUG_COMPONENT2( "FvZone", 0 )

bool FvZoneBoundary::Portal::ms_bDrawPortals = true;

FvZoneBoundary::FvZoneBoundary()
{
	// FV_GUARD;
	m_kPlane = FvPlane( FvVector3(0,0,0), 0 );
}

FvZoneBoundary::~FvZoneBoundary()
{
	// FV_GUARD;
	for (Portals::iterator kPIt = m_kBoundPortals.begin();
		kPIt != m_kBoundPortals.end();
		kPIt++)
	{
		delete *kPIt;
	}

	for (Portals::iterator kPIt = m_kUnboundPortals.begin();
		kPIt != m_kUnboundPortals.end();
		kPIt++)
	{
		delete *kPIt;
	}
}

void FvZoneBoundary::BindPortal( FvUInt32 uiUnboundIndex )
{
	// FV_GUARD;
	Portal *pkPortal = m_kUnboundPortals[uiUnboundIndex];
	m_kBoundPortals.push_back( pkPortal );
	m_kUnboundPortals.erase( m_kUnboundPortals.begin() + uiUnboundIndex );
}

void FvZoneBoundary::LoosePortal( FvUInt32 uiBoundIndex )
{
	// FV_GUARD;
	Portal *pkPortal = m_kBoundPortals[uiBoundIndex];
	m_kUnboundPortals.push_back( pkPortal );
	m_kBoundPortals.erase( m_kBoundPortals.begin() + uiBoundIndex );
}

void FvZoneBoundary::AddInvasivePortal( Portal *pkPortal )
{
	// FV_GUARD;
	m_kUnboundPortals.push_back( pkPortal );
}

void FvZoneBoundary::SplitInvasivePortal( FvZone *pkZone, unsigned int uiIDX )
{
	// FV_GUARD;
	return;
}

FvZoneBoundary::Portal::Portal(FvZoneBoundary* pkBoundary) :
m_bInternal( false ),
m_bPermissive( true ),
m_pkZone( NULL )
#if FV_UMBRA_ENABLE
,m_pkUmbraPortal( NULL )
#endif
{
	FV_ASSERT(pkBoundary);
	// FV_GUARD;
	m_pkBoundary = pkBoundary;
}

FvZoneBoundary::Portal::~Portal()
{
#if FV_UMBRA_ENABLE
	ReleaseUmbraPortal();
#endif
}

bool FvZoneBoundary::Portal::ResolveExtern( FvZone *pkOwnZone )
{
	// FV_GUARD;
	FvVector3 kConPt = pkOwnZone->Transform().ApplyPoint(
		m_kLCentre + m_kPlane.Normal() * -0.1f );
	FvZone *pkExternZone = pkOwnZone->Space()->GuessZone( kConPt, true );
	if (pkExternZone != NULL)
	{
		if (pkExternZone->Mapping() != pkOwnZone->Mapping())
		{
			m_pkZone = pkExternZone;
			return true;
		}
		else
		{
			delete pkExternZone;
			pkOwnZone->Mapping()->DecRef();
		}
	}

	return false;
}

#ifndef FV_SERVER

class FvPortal2DStore
{
public:
	
	static FvPortal2DRef Grab()
	{
		// FV_GUARD;
		if (ms_kPortalStore.empty())
		{
			FvPortal2D *pkPortal = new FvPortal2D();
			for (int i = 0; i < 8; i++) pkPortal->AddPoint( FvVector2::ZERO );
			ms_kPortalStore.push_back( pkPortal );
		}

		FvPortal2DRef kPRef;
		kPRef.m_pkVal = ms_kPortalStore.back();
		ms_kPortalStore.pop_back();
		FvPortal2DStore::Grab( kPRef.m_pkVal );
		kPRef->ErasePoints();
		return kPRef;
	}

	static void Fini()
	{
		// FV_GUARD;
		for (unsigned int i=0; i < ms_kPortalStore.size(); i++)
		{
			delete ms_kPortalStore[i];
		}
		ms_kPortalStore.clear();
	}

private:

	static void Grab( FvPortal2D *pkPortal )
	{
		// FV_GUARD;
		pkPortal->Refs( pkPortal->Refs() + 1 );
	}

	static void Give( FvPortal2D *pkPortal )
	{
		// FV_GUARD;
		if (pkPortal == NULL) return;
		pkPortal->Refs( pkPortal->Refs() - 1 );
		if (pkPortal->Refs() == 0)
		{
			ms_kPortalStore.push_back( pkPortal );
		}
	}

	static FvVectorNoDestructor<FvPortal2D*> ms_kPortalStore;

	friend class FvPortal2DRef;
};

FvVectorNoDestructor<FvPortal2D*> FvPortal2DStore::ms_kPortalStore;

FvPortal2DRef::FvPortal2DRef( bool bValid ) :
m_pkVal( bValid ? NULL : (FvPortal2D*)-1 )
{
}

FvPortal2DRef::FvPortal2DRef( const FvPortal2DRef &kOther ) :
m_pkVal( kOther.m_pkVal )
{
	// FV_GUARD;
	if (unsigned int(m_pkVal)+1 > 1)
		FvPortal2DStore::Grab( m_pkVal );
}

FvPortal2DRef & FvPortal2DRef::operator =( const FvPortal2DRef &kOther )
{
	// FV_GUARD;
	if (unsigned int(m_pkVal)+1 > 1)
		FvPortal2DStore::Give( m_pkVal );
	m_pkVal =kOther.m_pkVal;
	if (unsigned int(m_pkVal)+1 > 1)
		FvPortal2DStore::Grab( m_pkVal );
	return *this;
}

FvPortal2DRef::~FvPortal2DRef()
{
	// FV_GUARD;
	if (unsigned int(m_pkVal)+1 > 1)
		FvPortal2DStore::Give( m_pkVal );
}

static struct FvClipOutsideToPortal
{
	FvClipOutsideToPortal() : m_bB( false )
	{ 
		//FV_WATCH( "Render/clipOutsideToPortal", b_, Watcher::WT_READ_WRITE,
		//	"Clip outdoor chunks to indoor portals" ); 
	}

	operator bool() { return m_bB; }
	bool m_bB;
} s_clipOutsideToPortal;


/*static*/ void FvZoneBoundary::Fini()
{
	// FV_GUARD;
	FvPortal2DStore::Fini();

}

bool FvZoneBoundary::Portal::Inside( const FvVector3 &kPoint ) const
{
	// FV_GUARD;
	FvVector3 rel = kPoint - m_kOrigin;
	FvVector2 p( rel.DotProduct( m_kUAxis ), rel.DotProduct( m_kVAxis ) );

	for (FvUInt32 i = 0; i < m_kPoints.size(); i++)
	{
		const FvVector2& p1 = m_kPoints[i];
		const FvVector2& p2 = m_kPoints[(i+1) % m_kPoints.size()];

		FvVector2 diff(p1.y - p2.y, p2.x - p1.x);

		if (diff.DotProduct( p - p1 ) < 0.f)
		{
			return false;
		}
	}
	return true;
}

void FvZoneBoundary::Portal::ObjectSpacePoint( int iIDX, FvVector3 &kRet )
{
	kRet = m_kUAxis * m_kPoints[iIDX][0] + m_kVAxis * m_kPoints[iIDX][1] + m_kOrigin;
}

void FvZoneBoundary::Portal::ObjectSpacePoint( int iIDX, FvVector4 &kRet )
{
	kRet = FvVector4(m_kUAxis * m_kPoints[iIDX][0] + m_kVAxis * m_kPoints[iIDX][1] + m_kOrigin, 1);
}

FvUInt32 FvZoneBoundary::Portal::Outcode( const FvVector3 &kPoint ) const
{
	// FV_GUARD;
	FvUInt32 uiRes = 0;
	FvVector3 kRel = kPoint - m_kOrigin;
	FvVector2 kP( kRel.DotProduct( m_kUAxis ), kRel.DotProduct( m_kVAxis ) );

	for (FvUInt32 i = 0; i < m_kPoints.size(); i++)
	{
		const FvVector2 &kP1 = m_kPoints[i];
		const FvVector2 &kP2 = m_kPoints[(i+1) % m_kPoints.size()];

		FvVector2 kDiff(kP1.y - kP2.y, kP2.x - kP1.x);

		if (kDiff.DotProduct( kP - kP1 ) < 0.f)
		{
			uiRes |= 1 << i;
		}
	}
	return uiRes;
}

#if FV_UMBRA_ENABLE

void FvZoneBoundary::Portal::CreateUmbraPortal(FvZone *pkOwner)
{
	// FV_GUARD;
	ReleaseUmbraPortal();

	if (pkOwner->GetUmbraCell() == m_pkZone->GetUmbraCell())
		return;

	Umbra::Model* model = CreateUmbraPortalModel();
	model->autoRelease();

	m_pkUmbraPortal = Umbra::PhysicalPortal::create( model, m_pkZone->GetUmbraCell() );
	m_pkUmbraPortal->set(Umbra::Object::INFORM_PORTAL_ENTER, true);
	m_pkUmbraPortal->set(Umbra::Object::INFORM_PORTAL_EXIT, true);
	UmbraPortal* umbraPortal = new UmbraPortal(pkOwner);
	m_pkUmbraPortal->setUserPointer( (void*)umbraPortal );
	m_pkUmbraPortal->setObjectToCellMatrix( (Umbra::Matrix4x4&)pkOwner->Transform() );
	m_pkUmbraPortal->setCell( pkOwner->GetUmbraCell() );
}

void FvZoneBoundary::Portal::ReleaseUmbraPortal()
{
	// FV_GUARD;
	if (m_pkUmbraPortal)
	{
		UmbraPortal* up = (UmbraPortal*)m_pkUmbraPortal->GetUserPointer();
		delete up;
		m_pkUmbraPortal->Release();
		m_pkUmbraPortal = NULL;
	}
}

Umbra::Model* FvZoneBoundary::Portal::CreateUmbraPortalModel()
{
	// FV_GUARD;
	FvUInt32 nVertices = m_kPoints.size();

	std::vector<FvVector3> vertices;
	vertices.reserve(nVertices);

	for( FvUInt32 i = 0; i < nVertices; i++ )
	{
		vertices.push_back(m_kUAxis * m_kPoints[i][0] + m_kVAxis * m_kPoints[i][1] + m_kOrigin);
	}

	int nTriangles = nVertices - 2;

	std::vector<FvUInt32> triangles;
	triangles.reserve(nTriangles*3);

	MF_ASSERT_DEV(nTriangles > 0);

	for (FvUInt32 c = 2; c < nVertices; c++)
	{
		triangles.push_back( 0 );
		triangles.push_back( c-1 );
		triangles.push_back( c );
	}

	Umbra::Model* model = Umbra::MeshModel::create((Umbra::FvVector3*)&vertices.front(), (Umbra::Vector3i*)&triangles.front(), nVertices, nTriangles);
	model->set(Umbra::Model::BACKFACE_CULLABLE, true);
	return model;
}
#endif // FV_UMBRA_ENABLE

#ifdef FV_ENABLE_DRAW_PORTALS
void FvZoneBoundary::Portal::Visit( FvZoneVisitor *pkVisitor,
												const FvMatrix &kTransform, float fInset )
{
	// FV_GUARD;
	if(mRenderOp.vertexData == NULL)
	{
		FvVector3 kWorldPoint[16];
		FvVector3 kWorldAvgPt;

		FvVector2 kAvgPt(0.f,0.f);
		for( FvUInt32 i = 0; i < m_kPoints.size(); i++ ) kAvgPt += m_kPoints[i];
		kAvgPt /= float(m_kPoints.size());

		for( FvUInt32 i = 0; i < m_kPoints.size(); i++ )
		{
			kTransform.ApplyPoint( kWorldPoint[i], FvVector3(
				m_kUAxis * m_kPoints[i][0] +
				m_kVAxis * m_kPoints[i][1] +
				m_kOrigin ) );
			mBox.merge(*(Ogre::Vector3*)&kWorldPoint[i]);
		}

		kTransform.ApplyPoint(kWorldAvgPt, FvVector3(
			m_kUAxis * kAvgPt[0] +
			m_kVAxis * kAvgPt[1] +
			m_kOrigin ) );

		mRenderOp.vertexData = OGRE_NEW Ogre::VertexData();
		static const FvUInt16 POSITION_BINDING = 0;
		mRenderOp.indexData = 0;
		mRenderOp.vertexData->vertexCount = m_kPoints.size() + 2;
		mRenderOp.vertexData->vertexStart = 0;
		mRenderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_FAN; 
		mRenderOp.useIndexes = false;
		Ogre::VertexDeclaration *pkDecl = mRenderOp.vertexData->vertexDeclaration;
		Ogre::VertexBufferBinding *pkBind = mRenderOp.vertexData->vertexBufferBinding;
		pkDecl->addElement(POSITION_BINDING, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
		Ogre::HardwareVertexBufferSharedPtr spPositionBuffer = 
			Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			pkDecl->getVertexSize(POSITION_BINDING),
			mRenderOp.vertexData->vertexCount,
			Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
		pkBind->setBinding(POSITION_BINDING, spPositionBuffer);
		Ogre::Vector3 *pkPoint = static_cast<Ogre::Vector3*>(spPositionBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD));

		pkPoint[0] = *(Ogre::Vector3*)&kWorldAvgPt;

		for( FvUInt32 i = 0; i < m_kPoints.size(); i++ )
		{
			pkPoint[i + 1] = *(Ogre::Vector3*)&kWorldPoint[i];
		}

		pkPoint[m_kPoints.size() + 1] = *(Ogre::Vector3*)&kWorldPoint[0];

		spPositionBuffer->unlock();

		char kMaterialName[64];
		sprintf_s(kMaterialName,50,"FvPortalMaterial_%d",FvUInt32(this));
		Ogre::MaterialPtr spMaterial = Ogre::MaterialManager::getSingleton().getByName("BaseWhiteNoLighting");
		spMaterial->load();
		m_pMaterial = spMaterial->clone(kMaterialName);
		m_pMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(true);

		m_pMaterial->getTechnique(0)->getPass(0)->setAmbient(
			Ogre::ColourValue(0.5f,0.0f,0.0f,0.0f));
		m_pMaterial->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_ADD);
		m_pMaterial->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
		m_pMaterial->getTechnique(0)->getPass(0)->setPolygonMode(Ogre::PM_WIREFRAME);
	}

	if(pkVisitor->GetRenderQueue())
		pkVisitor->GetRenderQueue()->addRenderable(this,1);
}
#endif // !FV_ENABLE_DRAW_PORTALS
#endif // !FV_SERVER

FvVector3 FvZoneBoundary::Portal::ObjectSpacePoint( int i )
{
	return m_kUAxis * m_kPoints[i][0] + m_kVAxis * m_kPoints[i][1] + m_kOrigin;
}

void FvZoneBoundary::Portal::Transform( const FvMatrix &kTransform )
{
	m_kWPoints.clear();
	for(FvUInt32 i = 0; i < m_kPoints.size(); i++)
		m_kWPoints.push_back(kTransform.ApplyPoint(this->ObjectSpacePoint(i)));

	m_kCentre = kTransform.ApplyPoint( m_kLCentre );

	m_kWNormal = kTransform.ApplyVector(m_kPlane.Normal());
}

void FvPrivPortal::AddPoints( const std::vector<FvVector3> &kPoints )
{
	// FV_GUARD;
	const float IN_PORTAL_PLANE	= 0.2f;

	m_kPoints.assign( kPoints.begin(), kPoints.end() );

	std::vector<FvVector3>::size_type i = 0;

	for (i = 0; i < m_kPoints.size() - 2; ++i)
	{
		m_kPlaneEquation.Init(  m_kPoints[ i ], m_kPoints[ i + 1 ], m_kPoints[ i + 2 ] );

		if (! FvAlmostEqual( m_kPlaneEquation.Normal(), FvVector3( 0.f, 0.f, 0.f ) ) )
		{
			break;
		}
	}

	bool bFoundOne = true;

	while (bFoundOne)
	{
		for (i = 0; i < m_kPoints.size() && bFoundOne; ++i)
		{
			bFoundOne = false;

			if (fabs( m_kPlaneEquation.DistanceTo( m_kPoints[ i ] ) ) > IN_PORTAL_PLANE)
			{
				FV_ERROR_MSG( "FvPrivPortal::AddPoints: found a point that is not on the portal plane.\n" );
				m_kPoints.erase( m_kPoints.begin() + i );
				bFoundOne = true;
				break;
			}
		}
	}

	bFoundOne = true;

	while (bFoundOne)
	{
		bFoundOne = false;

		for (i = 0; i < m_kPoints.size(); ++i)
		{
			FvVector3 v0 = m_kPoints[ ( i + m_kPoints.size() ) % m_kPoints.size() ];
			FvVector3 v1 = m_kPoints[ ( i + 1 + m_kPoints.size() ) % m_kPoints.size() ];
			FvVector3 v2 = m_kPoints[ ( i + 2 + m_kPoints.size() ) % m_kPoints.size() ];
			FvVector3 n1 = v0 - v1;
			FvVector3 n2 = v1 - v2;
			n1.Normalise();
			n2.Normalise();

			if (FvAlmostEqual( v0, v1 ) || 
				FvAlmostEqual( v1, v2 ) ||
				FvAlmostEqual( n1, n2 )) 
			{
				m_kPoints.erase( m_kPoints.begin() + i + 1 );
				bFoundOne = true;
				break;
			}
		}
	}
}

void FvPrivPortal::Flag( const FvString &kFlag )
{
	m_iFlags |= int( kFlag == "heaven" ) << 1;
	m_iFlags |= int( kFlag == "earth" ) << 2;
	m_iFlags |= int( kFlag == "invasive" ) << 3;
}

void FvPrivPortal::Transform( const class FvMatrix &kTransform )
{
	// FV_GUARD;
	FvVector3 kPos = kTransform.ApplyPoint( m_kPlaneEquation.Normal() * m_kPlaneEquation.Distance() );
	FvVector3 kNorm = kTransform.ApplyVector( m_kPlaneEquation.Normal() );
	kNorm.Normalise();
	m_kPlaneEquation = FvPlane( kNorm, kPos.DotProduct( kNorm ) );

	for( FvUInt32 i = 0; i < m_kPoints.size(); i++ )
	{
		m_kPoints[ i ] = kTransform.ApplyPoint( m_kPoints[ i ] );
	}
}
