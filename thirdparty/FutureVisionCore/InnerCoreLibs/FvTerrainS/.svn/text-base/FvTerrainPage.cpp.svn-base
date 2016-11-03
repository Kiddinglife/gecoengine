#include "FvTerrainPage.h"
#include "FvTerrainPageSerializer.h"

#include <FvDebug.h>

#include <OgreResourceGroupManager.h>

#ifndef FV_SERVER
#include <OgreTextureManager.h>
#include <OgrePass.h>
#include <OgreTechnique.h>

#include "FvTerrainRenderable.h"
#include "FvTerrainLodController.h"

FvString FvTerrainPage::TERRAIN_MATERIAL_NAME[MATERIAL_TYPE_NUMBER] =
{
	"FvTerrain_Blend",
	"FvTerrain_Interim",
	"FvTerrain_Lod"
};
bool FvTerrainPage::ms_bTerrainVisible = false;
FvUInt32 FvTerrainPage::ms_uiTerrainPageIdentify = 0;
#endif // !FV_SERVER

FvString FvTerrainPage::TERRAIN_INERNAL_RESOURCE_GROUP = "General";

FvTerrainFinder *FvTerrainPage::ms_pkTerrainFinder = NULL;

namespace
{
	const FvUInt64 g_uiEmptyBlock = FvUInt64(0x0000ffff);

	class FvHoleHelper
	{
	public:
		FvHoleHelper(FvUInt8 *pkHoleData,FvUInt32 uiWidth,FvUInt32 uiHeight):
		m_pkHoleData(pkHoleData)
		{
			m_uiSrcStride = (uiWidth / 8) +
				((uiWidth & 3) ? 1 : 0);
			m_uiMaxX = uiWidth - 1;
			m_uiMaxY = uiWidth - 1;
		}

		bool Hole( FvUInt32 iX, FvUInt32 iY )
		{
			iY = std::min(iY, m_uiMaxY);
			iX = std::min(iX, m_uiMaxX);
			FvUInt8 uiValue =  m_pkHoleData[iY * m_uiSrcStride + (iX / 8)];
			return (uiValue & ( FvUInt8(1) << (iX & 0x7 ))) != 0;
		}

		FvUInt64 DXT1Block(FvUInt32 iX,FvUInt32 iY)
		{
			FvUInt64 uiBlock = FvUInt64(0x0000ffff);
			FvUInt64 uiMask = FvUInt64(1) << 32;
			for (FvUInt32 iYY = 0; iYY < 4; iYY++)
			{
				for (FvUInt32 iXX = 0; iXX < 4; iXX++)
				{	
					if (!this->Hole(iX + iXX,iY + iYY))
					{
						uiBlock |= uiMask;
					}
					uiMask <<= 2;
				}
			}
			return uiBlock;
		}

	protected:

		FvUInt32 m_uiSrcStride;
		FvUInt32 m_uiMaxX;
		FvUInt32 m_uiMaxY;
		FvUInt8 *m_pkHoleData;
	};

	const FvUInt32 LargerPow2( FvUInt32 uiNumber )
	{
		const float LOG2_10 = 3.3219280948873626f;
		float fN = log10f( float(uiNumber) ) * LOG2_10;
		FvUInt32 uiShift = FvUInt32(ceil(fN));
		if (fN - floor(fN) < 0.01f)
			uiShift = FvUInt32(floor(fN));
		return 1 << uiShift;
	}
}

FvTerrainPage::FvTerrainPage(FvTerrainSettingsPtr spSettings) :
m_pkHoles(NULL),
m_uiHolesMapWidth(0),
m_uiHolesMapHeight(0),
m_uiHolesMapSize(0),
m_uiHolesSize(0),
m_bAllHoles(false),
m_bNoHoles(true),
m_pkMaterialIndexs(NULL),
m_uiMaterialWidth(0),
m_uiMaterialHeight(0),
m_eOceanType(OCEAN_TYPE_NONE),
m_fMinHeight(0.f),
m_fMaxHeight(0.f),
m_uiHeightLodLevel(0),
m_uiVisiableOffset(0),
m_spTerrainSettings(spSettings)
#ifndef FV_SERVER
,m_kTerrainRenderable(spSettings->NumTitles()*
					  spSettings->NumTitles(),NULL),
m_bIsInLodControl(false)
#endif // !FV_SERVER
{
#ifndef FV_SERVER
	ms_uiTerrainPageIdentify++;
	FvString kPageIdentify = "FvTerrainPage_";
	char pcIdentify[9];
	sprintf_s(pcIdentify,9,"%I32x",FvInt32(ms_uiTerrainPageIdentify));
	kPageIdentify += pcIdentify;

	m_kHolesName = kPageIdentify + "_Holes";
	m_kNormalName = kPageIdentify + "_Normal";
	m_kLodNormalName = kPageIdentify + "_LodNormal";
	m_kHorizonShadowName = kPageIdentify + "_HorizonShadow";
	m_kLodName = kPageIdentify + "_LodTexture";
	//m_kBlendTextureName = kPageIdentify + "_BlendTexture";

	m_akMaterialName[MATERIAL_BLEND] =
		kPageIdentify + "_Material" + "_Blend";
	m_akMaterialName[MATERIAL_INTERIM] =
		kPageIdentify + "_Material" + "_Interim";
	m_akMaterialName[MATERIAL_LOD] =
		kPageIdentify + "_Material" + "_Lod";

#endif // !FV_SERVER
}

FvTerrainPage::~FvTerrainPage()
{
#ifndef FV_SERVER
	FvTerrainLodController::Instance().DelPage(this);
#endif // FV_SERVER

	delete[] m_pkHoles;
	delete[] m_pkMaterialIndexs;

#ifndef FV_SERVER

	this->ClearRenderable();

	for(int i = 0; i < MATERIAL_TYPE_NUMBER; i++)
	{
		if(!m_aspMaterial[i].isNull())
		{
			Ogre::MaterialManager::getSingleton().remove(m_aspMaterial[i]->getHandle());
			m_aspMaterial[i].setNull();
		}
	}

	if(!m_spHolesMap.isNull())
	{
		Ogre::TextureManager::getSingleton().remove(m_spHolesMap->getHandle());
		m_spHolesMap.setNull();
	}
	if(!m_spNormalMap.isNull())
	{
		Ogre::TextureManager::getSingleton().remove(m_spNormalMap->getHandle());
		m_spNormalMap.setNull();
	}
	if(!m_spLodNormalMap.isNull())
	{
		Ogre::TextureManager::getSingleton().remove(m_spLodNormalMap->getHandle());
		m_spLodNormalMap.setNull();
	}
	if(!m_spHorizonShadowMap.isNull())
	{
		Ogre::TextureManager::getSingleton().remove(m_spHorizonShadowMap->getHandle());
		m_spLodNormalMap.setNull();
	}
	if(!m_spLodMap.isNull())
	{
		Ogre::TextureManager::getSingleton().remove(m_spLodMap->getHandle());
		m_spLodMap.setNull();
	}

	for (size_t i = 0; i < m_kTextureLayers.size(); ++i)
	{
		FV_ASSERT(m_kTextureLayers[i]);
		if(m_kTextureLayers[i]->m_pkBlends != NULL)
			FV_ERROR_MSG("FvTerrainPage::~FvTerrainPage m_pkBlends not clear.");
	}
	m_kTextureLayers.clear();

	for (size_t i = 0; i < m_kCombinedLayers.size(); ++i)
	{
		if(!m_kCombinedLayers[i].m_spBlendTexture.isNull())
		{
			Ogre::TextureManager::getSingleton().remove(m_kCombinedLayers[i].m_spBlendTexture->getHandle());
			m_kCombinedLayers[i].m_spBlendTexture.setNull();
		}
		m_kCombinedLayers[i].m_kTextureLayers.clear();
	}

#endif // !FV_SERVER
}

bool FvTerrainPage::Load(const	FvString &kFileName, const FvString &kGroupName,
		  const FvVector3 &kWorldPosition, const FvVector3 &kCameraPosition)
{
	Ogre::DataStreamPtr spStream;
	try
	{
		spStream = Ogre::ResourceGroupManager::getSingleton().
			openResource(kFileName,kGroupName);
	}
	catch (...){}

	if(spStream.isNull())
		return false;

	FvTerrainPageSerializer kSerializer;
	if(!kSerializer.ImportTerrainPage(spStream,this))
	{
		return false;
	}

	m_kWorldPosition = kWorldPosition;

#ifndef FV_SERVER
	if(!m_aspMaterial[MATERIAL_BLEND].isNull())
		m_aspMaterial[MATERIAL_BLEND]->load();

	this->InitRenderable(kCameraPosition);

	m_kWireBB.setupBoundingBox(Ogre::AxisAlignedBox(
		Ogre::Vector3( kWorldPosition.x,
		kWorldPosition.y,
		kWorldPosition.z + m_fMinHeight), 
		Ogre::Vector3( kWorldPosition.x + FV_BLOCK_SIZE_METERS,
		kWorldPosition.y + FV_BLOCK_SIZE_METERS,
		kWorldPosition.z + m_fMaxHeight)));
#endif // !FV_SERVER

	m_kBoundingBox.SetBounds(FvVector3( 0,0,m_fMinHeight ),
		FvVector3( FV_BLOCK_SIZE_METERS,FV_BLOCK_SIZE_METERS,m_fMaxHeight ));

#ifndef FV_SERVER
	this->NotifyCameraPosition(kCameraPosition);
#endif // !FV_SERVER

	m_kResourceName = kFileName;

	return true;
}

