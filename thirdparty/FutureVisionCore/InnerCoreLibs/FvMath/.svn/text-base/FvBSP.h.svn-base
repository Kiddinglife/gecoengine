//{future header message}
#ifndef __FvBsp_H__
#define __FvBsp_H__

#include <string>
#include <map>

#include "FvPlane.h"
#include "FvBoundingBox.h"
#include "FvWorldPolygon.h"
#include "FvWorldTriangle.h"

#include <FvDebug.h>
#include <FvSmartPointer.h>

enum FvBSPSide
{
	FV_BSP_ON		= 0x0,
	FV_BSP_FRONT	= 0x1,
	FV_BSP_BACK	= 0x2,
	FV_BSP_BOTH	= 0x3
};

class FvCollisionVisitor
{
public:
	virtual ~FvCollisionVisitor() {};

	virtual bool Visit(const FvWorldTriangle& kHitTriangle, float fDist) = 0;
};

class FvBSPAllocator;
class FvBSPConstructor;
class FvBSP;
class FvBSPFile;

typedef std::vector<FvWorldTriangle::Flags> FvBSPFlagsMap;

class FV_MATH_API FvBSPTree
{
public:
	FvBSPTree(FvRealWorldTriangleSet& kTriangles);
	FvBSPTree();
	~FvBSPTree();

	bool Load(char *pkData,size_t stSize);
	//bool Save(const FvString &kFileName) const;

	void RemapFlags(FvBSPFlagsMap& kFlagsMap);

	const FvBSP* GetRoot() const;

	FvUInt32 Size() const;
	bool Empty() const;

	void GenerateBoundingBox();

	const FvBoundingBox& GetBoundingBox();

	const FvRealWorldTriangleSet& Triangles() const;

	enum UserDataKey
	{
		MD5_DIGEST,
		TIME_STAMP,
		USER_DEFINED = 1 << 16
	};

	class UserData : public FvSafeReferenceCount
	{
	public:
		UserData(size_t stSize)
		{
			FV_ASSERT(stSize != 0);
			m_pkData = new char[stSize];
		}
		~UserData()
		{
			delete[] m_pkData;
		}
		char *Data()
		{
			return m_pkData;
		}
		const size_t Size() const
		{
			return m_stSize;
		}
	private:	
		UserData(const UserData&);
		UserData& operator=(const UserData&);

		char *m_pkData;
		size_t m_stSize;
	};
	typedef FvSmartPointer<UserData> UserDataPtr;

	UserDataPtr GetUserData(UserDataKey eType);
	void SetUserData(UserDataKey eType, UserDataPtr spUserData);

	bool CanCollide() const;

private:

	bool LoadTrianglesForNode(FvBSPFile &kBSPFile,
		FvBSP &kNode, int iNumTriangles) const;

	FvBSP* m_pkRoot;
	FvRealWorldTriangleSet m_kTriangles;
	mutable FvUInt16* m_pusIndices;
	int m_iIndicesSize;
	char* m_pcNodeMemory;
	FvBoundingBox m_kBoundingBox;

	typedef std::map<UserDataKey, UserDataPtr> UserDataMap;
	UserDataMap m_kUserData;

	friend class FvBSP;
};

class FV_MATH_API FvBSP
{
public:
	bool Intersects(const FvWorldTriangle& kTriangle,
		const FvWorldTriangle** ppkHitTriangle = NULL) const;

	bool Intersects(const FvVector3& kStart,
		const FvVector3& kEnd, float& fDist,
		const FvWorldTriangle** ppkHitTriangle = NULL,
		FvCollisionVisitor * pkVisitor = NULL ) const;

	bool Intersects(const FvWorldTriangle& kTriangle,
		const FvVector3& kTranslation,
		FvCollisionVisitor* pkVisitor = NULL) const;

	void GetNumNodes(int& iNumNodes, int& iMaxTriangles) const;

	void Dump(int iDepth);
	FvUInt32 Size() const;

	bool Load( const FvBSPTree &kTree, FvBSPFile &kBSPFile,
		FvBSPAllocator &kAllocator );
	//bool Save(FILE *pkFile, const FvWorldTriangle *pkFront) const;

private:
	FvBSP();
	~FvBSP();

	FvBSPSide WhichSide(const FvWorldTriangle* pkTriangle) const;
	FvBSPSide WhichSide(const FvWorldPolygon& kPoly) const;

	bool IntersectsThisNode(const FvWorldTriangle& kTriangle,
		const FvWorldTriangle** ppkHitTriangle) const;

	bool IntersectsThisNode(const FvVector3& kStart,
		const FvVector3& kEnd, float& fDist,
		const FvWorldTriangle** ppkHitTriangle,
		FvCollisionVisitor* pkVisitor) const;

	bool IntersectsThisNode(const FvWorldTriangle& kTriangle,
		const FvVector3& kTranslation, FvCollisionVisitor* pkVisitor) const;

	void Partition(FvWorldTriangleSet& kTriangles,
		FvWorldPolygonSet& kPolygons, FvBSPConstructor& kConstructor);

	FvBSP* m_pkFront;
	FvBSP* m_pkBack;
	FvPlane m_kPlane;
	FvWorldTriangleSet m_kTriangles;
	bool m_bPartitioned;

	static const int MAX_SIZE;
	static const float TOLERANCE;

	friend class FvBSPConstructor;
	friend class FvBSPAllocator;
	friend class FvBSPTree;
};

#endif /* __FvBsp_H__ */
