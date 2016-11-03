#include "FvZoneParticles.h"
#include "FvZone.h"
#include "FvCamera.h"
#include "FvSceneNode.h"
#include "FvZoneVisitor.h"

#include <FvDebug.h>

FvUInt32 FvZoneParticles::ms_uiLightIdentify = 0;

int iZoneParticlesToken;

FV_IMPLEMENT_ZONE_ITEM_XML( FvZoneParticles, particles, 0 )

FvZoneParticles::FvZoneParticles() :
FvZoneItem( (WantFlags)(WANTS_VISIT) ),
m_bReflectionVisible( false ),
m_pkParticleNode(NULL),
m_pkParticleSystem(NULL)
{
	ms_uiLightIdentify++;
	char pcIdentify[9];
	sprintf_s(pcIdentify,9,"%I32x",FvInt32(ms_uiLightIdentify));
	m_kParticleSystemName = "FvParticleSystem_";
	m_kParticleSystemName += pcIdentify;

	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);
	m_pkParticleNode = pkSceneManager->createSceneNode();
}

FvZoneParticles::~FvZoneParticles()
{
	ParticleUniverse::ParticleSystemManager *pkManager = 
		ParticleUniverse::ParticleSystemManager::getSingletonPtr();
	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkManager && pkSceneManager);

	if(m_pkParticleSystem)
		m_pkParticleSystem->stop();

	pkManager->destroyParticleSystem(m_pkParticleSystem, pkSceneManager);

	if(m_pkParticleNode)
		pkSceneManager->destroySceneNode(m_pkParticleNode);
}

void FvZoneParticles::Visit( FvCamera *pkCamera, FvZoneVisitor *pkVisitor )
{
	if(!m_pkParticleNode)
		return;

	if(pkVisitor->HasFlag(FvZoneVisitor::VISIT_FLAG_WATER_REFLECTION))
		return;

	bool bOnlyShadowCaster = pkVisitor->HasFlag(FvZoneVisitor::VISIT_FLAG_SHADOW_CASTER);
	if(bOnlyShadowCaster)
		return;

	m_pkParticleNode->_update(true,false);
	m_pkParticleNode->_findVisibleObjects(
		pkCamera,pkVisitor->GetRenderQueue(),
		pkVisitor->GetVisibleObjectsBoundsInfo(),
		true,false,bOnlyShadowCaster);
}

void FvZoneParticles::Toss( FvZone *pkZone )
{
	// FV_GUARD;
	this->FvZoneItem::Toss( pkZone );

	FvMatrix kMatrix = m_kLocalTransform;
	if (m_pkZone != NULL)
	{
		kMatrix.PostMultiply( m_pkZone->Transform() );
		this->SetMatrix(kMatrix);

		if(m_pkParticleNode)
			static_cast<FvSceneNode*>(m_pkParticleNode)->SetZone(m_pkZone,true);
	}
}

bool FvZoneParticles::Load( FvXMLSectionPtr spSection )
{
	// FV_GUARD;
	m_kLocalTransform = spSection->ReadMatrix34( "transform", FvMatrix::IDENTITY );

	m_kResourceID = spSection->ReadString( "resource" );
	if (m_kResourceID.empty())
		return false;

	ParticleUniverse::ParticleSystemManager *pkManager = 
		ParticleUniverse::ParticleSystemManager::getSingletonPtr();
	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkManager && pkSceneManager);

	m_pkParticleSystem = pkManager->createParticleSystem(m_kParticleSystemName,
		m_kResourceID, pkSceneManager);

	if(m_pkParticleSystem == NULL)
		return false;

	FV_ASSERT(m_pkParticleNode);

	m_pkParticleSystem->setRenderQueueGroup(89);
	m_pkParticleNode->attachObject(m_pkParticleSystem);
	m_pkParticleNode->_update(true,true);
	m_pkParticleSystem->start();

	return true;
}

void FvZoneParticles::SetMatrix( const FvMatrix &kMatrix )	
{ 
	m_kWorldTransform = kMatrix; 
	if(m_pkParticleNode)
	{
		Ogre::Vector3 kPosition,kScale;
		FvQuaternion kRotation;
		FvMatrixDecomposeWorld4f((float*)&kRotation,(float*)&kScale,(float*)&kPosition,
			(float*)&kMatrix);
		m_pkParticleNode->setPosition(kPosition);
		m_pkParticleSystem->setScale(kScale);
		m_pkParticleNode->setOrientation(Ogre::Quaternion(kRotation.w,kRotation.x,kRotation.y,kRotation.z));
	}
}

bool FvZoneParticles::AddZBounds( FvBoundingBox &kBound ) const
{
	kBound.AddZBounds( m_kLocalTransform.ApplyToOrigin().z );
	return true;
}