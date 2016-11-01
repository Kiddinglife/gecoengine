//{future header message}
#ifndef __FvPatrolGraph_H__
#define __FvPatrolGraph_H__


#include <FvXMLSection.h>
#include <vector>
#include <map>


class Graph;

class Node
{
public:
	class Properties
	{
	public:
		Properties():
			m_fMinStay( 0.f ),
			m_fMaxStay( 0.f ),
			m_fRadius( 0.f ),
			m_fMinSpeed( 0.f ),
			m_fMaxSpeed( 0.f )
		{}

		void Init(FvXMLSectionPtr spSection);

		float MinStay() const	{ return m_fMinStay; }
		float MaxStay() const	{ return m_fMaxStay; }
		float Radius() const	{ return m_fRadius; }
		float MinSpeed() const	{ return m_fMinSpeed; }
		float MaxSpeed() const	{ return m_fMaxSpeed; }

	private:
		float m_fMinStay;
		float m_fMaxStay;
		float m_fRadius;
		float m_fMinSpeed;
		float m_fMaxSpeed;

		friend class Graph;
	};

	Node();

	typedef std::map<FvString,int> NodeMap;

	bool Init1(int iIndex, FvXMLSectionPtr spSection,
			const Properties& kDefaultProps);
	bool Init2(const NodeMap& kMap, FvXMLSectionPtr spSection);

	float FlatDistTo(const FvVector3& kPos) const;
	float FlatDistSqrTo(const FvVector3& kPos) const;

	int NextNode() const;
	int RandomisePosition(FvVector3& kPos, const Graph& kGraph) const;

	FvVector3 GenerateDestination() const;
	float GenerateStayTime() const;
	float GenerateSpeed(float fSpeed) const;

	const FvString& Name() const	{ return m_kName; }
	const Properties& Props() const	{ return m_kProps; }

	const FvVector3& Position() const	{ return m_kPosition; }

private:
	typedef std::vector<int> Adjacencies;
	Adjacencies	m_kToNodes;
	FvVector3	m_kPosition;
	int			m_iID;
	FvString	m_kName;
	Properties	m_kProps;

	friend class Graph;
};


class Graph
{
public:
	Graph();
	bool Init(FvXMLSectionPtr spSection);
	void Init(float fScalePos, float fScaleSpeed);

	int FindClosestNode(const FvVector3& kPosition) const;
	const Node* GetNode(int iIndex) const;

	int Size() const { return m_kNodes.size(); }

private:
	typedef std::vector<Node> Nodes;
	Nodes m_kNodes;
};


class Graphs
{
public:
	Graphs();
	Graph* GetGraph(const FvString& kGraphName,
		float fScalePos, float fScaleSpeed);

private:
	typedef std::map<FvString, Graph> Map;
	Map	m_kRawGraphs;
	Map m_kGraphs;
};



#endif//__FvPatrolGraph_H__