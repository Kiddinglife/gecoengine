//{future header message}
#include "FvHullTreeTest.h"
#include "FvHullTree.h"
#include <stdio.h>

class hullData : public FvHullContents
{
public:
	hullData(int d)
	{
		id = d;
	}

	void Print() const
	{
		printf_s("Id: %d,", id);
		if(m_pkNext)
			((const hullData*)m_pkNext)->Print();
		else
			printf_s("\n");
	}

protected:
	int id;
};

void FvHullTreeTest::RunTest()
{
	FvHullBorder border0;
	hullData data0(0);

	FvHullBorder border1;
	hullData data1(1);

	FvHullBorder border2;
	hullData data2(2);

	border0.push_back(FvPlane(FvVector3(0,0,1), -10));
	border0.push_back(FvPlane(FvVector3(0,0,-1), -10));
	border0.push_back(FvPlane(FvVector3(0,1,0), -10));
	border0.push_back(FvPlane(FvVector3(0,-1,0), -10));
	border0.push_back(FvPlane(FvVector3(1,0,0), -10));
	border0.push_back(FvPlane(FvVector3(-1,0,0), -10));

	border1.push_back(FvPlane(FvVector3(0,0,1), -20));
	border1.push_back(FvPlane(FvVector3(0,0,-1), -20));
	border1.push_back(FvPlane(FvVector3(0,1,0), -20));
	border1.push_back(FvPlane(FvVector3(0,-1,0), -20));
	border1.push_back(FvPlane(FvVector3(1,0,0), -20));
	border1.push_back(FvPlane(FvVector3(-1,0,0), -20));

	border2.push_back(FvPlane(FvVector3(0,0,1), -30));
	border2.push_back(FvPlane(FvVector3(0,0,-1), 10));
	border2.push_back(FvPlane(FvVector3(0,1,0), -20));
	border2.push_back(FvPlane(FvVector3(0,-1,0), -20));
	border2.push_back(FvPlane(FvVector3(1,0,0), -20));
	border2.push_back(FvPlane(FvVector3(-1,0,0), -20));

	FvHullTree kTree;
	kTree.Add(border0, &data0);
	kTree.Add(border1, &data1);
	kTree.Add(border2, &data2);

	const FvHullContents* datatest = kTree.TestPoint(FvVector3(9.9f,5,-19.9f));

	printf_s("TestPoint(FvVector3(9.9f,5,-19.9f)):\n");
	((const hullData*)(datatest))->Print();

	datatest = kTree.TestPoint(FvVector3(0,5,-5));
	printf_s("TestPoint(FvVector3(0,5,-5)):\n");
	((const hullData*)(datatest))->Print();

	datatest = kTree.TestPoint(FvVector3(0,5,-25));
	printf_s("TestPoint(FvVector3(0,5,-25)):\n");
	((const hullData*)(datatest))->Print();

}