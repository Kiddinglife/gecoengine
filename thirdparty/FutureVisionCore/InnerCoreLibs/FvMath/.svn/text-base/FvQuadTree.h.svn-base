//{future header message}
#ifndef __FvQuadTree_H__
#define __FvQuadTree_H__

#include <vector>
#include <FvDebug.h>
#include <float.h>
#include "FvVector3.h"

template <class FV_MEMBER_TYPE> class FvQuadTreeTraversal;
template <class FV_MEMBER_TYPE> class FvQuadTreeNode;

typedef int FvQuadTreeIndex;

enum FvQuad
{
	FV_QUAD_BL = 0,
	FV_QUAD_BR = 1,
	FV_QUAD_TL = 2,
	FV_QUAD_TR = 3
};
//----------------------------------------------------------------------------
class FV_MATH_API FvQuadTreeCoord
{
public:
	void ClipMin();

	void ClipMax(int iDepth);

	int FindChild(int iDepth) const;

	void Offset(int iQuad, int iDepth);

	int x;
	int y;
};
//----------------------------------------------------------------------------
class FV_MATH_API FvQuadTreeRange
{
public:
	bool Fills(int iDepth) const;

	bool IsValid(int iDepth) const;

	bool InQuad(int iDepth) const;

	void Clip(int iDepth);
	
	union
	{
		FvQuadTreeIndex m_akRanges[4];
		FvQuadTreeCoord m_akCorner[2];
		struct
		{
			FvQuadTreeIndex m_kLeft;
			FvQuadTreeIndex m_kBottom;
			FvQuadTreeIndex m_kRight;
			FvQuadTreeIndex m_kTop;
		};
	};
};
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
class FvQuadTree
{
public:
	typedef FvQuadTree<FV_MEMBER_TYPE> This;
	typedef FvQuadTreeNode<FV_MEMBER_TYPE> Node;
	typedef FvQuadTreeTraversal<FV_MEMBER_TYPE> Traversal;

	FvQuadTree(float fX, float fY, int iDepth, float fRange);
	~FvQuadTree();

	void Add(const FV_MEMBER_TYPE& tElement);
	void Del(const FV_MEMBER_TYPE& tElement);

	const FV_MEMBER_TYPE* TestPoint(const FvVector3& kPoint) const;

	Node* GetRoot()
	{
		return &m_kRoot;
	}

	const Node*	GetRoot() const
	{
		return &m_kRoot;
	}

	void Print() const;
	int Size() const;

	Traversal Traverse(const FvVector3& kSource,
		const FvVector3& kExtent,
		float fRadius = 0.0f) const;

	int Depth() const
	{
		return m_iDepth;
	}

	float Range() const
	{
		return m_fRange;
	}

private:
	FvQuadTree(const FvQuadTree&);
	FvQuadTree& operator = (const FvQuadTree&);

	Node m_kRoot;
	const FvVector2 m_kOrigin;
	const int m_iDepth;
	const float m_fRange;
};
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
class FvQuadTreeNode
{
public:
	FvQuadTreeNode()
	{
		m_apkChild[0] = NULL;
		m_apkChild[1] = NULL;
		m_apkChild[2] = NULL;
		m_apkChild[3] = NULL;
	}
	~FvQuadTreeNode()
	{
		delete m_apkChild[0];
		delete m_apkChild[1];
		delete m_apkChild[2];
		delete m_apkChild[3];
	}

	void Add(const FV_MEMBER_TYPE& tElement,
		const FvQuadTreeRange& kRange, int iDepth);
	bool Del(const FV_MEMBER_TYPE& tElement,
		const FvQuadTreeRange& kRange, int iDepth);

	const FV_MEMBER_TYPE* TestPoint(const FvVector3& kPoint,
		FvQuadTreeCoord kCoord, int iDepth) const;

	typedef std::vector<const FV_MEMBER_TYPE*> Elements;
	const Elements& GetElements() const
	{
		return m_kElements;
	}

	FvQuadTreeNode<FV_MEMBER_TYPE>* Child(int i) const
	{
		return m_apkChild[i];
	}

	int Size() const;
	void Print(int iDepth) const;
	int CountAt(FvQuadTreeCoord kCoord, int iDepth) const;

private:
	FvQuadTreeNode<FV_MEMBER_TYPE>* GetOrCreateChild(int i)
	{
		if (m_apkChild[i] == NULL)
		{
			m_apkChild[i] = new FvQuadTreeNode;
		}

		return m_apkChild[i];
	}

