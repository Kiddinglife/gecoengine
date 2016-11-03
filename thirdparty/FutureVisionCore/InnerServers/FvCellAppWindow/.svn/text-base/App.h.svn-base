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

	void			SetFramePeriod(int period);
	void			SetTimerPeriod(int period);
	void			SetWindowSize(int w, int h);
	void			SetWindowTitle(const char* pcTitle);
	void			SetViewCenter(float x, float y);
	void			SetWindowFit2Rect(const FvVector4& rect);
};


enum KEYVAL
{
	KV_0 = 0x30,
	KV_1,
	KV_2,
	KV_3,
	KV_4,
	KV_5,
	KV_6,
	KV_7,
	KV_8,
	KV_9,

	KV_A = 0x41,
	KV_B,
	KV_C,
	KV_D,
	KV_E,
	KV_F,
	KV_G,
	KV_H,
	KV_I,
	KV_J,
	KV_K,
	KV_L,
	KV_M,
	KV_N,
	KV_O,
	KV_P,
	KV_Q,
	KV_R,
	KV_S,
	KV_T,
	KV_U,
	KV_V,
	KV_W,
	KV_X,
	KV_Y,
	KV_Z,

	KV_F1 = 0x70,
	KV_F2,
	KV_F3,
	KV_F4,
	KV_F5,
	KV_F6,
	KV_F7,
	KV_F8,
};
