//{future header message}
#ifndef __FvFloraVertexType_H__
#define __FvFloraVertexType_H__

#define FV_FLORA_PACK_UV   16383.f
#define FV_FLORA_PACK_FLEX 255.f

#pragma pack(push, 1 )

struct FvVertexXYZNUVC
{
	FvVector3 m_kPos;
	FvVector3 m_kNormal;
	FvVector2 m_kUV;
	FvUInt32 m_uiColor;
};

struct FvFloraVertex
{
	FvVector4 m_kPos;
	FvVector3 m_kNormal;
	float m_aiUV[2];
	FvUInt32 m_uiColor;

	void Set( const FvVertexXYZNUVC &kVertex )
	{		
		m_kPos.x = kVertex.m_kPos.x;
		m_kPos.y = kVertex.m_kPos.y;
		m_kPos.z = kVertex.m_kPos.z;
		m_kPos.w = 1;
		m_kNormal = kVertex.m_kNormal;
		m_aiUV[0] = kVertex.m_kUV[0];
		m_aiUV[1] = kVertex.m_kUV[1];
		m_uiColor = kVertex.m_uiColor;
	}

	void Set( const FvFloraVertex &kVertex, const FvMatrix *pkTransform )
	{		
		pkTransform->ApplyPoint( m_kPos, kVertex.m_kPos );
		m_kNormal = kVertex.m_kNormal;
		m_aiUV[0] = kVertex.m_aiUV[0];
		m_aiUV[1] = kVertex.m_aiUV[1];
		m_uiColor = kVertex.m_uiColor;
	}

	void UV( const FvVector2 &kUV )
	{
		m_aiUV[0] = kUV[0];
		m_aiUV[1] = kUV[1];
	}
};

#pragma pack( pop )

#endif // __FvFloraVertexType_H__