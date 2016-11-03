#include "MyApp.h"
#include "stdio.h"
#include "Draw2D.h"
#include <FvTimestamp.h>

#include "demo.h"
#include "FvRefMap.h"


MyApp* g_pkApp(NULL);
extern void ConvertWorldToScreen(FvVector2& p, int& x, int& y);


#define LINE_START		10
#define LINE_STEP		12
#define LINE_BOTTOM		20


MyApp::MyApp()
:m_pkDemo(NULL)
,m_bRunStep(false)
{
	srand((unsigned int)Timestamp());
	SetFramePeriod(100);
	SetTimerPeriod(100);
/**
	int numObjects = 100;
	float volumetricDensity = 1000.0f;
	float minBoxSize = 80.0f;
	float maxBoxSize = 80.0f;
	float boxVelocity = 2.0f;
	int seed = 23467;
	int iObjsPerCell = 10000;
	//int numObjects = 10000;
	//float volumetricDensity = 0.05f;
	//float minBoxSize = 3.0f;
	//float maxBoxSize = 7.0f;
	//float boxVelocity = 0.5f;
	//int seed = 23467;
	//int iObjsPerCell = 300;

	m_pkDemo = new Demo( numObjects, volumetricDensity, minBoxSize, maxBoxSize, boxVelocity, seed, iObjsPerCell );
**/

/**
	m_iMatrixSize = 3000;
	m_iTestCnt = 1000000;
	m_pTestData = new WORD[m_iTestCnt * 2];
	for(int i=0; i<m_iTestCnt; ++i)
	{
		m_pTestData[i<<1] = rand() % m_iMatrixSize;
		m_pTestData[(i<<1)+1] = rand() % m_iMatrixSize;

		while(m_pTestData[(i<<1)+1] == m_pTestData[i<<1])
			m_pTestData[(i<<1)+1] = rand() % m_iMatrixSize;
	}

	m_kMatrix.Init(m_iMatrixSize);
**/

/**
	//InitMyMemAlloc(100001);
	InitMyMemAlloc(400001);
	//m_pkMap = new stdext::hash_map<unsigned int, void*, hash_A, STLAllocator<std::pair<unsigned int, void*>>>;
	m_pkMap = new std::map<unsigned int, void*, std::less<unsigned int>, STLAllocator<std::pair<unsigned int, void*>>>;

	m_iHashTestCnt = 200000;
	m_pHashKey = new unsigned int[m_iHashTestCnt];
	//m_pMapItr = new stdext::hash_map<unsigned int, void*, hash_A, STLAllocator<std::pair<unsigned int, void*>>>::iterator[m_iHashTestCnt];
	void* p = NULL;
	for(int i=0; i<m_iHashTestCnt; ++i)
	{
		p = m_pHashKey +i;
		unsigned int key = rand()*100 + rand();
		while(!m_pkMap->insert(std::make_pair(key, p)).second)
			key = rand() + rand()*100;

		m_pHashKey[i] = key;
	}

	m_pHashKey1 = new unsigned int[m_iHashTestCnt];
	//m_pMapItr = new stdext::hash_map<unsigned int, void*, hash_A, STLAllocator<std::pair<unsigned int, void*>>>::iterator[m_iHashTestCnt];
	//void* p = NULL;
	for(int i=0; i<m_iHashTestCnt; ++i)
	{
		p = m_pHashKey1 +i;
		unsigned int key = rand()*100 + rand();
		while(!m_pkMap->insert(std::make_pair(key, p)).second)
			key = rand() + rand()*100;

		m_pHashKey1[i] = key;
	}

	delete m_pkMap;
	UnInitMyMemAlloc();

	InitMyMemAlloc(200001);
	//m_pkMap = new stdext::hash_map<unsigned int, void*, hash_A, STLAllocator<std::pair<unsigned int, void*>>>;
	m_pkMap = new std::map<unsigned int, void*, std::less<unsigned int>, STLAllocator<std::pair<unsigned int, void*>>>;

	for(int i=0; i<m_iHashTestCnt; ++i)
	{
		p = m_pHashKey +i;
		m_pkMap->insert(std::make_pair(m_pHashKey[i], p));
	}

	m_bUse1 = false;
**/

/**
	m_iNodeCnt = 1000;
	m_kNodeHead.pPre = NULL;
	m_kNodeHead.pNex = &m_kNodeTail;
	m_kNodeTail.pPre = &m_kNodeHead;
	m_kNodeTail.pNex = NULL;
	m_kNodeHead.objectID = 0;
	m_kNodeTail.objectID = 0;
	m_pIDs = new unsigned short[m_iNodeCnt];
	//m_pkNodes = new RelateNode[m_iNodeCnt];
	memset(m_pIDs, 0, sizeof(unsigned short)*m_iNodeCnt);
	m_pkArray = new int[m_iNodeCnt];
	for(int i=0; i<m_iNodeCnt; ++i)
	{
		RelateNode* pNode = new RelateNode();
		//RelateNode* pNode = &m_pkNodes[i];
		pNode->objectID = (unsigned short)(i+1);
		pNode->pNex = m_kNodeHead.pNex;
		pNode->pPre = &m_kNodeHead;
		m_kNodeHead.pNex = pNode;
		pNode->pNex->pPre = pNode;

		int idx = rand() % m_iNodeCnt;
		while(m_pIDs[idx])
			idx = rand() % m_iNodeCnt;

		m_pIDs[idx] = (unsigned short)(i+1);
		m_pkArray[idx] = i+1;
	}

	//kList = new FvRefList<int>;
	//kNodeList = new FvRefNode<int>[uiCNT];

	////! 插入
	//for (FvUInt32 uiIdx = 0; uiIdx < uiCNT; ++uiIdx)
	//{
	//	kNodeList[uiIdx].m_Content = uiIdx;
	//	kList->AttachBack(kNodeList[uiIdx]);
	//}
	//FV_ASSERT(kList->Size() == uiCNT);


	m_kMyIntArray.resize(m_iNodeCnt);
	for(int i=0; i<m_iNodeCnt; ++i)
	{
		m_kMyIntArray[i] = i;
	}
**/
}

MyApp::~MyApp()
{
/**
	if(m_pkDemo)
	{
		delete m_pkDemo;
		m_pkDemo = NULL;
	}

	delete [] m_pTestData;
**/
/**
	delete [] m_pHashKey;
	delete [] m_pHashKey1;
	//delete [] m_pMapItr;
	delete m_pkMap;
	UnInitMyMemAlloc();
**/
/**
	RelateNode* pNode = m_kNodeHead.pNex;
	while(pNode != &m_kNodeTail)
	{
		RelateNode* pNodeT = pNode;
		pNode = pNode->pNex;
		delete pNodeT;
	}
	delete [] m_pIDs;
	//delete [] m_pkNodes;
	delete [] m_pkArray;

	//for (FvUInt32 uiIdx = 0; uiIdx < uiCNT; ++uiIdx)
	//{
	//	kNodeList[uiIdx].Detach();
	//}

	delete [] m_pkObs;

	//delete [] m_pkValues;
**/
}

