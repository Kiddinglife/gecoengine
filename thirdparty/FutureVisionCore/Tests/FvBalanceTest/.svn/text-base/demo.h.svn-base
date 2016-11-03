#pragma once

#include <vector>
#include <stdlib.h>
#include "SweepAndPrune.hpp"


struct aabb {
	float pos[2];
	float width[2];
};

struct box
{
	float pos[2];
	float width[2];
	float vel[2];
	int key;
	bool overlapStatus;
};

class Demo
{
	typedef SAP::SweepAndPrune<2, int> ColliderType;

	std::vector<box> boxes;

	// Settings
	float density;
	float spaceSize;
	float minObjSize, maxObjSize;
	float objVelocity;

	ColliderType collider;

	void ComputeSpatialBounds(int iObjsPerCell);

public:
	Demo( int _numObjects, float _density, float _minObjSize, float _maxObjSize, float _objVelocity, int _seedValue, int _iObjsPerCell );
	int  UpdateObjects();
	void GetDrawElements( std::vector<aabb>& aabbs, std::vector<int>& color );
	void GetSpatialBounds(std::vector<float>& mins, std::vector<float>& maxs);

};