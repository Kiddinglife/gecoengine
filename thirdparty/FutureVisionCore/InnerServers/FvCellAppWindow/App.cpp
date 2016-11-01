#include "App.h"
#include <glui/GL/glui.h>

extern int width;
extern int height;
extern FvVector2 homeViewCenter;
extern FvVector2 viewCenter;
extern float homeViewZoom;
extern float viewZoom;
extern int framePeriod;
extern int timerPeriod;

void App::SetFramePeriod(int period)
{
	if(period > 0)
		framePeriod = period;
}

void App::SetTimerPeriod(int period)
{
	if(period)
		timerPeriod = period;
}

void App::SetWindowSize(int w, int h)
{
	glutReshapeWindow(w, h);
	width = w;
	height = h;
}

void App::SetWindowTitle(const char* pcTitle)
{
	glutSetWindowTitle(pcTitle);
}

void App::SetViewCenter(float x, float y)
{
	homeViewCenter.Set(x, y);
	viewCenter.Set(x, y);
}

void App::SetWindowFit2Rect(const FvVector4& rect)
{
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	float ratioX = float(rect.y-rect.x) / float(tw);
	float ratioY = float(rect.w-rect.z) / float(th);
	homeViewZoom = max(ratioX, ratioY) * th * 0.5f;
	viewZoom = homeViewZoom;
}