bool MyApp::Init(int argc, char* argv[])
{
	//if(!m_pkDemo)
	//	return false;

	SetWindowSize(800, 600);

/**
	m_pkValues = new FvUInt32[320];
	if(argc != 321)
		return false;

	for(int i=0; i<320; ++i)
		m_pkValues[i] = atoi(argv[i+1]);
**/

	//m_pkDemo->GetSpatialBounds(m_kMins, m_kMaxs);

	float fWidth = 7000.0f;
	m_GridMinX = -fWidth;
	m_GridMinY = -fWidth;
	m_GridMaxX = fWidth;
	m_GridMaxY = fWidth;
	m_kGrid.Init(m_GridMinX, m_GridMinY, m_GridMaxX-m_GridMinX, m_GridMaxY-m_GridMinY,
					m_GridMaxX-m_GridMinX, 1, 0.1f,
					1024, 512,
					1024, 512,
					1024, 512,
					1024*128, 1024*128);

	m_iObsCnt = 1000;
	//m_iPlayerCnt = 1;
	m_iPlayerCnt = (int)(m_iObsCnt / 9.0f);
	m_iMoveCnt = (int)(m_iObsCnt * 2.0f / 9.0f);
	m_pkObs = new MyObs[m_iObsCnt];
	m_fVel = 10.0f;
	m_fVision1 = m_fVision = 80.0f;

	for(int i=0; i<m_iObsCnt; ++i)
	{
		float x = rand() * (m_GridMaxX-m_GridMinX) / (RAND_MAX+1) + m_GridMinX;
		float y = rand() * (m_GridMaxY-m_GridMinY) / (RAND_MAX+1) + m_GridMinY;
		m_pkObs[i].objID = i;
		m_pkObs[i].obsID = m_iObsCnt + i;
		m_pkObs[i].x = x;
		m_pkObs[i].y = y;
		m_pkObs[i].vx = ((rand() +1)%137-68) * m_fVel / 68;
		m_pkObs[i].vy = ((rand() +1)%137-68) * m_fVel / 68;

		//! 人
		if(i < m_iPlayerCnt)
		{
			m_pkObs[i].objHandle = m_kGrid.AddObject(2, x, y, m_fVision, m_pkObs+i);
			m_pkObs[i].obsHandle = m_kGrid.AddObserver(m_pkObs[i].objHandle, 3, x, y, m_fVision, 0.0f, true, m_pkObs+i);
		}
		else//! 怪
		{
			m_pkObs[i].objHandle = m_kGrid.AddObject(1, x, y, m_fVision, m_pkObs+i);
			m_pkObs[i].obsHandle = m_kGrid.AddObserver(m_pkObs[i].objHandle, 2, x, y, m_fVision, 0.0f, false, m_pkObs+i);
		}
	}

	//m_kObs.x = 10;
	//m_kObs.y = 10;
	//m_kObs.id = 100;
	//m_kObs.handle = m_kGrid.AddObserver(m_kObs.x, m_kObs.y, 80.0f, 0.0f, true, &m_kObs);

	//for(int i=0; i<m_iObjCnt; ++i)
	//{
	//	do
	//	{
	//		float x = rand() * (m_GridMaxX-m_GridMinX) / (RAND_MAX+1) + m_GridMinX;
	//		float y = rand() * (m_GridMaxY-m_GridMinY) / (RAND_MAX+1) + m_GridMinY;
	//		m_kObj[i].id = i;
	//		m_kObj[i].handle = m_kGrid.AddObject(x, y, 80.0f, m_kObj+i);
	//		m_kObj[i].x = x;
	//		m_kObj[i].y = y;
	//	}while(!m_kObj[i].handle);
	//}

	m_ShowAoI = 1;
	m_bShowID = false;
	m_bShow = true;
	m_bShowLog = true;
	m_bShowGrid = false;
	//m_bRunStep = true;
	m_bAdd = true;
	m_bSetMask = true;

	return true;
}

inline void swap1(unsigned short& a, unsigned short& b)
{
	a = a ^ b;
	b = a ^ b;
	a = a ^ b;
}

inline void swap2(unsigned short& a, unsigned short& b)
{
	unsigned short c = a;
	a = b;
	b = c;
}

#define swap3(u, v)		\
	__asm	mov ax, u	\
	__asm	xchg ax, v	\
	__asm	xchg ax, u

#define swap4(u, v)		\
	__asm	mov ax, u	\
	__asm	mov bx, v	\
	__asm   mov u, bx	\
	__asm   mov v, ax

inline int Float2Int(float f)
{
	const float FLOAT_FTOI_MAGIC_NUM =  (float)(3<<21);
	const float IT_FTOI_MAGIC_NUM =  (float)(3<<21);
	f += FLOAT_FTOI_MAGIC_NUM - 0.2f;
	return ((*((int*)&f) - *((int*)&IT_FTOI_MAGIC_NUM)) >>1);
}

//inline DWORD EncodeFloat(const float val)
//{
//	DWORD ir = (DWORD&)(val);
//	if(ir & 0x80000000) ir = ~ir;
//	else				ir |= 0x80000000;
//	return ir;
//}

inline DWORD Float2Int1(float f)
{
	return DWORD(f);
}

inline void Float2Int2(int& ival, double fval)
{
	_asm
	{
		fld fval
		mov edx, dword ptr [ival]
		fistp dword ptr [edx]
	}
}

//inline int Float2Int3(float fval)
//{
//	static const float magic = 12582910.5f;
//	fval += magic;
//	return *(int*)&fval;
//}

inline int Float2Int3(double fval)
{
	static const double magic = 6755399441055744.0;
	fval += magic;
	return *(int*)&fval;
}


class TestSwap
{
public:
	TestSwap(unsigned short a_, unsigned short b_)
	{
		iCnt = 100000000;
		T1 = T2 = T3 = T4 = 0.0f;
		s1 = s2 = s3 = s4 = 0;

		pA = new unsigned short[iCnt];
		pB = new unsigned short[iCnt];
	}

	~TestSwap()
	{
		delete [] pA;
		delete [] pB;
	}

	void Test1()
	{
		for(int i=0; i<iCnt; ++i)
		{
			unsigned short a = pA[i];
			unsigned short b = pB[i];
			s1 += (a<<1) + b;
		}

		FvUInt64 t1, t2;
		t1 = Timestamp();
		for(int i=0; i<iCnt; ++i)
		{
			unsigned short a = pA[i];
			unsigned short b = pB[i];
			swap1(a, b);
			s1 += (a<<1) + b;
		}
		t2 = Timestamp();
		T1 = double(t2-t1)/StampsPerSecond()*1000.0;
	}

	void Test2()
	{
		for(int i=0; i<iCnt; ++i)
		{
			unsigned short a = pA[i];
			unsigned short b = pB[i];
			s2 += (a<<1) + b;
		}

		FvUInt64 t1, t2;
		t1 = Timestamp();
		for(int i=0; i<iCnt; ++i)
		{
			unsigned short a = pA[i];
			unsigned short b = pB[i];
			swap2(a, b);
			s2 += (a<<1) + b;
		}
		t2 = Timestamp();
		T2 = double(t2-t1)/StampsPerSecond()*1000.0;
	}

	void Test3()
	{
		//FvUInt64 t1, t2;
		//t1 = Timestamp();
		//for(int i=0; i<iCnt; ++i)
		//{
		//	swap3(a, b);
		//}
		//t2 = Timestamp();
		//T3 = double(t2-t1)/StampsPerSecond()*1000.0;
	}

	void Test4()
	{
		//FvUInt64 t1, t2;
		//t1 = Timestamp();
		//for(int i=0; i<iCnt; ++i)
		//{
		//	swap4(a, b);
		//}
		//t2 = Timestamp();
		//T4 = double(t2-t1)/StampsPerSecond()*1000.0;
	}

	void Print()
	{
		printf("t1:%f, t2:%f, t3:%f, t4:%f, s:%I64u\n", T1, T2, T3, T4, s1+s2+s3+s4);
	}

private:
	unsigned short* pA, * pB;
	double T1, T2, T3, T4;
	FvUInt64 s1, s2, s3, s4;
	int iCnt;
};

const FvUInt32 YISA_CNT = 10240;

int testChar(int k)
{
	int iSum(0);
	char buf[YISA_CNT];
	for(int i=0; i<YISA_CNT; ++i)
	{
		buf[i] = i+k;
	}
	for(int i=0; i<YISA_CNT; ++i)
	{
		iSum += buf[i];
	}
	return iSum;
}

int testStaticChar(int k)
{
	int iSum(0);
	static char buf[YISA_CNT];
	for(int i=0; i<YISA_CNT; ++i)
	{
		buf[i] = i+k;
	}
	for(int i=0; i<YISA_CNT; ++i)
	{
		iSum += buf[i];
	}
	return iSum;
}

int testStack(int k)
{
	int iSum(0);
	//char* buf = (char*)_malloca(YISA_CNT);
	//for(int i=0; i<YISA_CNT; ++i)
	//{
	//	buf[i] = i+k;
	//}
	//for(int i=0; i<YISA_CNT; ++i)
	//{
	//	iSum += buf[i];
	//}
	return iSum;
}

int testHeap(int k)
{
	int iSum(0);
	//char* buf = (char*)malloc(YISA_CNT);
	//for(int i=0; i<YISA_CNT; ++i)
	//{
	//	buf[i] = i+k;
	//}
	//for(int i=0; i<YISA_CNT; ++i)
	//{
	//	iSum += buf[i];
	//}
	//free(buf);
	return iSum;
}

FvUInt32 TestLog2(FvUInt32 uiValue)
{
	FvUInt32 uiN;
	__asm {
		bsf eax, uiValue
		mov uiN, eax
	}
	return uiN;
}


