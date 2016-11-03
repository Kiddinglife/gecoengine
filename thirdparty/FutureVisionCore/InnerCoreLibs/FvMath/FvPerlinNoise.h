//{future header message}
#ifndef __FvPerlinNoise_H__
#define __FvPerlinNoise_H__

#include "FvVector2.h"

class FV_MATH_API FvPerlinNoise
{
public:
	void Init();

	float Noise1(float fArgument);

	float Noise2(const FvVector2& kVector);

	float Noise3(float afVector[3]);

	float SumHarmonics2(const FvVector2& kVector, float fPersistence, float fFrequency, float fIterations);

};

#endif /* __FvPerlinNoise_H__ */
