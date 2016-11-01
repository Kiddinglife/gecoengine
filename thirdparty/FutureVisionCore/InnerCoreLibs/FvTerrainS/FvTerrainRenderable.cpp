#include "FvTerrainRenderable.h"

#include <FvDebug.h>

#include <OgreRoot.h>
#include <OgreCamera.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreHardwareBufferManager.h>

using namespace Ogre;

#define MAX_LOD_DIFF 1

FvUInt16 MakeIndex( FvUInt32 uiX, FvUInt32 uiY, FvUInt32 uiPageWidth, FvUInt32 uiNumTitles )
{
	return ( uiX + uiY * ((uiPageWidth/uiNumTitles) + 1));
}

int StitchEdge(FvTerrainRenderable::Neighbor eNeighbor, FvUInt32 uiHighLOD, FvUInt32 uiLowLOD, 
			   bool bOmitFirstTri, bool bOmitLastTri, unsigned short** ppIdx,
			   FvUInt32 uiPageWidth, FvUInt32 uiNumTitles,FvUInt32 uiTitleSize)
{
	FV_ASSERT(uiLowLOD > uiHighLOD);

	unsigned short* pkIdx = *ppIdx;

	FvUInt32 uiNumIndexes = 0;

	FvInt32 iSetp = 1 << uiHighLOD;
	FvInt32 iSuperStep = 1 << uiLowLOD;
	FvInt32 iHalfSuperStep = iSuperStep >> 1;

	FvInt32 iStartX, iStartY, iEndX, iRowStep;
	bool bHorizontal;
	switch(eNeighbor)
	{
	case FvTerrainRenderable::NORTH:
		iStartX = iStartY = 0;
		iEndX = uiTitleSize - 1;
		iRowStep = iSetp;
		bHorizontal = true;
		break;
	case FvTerrainRenderable::SOUTH:
		iStartX = iStartY = uiTitleSize - 1;
		iEndX = 0;
		iRowStep = -iSetp;
		iSetp = -iSetp;
		iSuperStep = -iSuperStep;
		iHalfSuperStep = -iHalfSuperStep;
		bHorizontal = true;
		break;
	case FvTerrainRenderable::EAST:
		iStartX = 0;
		iEndX = uiTitleSize - 1;
		iStartY = uiTitleSize - 1;
		iRowStep = -iSetp;
		bHorizontal = false;
		break;
	case FvTerrainRenderable::WEST:
		iStartX = uiTitleSize - 1;
		iEndX = 0;
		iStartY = 0;
		iRowStep = iSetp;
		iSetp = -iSetp;
		iSuperStep = -iSuperStep;
		iHalfSuperStep = -iHalfSuperStep;
		bHorizontal = false;
		break;
	};

	for ( int j = iStartX; j != iEndX; j += iSuperStep )
	{
		int k;
		for (k = 0; k != iHalfSuperStep; k += iSetp)
		{
			int jk = j + k;
			if ( j != iStartX || k != 0 || !bOmitFirstTri )
			{
				if (bHorizontal)
				{			
					*pkIdx++ = MakeIndex( jk + iSetp, iStartY + iRowStep, uiPageWidth, uiNumTitles ); uiNumIndexes++;
					*pkIdx++ = MakeIndex( jk, iStartY + iRowStep, uiPageWidth, uiNumTitles ); uiNumIndexes++;
					*pkIdx++ = MakeIndex( j , iStartY, uiPageWidth, uiNumTitles ); uiNumIndexes++;
				}
				else
				{			
					*pkIdx++ = MakeIndex( iStartY + iRowStep, jk + iSetp, uiPageWidth, uiNumTitles); uiNumIndexes++;
					*pkIdx++ = MakeIndex( iStartY + iRowStep, jk, uiPageWidth, uiNumTitles ); uiNumIndexes++;
					*pkIdx++ = MakeIndex( iStartY, j, uiPageWidth, uiNumTitles ); uiNumIndexes++;
				}
			}
		}

		if (bHorizontal)
		{
			*pkIdx++ = MakeIndex( j + iSuperStep, iStartY, uiPageWidth, uiNumTitles ); uiNumIndexes++;
			*pkIdx++ = MakeIndex( j + iHalfSuperStep, iStartY + iRowStep, uiPageWidth, uiNumTitles); uiNumIndexes++;
			*pkIdx++ = MakeIndex( j, iStartY, uiPageWidth, uiNumTitles ); uiNumIndexes++;
		}
		else
		{
			*pkIdx++ = MakeIndex( iStartY, j + iSuperStep, uiPageWidth, uiNumTitles ); uiNumIndexes++;
			*pkIdx++ = MakeIndex( iStartY + iRowStep, j + iHalfSuperStep, uiPageWidth, uiNumTitles ); uiNumIndexes++;
			*pkIdx++ = MakeIndex( iStartY, j, uiPageWidth, uiNumTitles ); uiNumIndexes++;
		}

		for (k = iHalfSuperStep; k != iSuperStep; k += iSetp)
		{
			int jk = j + k;
			if ( j != iEndX - iSuperStep || k != iSuperStep - iSetp || !bOmitLastTri )
			{
				if (bHorizontal)
				{			
					*pkIdx++ = MakeIndex( jk + iSetp, iStartY + iRowStep, uiPageWidth, uiNumTitles ); uiNumIndexes++;
					*pkIdx++ = MakeIndex( jk, iStartY + iRowStep, uiPageWidth, uiNumTitles ); uiNumIndexes++;
					*pkIdx++ = MakeIndex( j + iSuperStep, iStartY, uiPageWidth, uiNumTitles ); uiNumIndexes++;
				}
				else
				{
					*pkIdx++ = MakeIndex( iStartY + iRowStep, jk + iSetp, uiPageWidth, uiNumTitles ); uiNumIndexes++;
					*pkIdx++ = MakeIndex( iStartY + iRowStep, jk, uiPageWidth, uiNumTitles ); uiNumIndexes++;
					*pkIdx++ = MakeIndex( iStartY, j + iSuperStep, uiPageWidth, uiNumTitles ); uiNumIndexes++;
				}
			}
		}
	}

	*ppIdx = pkIdx;

	return uiNumIndexes;
}

