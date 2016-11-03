#pragma once

#include <FvVector3.h>
#include <FvVector4.h>
#include <glui/GL/glui.h>

struct p2Clr
{
	p2Clr(float r_, float g_, float b_):r(r_),g(g_),b(b_) {}
	p2Clr():r(1.0f),g(1.0f),b(1.0f) {}

	void Set(float r_, float g_, float b_)
	{
		r = r_;
		g = g_;
		b = b_;
	}

	void Add(p2Clr& clr)
	{
		r += clr.r;
		g += clr.g;
		b += clr.b;
	}

	float r,g,b;
};

const p2Clr CLR_WHITE;
const p2Clr CLR_RED(1.0f, 0.0f, 0.0f);
const p2Clr CLR_BLUE(0.0f, 0.0f, 1.0f);
const p2Clr CLR_YELLOW(1.0f, 1.0f, 0.0f);
const p2Clr CLR_GREEN(0.0f, 1.0f, 0.0f);

void DrawString(int x, int y, const p2Clr& clr, const char* string, ...);
void DrawPoint(const FvVector3& pt, const p2Clr& clr=CLR_WHITE, float size=3.0f);
void DrawRect(const FvVector4& rt, const p2Clr& clr=CLR_WHITE);
void DrawCircle(const FvVector3& center, float radius, const p2Clr& color=CLR_WHITE);
//void DrawSegment(FvVector3& p1, p2Vec& p2, const p2Clr& clr=CLR_WHITE);
//void DrawPolygon(const FvVector3* vertices, int vertexCount, const p2Clr& color=CLR_WHITE);
//void DrawAABB(p2AABB* aabb, const p2Clr& color=CLR_WHITE);


