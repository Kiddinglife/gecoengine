#include <stdio.h>
#include <FvVector2.h>
#include <FvBaseTypes.h>
#include <FvTimestamp.h>
#include <glui/GL/glui.h>
#include <freeglut/freeglut_internal.h>
#include "App.h"

int width = 64;
int height = 72;
FvVector2 homeViewCenter(0.0f, 0.0f);
FvVector2 viewCenter(0.0f, 0.0f);
float homeViewZoom = 25.0f;
float viewZoom = 9000.f;
int framePeriod = 15;
int timerPeriod = 16;
FvInt64 tickTime = 0;

namespace
{
	DWORD lastTime;
	int mainWindow;
	GLUI *glui(NULL);
	GLUI_CommandLine* pkCmd(NULL);
	int tx, ty, tw, th;
	bool lMouseDown = false;
	bool rMouseDown = false;
	FvVector2 lastp;
	App* g_pApp = NULL;
	bool kKeyPool[128];
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
	FvUInt64 uiNowTime = Timestamp();
	double detaTime = double(uiNowTime-tickTime)/StampsPerSecond();
	tickTime = uiNowTime;
	
	if(g_pApp) g_pApp->Tick(detaTime);

	glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutTimerFunc(timerPeriod, Timer, 0);
}

void Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		ConvertScreenToWorld(x, y, lastp);
		if(button == GLUT_LEFT_BUTTON)
			lMouseDown = true;
		else if(button == GLUT_RIGHT_BUTTON)
			rMouseDown = true;
		if(g_pApp) g_pApp->OnMouseDown(button,lastp);
	}

	if (state == GLUT_UP)
	{
		ConvertScreenToWorld(x, y, lastp);
		if(button == GLUT_LEFT_BUTTON)
			lMouseDown = false;
		else if(button == GLUT_RIGHT_BUTTON)
			rMouseDown = false;
		if(g_pApp) g_pApp->OnMouseUp(button,lastp);
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
	if(lMouseDown || rMouseDown)
	{
		ConvertScreenToWorld(x, y, lastp);
		if(lMouseDown)
			g_pApp->OnMouseWheel(GLUT_LEFT_BUTTON,direction,lastp);
		if(rMouseDown)
			g_pApp->OnMouseWheel(GLUT_RIGHT_BUTTON,direction,lastp);
		return;
	}

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
	if(!g_pApp)
		return;

	int iKeyVal(0);

	if(key>='0' && key<='9')
		iKeyVal = key-'0'+KV_0;
	else if(key>='a' && key<='z')
		iKeyVal = key-'a'+KV_a;
	else if(key>='A' && key<='Z')
		iKeyVal = key-'A'+KV_A;
	else if(key == 27)//! ESC
		iKeyVal = KV_ESC;
	else
		return;

	bool bRep = kKeyPool[iKeyVal];
	kKeyPool[iKeyVal] = true;

	g_pApp->OnKeyDown(iKeyVal, bRep);

	if(iKeyVal == KV_ESC)
	{
		DestroyApp();
		if(pkCmd)
		{
			delete pkCmd;
			pkCmd = NULL;
		}
		if(glui)
		{
			delete glui;
			glui = NULL;
		}
		exit(0);
		return;
	}
}

void KeyboardUp(unsigned char key, int x, int y)
{
	if(!g_pApp)
		return;

	int iKeyVal(0);

	if(key>='0' && key<='9')
		iKeyVal = key-'0'+KV_0;
	else if(key>='a' && key<='z')
		iKeyVal = key-'a'+KV_a;
	else if(key>='A' && key<='Z')
		iKeyVal = key-'A'+KV_A;
	else if(key == 27)//! ESC
		iKeyVal = KV_ESC;
	else
		return;

	kKeyPool[iKeyVal] = false;

	g_pApp->OnKeyUp(iKeyVal);
}

void KeyboardSpecial(int key, int x, int y)
{
	if(!g_pApp)
		return;

	int iKeyVal(0);

	if(key>=GLUT_KEY_F1 && key<=GLUT_KEY_F12)
		iKeyVal = key-GLUT_KEY_F1+KV_F1;
	else if(key>=GLUT_KEY_LEFT && key<=GLUT_KEY_INSERT)
		iKeyVal = key-GLUT_KEY_LEFT+KV_LEFT;
	else
		return;

	bool bRep = kKeyPool[iKeyVal];
	kKeyPool[iKeyVal] = true;

	g_pApp->OnKeyDown(iKeyVal, bRep);

	if(iKeyVal == KV_HOME)
	{
		viewZoom = homeViewZoom;
		viewCenter.Set(homeViewCenter.x, homeViewCenter.y);
		Resize(width, height);
		return;
	}
}

void KeyboardSpecialUp(int key, int x, int y)
{
	if(!g_pApp)
		return;

	int iKeyVal(0);

	if(key>=GLUT_KEY_F1 && key<=GLUT_KEY_F12)
		iKeyVal = key-GLUT_KEY_F1+KV_F1;
	else if(key>=GLUT_KEY_LEFT && key<=GLUT_KEY_INSERT)
		iKeyVal = key-GLUT_KEY_LEFT+KV_LEFT;
	else
		return;

	kKeyPool[iKeyVal] = false;

	g_pApp->OnKeyUp(iKeyVal);
}

void CmdLineCallBack(int id)
{
	if(!g_pApp)
		return;

	const char* pcTxt = pkCmd->get_text();
	if(!pcTxt) return;

	g_pApp->OnCmd(pcTxt);

	pkCmd->set_text("");
}

int main(int argc, char* argv[])
{
	memset(kKeyPool, false, sizeof(kKeyPool));

	//! Create App
	if(!(g_pApp = CreateApp()))
		return 1;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	char title[32];
	sprintf(title, "FvBalanceTest");
	mainWindow = glutCreateWindow(title);

	StampsPerSecond();
	tickTime = Timestamp();

	//! Init App
	if(!g_pApp->Init(argc, argv))
	{
		DestroyApp();
		return 1;
	}

	glutDisplayFunc(LogicLoop);
	GLUI_Master.set_glutReshapeFunc(Resize);
	GLUI_Master.set_glutKeyboardFunc(Keyboard);
	GLUI_Master.set_glutKeyboardUpFunc(KeyboardUp);
	GLUI_Master.set_glutSpecialFunc(KeyboardSpecial);
	GLUI_Master.set_glutSpecialUpFunc(KeyboardSpecialUp);
	GLUI_Master.set_glutMouseFunc(Mouse);
#ifdef FREEGLUT
	glutMouseWheelFunc(MouseWheel);
#endif
	glutMotionFunc(MouseMotion);

	//! √¸¡Ó––start
	glui = GLUI_Master.create_glui_subwindow( mainWindow, GLUI_SUBWINDOW_BOTTOM );
	pkCmd = new GLUI_CommandLine(glui, "Cmd", NULL, 1, CmdLineCallBack);
	pkCmd->set_w(300);
	//! √¸¡Ó––end

	// Use a timer to control the frame rate.
	glutTimerFunc(timerPeriod, Timer, 0);
	lastTime = GetTickCount();

	glutMainLoop();

	return 0;
}


















