
FV_INLINE
FvBoundingBox const &FvModel::BoundingBox() const
{
	return m_kBoudingBox;
}
#ifndef FV_SERVER
FV_INLINE
SceneNode *FvModel::GetNode()
{
	return m_akNodes[m_u32CurrentLodLevel].m_pkNode;
}

FV_INLINE
AnimationState *FvModel::GetAnimation(const FvString &kName)
{
	ModelAnimations::iterator kFind = m_kModelAnimations.find(kName);
	if(kFind == m_kModelAnimations.end())
		return NULL;
	return kFind->second;
}
#endif // !FV_SERVER
FV_INLINE
const FvBSPTree *FvModel::GetBSPTree() const
{
	if(m_spBSPTree.getPointer())
	{
		return (FvBSPTree*)(*m_spBSPTree);
	}
	return NULL;	
}

FV_INLINE
const FvString FvModel::GetFileName() const
{
	return m_kFileName;
}