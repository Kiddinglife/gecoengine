//{future header message}
#ifndef __FvTree_H__
#define __FvTree_H__

#include <FvBaseTypes.h>
#include <FvSmartPointer.h>
#include <FvBoundingBox.h>
#include <FvStringMap.h>
#include <FvBSP.h>

#include "FvModelDefines.h"
#include "FvBSPProxy.h"

#include "FvInstanceEntity.h"

#include <vector>

namespace Ogre
{
	class SceneManager;
	class AnimationState;
};

class FvMatrix;

using namespace Ogre;

class FV_MODEL_API FvTree : public FvSafeReferenceCount
{
public:
	struct Node
	{
		Node() : m_pkNode(NULL) {}
		SceneNode* m_pkNode;
		FvRefList<FvInstanceEntity*, FvRefNode1> m_kEntityList;
	};

	FvTree(const FvString& kZoneName);
	~FvTree();

	bool Load(const	FvString &kFileName, const FvString &kGroupName,
		const FvString& kExtraName, const FvMatrix& kMatrix,
		bool bCastShadows = false);

	FvBoundingBox const &BoundingBox() const;

	const FvTree::Node& GetTreeNode();

	SceneNode *FvTree::GetNode();

	const FvBSPTree *GetBSPTree() const;

	const FvString GetFileName() const;

	void Transform(const FvMatrix &kMatrix);

	void SetCurrentLodLevel(FvUInt32 u32CurrentLodLevel);

	float GetVolume();

	FvUInt32 GetCurrentLodLevel();

private:

	const FvString m_kZoneName;

	FvBSPProxyPtr m_spBSPTree;

	FvBoundingBox m_kBoudingBox;

	FvString m_kFileName;

	FvString m_kGroupName;

	FvString m_kExtaName;

	float m_fVolume;

	static FvInt32 ms_uiTreeIdentify;

	bool m_bCastShadows;

	Node m_akNodes[FV_MAX_LOD_LEVEL];

	FvUInt32 m_u32CurrentLodLevel;

	//SceneNode *m_pkNode;
	//FvRefList<FvInstanceEntity*> m_kEntityList;

	friend class FvXMLTreeSerializerImpl;
};

typedef FvSmartPointer<FvTree> FvTreePtr;


#endif // __FvTree_H__
