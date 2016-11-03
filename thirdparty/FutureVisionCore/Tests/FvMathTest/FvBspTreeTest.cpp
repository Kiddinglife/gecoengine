//{future header message}
#include "FvBspTreeTest.h"
#include "FvBsp.h"

void FvBspTreeTest::RunTest()
{
	FvWorldTriangle angle0(FvVector3(-1, 1, -1), FvVector3(1, 1, -1), FvVector3(-1, -1, -1));
	FvWorldTriangle angle1(FvVector3(-1, -1, -1), FvVector3(1, 1, -1), FvVector3(1, -1, -1));

	FvWorldTriangle angle2(FvVector3(-1, 1, 1), FvVector3(1, 1, 1), FvVector3(-1, 1, -1));
	FvWorldTriangle angle3(FvVector3(-1, 1, -1), FvVector3(1, 1, 1), FvVector3(1, 1, -1));

	FvWorldTriangle angle4(FvVector3(1, 1, 1), FvVector3(-1, 1, 1), FvVector3(-1, -1, 1));
	FvWorldTriangle angle5(FvVector3(1, 1, 1), FvVector3(-1, -1, 1), FvVector3(1, -1, 1));

	FvWorldTriangle angle6(FvVector3(-1, 1, 1), FvVector3(-1, 1, -1), FvVector3(-1, -1, 1));
	FvWorldTriangle angle7(FvVector3(-1, 1, -1), FvVector3(-1, -1, -1), FvVector3(-1, -1, 1));

	FvWorldTriangle angle8(FvVector3(1,1,-1), FvVector3(1,1,1), FvVector3(1,-1,-1));
	FvWorldTriangle angle9(FvVector3(1, 1, 1), FvVector3(1, -1, -1), FvVector3(1, -1, 1));

	FvWorldTriangle angle10(FvVector3(1,-1,1), FvVector3(-1,-1,1), FvVector3(1,-1,-1));
	FvWorldTriangle angle11(FvVector3(1, -1, -1), FvVector3(-1, -1, 1), FvVector3(-1, -1, -1));

	FvRealWorldTriangleSet set;
	set.push_back(angle0);
	set.push_back(angle1);
	set.push_back(angle2);
	set.push_back(angle3);
	set.push_back(angle4);
	set.push_back(angle5);
	set.push_back(angle6);
	set.push_back(angle7);
	set.push_back(angle8);
	set.push_back(angle9);
	set.push_back(angle10);
	set.push_back(angle11);

	FvBSPTree bsp(set);

	const FvWorldTriangle * ppHitangle[10];

	float resdist = 1000;

	//bool res = bsp.pRoot()->Intersects(FvWorldTriangle(FvVector3(0.9,0,0.9), FvVector3(0.9,0,-2), FvVector3(0,0,-2)), ppHitangle);
	bool res = bsp.GetRoot()->Intersects(FvVector3(-3,0,0), FvVector3(3,0,0), resdist, ppHitangle);
	//bool res = angle6.Intersects(FvVector3(-2,0,0), FvVector3(1,0,0), resdist);


	//FvWorldTriangle angle0(FvVector3(-1, 1, -1), FvVector3(1, 1, -1), FvVector3(-1, -1, -1));
}
