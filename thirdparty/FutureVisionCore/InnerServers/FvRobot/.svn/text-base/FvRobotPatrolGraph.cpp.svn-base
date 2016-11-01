#include "FvRobotPatrolGraph.h"

#include <FvDebug.h>

#include <OgreResourceGroupManager.h>
#include <OgreLogManager.h>
#include <OgreArchiveManager.h>
#include <OgreFileSystem.h>


float RandInRange(float fMin, float fMax)
{
	if(fMin >= fMax)
		return fMin;
	return fMin + (fMax-fMin)*float(rand())/float(RAND_MAX);
}

Node::Node()
{
}


bool Node::Init1(int iIndex, FvXMLSectionPtr spSection,
				 const Properties& kDefaultProps)
{
	m_kProps = kDefaultProps;

	if(!spSection) return false;

	m_iID = iIndex;

	m_kPosition = spSection->ReadVector3("pos");
	m_kName = spSection->ReadString("name");

	m_kProps.Init(spSection);

	if(m_kName.empty())
	{
		char buf[10];
		_snprintf(buf, 10, "%d", iIndex);
		m_kName = buf;
	}

	return true;
}

bool Node::Init2(const NodeMap& kMap, FvXMLSectionPtr spSection)
{
	int iNumEdges = 0;
	FvXMLSectionPtr spEdges = spSection->OpenSection("edges");

	if(spEdges)
	{
		iNumEdges = spEdges->CountChildren();
	}
	else
	{
		FV_WARNING_MSG("Graph::Init2: No edges section\n");
	}

	m_kToNodes.resize(iNumEdges);

	for(int i=0; i<iNumEdges; ++i)
	{
		FvString kDstName = spEdges->OpenChild(i)->AsString();
		NodeMap::const_iterator iter = kMap.find(kDstName);

		if(iter == kMap.end())
		{
			FV_ERROR_MSG("Graph::Init2: Could not find %s for %s\n",
					kDstName.c_str(), m_kName.c_str());
			return false;
		}

		m_kToNodes[i] = iter->second;
	}

	return true;
}


float Node::FlatDistSqrTo(const FvVector3& kPos) const
{
	float x = m_kPosition.x - kPos.x;
	float z = m_kPosition.z - kPos.z;
	return x*x + z*z;
}


float Node::FlatDistTo(const FvVector3& kPos) const
{
	return sqrtf(this->FlatDistSqrTo(kPos));
}


int Node::NextNode() const
{
	if(!m_kToNodes.empty())
	{
		return m_kToNodes[rand() % m_kToNodes.size()];
	}
	else
	{
		return m_iID;
	}
}


int Node::RandomisePosition(FvVector3& kPos, const Graph& kGraph) const
{
	if(m_kToNodes.empty())
	{
		kPos = m_kPosition;
	   	return m_iID;
	}

	int iNextNode = m_kToNodes[rand() % m_kToNodes.size()];
	kPos = m_kPosition;
	kPos += (kGraph.GetNode(iNextNode)->m_kPosition - m_kPosition) *
		(float(rand())/float(RAND_MAX));
	return iNextNode;
}


FvVector3 Node::GenerateDestination() const
{
	float fRadius = m_kProps.Radius();

	return FvVector3(RandInRange(m_kPosition.x-fRadius, m_kPosition.x+fRadius),
			RandInRange(m_kPosition.y-fRadius, m_kPosition.y+fRadius),
			0.0f);
}


float Node::GenerateStayTime() const
{
	return RandInRange(m_kProps.MinStay(), m_kProps.MaxStay());
}


float Node::GenerateSpeed(float fSpeed) const
{
	if(m_kProps.MinSpeed() <= 0.f || m_kProps.MaxSpeed() <= 0.f) return fSpeed;

	return RandInRange(m_kProps.MinSpeed(), m_kProps.MaxSpeed());
}


void Node::Properties::Init(FvXMLSectionPtr spSection)
{
	if(!spSection) return;

	m_fMinStay = spSection->ReadFloat("minStay", m_fMinStay);
	m_fMaxStay = spSection->ReadFloat("maxStay", m_fMaxStay);
	m_fRadius = spSection->ReadFloat("radius", m_fRadius);
	m_fMinSpeed = spSection->ReadFloat("minSpeed", m_fMinSpeed);
	m_fMaxSpeed = spSection->ReadFloat("maxSpeed", m_fMaxSpeed);
}


Graph::Graph()
{
}


