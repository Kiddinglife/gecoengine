//{future header message}
#ifndef __FvGameObjectModel_H__
#define __FvGameObjectModel_H__

#include <FvBaseTypes.h>
#include <FvSmartPointer.h>
#include <FvBoundingBox.h>
#include <FvStringMap.h>
#include <FvBSP.h>

#include "FvModelDefines.h"
#include "FvBSPProxy.h"

#include "FvInstanceEntity.h"
#include "FvAnimatedCharacter.h"

#include <vector>

namespace Ogre
{
	class SceneNode;
	class SceneManager;
	class AnimationState;
	class Entity;
};

class FvMatrix;

using namespace Ogre;

class FV_MODEL_API FvGameObjectModel : public FvSafeReferenceCount
{
public:
	FvGameObjectModel();
	~FvGameObjectModel();

	bool Load(const	FvString &kFileName, const FvString &kGroupName,
		const FvString& kExtraName, const FvMatrix& kMatrix,
		bool bCastShadows = false);

	FvBoundingBox const &BoundingBox() const;

	const FvBSPTree *GetBSPTree() const;

	const FvString GetFileName() const;

#ifndef FV_SERVER
	struct Node
	{
		Node() : m_pkNode(NULL) { m_kEntityList.reserve(10); }
		SceneNode* m_pkNode;
		std::vector<Entity*> m_kEntityList;
	};

	SceneNode *GetNode();

	const Node& GetModleNode();

	FvUInt32 GetCurrentLodLevel();

	void SetCurrentLodLevel(FvUInt32 u32CurrentLodLevel);

	AnimationState *GetAnimation(const FvString &kName);

	void Transform(const FvMatrix &kMatrix);

	float GetVolume();

	void SetRenderState(FvAnimatedCharacterNodePrivateData::RenderState eState);

	FvAnimatedCharacterNodePrivateData::RenderState GetRenderState();

	void SetAddColor(float r, float g, float b);

	void SetMulColor(float r, float g, float b);

	void SetBlendAlpha(float a, float x = 0.1f, float y = 0.1f, float fRefraction = 0.7f);

	void SetFadeOutAlpha(float a);

	void SetEdgeHighLight(float r, float g, float b, float a);

	void SetEdgeHighLightWidth(float fWidth);

	void UpdateRenderQueue(Ogre::RenderQueue* pkQueue, const Ogre::Vector4& kID);

private:
	const FvString m_kZoneName;

	FvString m_kNodeName;

	static FvInt32 ms_uiGameObjectModelIdentify;

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

	FvAnimatedCharacterNodePrivateData::RenderState m_eState;

	friend class FvXMLGameObjectModelSerializerImpl;
};

typedef FvSmartPointer<FvGameObjectModel> FvGameObjectModelPtr;

#include "FvGameObjectModel.inl"

#endif // __FvGameObjectModel_H__
