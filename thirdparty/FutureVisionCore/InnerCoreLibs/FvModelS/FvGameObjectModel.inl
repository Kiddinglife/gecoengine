
FV_INLINE
FvBoundingBox const &FvGameObjectModel::BoundingBox() const
{
	return m_kBoudingBox;
}
#ifndef FV_SERVER
FV_INLINE
SceneNode *FvGameObjectModel::GetNode()
{
	return m_akNodes[m_u32CurrentLodLevel].m_pkNode;
}

FV_INLINE
AnimationState *FvGameObjectModel::GetAnimation(const FvString &kName)
{
	ModelAnimations::iterator kFind = m_kModelAnimations.find(kName);
	if(kFind == m_kModelAnimations.end())
		return NULL;
	return kFind->second;
}
#endif // !FV_SERVER
FV_INLINE
const FvBSPTree *FvGameObjectModel::GetBSPTree() const
{
	if(m_spBSPTree.getPointer())
	{
		return (FvBSPTree*)(*m_spBSPTree);
	}
	return NULL;	
}

FV_INLINE
const FvString FvGameObjectModel::GetFileName() const
{
	return m_kFileName;
}