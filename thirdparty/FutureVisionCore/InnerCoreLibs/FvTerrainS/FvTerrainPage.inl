
FV_INLINE
FvBoundingBox const &FvTerrainPage::BoundingBox() const
{
	return m_kBoundingBox;
}

FV_INLINE
FvVector3 const &FvTerrainPage::WorldPosition() const
{
	return m_kWorldPosition;
}

FV_INLINE
float FvTerrainPage::SpacingX() const
{
	return FV_BLOCK_SIZE_METERS / this->PageWidth();
}

FV_INLINE
float FvTerrainPage::SpacingY() const
{
	return FV_BLOCK_SIZE_METERS / this->PageHeight();
}

FV_INLINE
FvUInt32 FvTerrainPage::PageWidth() const
{
	return FvUInt32(m_kHeights.getWidth()) - m_uiVisiableOffset * 2 - 1;
}

FV_INLINE
FvUInt32 FvTerrainPage::PageHeight() const
{
	return FvUInt32(m_kHeights.getHeight()) - m_uiVisiableOffset * 2 - 1;
}

FV_INLINE
FvUInt32 FvTerrainPage::VerticesWidth() const
{
	return FvUInt32(m_kHeights.getWidth()) - m_uiVisiableOffset * 2;
}

FV_INLINE
FvUInt32 FvTerrainPage::VerticesHeight() const
{
	return FvUInt32(m_kHeights.getHeight()) - m_uiVisiableOffset * 2;
}

FV_INLINE
float FvTerrainPage::MinHeight() const
{
	return m_fMinHeight;
}

FV_INLINE
float FvTerrainPage::MaxHeight() const
{
	return m_fMaxHeight;
}

FV_INLINE
float FvTerrainPage::HeightAt(int iX, int iY) const
{
	return GetHeightValue(iX + m_uiVisiableOffset, iY + m_uiVisiableOffset);
}

FV_INLINE
bool FvTerrainPage::NoHoles() const
{
	return m_bNoHoles;
}

FV_INLINE
bool FvTerrainPage::AllHoles() const
{
	return m_bAllHoles;
}

FV_INLINE
FvUInt32 FvTerrainPage::HolesMapWidth() const
{
	return m_uiHolesMapWidth;
}

FV_INLINE
FvUInt32 FvTerrainPage::HolesMapHeight() const
{
	return m_uiHolesMapHeight;
}

FV_INLINE
FvUInt32 FvTerrainPage::HolesMapSize() const
{
	return m_uiHolesMapSize;
}

FV_INLINE
FvUInt32 FvTerrainPage::HolesSize() const
{
	return m_uiHolesSize;
}

FV_INLINE
FvUInt32 FvTerrainPage::MaterialWidth() const
{
	return m_uiMaterialWidth;
}

FV_INLINE
FvUInt32 FvTerrainPage::MaterialHeight() const
{
	return m_uiMaterialHeight;
}

FV_INLINE
FvTerrainPage::OceanType FvTerrainPage::GetOceanType()
{
	return m_eOceanType;
}

FV_INLINE
const FvString &FvTerrainPage::ResourceName() const
{
	return m_kResourceName;
}

#ifndef FV_SERVER
FV_INLINE
Ogre::Renderable *FvTerrainPage::GetWireBB()
{
	return &m_kWireBB;
}

FV_INLINE
FvUInt32 FvTerrainPage::NumTitles() const
{
	return m_spTerrainSettings->NumTitles();
}

FV_INLINE
FvUInt32 FvTerrainPage::NumLods() const
{
	return m_spTerrainSettings->NumLods();
}

FV_INLINE
FvTerrainRenderable *FvTerrainPage::GetRenderable(FvUInt32 uiIndex)
{
	return (uiIndex < this->NumTitles()*this->NumTitles())?m_kTerrainRenderable[uiIndex]:NULL;
}

FV_INLINE
FvTerrainRenderable *FvTerrainPage::GetRenderable(FvUInt32 uiTitleX,FvUInt32 uiTitleY)
{
	return (uiTitleX < this->NumTitles() && 
		uiTitleY < this->NumTitles())?m_kTerrainRenderable[uiTitleY * this->NumTitles() + uiTitleX]:NULL;
}

FV_INLINE
bool FvTerrainPage::IsInLodControl()
{
	return m_bIsInLodControl;
}

FV_INLINE
void FvTerrainPage::SetInLodControl(bool bInLodControl)
{
	m_bIsInLodControl = bInLodControl;
}

