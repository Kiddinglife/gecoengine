#ifndef __FVANIMATIONMODEL_H__
#define __FVANIMATIONMODEL_H__


#include <FvBaseTypes.h>
#include <FvSmartPointer.h>
#include <FvBoundingBox.h>
#include <FvStringMap.h>

#include "FvModelDefines.h"
#include "FvBSPProxy.h"
#include "FvAnimatedCharacter.h"

namespace Ogre
{
	class SceneNode;
	class SceneManager;
	class AnimationState;
	class Entity;
};
using namespace Ogre;
class FV_MODEL_API FvAnimationModel : public FvSafeReferenceCount
{
public:
	struct Node
	{
		Node() : m_pkNode(NULL)	{ m_kEntityList.reserve(10); }
		SceneNode* m_pkNode;
		std::vector<Entity*> m_kEntityList;
	};

	FvAnimationModel(const FvString& kZoneName);
	~FvAnimationModel();

	bool Load(const FvString& kFileName, const FvString& kGroupName,
		const FvString& kExtraName, const FvMatrix& kMatrix, bool bCastShadows = false);

	FvBoundingBox const &BoundingBox() const;
	SceneNode* GetNode();
	const Node& GetModelNode();
	FvUInt32 GetCurrentLodLevel();
	void DestroyModelNode(Ogre::SceneNode *pkNode);

	FvString GetDefaultAnimName();
	AnimationState* GetDefaultAnim();

	const FvBSPTree*	GetBSPTree()   const;

	const FvAnimationModel::Node& GetAnimationModelNode();

	const FvString GetFileName() const;

	void Transform(const FvMatrix &kMatrix);

	void SetCurrentLodLevel(FvUInt32 u32CurrentLodLevel);

	AnimationState* GetAnimation(const FvString& kName);

	float GetVolume();

	void SetRenderState(FvAnimatedCharacterNodePrivateData::RenderState eState);
	FvAnimatedCharacterNodePrivateData::RenderState GetRenderState(void);


	const FvString m_kZoneName;
	FvString m_kNodeName;
	static FvInt32 ms_uiAnimationModelIdentify;
	bool m_bCastShadows;
	Node m_akNodes[FV_MAX_LOD_LEVEL];
	FvUInt32 m_u32CurrentLodLevel;

	typedef FvStringHashMap<AnimationState*>  ModelAnimations;
	ModelAnimations m_kModelAnimations;
	FvString	  m_sDefaultAnimName;

	FvBSPProxyPtr m_spBSPTree;
	FvBoundingBox m_kBoundingBox;
	FvString	  m_kFileName;
	FvString      m_kGroupName;
	FvString      m_kExtraName;
	float		  m_fVolume;

	FvAnimatedCharacterNodePrivateData::RenderState m_eState;
	friend	class FvXMLGameObjectModelSerializerImpl;
};
typedef FvSmartPointer<FvAnimationModel> FvAnimationModelPtr;

#include "FvAnimationModel.inl"
#endif //__FVANIMATIONMODEL_H__