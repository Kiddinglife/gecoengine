#ifndef __AvatarAssisstant_H__
#define __AvatarAssisstant_H__

#include "VisualStruct.h"
#include "FvCliEntity.h"
#include "GxVisualAvatar.h"


class ClientAvatarNode : public GxAvatarNode
{
public:
	ClientAvatarNode();
	virtual ~ClientAvatarNode();

	static FvUInt64 GenKey();

	void Load(const VisualItem& kInfo, FvAnimatedCharacter& kAvatar, const FvUInt32 uiGender, const _LinkState uiState = LINK_1);
	void Load(const VisualBody& kInfo, FvAnimatedCharacter& kAvatar, const _LinkState uiState = LINK_1);
	void Load(FvAnimatedCharacter& kAvatar, const char* pcName, const char* pcMesh, const char* pcMaterial);

	static const char* ClientAvatarNode::GetResName(const char* pcRes, const FvUInt32 uiGender);

	const VisualItem* Info()const{return m_rpInfo;}
	const VisualBody* Body()const{return m_rpBody;}

	std::vector<FvString> m_kNames;
	std::vector<FvString> m_kMeshs;
	std::vector<FvString> m_kMaterials;

	virtual void SetLinkState(const _LinkState uiState);
	void StartRibbonTrail(FvAvatarAttachment& kAvatarAttachment, FvAnimatedCharacter& kAvatar);
	void EndRibbonTrail();
	void InitRibbonTrail();

	virtual bool LoadFinishAll();

	bool TEST_FLAG;
protected:

	void _OnRibbonTrailTick(const float fDeltaTime);
	FvTickNode<ClientAvatarNode, &ClientAvatarNode::_OnRibbonTrailTick> m_kTickNode;
	

	FvRibbonTrail* m_opRibbonTrail;

	const VisualItem* m_rpInfo;
	const VisualBody* m_rpBody;

};
class GameUnitEntity;
class AvatarFinishLoadCallBack : public FvAvatarFinishLoadCallBack
{
public:
	AvatarFinishLoadCallBack()
	{}
	void SetListener(GameUnitEntity* pkListener){m_rpListener=pkListener;}
	virtual void	CallBack();

protected:
	FvReferencePtr<GameUnitEntity>	m_rpListener;
};

class ClientGameUnitAvatar: public GxVisualAvatar
{
public:

	ClientGameUnitAvatar();
	~ClientGameUnitAvatar();

	void SetBody(const FvUInt32 uiBodyID, const FvUInt32 uiGender);
	void SetBody(const FvUInt32 uiBodyID);
	void AddBody(const FvUInt32 uiBodyID);
	void AddItem(const FvUInt32 uiBodyID, const FvUInt32 uiGender);
	void Clear();

	void StartRibbonTail();
	void EndRibbonTail();

	void DisplayEquipOnItem(const FvUInt32 uiSlot, const VisualItem& kInfo, const FvUInt32 uiGender);
	void DisplayEquipOffItem(const FvUInt32 uiSlot);

	virtual void SetLinkState(const GxAvatarNode::_LinkState eState);

	void SetPartSize(const FvUInt32 uiSize);
	std::vector<ClientAvatarNode*>& AllParts(){return m_PartList;}
	std::list<ClientAvatarNode*>& AllBodys() {return m_BodyVisualList;}
	void ClearAllParts();

	ClientAvatarNode* Body()const{return m_opDefaultBody;}

	void OnRotateStart(const FvInt32 iSign);
	void OnRotateEnd(const FvInt32 iSign);
	void OnRotateUpdated();
	//typedef FvTRotateCursor<ClientGameUnitAvatar, &OnRotateStart, &OnRotateEnd, &OnRotateUpdated> PlayerRotateCursor;
	//PlayerRotateCursor& RotateCursor(){return m_kPlayerRotateCursor;}

	//WeaponAnimGroup& AnimDict(){ return m_kAnimDict;}
	
private:

	ClientAvatarNode* _DisplayItemVisual(FvAnimatedCharacter& kAvatar, const VisualItem& kInfo, const FvUInt32 uiGender);
	ClientAvatarNode* _DisplayBodyVisual(FvAnimatedCharacter& kAvatar, const VisualBody& kInfo);
	
	ClientAvatarNode* CreatePartNode(const FvUInt32 uiPart);

	std::vector<ClientAvatarNode*> m_PartList;
	std::list<ClientAvatarNode*> m_BodyVisualList;
	ClientAvatarNode* m_opDefaultBody;
	ClientAvatarNode::_LinkState m_eLinkState;

	//PlayerRotateCursor m_kPlayerRotateCursor;

	//WeaponAnimGroup m_kAnimDict;
	//

};

#endif //__AvatarAssisstant_H__