void MyApp::Tick()
{
	if(m_bRunStep)
		return;

	MyTick();
	return;

	//RefMap_Demo::Test();
/**
	FvUInt32 buf[320];
	memcpy(buf, m_pkValues, sizeof(FvUInt32)*320);

	FvUInt64 t1, t2;
	t1 = t2 = 0;
	double T1, T2, T3, T4, T5;
	T1 = T2 = T3 = T4 = T5 = 0;
	DWORD tmp1, tmp2, tmp3, tmp4;
	tmp1 = tmp2 = tmp3 = tmp4 = 0;

	t1 = Timestamp();
	for(int j=0; j<320; ++j)
	{
		tmp1 += TestLog2(buf[j]);
	}
	t2 = Timestamp();
	T1 = double(t2-t1)/StampsPerSecond()*1000.0;

	printf("t:%f, tmp:%d\n", T1, tmp1);
**/

/**
	int sss = RoundToInt(1.0f);

	for(int i=0; i<10000; ++i)
	{
		for(int j=0; j<10000000; ++j)
		{
			float f;
			int k;
			f = -i - j*0.0000001f;
			k = RoundToInt(f);
			if(j==0 && k != -i ||
				j!=0 && (k != -i-1 && k != -i))
			{
				printf("-%d.%d\n", i, j);
			}
			f = i + j*0.0000001f;
			k = RoundToInt(f);
			if(j==0 && k != i ||
				j!=0 && (k != i+1 && k != i))
			{
				printf("%d.%d\n", i, j);
			}
		}
	}

	printf(">>>>>>>>>\n");
	int ssd = 0;
**/
	//static FvUInt64 sTime = Timestamp();
	//if(Timestamp() < sTime + StampsPerSecond())
	//	return;
	//sTime = Timestamp();
/**
	class MyListener
	{
	public:
		void OnEnter(void* pkObsData, FvAoIExt* pkExt)
		{

		}
		void OnStand(void* pkObsData, FvAoIExt* pkExt)
		{

		}
		void OnLeave(void* pkObsData, FvAoIExt* pkExt)
		{

		}
	};

	MyListener kListener;

	FvUInt64 t1, t2;
	double T1,T2;
	T1 = T2 = 0;
	FvUInt64 cnt(0);

	if(m_bAdd)
	{
		m_fVision1 += 1.0f;
		if(m_fVision1 > 200.0f)
			m_bAdd = false;
	}
	else
	{
		m_fVision1 -= 1.0f;
		if(m_fVision1 < 0.0f)
		{
			m_fVision1 = 0.0f;
			m_bAdd = true;
		}
	}

	//printf("------------------\n");
	//class Visiter
	//{
	//public:
	//	void OnVisit(void* pkObsData, FvAoIExt* pkExt)
	//	{
	//		FV_ASSERT(pkObsData);
	//		FV_ASSERT(pkExt);
	//		MyObs* pkObs = (MyObs*)pkObsData;
	//		MyAoIExt* pkMyExt = (MyAoIExt*)pkExt;
	//		printf("%d -> %d\n", pkObs->objID, pkMyExt->id);
	//	}
	//};
	//Visiter kVisiter;

	t1 = Timestamp();
	for(int i=0; i<m_iObsCnt; ++i)
	//for(int i=0; i<m_iMoveCnt; ++i)
	{
		//float x = rand() * (m_GridMaxX-m_GridMinX) / (RAND_MAX+1) + m_GridMinX;
		//float y = rand() * (m_GridMaxY-m_GridMinY) / (RAND_MAX+1) + m_GridMinY;
		float x = m_pkObs[i].x + m_pkObs[i].vx;
		float y = m_pkObs[i].y + m_pkObs[i].vy;

		if(x < m_GridMinX || m_GridMaxX < x)
			m_pkObs[i].vx = -m_pkObs[i].vx;
		if(y < m_GridMinY || m_GridMaxY < y)
			m_pkObs[i].vy = -m_pkObs[i].vy;

//#ifdef FV_DEBUG
//		cnt += m_kGrid.Move(m_pkObs[i].objHandle, x, y);
//		cnt += m_kGrid.Move(m_pkObs[i].obsHandle, x, y);
//#else
//		m_kGrid.Move(m_pkObs[i].objHandle, x, y);
//		m_kGrid.Move(m_pkObs[i].obsHandle, x, y);
//#endif

		if(i < m_iPlayerCnt)
		{
			m_kGrid.Move(m_pkObs[i].objHandle, x, y);
			m_kGrid.Move(m_pkObs[i].obsHandle, x, y);
			//m_kGrid.QueryVision<Visiter, &Visiter::OnVisit>(m_pkObs[i].obsHandle, &kVisiter);
			//if(m_bAdd)
			//{
			//	m_fVision += 1.0f;
			//	if(m_fVision > 200.0f)
			//		m_bAdd = false;
			//}
			//else
			//{
			//	m_fVision -= 1.0f;
			//	if(m_fVision < 0.0f)
			//	{
			//		m_fVision = 0.0f;
			//		m_bAdd = true;
			//	}
			//}
			//m_kGrid.SetVision(m_pkObs[i].obsHandle, m_fVision);
			//m_kGrid.SetDisVisibility(m_pkObs[i].obsHandle, m_fVision);
			//if(m_bSetMask)
			//{
			//	m_kGrid.SetMask(m_pkObs[i].objHandle, 1);
			//	m_kGrid.SetMask(m_pkObs[i].obsHandle, 2);
			//}
			//else
			//{
			//	m_kGrid.SetMask(m_pkObs[i].objHandle, 2);
			//	m_kGrid.SetMask(m_pkObs[i].obsHandle, 3);
			//}
		}
		else
		{
			m_kGrid.Move(m_pkObs[i].objHandle, x, y);
			//m_kGrid.SetVisibility(m_pkObs[i].objHandle, m_fVision1);
			m_kGrid.Move(m_pkObs[i].obsHandle, x, y);
			//m_kGrid.QueryVision<Visiter, &Visiter::OnVisit>(m_pkObs[i].obsHandle, &kVisiter);
			//if(m_bSetMask)
			//{
			//	m_kGrid.SetMask(m_pkObs[i].objHandle, 2);
			//	m_kGrid.SetMask(m_pkObs[i].obsHandle, 3);
			//}
			//else
			//{
			//	m_kGrid.SetMask(m_pkObs[i].objHandle, 1);
			//	m_kGrid.SetMask(m_pkObs[i].obsHandle, 2);
			//}
		}

		m_pkObs[i].x = x;
		m_pkObs[i].y = y;
	}
	t2 = Timestamp();
	T1 = double(t2-t1)/StampsPerSecond()*1000.0;

	m_bSetMask = !m_bSetMask;

	FvUInt64 t21, t22, t23;
	FvUInt64 n1,n2,n3;
	t21 = t22 = t23 = 0;
	n1 = n2 = n3 = 0;

#ifdef FV_DEBUG
	t1 = Timestamp();
	for(int i=0; i<m_iPlayerCnt; ++i)
	{
		m_kGrid.UpdateAoI<MyListener,
			&MyListener::OnEnter,
			&MyListener::OnStand,
			&MyListener::OnLeave>
			(m_pkObs[i].obsHandle, &kListener,
			n1, n2, n3);
	}
	for(int i=m_iPlayerCnt; i<m_iObsCnt; ++i)
	{
		m_kGrid.UpdateTrap<MyListener,
			&MyListener::OnEnter,
			&MyListener::OnLeave>
			(m_pkObs[i].obsHandle, &kListener,
			n1, n2, n3);
	}
	t2 = Timestamp();
	T2 = double(t2-t1)/StampsPerSecond()*1000.0;
#else
	t1 = Timestamp();
	for(int i=0; i<m_iPlayerCnt; ++i)
	{
		m_kGrid.UpdateAoI<MyListener,
			&MyListener::OnEnter,
			&MyListener::OnStand,
			&MyListener::OnLeave>
			(m_pkObs[i].obsHandle, &kListener);
	}
	for(int i=m_iPlayerCnt; i<m_iObsCnt; ++i)
	{
		m_kGrid.UpdateTrap<MyListener,
			&MyListener::OnEnter,
			&MyListener::OnLeave>
			(m_pkObs[i].obsHandle, &kListener);
	}
	t2 = Timestamp();
	T2 = double(t2-t1)/StampsPerSecond()*1000.0;
#endif

	if(m_bShowLog)
	{
		printf("t1:%f, t2:%f, n0:%I64u, n1:%I64u, n2:%I64u, n3:%I64u, n4:%d, n5:%d\n", T1, T2,
			cnt, n1, n2, n3,
			m_kGrid.GetObjCntInVision(m_pkObs[0].obsHandle),
			m_kGrid.GetObsCntInDVision(m_pkObs[0].objHandle));
		//printf("t1:%f, t2:%f, t21:%f, t22:%f, t23:%f, n1:%I64d, n2:%I64d, n3:%I64d\n", T1, T2,
		//	float(double(t21)/double(StampsPerSecond()))*1000.0f, float(double(t22)/double(StampsPerSecond()))*1000.0f, float(double(t23)/double(StampsPerSecond()))*1000.0f,
		//	n1, n2, n3);
	}
**/
/**
	int iCnt = 10000;
	FvUInt64 t1, t2;
	t1 = t2 = 0;
	double T1, T2, T3, T4, T5;
	T1 = T2 = T3 = T4 = T5 = 0;
	DWORD tmp1, tmp2, tmp3, tmp4;
	tmp1 = tmp2 = tmp3 = tmp4 = 0;

	//static char buf[256];
	//char* buf = (char*)_malloca(256);
	//char* buf = (char*)malloc(256);
	//int iSum(0);

	//for(int i=0; i<sizeof(buf); ++i)
	//{
	//	buf[i] = rand();
	//}

	t1 = Timestamp();
	for(int j=0; j<iCnt; ++j)
	{
		tmp4 += testHeap(j);
	}
	t2 = Timestamp();
	T4 = double(t2-t1)/StampsPerSecond()*1000.0;


	t1 = Timestamp();
	for(int j=0; j<iCnt; ++j)
	{
		tmp3 += testStack(j);
	}
	t2 = Timestamp();
	T3 = double(t2-t1)/StampsPerSecond()*1000.0;

	t1 = Timestamp();
	for(int j=0; j<iCnt; ++j)
	{
		tmp2 += testStaticChar(j);
	}
	t2 = Timestamp();
	T2 = double(t2-t1)/StampsPerSecond()*1000.0;

	t1 = Timestamp();
	for(int j=0; j<iCnt; ++j)
	{
		tmp1 += testChar(j);
	}
	t2 = Timestamp();
	T1 = double(t2-t1)/StampsPerSecond()*1000.0;

	printf("t1:%f, t2:%f, t3:%f, t4:%f, t5:%f, tmp:%d/%d/%d/%d\n", T1, T2, T3, T4, T5, tmp1, tmp2, tmp3, tmp4);

	//free(buf);
**/
/**
	char* pStack(NULL);
	pStack = (char*)_malloca(1000);
	pStack[0] = 0;
	_freea(pStack);
	pStack = (char*)_malloca(100);
	pStack[0] = 0;
	_freea(pStack);

	printf("--------------------\n");
**/
	//__try
	//{
	//	pStack = (char*)_malloca(102400);
	//}
	//__except ((EXCEPTION_STACK_OVERFLOW == GetExceptionCode()) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	//{
	//	_resetstkoflw();
	//	printf(">>>>>>>>>>>>>> 1\n");
	//}


	//try
	//{
	//	pStack = (char*)_alloca(10240000);
	//}
	//catch(...)
	//{
	//	if(!pStack)
	//	{
	//		printf(">>>>>>>>>>>>>>\n");
	//	}
	//}

	int iCnt = 100000;
	FvUInt64 t1, t2;
	t1 = t2 = 0;
	double T1, T2, T3, T4, T5, T6;
	T1 = T2 = T3 = T4 = T5 = T6 = 0;
	DWORD tmp1, tmp2, tmp3, tmp4;
	tmp1 = tmp2 = tmp3 = tmp4 = 0;

	for(int i=0; i<m_iNodeCnt; ++i)
	{
		tmp1 += FindNode(m_pIDs[i])->objectID;
	}

	//t1 = Timestamp();
	//for(int i=0, j=0; i<iCnt; ++i, ++j)
	//{
	//	if(j >= m_iNodeCnt)
	//		j -= m_iNodeCnt;

	//	tmp2 += FindNode(m_pIDs[j])->objectID;
	//}
	//t2 = Timestamp();
	//T1 = double(t2-t1)/StampsPerSecond()*1000.0;

	//t1 = Timestamp();
	//for(int i=0, j=0; i<iCnt; ++i, ++j)
	//{
	//	if(j >= m_iNodeCnt)
	//		j -= m_iNodeCnt;

	//	tmp3 += j;
	//}
	//t2 = Timestamp();
	//T2 = double(t2-t1)/StampsPerSecond()*1000.0;

	t1 = Timestamp();
	for(int i=0, j=0; i<iCnt; ++i, ++j)
	{
		tmp4 += TraversalMyArray2();
	}
	t2 = Timestamp();
	T1 = double(t2-t1)/StampsPerSecond()*1000.0;

	t1 = Timestamp();
	for(int i=0, j=0; i<iCnt; ++i, ++j)
	{
		tmp4 += TraversalMyArray3();
	}
	t2 = Timestamp();
	T2 = double(t2-t1)/StampsPerSecond()*1000.0;

	//t1 = Timestamp();
	//for(int i=0; i<iCnt; ++i)
	//{
	//	tmp4 += TestRefList();
	//}
	//t2 = Timestamp();
	//T5 = double(t2-t1)/StampsPerSecond()*1000.0;

	//t1 = Timestamp();
	//for(int i=0; i<iCnt; ++i)
	//{
	//	tmp4 += TraversalList();
	//}
	//t2 = Timestamp();
	//T3 = double(t2-t1)/StampsPerSecond()*1000.0;

	t1 = Timestamp();
	for(int i=0; i<iCnt; ++i)
	{
		tmp4 += TraversalMyArray1();
	}
	t2 = Timestamp();
	T5 = double(t2-t1)/StampsPerSecond()*1000.0;

	t1 = Timestamp();
	for(int i=0; i<iCnt; ++i)
	{
		tmp4 += TraversalMyArray();
	}
	t2 = Timestamp();
	T3 = double(t2-t1)/StampsPerSecond()*1000.0;

	t1 = Timestamp();
	for(int i=0; i<iCnt; ++i)
	{
		tmp4 += TraversalArray();
	}
	t2 = Timestamp();
	T4 = double(t2-t1)/StampsPerSecond()*1000.0;

	t1 = Timestamp();
	for(int i=0; i<iCnt; ++i)
	{
		tmp4 += TraversalArray1();
	}
	t2 = Timestamp();
	T6 = double(t2-t1)/StampsPerSecond()*1000.0;

	printf("t1:%f, t2:%f, t3:%f, t4:%f, t5:%f, t6:%f, tmp:%d\n", T1, T2, T3, T4, T5, T6, tmp1+tmp2+tmp3+tmp4);


/**
	unsigned int* pUse, * pNex;
	if(m_bUse1)
	{
		pUse = m_pHashKey1;
		pNex = m_pHashKey;
	}
	else
	{
		pUse = m_pHashKey;
		pNex = m_pHashKey1;
	}
	m_bUse1 = !m_bUse1;

	FvUInt64 v1, v2, v3, v4, v5;
	v1 = v2 = v3 = v4 = v5 = 0;
	for(int i=0; i<m_iHashTestCnt; ++i)
	{
		v1 += pUse[i];
		v2 += pNex[i];
	}

	int iCnt = 10000;
	FvUInt64 t;
	double T1, T2, T3, T4;
	T1 = T2 = T3 = T4 = 0.0f;

	//t = Timestamp();
	//for(int i=0, j=0; i<iCnt; ++i, ++j)
	//{
	//	if(j >= m_iHashTestCnt)
	//		j -= m_iHashTestCnt;
	//	unsigned int key = m_pHashKey[j];
	//	vvv += key;
	//	//m_pMapItr[j] = m_kMap.find(key);
	//	//vvv += *(unsigned int*)(m_pMapItr[j]->second);
	//	//vvv += *(unsigned int*)((m_kMap.find(key))->second);
	//}
	//T4 = double(Timestamp() - t)/StampsPerSecond()*1000.0;

	t = Timestamp();
	for(int i=m_iHashTestCnt-1; i>=0; --i)
	{
		unsigned int key = pUse[i];
		v1 += key;
	}
	T4 = double(Timestamp() - t)/StampsPerSecond()*1000.0;

	//for(int i=0, j=0; i<iCnt; ++i, ++j)
	//{
	//	if(j >= m_iHashTestCnt)
	//		j -= m_iHashTestCnt;
	//	unsigned int key = m_pHashKey[j];
	//	//vvv += key;
	//	//m_pMapItr[j] = m_kMap.find(key);
	//	//vvv += *(unsigned int*)(m_pMapItr[j]->second);
	//	stdext::hash_map<unsigned int, void*, hash_A, STLAllocator<std::pair<unsigned int, void*>>>::iterator itr = m_pkMap->find(key);
	//	vvv += *(unsigned int*)(itr->second);
	//	//vvv += *(unsigned int*)((m_pkMap->find(key))->second);
	//}

	//t = Timestamp();
	//for(int i=0, j=0; i<iCnt; ++i, ++j)
	//{
	//	if(j >= m_iHashTestCnt)
	//		j -= m_iHashTestCnt;
	//	unsigned int key = m_pHashKey[j];
	//	//vvv += key;
	//	//m_pMapItr[j] = m_kMap.find(key);
	//	//vvv += *(unsigned int*)(m_pMapItr[j]->second);
	//	stdext::hash_map<unsigned int, void*, hash_A, STLAllocator<std::pair<unsigned int, void*>>>::iterator itr = m_pkMap->find(key);
	//	vvv += *(unsigned int*)(itr->second);
	//	//vvv += *(unsigned int*)((m_pkMap->find(key))->second);
	//}
	//T1 = double(Timestamp() - t)/StampsPerSecond()*1000.0;

	t = Timestamp();
	for(int i=m_iHashTestCnt-1; i>=0; --i)
	{
		unsigned int key = pUse[i];
		//stdext::hash_map<unsigned int, void*, hash_A, STLAllocator<std::pair<unsigned int, void*>>>::iterator itr = m_pkMap->find(key);
		std::map<unsigned int, void*, std::less<unsigned int>, STLAllocator<std::pair<unsigned int, void*>>>::iterator itr = m_pkMap->find(key);
		v3 += *(unsigned int*)(itr->second);
		//m_pkMap->erase(itr);
	}
	T3 = double(Timestamp() - t)/StampsPerSecond()*1000.0;

	//t = Timestamp();
	//for(int i=m_iHashTestCnt-1; i>=0; --i)
	//{
	//	unsigned int key = m_pHashKey[i];
	//	m_pkMap->insert(std::make_pair(key, m_pHashKey+i));
	//}
	//T2 = double(Timestamp() - t)/StampsPerSecond()*1000.0;

	t = Timestamp();
	for(int i=m_iHashTestCnt-1; i>=0; --i)
	{
		unsigned int key = pUse[i];
		//stdext::hash_map<unsigned int, void*, hash_A, STLAllocator<std::pair<unsigned int, void*>>>::iterator itr = m_pkMap->find(key);
		//v2 += *(unsigned int*)(itr->second);
		//m_pkMap->erase(itr);
		m_pkMap->erase(key);
		m_pkMap->insert(std::make_pair(pNex[i], pNex+i));
	}
	T1 = double(Timestamp() - t)/StampsPerSecond()*1000.0;

	//for(int i=m_iHashTestCnt-1; i>=0; --i)
	//{
	//	unsigned int key = m_pHashKey[i];
	//	stdext::hash_map<unsigned int, void*, hash_A, STLAllocator<std::pair<unsigned int, void*>>>::iterator itr = m_pkMap->find(key);
	//	vvv += *(unsigned int*)(itr->second);
	//}

	printf("t1:%f, t2:%f, t3:%f, t4:%f, %I64u\n", T1, T2, T3, T4, v1+v2+v3+v4+v5);

	//for(int i=m_iHashTestCnt-1; i>=0; --i)
	//{
	//	unsigned int key = m_pHashKey[i];
	//	m_pkMap->insert(std::make_pair(key, m_pHashKey+i));
	//}
**/

/**
	FvUInt64 t1, t2;
	t1 = t2 = 0;
	double T1, T2, T3, T4;
	T1 = T2 = T3 = T4 = 0;
	DWORD tmp1, tmp2, tmp3, tmp4;
	tmp1 = tmp2 = tmp3 = tmp4 = 0;

	for(int i=0; i<m_iMatrixSize-1; ++i)
	{
		m_kMatrix.SetBitY(i, i+1);
	}

	for(int i=0; i<m_iTestCnt; ++i)
	{
		WORD x, y;
		x = m_pTestData[i<<1];
		y = m_pTestData[(i<<1)+1];
		t1 += x + y;
	}

	t1 = Timestamp();
	for(int i=0; i<m_iTestCnt; ++i)
	{
		WORD x, y;
		x = m_pTestData[i<<1];
		y = m_pTestData[(i<<1)+1];
		tmp3 += x + y;
		m_kMatrix.ClearBitY(x, y);
	}
	t2 = Timestamp();
	T3 = double(t2-t1)/StampsPerSecond()*1000.0;

	t1 = Timestamp();
	for(int i=0; i<m_iTestCnt; ++i)
	{
		WORD x, y;
		x = m_pTestData[i<<1];
		y = m_pTestData[(i<<1)+1];
		tmp1 += x + y;
		m_kMatrix.SetBitX(x, y);
	}
	t2 = Timestamp();
	T1 = double(t2-t1)/StampsPerSecond()*1000.0;

	t1 = Timestamp();
	for(int i=0; i<m_iTestCnt; ++i)
	{
		WORD x, y;
		x = m_pTestData[i<<1];
		y = m_pTestData[(i<<1)+1];
		tmp4 += x + y;
		m_kMatrix.ClearBit(x, y);
	}
	t2 = Timestamp();
	T4 = double(t2-t1)/StampsPerSecond()*1000.0;

	t1 = Timestamp();
	for(int i=0; i<m_iTestCnt; ++i)
	{
		WORD x, y;
		x = m_pTestData[i<<1];
		y = m_pTestData[(i<<1)+1];
		tmp2 += x + y;
		m_kMatrix.SetBitYRet(x, y);
	}
	t2 = Timestamp();
	T2 = double(t2-t1)/StampsPerSecond()*1000.0;

	printf("t1:%f, t2:%f, t3:%f, t4:%f, tmp:%d\n", T1, T2, T3, T4, tmp1+tmp2+tmp3+tmp4);
**/

/**
	TestSwap ttt(1, 2);
	ttt.Test4();
	ttt.Test3();
	ttt.Test1();
	ttt.Test2();
	ttt.Print();
**/
/**
	int i1 = Float2Int(0.0f);
	int i2 = Float2Int(0.1f);
	int i3 = Float2Int(0.5f);
	int i4 = Float2Int(0.9f);
	int i5 = Float2Int(1.0f);
	int i6 = Float2Int(1.1f);
	int i7 = Float2Int(1.5f);
	int i8 = Float2Int(1.6f);
	int i9 = Float2Int(2.0f);
	int i10 = Float2Int(2.1f);
	int i11 = Float2Int(2.5f);
	int i12 = Float2Int(2.9f);
	int i13 = Float2Int(3.0f);
	int i14 = Float2Int(3.5f);
	int i15 = Float2Int(4.0f);
	int i16 = Float2Int(4.5f);
	int i17 = Float2Int(999.9f);


	int iCnt = 100000000;
	float fV, fV1, fV2;
	fV = fV1 = fV2 = 0;
	FvUInt64 t;
	double T1, T2;
	int iV, iV1, iV2;
	iV = iV1 = iV2 = 0;
	T1 = T2 = 0;

	for(int i=0; i<iCnt; ++i)
	{
		float f = float(i);
		fV += f;
		iV += i;
	}
	iV1 = iV;

	t = Timestamp();
	for(int i=0; i<iCnt; ++i)
	{
		float f = float(i);
		fV1 += f;
		iV1 += Float2Int3(f);
	}
	T1 += double(Timestamp() - t)/StampsPerSecond()*1000.0;

	t = Timestamp();
	for(int i=0; i<iCnt; ++i)
	{
		float f = float(i);
		fV2 += f;
		iV2 += Float2Int3(f);
	}
	T2 += double(Timestamp() - t)/StampsPerSecond()*1000.0;

	//t = Timestamp();
	//for(int i=0; i<iCnt; ++i)
	//{
	//	float f = float(i);
	//	fV1 += f;
	//	iV1 += Float2Int3(f);
	//}
	//T1 += double(Timestamp() - t)/StampsPerSecond()*1000.0;

	//t = Timestamp();
	//for(int i=0; i<iCnt; ++i)
	//{
	//	float f = float(i);
	//	fV2 += f;
	//	iV2 += Float2Int(f);
	//}
	//T2 += double(Timestamp() - t)/StampsPerSecond()*1000.0;

	printf("t1:%f, t2:%f, f:%f, d:%d\n", T1, T2, fV+fV1+fV2, iV+iV1+iV2);
**/

/**
	int iCDCnt(0);
	int iCnt = 1;
	FvUInt64 t1 = Timestamp();
	for(int i=0; i<iCnt; ++i)
	{
		iCDCnt = m_pkDemo->UpdateObjects();
	}
	FvUInt64 t2 = Timestamp();
	printf("%f\t%d\n", double(t2-t1)/StampsPerSecond()*1000.0f/iCnt, iCDCnt);
**/
}