	FvQuadTreeNode<FV_MEMBER_TYPE> *m_apkChild[4];
	Elements m_kElements;
};
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
class FvQuadTreeTraversal
{
	static const int Q0 = 1 << 0;
	static const int Q1 = 1 << 1;
	static const int Q2 = 1 << 2;
	static const int Q3 = 1 << 3;

	typedef FvQuadTree<FV_MEMBER_TYPE> Tree;
	typedef typename Tree::Node	Node;

public:
	const FV_MEMBER_TYPE* Next();

private:
	FvQuadTreeTraversal(const Tree* pkTree, const FvVector3& kSrc,
		const FvVector3& kDst, float fRadius,
		const FvVector2& kOrigin);

	struct StackElement
	{
		const Node* m_pkNode;
		FvQuadTreeCoord m_kCoord;
		int m_iDepth;
	};

	void PushChild(const StackElement& kCurr, int iQuad);
	void Push(const Node* pkNode, FvQuadTreeCoord kCoord, int iDepth);
	StackElement Pop();
	const FV_MEMBER_TYPE* ProcessHead();

	static const int STACK_SIZE = 20;	
	StackElement m_kStack[STACK_SIZE];
	int m_iSize;

	size_t m_stHeadIndex;

	float m_fXLow0;
	float m_fYLow0;

	float m_fXHigh0;
	float m_fYHigh0;

	float m_fXLowDelta;
	float m_fYLowDelta;

	float m_fXHighDelta;
	float m_fYHighDelta;

	int m_iDirType;