bool FvTerrainPage::Collide(FvVector3 const &kStart, FvVector3 const &kEnd,
			 FvTerrainCollisionCallback *pkCallback) const
{
	return this->HMCollide(kStart, kStart, kEnd, pkCallback);
}

bool FvTerrainPage::Collide(FvWorldTriangle const &kStart,FvVector3 const &kEnd,
			 FvTerrainCollisionCallback *pkCallback) const
{
	FvVector3 kDelta = kEnd - kStart.Point0();
	FvBoundingBox kBB;
	kBB.AddBounds(kStart.Point0());
	kBB.AddBounds(kStart.Point1());
	kBB.AddBounds(kStart.Point2());
	kBB.AddBounds(kEnd);
	kBB.AddBounds(kStart.Point1() + kDelta);
	kBB.AddBounds(kStart.Point2() + kDelta);

	return this->HMCollide(kStart, kEnd, kBB.MinBounds().x, kBB.MinBounds().y,
		kBB.MaxBounds().x, kBB.MaxBounds().y, pkCallback);
}

namespace
{
	const float COLLISION_EPSILON = 0.0001f;
}

bool FvTerrainPage::HMCollide(FvVector3 const &kOriginalSource,FvVector3 const &kStart, 
			   FvVector3 const &kEnd,FvTerrainCollisionCallback *pkCallback) const
{
	FvVector3 kS = kStart;
	FvVector3 kE = kEnd;

	const float BOUNDING_BOX_EPSILON = 0.1f;

	FvBoundingBox kBB( FvVector3( 0, 0, this->MinHeight() ),
		FvVector3( FV_BLOCK_SIZE_METERS,FV_BLOCK_SIZE_METERS,
		this->MaxHeight() ) );

	const FvVector3 kGridAlign( float(this->PageWidth()) / FV_BLOCK_SIZE_METERS,
		float(this->PageHeight()) / FV_BLOCK_SIZE_METERS, 1.f);

	if (kBB.Clip(kS, kE, BOUNDING_BOX_EPSILON ))
	{
		kS.ParallelMultiply(kGridAlign);
		kE.ParallelMultiply(kGridAlign);

		FvVector3 kDir = kE - kS;
		if (!(FvAlmostZero(kDir.x) && FvAlmostZero(kDir.y)))
		{
			kDir *= 1.f / sqrtf(kDir.x * kDir.x + kDir.y * kDir.y);
		}

		FvInt32 iGridX = FvInt32(floorf(kS.x));
		FvInt32 iGridY = FvInt32(floorf(kS.y));

		FvInt32 iGridEndX = FvInt32(floorf(kE.x));
		FvInt32 iGridEndY = FvInt32(floorf(kE.y));

		FvInt32 iGridDirX = 0;
		FvInt32 iGridDirY = 0;

		if (kDir.x < 0) iGridDirX = -1; else if (kDir.x > 0) iGridDirX = 1;
		if (kDir.y < 0) iGridDirY = -1; else if (kDir.y > 0) iGridDirY = 1;

		while ((iGridX != iGridEndX &&
			!FvAlmostEqual( kS.x, kE.x, COLLISION_EPSILON)) ||
			(iGridY != iGridEndY &&
			!FvAlmostEqual( kS.y, kE.y, COLLISION_EPSILON )))
		{
			if (CheckGrid( iGridX, iGridY,
				kOriginalSource, kEnd, pkCallback))
				return true;

			float xDistNextGrid = iGridDirX < 0 ? float(iGridX) - kS.x : float(iGridX + 1) - kS.x;
			float xDistEnd = kE.x - kS.x;

			bool xEnding = fabsf(xDistEnd) < fabsf(xDistNextGrid);

			float xDist = xEnding ? xDistEnd : xDistNextGrid;

			float yDistNextGrid = iGridDirY < 0 ? float(iGridY) - kS.y : float(iGridY + 1) - kS.y;
			float yDistEnd = kE.y - kS.y;

			bool yEnding = fabsf(yDistEnd) < fabsf(yDistNextGrid);

			float yDist = yEnding ? yDistEnd : yDistNextGrid;

			float a = xDist / kDir.x;
			float b = yDist / kDir.y;

			if (iGridDirY == 0 ||
				a < b)
			{
				if (xEnding)
				{
					kS = kE;
				}
				else
				{
					iGridX += iGridDirX;
					kS.x += a * kDir.x;
					kS.y += a * kDir.y;
					kS.z += a * kDir.z;
				}
			}
			else
			{
				if (yEnding)
				{
					kS = kE;
				}
				else
				{
					iGridY += iGridDirY;
					kS.x += b * kDir.x;
					kS.y += b * kDir.y;
					kS.z += b * kDir.z;
				}
			}
		}
		if (CheckGrid( iGridX, iGridY,
			kOriginalSource, kEnd, pkCallback))
			return true;
	}
	return false;
}

bool FvTerrainPage::HMCollide(FvWorldTriangle const &kTriStart, FvVector3 const &kTriEnd,
			   float fXStart,float fYStart,float fXEnd,float fYEnd,
			   FvTerrainCollisionCallback *pkCallback) const
{
	const float fXMul = 1.f / SpacingX();
	const float fYMul = 1.f / SpacingY();

	int iXS = int(fXStart * fXMul);
	int iXE = int(ceilf(fXEnd * fXMul));
	int iYS = int(fYStart * fYMul);
	int iYE = int(ceilf(fYEnd * fYMul));

	for (int y = iYS; y < iYE; y++)
	{
		for (int x = iXS; x < iXE; x++)
		{
			if ( CheckGrid( x, y, kTriStart, kTriEnd, pkCallback ) )
			{
				return true;
			}
		}
	}

	return false;
}

float FvTerrainPage::HeightAt(float fX, float fY) const
{
	if(this->HoleAt(fX,fY))
		return FV_NO_TERRAIN;

	float fXS = (fX / this->SpacingX()) + m_uiVisiableOffset;
	float fYS = (fY / this->SpacingY()) + m_uiVisiableOffset;

	float fXF = fXS - floorf(fXS);
	float fYF = fYS - floorf(fYS);

	FvInt32 iXOff = FvInt32(floorf(fXS));
	FvInt32 iYOff = FvInt32(floorf(fYS));

	float fRes = 0;

	if ((iXOff ^ iYOff) & 1)
	{
		float fH01 = GetHeightValue(iXOff,iYOff + 1);
		float fH10 = GetHeightValue(iXOff + 1,iYOff);

		if ((1.f - fXF) > fYF)
		{
			float fH00 = GetHeightValue(iXOff,iYOff);
			fRes = fH00 + (fH10 - fH00) * fXF + (fH01 - fH00) * fYF;
		}
		else
		{
			float fH11 = GetHeightValue(iXOff + 1,iYOff + 1);
			fRes = fH11 + (fH01 - fH11) * (1.f - fXF) + (fH10 - fH11) * (1.f - fYF);
		}
	}
	else
	{
		float fH00 = GetHeightValue(iXOff,iYOff);
		float fH11 = GetHeightValue(iXOff + 1,iYOff + 1);

		if (fXF > fYF)
		{
			float fH10 = GetHeightValue(iXOff + 1,iYOff);
			fRes = fH10 + (fH00 - fH10) * (1.f - fXF) + (fH11 - fH10) * fYF;
		}
		else
		{
			float fH01 = GetHeightValue(iXOff,iYOff + 1);
			fRes = fH01 + (fH11 - fH01) * fXF + (fH00 - fH01) * (1.f - fYF);
		}
	}

	return fRes;
}