void MyApp::MyTick()
{
	class MyListener
	{
	public:
		void OnEnter(void* pkObsData, FvAoIExt* pkExt)
		{

		}
		void OnStand(void* pkObsData, FvAoIExt* pkExt)
		{

		}
		void OnLeave(void* pkObsData, FvAoIExt* pkExt)
		{

		}
	};

	MyListener kListener;

	FvUInt64 t1, t2;
	double T1,T2;
	T1 = T2 = 0;
	FvUInt64 cnt(0);

	t1 = Timestamp();
	for(int i=0; i<m_iObsCnt; ++i)
		//for(int i=0; i<m_iMoveCnt; ++i)
	{
		//float x = rand() * (m_GridMaxX-m_GridMinX) / (RAND_MAX+1) + m_GridMinX;
		//float y = rand() * (m_GridMaxY-m_GridMinY) / (RAND_MAX+1) + m_GridMinY;
		float x = m_pkObs[i].x + m_pkObs[i].vx;
		float y = m_pkObs[i].y + m_pkObs[i].vy;

		if(x < m_GridMinX || m_GridMaxX < x)
			m_pkObs[i].vx = -m_pkObs[i].vx;
		if(y < m_GridMinY || m_GridMaxY < y)
			m_pkObs[i].vy = -m_pkObs[i].vy;

		m_pkObs[i].x = x;
		m_pkObs[i].y = y;

#ifdef FV_DEBUG
		if(rand() > (RAND_MAX>>1))
		{
			cnt += m_kGrid.Move(m_pkObs[i].objHandle, x, y);
			cnt += m_kGrid.Move(m_pkObs[i].obsHandle, x, y);

			m_pkObs[i].x = x;
			m_pkObs[i].y = y;
		}
		else
		{
			m_kGrid.Remove(m_pkObs[i].objHandle);
			m_kGrid.Remove(m_pkObs[i].obsHandle);

			float x = rand() * (m_GridMaxX-m_GridMinX) / (RAND_MAX+1) + m_GridMinX;
			float y = rand() * (m_GridMaxY-m_GridMinY) / (RAND_MAX+1) + m_GridMinY;
			m_pkObs[i].x = x;
			m_pkObs[i].y = y;
			m_pkObs[i].vx = ((rand() +1)%137-68) * m_fVel / 68;
			m_pkObs[i].vy = ((rand() +1)%137-68) * m_fVel / 68;

			//! 人
			if(i < m_iPlayerCnt)
			{
				m_pkObs[i].objHandle = m_kGrid.AddObject(2, x, y, m_fVision, m_pkObs+i);
				m_pkObs[i].obsHandle = m_kGrid.AddObserver(m_pkObs[i].objHandle, 3, x, y, m_fVision, 0.0f, true, m_pkObs+i);
			}
			else//! 怪
			{
				m_pkObs[i].objHandle = m_kGrid.AddObject(1, x, y, m_fVision, m_pkObs+i);
				m_pkObs[i].obsHandle = m_kGrid.AddObserver(m_pkObs[i].objHandle, 2, x, y, m_fVision, 0.0f, false, m_pkObs+i);
			}
		}
#else
		m_kGrid.Move(m_pkObs[i].objHandle, x, y);
		m_kGrid.Move(m_pkObs[i].obsHandle, x, y);
#endif

		//if(i < m_iPlayerCnt)
		//{
		//	m_kGrid.Move(m_pkObs[i].objHandle, x, y);
		//	m_kGrid.Move(m_pkObs[i].obsHandle, x, y);
		//}
		//else
		//{
		//	m_kGrid.Move(m_pkObs[i].objHandle, x, y);
		//}
	}
	t2 = Timestamp();
	T1 = double(t2-t1)/StampsPerSecond()*1000.0;

	FvUInt64 t21, t22, t23;
	FvUInt64 n1,n2,n3;
	t21 = t22 = t23 = 0;
	n1 = n2 = n3 = 0;

#ifdef FV_DEBUG
	t1 = Timestamp();
	for(int i=0; i<m_iPlayerCnt; ++i)
	{
		m_kGrid.UpdateAoI<MyListener,
			&MyListener::OnEnter,
			&MyListener::OnStand,
			&MyListener::OnLeave>
			(m_pkObs[i].obsHandle, &kListener,
			n1, n2, n3);
	}
	for(int i=m_iPlayerCnt; i<m_iObsCnt; ++i)
	{
		m_kGrid.UpdateTrap<MyListener,
			&MyListener::OnEnter,
			&MyListener::OnLeave>
			(m_pkObs[i].obsHandle, &kListener,
			n1, n2, n3);
	}
	t2 = Timestamp();
	T2 = double(t2-t1)/StampsPerSecond()*1000.0;
#else
	t1 = Timestamp();
	for(int i=0; i<m_iPlayerCnt; ++i)
	{
		m_kGrid.UpdateAoI<MyListener,
			&MyListener::OnEnter,
			&MyListener::OnStand,
			&MyListener::OnLeave>
			(m_pkObs[i].obsHandle, &kListener);
	}
	for(int i=m_iPlayerCnt; i<m_iObsCnt; ++i)
	{
		m_kGrid.UpdateTrap<MyListener,
			&MyListener::OnEnter,
			&MyListener::OnLeave>
			(m_pkObs[i].obsHandle, &kListener);
	}
	t2 = Timestamp();
	T2 = double(t2-t1)/StampsPerSecond()*1000.0;
#endif

	if(m_bShowLog)
	{
		printf("t1:%f, t2:%f, n0:%I64u, n1:%I64u, n2:%I64u, n3:%I64u\n", T1, T2,
			cnt, n1, n2, n3);
		//printf("t1:%f, t2:%f, t21:%f, t22:%f, t23:%f, n1:%I64d, n2:%I64d, n3:%I64d\n", T1, T2,
		//	float(double(t21)/double(StampsPerSecond()))*1000.0f, float(double(t22)/double(StampsPerSecond()))*1000.0f, float(double(t23)/double(StampsPerSecond()))*1000.0f,
		//	n1, n2, n3);
	}
}

