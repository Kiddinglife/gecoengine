#include <stdio.h>
#include <FvVector2.h>
#include <FvBaseTypes.h>
#include <glui/GL/glui.h>
#include <freeglut/freeglut_internal.h>
#include "App.h"

int width = 64;
int height = 72;
FvVector2 homeViewCenter(0.0f, 0.0f);
FvVector2 viewCenter(0.0f, 0.0f);
float homeViewZoom = 25.0f;
float viewZoom = 25.0f;
int framePeriod = 500;
int timerPeriod = 16;

namespace
{
	DWORD lastTime;
	int mainWindow;
	GLUI *glui;
	int tx, ty, tw, th;
	bool lMouseDown = false;
	bool rMouseDown = false;
	FvVector2 lastp;
	WNDPROC	g_pfnMainWndProc = NULL;
	App* g_pApp = NULL;
}


void Resize(int w, int h)
{
	//printf("Window Resize to w:%d, h:%d\n", w, h);

	width = w;
	height = h;

	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	glViewport(tx, ty, tw, th);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float ratio = float(tw) / float(th);

	float exX = ratio * viewZoom;
	float exY = viewZoom;
	//float exX = ratio * 25.0f * viewZoom;
	//float exY = 25.0f * viewZoom;

	// L/R/B/T
	gluOrtho2D(viewCenter.x-exX, viewCenter.x+exX, viewCenter.y-exY, viewCenter.y+exY);
}

void ConvertScreenToWorld(int x, int y, FvVector2& p)
{
	float u = x / float(tw);
	float v = (th - y) / float(th);

	float ratio = float(tw) / float(th);

	float exX = viewZoom * ratio;
	float exY = viewZoom;
	FvVector2 dx(exX, exY);
	//FvVector2 dx(ratio * 25.0f * viewZoom, 25.0f * viewZoom);
	FvVector2 lower = viewCenter - dx;
	FvVector2 upper = viewCenter + dx;

	p.Set((1.0f - u) * lower.x + u * upper.x, (1.0f - v) * lower.y + v * upper.y);
}

void ConvertWorldToScreen(FvVector2& p, int& x, int& y)
{
	float ratio = float(tw) / float(th);

	float exX = viewZoom * ratio;
	float exY = viewZoom;
	FvVector2 dx(exX, exY);
	//FvVector2 dx(ratio * 25.0f * viewZoom, 25.0f * viewZoom);
	FvVector2 lower = viewCenter - dx;
	FvVector2 upper = viewCenter + dx;
	FvVector2 ori = lower;
	ori.y = upper.y;

	FvVector2 dis = p-ori;
	dis.y = -dis.y;

	x = int(dis.x * tw / (upper.x-lower.x));
	y = int(dis.y * th / (upper.y-lower.y));
}

// This is used to control the frame rate (60Hz).
void Timer(int)
{
	if(g_pApp) g_pApp->Tick();

	glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutTimerFunc(timerPeriod, Timer, 0);
}

void HandleKeyDown(int wParam, int lParam)
{
	bool bRep = lParam & 0x40000000 ? true : false;

	if(wParam == VK_ESCAPE)
	{
		DestroyApp();
		exit(0);
		return;
	}
	else if(wParam == VK_HOME)
	{
		viewZoom = homeViewZoom;
		viewCenter.Set(homeViewCenter.x, homeViewCenter.y);
		Resize(width, height);
		return;
	}

	if(wParam>=KV_0 && wParam<=KV_9
	|| wParam>=KV_A && wParam<=KV_Z
	|| wParam>=KV_F1 && wParam<=KV_F8)
	{
		if(g_pApp) g_pApp->OnKeyDown(wParam, bRep);
	}
}

void HandleKeyUp(int wParam, int lParam)
{
	if(wParam>=KV_0 && wParam<=KV_9
		|| wParam>=KV_A && wParam<=KV_Z
		|| wParam>=KV_F1 && wParam<=KV_F8)
	{
		if(g_pApp) g_pApp->OnKeyUp(wParam);
	}
}

LRESULT CALLBACK HookedWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_KEYDOWN:
		HandleKeyDown(wParam, lParam);
		return true;
	case WM_KEYUP:
		HandleKeyUp(wParam, lParam);
		return true;
	}

	return(CallWindowProc(g_pfnMainWndProc,hWnd,uMsg,wParam,lParam));
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			ConvertScreenToWorld(x, y, lastp);
			lMouseDown = true;
			if(g_pApp) g_pApp->OnMouseDown(lastp);
		}

		if (state == GLUT_UP)
		{
			lMouseDown = false;
			if(g_pApp) g_pApp->OnMouseUp();
		}
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{	
			ConvertScreenToWorld(x, y, lastp);
			rMouseDown = true;
		}

		if (state == GLUT_UP)
		{
			rMouseDown = false;
		}
	}
}

void MouseMotion(int x, int y)
{
	FvVector2 p;
	ConvertScreenToWorld(x, y, p);
	if(lMouseDown && g_pApp)
		g_pApp->OnMouseMove(p);

	if (rMouseDown)
	{
		FvVector2 diff = p - lastp;
		viewCenter -= diff;
		Resize(width, height);
		ConvertScreenToWorld(x, y, lastp);
	}
}

void MouseWheel(int wheel, int direction, int x, int y)
{
	if (direction > 0) {
		viewZoom /= 1.1f;
	} else {
		viewZoom *= 1.1f;
	}
	Resize(width, height);
}

void LogicLoop()
{
	DWORD nowTime = GetTickCount();
	if(nowTime < lastTime+framePeriod)
		return;
	lastTime = nowTime;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(g_pApp) g_pApp->Draw();

	glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y)
{
}

void KeyboardSpecial(int key, int x, int y)
{
}

int main(int argc, char* argv[])
{
	//! Create App
	if(!(g_pApp = CreateApp()))
		return 1;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	char title[32];
	sprintf(title, "TestBed2D");
	mainWindow = glutCreateWindow(title);

	//! Init App
	if(!g_pApp->Init(argc, argv))
	{
		DestroyApp();
		return 1;
	}

	glutDisplayFunc(LogicLoop);
	GLUI_Master.set_glutReshapeFunc(Resize);
	GLUI_Master.set_glutKeyboardFunc(Keyboard);
	GLUI_Master.set_glutSpecialFunc(KeyboardSpecial);
	GLUI_Master.set_glutMouseFunc(Mouse);
#ifdef FREEGLUT
	glutMouseWheelFunc(MouseWheel);
#endif
	glutMotionFunc(MouseMotion);

	//glui = GLUI_Master.create_glui_subwindow( mainWindow, GLUI_SUBWINDOW_BOTTOM );
	//glui->add_edittext("Cmd");
	//glui->set_main_gfx_window( mainWindow );

	// Use a timer to control the frame rate.
	glutTimerFunc(timerPeriod, Timer, 0);
	lastTime = GetTickCount();

	// Set windows hook
	g_pfnMainWndProc = (WNDPROC)glutGetWindowProc(mainWindow);
	int ret = glutSetWindowProc(mainWindow, (long)HookedWindowProc);

	glutMainLoop();

	return 0;
}


















