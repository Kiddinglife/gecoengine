#include <FvDebug.h>
#include "FvRobotMovementController.h"
#include "FvRobotClientApp.h"
#include "FvRobotMainApp.h"


FV_DECLARE_DEBUG_COMPONENT2( "Bots", 0 );

Graphs s_kGraphs;

FvRobotMovementFactory::FvRobotMovementFactory( const char * name )
{
	FvRobotMainApp::AddFactory( name, *this );
}


FvGraphTraverser::FvGraphTraverser(const Graph& kGraph, float& fSpeed,
								   FvVector3& kStartPosition, bool bRandomise, bool bSnap)
:m_iDestinationNode(-1)
,m_fStayTime(-1.f)
,m_kGraph(kGraph)
{
	if(bRandomise)
	{
		m_iDestinationNode = rand() % m_kGraph.Size();
	}
	else
	{
		m_iDestinationNode = m_kGraph.FindClosestNode(kStartPosition);
	}

	const Node* pNode = m_kGraph.GetNode(m_iDestinationNode);
	if(bSnap)
	{
		m_iDestinationNode = pNode->RandomisePosition(kStartPosition, m_kGraph);
		pNode = m_kGraph.GetNode(m_iDestinationNode);
	}

	m_kDestinationPos = pNode->GenerateDestination();
	fSpeed = pNode->GenerateSpeed(fSpeed);
}


bool FvGraphTraverser::NextStep(float& fSpeed, float dTime,
								FvVector3& kPos, FvDirection3& kDir)
{
	const Node* pNode = m_kGraph.GetNode(m_iDestinationNode);

	if(m_fStayTime >= 0.f)
	{
		m_fStayTime -= dTime;

		if(m_fStayTime <= 0.f)
		{
			m_iDestinationNode = pNode->NextNode();
			pNode = m_kGraph.GetNode(m_iDestinationNode);
			m_kDestinationPos = pNode->GenerateDestination();
			m_fStayTime = -1.f;
			fSpeed = pNode->GenerateSpeed(fSpeed);
		}
	}

	float fDistance = fSpeed * dTime;

	FvVector3 kDirVec = m_kDestinationPos - kPos;
	float fLength = kDirVec.Length();

	if(fLength < fDistance)
	{
		if(m_fStayTime < 0.f)
		{
			m_fStayTime = pNode->GenerateStayTime();
		}

		m_kDestinationPos = pNode->GenerateDestination();
	}
	else
	{
		kPos += (fDistance/fLength) * kDirVec;
		kDir.SetYaw(kDirVec.GetYaw());
	}

	return true;
}


class FvPatrolFactory : public FvRobotMovementFactory
{
public:
	FvPatrolFactory() : FvRobotMovementFactory( "Patrol" )
	{
	}
	virtual ~FvPatrolFactory() {}

	FvRobotMovementController *
		Create(const FvString& kData, float& fSpeed, FvVector3& kStartPos)
	{
		FvString kGraphName;
		bool bRandomise = false, bSnap = false;
		float fScalePos = 1.f, fScaleSpeed = 1.f;

		int iArgBeg = 0;
		int iArgNum = 0;
		do
		{
			int iArgEnd = kData.find_first_of(',', iArgBeg);
			if(iArgEnd >= (int)kData.size() || iArgEnd == -1)
				iArgEnd = kData.size();
			FvString kArgStr = kData.substr(iArgBeg, iArgEnd-iArgBeg);

			if(iArgNum == 0)
			{
				kGraphName = kArgStr;
			}
			else
			{
				if(kArgStr == "random")
					bRandomise = true;
				else if(kArgStr == "snap")
					bSnap = true;
				else if(kArgStr.substr(0, 9) == "scalePos=")
					fScalePos = atof(kArgStr.substr(9).c_str());
				else if(kArgStr.substr(0, 11) == "scaleSpeed=")
					fScaleSpeed = atof(kArgStr.substr(11).c_str());
				else
				{
					FV_ERROR_MSG("PatrolFactory::Create: "
						"Unknown argument '%s'\n", kArgStr.c_str());
				}
			}
			iArgBeg = iArgEnd+1;
			++iArgNum;
		}
		while(iArgBeg < (int)kData.size());

		Graph* pkGraph = s_kGraphs.GetGraph(kGraphName, fScalePos, fScaleSpeed);
		if(pkGraph)
		{
			return new FvGraphTraverser(*pkGraph, fSpeed, kStartPos, bRandomise, bSnap);
		}

		FV_ERROR_MSG("Unable to load Graph '%s'", kGraphName.c_str());
		return NULL;
	}
};