void MyApp::Draw()
{
	if(!m_bShow)
		return;

	int lineStart(LINE_START);
	int lineStep(LINE_STEP);

	FvVector4 rt(m_GridMinX,m_GridMaxX,m_GridMinY,m_GridMaxY);
	DrawRect(rt, CLR_GREEN);

	if(m_bShowGrid)
	{
		for(FvUInt32 j=0; j<m_kGrid.m_kGrid.m_uiGridIDCntY; ++j)
		{
			for(FvUInt32 i=0; i<m_kGrid.m_kGrid.m_uiGridIDCntX; ++i)
			{
				FvVector4 rt;
				rt.x = m_kGrid.m_kGrid.m_pkBorderX[i] * m_kGrid.m_fCoorScaleInv;
				rt.y = m_kGrid.m_kGrid.m_pkBorderX[i+1] * m_kGrid.m_fCoorScaleInv;
				rt.z = m_kGrid.m_kGrid.m_pkBorderY[j] * m_kGrid.m_fCoorScaleInv;
				rt.w = m_kGrid.m_kGrid.m_pkBorderY[j+1] * m_kGrid.m_fCoorScaleInv;

				DrawRect(rt, CLR_YELLOW);
			}
		}
	}

	FvAoIObject* pkRefs = m_kGrid.m_pkRefPts;
	if(pkRefs)
	{
		int cnt = m_kGrid.m_uiRefPtCntInAxis * m_kGrid.m_uiRefPtCntInAxis;
		float fRate = m_kGrid.m_fCoorScaleInv;
		for(int j=0; j<cnt; ++j)
		{
			FvVector3 pt(pkRefs[j].m_pkPos->m_iX * fRate, pkRefs[j].m_pkPos->m_iY * fRate, .0f);
			DrawPoint(pt, CLR_GREEN, 6.0f);
		}
	}

	for(int i=0; i<m_iObsCnt; ++i)
	{
		bool bInAoI(false);

		FvAoIDLNode* pkBeginNode(NULL);
		FvAoIDLNode* pkEndNode(NULL);
		pkBeginNode = m_pkObs[i].objHandle->m_kRelateMin.m_pkNex;
		pkEndNode = &m_pkObs[i].objHandle->m_kRelateMax;
		//if(pkBeginNode != pkEndNode)
		//	bInAoI = true;
		while(pkBeginNode != pkEndNode)
		{
			if(AOIRELATEOBJ(pkBeginNode)->m_pkExt)
			{
				bInAoI = true;
				break;
			}

			pkBeginNode = pkBeginNode->m_pkNex;
		}

		FvVector3 pt(m_pkObs[i].x, m_pkObs[i].y, .0f);

		if(bInAoI)
		{
			if(i < m_iPlayerCnt)
				DrawPoint(pt, CLR_RED);
			else
				DrawPoint(pt, CLR_WHITE);
		}
		else
		{
			if(i < m_iPlayerCnt)
				DrawPoint(pt, CLR_BLUE);
			else
				DrawPoint(pt, CLR_GREEN);
		}

		FvAoIObserver* pkObs = (FvAoIObserver*)m_pkObs[i].obsHandle;

		if(m_ShowAoI==1 && i==0)
		{
			FvVector4 rt(m_pkObs[i].x-m_fVision,
				m_pkObs[i].x+m_fVision,
				m_pkObs[i].y-m_fVision,
				m_pkObs[i].y+m_fVision);
			DrawRect(rt, CLR_RED);
			DrawCircle(pt, m_fVision, CLR_RED);
		}
		else if(m_ShowAoI == 2)
		{
			DrawCircle(pt, m_fVision, CLR_RED);
		}

		if(m_bShowID)
		{
			FvVector2 p2(pt.x, pt.y);
			int x, y;
			ConvertWorldToScreen(p2, x, y);
			char ids[16] = {0};
			sprintf_s(ids, sizeof(ids), "%d", m_pkObs[i].objID);

			if(bInAoI)
			{
				if(m_bShowID) DrawString(x, y, CLR_WHITE, ids);
			}
			else
			{
				if(m_bShowID) DrawString(x, y, CLR_BLUE, ids);
			}
		}
	}

	//class Visiter
	//{
	//public:
	//	Visiter(MyObs* pkObs):m_pkObs(pkObs) {}
	//	void OnVisit(void* pkObjData)
	//	{
	//		FV_ASSERT(pkObjData);
	//		MyObs* pkObj = (MyObs*)pkObjData;

	//		FvVector3 pt(m_pkObs[pkObj->objID].x, m_pkObs[pkObj->objID].y, .0f);
	//		DrawPoint(pt, CLR_YELLOW);
	//	}

	//	MyObs*	m_pkObs;
	//};
	//Visiter kVisiter(m_pkObs);
	//if(1)
	//{
	//	FvVector3 pt(.0f, .0f, .0f);
	//	float fRadius = 100.0f;
	//	m_kGrid.QueryArea<Visiter, &Visiter::OnVisit>(pt.x, pt.y, fRadius, &kVisiter);
	//	DrawCircle(pt, fRadius, CLR_YELLOW);
	//}


/**
	if(1)
	{
		FvAoIDLNode* pkBeginNode(NULL);
		FvAoIDLNode* pkEndNode(NULL);
		pkBeginNode = m_kObs.handle->m_kRelateMin.m_pkNex;
		pkEndNode = &m_kObs.handle->m_kRelateMax;
		while(pkBeginNode != pkEndNode)
		{
			MyAoIExt* pkExt(NULL);
			if(pkExt = (MyAoIExt*)AOIRELATEOBS(pkBeginNode)->m_pkExt)
			{
				DrawString(0, lineStart, CLR_WHITE, "%d -> %d", m_kObs.id, pkExt->id);
				lineStart += lineStep;
			}
			else
			{
				break;
			}

			pkBeginNode = pkBeginNode->m_pkNex;
		}
	}

	int iCellCnt = m_kGrid.m_uiCellCntX * m_kGrid.m_uiCellCntY;
	for(int i=0; i<iCellCnt; ++i)
	{
		FvVector4 rt(m_kGrid.m_pkCells[i].m_fCellMinX,
			m_kGrid.m_pkCells[i].m_fCellMaxX,
			m_kGrid.m_pkCells[i].m_fCellMinY,
			m_kGrid.m_pkCells[i].m_fCellMaxY);

		DrawRect(rt, CLR_YELLOW);
	}

	for(int i=0; i<iCellCnt; ++i)
	{
		int iRefCnt = m_kGrid.m_pkCells[i].m_uiRefCntX * m_kGrid.m_pkCells[i].m_uiRefCntX;
		FvAoIObject* pkRefs = m_kGrid.m_pkCells[i].m_pkRefs;
		for(int j=0; j<iRefCnt; ++j)
		{
			FvVector3 pt(pkRefs[j].m_fX, pkRefs[j].m_fY, .0f);
			DrawPoint(pt, CLR_GREEN);
		}
	}

	for(int i=0; i<m_iObjCnt; ++i)
	{
		bool bInAoI(false);

		FvAoIDLNode* pkBeginNode(NULL);
		FvAoIDLNode* pkEndNode(NULL);
		pkBeginNode = m_kObj[i].handle->m_kRelateMin.m_pkNex;
		pkEndNode = &m_kObj[i].handle->m_kRelateMax;
		while(pkBeginNode != pkEndNode)
		{
			if(AOIRELATEOBJ(pkBeginNode)->m_pkExt)
			{
				bInAoI = true;
				break;
			}

			pkBeginNode = pkBeginNode->m_pkNex;
		}

		FvVector3 pt(m_kObj[i].x, m_kObj[i].y, .0f);

		if(bInAoI)
		{
			DrawPoint(pt, CLR_WHITE);
		}
		else
		{
			DrawPoint(pt, CLR_BLUE);
		}

		if(m_bShowID)
		{
			FvVector2 p2(pt.x, pt.y);
			int x, y;
			ConvertWorldToScreen(p2, x, y);
			char ids[16] = {0};
			sprintf_s(ids, sizeof(ids), "%d", m_kObj[i].id);

			if(bInAoI)
			{
				if(m_bShowID) DrawString(x, y, CLR_WHITE, ids);
			}
			else
			{
				if(m_bShowID) DrawString(x, y, CLR_BLUE, ids);
			}
		}
	}

	if(1)
	{
		FvAoIObserver* pkObs = (FvAoIObserver*)m_kObs.handle;
		FvVector4 rt(pkObs->m_fX-pkObs->m_fVision,
						pkObs->m_fX+pkObs->m_fVision,
						pkObs->m_fY-pkObs->m_fVision,
						pkObs->m_fY+pkObs->m_fVision);
		DrawRect(rt, CLR_RED);

		FvVector3 pt(m_kObs.x, m_kObs.y, .0f);

		DrawCircle(pt, pkObs->m_fVision, CLR_RED);

		DrawPoint(pt, CLR_RED);

		if(m_bShowID)
		{
			FvVector2 p2(pt.x, pt.y);
			int x, y;
			ConvertWorldToScreen(p2, x, y);
			char ids[16] = {0};
			sprintf_s(ids, sizeof(ids), "%d", m_kObs.id);
			DrawString(x, y, CLR_RED, ids);
		}
	}
**/

/**
	int iCellCnt = (int)m_kMins.size()/2;
	for(int i=0; i<iCellCnt; ++i)
	{
		FvVector4 rt(m_kMins[i*2+0],
					m_kMaxs[i*2+0],
					m_kMins[i*2+1],
					m_kMaxs[i*2+1]);

		DrawRect(rt, CLR_GREEN);
	}

	std::vector<aabb> boxes;
	std::vector<int> colors;
	m_pkDemo->GetDrawElements( boxes, colors );

	for(int i=0; i<(int)boxes.size(); ++i)
	{
		FvVector4 rt(boxes[i].pos[0],
					boxes[i].pos[0]+boxes[i].width[0],
					boxes[i].pos[1],
					boxes[i].pos[1]+boxes[i].width[1]);
		if(colors[i] == 0)		//! blue
			DrawRect(rt, CLR_BLUE);
		else					//! red
			DrawRect(rt, CLR_RED);
	}
**/
}