FvVector3 FvTerrainPage::NormalAt(int iX, int iY) const
{
	float fSpaceX = this->SpacingX();
	float fSpaceY = this->SpacingY();

	FV_ASSERT( fSpaceX == fSpaceY );

	const float fDiagonalMultiplier = 0.7071067811f;// sqrt( 0.5 )

	FvVector3 kRet;

	kRet.z = sqrtf( SpacingX() * SpacingY() * 2 ) * 4 + (fSpaceX * 2 + fSpaceY * 2);

	float fVal1 = 0.0f;
	float fVal2 = 0.0f;
	if (iX - 1 < 0 || iY - 1 < 0 ||
		iX + 1 >= int( m_kHeights.getWidth() ) || iY + 1 >= int( m_kHeights.getHeight() ))
	{
		kRet.x = this->HeightAt( iX-1, iY ) - this->HeightAt( iX+1, iY );
		kRet.y = this->HeightAt( iX, iY-1 ) - this->HeightAt( iX, iY+1 );

		fVal1 = this->HeightAt( iX-1, iY-1 ) - this->HeightAt( iX+1, iY+1 );
		fVal2 = this->HeightAt( iX-1, iY+1 ) - this->HeightAt( iX+1, iY-1 );
	}
	else
	{
		float *pfYMin = this->GetHeightRow( iY - 1 ) + iX;
		float *pfYCen = this->GetHeightRow( iY ) + iX;
		float *pfYMax = this->GetHeightRow( iY + 1 ) + iX;

		kRet.x = *(pfYCen - 1) - *(pfYCen + 1);
		kRet.y = *(pfYMin) - *(pfYMax);

		fVal1 = *(pfYMin - 1) - *(pfYMax + 1);
		fVal2 = *(pfYMax - 1) - *(pfYMin + 1);
	}

	fVal1 *= fDiagonalMultiplier;

	fVal2 *= fDiagonalMultiplier;

	kRet.x += fVal1;
	kRet.y += fVal1;

	kRet.x += fVal2;
	kRet.y -= fVal2;
	kRet.Normalise();

	return kRet;
}

FvVector3 FvTerrainPage::NormalAt(float fX, float fY) const
{
	float fXF = fX/this->SpacingX() + m_uiVisiableOffset;
	float fYF = fY/this->SpacingY() + m_uiVisiableOffset;

	FvVector3 kRet;
	float fXFrac = fXF - ::floorf(fXF);
	float fYFrac = fYF - ::floorf(fYF);

	fXF -= fXFrac;
	fYF -= fYFrac;

	int iXF = (int)fXF;
	int iYF = (int)fYF;

	const float FRAC_EPSILON = 1e-3f;
	if (fabs(fXFrac) < FRAC_EPSILON)
	{
		if (fabs(fYFrac) < FRAC_EPSILON)
		{
			return this->NormalAt(iXF, iYF);
		}
		else
		{
			FvVector3 n1 = this->NormalAt(iXF, iYF    );
			FvVector3 n2 = this->NormalAt(iXF, iYF + 1);
			n1 *= (1.0f - fYFrac);
			n2 *= fYFrac;
			kRet = n1;
			kRet += n2;
			kRet.Normalise();
			return kRet;
		}
	}
	else if (fabs(fYFrac) < FRAC_EPSILON)
	{
		FvVector3 n1 = this->NormalAt(iXF    , iYF);
		FvVector3 n2 = this->NormalAt(iXF + 1, iYF);
		n1 *= (1.0f - fXFrac);
		n2 *= fXFrac;
		kRet = n1;
		kRet += n2;
		kRet.Normalise();
		return kRet;
	}

	FvVector3 n1 = this->NormalAt(iXF    , iYF    );
	FvVector3 n2 = this->NormalAt(iXF + 1, iYF    );
	FvVector3 n3 = this->NormalAt(iXF    , iYF + 1);
	FvVector3 n4 = this->NormalAt(iXF + 1, iYF + 1);

	n1 *= (1 - fXFrac)*(1 - fYFrac);
	n2 *= (    fXFrac)*(1 - fYFrac);
	n3 *= (1 - fXFrac)*(	fYFrac);
	n4 *= (    fXFrac)*(	fYFrac);

	kRet  = n1;
	kRet += n2;
	kRet += n3;
	kRet += n4;

	kRet.Normalise();
	return kRet;
}

bool FvTerrainPage::HoleAt(float fX, float fY) const
{
	if (m_bAllHoles) return true;
	if (m_bNoHoles) return false;

	return this->GetHoleValue( FvInt32((fX / FV_BLOCK_SIZE_METERS) * float(HolesMapWidth())),
		FvInt32((fY / FV_BLOCK_SIZE_METERS) * float(HolesMapHeight())) );
}

bool FvTerrainPage::HoleAt(float fStartX, float fStartY, float fEndX, float fEndY) const
{
	if (m_bAllHoles) return true;
	if (m_bNoHoles) return false;

	FvInt32 iStartX = FvInt32(floorf((fStartX / FV_BLOCK_SIZE_METERS) * float(this->HolesMapWidth())));
	FvInt32 iEndX = FvInt32(floorf((fEndX / FV_BLOCK_SIZE_METERS) * float(this->HolesMapWidth())));

	FvInt32 iStartY = FvInt32(floorf((fStartY / FV_BLOCK_SIZE_METERS) * float(this->HolesMapHeight())));
	FvInt32 iEndY = FvInt32(floorf((fEndY / FV_BLOCK_SIZE_METERS) * float(this->HolesMapHeight())));

	for (FvInt32 iY = iStartY; iY <= iEndY; iY++)
	{
		for (FvInt32 iX = iStartX; iX <= iEndX; iX++)
		{
			if (this->GetHoleValue(iX, iY))
			{
				return true;
			}
		}
	}

	return false;
}

const FvString &FvTerrainPage::MaterialName(float fX,float fY) const
{
	FvUInt8 uiIndex = this->MaterialIndex(fX,fY);
	FV_ASSERT( uiIndex < m_kMaterialNames.size() );
	return m_kMaterialNames[uiIndex];
}

bool FvTerrainPage::GenerateHolesMap(FvUInt8 *pkData, FvUInt32 uiWidth, FvUInt32 uiHeight)
{
	if(pkData)
	{
		m_uiHolesSize = uiWidth;
		m_uiHolesMapSize = LargerPow2( m_uiHolesSize );
		size_t stSize = uiHeight * ((uiWidth >> 3) + (((uiWidth % 8) != 0) ? 1 : 0));
		m_pkHoles = new FvUInt8[stSize];
		::memcpy(m_pkHoles,pkData,stSize);
#ifndef FV_SERVER
		FvUInt32 uiDataSize = uiWidth*uiHeight/2;
		uchar *pkData = OGRE_ALLOC_T(uchar,uiDataSize,MEMCATEGORY_GENERAL);
		FvUInt64 *pkTempData = (FvUInt64*)pkData;
		FvHoleHelper kHoleHelper(m_pkHoles,uiWidth,uiHeight);
		for (FvUInt32 iY = 0; iY < uiHeight; iY += 4)
		{
			for (FvUInt32 iX = 0; iX < uiWidth; iX+= 4)
			{
				*(pkTempData++) = kHoleHelper.DXT1Block(iX,iY);
			}
		}
		Ogre::DataStreamPtr spStreamData(OGRE_NEW Ogre::MemoryDataStream(pkData,uiDataSize,true));
		m_spHolesMap = Ogre::TextureManager::getSingleton().loadRawData(this->m_kHolesName,
			FvTerrainPage::TERRAIN_INERNAL_RESOURCE_GROUP,spStreamData,uiWidth,uiHeight,Ogre::PF_DXT1);
#endif // !FV_SERVER
		{
			m_uiHolesMapWidth = uiWidth;
			m_uiHolesMapHeight = uiHeight;
			this->RecalculateHoles();
		}
	}
	return false;
}

void FvTerrainPage::RecalculateHoles()
{
	if (m_pkHoles == NULL)
	{
		m_bAllHoles = false;
		m_bNoHoles = true;
	}
	else
	{
		m_bAllHoles = true;
		bool bHoles = false;

		for (FvUInt32 uiY = 0; uiY < m_uiHolesMapHeight; ++uiY)
		{
			for (FvUInt32 uiX = 0; uiX < m_uiHolesMapWidth; ++uiX)
			{
				bool bV = this->GetHoleValue(uiX, uiY);
				m_bAllHoles &= bV;
				bHoles |= bV;
			}
		}
		m_bNoHoles = !bHoles;
		if (m_bNoHoles || m_bAllHoles)
		{
			delete m_pkHoles;
			m_pkHoles = NULL;
#ifndef FV_SERVER
			TextureManager::getSingleton().remove(m_spHolesMap->getHandle());
			m_spHolesMap.setNull();
#endif // !FV_SERVER
		}
	}
}

FvTerrainFinder::Details FvTerrainPage::FindTerrainPage(FvVector3 const &kPos)
{
	if (ms_pkTerrainFinder == NULL)
		return FvTerrainFinder::Details();
	else
		return ms_pkTerrainFinder->FindTerrainPage(kPos);
}

float FvTerrainPage::GetHeight(float fX, float fY)
{
	FvVector3 kPos(fX, fY, 0.0f);
	FvTerrainFinder::Details kFindDetails =
		FvTerrainPage::FindTerrainPage(kPos);

	FvTerrainPage *pkPage = kFindDetails.m_pkPage;
	if (pkPage != NULL)
	{
		FvVector3 kTerPos = kFindDetails.m_pkInvMatrix->ApplyPoint(kPos);
		float fHeight = pkPage->HeightAt(kTerPos.x, kTerPos.y);
		if (fHeight != FV_NO_TERRAIN)
		{
			return fHeight - kFindDetails.m_pkInvMatrix->ApplyToOrigin().z;
		}
	}

	return FV_NO_TERRAIN;
}

