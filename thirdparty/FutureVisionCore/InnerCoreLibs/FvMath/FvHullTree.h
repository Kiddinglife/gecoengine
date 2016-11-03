//{future header message}
#ifndef __FvHullTree_H__
#define __FvHullTree_H__

#include <vector>
#include <FvVectorNoDestructor.h>
#include "FvPlane.h"
#include "FvPortal2D.h"

class FV_MATH_API FvHullContents
{
public:
	FvHullContents();
	virtual ~FvHullContents();

	mutable const FvHullContents* m_pkNext;
};

typedef std::vector<const FvHullContents*> FvHullContentsSet;

class FvHullBorder : public std::vector<FvPlane>
{
};

class FV_MATH_API FvHullTree
{
public:
	FvHullTree();
	~FvHullTree();

	void Add(const FvHullBorder& kBorder, const FvHullContents* kTag);

	const FvHullContents* TestPoint(const FvVector3& kPoint) const;

	int Size(int iDepth, bool bBack) const;

	class Traversal
	{
	public:
		const FvHullContents* Next();

	private:
		Traversal(const FvHullTree* pkTree, const FvVector3& kSource,
			const FvVector3& kExtent, float fRadius);

		FvVector3 m_kDelta;
		const FvVector3& m_kSource;

		const FvHullContentsSet* m_pkPull;
		unsigned int	m_uiPullAt;

		float			m_fRadius;
		bool			m_bZeroRadius;

		struct StackElt
		{
			StackElt(const FvHullTree* pkNode, int eBack, float st, float et);

			const FvHullTree* m_pkNode;
			int m_iBack;
			float m_fSt;
			float m_fEt;
		};

		static FvVectorNoDestructor<StackElt> m_kStack;

		friend class FvHullTree;
	};

	friend class Traversal;

	Traversal Traverse(const FvVector3& kSource, const FvVector3& kExtent,
		float fRadius = 0.0f) const;

private:
	FvHullTree(const FvPlane& kPlane, FvHullTree*& pkFirstMarked);

	class Portal3D
	{
	public:
		Portal3D();
		Portal3D(const FvPlane& kPlane, FvPortal2D& kPortal);

		typedef std::vector<FvVector3> FvVector3Vec;

		FvVector3Vec& Points();

	private:
		FvVector3Vec m_kPoints;
	};

	void AddPlane(const FvPlane& kPlane, Portal3D& kOutline,
		FvHullTree*& pkFirstMarked);

	FvPlane m_kDivider;

	FvHullTree* m_pkFront;
	FvHullTree* m_pkBack;

	FvHullContentsSet m_kTagFront;
	FvHullContentsSet m_kTagBack;

	int m_iMarked;
	FvHullTree* m_pkNextMarked;
};

#endif /* __FvHullTree_H__ */