bool Graph::Init(FvXMLSectionPtr spSection)
{
	FvXMLSectionPtr spNodes = spSection->OpenSection("nodes");

	if(!spNodes)
	{
		FV_ERROR_MSG("Graph::Init: No nodes section\n");
		return false;
	}

	Node::Properties kNodeDefaults;
	kNodeDefaults.Init(spSection->OpenSection("nodeDefaults"));

	int iNumNodes = spNodes->CountChildren();
	m_kNodes.resize(iNumNodes);

	Node::NodeMap kNodeMap;

	for(int i=0; i<iNumNodes; ++i)
	{
		FvXMLSectionPtr spNode = spNodes->OpenChild(i);

		if(!m_kNodes[i].Init1(i, spNode.GetObject(), kNodeDefaults))
		{
			FV_ERROR_MSG("Graph::Init: Failed to initialise node %d\n", i);
			return false;
		}

		if(!kNodeMap.insert(std::make_pair(m_kNodes[i].Name(), i)).second)
		{
			FV_ERROR_MSG("Graph::Init: Node %s is a duplicate\n",
					m_kNodes[i].Name().c_str());
			return false;
		}
	}

	for(int i=0; i<iNumNodes; ++i)
	{
		FvXMLSectionPtr spNode = spNodes->OpenChild(i);

		if(!m_kNodes[i].Init2(kNodeMap, spNode))
		{
			FV_ERROR_MSG("Graph::Init: Failed to initialise edges for node %d\n",
					i);
			return false;
		}
	}

	return true;
}

void Graph::Init(float fScalePos, float fScaleSpeed)
{
	for(int i=0; i<(int)m_kNodes.size(); ++i)
	{
		m_kNodes[i].m_kPosition *= fScalePos;
		m_kNodes[i].m_kProps.m_fRadius *= fScalePos;
		m_kNodes[i].m_kProps.m_fMinSpeed *= fScaleSpeed;
		m_kNodes[i].m_kProps.m_fMaxSpeed *= fScaleSpeed;
	}
}

int Graph::FindClosestNode(const FvVector3& kPosition) const
{
	if(m_kNodes.empty())
		return -1;

	float fBestDist = m_kNodes[0].FlatDistSqrTo(kPosition);
	int iBestIndex = 0;

	int iSize = m_kNodes.size();

	for(int i=1; i<iSize; ++i)
	{
		float fDist = m_kNodes[i].FlatDistSqrTo(kPosition);

		if(fDist < fBestDist)
		{
			fBestDist = fDist;
			iBestIndex = i;
		}
	}

	return iBestIndex;
}


const Node* Graph::GetNode(int iIndex) const
{
	return &m_kNodes[iIndex];
}


Graphs::Graphs()
{
}


Graph* Graphs::GetGraph(const FvString& kGraphName,
	float fScalePos, float fScaleSpeed)
{
	char dnbuf[1024];
	if(kGraphName.size() > sizeof(dnbuf)-64) return NULL;

	sprintf_s(dnbuf, sizeof(dnbuf), "%s,%f,%f",
		kGraphName.c_str(), fScalePos, fScaleSpeed);
	FvString kDecoratedName = dnbuf;

	Map::iterator iter = m_kGraphs.find(kDecoratedName);
	if(iter != m_kGraphs.end())
	{
		return &iter->second;
	}
	else
	{
		iter = m_kRawGraphs.find(kGraphName);
		if(iter != m_kRawGraphs.end())
		{
			Graph& kRawGraph = iter->second;
			Graph& kGraph = m_kGraphs.insert(std::make_pair(kDecoratedName, kRawGraph)).first->second;
			kGraph.Init(fScalePos, fScaleSpeed);
			return &kGraph;
		}
		else
		{
			FvString kFullName = "Robot/" + kGraphName;
			Ogre::DataStreamPtr spDataStream = Ogre::ResourceGroupManager::getSingleton().openResource(kFullName);
			if(spDataStream.isNull())
				return NULL;

			FvXMLSectionPtr spSection = FvXMLSection::OpenSection(spDataStream);
			if(spSection == NULL)
				return NULL;

			Graph& kRawGraph = m_kRawGraphs[kGraphName];
			if(kRawGraph.Init(spSection))
			{
				Graph& kGraph = m_kGraphs.insert(std::make_pair(kDecoratedName, kRawGraph)).first->second;
				kGraph.Init(fScalePos, fScaleSpeed);
				return &kGraph;
			}
			else
			{
				m_kRawGraphs.erase(kGraphName);
				return NULL;
			}
		}
	}
}

