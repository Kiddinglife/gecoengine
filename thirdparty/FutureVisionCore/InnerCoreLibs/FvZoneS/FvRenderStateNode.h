//{future header message}
#ifndef __FvRenderStateNode_H__
#define __FvRenderStateNode_H__

#include <FvSmartPointer.h>
#include <FvDoubleLink.h>
#include "FvRenderState.h"

class FvRenderStateNode;
class FvRenderDrawNode;

typedef FvDoubleLink<FvRenderStateNode,FvDoubleLinkNode1> FvRenderStateNodeList;
typedef FvDoubleLink<FvRenderDrawNode,FvDoubleLinkNode1> FvRenderDrawNodeList;

class FvRenderStateNode : public FvDoubleLinkNode1<FvRenderStateNode>
{
public:
	struct Data : FvReferenceCount
	{
		FvRenderState** m_ppkRenderStates;
		FvUInt32 m_u32StateNumber;
	};

	typedef FvSmartPointer<Data> DataPtr;

protected:
	DataPtr m_spData;
	FvRenderDrawNodeList m_kDrawChildrenForRender;
	FvRenderStateNodeList m_kStateChildrenForRender;
	FvRenderStateNode* m_pkParent;
	FvString m_kNodeName;

};

enum _D3DPRIMITIVETYPE;

class FvRenderDrawNode : public FvDoubleLinkNode1<FvRenderDrawNode>
{
public:
	struct IndexedDraw
	{
		_D3DPRIMITIVETYPE m_eType;
		FvInt32 m_i32BaseVertexIndex;
		FvUInt32 m_u32MinIndex;
		FvUInt32 m_u32NumVertices;
		FvUInt32 m_u32StartIndex;
		FvUInt32 m_u32PrimitiveCount;
	};

	struct UnIndexedDraw
	{
		_D3DPRIMITIVETYPE m_eType;
		FvUInt32 m_i32StartVertex;
		FvUInt32 m_u32PrimitiveCount;
		FvUInt32 m_u32Blank0;
		FvUInt32 m_u32Blank1;
		FvUInt32 m_u32Blank2;
	};

	enum DrawType
	{
		INDEXED_DRAW,
		UNINDEXED_DRAW
	};

	struct Data : FvReferenceCount
	{
		DrawType m_eType;

		union
		{
			IndexedDraw m_kIndexed;
			UnIndexedDraw m_kUnindexed;
		};
	};

	typedef FvSmartPointer<Data> DataPtr;
	
protected:
	DataPtr m_spData;

};

#endif // __FvRenderStateNode_H__
