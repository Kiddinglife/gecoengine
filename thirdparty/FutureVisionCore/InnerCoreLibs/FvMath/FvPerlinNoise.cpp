//{future source message}
#include "FvPerlinNoise.h"

#define B 0x100
#define BM 0xff

static int p[B + B + 2];
static float g3[B + B + 2][3];
static float g2[B + B + 2][2];
static float g1[B + B + 2];
static bool start = true;

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

#define s_curve(t) ( t * t * (3.0f - 2.0f * t) )

#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.0f;

#define at2(rx,ry) (rx * q[0] + ry * q[1])

#define at3(rx,ry,rz) (rx * q[0] + ry * q[1] + rz * q[2])

//----------------------------------------------------------------------------
void FvPerlinNoise::Init()
{
	int i, j, k;

	for (i = 0 ; i < B ; i++) {
		p[i] = i;

		g1[i] = (float)((FvRand() % (B + B)) - B) / B;

		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = (float)((FvRand() % (B + B)) - B) / B;
		FvNormalize2f(g2[i], g2[i]);

		for (j = 0 ; j < 3 ; j++)
			g3[i][j] = (float)((FvRand() % (B + B)) - B) / B;
		FvNormalize3f(g3[i], g3[i]);
	}

	while (--i) {
		k = p[i];
		p[i] = p[j = FvRand() % B];
		p[j] = k;
	}

	for (i = 0 ; i < B + 2 ; i++) {
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0 ; j < 2 ; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0 ; j < 3 ; j++)
			g3[B + i][j] = g3[i][j];
	}
}
//----------------------------------------------------------------------------
float FvPerlinNoise::Noise1(float fArgument)
{
	int bx0, bx1;
	float rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = fArgument;
	if (start) {
		start = false;
		Init();
	}

	setup(0, bx0,bx1, rx0,rx1);

	sx = s_curve(rx0);

	u = rx0 * g1[ p[ bx0 ] ];
	v = rx1 * g1[ p[ bx1 ] ];

	return FV_LERP(sx, u, v);
}
//----------------------------------------------------------------------------
float FvPerlinNoise::Noise2(const FvVector2& vec)
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	register int i, j;

	if (start) {
		start = false;
		Init();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	sx = s_curve(rx0);
	sy = s_curve(ry0);

	q = g2[ b00 ] ; u = at2(rx0,ry0);
	q = g2[ b10 ] ; v = at2(rx1,ry0);
	a = FV_LERP(sx, u, v);

	q = g2[ b01 ] ; u = at2(rx0,ry1);
	q = g2[ b11 ] ; v = at2(rx1,ry1);
	b = FV_LERP(sx, u, v);

	return FV_LERP(sy, a, b);
}
//----------------------------------------------------------------------------
float FvPerlinNoise::Noise3(float vec[3])
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	register int i, j;

	if (start) {
		start = false;
		Init();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);
	setup(2, bz0,bz1, rz0,rz1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);

	q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
	q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
	a = FV_LERP(t, u, v);

	q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
	q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
	b = FV_LERP(t, u, v);

	c = FV_LERP(sy, a, b);

	q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
	q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
	a = FV_LERP(t, u, v);

	q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
	q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
	b = FV_LERP(t, u, v);

	d = FV_LERP(sy, a, b);

	return FV_LERP(sz, c, d);
}
//----------------------------------------------------------------------------
float FvPerlinNoise::SumHarmonics2(const FvVector2& kVector,
	float fPersistence, float fFrequency, float fIterations)
{
	FvVector2 kCurrent(kVector);
	kCurrent *= fFrequency;
	FvVector2 kNormal(kVector);
	kNormal.Normalise();
	kNormal *= fFrequency;

	float fSum = 0.f;
	float fScale = 1.f;
	float i = 0.f;

	while (i < fIterations)
	{
		fSum += (fScale * Noise2(kCurrent));
		fScale *= fPersistence;
		kNormal += kNormal;
		i += 1.f;
		kCurrent += kNormal;
	}

	return fSum + 0.5f;
}
//----------------------------------------------------------------------------