void FvTerrainPage::SetTerrainFinder(FvTerrainFinder &kTerrainFinder)
{
    ms_pkTerrainFinder = &kTerrainFinder;
}

#ifndef FV_SERVER

void FvTerrainPage::NotifyCameraPosition( const FvVector3 &kPos )
{
	
}

void FvTerrainPage::InitRenderable(const FvVector3 &kCameraPosition)
{
	for(FvUInt32 uiTitleX = 0; uiTitleX < this->NumTitles(); uiTitleX++)
	{
		for(FvUInt32 uiTitleY = 0; uiTitleY < this->NumTitles(); uiTitleY++)
		{
			FvTerrainRenderable *pkRenderable = new FvTerrainRenderable(*this, uiTitleX, uiTitleY);
			m_kTerrainRenderable[uiTitleY * this->NumTitles() + uiTitleX] = pkRenderable;
			pkRenderable->NotifyCameraPosition(kCameraPosition);
		}
	}
}

void FvTerrainPage::ClearRenderable()
{
	Renderables::iterator kIt = m_kTerrainRenderable.begin();
	for(; kIt != m_kTerrainRenderable.end(); ++kIt)
		delete *kIt;

	m_kTerrainRenderable.clear();
}

Ogre::TexturePtr CreateBlendTexture( 
						FvTerrainPage::TextureLayerPtr spLayer0,
						FvTerrainPage::TextureLayerPtr spLayer1,
						FvTerrainPage::TextureLayerPtr spLayer2,
						FvTerrainPage::TextureLayerPtr spLayer3)
{
	if(!spLayer0)
	{
		FV_ERROR_MSG("CreateBlendTexture No layer0 to create texture from\n");
		return Ogre::TexturePtr();
	}

	FvUInt32 uiLayerNum = 0;
	if (spLayer0) ++uiLayerNum;
	if (spLayer1) ++uiLayerNum;
	if (spLayer2) ++uiLayerNum;
	if (spLayer3) ++uiLayerNum;

	FvUInt32 uiWidth = FvUInt32(spLayer0->m_uiWidth);
	FvUInt32 uiHeight = FvUInt32(spLayer0->m_uiHeight);

	FvUInt32 uiPixelSize = 4;
	Ogre::PixelFormat ePixelFormat= Ogre::PF_A8R8G8B8;
	switch(uiLayerNum)
	{
	case 1:ePixelFormat = Ogre::PF_A8		;uiPixelSize = 1;break;
	case 2:ePixelFormat = Ogre::PF_BYTE_LA	;uiPixelSize = 2;break;
	case 3:ePixelFormat = Ogre::PF_A8R8G8B8	;uiPixelSize = 4;break;
	case 4:ePixelFormat = Ogre::PF_A8R8G8B8	;uiPixelSize = 4;break;
	}

	FvUInt32 uiDataSize = uiHeight*uiWidth*uiPixelSize;
	uchar *pcRawData = OGRE_ALLOC_T(uchar, uiDataSize, MEMCATEGORY_GENERAL);
	uchar *pcTempData = pcRawData;

	static FvInt32 iIdentify = 0;
	char pcBlendTextureName[32];
	sprintf_s(pcBlendTextureName,32,"%I32x",iIdentify);
	iIdentify++;

	if(uiLayerNum == 1)
	{
		for(FvUInt32 i = 0; i < uiHeight; i++)
		{
			for(FvUInt32 j = 0; j < uiWidth; j++)
			{
				*(pcTempData) = *(spLayer0->m_pkBlends->getData() + i * uiWidth + j);
				pcTempData += uiPixelSize;
			}
		}
	}
	else if(uiLayerNum == 2)
	{
		for(FvUInt32 i = 0; i < uiHeight; i++)
		{
			for(FvUInt32 j = 0; j < uiWidth; j++)
			{
				*(pcTempData) = *(spLayer1->m_pkBlends->getData() + i * uiWidth + j);
				*(pcTempData + 1) = *(spLayer0->m_pkBlends->getData() + i * uiWidth + j);
				pcTempData += uiPixelSize;
			}
		}
	}
	else if(uiLayerNum == 3)
	{
		for(FvUInt32 i = 0; i < uiHeight; i++)
		{
			for(FvUInt32 j = 0; j < uiWidth; j++)
			{
				*(pcTempData + 1) = *(spLayer2->m_pkBlends->getData() + i * uiWidth + j);
				*(pcTempData + 2) = *(spLayer1->m_pkBlends->getData() + i * uiWidth + j);
				*(pcTempData + 3) = *(spLayer0->m_pkBlends->getData() + i * uiWidth + j);
				pcTempData += uiPixelSize;
			}
		}
	}
	else if(uiLayerNum == 4)
	{
		for(FvUInt32 i = 0; i < uiHeight; i++)
		{
			for(FvUInt32 j = 0; j < uiWidth; j++)
			{
				*(pcTempData) = *(spLayer3->m_pkBlends->getData() + i * uiWidth + j);
				*(pcTempData + 1) = *(spLayer2->m_pkBlends->getData() + i * uiWidth + j);
				*(pcTempData + 2) = *(spLayer1->m_pkBlends->getData() + i * uiWidth + j);
				*(pcTempData + 3) = *(spLayer0->m_pkBlends->getData() + i * uiWidth + j);
				pcTempData += uiPixelSize;
			}
		}
	}

	Ogre::MemoryDataStreamPtr spStreamData(OGRE_NEW Ogre::MemoryDataStream(pcRawData,uiDataSize,true));
	return Ogre::TextureManager::getSingleton().loadRawData(pcBlendTextureName,
		FvTerrainPage::TERRAIN_INERNAL_RESOURCE_GROUP,DataStreamPtr(spStreamData),uiWidth,uiHeight,ePixelFormat);
}

void FvTerrainPage::GenerateCombinedLayers()
{
	if(m_kTextureLayers.size() <= 0)
		return;

	TextureLayers kTempTextureLayers = m_kTextureLayers;

	m_kCombinedLayers.clear();
	m_kCombinedLayers.reserve( ( kTempTextureLayers.size() >> 2 ) + 1 );

	while (kTempTextureLayers.size())
	{
		TextureLayerPtr aspLayers[4];
		FvUInt32 uiLayerIndex = 0;
		CombinedLayer kCombinedLayer;
		kCombinedLayer.m_uiHeight = kTempTextureLayers[0]->m_uiHeight;
		kCombinedLayer.m_uiWidth = kTempTextureLayers[0]->m_uiWidth;
		for (int i = 0; (i < (signed)kTempTextureLayers.size()) &&
			(uiLayerIndex != 4); i++)
		{
			if (kTempTextureLayers[i]->m_uiHeight == kCombinedLayer.m_uiHeight &&
				kTempTextureLayers[i]->m_uiWidth == kCombinedLayer.m_uiWidth)
			{
				aspLayers[uiLayerIndex++] = kTempTextureLayers[i];
				kCombinedLayer.m_kTextureLayers.push_back( kTempTextureLayers[i] );
				kTempTextureLayers.erase( kTempTextureLayers.begin() + i );
				--i;
			}
		}

		kCombinedLayer.m_spBlendTexture = 
			CreateBlendTexture(aspLayers[0], aspLayers[1], aspLayers[2], aspLayers[3]);

		if (kCombinedLayer.m_spBlendTexture.isNull())
		{
			FV_ERROR_MSG( "FvTerrainPage::GenerateCombinedLayers: "
				"Error creating blend texture (see log)." );
		}
		else
		{
			m_kCombinedLayers.push_back( kCombinedLayer );
		}
	}

	for (size_t i = 0; i < m_kTextureLayers.size(); ++i)
	{
		TextureLayer *pkLayer =
			((TextureLayer *)m_kTextureLayers[i].Get());
		delete pkLayer->m_pkBlends;
		pkLayer->m_pkBlends = NULL;
	}
}

static const Ogre::Vector4 kLayerMaskTable[] = 
{										   // layers x  y  z  w
	Ogre::Vector4(0.0f, 0.0f, 0.0f, 0.0f), // 0
	Ogre::Vector4(1.0f, 0.0f, 0.0f, 0.0f), // 1      t0      
	Ogre::Vector4(1.0f, 0.0f, 0.0f, 1.0f), // 2      t0       t1
	Ogre::Vector4(1.0f, 1.0f, 1.0f, 0.0f), // 3      t2 t1 t0
	Ogre::Vector4(1.0f, 1.0f, 1.0f, 1.0f)  // 4      t2 t1 t0 t3
};

static const Ogre::Vector4 kLayerMaskTable32bit[] = 
{										   // layers x  y  z  w
	Ogre::Vector4(0.0f, 0.0f, 0.0f, 0.0f), // 0
	Ogre::Vector4(0.0f, 0.0f, 0.0f, 1.0f), // 1            t0      
	Ogre::Vector4(1.0f, 0.0f, 0.0f, 1.0f), // 2         t1 t0 
	Ogre::Vector4(1.0f, 1.0f, 0.0f, 1.0f), // 3      t2 t1 t0
	Ogre::Vector4(1.0f, 1.0f, 1.0f, 1.0f)  // 4      t2 t1 t0 t3
};