IndexData *GenerateTriListIndexes(FvUInt32 uiLodLevel, FvUInt32 uiStitchFlags,FvUInt32 *pkNeiLod,
									FvUInt32 uiPageWidth, FvUInt32 uiNumTitles,FvUInt32 uiTitleSize)
{
	FvUInt32 uiNumIndexes = 0;
	FvUInt32 uiStep = 1 << uiLodLevel;

	IndexData* pkIndexData = 0;

	FvUInt32 uiNorth = uiStitchFlags & FvTerrainRenderable::STITCH_NORTH ? uiStep : 0;
	FvUInt32 uiSouth = uiStitchFlags & FvTerrainRenderable::STITCH_SOUTH ? uiStep : 0;
	FvUInt32 uiEast = uiStitchFlags & FvTerrainRenderable::STITCH_EAST ? uiStep : 0;
	FvUInt32 uiWest = uiStitchFlags & FvTerrainRenderable::STITCH_WEST ? uiStep : 0;

	FvUInt32 uiNewLength = ( uiTitleSize / uiStep ) * ( uiTitleSize / uiStep ) * 2 * 2 * 2 ;

	pkIndexData = OGRE_NEW IndexData;
	pkIndexData->indexBuffer = 
		HardwareBufferManager::getSingleton().createIndexBuffer(
		HardwareIndexBuffer::IT_16BIT,
		uiNewLength, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	unsigned short* pkIdx = static_cast<unsigned short*>(
		pkIndexData->indexBuffer->lock(0, 
		pkIndexData->indexBuffer->getSizeInBytes(), 
		HardwareBuffer::HBL_DISCARD));

	for ( FvUInt32 j = uiNorth; j < uiTitleSize - 1 - uiSouth; j += uiStep )
	{
		for ( FvUInt32 i = uiWest; i < uiTitleSize - 1 - uiEast; i += uiStep )
		{
			if((i/uiStep)%2 == (j/uiStep)%2)
			{							
				*pkIdx++ = MakeIndex( i + uiStep, j, uiPageWidth,uiNumTitles ); uiNumIndexes++;
				*pkIdx++ = MakeIndex( i, j + uiStep, uiPageWidth,uiNumTitles ); uiNumIndexes++;
				*pkIdx++ = MakeIndex( i, j, uiPageWidth,uiNumTitles ); uiNumIndexes++;

				*pkIdx++ = MakeIndex( i + uiStep, j, uiPageWidth,uiNumTitles ); uiNumIndexes++;
				*pkIdx++ = MakeIndex( i + uiStep, j + uiStep, uiPageWidth,uiNumTitles ); uiNumIndexes++;
				*pkIdx++ = MakeIndex( i, j + uiStep, uiPageWidth,uiNumTitles ); uiNumIndexes++;
			}
			else
			{
				*pkIdx++ = MakeIndex( i, j, uiPageWidth,uiNumTitles ); uiNumIndexes++;
				*pkIdx++ = MakeIndex( i + uiStep, j, uiPageWidth,uiNumTitles ); uiNumIndexes++;
				*pkIdx++ = MakeIndex( i + uiStep, j + uiStep, uiPageWidth,uiNumTitles ); uiNumIndexes++;				

				*pkIdx++ = MakeIndex( i + uiStep, j + uiStep, uiPageWidth,uiNumTitles ); uiNumIndexes++;
				*pkIdx++ = MakeIndex( i, j + uiStep, uiPageWidth,uiNumTitles ); uiNumIndexes++;
				*pkIdx++ = MakeIndex( i, j, uiPageWidth,uiNumTitles ); uiNumIndexes++;
			}
		}
	}

	if ( uiNorth > 0 )
	{
		uiNumIndexes += StitchEdge(FvTerrainRenderable::NORTH, uiLodLevel, pkNeiLod[FvTerrainRenderable::NORTH],
			uiWest > 0, uiEast > 0, &pkIdx,
			uiPageWidth,uiNumTitles,uiTitleSize);
	}
	if ( uiEast > 0 )
	{
		uiNumIndexes += StitchEdge(FvTerrainRenderable::EAST, uiLodLevel, pkNeiLod[FvTerrainRenderable::EAST],
			uiNorth > 0, uiSouth > 0, &pkIdx,
			uiPageWidth,uiNumTitles,uiTitleSize);
	}
	if ( uiSouth > 0 )
	{
		uiNumIndexes += StitchEdge(FvTerrainRenderable::SOUTH, uiLodLevel, pkNeiLod[FvTerrainRenderable::SOUTH],
			uiEast > 0, uiWest > 0, &pkIdx,
			uiPageWidth,uiNumTitles,uiTitleSize);
	}
	if ( uiWest > 0 )
	{
		uiNumIndexes += StitchEdge(FvTerrainRenderable::WEST, uiLodLevel, pkNeiLod[FvTerrainRenderable::WEST],
			uiSouth > 0, uiNorth > 0, &pkIdx,
			uiPageWidth,uiNumTitles,uiTitleSize);
	}

	pkIndexData->indexBuffer->unlock();
	pkIndexData->indexCount = uiNumIndexes;
	pkIndexData->indexStart = 0;

	return pkIndexData;
}

FvTerrainRenderable::GridVertexBufferMap *
FvTerrainRenderable::ms_pkGridVertexBufferMap = NULL;

FvTerrainRenderable::LevelIndexMap *
FvTerrainRenderable::ms_pkLevelIndexMap = NULL;

FvTerrainRenderable::FvTerrainRenderable(FvTerrainPage &kTerrainPage,
										 FvUInt32 uiTitleX,FvUInt32 uiTitleY) :
m_uiTitleX(uiTitleX),
m_uiTitleY(uiTitleY),
m_uiTitleSize(kTerrainPage.PageWidth()/
			  kTerrainPage.NumTitles() + 1),
m_uiLodLevel(-1),
m_uiLightMask(0xFFFFFFFF),
m_uiLightListUpdated(0),
m_kTerrainPage(kTerrainPage),
m_kHeightVertexBuffers(kTerrainPage.NumLods()),
m_eState(FvTerrainPage::MATERIAL_BLEND),
m_kLevelIndex((*ms_pkLevelIndexMap)[
	TerrainRenderIndex(m_kTerrainPage.NumTitles(),
		m_kTerrainPage.PageWidth())])
{
	for ( int i = 0; i < NEIGHBORS; i++ )
	{
		m_pkNeighbors[ i ] = NULL;
	}

	m_pkVertexData = OGRE_NEW VertexData;
	m_pkVertexData->vertexStart = 0;
	m_pkVertexData->vertexCount = m_uiTitleSize * m_uiTitleSize ;

	VertexDeclaration *pkDecl = m_pkVertexData->vertexDeclaration;
	VertexBufferBinding *pkBind = m_pkVertexData->vertexBufferBinding;

	pkDecl->addElement(HEIGHT_BINDING, 0, VET_FLOAT1, VES_POSITION, 0);
	pkDecl->addElement(MORPH_BINDING, 0, VET_FLOAT1, VES_POSITION, 1);
	pkDecl->addElement(GRID_BINDING, 0, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);

	pkBind->setBinding(GRID_BINDING, this->GetGridBuffer(m_uiTitleX,m_uiTitleY));

	FvUInt32 uiTitleMeters = FV_BLOCK_SIZE_METERS/m_kTerrainPage.NumTitles();
	FvVector3 kPos = m_kTerrainPage.WorldPosition() + 
		FvVector3(uiTitleMeters * m_uiTitleX,uiTitleMeters * m_uiTitleY,0);
	m_kBoundingBox.SetBounds(FvVector3( kPos.x, kPos.y, m_kTerrainPage.MinHeight() ),
		FvVector3( kPos.x + uiTitleMeters,
		kPos.y + uiTitleMeters,
		m_kTerrainPage.MaxHeight()));

	m_kWireBB.setupBoundingBox(Ogre::AxisAlignedBox(
		Ogre::Vector3( m_kBoundingBox.MinBounds().x,
		m_kBoundingBox.MinBounds().y,
		m_kBoundingBox.MinBounds().z), 
		Ogre::Vector3( m_kBoundingBox.MaxBounds().x,
		m_kBoundingBox.MaxBounds().y,
		m_kBoundingBox.MaxBounds().z)));

	m_kCentre = m_kBoundingBox.Centre();
	float fX = (m_kBoundingBox.MaxBounds().x - m_kBoundingBox.MinBounds().x);
	float fY = (m_kBoundingBox.MaxBounds().y - m_kBoundingBox.MinBounds().y);
	float fZ = (m_kBoundingBox.MaxBounds().z - m_kBoundingBox.MinBounds().z);
	m_fBoundingRadius = FvSqrtf(fX * fX + fY * fY + fZ * fZ) / 2;

	for(FvUInt32 uiLodLevel = 0; uiLodLevel < m_kTerrainPage.NumLods(); uiLodLevel++)
	{
		Ogre::HardwareVertexBufferSharedPtr &spHeightBuffer = m_kHeightVertexBuffers[uiLodLevel];

		if(spHeightBuffer.isNull())
		{
			spHeightBuffer =
				HardwareBufferManager::getSingleton().createVertexBuffer(
				VertexElement::getTypeSize(VET_FLOAT1),
				m_pkVertexData->vertexCount, 
				HardwareBuffer::HBU_STATIC_WRITE_ONLY);

			float *pVoid = static_cast<float*>(spHeightBuffer->lock(HardwareBuffer::HBL_DISCARD));
			for(FvUInt32 j = 0 ; j < m_uiTitleSize; j++)
				for(FvUInt32 i = 0 ; i < m_uiTitleSize; i++)
					*(pVoid + j * m_uiTitleSize + i) = LodHeight(i, j, 0, uiLodLevel);

			spHeightBuffer->unlock();
		}
	}

	if(m_kLevelIndex.size() == 0)
	{
		m_kLevelIndex.resize(m_kTerrainPage.NumLods(),IndexMap());

		for(FvUInt32 uiLod = 0; uiLod < m_kTerrainPage.NumLods(); uiLod++)
		{
			for(FvUInt32 uiEastLod = 0; uiEastLod < m_kTerrainPage.NumLods(); uiEastLod++)
			{
				if((uiEastLod - uiLod) > MAX_LOD_DIFF) continue;
				for(FvUInt32 uiWestLod = 0; uiWestLod < m_kTerrainPage.NumLods(); uiWestLod++)
				{
					if((uiWestLod - uiLod) > MAX_LOD_DIFF) continue;
					for(FvUInt32 uiNorthLod = 0; uiNorthLod < m_kTerrainPage.NumLods(); uiNorthLod++)
					{
						if((uiNorthLod - uiLod) > MAX_LOD_DIFF) continue;
						for(FvUInt32 uiSouthLod = 0; uiSouthLod < m_kTerrainPage.NumLods(); uiSouthLod++)
						{
							if((uiSouthLod - uiLod) > MAX_LOD_DIFF) continue;
							FvUInt32 auiNeighborsLod[NEIGHBORS];
							auiNeighborsLod[EAST] = uiEastLod;
							auiNeighborsLod[WEST] = uiWestLod;
							auiNeighborsLod[NORTH] = uiNorthLod;
							auiNeighborsLod[SOUTH] = uiSouthLod;
							FvUInt32 uiStitchFlags = 0;

							if ( auiNeighborsLod[EAST] > uiLod )
							{
								uiStitchFlags |= STITCH_EAST;
								uiStitchFlags |= 
									(auiNeighborsLod[EAST] - uiLod) << STITCH_EAST_SHIFT;
							}

							if ( auiNeighborsLod[WEST] > uiLod )
							{
								uiStitchFlags |= STITCH_WEST;
								uiStitchFlags |= 
									(auiNeighborsLod[WEST] - uiLod) << STITCH_WEST_SHIFT;
							}

							if ( auiNeighborsLod[NORTH] > uiLod )
							{
								uiStitchFlags |= STITCH_NORTH;
								uiStitchFlags |= 
									(auiNeighborsLod[NORTH] - uiLod) << STITCH_NORTH_SHIFT;
							}

							if ( auiNeighborsLod[SOUTH] > uiLod )
							{
								uiStitchFlags |= STITCH_SOUTH;
								uiStitchFlags |= 
									(auiNeighborsLod[SOUTH] - uiLod) << STITCH_SOUTH_SHIFT;
							}

							IndexMap::iterator kIt = m_kLevelIndex[ uiLod ].find( uiStitchFlags );

							if ( kIt == m_kLevelIndex[ uiLod ].end())
							{	
								Ogre::IndexData *pkIndexData = GenerateTriListIndexes(uiLod,uiStitchFlags,auiNeighborsLod,
									m_kTerrainPage.PageWidth(),m_kTerrainPage.NumTitles(),m_uiTitleSize);
								m_kLevelIndex[uiLod].insert(
									IndexMap::value_type(uiStitchFlags, pkIndexData));
							}
						}
					}
				}
			}
		}
	}
}

FvTerrainRenderable::~FvTerrainRenderable()
{
	OGRE_DELETE m_pkVertexData;
}

void FvTerrainRenderable::Init()
{
	if(ms_pkGridVertexBufferMap == NULL)
		ms_pkGridVertexBufferMap = new GridVertexBufferMap;

	if(ms_pkLevelIndexMap == NULL)
		ms_pkLevelIndexMap = new LevelIndexMap;
}

void FvTerrainRenderable::Fini()
{
	delete ms_pkLevelIndexMap;
	delete ms_pkGridVertexBufferMap;
}

void FvTerrainRenderable::getRenderOperation(RenderOperation &kOP)
{
	FV_ASSERT(m_uiLodLevel < m_kTerrainPage.NumLods());
	FvUInt32 uiStitchFlags = 0;
	FvUInt32 uiLodDiff = 0;

	if ( m_pkNeighbors[ EAST ] != 0 && m_pkNeighbors[ EAST ]->m_uiLodLevel > m_uiLodLevel )
	{
		uiLodDiff = (m_pkNeighbors[ EAST ]->m_uiLodLevel - m_uiLodLevel);
		uiStitchFlags |= STITCH_EAST;
		uiStitchFlags |= ((uiLodDiff > MAX_LOD_DIFF)?MAX_LOD_DIFF:uiLodDiff) << STITCH_EAST_SHIFT;
	}

	if ( m_pkNeighbors[ WEST ] != 0 && m_pkNeighbors[ WEST ]->m_uiLodLevel > m_uiLodLevel )
	{
		uiLodDiff = (m_pkNeighbors[ WEST ]->m_uiLodLevel - m_uiLodLevel);
		uiStitchFlags |= STITCH_WEST;
		uiStitchFlags |= ((uiLodDiff > MAX_LOD_DIFF)?MAX_LOD_DIFF:uiLodDiff) << STITCH_WEST_SHIFT;
	}

	if ( m_pkNeighbors[ NORTH ] != 0 && m_pkNeighbors[ NORTH ]->m_uiLodLevel > m_uiLodLevel )
	{
		uiLodDiff = (m_pkNeighbors[ NORTH ]->m_uiLodLevel - m_uiLodLevel);
		uiStitchFlags |= STITCH_NORTH;
		uiStitchFlags |= ((uiLodDiff > MAX_LOD_DIFF)?MAX_LOD_DIFF:uiLodDiff) << STITCH_NORTH_SHIFT;
	}

	if ( m_pkNeighbors[ SOUTH ] != 0 && m_pkNeighbors[ SOUTH ]->m_uiLodLevel > m_uiLodLevel )
	{
		uiLodDiff = (m_pkNeighbors[ SOUTH ]->m_uiLodLevel - m_uiLodLevel);
		uiStitchFlags |= STITCH_SOUTH;
		uiStitchFlags |= ((uiLodDiff > MAX_LOD_DIFF)?MAX_LOD_DIFF:uiLodDiff) << STITCH_SOUTH_SHIFT;
	}

	FV_ASSERT(m_pkVertexData)
	FV_ASSERT(m_kLevelIndex[m_uiLodLevel][uiStitchFlags]);
	kOP.useIndexes = true;
	kOP.operationType = RenderOperation::OT_TRIANGLE_LIST;
	kOP.vertexData = m_pkVertexData;
	kOP.indexData = m_kLevelIndex[m_uiLodLevel][uiStitchFlags];
}

void FvTerrainRenderable::getWorldTransforms(Matrix4 *pkXForm) const
{
	pkXForm->makeTrans(m_kTerrainPage.WorldPosition().x,
		m_kTerrainPage.WorldPosition().y,
		m_kTerrainPage.WorldPosition().z);
}

Real FvTerrainRenderable::getSquaredViewDepth(const Camera *pkCam) const
{
	Vector3 kDiff = *(Vector3*)&this->Centre() - pkCam->getDerivedPosition();
	return kDiff.squaredLength();
}

const LightList &FvTerrainRenderable::getLights(void) const
{
	Ogre::SceneManager *pkSceneManager = Ogre::Root::getSingleton().
		_getCurrentSceneManager();
	FV_ASSERT(pkSceneManager);
	FvUInt64 uiFrame = pkSceneManager->_getLightsDirtyCounter();
	if ((m_uiLightListUpdated != uiFrame) ||
		(m_kLightList.size() == 0))
	{
		m_uiLightListUpdated = uiFrame;
		pkSceneManager->_populateLightList(*(Vector3*)&m_kCentre, 
			m_fBoundingRadius, m_kLightList, m_uiLightMask);
	}

	return m_kLightList;
}

void FvTerrainRenderable::NotifyCameraPosition( const FvVector3 &kPos )
{
	FvVector3 kRelativeCameraPos = kPos - m_kBoundingBox.MinBounds();
	float fMinDistanceToCamera,fMaxDistanceToCamera;
	FvTerrainSettings::MinMaxXZDistance( kRelativeCameraPos , 
		FV_BLOCK_SIZE_METERS/m_kTerrainPage.NumTitles(), 
		fMinDistanceToCamera, 
		fMaxDistanceToCamera );

	if(fMaxDistanceToCamera < m_kTerrainPage.TerrainSettings()->LodTextureStart())
	{
		m_eState = FvTerrainPage::MATERIAL_BLEND;
	}
	else if(fMinDistanceToCamera > (m_kTerrainPage.TerrainSettings()->LodTextureStart() +
		m_kTerrainPage.TerrainSettings()->LodTextureDistance()))
	{
		m_eState = FvTerrainPage::MATERIAL_LOD;
	}
	else
	{
		m_eState = FvTerrainPage::MATERIAL_INTERIM;
	}	

	FvUInt32 uiLodLevel = m_kTerrainPage.TerrainSettings()->CalculateLodLevel(fMinDistanceToCamera);

	if(m_uiLodLevel != uiLodLevel)
	{
		m_uiLodLevel = uiLodLevel;
		FvVector2 kMorphRange = m_kTerrainPage.TerrainSettings()->CalcMorphRanges(m_uiLodLevel);
		this->setCustomParameter(CUSTOM_PARAM_INDEX_MORPH,
			Ogre::Vector4(FV_BLOCK_SIZE_METERS,FV_BLOCK_SIZE_METERS,kMorphRange.x,kMorphRange.y));

		m_pkVertexData->vertexBufferBinding->setBinding(HEIGHT_BINDING, 
			m_kHeightVertexBuffers[m_uiLodLevel]);
		m_pkVertexData->vertexBufferBinding->setBinding(MORPH_BINDING, 
			m_kHeightVertexBuffers[((m_uiLodLevel + 1) < m_kTerrainPage.NumLods()) ? 
			(m_uiLodLevel + 1) : (m_kTerrainPage.NumLods() - 1)]);
	}
}

Ogre::HardwareVertexBufferSharedPtr &FvTerrainRenderable::GetGridBuffer(
	FvUInt32 uiTitleX, FvUInt32 uiTitleY ) const
{
	TerrainRenderIndex kIndex(m_kTerrainPage.NumTitles(),
		m_kTerrainPage.PageWidth());

	GridVertexBuffers &kGridVertexBuffer = (*ms_pkGridVertexBufferMap)[kIndex];
	if(kGridVertexBuffer.size() == 0)
		kGridVertexBuffer.resize(m_kTerrainPage.NumTitles()*m_kTerrainPage.NumTitles(),
		HardwareVertexBufferSharedPtr());

	HardwareVertexBufferSharedPtr &spVertexBuffer = kGridVertexBuffer[ 
		uiTitleY * m_kTerrainPage.NumTitles() + uiTitleX];

	if(spVertexBuffer.isNull())
	{
		spVertexBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			Ogre::VertexElement::getTypeSize(VET_FLOAT2),
			m_uiTitleSize * m_uiTitleSize, 
			Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
		kGridVertexBuffer[uiTitleY * m_kTerrainPage.NumTitles() + uiTitleX] = spVertexBuffer;

		FvVector2 *pkBuffer = static_cast<FvVector2*>(spVertexBuffer->lock(HardwareBuffer::HBL_DISCARD));
		for(FvUInt32 iGirdY = 0; iGirdY < m_uiTitleSize; iGirdY++)
		{
			for(FvUInt32 iGirdX = 0; iGirdX < m_uiTitleSize; iGirdX++)
			{
				FvVector2 &kGrid = *(pkBuffer + iGirdY * m_uiTitleSize + iGirdX);
				kGrid.x = float(uiTitleX * (m_uiTitleSize - 1) + iGirdX)/float(m_kTerrainPage.PageWidth());
				kGrid.y = float(uiTitleY * (m_uiTitleSize - 1) + iGirdY)/float(m_kTerrainPage.PageWidth());
			}
		}
		spVertexBuffer->unlock();
	}

	return spVertexBuffer;
}

float FvTerrainRenderable::LodHeight(FvUInt32 uiX, FvUInt32 uiY, FvUInt32 uiCurLodLevel, FvUInt32 uiLodLevel)
{
	if(uiCurLodLevel == uiLodLevel)
	{
		return m_kTerrainPage.HeightAt(
			int(m_uiTitleX * (m_uiTitleSize - 1) + (uiX << uiLodLevel)), int(m_uiTitleY * (m_uiTitleSize - 1) + (uiY << uiLodLevel)));
	}
	else
	{
		FvUInt32 uiLodX = uiX >> 1;
		FvUInt32 uiLodY = uiY >> 1;
		FvUInt32 uiNextLodLevel = uiCurLodLevel + 1;

		if(uiY & 1)
		{
			if(uiX & 1)
			{
				if(uiLodY & 1)
				{
					return (LodHeight(uiLodX + 1, uiLodY, uiNextLodLevel, uiLodLevel) + LodHeight(uiLodX, uiLodY + 1, uiNextLodLevel, uiLodLevel)) * 0.5f;
				}
				else
				{
					return (LodHeight(uiLodX, uiLodY, uiNextLodLevel, uiLodLevel) + LodHeight(uiLodX + 1, uiLodY + 1, uiNextLodLevel, uiLodLevel)) * 0.5f;
				}
			}
			else
			{
				return (LodHeight(uiLodX, uiLodY, uiNextLodLevel, uiLodLevel) + LodHeight(uiLodX, uiLodY + 1, uiNextLodLevel, uiLodLevel)) * 0.5f;
			}
		}
		else
		{
			if(uiX & 1)
			{
				return (LodHeight(uiLodX, uiLodY, uiNextLodLevel, uiLodLevel) + LodHeight(uiLodX + 1, uiLodY, uiNextLodLevel, uiLodLevel)) * 0.5f;
			}
			else
			{
				return LodHeight(uiLodX, uiLodY, uiNextLodLevel, uiLodLevel);
			}
		}
	}
}