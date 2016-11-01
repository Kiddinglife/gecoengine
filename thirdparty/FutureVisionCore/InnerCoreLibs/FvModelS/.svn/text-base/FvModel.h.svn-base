//{future header message}
#ifndef __FvModel_H__
#define __FvModel_H__

#include <FvBaseTypes.h>
#include <FvSmartPointer.h>
#include <FvBoundingBox.h>
#include <FvStringMap.h>
#include <FvBSP.h>

#include "FvInstanceEntity.h"
#include "FvModelDefines.h"
#include "FvBSPProxy.h"

namespace Ogre
{
	class SceneNode;
	class SceneManager;
	class AnimationState;
};

class FvMatrix;

using namespace Ogre;

class FV_MODEL_API FvModel : public FvSafeReferenceCount
{
public:
	FvModel(const FvString& kZoneName);
	FvModel();
	~FvModel();

	bool Load(const	FvString &kFileName, const FvString &kGroupName,
		const FvString& kExtraName, const FvMatrix& kMatrix,
		bool bCastShadows = false);

	FvBoundingBox const &BoundingBox() const;
	
	const FvBSPTree *GetBSPTree() const;

	const FvString GetFileName() const;

#ifndef FV_SERVER
	struct Node
	{
		Node() : m_pkNode(NULL) {}
		SceneNode* m_pkNode;
		FvRefList<FvInstanceEntity*, FvRefNode1> m_kEntityList;
	};

	SceneNode *GetNode();

	const Node& GetModleNode();

	FvUInt32 GetCurrentLodLevel();

	void SetCurrentLodLevel(FvUInt32 u32CurrentLodLevel);

	AnimationState *GetAnimation(const FvString &kName);

	void Transform(const FvMatrix &kMatrix);

	float GetVolume();

private:
	const FvString m_kZoneName;

	FvString m_kNodeName;

	static FvInt32 ms_uiModelIdentify;

	bool m_bCastShadows;

	Node m_akNodes[FV_MAX_LOD_LEVEL];

	FvUInt32 m_u32CurrentLodLevel;

	//SceneNode *m_pkNode;

	typedef FvStringHashMap<AnimationState*> ModelAnimations;
	ModelAnimations m_kModelAnimations;
#else // !FV_SERVER
private:
#endif // FV_SERVER

	FvBSPProxyPtr m_spBSPTree;

	FvBoundingBox m_kBoudingBox;

	FvString m_kFileName;

	FvString m_kGroupName;

	FvString m_kExtaName;

	float m_fVolume;

	friend class FvXMLModelSerializerImpl;
};

typedef FvSmartPointer<FvModel> FvModelPtr;

#include "FvModel.inl"

#endif // __FvModel_H__