void MyApp::OnKeyDown(int keyVal, bool bRep)
{
	float step = 5.0f;

	//float x = m_kObs.x;
	//float y = m_kObs.y;

	if(keyVal == KV_n)
	{
		if(m_bRunStep)
			MyTick();
			//m_pkDemo->UpdateObjects();
	}
	else if(keyVal == KV_p)
	{
		m_bRunStep = !m_bRunStep;
	}
	else if(keyVal == KV_i)
	{
		m_bShowID = !m_bShowID;
	}
	else if(keyVal == KV_g)
	{
		m_bShow = !m_bShow;
	}
	else if(keyVal == KV_l)
	{
		m_bShowLog = !m_bShowLog;
	}
	else if(keyVal == KV_o)
	{
		++m_ShowAoI;
		if(m_ShowAoI > 2)
			m_ShowAoI -= 3;
	}
	else if(keyVal == KV_c)
	{
		m_bShowGrid = !m_bShowGrid;
	}
/**
	else if(keyVal == KV_a)//! 左
	{
		x -= step;
		if(m_kGrid.Move(m_kObs.handle, x, y))
		{
			m_kObs.x = x;
			m_kObs.y = y;
		}
	}
	else if(keyVal == KV_d)//! 右
	{
		x += step;
		if(m_kGrid.Move(m_kObs.handle, x, y))
		{
			m_kObs.x = x;
			m_kObs.y = y;
		}
	}
	else if(keyVal == KV_w)//! 上
	{
		y += step;
		if(m_kGrid.Move(m_kObs.handle, x, y))
		{
			m_kObs.x = x;
			m_kObs.y = y;
		}
	}
	else if(keyVal == KV_s)//! 下
	{
		y -= step;
		if(m_kGrid.Move(m_kObs.handle, x, y))
		{
			m_kObs.x = x;
			m_kObs.y = y;
		}
	}
**/
}