FV_INLINE
Ogre::MaterialPtr &FvTerrainPage::GetMaterial(MaterialType eType)
{
	return m_aspMaterial[eType];
}

FV_INLINE
Ogre::Technique *FvTerrainPage::GetTechnique(MaterialType eType)
{
	return GetMaterial(eType)->getBestTechnique(0,NULL);
}

FV_INLINE
bool FvTerrainPage::GetTerrainVisible()
{
	return ms_bTerrainVisible;
}

FV_INLINE
void FvTerrainPage::SetTerrainVisible(bool bVisible)
{
	ms_bTerrainVisible = bVisible;
}

#endif // !FV_SERVER

FV_INLINE
FvTerrainSettingsPtr &FvTerrainPage::TerrainSettings()
{
	return m_spTerrainSettings;
}

FV_INLINE
float FvTerrainPage::GetHeightValue(int iX, int iY) const
{
	float *pkHeight = (float*)m_kHeights.getData();
	FV_ASSERT(pkHeight);
	iX = FvClampEx(0,iX,(int)m_kHeights.getWidth() - 1);
	iY = FvClampEx(0,iY,(int)m_kHeights.getHeight() - 1);
	return *(pkHeight + iY* m_kHeights.getWidth() + iX);
}

FV_INLINE
void FvTerrainPage::SetHeightValue(int iX, int iY, float fValue)
{	
	float *pkHeight = (float*)m_kHeights.getData();
	FV_ASSERT(pkHeight);
	if (0 <= iX && iX < (int)m_kHeights.getWidth() && 0 <= iY && iY < (int)m_kHeights.getHeight())
		*(pkHeight + iY* m_kHeights.getWidth() + iX) = fValue;
}

FV_INLINE
bool FvTerrainPage::GetHoleValue(int iX, int iY) const
{
	FV_ASSERT(m_pkHoles);	
	iX = FvClampEx(0,iX,(int)m_uiHolesMapWidth - 1);
	iY = FvClampEx(0,iY,(int)m_uiHolesMapHeight - 1);
	FvUInt32 uiHX = iX >> 3;
	FvUInt32 uiMX = (iX & 0x07);
	FvUInt32 uiStride = (m_uiHolesMapWidth >> 3) + (((m_uiHolesMapWidth % 8) != 0) ? 1 : 0);
	return (m_pkHoles[iY*uiStride + uiHX] & (1 << uiMX)) != 0;
}

FV_INLINE
void FvTerrainPage::SetHoleValue(int iX, int iY, bool bValue)
{
	FV_ASSERT(m_pkHoles);
	iX = FvClampEx(0,iX,(int)m_uiHolesMapWidth - 1);
	iY = FvClampEx(0,iY,(int)m_uiHolesMapHeight - 1);
	FvUInt32 uiHX = iX >> 3;
	FvUInt8 uiMX = (FvUInt8)(iX & 0x07);	
	FvUInt32 uiStride = (m_uiHolesMapWidth >> 3) + (((m_uiHolesMapWidth % 8) != 0) ? 1 : 0);
	if (bValue)
		m_pkHoles[iY*uiStride + uiHX] |= (1 << uiMX);
	else
		m_pkHoles[iY*uiStride + uiHX] &= ~(1 << uiMX);
}

FV_INLINE
FvUInt8 FvTerrainPage::MaterialIndex( float fX, float fY ) const
{
	int iX = int((fX/FV_BLOCK_SIZE_METERS) * (m_uiMaterialWidth - 1) + 0.5f);
	int iY = int((fY/FV_BLOCK_SIZE_METERS) * (m_uiMaterialHeight - 1) + 0.5f);

	iX = FvClampEx(0,iX,(int)m_uiMaterialWidth - 1);
	iY = FvClampEx(0,iY,(int)m_uiMaterialHeight - 1);
	return *(m_pkMaterialIndexs + iY*m_uiMaterialWidth + iX);
}

FV_INLINE
float *const FvTerrainPage::GetHeightRow(FvUInt32 uiY) const
{
	float *pkHeight = (float*)m_kHeights.getData();
	FV_ASSERT(pkHeight);
	return pkHeight + uiY* m_kHeights.getWidth();
}

FV_INLINE
float *FvTerrainPage::GetHeightRow(FvUInt32 uiY)
{
	float *pkHeight = (float*)m_kHeights.getData();
	FV_ASSERT(pkHeight);
	return pkHeight + uiY* m_kHeights.getWidth();
}