void FvTerrainPage::GenerateMaterial()
{
	FV_ASSERT(m_spTerrainSettings);

	float fBlendMapSize = (float)m_spTerrainSettings->BlendMapSize();
	float fNormalMapSize = (float)m_spTerrainSettings->NormalMapSize();
	float fShadowMapSize = (float)m_spTerrainSettings->ShadowMapSize();
	float fInvBlendMapSize = 1.0f / fBlendMapSize;
	float fInvNormalMapSize = 1.0f / fNormalMapSize;
	float fInvShadowMapSize = 1.0f / fShadowMapSize;
	float fHoleMapSize = (float)m_spTerrainSettings->HoleMapSize();
	float fInvHoleMapSize = 1.0f / fHoleMapSize;

	float fLodMapSize = (float)(m_spLodMap.isNull()?128.f:m_spLodMap->getWidth());
	float fLodNormalMapSize = (float)(m_spLodNormalMap.isNull()?128.f:m_spLodNormalMap->getWidth());
	float fInvLodMapSize =  1.0f / fLodMapSize;
	float fInvLodNormalMapSize = 1.0f / fLodNormalMapSize;

	Ogre::Vector4 kMapBias;
	kMapBias.x = (fBlendMapSize - 1) * fInvBlendMapSize;
	kMapBias.y = (fNormalMapSize - 1) * fInvNormalMapSize;
	kMapBias.z = fInvBlendMapSize * 0.5f;
	kMapBias.w = fInvNormalMapSize * 0.5f;

	Ogre::Vector4 kMapLodBias;
	kMapLodBias.x = (fLodMapSize - 1) * fInvLodMapSize;
	kMapLodBias.y = (fLodNormalMapSize - 1) * fInvLodNormalMapSize;
	kMapLodBias.z = fInvLodMapSize * 0.5f;
	kMapLodBias.w = fInvLodNormalMapSize * 0.5f;

	Ogre::Vector4 kExtra;
	kExtra.x = (float)(m_spTerrainSettings->LodTextureStart() + m_spTerrainSettings->LodTextureDistance());
	kExtra.y = (float)m_spTerrainSettings->LodTextureStart();
	kExtra.y = 1.0f / (kExtra.y - kExtra.x);

	kExtra.z = (fShadowMapSize - 1) * fInvShadowMapSize;
	kExtra.w = fInvShadowMapSize * 0.5f;

	Ogre::Vector4 kExtraLod;
	kExtraLod.x = (float)m_spTerrainSettings->LodTextureStart();
	kExtraLod.y = (float)(m_spTerrainSettings->LodTextureStart() + m_spTerrainSettings->LodTextureDistance());
	kExtraLod.y = 1.0f / (kExtraLod.y - kExtraLod.x);

	kExtraLod.z = kExtra.z;
	kExtraLod.w = kExtra.w;

	bool bHasHole = !(m_spHolesMap.isNull());

	Ogre::Vector4 kHoleBias;
	if(bHasHole)
	{
		kHoleBias.x = (fHoleMapSize - 1) * fInvHoleMapSize;
		kHoleBias.y = fInvHoleMapSize * 0.5f;
	}

	for(FvUInt32 i(0); i < MATERIAL_TYPE_NUMBER; ++i)
	{
		m_aspMaterial[i] = Ogre::MaterialManager::getSingleton().create(
			m_akMaterialName[i], FvTerrainPage::TERRAIN_INERNAL_RESOURCE_GROUP);

		m_aspMaterial[i]->removeAllTechniques();

		GenerateShadowCaster(i, m_aspMaterial[i], bHasHole);

		GenerateLevel2Lighting(i, m_aspMaterial[i], bHasHole);

		GenerateLevel2(i, m_aspMaterial[i], bHasHole);

		GenerateLevel0(i, m_aspMaterial[i], bHasHole);

		GenerateLevelLow(m_aspMaterial[i], bHasHole);
	}

	for(FvUInt32 i(0); i < MATERIAL_TYPE_NUMBER; ++i)
	{
		if(m_aspMaterial[i].isNull())
			return;

		for(FvUInt32 j(0); j < m_aspMaterial[i]->getNumTechniques(); ++j)
		{
			Ogre::Pass* pkPass = m_aspMaterial[i]->getTechnique(j)->getPass("Hole");
			if(pkPass)
			{
				Ogre::TextureUnitState *pkTexUnit = NULL;

				pkTexUnit = pkPass->getTextureUnitState("HoleMap");
				if(pkTexUnit)
				{
					pkTexUnit->setTextureName(m_kHolesName);
				}

				GpuProgramParametersSharedPtr spVertexParameters = pkPass->getVertexProgramParameters();

				spVertexParameters->setConstant(10, kHoleBias);
			}

			char acBuffer[64];

			for(FvUInt32 k(0); k < m_kCombinedLayers.size(); ++k)
			{
				sprintf_s(acBuffer, "Blend%d", k);
				pkPass = m_aspMaterial[i]->getTechnique(j)->getPass(acBuffer);
				if(pkPass)
				{
					GpuProgramParametersSharedPtr spVertexParameters = pkPass->getVertexProgramParameters();
					GpuProgramParametersSharedPtr spPixelParameters = pkPass->getFragmentProgramParameters();

					Ogre::TextureUnitState *pkTexUnit = NULL;

					pkTexUnit = pkPass->getTextureUnitState("NormalMap");
					if(pkTexUnit)
					{
						pkTexUnit->setTextureName(m_kNormalName);
					}

					pkTexUnit = pkPass->getTextureUnitState("BlendMap");
					if(pkTexUnit)
					{
						pkTexUnit->setTextureName(m_kCombinedLayers[k].m_spBlendTexture->getName());
					}		

					for(FvUInt32 l(0); l < m_kCombinedLayers[k].m_kTextureLayers.size(); ++l)
					{		
						sprintf_s(acBuffer, "LayerMap%d", l);
						pkTexUnit = pkPass->getTextureUnitState(acBuffer);
						if(pkTexUnit)
						{
							pkTexUnit->setTextureName(m_kCombinedLayers[k].m_kTextureLayers[l]->m_kTextureName);

							spVertexParameters->setConstant(12 + l*2, 
								Ogre::Vector4(m_kCombinedLayers[k].m_kTextureLayers[l]->m_kUProjection.x,
								m_kCombinedLayers[k].m_kTextureLayers[l]->m_kUProjection.y,
								m_kCombinedLayers[k].m_kTextureLayers[l]->m_kUProjection.z,
								m_kCombinedLayers[k].m_kTextureLayers[l]->m_kUProjection.w));
							spVertexParameters->setConstant(13 + l*2,
								Ogre::Vector4(m_kCombinedLayers[k].m_kTextureLayers[l]->m_kVProjection.x,
								m_kCombinedLayers[k].m_kTextureLayers[l]->m_kVProjection.y,
								m_kCombinedLayers[k].m_kTextureLayers[l]->m_kVProjection.z,
								m_kCombinedLayers[k].m_kTextureLayers[l]->m_kVProjection.w));
						}
					}

					pkTexUnit = pkPass->getTextureUnitState("HorizonShadowMap");
					if(pkTexUnit)
					{
						pkTexUnit->setTextureName(m_kHorizonShadowName);
					}

					spVertexParameters->setConstant(10, kExtra);
					spVertexParameters->setConstant(11, kMapBias);
				}
			}

			pkPass = m_aspMaterial[i]->getTechnique(j)->getPass("Lod");
			if(pkPass)
			{
				GpuProgramParametersSharedPtr spVertexParameters = pkPass->getVertexProgramParameters();
				GpuProgramParametersSharedPtr spPixelParameters = pkPass->getFragmentProgramParameters();

				Ogre::TextureUnitState *pkTexUnit = NULL;

				pkTexUnit = pkPass->getTextureUnitState("LodMap");
				if(pkTexUnit)
				{
					pkTexUnit->setTextureName(m_kLodName);
				}

				pkTexUnit = pkPass->getTextureUnitState("NormalMap");
				if(pkTexUnit)
				{
					pkTexUnit->setTextureName(m_kLodNormalName);
				}

				pkTexUnit = pkPass->getTextureUnitState("HorizonShadowMap");
				if(pkTexUnit)
				{
					pkTexUnit->setTextureName(m_kHorizonShadowName);
				}

				spVertexParameters->setConstant(10, kExtraLod);
				spVertexParameters->setConstant(11, kMapLodBias);
			}

			pkPass = m_aspMaterial[i]->getTechnique(j)->getPass("Low");
			if(pkPass)
			{
				Ogre::TextureUnitState *pkTexUnit = NULL;

				pkTexUnit = pkPass->getTextureUnitState("LodMap");
				if(pkTexUnit)
				{
					pkTexUnit->setTextureName(m_kLodName);
				}

				GpuProgramParametersSharedPtr spVertexParameters = pkPass->getVertexProgramParameters();

				spVertexParameters->setConstant(10, kMapLodBias);
			}

			/*Ogre::Pass* pkPass = m_aspMaterial[i]->getTechnique(j)->getPass("Blend");
			if(pkPass)
			{
				GpuProgramParametersSharedPtr spVertexParameters = pkPass->getVertexProgramParameters();
				GpuProgramParametersSharedPtr spPixelParameters = pkPass->getFragmentProgramParameters();

				Ogre::TextureUnitState *pkTexUnit = NULL;

				pkTexUnit = pkPass->getTextureUnitState("HolesMap");
				if(pkTexUnit)
				{
					pkTexUnit->setTextureName(m_kHolesName);
				}

				pkTexUnit = pkPass->getTextureUnitState("NormalMap");
				if(pkTexUnit)
				{
					pkTexUnit->setTextureName(m_kNormalName);
				}

				if(m_kCombinedLayers.size() == 0) continue;

				pkTexUnit = pkPass->getTextureUnitState("BlendMap");
				if(pkTexUnit)
				{
					pkTexUnit->setTextureName(m_kCombinedLayers[0].m_spBlendTexture->getName());

					spPixelParameters->setConstant(0,kLayerMaskTable32bit[m_kCombinedLayers[0].m_kTextureLayers.size()]);
				}

				char acLayerMap[10];			

				for(FvUInt32 k(0); k < m_kCombinedLayers[0].m_kTextureLayers.size(); ++k)
				{		
					sprintf_s(acLayerMap, "LayerMap%d", k);
					pkTexUnit = pkPass->getTextureUnitState(acLayerMap);
					if(pkTexUnit)
					{
						pkTexUnit->setTextureName(m_kCombinedLayers[0].m_kTextureLayers[k]->m_kTextureName);

						spVertexParameters->setConstant(12 + k*2, 
							Ogre::Vector4(m_kCombinedLayers[0].m_kTextureLayers[k]->m_kUProjection.x,
							m_kCombinedLayers[0].m_kTextureLayers[k]->m_kUProjection.y,
							m_kCombinedLayers[0].m_kTextureLayers[k]->m_kUProjection.z,
							m_kCombinedLayers[0].m_kTextureLayers[k]->m_kUProjection.w));
						spVertexParameters->setConstant(13 + k*2,
							Ogre::Vector4(m_kCombinedLayers[0].m_kTextureLayers[k]->m_kVProjection.x,
							m_kCombinedLayers[0].m_kTextureLayers[k]->m_kVProjection.y,
							m_kCombinedLayers[0].m_kTextureLayers[k]->m_kVProjection.z,
							m_kCombinedLayers[0].m_kTextureLayers[k]->m_kVProjection.w));
					}
				}

				pkTexUnit = pkPass->getTextureUnitState("HorizonShadowMap");
				if(pkTexUnit)
				{
					pkTexUnit->setTextureName(m_kHorizonShadowName);
				}

				spVertexParameters->setConstant(10, kExtra);
				spVertexParameters->setConstant(11, kMapBias);
			}

			pkPass = m_aspMaterial[i]->getTechnique(j)->getPass("Lod");
			if(pkPass)
			{
				GpuProgramParametersSharedPtr spVertexParameters = pkPass->getVertexProgramParameters();
				GpuProgramParametersSharedPtr spPixelParameters = pkPass->getFragmentProgramParameters();

				Ogre::TextureUnitState *pkTexUnit = NULL;

				pkTexUnit = pkPass->getTextureUnitState("HolesMap");
				if(pkTexUnit)
				{
					pkTexUnit->setTextureName(m_kHolesName);
				}

				pkTexUnit = pkPass->getTextureUnitState("LodMap");
				if(pkTexUnit)
				{
					pkTexUnit->setTextureName(m_kLodName);
				}

				pkTexUnit = pkPass->getTextureUnitState("NormalMap");
				if(pkTexUnit)
				{
					pkTexUnit->setTextureName(m_kLodNormalName);
				}

				pkTexUnit = pkPass->getTextureUnitState("HorizonShadowMap");
				if(pkTexUnit)
				{
					pkTexUnit->setTextureName(m_kHorizonShadowName);
				}

				spVertexParameters->setConstant(10, kExtraLod);
				spVertexParameters->setConstant(11, kMapLodBias);
			}*/
		}
	}
}


