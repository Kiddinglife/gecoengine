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


enum
{
	KV_0,
	KV_1,
	KV_2,
	KV_3,
	KV_4,
	KV_5,
	KV_6,
	KV_7,
	KV_8,
	KV_9,

	KV_a,
	KV_b,
	KV_c,
	KV_d,
	KV_e,
	KV_f,
	KV_g,
	KV_h,
	KV_i,
	KV_j,
	KV_k,
	KV_l,
	KV_m,
	KV_n,
	KV_o,
	KV_p,
	KV_q,
	KV_r,
	KV_s,
	KV_t,
	KV_u,
	KV_v,
	KV_w,
	KV_x,
	KV_y,
	KV_z,

	KV_A,
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

	KV_F1,
	KV_F2,
	KV_F3,
	KV_F4,
	KV_F5,
	KV_F6,
	KV_F7,
	KV_F8,
	KV_F9,
	KV_F10,
	KV_F11,
	KV_F12,

	KV_LEFT,
	KV_UP,
	KV_RIGHT,
	KV_DOWN,
	KV_PAGE_UP,
	KV_PAGE_DOWN,
	KV_HOME,
	KV_END,
	KV_INSERT,
	KV_ESC,
};
