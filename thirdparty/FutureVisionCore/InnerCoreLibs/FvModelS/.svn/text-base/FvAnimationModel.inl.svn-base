FV_INLINE
FvBoundingBox const &FvAnimationModel::BoundingBox() const
{
	return m_kBoundingBox;
}

FV_INLINE
SceneNode* FvAnimationModel::GetNode()
{
	return m_akNodes[m_u32CurrentLodLevel].m_pkNode;
}

FV_INLINE
AnimationState* FvAnimationModel::GetAnimation(const FvString& kName)
{
	ModelAnimations::iterator kFind = m_kModelAnimations.find(kName);
	if (kFind == m_kModelAnimations.end())
		return NULL;
	return kFind->second;
}

FV_INLINE
const FvBSPTree* FvAnimationModel::GetBSPTree() const
{
	if (m_spBSPTree.getPointer())
	{
		return (FvBSPTree*)(*m_spBSPTree);
	}
	return NULL;
}

FV_INLINE
const FvString FvAnimationModel::GetFileName() const
{
	return m_kFileName;
}

FV_INLINE
const FvAnimationModel::Node& FvAnimationModel::GetAnimationModelNode()
{
	return m_akNodes[m_u32CurrentLodLevel];
}