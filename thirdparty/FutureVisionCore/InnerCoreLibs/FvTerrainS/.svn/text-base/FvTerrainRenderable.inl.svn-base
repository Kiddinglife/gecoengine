
FV_INLINE
FvBoundingBox const &FvTerrainRenderable::BoundingBox() const
{
	return m_kBoundingBox;
}

FV_INLINE
FvVector3 const &FvTerrainRenderable::Centre() const
{
	return m_kCentre;
}

FV_INLINE
float FvTerrainRenderable::BoundingRadius() const
{
	return m_fBoundingRadius;
}

FV_INLINE
void FvTerrainRenderable::SetLightMask(FvUInt32 uiLightMask)
{
	m_uiLightMask = uiLightMask;
}

FV_INLINE
FvUInt32 FvTerrainRenderable::GetLightMask()
{
	return m_uiLightMask;
}

FV_INLINE
const MaterialPtr &FvTerrainRenderable::getMaterial(void) const
{
	return m_kTerrainPage.GetMaterial(m_eState);
}

FV_INLINE
Technique *FvTerrainRenderable::getTechnique(void) const
{
	return m_kTerrainPage.GetTechnique(m_eState);
}

FV_INLINE
bool FvTerrainRenderable::getCastsShadows(void) const
{
	return false;
}

FV_INLINE
void FvTerrainRenderable::SetNeighbor( Neighbor eNeighbor, FvTerrainRenderable *pkTitle )
{
	m_pkNeighbors[eNeighbor] = pkTitle;
}

FV_INLINE
FvTerrainRenderable *FvTerrainRenderable::GetNeighbor( Neighbor eNeighbor )
{
	return m_pkNeighbors[eNeighbor];
}

FV_INLINE
FvUInt32 FvTerrainRenderable::GetLodLevel()
{
	return m_uiLodLevel;
}

FV_INLINE
Ogre::Renderable *FvTerrainRenderable::GetWireBB()
{
	return &m_kWireBB;
}

//FV_INLINE
//FvUInt16 FvTerrainRenderable::MakeIndex( FvUInt32 uiX, FvUInt32 uiY ) const
//{
//	return ( uiX + uiY * ((m_kTerrainPage.PageWidth()/m_kTerrainPage.NumTitles()) + 1));
//}