Ogre::Technique* FvTerrainPage::GenerateShadowCaster(
	FvUInt32 u32Number, const Ogre::MaterialPtr& spMaterial, bool bHole)
{
	FV_ASSERT(!spMaterial.isNull());

	Technique* pkShadowCaster = spMaterial->createTechnique();
	pkShadowCaster->setName("ShadowCaster");
	pkShadowCaster->setSchemeName("shadow_caster");

	if(bHole)
	{
		Pass* pkHole = pkShadowCaster->createPass();
		pkHole->setName("Hole");
		pkHole->setVertexProgram("FvTerrain_hole_ShadowCaster_VP");
		pkHole->setFragmentProgram("FvTerrain_hole_ShadowCaster_FP");
		Ogre::TextureUnitState* pkTex = pkHole->createTextureUnitState();
		pkTex->setName("HoleMap");
		pkTex->setTextureFiltering(Ogre::TFO_NONE);
		pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

		pkHole->setCullingMode(Ogre::CULL_NONE);
	}
	else
	{
		Pass* pkMain = pkShadowCaster->createPass();
		pkMain->setName("Main");
		pkMain->setVertexProgram("FvTerrain_ShadowCaster_VP");
		pkMain->setFragmentProgram("FvTerrain_ShadowCaster_FP");

		pkMain->setCullingMode(Ogre::CULL_NONE);
	}

	return pkShadowCaster;
}


Ogre::Technique* FvTerrainPage::GenerateLevel2Lighting(FvUInt32 u32Number,
	const Ogre::MaterialPtr& spMaterial, bool bHole)
{
	FV_ASSERT(!spMaterial.isNull());

	Technique* pkLevel2Lit = spMaterial->createTechnique();
	pkLevel2Lit->setName("level_2_lighting");
	pkLevel2Lit->setSchemeName("level_2_lighting");

	switch(u32Number)
	{
	case MATERIAL_BLEND:
		{
			Pass* pkLod = CreateLevel2LodLightingPass(pkLevel2Lit);
			if(bHole)
			{
				pkLod->setDepthFunction(Ogre::CMPF_EQUAL);
				pkLod->setDepthWriteEnabled(false);
			}
		}
		break;
	case MATERIAL_INTERIM:
		{
			Pass* pkLod = CreateLevel2LodLightingPass(pkLevel2Lit);
			if(bHole)
			{
				pkLod->setDepthFunction(Ogre::CMPF_EQUAL);
				pkLod->setDepthWriteEnabled(false);
			}
		}
		break;
	case MATERIAL_LOD:
		{
			Pass* pkLod = CreateLevel2LodLightingPass(pkLevel2Lit);
			if(bHole)
			{
				pkLod->setDepthFunction(Ogre::CMPF_EQUAL);
				pkLod->setDepthWriteEnabled(false);
			}
		}

		break;
	}

	return pkLevel2Lit;
}

Ogre::Technique* FvTerrainPage::GenerateLevel2( FvUInt32 u32Number,
	const Ogre::MaterialPtr& spMaterial, bool bHole)
{
	FV_ASSERT(!spMaterial.isNull());

	Technique* pkLevel2 = spMaterial->createTechnique();
	pkLevel2->setName("level_2");
	pkLevel2->setSchemeName("level_2");

	if(bHole)
	{
		CreateHolePass(pkLevel2);
	}

	switch(u32Number)
	{
	case MATERIAL_BLEND:
		{
			Pass* pkBlend = CreateLevel2BlendPass(pkLevel2, m_kCombinedLayers.size() - 1, "");
			if(bHole)
			{
				pkBlend->setDepthFunction(Ogre::CMPF_EQUAL);
				pkBlend->setDepthWriteEnabled(false);
			}

			for(FvInt32 i(((FvInt32)m_kCombinedLayers.size()) - 2); i >= 0; --i)
			{
				pkBlend = CreateLevel2BlendPass(pkLevel2, i, "_extra");
				pkBlend->setSceneBlending(Ogre::SBT_ADD);
				pkBlend->setDepthWriteEnabled(false);
				pkBlend->setDepthFunction(Ogre::CMPF_EQUAL);
			}
		}
		break;
	case MATERIAL_INTERIM:
		{
			Pass* pkLod = CreateLevel2LodPass(pkLevel2);
			if(bHole)
			{
				pkLod->setDepthFunction(Ogre::CMPF_EQUAL);
				pkLod->setDepthWriteEnabled(false);
			}

			Pass* pkBlend = CreateLevel2BlendPass(pkLevel2, m_kCombinedLayers.size() - 1, "_part");
			pkBlend->setSceneBlending(Ogre::SBT_ADD);
			pkBlend->setDepthWriteEnabled(false);
			pkBlend->setDepthFunction(Ogre::CMPF_EQUAL);

			for(FvInt32 i(((FvInt32)m_kCombinedLayers.size()) - 2); i >= 0; --i)
			{
				pkBlend = CreateLevel2BlendPass(pkLevel2, i, "_extra");
				pkBlend->setSceneBlending(Ogre::SBT_ADD);
				pkBlend->setDepthWriteEnabled(false);
				pkBlend->setDepthFunction(Ogre::CMPF_EQUAL);
			}
		}
		break;
	case MATERIAL_LOD:
		{
			Pass* pkLod = CreateLevel2LodPass(pkLevel2);
			if(bHole)
			{
				pkLod->setDepthFunction(Ogre::CMPF_EQUAL);
				pkLod->setDepthWriteEnabled(false);
			}
		}

		break;
	}

	return pkLevel2;
}