App* CreateApp()
{
	if(!g_pkApp)
		g_pkApp = new MyApp();
	return g_pkApp;
}

void DestroyApp()
{
	if(g_pkApp)
	{
		delete g_pkApp;
		g_pkApp = NULL;
	}
}


p2SquareSymmetricMatrix::p2SquareSymmetricMatrix()
:m_pBits(NULL),m_Size(0)
{
}

p2SquareSymmetricMatrix::p2SquareSymmetricMatrix(WORD size)
:m_pBits(NULL),m_Size(0)
{
	Init(size);
}

p2SquareSymmetricMatrix::~p2SquareSymmetricMatrix()
{
	if(m_pBits)
	{
		delete [] m_pBits;
		m_pBits = NULL;
	}
	//P2_SAFE_DELETE_ARRAY(m_pBits);
}

inline DWORD BitsToDwords(DWORD bits)
{
	return (bits>>5) + ((bits&31) ? 1 : 0);
}

bool p2SquareSymmetricMatrix::Init(WORD size)
{
	m_Size = BitsToDwords((size+1)*size);

	if(m_pBits)
	{
		delete [] m_pBits;
		m_pBits = NULL;
	}
	//P2_SAFE_DELETE_ARRAY(m_pBits);
	m_pBits = new DWORD[m_Size];

	ClearAll();
	return true;
}

