#include "App.h"


class FvServerResource;
class FvNetNub;
struct MessageFile;
class FvEntityManager;


class MyApp : public App
{
public:
	MyApp();
	virtual~MyApp();

	virtual	bool	Init(int argc, char* argv[]);
	virtual	void	Tick();
	virtual	void	Draw();

	virtual	void	OnKeyDown(int keyVal, bool bRep);
	virtual	void	OnKeyUp(int keyVal);

protected:
	FvServerResource*	pkServerResource;
	FvNetNub*			pkNub;
	MessageFile*		pkMsgFile;
	FvEntityManager*	pkMgr;

	bool			m_bSetTitle;
	int				m_iSpaceIdx;
	bool			m_bResetWindow4Space;
	bool			m_bShowAoi;
	bool			m_bShowLod;
	bool			m_bShowInnerCellBorder;
	bool			m_bShowEntityProp;
	bool			m_bShowEntityID;
	bool			m_bShowEntityPos;
	bool			m_bShowTraps;
};


App* CreateApp();
void DestroyApp();