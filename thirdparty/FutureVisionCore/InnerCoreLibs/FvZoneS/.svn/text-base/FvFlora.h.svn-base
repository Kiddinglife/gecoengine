//{future header message}
#ifndef __FvFlora_H__
#define __FvFlora_H__

#include <FvMatrix.h>
#include <FvVector2.h>
#include <FvStringMap.h>
#include <FvXMLSection.h>
#include <FvTerrainPage.h>
#include <OgreMovableObject.h>

#include "FvZoneDefine.h"

#include "FvEcotype.h"
#include "FvFloraBlock.h"
#include "FvFloraConstants.h"

class FvCamera;

class FV_ZONE_API FvFlora
{
public:
	FvFlora();
	~FvFlora();

	static std::vector< FvFlora* > Floras() { return ms_kFloras; }

	bool Init( FvXMLSectionPtr spSection );

	void VisitRenderable( Ogre::RenderQueue *pkQueue,
		FvCamera *pkCamera,
		bool bOnlyShadowCasters );

	void Activate();
	void Deactivate();

	void Update( float fTime, const FvVector3 &kCamPos );

	void CreateUnmanagedObjects();
	void DeleteUnmanagedObjects();

	FvEcotype &EcotypeAt( const FvVector2 & );
	FvEcotype::ID GenerateEcotypeID( const FvVector2 &kPos2 );
	const char *GroundType( float fX, float fY );
	void SeedOffsetTable( const FvVector2& );
	const FvVector2 &NextOffset();
	float NextRotation();
	float NextRandomFloat();
	FvTerrainPagePtr GetTerrainPage( const FvVector3 &kPos, 
		FvVector3 &kRelativePos, 
		const FvVector2 *pkReferencePt = NULL ) const;
	int GetTerrainPageID( const FvMatrix &kTerrainPageTransform ) const;

	void ResetBlockAt( float fX, float fY );
	static void FloraReset();
	static void Enabled( bool bState ) { ms_bEnabled = bState; }
	static bool Enabled() { return ms_bEnabled; }

	void VBSize( FvUInt32 uiBytes );	
	FvUInt32 VBSize() const	{ return m_uiVBSize; }
	static void FloraVBSize( FvUInt32 uiBytes );

	const FvMatrix &Transform( const FvFloraBlock &kBlock );	
	class FvFloraTexture *FloraTexture() { return m_pkFloraTexture; }	

	void FillBlocks();

	void InitialiseOffsetTable( float fBlurAmount = 2.f );

	FvXMLSectionPtr Data() { return m_spData; }

	FvUInt32 MaxVBSize() const;

	class FloraBlockDistancePair
	{
	public:
		FloraBlockDistancePair( float fDistance, const FvFloraBlock &kBlock ):
			m_fDistance(fDistance),
			m_kBlock(kBlock)
		{
		}

		static bool SortReverse( const FloraBlockDistancePair *pkA, 
			const FloraBlockDistancePair *pkB )
		{
			return (pkA->m_fDistance > pkB->m_fDistance);
		}

		float m_fDistance;
		const FvFloraBlock &m_kBlock;
	};

	bool PreLock();	
	FvFloraVertexContainer *Lock( FvUInt32 uiFirstVertex, FvUInt32 uiNumVertices );
	bool Unlock( FvFloraVertexContainer * );
	void PostUnlock();

	Ogre::HardwareVertexBufferSharedPtr &GetVertexBuffer() { return m_spVertexBuffer; }
	Ogre::MaterialPtr &GetMaterial();
	
	void SetDetailBendingEnable(bool bEnable) { m_bEnableBending = bEnable; }
	bool GetDetailBendingEnable() { return m_bEnableBending; }

private:

	bool MoveBlocks( const FvVector2 &kCamPos2 );
	void AccumulateBoundingBoxes();
	void Cull(FvCamera *pkCamera);	
	void TeleportCamera( const FvVector2 &kCamPos2 );
	void GetViewLocation( FvVector3 &kRet );

	void ClearVertexBuffer( FvUInt32 uiOffset = 0, FvUInt32 uiSize = 0 );

	FvXMLSectionPtr m_spData;
	FvFloraBlock *m_apBlocks[FV_FLORA_BLOCK_STRIDE][FV_FLORA_BLOCK_STRIDE];

	typedef std::vector<FloraBlockDistancePair*> SortedFloraBlocks;
	SortedFloraBlocks m_kSortedBlocks;
	FvEcotype *m_apEcotypes[256];
	FvEcotype m_kDegenerateEcotype;
	FvUInt32 m_uiVBSize;
	FvUInt32 m_uiNumVertices;
	FvVector2 m_akOffsets[FV_FLORA_LUT_SIZE];
	float m_afRandoms[FV_FLORA_LUT_SIZE];
	int m_iLutSeed;
	FvVector2 m_kLastPos;
	bool m_bCameraTeleport;
	FvUInt32 m_uiNumVerticesPerBlock;
	FvUInt32 m_uiMaxVBSize;
	float m_fMaxSlope;

	FvFloraVertexContainer *m_pkContainer;
	Ogre::HardwareVertexBufferSharedPtr m_spVertexBuffer;
	FvUInt32 m_uiCapacity;
	FvUInt32 m_uiBufferSize;
	FvUInt8 *m_pkLockedBuffer;
	Ogre::MaterialPtr m_spMaterial;
	bool m_bEnableBending;

	int m_iCenterBlockX;
	int m_iCenterBlockY;

	class MacroBB
	{
	public:
		FvBoundingBox m_kBB;
		FvFloraBlock *m_apBlocks[25];
	};
	MacroBB m_akMacroBB[25];
	std::vector<FvFloraBlock*> m_kMovedBlocks;

	mutable FvTerrainFinder::Details m_kDetails;
	mutable FvVector2 m_kLastRefPt;

	void CalculateTransform( const FvFloraBlock &kBlock );
	bool m_abCalculated[4];
	FvMatrix m_akTransform[4];
	FvStringHashMap<FvEcotype::ID> m_kTexToEcotype;

	static bool ms_bEnabled;
	static std::vector< FvFlora* > ms_kFloras;
	class FvFloraTexture *m_pkFloraTexture;
};

#endif // __FvFlora_H__