//{future header message}
#ifndef __FvTerrainRenderable_H__
#define __FvTerrainRenderable_H__

#include "FvTerrain.h"
#include "FvTerrainPage.h"

#include <OgreRenderable.h>

using namespace Ogre;

typedef std::pair<FvUInt16,FvUInt16> TerrainRenderIndex;

class FV_TERRAIN_API FvTerrainRenderable : public Renderable
{
public:

	enum
	{
		HEIGHT_BINDING = 0,
		MORPH_BINDING  = 1,
		GRID_BINDING   = 2
	};

	enum 
	{
		CUSTOM_PARAM_INDEX_MORPH = 1
	};

	enum Neighbor
	{
		NORTH		= 0,
		SOUTH		= 1,
		EAST		= 2,
		WEST		= 3,
		NEIGHBORS	= 4
	};

	enum
	{
		STITCH_NORTH_SHIFT	= 0,
		STITCH_SOUTH_SHIFT	= 8,
		STITCH_WEST_SHIFT	= 16,
		STITCH_EAST_SHIFT	= 24,
		STITCH_NORTH		= 128 << STITCH_NORTH_SHIFT,
		STITCH_SOUTH		= 128 << STITCH_SOUTH_SHIFT,
		STITCH_WEST			= 128 << STITCH_WEST_SHIFT,
		STITCH_EAST			= 128 << STITCH_EAST_SHIFT,
	};

	FvTerrainRenderable(FvTerrainPage &kTerrainPage,
		FvUInt32 uiTitleX,FvUInt32 uiTitleY);
	~FvTerrainRenderable();

	static void Init();
	static void Fini();

	FvBoundingBox const &BoundingBox() const;
	FvVector3 const &Centre() const;
	float BoundingRadius() const;

	void SetLightMask(FvUInt32 uiLightMask);
	FvUInt32 GetLightMask();

	const MaterialPtr& getMaterial(void) const;

	Technique *getTechnique(void) const;

	void getRenderOperation(RenderOperation &kOP);
    
	void getWorldTransforms(Matrix4 *pkXForm) const;

	Real getSquaredViewDepth(const Camera *pkCam) const;

	const LightList& getLights(void) const;

	bool getCastsShadows(void) const;

	void NotifyCameraPosition( const FvVector3 &kPos );

	void SetNeighbor( Neighbor eNeighbor, FvTerrainRenderable *pkTitle );

	FvTerrainRenderable *GetNeighbor( Neighbor eNeighbor );

	FvUInt32 GetLodLevel();

	Ogre::Renderable *GetWireBB();

protected:

	Ogre::HardwareVertexBufferSharedPtr &GetGridBuffer(
		FvUInt32 uiTitleX, FvUInt32 uiTitleY ) const;

	float LodHeight(FvUInt32 x, FvUInt32 y, FvUInt32 uiCurLodLevel, FvUInt32 uiLodLevel);

	FvTerrainRenderable *m_pkNeighbors[NEIGHBORS];

	FvUInt32 m_uiTitleX;
	FvUInt32 m_uiTitleY;

	FvUInt32 m_uiTitleSize;

	FvUInt32 m_uiLodLevel;

	FvBoundingBox m_kBoundingBox;

	Ogre::WireBoundingBox m_kWireBB;

	FvVector3 m_kCentre;
	float m_fBoundingRadius;

	VertexData *m_pkVertexData;

	HardwareVertexBufferSharedPtr m_spHeight;

	FvUInt32 m_uiLightMask;
	mutable LightList m_kLightList;
	mutable FvUInt64 m_uiLightListUpdated;

	FvTerrainPage &m_kTerrainPage;

	FvTerrainPage::MaterialType m_eState;

	typedef std::vector<Ogre::HardwareVertexBufferSharedPtr> HeightVertexBuffers;

	HeightVertexBuffers m_kHeightVertexBuffers;

 	typedef std::vector<Ogre::HardwareVertexBufferSharedPtr> GridVertexBuffers;
	typedef std::map<TerrainRenderIndex,GridVertexBuffers> GridVertexBufferMap;

	static GridVertexBufferMap *ms_pkGridVertexBufferMap;

	typedef std::map<FvUInt32, Ogre::IndexData*> IndexMap;
	typedef std::vector<IndexMap> LevelArray;
	class LevelIndexMap : public std::map<TerrainRenderIndex, LevelArray>
	{
	public:
		~LevelIndexMap()
		{
			LevelIndexMap::iterator kIt = this->begin();
			for(; kIt != this->end(); ++kIt)
			{
				LevelArray::iterator kArrayIt = kIt->second.begin();
				for(; kArrayIt != kIt->second.end(); ++kArrayIt)
				{
					IndexMap::iterator kIndexIt = (*kArrayIt).begin();
					for(; kIndexIt != (*kArrayIt).end(); ++kIndexIt)
					{
						OGRE_DELETE kIndexIt->second;
					}
					(*kArrayIt).clear();
				}
				kIt->second.clear();
			}
			this->clear();
		}
	};

	LevelArray &m_kLevelIndex;

	static LevelIndexMap *ms_pkLevelIndexMap;
};

#include "FvTerrainRenderable.inl"

#endif // __FvTerrainRenderable_H__