	friend class FvQuadTree<FV_MEMBER_TYPE>;
};
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
FvQuadTree<FV_MEMBER_TYPE>::FvQuadTree(float fX, float fY,
	int iDepth, float fRange) : m_kOrigin(fX, fY), m_iDepth(iDepth),
	m_fRange(fRange)
{

}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
FvQuadTree<FV_MEMBER_TYPE>::~FvQuadTree()
{

}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
void FvQuadTree<FV_MEMBER_TYPE>::Add(const FV_MEMBER_TYPE& tElement)
{
	FvQuadTreeRange kRange = FvCalculateQuadTreeRange(tElement, m_kOrigin,
		m_fRange, m_iDepth);

	if (!kRange.InQuad(m_iDepth))
	{
		return;
	}

	kRange.Clip(m_iDepth);

	m_kRoot.Add(tElement, kRange, m_iDepth);
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
void FvQuadTree<FV_MEMBER_TYPE>::Del(const FV_MEMBER_TYPE& tElement)
{
	FvQuadTreeRange kRange = FvCalculateQuadTreeRange(tElement,
		m_fRange, m_iDepth);
	m_kRoot.Del(tElement, kRange);
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
const FV_MEMBER_TYPE* FvQuadTree<FV_MEMBER_TYPE>::TestPoint(
	const FvVector3& kPoint) const
{
	return m_kRoot.TestPoint(kPoint);
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
void FvQuadTree<FV_MEMBER_TYPE>::Print() const
{
	
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
int FvQuadTree<FV_MEMBER_TYPE>::Size() const
{
	return sizeof(*this) + m_kRoot.Size();
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
FvQuadTreeTraversal<FV_MEMBER_TYPE> FvQuadTree<FV_MEMBER_TYPE>::Traverse(
	const FvVector3& kSource, const FvVector3& kExtent, float fRadius) const
{
	return FvQuadTreeTraversal<FV_MEMBER_TYPE>(this, kSource, kExtent,
		fRadius, m_kOrigin);
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
FvQuadTreeTraversal<FV_MEMBER_TYPE>::FvQuadTreeTraversal(const Tree* pkTree,
	const FvVector3& kSrc, const FvVector3& kDst, float fRadius,
	const FvVector2& kOrigin) : m_iSize(0), m_stHeadIndex(0)
{
	const int DEPTH = pkTree->Depth();
	const float RANGE = pkTree->Range();
	FvQuadTreeCoord kRoot;
	kRoot.x = kRoot.y = 0;

	this->Push(pkTree->GetRoot(), kRoot, DEPTH);

	FvVector2 kDir(kDst.x - kSrc.x, kDst.y - kSrc.y);

	const bool bPointsLeft = (kDir.x < 0);
	const bool bPointsDown = (kDir.y < 0);
	m_iDirType = 2 * bPointsDown + bPointsLeft;

	FvVector2 kDirNorm = kDir;
	kDirNorm.Normalise();
	kDirNorm *= fRadius;
	FvVector2 kPerp(-kDirNorm.y, kDirNorm.x);

	if(bPointsDown != bPointsLeft)
	{
		kPerp *= -1.0f;
	}

	kDir += 2.0f * kDirNorm;

	FvVector2 kSrcLow(kSrc.x, kSrc.y);
	kSrcLow -= kDirNorm;
	kSrcLow -= kPerp;

	FvVector2 kSrcHigh(kSrc.x, kSrc.y);
	kSrcHigh -= kDirNorm;
	kSrcHigh += kPerp;

	FvVector2 kDstLow(kDst.x, kDst.y);
	kDstLow += kDirNorm;
	kDstLow -= kPerp;
	FvVector2 kDstHigh(kDst.x, kDst.y);
	kDstHigh += kDirNorm;
	kDstHigh += kPerp;

	const int WIDTH = 1 << DEPTH;

	const float BIG_FLOAT = FLT_MAX / 4.f;

	if(kDir.x != 0.f)
	{
		m_fXLow0 = (kOrigin.x + bPointsLeft * RANGE - kSrcLow.x) / kDir.x;
		m_fXHigh0 = (kOrigin.x + bPointsLeft * RANGE - kSrcHigh.x) / kDir.x;

		m_fXLowDelta = RANGE / WIDTH / FvFabsf(kDir.x);
		m_fXHighDelta = m_fXLowDelta;
	}
	else
	{
		const float fLowCrossing =
			FvFloorf(WIDTH * (kSrcLow.x - kOrigin.x) / RANGE) + 0.5f;
		m_fXLow0 = (fLowCrossing > 0.f) ? -BIG_FLOAT : BIG_FLOAT;
		m_fXLowDelta = m_fXLow0/fLowCrossing;

		const float fHighCrossing =
			FvFloorf(WIDTH * (kSrcHigh.x - kOrigin.x) / RANGE) + 0.5f;
		m_fXHigh0 = -BIG_FLOAT;
		m_fXHighDelta = BIG_FLOAT / (fHighCrossing + 0.5f);
	}

	if(kDir.y != 0.f)
	{
		m_fYLow0 = (kOrigin.y + bPointsDown * RANGE - kSrcLow.y) / kDir.y;
		m_fYHigh0 = (kOrigin.y + bPointsDown * RANGE - kSrcHigh.y) / kDir.y;
		m_fYLowDelta = RANGE / WIDTH / FvFabsf(kDir.y);
		m_fYHighDelta = m_fYLowDelta;
	}
	else
	{
		const float fLowCrossing =
			FvFloorf(WIDTH * (kSrcLow.y - kOrigin.y) / RANGE) + 0.5f;
		m_fYLow0 = (fLowCrossing > 0.f) ? -BIG_FLOAT : BIG_FLOAT;
		m_fYLowDelta = -m_fYLow0 / fLowCrossing;

		const float highCrossing =
			FvFloorf(WIDTH * (kSrcHigh.y - kOrigin.y) / RANGE) + 0.5f;
		m_fYHigh0 = -BIG_FLOAT;
		m_fYHighDelta = BIG_FLOAT/(highCrossing + 0.5f);
	}
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
void FvQuadTreeTraversal<FV_MEMBER_TYPE>::PushChild(
	const StackElement& kCurr, int iQuad)
{
	const Node* pkChild = kCurr.m_pkNode->Child(iQuad ^ m_iDirType);

	if(pkChild)
	{
		FvQuadTreeCoord kNewCoord = kCurr.m_kCoord;

		kNewCoord.Offset(iQuad, kCurr.m_iDepth - 1);
		Push(pkChild, kNewCoord, kCurr.m_iDepth - 1);
	}
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
const FV_MEMBER_TYPE* FvQuadTreeTraversal<FV_MEMBER_TYPE>::Next()
{
	if (m_iSize <= 0)
	{
		return NULL;
	}

	while(m_iSize > 0)
	{
		const FV_MEMBER_TYPE* ptResult = this->ProcessHead();

		if(ptResult)
		{
			return ptResult;
		}

		StackElement kCurr = this->Pop();

		if (kCurr.m_iDepth > 0)
		{
			FvQuadTreeCoord & kCoord = kCurr.m_kCoord;
			const int iHalfWidth = 1 << (kCurr.m_iDepth - 1);

			const float fCurrLowXDelta = iHalfWidth * m_fXLowDelta;
			const float fCurrLowYDelta = iHalfWidth * m_fYLowDelta;
			const float fCurrHighXDelta = iHalfWidth * m_fXHighDelta;
			const float fCurrHighYDelta = iHalfWidth * m_fYHighDelta;

			const float fXMinLow = m_fXLow0 + kCoord.x * m_fXLowDelta;
			const float fXMidLow = fXMinLow + fCurrLowXDelta;

			const float fYMinLow = m_fYLow0 + kCoord.y * m_fYLowDelta;
			const float fYMidLow = fYMinLow + fCurrLowYDelta;
			const float fYMaxLow = fYMidLow + fCurrLowYDelta;

			const float fXMinHigh = m_fXHigh0 + kCoord.x * m_fXHighDelta;
			const float fXMidHigh = fXMinHigh + fCurrHighXDelta;
			const float fXMaxHigh = fXMidHigh + fCurrHighXDelta;

			const float fYMinHigh = m_fYHigh0 + kCoord.y * m_fYHighDelta;
			const float fYMidHigh = fYMinHigh + fCurrHighYDelta;

			const bool bAboveMidpoint = (fYMidHigh <= fXMidHigh);
			const bool bBelowMidpoint = (fXMidLow <= fYMidLow);

			const bool bAboveQ0 = (fYMidLow < fXMinLow);
			const bool bBelowQ0 = (fXMidHigh < fYMinHigh);

			const bool bAboveQ3 = (fYMaxLow < fXMidLow);
			const bool bBelowQ3 = (fXMaxHigh < fYMidHigh);

			const bool bToLeft	= (fXMidLow > 1.f);
			const bool bToRight	= (fXMidHigh < 0.f);
			const bool bIsAbove	= (fYMidLow < 0.f);
			const bool bIsBelow	= (fYMidHigh > 1.f);

			if (!bAboveQ3 && !bBelowQ3 && !bToLeft && !bIsBelow)
				PushChild(kCurr, 3);

			if (bBelowMidpoint && !bToLeft && !bIsAbove)
				PushChild(kCurr, 1);

			if (bAboveMidpoint && !bToRight && !bIsBelow)
				PushChild(kCurr, 2);

			if (!bAboveQ0 && !bBelowQ0 && !bToRight && !bIsAbove)
				PushChild(kCurr, 0);
		}
	}

	return NULL;
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
void FvQuadTreeTraversal<FV_MEMBER_TYPE>::Push(
	const Node* pkNode, FvQuadTreeCoord kCoord, int iDepth)
{
	FV_ASSERT(m_iSize < STACK_SIZE);
	FV_ASSERT(m_stHeadIndex == 0);

	StackElement& kCurr = m_kStack[m_iSize];
	kCurr.m_pkNode = pkNode;
	kCurr.m_kCoord = kCoord;
	kCurr.m_iDepth = iDepth;
	m_iSize++;
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
typename FvQuadTreeTraversal<FV_MEMBER_TYPE>::StackElement
FvQuadTreeTraversal<FV_MEMBER_TYPE>::Pop()
{
	m_stHeadIndex = 0;
	return m_kStack[--m_iSize];
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
const FV_MEMBER_TYPE* FvQuadTreeTraversal<FV_MEMBER_TYPE>::ProcessHead()
{
	StackElement& kHead = m_kStack[m_iSize-1];
	const Node* pkCurr = kHead.m_pkNode;

	const typename Node::Elements& kElements = pkCurr->GetElements();

	while(m_stHeadIndex < kElements.size())
	{
		const FV_MEMBER_TYPE* ptNext = kElements[m_stHeadIndex];
		m_stHeadIndex++;

		const bool bShouldConsider = true;

		if(bShouldConsider)
		{
			return ptNext;
		}
	}

	return NULL;
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
inline void FvQuadTreeNode<FV_MEMBER_TYPE>::Add(
	const FV_MEMBER_TYPE & kElement, const FvQuadTreeRange& kRange,
	int iDepth)
{
	FV_ASSERT(kRange.IsValid(iDepth));

	if (kRange.Fills(iDepth))
	{
		bool bWillGrow = m_kElements.size() == m_kElements.capacity();
		m_kElements.push_back(&kElement);
	}
	else
	{
		const int iNewDepth = iDepth - 1;
		const int MASK = (1 << iNewDepth);
		const int CLEAR = ~(1 << iNewDepth);
		const int MAX = (1 << iNewDepth) - 1;

		int iLeft = !(kRange.m_kLeft & MASK);
		int iBottom = !(kRange.m_kBottom & MASK);
		int iRight = kRange.m_kRight & MASK;
		int iTop = kRange.m_kTop	& MASK;

		FvQuadTreeRange kOrigRange = kRange;
		for(int i = 0; i < 4; i++)
		{
			kOrigRange.m_akRanges[i] &= CLEAR;
		}
		FvQuadTreeRange kNewRange = kOrigRange;

		if(iLeft)
		{
			if(iRight)
				kNewRange.m_kRight = MAX;

			if(iTop)
			{
				if(iBottom)
				{
					kNewRange.m_kBottom = 0;
				}

				FV_ASSERT(kNewRange.IsValid(iNewDepth));
				GetOrCreateChild(FV_QUAD_TL)->
					Add(kElement, kNewRange, iNewDepth);
				kNewRange.m_kBottom = kOrigRange.m_kBottom;
				kNewRange.m_kTop = MAX;
			}

			if(iBottom)
			{
				FV_ASSERT( kNewRange.IsValid(iNewDepth));
				GetOrCreateChild(FV_QUAD_BL)->
					Add(kElement, kNewRange, iNewDepth);
			}

			kNewRange.m_kRight = kOrigRange.m_kRight;
		}

		if(iRight)
		{
			if(iLeft)
				kNewRange.m_kLeft = 0;
			if (iTop)
				kNewRange.m_kTop = MAX;

			if(iBottom)
			{
				FV_ASSERT(kNewRange.IsValid(iNewDepth));
				GetOrCreateChild(FV_QUAD_BR)->
					Add(kElement, kNewRange, iNewDepth);
				kNewRange.m_kBottom = 0;
			}

			if(iTop)
			{
				kNewRange.m_kTop = kOrigRange.m_kTop;
				FV_ASSERT(kNewRange.IsValid(iNewDepth));
				GetOrCreateChild( FV_QUAD_TR )->
					Add(kElement, kNewRange, iNewDepth);
			}
		}
	}
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
bool FvQuadTreeNode<FV_MEMBER_TYPE>::Del(const FV_MEMBER_TYPE& kElement,
	const FvQuadTreeRange& kRange, int iDepth)
{
	return false;
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
const FV_MEMBER_TYPE* FvQuadTreeNode<FV_MEMBER_TYPE>::TestPoint(
	const FvVector3& kPoint, FvQuadTreeCoord kCoord, int iDepth) const
{
	const FV_MEMBER_TYPE* pkResult = NULL;

	if(iDepth > 0)
	{
		const int iQuad = kCoord.FindChild(iDepth - 1);
		This* pkChild = m_apkChild[iQuad];

		if (pkChild)
		{
			pkResult = pkChild->TestPoint(kPoint, kCoord, iDepth);
		}
	}

	if(pkResult == NULL)
	{
		typename Elements::iterator iter = m_kElements.begin();

		while(iter != m_kElements.end())
		{
			const FV_MEMBER_TYPE* pkCurr = (*iter);

			if(FvContainsTestPoint(*pkCurr, kPoint))
			{
				return pkCurr;
			}

			iter++;
		}
	}

	return pkResult;
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
int FvQuadTreeNode<FV_MEMBER_TYPE>::Size() const
{
	int iChildrenSize = 0;

	for (int i = 0; i < 4; i++)
	{
		iChildrenSize += (m_apkChild[i] ? m_apkChild[i]->Size() : 0);
	}

	return iChildrenSize + sizeof( *this ) +
		sizeof( const FV_MEMBER_TYPE * ) * m_kElements.capacity();
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
void FvQuadTreeNode<FV_MEMBER_TYPE>::Print(int iDepth) const
{
	
}
//----------------------------------------------------------------------------
template <class FV_MEMBER_TYPE>
int FvQuadTreeNode<FV_MEMBER_TYPE>::CountAt(FvQuadTreeCoord kCoord,
	int iDepth) const
{
	int iCount = m_kElements.size();

	if (iDepth > 0)
	{
		int iQuad = kCoord.FindChild(iDepth - 1);

		FvQuadTreeNode<FV_MEMBER_TYPE>* pkChild = this->Child(iQuad);

		if (pkChild)
		{
			iCount += pkChild->CountAt(kCoord, iDepth - 1);
		}
	}

	return iCount;
}
//----------------------------------------------------------------------------

#endif /* __FvQuadTree_H__ */