Ogre::Pass* FvTerrainPage::CreateLevel2BlendPass(
	Ogre::Technique* pkTech, FvUInt32 u32CombinedLayer,
	const char* pcOption)
{
	char acBuffer[64];
	Pass* pkBlend = pkTech->createPass();

	sprintf_s(acBuffer, "Blend%d", u32CombinedLayer);
	pkBlend->setName(acBuffer);

	FvUInt32 u32CurrentLayerNumber = m_kCombinedLayers[u32CombinedLayer].m_kTextureLayers.size();
	FV_ASSERT(u32CurrentLayerNumber <= 4);

	sprintf_s(acBuffer, "FvTerrain_level_2_blend%s_%d_VP", pcOption, u32CurrentLayerNumber);
	pkBlend->setVertexProgram(acBuffer);

	sprintf_s(acBuffer, "FvTerrain_level_2_blend%s_%d_FP", pcOption, u32CurrentLayerNumber);
	pkBlend->setFragmentProgram(acBuffer);

	Ogre::TextureUnitState* pkTex = pkBlend->createTextureUnitState();
	FV_ASSERT(pkTex);
	pkTex->setName("BlendMap");
	pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

	for(FvUInt32 i(0); i < u32CurrentLayerNumber; ++i)
	{
		pkTex = pkBlend->createTextureUnitState();
		FV_ASSERT(pkTex);
		sprintf_s(acBuffer, "LayerMap%d", i);
		pkTex->setName(acBuffer);
		pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
		pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
	}

	FvString kOption(pcOption);

	if(kOption == "")
	{
		pkTex = pkBlend->createTextureUnitState();
		FV_ASSERT(pkTex);
		pkTex->setName("NormalMap");
		pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
		pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

		pkTex = pkBlend->createTextureUnitState();
		FV_ASSERT(pkTex);
		pkTex->setName("HorizonShadowMap");
		pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
		pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	}
	else if(kOption != "_extra")
	{
		pkTex = pkBlend->createTextureUnitState();
		FV_ASSERT(pkTex);
		pkTex->setName("NormalMap");
		pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
		pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	}	

	return pkBlend;
}


Ogre::Pass* FvTerrainPage::CreateLevel2LodLightingPass(Ogre::Technique* pkTech)
{
	Pass* pkLod = pkTech->createPass();
	pkLod->setName("Lod");
	pkLod->setVertexProgram("FvTerrain_level_2_lod_lighting_VP");
	pkLod->setFragmentProgram("FvTerrain_level_2_lod_lighting_FP");
	pkLod->createTextureUnitState();
	pkLod->createTextureUnitState();
	pkLod->createTextureUnitState();
	Ogre::TextureUnitState* pkTex;
	pkTex = pkLod->getTextureUnitState(0);
	FV_ASSERT(pkTex);
	pkTex->setName("LodMap");
	pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	pkTex = pkLod->getTextureUnitState(1);
	FV_ASSERT(pkTex);
	pkTex->setName("NormalMap");
	pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	pkTex = pkLod->getTextureUnitState(2);
	FV_ASSERT(pkTex);
	pkTex->setName("HorizonShadowMap");
	pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

	return pkLod;
}

Ogre::Pass* FvTerrainPage::CreateLevel2LodPass(Ogre::Technique* pkTech)
{
	Pass* pkLod = pkTech->createPass();
	pkLod->setName("Lod");
	pkLod->setVertexProgram("FvTerrain_level_2_lod_VP");
	pkLod->setFragmentProgram("FvTerrain_level_2_lod_FP");
	pkLod->createTextureUnitState();
	pkLod->createTextureUnitState();
	pkLod->createTextureUnitState();
	Ogre::TextureUnitState* pkTex;
	pkTex = pkLod->getTextureUnitState(0);
	FV_ASSERT(pkTex);
	pkTex->setName("LodMap");
	pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	pkTex = pkLod->getTextureUnitState(1);
	FV_ASSERT(pkTex);
	pkTex->setName("NormalMap");
	pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	pkTex = pkLod->getTextureUnitState(2);
	FV_ASSERT(pkTex);
	pkTex->setName("HorizonShadowMap");
	pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

	return pkLod;
}

Ogre::Technique* FvTerrainPage::GenerateLevel0(FvUInt32 u32Number,
	const Ogre::MaterialPtr& spMaterial, bool bHole)
{
	FV_ASSERT(!spMaterial.isNull());

	Technique* pkLevel0 = spMaterial->createTechnique();
	pkLevel0->setName("level_0");
	pkLevel0->setSchemeName("level_0");

	if(bHole)
	{
		CreateHolePass(pkLevel0);
	}

	switch(u32Number)
	{
	case MATERIAL_BLEND:
		{
			Pass* pkBlend = CreateLevel0BlendPass(pkLevel0, m_kCombinedLayers.size() - 1);
			if(bHole)
			{
				pkBlend->setDepthFunction(Ogre::CMPF_EQUAL);
				pkBlend->setDepthWriteEnabled(false);
			}

			for(FvInt32 i(((FvInt32)m_kCombinedLayers.size()) - 2); i >= 0; --i)
			{
				pkBlend = CreateLevel0BlendPass(pkLevel0, i);
				pkBlend->setSceneBlending(Ogre::SBT_ADD);
				pkBlend->setDepthWriteEnabled(false);
				pkBlend->setDepthFunction(Ogre::CMPF_EQUAL);
				pkBlend->setColourWriteEnabled((unsigned int)0xffffff);
			}
		}
		break;
	case MATERIAL_INTERIM:
		{
			Pass* pkLod = CreateLevel0LodPass(pkLevel0);
			if(bHole)
			{
				pkLod->setDepthFunction(Ogre::CMPF_EQUAL);
				pkLod->setDepthWriteEnabled(false);
			}

			for(FvUInt32 i(0); i < m_kCombinedLayers.size(); ++i)
			{
				Pass* pkBlend = CreateLevel0BlendPass(pkLevel0, i);
				pkBlend->setSceneBlending(Ogre::SBT_ADD);
				pkBlend->setDepthWriteEnabled(false);
				pkBlend->setDepthFunction(Ogre::CMPF_EQUAL);
				if(i > 0)
				{
					pkBlend->setColourWriteEnabled((unsigned int)0xffffff);
				}
			}
		}
		break;
	case MATERIAL_LOD:
		{
			Pass* pkLod = CreateLevel0LodPass(pkLevel0);
			if(bHole)
			{
				pkLod->setDepthFunction(Ogre::CMPF_EQUAL);
				pkLod->setDepthWriteEnabled(false);
			}
		}

		break;
	}

	return pkLevel0;
}

Ogre::Pass* FvTerrainPage::CreateLevel0BlendPass(Ogre::Technique* pkTech, FvUInt32 u32CombinedLayer)
{
	char acBuffer[64];
	Pass* pkBlend = pkTech->createPass();
	
	sprintf_s(acBuffer, "Blend%d", u32CombinedLayer);
	pkBlend->setName(acBuffer);

	FvUInt32 u32CurrentLayerNumber = m_kCombinedLayers[u32CombinedLayer].m_kTextureLayers.size();
	FV_ASSERT(u32CurrentLayerNumber <= 4);

	sprintf_s(acBuffer, "FvTerrain_level_0_blend_%d_VP", u32CurrentLayerNumber);
	pkBlend->setVertexProgram(acBuffer);

	sprintf_s(acBuffer, "FvTerrain_level_0_blend_%d_FP", u32CurrentLayerNumber);
	pkBlend->setFragmentProgram(acBuffer);

	Ogre::TextureUnitState* pkTex = pkBlend->createTextureUnitState();
	FV_ASSERT(pkTex);
	pkTex->setName("BlendMap");
	pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

	for(FvUInt32 i(0); i < u32CurrentLayerNumber; ++i)
	{
		pkTex = pkBlend->createTextureUnitState();
		FV_ASSERT(pkTex);
		sprintf_s(acBuffer, "LayerMap%d", i);
		pkTex->setName(acBuffer);
		pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
		pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
	}

	pkTex = pkBlend->createTextureUnitState();
	FV_ASSERT(pkTex);
	pkTex->setName("HorizonShadowMap");
	pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

	return pkBlend;
}

