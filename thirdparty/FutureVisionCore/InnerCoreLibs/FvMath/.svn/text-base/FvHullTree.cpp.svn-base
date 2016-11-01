//{future header message}
#include "FvHullTree.h"

FvVectorNoDestructor<FvHullTree::Traversal::StackElt>
	FvHullTree::Traversal::m_kStack;

//----------------------------------------------------------------------------
FvHullContents::FvHullContents() : m_pkNext(NULL)
{

}
//----------------------------------------------------------------------------
FvHullContents::~FvHullContents()
{

}
//----------------------------------------------------------------------------
FvHullTree::FvHullTree() : m_kDivider(FvVector3(0,0,0), 0), m_pkFront(NULL),
	m_pkBack(NULL), m_iMarked(0), m_pkNextMarked(NULL)
{

}
//----------------------------------------------------------------------------
FvHullTree::FvHullTree(const FvPlane& kPlane, FvHullTree*& pkFirstMarked) :
	m_kDivider(kPlane), m_pkFront(NULL), m_pkBack(NULL),
	m_iMarked(2 | 65536), m_pkNextMarked(pkFirstMarked)
{
	pkFirstMarked = this;
}
//----------------------------------------------------------------------------
FvHullTree::~FvHullTree()
{
	if(m_pkFront)
		delete m_pkFront;
	if(m_pkBack)
		delete m_pkBack;
}
//----------------------------------------------------------------------------
void FvHullTree::Add(const FvHullBorder& kBorder, const FvHullContents* kTag)
{
	FvHullTree* pkFirstMark = (FvHullTree*)1;

	FvHullBorder::const_iterator kBendIter = kBorder.end();
	for(FvHullBorder::const_iterator it = kBorder.begin(); it != kBendIter;
		it++)
	{
		FvPortal2D kOutline;

		std::vector<FvLine>	kLines;

		for(FvHullBorder::const_iterator bt = kBorder.begin();
			bt != kBendIter; bt++)
		{
			if(bt == it) continue;

			FvLine kLine = it->Intersect(*bt);

			if(kLine.Normal() != FvVector2::ZERO)
				kLines.push_back(kLine);
		}

		for(unsigned int i = 0; i < kLines.size(); i++)
		{
			float fMinDist = -1000000.f;
			float fMaxDist =  1000000.f;
			for(unsigned int j = 0; j < kLines.size(); j++)
			{
				if(j == i) continue;

				if(kLines[i].IsParallel(kLines[j]))
				{
					if(kLines[i].Normal().DotProduct(
						kLines[j].Normal()) < 0.0f)
						continue;
					if(kLines[i].IsInFrontOf(kLines[j].Param(0.0f)))
						continue;

					fMinDist = 1;
					fMaxDist = -1;
					break;
				}

				float iDist = kLines[i].Intersect(kLines[j]);

				if(kLines[i].IsMinCutter(kLines[j]))
				{
					if(fMinDist < iDist)
					{
						fMinDist = iDist;
					}
				}
				else
				{
					if(fMaxDist > iDist)
					{
						fMaxDist = iDist;
					}
				}
			}

			if(fMinDist >= fMaxDist)	continue;

			FvVector2 kLineEnd = kLines[i].Param(fMaxDist);
			kOutline.AddPoint(kLineEnd);
		}

		Portal3D kOutline3D(*it, kOutline);

		AddPlane(*it, kOutline3D, pkFirstMark);
	}

	int iBack = 0;
	int iFront = 0;
	FvHullTree* pkHullTreeNM;
	
	for(FvHullTree* pkWalk = pkFirstMark; pkWalk != (FvHullTree *)1;
		pkWalk = pkHullTreeNM)
	{
		if(pkWalk->m_iMarked & 1)
		{
			pkWalk->m_kTagBack.push_back(kTag);
			iBack++;
		}

		if(pkWalk->m_iMarked & 2)
		{
			pkWalk->m_kTagFront.push_back(kTag);
			iFront++;
		}

		pkHullTreeNM = pkWalk->m_pkNextMarked;

		pkWalk->m_iMarked = 0;
		pkWalk->m_pkNextMarked = NULL;
	}
}
//----------------------------------------------------------------------------
const FvHullContents* FvHullTree::TestPoint(const FvVector3& kPoint) const
{
	const FvHullContents* pkTags = NULL;

	int iOutBack;
	for(const FvHullTree* pkLook = this; pkLook != NULL;
		pkLook = (&pkLook->m_pkFront)[iOutBack])
	{
		iOutBack = !pkLook->m_kDivider.IsInFrontOf(kPoint);

		const FvHullContentsSet& kOwnTag = (&pkLook->m_kTagFront)[iOutBack];

		for(FvHullContentsSet::const_iterator it = kOwnTag.begin();
			it != kOwnTag.end(); it++)
		{
			const FvHullContents* pkContentHC = *it;
			pkContentHC->m_pkNext = pkTags;
			pkTags = pkContentHC;
		}
	}

	return pkTags;
}
//----------------------------------------------------------------------------
int FvHullTree::Size(int iDepth, bool bBack) const
{
	int iSize = sizeof(FvHullTree) + sizeof(FvHullContents*) *
		(m_kTagFront.capacity() + m_kTagBack.capacity());
	if (m_pkFront != NULL)
		iSize += m_pkFront->Size(iDepth + 1, false);
	if (m_pkBack != NULL)
		iSize += m_pkBack->Size(iDepth + 1, false);
	return iSize;
}
//----------------------------------------------------------------------------
FvHullTree::Traversal FvHullTree::Traverse(const FvVector3& kSource,
	const FvVector3& kExtent, float fRadius) const
{
	return Traversal(this, kSource, kExtent, fRadius);
}
//----------------------------------------------------------------------------
void FvHullTree::AddPlane(const FvPlane& kPlane, Portal3D& kOutline,
	FvHullTree*& pkFirstMarked)
{
	bool abAnySide[2] = {false, false};

	if(m_kDivider.Normal() != FvVector3::ZERO)
	{
		for(unsigned int i = 0; i < kOutline.Points().size(); i++)
		{
			const FvVector3& kPoint = kOutline.Points()[i];
			float d = m_kDivider.DistanceTo(kPoint);
			if(d >  0.01f) abAnySide[1] = true;
			else if(d < -0.01f) abAnySide[0] = true;
		}
	}
	else
	{
		*this = FvHullTree(kPlane, pkFirstMarked);
		pkFirstMarked = this;
		return;
	}

	if(m_pkNextMarked == NULL)
	{
		m_pkNextMarked = pkFirstMarked;
		pkFirstMarked = this;
	}

	if(!(abAnySide[0] | abAnySide[1]))
	{
		bool bSameDir = (m_kDivider.Normal().DotProduct(
			kPlane.Normal()) > 0.f);

		if(!(m_iMarked & (256 << int(bSameDir))))
		{
			m_iMarked |= (1 << int(bSameDir));
		}

		m_iMarked &= ~(1 << int(!bSameDir));
		m_iMarked |= (256 << int(!bSameDir));

		m_iMarked |= 65536;

		return;
	}

	Portal3D kInFront;
	Portal3D kOutBack;

	if(abAnySide[0] && abAnySide[1])
	{
		kInFront = kOutline;
		kOutBack = kOutline;
	}
	else if(!(m_iMarked & 65536))
	{
		int iSideWeAreNotOn = int(abAnySide[0]);

		bool bPointingSameWay = m_kDivider.Normal().DotProduct(
			kPlane.Normal()) > 0.0f;

		if(bPointingSameWay ^ abAnySide[1])
		{
			if(!(m_iMarked & (256 << iSideWeAreNotOn)))
				m_iMarked |= (1 << iSideWeAreNotOn);
		}
		else
		{
			m_iMarked &= ~(1 << iSideWeAreNotOn);
			m_iMarked |= (256 << iSideWeAreNotOn);
		}
	}


	if(abAnySide[0])
	{
		m_iMarked &= ~1;
		m_iMarked |= 256;

		Portal3D& kClipped = abAnySide[1] ? kOutBack : kOutline;

		if(m_pkBack != NULL)
			m_pkBack ->AddPlane(kPlane, kClipped, pkFirstMarked);
		else
			m_pkBack = new FvHullTree(kPlane, pkFirstMarked);
	}
	if(abAnySide[1])
	{
		m_iMarked &= ~2;
		m_iMarked |= 512;

		Portal3D& kClipped = abAnySide[0] ? kInFront : kOutline;

		if(m_pkFront != NULL)
			m_pkFront->AddPlane(kPlane, kClipped, pkFirstMarked);
		else
			m_pkFront = new FvHullTree(kPlane, pkFirstMarked);
	}
}
//----------------------------------------------------------------------------
FvHullTree::Traversal::Traversal(const FvHullTree* pkTree,
	const FvVector3& kSource, const FvVector3& kExtent, float fRadius) :
	m_kDelta(kExtent - kSource), m_kSource(kSource), m_pkPull( NULL ),
	m_fRadius(fRadius), m_bZeroRadius(fRadius == 0.0f)
{
	m_kStack.clear();
	m_kStack.push_back(StackElt(pkTree, -1, 0, 1));
}
//----------------------------------------------------------------------------
const FvHullContents* FvHullTree::Traversal::Next()
{
	if(m_pkPull != NULL)
	{
		if(m_uiPullAt < m_pkPull->size())
		{
			return (*m_pkPull)[m_uiPullAt++];
		}
		m_pkPull = NULL;
	}

	float sDist, eDist;
	int sBack, eBack;

	while(!m_kStack.empty())
	{
		StackElt kCur = m_kStack.back();

		sDist = kCur.m_fSt;
		eDist = kCur.m_fEt;

		if(kCur.m_iBack != -1)
		{
			sBack = kCur.m_iBack;
			eBack = kCur.m_iBack;

			m_kStack.pop_back();
		}

		else
		{
			const FvPlane& kPlane = kCur.m_pkNode->m_kDivider;
			float sOff = kPlane.DistanceTo(m_kSource + sDist * m_kDelta );
			float eOff = kPlane.DistanceTo(m_kSource + eDist * m_kDelta );
			if(m_bZeroRadius)
			{
				sBack = int(sOff < 0.f);
				eBack = int(eOff < 0.f);
			}
			else
			{
				bool sOff_l_nr = sOff < -m_fRadius, sOff_l_pr =
					sOff < m_fRadius;
				bool eOff_l_nr = eOff < -m_fRadius, eOff_l_pr =
					eOff < m_fRadius;
				bool sOff_l_eOff = sOff < eOff;
				sBack = int(sOff_l_nr | (sOff_l_pr &  sOff_l_eOff));
				eBack = int(eOff_l_nr | (eOff_l_pr & !sOff_l_eOff));
			}

			if(sBack != eBack)
			{
				if(m_bZeroRadius)
				{
					eDist = kPlane.IntersectRayHalf(m_kSource,
						kPlane.Normal().DotProduct(m_kDelta));

					m_kStack.back().m_iBack = eBack;
					m_kStack.back().m_fSt = eDist;
				}
				else
				{
					m_kStack.back().m_iBack = eBack;
				}
			}
			else
				m_kStack.pop_back();
		}

		const FvHullTree* pkDown = (&kCur.m_pkNode->m_pkFront)[sBack];
		if(pkDown != NULL)
			m_kStack.push_back(StackElt(pkDown, -1, sDist, eDist));

		m_pkPull = (&kCur.m_pkNode->m_kTagFront) + sBack;
		if(!m_pkPull->empty())
		{
			m_uiPullAt = 1;
			return m_pkPull->front();
		}
	}

	return NULL;
}
//----------------------------------------------------------------------------
FvHullTree::Traversal::StackElt::StackElt(const FvHullTree* pkNode,
	int eBack, float st, float et) : m_pkNode( pkNode ), m_iBack( eBack ),
	m_fSt(st), m_fEt(et)
{

}
//----------------------------------------------------------------------------
FvHullTree::Portal3D::Portal3D()
{

}
//----------------------------------------------------------------------------
FvHullTree::Portal3D::Portal3D(const FvPlane& kPlane, FvPortal2D& kPortal)
{
	for (unsigned int i = 0; i < kPortal.Points().size(); i++)
	{
		m_kPoints.push_back(kPlane.Param(kPortal.Points()[i]));
	}
}
//----------------------------------------------------------------------------
FvHullTree::Portal3D::FvVector3Vec& FvHullTree::Portal3D::Points()
{
	return m_kPoints;
}
//----------------------------------------------------------------------------
