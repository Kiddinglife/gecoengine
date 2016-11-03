#pragma once

#include <FvVector2.h>
#include <FvVector4.h>


class App;
extern App* CreateApp();
extern void DestroyApp();


class App
{
public:
	App() {}
	virtual~App() {}

	virtual	bool	Init(int argc, char* argv[]) { return true; }
	virtual	void	Tick() {}
	virtual	void	Draw() {}

	virtual	void	OnKeyDown(int keyVal, bool bRep) {}
	virtual	void	OnKeyUp(int keyVal) {}
	virtual	void	OnMouseDown(FvVector2& pos) {}
	virtual	void	OnMouseUp() {}
	virtual	void	OnMouseMove(FvVector2& pos) {}
	virtual	void	OnCmd(const char* pcCmd) {}

	void			SetFramePeriod(int period);
	void			SetTimerPeriod(int period);
	void			SetWindowSize(int w, int h);
	void			SetWindowTitle(const char* pcTitle);
	void			SetViewCenter(float x, float y);
	void			SetWindowFit2Rect(const FvVector4& rect);

	void			GetWindowSize(int& w, int& h);
};


