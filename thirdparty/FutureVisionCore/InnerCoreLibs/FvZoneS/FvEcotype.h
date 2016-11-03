//{future header message}
#ifndef __FvEcotype_H__
#define __FvEcotype_H__

#include <FvXMLSection.h>
#include <FvBoundingBox.h>
#include <FvConcurrency.h>

#include <OgreTexture.h>

#include "FvFloraTexture.h"

class FvEcotype
{
public:	
	typedef FvUInt8 ID;
	static const FvUInt32 ID_AUTO = 255;

	FvEcotype( class FvFlora *pkFlora );
	~FvEcotype();

	void Init( FvXMLSectionPtr spAllEcotypesSection, FvUInt8 uiID );
	void Finz();

	bool m_bIsLoading;
	bool m_bIsInited;

	const FvString &SoundTag() const { return m_kTag; }
	void OffsetUV( FvVector2 &kOffset );
	const FvVector2 &UVOffset() { return m_kUVOffset; }
	FvUInt32 Generate(		
	class FvFloraVertexContainer *pkVerts,
		FvUInt32 uiID,
		FvUInt32 uiMaxVerts,
		const FvMatrix &kObjectToWorld,
		const FvMatrix &kObjectToZone,
		FvBoundingBox &kBB );
	bool IsEmpty() const;
	FvUInt8 GetID() const { return m_uiID; }
	FvString &TextureResource( int iIndex ) { FV_ASSERT(iIndex < FV_FLORA_TEXTURE_NUMBER); return m_kTextureResources[iIndex]; }
	void TextureResource( const FvString &kTex, int iIndex ) { FV_ASSERT(iIndex < FV_FLORA_TEXTURE_NUMBER); m_kTextureResources[iIndex] = kTex; }
	FvD3DSystemMemoryTexturePtr pTexture( int iIndex ) { FV_ASSERT(iIndex < FV_FLORA_TEXTURE_NUMBER); return m_spTextures[iIndex]; }
	void pTexture( FvD3DSystemMemoryTexturePtr spTex, int iIndex ) { FV_ASSERT(iIndex < FV_FLORA_TEXTURE_NUMBER); m_spTextures[iIndex] = spTex; }
	void Tag( const FvString &kTag ) { m_kTag = kTag; }
	void Generator( class FvEcotypeGenerator *kGen ) { m_pkGenerator = kGen; }

	class FvEcotypeGenerator *Generator() const	{ return m_pkGenerator; }

	class FvFlora *Flora() const { return m_pkFlora; }

	void IncRef();
	void DecRef();

	static FvSimpleMutex ms_kDeleteMutex;

private:
	void GrabTexture();
	void FreeTexture();
	struct BkLoadInfo
	{					
		class FvBackgroundTask *m_pkLoadingTask;
		FvEcotype *m_pkEcotype;
		FvXMLSectionPtr m_spSection;
	};
	static void BackgroundInit( void * );
	static void OnBackgroundInitComplete( void * );
	std::vector<FvString> m_kTextureResources;
	std::vector<FvD3DSystemMemoryTexturePtr> m_spTextures;
	FvString m_kTag;
	FvVector2 m_kUVOffset;	
	int m_iRefCount;
	FvUInt8	m_uiID;
	class FvEcotypeGenerator *m_pkGenerator;
	class FvFlora *m_pkFlora;
	BkLoadInfo *m_pkLoadInfo;
};

#endif // __FvEcotype_H__