//const int g_iItemSize = 16;
const int g_iItemSize = 24;
struct MyMemCell
{
	MyMemCell*	pCell;
};
MyMemCell* g_pFreeCell = NULL;
unsigned int g_iMaxCnt;
unsigned int g_iFreeCnt;
char* g_pBuf;
char* g_pIdx;
char* g_pEnd;

void InitMyMemAlloc(size_t cnt)
{
	g_iMaxCnt = cnt;
	g_iFreeCnt = cnt;
	g_pIdx = g_pBuf = new char[g_iItemSize*cnt];
	g_pEnd = g_pIdx + g_iItemSize*cnt -1;
	g_pFreeCell = NULL;
}

void UnInitMyMemAlloc()
{
	if(g_pBuf)
	{
		delete [] g_pBuf;
		g_pBuf = NULL;
	}
}

void* MyMalloc(size_t size, size_t cnt)
{
	if(cnt == 1)
	{
		if(!g_iFreeCnt)
		{
			return NULL;
		}

		char* pCell;
		if(g_pIdx > g_pEnd)
		{
			pCell = (char*)g_pFreeCell;
			g_pFreeCell = g_pFreeCell->pCell;
		}
		else
		{
			pCell = g_pIdx;
			g_pIdx += g_iItemSize;
		}

		--g_iFreeCnt;
		return pCell;
	}
	else
	{
		return malloc(size * cnt);
	}
}

void MyFree(void* p, size_t cnt)
{
	if(cnt == 1)
	{
		MyMemCell* pCell = (MyMemCell*)p;
		pCell->pCell = g_pFreeCell;
		g_pFreeCell = pCell;
		++g_iFreeCnt;
	}
	else
	{
		free(p);
	}
}

RelateNode* MyApp::FindNode(unsigned short id)
{
	RelateNode* pNode(m_kNodeHead.pNex);
	while(pNode->objectID!=id && pNode!=&m_kNodeTail)
		pNode = pNode->pNex;
	return pNode->objectID==id ? pNode : NULL;
}

int MyApp::TraversalList()
{
	int sum(0);
	RelateNode* pNode(m_kNodeHead.pNex);
	while(pNode != &m_kNodeTail)
	{
		sum += pNode->objectID;
		pNode = pNode->pNex;
	}
	return sum;
}

int MyApp::TraversalArray()
{
	//int sum(0);
	//int sArray[10];
	////for(int i=0; i<m_iNodeCnt; ++i)
	////{
	////	sArray[i] = i;
	////}
	//for(int i=0; i<m_iNodeCnt; ++i)
	//{
	//	sum += sArray[i];// + sArray[i+1];
	//}
	//return sum;

	int sum(0);
	for(int i=0; i<m_iNodeCnt; ++i)
	{
		sum += m_pkArray[i];
	}
	return sum;
}


int MyApp::TraversalArray1()
{
	int sum(0);
	int* pHead = m_pkArray;
	int* pEnd = m_pkArray + m_iNodeCnt;
	for(; pHead!=pEnd; ++pHead)
	{
		sum += *pHead;
	}
	return sum;
}


int	MyApp::TraversalMyArray()
{
	int sum(0);
	int cnt = m_kMyIntArray.size();
	for(int i=0; i<cnt; ++i)
	{
		sum += m_kMyIntArray[i];
	}
	return sum;
}

int	MyApp::TraversalMyArray1()
{
	int sum(0);
	FvIndexVector<int>::iterator itrB = m_kMyIntArray.begin();
	FvIndexVector<int>::iterator itrE = m_kMyIntArray.end();
	for(; itrB!=itrE; ++itrB)
	{
		sum += *itrB;
	}
	return sum;
}

int	MyApp::TraversalMyArray2()
{
	int sum(0);
	int cnt = m_kMyIntArray.size();
	int* pp = m_kMyIntArray.GetHead();
	for(int i=0; i<cnt; ++i)
	{
		sum += pp[i];
	}
	return sum;

	//int sum(0);
	//int cnt = m_kMyIntArray.size();
	//int** pp = m_kMyIntArray.GetHead();
	//for(int i=0; i<cnt; ++i)
	//{
	//	sum += *pp[i];
	//}
	//return sum;
}

int	MyApp::TraversalMyArray3()
{
	int sum(0);
	int cnt = m_kMyIntArray.size();
	int* pp = m_kMyIntArray.GetHead();
	int* ppE = pp + cnt;
	for(; pp<ppE; ++pp)
	{
		sum += *pp;
	}
	return sum;

	//int sum(0);
	//int cnt = m_kMyIntArray.size();
	//int** pp = m_kMyIntArray.GetHead();
	//int** ppE = pp + cnt;
	//for(; pp<ppE; ++pp)
	//{
	//	sum += **pp;
	//}
	//return sum;
}


int MyApp::TestRefList()
{
	////! 外置迭代器
	//FvRefList<int> kList;
	//const FvUInt32 uiCNT = 10000;
	//FvRefNode<int> kNodeList[uiCNT];

	////! 插入
	//for (FvUInt32 uiIdx = 0; uiIdx < uiCNT; ++uiIdx)
	//{
	//	kList.AttachBack(kNodeList[uiIdx]);
	//}

	//FV_ASSERT(kList.Size() == uiCNT);

	////! 遍历
	//int sum(0);
	//FvRefList<int>::iterator iter = kList->GetHead();
	//while (!kList->IsEnd(iter))
	//{
	//	int uiValue = (*iter).m_Content;
	//	FvRefList<int>::Next(iter);		

	//	///<使用>
	//	sum += uiValue;
	//	///</使用>
	//}
	////! 删除
	//for (FvUInt32 uiIdx = 0; uiIdx < uiCNT; ++uiIdx)
	//{
	//	kNodeList[uiIdx].Detach();
	//}

	return 0;//sum;
}


