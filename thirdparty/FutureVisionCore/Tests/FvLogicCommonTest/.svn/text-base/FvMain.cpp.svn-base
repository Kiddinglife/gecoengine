

#include <vld.h>
#include <FvCaller.h>
#include <FvFastTimer.h>
#include <FvTimeFlow.h>
#include <FvMemoryNode.h>
#include <FvDestroyManager.h>
#include <FvRoute.h>
#include <FvValueModifier.h>
#include <FvValueNotifier.h>
#include <FvLogBus.h>
#include <FvMotor.h>
#include "WorkTimeFlow.h"

class Test
{
public:
	Test()
	{
		FvLogBus::CritOk("Test构造");
	}
	~Test()
	{
		FvLogBus::CritOk("Test析构");
	}
};

class _Class
{
public:
	Test Get(){return m_kTest;}
	Test m_kTest;

	int k;
};


class C
{
public:
	//...

	int a_;
};


//class TypeNull;
//
//template<typename HEAD, typename TAIL>
//struct TypeList
//{
//	typedef HEAD Head;
//	typedef TAIL Tail;
//};
//
//#define TYPELIST_1(T1)                  TypeList<T1,TypeNull>
//#define TYPELIST_2(T1, T2)              TypeList<T1, TYPELIST_1(T2) >
//#define TYPELIST_3(T1, T2, T3)          TypeList<T1, TYPELIST_2(T2, T3) >
//#define TYPELIST_4(T1, T2, T3, T4)      TypeList<T1, TYPELIST_3(T2, T3, T4) >
//#define TYPELIST_5(T1, T2, T3, T4, T5)  TypeList<T1, TYPELIST_4(T2, T3, T4, T5) >
//#endif


//template<class T> 
//struct ParamList
//{
//	T _param;
//};

//template<> 
//struct ParamList<TypeNull> 
//{};
//
//template <typename T, typename N>  
//struct FvTypeList
//{  
//	typedef T head;  
//	typedef N tail;  
//};  
//struct NULLType;  
//
//template<typename FvTypeList>  
//struct length;  
//
//template<>  
//struct length<NULLType>
//{  
//	enum{ len = 0};  
//};  
//
////假如以FvTypeList<char, FvTypeList<string, NULLType>> 为例  
////  
//template<typename T, typename N>  
//struct length< FvTypeList<T,N> >
//{//这儿的N就是FvTypeList<string,NULLType>  
//	enum{len = 1+ length<N>::len};//递归调用上面，直到遇到一个特化版本  
//};  
//
//void Done()
//{
//	ParamList<char, ParamList<char, char>> test1;
//	test1._head;
//	test1._tail;
//	test1._tail._head;
//}


template <class Head, class Tail>
struct ValueList
{
	Head _head;
	Tail _tail;

	void Done()
	{
		_head;
		_tail.Done();
	}
};


#include <queue>
#include <vector>

class TestClass_priority_queue
{
public:
	int _value;
};
class TestClass_priority_queueComparator
{
public:
	bool operator()
		( const TestClass_priority_queue * a, const TestClass_priority_queue * b )
	{
		return a->_value > b->_value;
	}
};

void test()
{
	typedef std::priority_queue<TestClass_priority_queue*, std::vector<TestClass_priority_queue*>, TestClass_priority_queueComparator> PriorityQueue;
	PriorityQueue kPriorityQueue;
	for (int i = 0; i < 10000; ++i)
	{
		TestClass_priority_queue* pkNew = new TestClass_priority_queue();
		pkNew->_value = i;
		kPriorityQueue.push(pkNew);
	}
	for (int i = 0; i < 10000; ++i)
	{
		kPriorityQueue.pop();
	}
}


#include "FvWorkFlow.h"

#include "FvGameRandom.h"
#include "FvValueMapping.h"


#include "Log.h"




//+----------------------------------------------------------------------------------------------------
#include "FvMotor.h"
void TestMotor()
{
	while(1)
	{
		FvGravityTrackMotor kMotor;
		FvMotor::TrackBallPtr spTrackBall = new FvMotor::TrackBall();
		const FvVector3 kStartPos(0, 0, 0);
		const FvVector3 kEndPos(100, 0, 0);
		spTrackBall->SetValue0(kEndPos);
		kMotor.SetTransltate(kStartPos);
		kMotor.SetTarget(spTrackBall);
		kMotor.Start(3.0f);

		FvLogBus::CritOk("Start(%f, %f, %f) End(%f, %f, %f)", kStartPos.x, kStartPos.y, kStartPos.z, 
			kEndPos.x, kEndPos.y, kEndPos.z);

		float fTime = 0.0f;
		while(fTime <= 3.1f)
		{
			kMotor.Update(0.03f);

			const FvVector3& kPos = kMotor.GetTranslate();
			const FvVector3& kDir = kMotor.GetDirection();
			const FvVector3& kVel = kMotor.Velocity();
			FvLogBus::CritOk("Pos(%f, %f, %f) Dir(%f, %f, %f) Vel(%f, %f, %f)", kPos.x, kPos.y, kPos.z, 
				kDir.x, kDir.y, kDir.z, 
				kVel.x, kVel.y, kVel.z);

			fTime += 0.03f;
		}
	}
}



#include <FvLodTickTask.h>

#include <FvRoundPoint.h>
#include <FvRefNodePool.h>


void TestStr(const std::string& kStr)
{

}

const std::string& S_S = "S_S";


#include "FvTestLogBus.h"
_INT_PRINT_(FvUInt8)
_INT_PRINT_(FvUInt16)
_INT_PRINT_(FvInt32)
_INT_PRINT_(FvUInt32)
_INT_PRINT_(FvInt64)
_INT_PRINT_(FvUInt64)
_INT_PRINT_(bool)
_FLOAT_PRINT_(float)
_FLOAT_PRINT_(double)


#include "FvGeographicObject.h"

void TestWingInGround()
{
	FvVector3 kNormal(0.4f,0.4f,0.8f);
	kNormal.Normalise();
	FvVector3 kFront(0.4f,0.4f, 0);
	kFront.Normalise();
	GroundWinger::Wing(GroundWinger::LINE, kNormal, kFront);
}


#include <FvCallback.h>
#include "FvRefTree.h"
#include "CommunicationTask.h "
#include "FvTrackFilter.h"
#include "FvRefMap.h"
#include "FvWeightList.h"
#include "FvAttachRelation.h"

void main()
{
	AttachRelation_Demo::TEST();
}

