#include "Draw2D.h"


void DrawString(int x, int y, const p2Clr& clr, const char* string, ...)
{
	char buffer[128];

	va_list arg;
	va_start(arg, string);
	vsprintf(buffer, string, arg);
	va_end(arg);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(clr.r, clr.g, clr.b);
	glRasterPos2i(x, y);
	int length = (int)strlen(buffer);
	for (int i = 0; i < length; ++i)
	{
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, buffer[i]);
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void DrawPoint(const FvVector2& pt, const p2Clr& clr, float size)
{
	glPointSize(size);
	glBegin(GL_POINTS);
	glColor3f(clr.r, clr.g, clr.b);
	glVertex2f(pt.x, pt.y);
	glEnd();
	glPointSize(1.0f);
}

void DrawRect(const FvVector4& rt, const p2Clr& color)
{
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_LINE_LOOP);
	glVertex2f(rt.x, rt.z);
	glVertex2f(rt.x, rt.w);
	glVertex2f(rt.y, rt.w);
	glVertex2f(rt.y, rt.z);
	glEnd();
}

void DrawCircle(const FvVector2& center, float radius, const p2Clr& color)
{
	const int k_segments = 64;
	const float k_increment = 2.0f * FV_MATH_PI_F / k_segments;
	float theta = 0.0f;
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_LINE_LOOP);
	for(int i = 0; i < k_segments; ++i)
	{
		FvVector3 p;
		p.x = center.x + radius * cosf(theta);
		p.y = center.y + radius * sinf(theta);
		glVertex2f(p.x, p.y);
		theta += k_increment;
	}
	glEnd();
}

//void DrawSegment(p2Vec& p1, p2Vec& p2, const p2Clr& clr)
//{
//	glColor3f(clr.r, clr.g, clr.b);
//	glBegin(GL_LINES);
//	glVertex2f(p1.x, p1.y);
//	glVertex2f(p2.x, p2.y);
//	glEnd();
//}
//
//void DrawPolygon(const p2Vec* vertices, int vertexCount, const p2Clr& color)
//{
//	glColor3f(color.r, color.g, color.b);
//	glBegin(GL_LINE_LOOP);
//	for (int i = 0; i < vertexCount; ++i)
//	{
//		glVertex2f(vertices[i].x, vertices[i].y);
//	}
//	glEnd();
//}
//
//void DrawAABB(p2AABB* aabb, const p2Clr& color)
//{
//	glColor3f(color.r, color.g, color.b);
//	glBegin(GL_LINE_LOOP);
//	glVertex2f(aabb->min.x, aabb->min.y);
//	glVertex2f(aabb->max.x, aabb->min.y);
//	glVertex2f(aabb->max.x, aabb->max.y);
//	glVertex2f(aabb->min.x, aabb->max.y);
//	glEnd();
//}






