FvPatrolFactory s_kPatrolFactory;


const int FvZigzagGraphTraverser::DEFAULT_CORRIDOR_WIDTH = 50;

static float frand(float f) { return f * (float)rand()/RAND_MAX; }

FvZigzagGraphTraverser::FvZigzagGraphTraverser(const Graph& kGraph, float& fSpeed,
											   FvVector3& kStartPosition,
											   float fCorridorWidth)
:FvGraphTraverser(kGraph, fSpeed, kStartPosition, true, true)
,m_fCorridorWidth(fCorridorWidth)
{
	m_kSourcePos = kStartPosition;
	SetZigzagPos(kStartPosition);
}

void FvZigzagGraphTraverser::SetZigzagPos(const FvVector3& kCurrPos)
{
	FvVector3 a(m_kSourcePos), b(m_kDestinationPos), p(kCurrPos);

	float x,y;
	FvVector3 poi;
	if(b.x != a.x)
	{
		float m = (b.y - a.y)/(b.x - a.x);
		x = (p.x + m*p.y + m*m*a.x - m*a.y) / (m*m + 1);
		y = m * (x - a.x) + a.y;
		poi = FvVector3(x, y, kCurrPos.z);
	}
	else
	{
		poi = FvVector3( a.x, p.y, kCurrPos.z);
	}

	if((m_kDestinationPos - poi).Length() < 2 * m_fCorridorWidth)
	{
		m_kZigzagPos = m_kDestinationPos;
		return;
	}

	FvVector3 along = b - a; along.Normalise();
	FvVector3 across = poi - p; across.Normalise();

	float alongAmt = 2 * frand(m_fCorridorWidth);
	float acrossAmt = frand(m_fCorridorWidth);
	m_kZigzagPos = (poi + along*alongAmt + across*acrossAmt);
}

bool FvZigzagGraphTraverser::NextStep(float& fSpeed, float dTime,
									  FvVector3& kPos, FvDirection3& kDir)
{
	float distance = fSpeed * dTime;

	const Node* pNode = m_kGraph.GetNode(m_iDestinationNode);

	FvVector3 destVec = m_kDestinationPos - kPos;
	FvVector3 zzVec = m_kZigzagPos - kPos;

	if(m_fStayTime > 0)
	{
		m_fStayTime -= dTime;

		if(m_fStayTime <= 0)
		{
			m_kSourcePos = pNode->Position();
			m_iDestinationNode = pNode->NextNode();
			pNode = m_kGraph.GetNode(m_iDestinationNode);
			m_kDestinationPos = pNode->GenerateDestination();
			SetZigzagPos(kPos);
		}
		else if(zzVec.Length() < distance)
		{
			m_kZigzagPos = pNode->GenerateDestination();
		}
	}
	else if(destVec.Length() < distance)
	{
		m_fStayTime = pNode->GenerateStayTime();
		m_kZigzagPos = pNode->GenerateDestination();
	}
	else if(zzVec.Length() < distance)
	{
		SetZigzagPos(kPos);
	}

	zzVec = m_kZigzagPos - kPos;
	kPos += zzVec * (distance / zzVec.Length());
	kDir.SetYaw(zzVec.GetYaw());

	return true;
}


class FvZigzagPatrolFactory : public FvRobotMovementFactory
{
public:
	FvZigzagPatrolFactory() : FvRobotMovementFactory( "ZigzagPatrol" )
	{
	}
	virtual ~FvZigzagPatrolFactory() {}

	FvRobotMovementController* Create(const FvString& kData,
		float& fSpeed, FvVector3& kStartPos)
	{
		FvString kGraphName;
		float fCorridorWidth = -1;

		int iArgPos = kData.find_first_of(',');
		if(iArgPos < (int)kData.size())
		{
			kGraphName = kData.substr(0, iArgPos);
			FvString iArg2 = kData.substr(iArgPos+1);
			fCorridorWidth = atof(iArg2.c_str());
		}
		else
		{
			kGraphName = kData;
		}

		Graph* pkGraph = s_kGraphs.GetGraph(kGraphName, 1.f, 1.f);
		if(!pkGraph)
		{
			FV_ERROR_MSG("Unable to load %s", kGraphName.c_str());
			return NULL;
		}
		else if(fCorridorWidth > 0)
			return new FvZigzagGraphTraverser(*pkGraph, fSpeed, kStartPos,
											 fCorridorWidth);
		else
			return new FvZigzagGraphTraverser(*pkGraph, fSpeed, kStartPos);
	}
};

FvZigzagPatrolFactory s_patrolFactory;