Ogre::Pass* FvTerrainPage::CreateLevel0LodPass(Ogre::Technique* pkTech)
{
	Pass* pkLod = pkTech->createPass();
	pkLod->setName("Lod");
	pkLod->setVertexProgram("FvTerrain_level_0_lod_VP");
	pkLod->setFragmentProgram("FvTerrain_level_0_lod_FP");
	pkLod->createTextureUnitState();
	pkLod->createTextureUnitState();
	Ogre::TextureUnitState* pkTex;
	pkTex = pkLod->getTextureUnitState(0);
	FV_ASSERT(pkTex);
	pkTex->setName("LodMap");
	pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	pkTex = pkLod->getTextureUnitState(1);
	FV_ASSERT(pkTex);
	pkTex->setName("HorizonShadowMap");
	pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

	return pkLod;
}

Ogre::Technique* FvTerrainPage::GenerateLevelLow(
	const Ogre::MaterialPtr& spMaterial, bool bHole)
{
	FV_ASSERT(!spMaterial.isNull());

	Technique* pkLevelLow = spMaterial->createTechnique();
	pkLevelLow->setName("level_low");
	pkLevelLow->setSchemeName("level_low");

	if(bHole)
	{
		CreateHolePass(pkLevelLow);
	}
	
	Pass* pkLow = pkLevelLow->createPass();
	pkLow->setName("Low");
	pkLow->setVertexProgram("FvTerrain_level_low_VP");
	pkLow->setFragmentProgram("FvTerrain_level_low_FP");
	Ogre::TextureUnitState* pkTex = pkLow->createTextureUnitState();
	pkTex->setName("LodMap");
	pkTex->setTextureFiltering(Ogre::TFO_TRILINEAR);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

	if(bHole)
	{
		pkLow->setDepthFunction(Ogre::CMPF_EQUAL);
		pkLow->setDepthWriteEnabled(false);
	}

	return pkLevelLow;
}

Ogre::Pass* FvTerrainPage::CreateHolePass(Ogre::Technique* pkTech)
{
	Pass* pkHole = pkTech->createPass();
	pkHole->setName("Hole");
	pkHole->setVertexProgram("FvTerrain_hole_VP");
	pkHole->setFragmentProgram("FvTerrain_hole_FP");
	pkHole->setAlphaRejectFunction(Ogre::CMPF_LESS);
	pkHole->setAlphaRejectValue(128);
	pkHole->setColourWriteEnabled(false);
	Ogre::TextureUnitState* pkTex = pkHole->createTextureUnitState();
	pkTex->setName("HoleMap");
	pkTex->setTextureFiltering(Ogre::TFO_NONE);
	pkTex->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

	return pkHole;
}

#endif // !FV_SERVER

namespace
{
	const float EXTEND_BIAS = 0.001f;

	void Extend( FvWorldTriangle &kWT )
	{
		FvVector3 kA = kWT.Point1() - kWT.Point0();
		FvVector3 kB = kWT.Point2() - kWT.Point1();
		FvVector3 kC = kWT.Point0() - kWT.Point2();
		kA *= EXTEND_BIAS;
		kB *= EXTEND_BIAS;
		kC *= EXTEND_BIAS;

		kWT = FvWorldTriangle( kWT.Point0() - kA + kC,
			kWT.Point1() - kB + kA,
			kWT.Point2() - kC + kB, FV_TRIANGLE_TERRAIN );
	}
}

bool FvTerrainPage::CheckGrid(int iGridX, int iGridY, FvVector3 const &kStart, 
			   FvVector3 const &kEnd, FvTerrainCollisionCallback *pkCallback) const
{
	bool bRes = false;

	if (iGridX >= 0 && iGridX < (int)VerticesWidth() &&
		iGridY >= 0 && iGridY < (int)VerticesHeight() )
	{
		FvVector3 kBottomLeft( float(iGridX) * SpacingX(),
			float(iGridY) * SpacingY(),
			HeightAt(iGridX, iGridY));

		FvVector3 kBottomRight = kBottomLeft;
		kBottomRight.x += SpacingX();
		kBottomRight.z = HeightAt(iGridX + 1, iGridY);

		FvVector3 kTopLeft = kBottomLeft;
		kTopLeft.y += SpacingY();
		kTopLeft.z =  HeightAt(iGridX, iGridY + 1);

		FvVector3 kTopRight = kTopLeft;
		kTopRight.x += SpacingX();
		kTopRight.z =  HeightAt(iGridX + 1, iGridY + 1);

		FvWorldTriangle kTriA;
		FvWorldTriangle kTriB;

		if ((iGridX ^ iGridY) & 1)
		{
			kTriA = FvWorldTriangle( kBottomRight, kTopLeft, kBottomLeft, 
				FV_TRIANGLE_TERRAIN );
			kTriB = FvWorldTriangle( kBottomRight, kTopRight, kTopLeft,
				FV_TRIANGLE_TERRAIN );
		}
		else
		{
			kTriA = FvWorldTriangle( kTopRight, kTopLeft, kBottomLeft, 
				FV_TRIANGLE_TERRAIN );
			kTriB = FvWorldTriangle( kBottomLeft, kBottomRight, kTopRight,
				FV_TRIANGLE_TERRAIN );
		}

		Extend( kTriA );
		Extend( kTriB );

		FvVector3 kDir = kEnd - kStart;
		float fDist = kDir.Length();
		float fDist2 = fDist;
		kDir *= 1.f / fDist;

		bool bIntersectsA = kTriA.Intersects( kStart, kDir, fDist );
		bool bIntersectsB = kTriB.Intersects( kStart, kDir, fDist2 );

		if (pkCallback)
		{
			if (bIntersectsA && bIntersectsB)
			{
				if (fDist < fDist2)
				{
					bRes = pkCallback->Collide( kTriA, fDist );
					if (!bRes)
					{
						bRes = pkCallback->Collide( kTriB, fDist2 );
					}
				}
				else
				{
					bRes = pkCallback->Collide( kTriB, fDist2 );
					if (!bRes)
					{
						bRes = pkCallback->Collide( kTriA, fDist );
					}
				}
			}
			else if (bIntersectsA)
			{
				bRes = pkCallback->Collide( kTriA, fDist );
			}
			else if (bIntersectsB)
			{
				bRes = pkCallback->Collide( kTriB, fDist2 );
			}
		}
		else
		{
			bRes = bIntersectsA || bIntersectsB;
		}
	}

	return bRes;
}

bool FvTerrainPage::CheckGrid(int iGridX, int iGridY, FvWorldTriangle const &kStart, 
			   FvVector3 const &kEnd, FvTerrainCollisionCallback *pkCallback) const
{
	bool bRes = false;

	if (iGridX >= 0 && iGridX < (int)VerticesWidth() &&
		iGridY >= 0 && iGridY < (int)VerticesHeight() )
	{
		FvVector3 kBottomLeft( float(iGridX) * SpacingX(),
			float(iGridY) * SpacingY(),
			HeightAt(iGridX, iGridY));

		FvVector3 kBottomRight = kBottomLeft;
		kBottomRight.x += SpacingX();
		kBottomRight.z = HeightAt(iGridX + 1, iGridY);

		FvVector3 kTopLeft = kBottomLeft;
		kTopLeft.y += SpacingY();
		kTopLeft.z =  HeightAt(iGridX, iGridY + 1);

		FvVector3 kTopRight = kTopLeft;
		kTopRight.x += SpacingX();
		kTopRight.z =  HeightAt(iGridX + 1, iGridY + 1);

		FvWorldTriangle kTriA;
		FvWorldTriangle kTriB;

		if ((iGridX ^ iGridY) & 1)
		{
			kTriA = FvWorldTriangle( kBottomRight, kTopLeft,
				kBottomLeft, FV_TRIANGLE_TERRAIN );
			kTriB = FvWorldTriangle( kBottomRight, kTopRight,
				kTopLeft, FV_TRIANGLE_TERRAIN );
		}
		else
		{
			kTriA = FvWorldTriangle( kTopRight, kTopLeft,
				kBottomLeft, FV_TRIANGLE_TERRAIN );
			kTriB = FvWorldTriangle( kBottomLeft, kBottomRight,
				kTopRight, FV_TRIANGLE_TERRAIN );
		}

		FvVector3 kOffset = kEnd - kStart.Point0();

		bool bIntersectsA = kTriA.Intersects( kStart, kOffset );
		bool bIntersectsB = kTriB.Intersects( kStart, kOffset );

		float fDist = 0.f;

		if (pkCallback)
		{
			if (bIntersectsA && bIntersectsB)
			{
				bRes = pkCallback->Collide( kTriA, fDist );
				if (!bRes)
				{
					bRes = pkCallback->Collide( kTriB, fDist );
				}
			}
			else if (bIntersectsA)
			{
				bRes = pkCallback->Collide( kTriA, fDist );
			}
			else if (bIntersectsB)
			{
				bRes = pkCallback->Collide( kTriB, fDist );
			}
		}
		else
		{
			bRes = bIntersectsA || bIntersectsB;
		}
	}

	return bRes;
}
