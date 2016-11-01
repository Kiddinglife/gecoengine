#include "App.h"


struct MessageFile;


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
	virtual	void	OnCmd(const char* pcCmd);

protected:
	void			Clear();
	void			DrawGlobalMode();
	void			DrawSingleMode();
	void			DrawHelp();
	void			DrawConfig();

protected:
	MessageFile*	m_pkMsgFile;

	int				m_iSpaceIdx;
	bool			m_bResetWindow4Space;
	bool			m_bShowEntityProp;
	bool			m_bShowEntityID;
	bool			m_bShowEntityPos;
	bool			m_bShowRobotID;
	int				m_iResetWindowSize;
	int				m_iShowMode;
	int				m_iInfoPage4Help;
	int				m_iInfoPage4Config;
};


App* CreateApp();
void DestroyApp();