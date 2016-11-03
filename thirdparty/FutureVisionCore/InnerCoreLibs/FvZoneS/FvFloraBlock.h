//{future header message}
#ifndef __FvFloraBlock_H__
#define __FvFloraBlock_H__

#include <FvBoundingBox.h>

#include <vector>

struct FvFloraVertex;
class FvEcotype;
class FvVector2;
class FvFloraRenderable;

class FvCamera;

class FvFloraBlock
{
public:
	FvFloraBlock( class FvFlora *pkFlora );
	~FvFloraBlock();

	void Init( const FvVector2 &kPos, FvUInt32 uiOffset );
	FvUInt32 Offset() const { return m_uiOffset; }	
	void Fill( FvUInt32 uiNumVertsAllowed );
	void Invalidate();
	bool NeedsRefill() const { return m_bRefill; }

	const FvVector2 &Center() const	{ return m_kCenter; }
	void Center( const FvVector2 &kPos );

	void Cull(FvCamera *pkCamera);
	bool Culled() const	{ return m_bCulled; }
	void Culled( bool bCull ) { m_bCulled = bCull; }

	int	 BlockID() const { return m_iBlockID; }
	const FvBoundingBox &Bounds() const	{ return m_kBB; }

	FvFloraRenderable *GetRenderable() const { return m_pkRenderable; }

private:

	bool NextTransform( const FvVector2 &kCenter, FvMatrix &kRet, FvVector2 &kRetEcotypeSamplePt );
	FvVector2 m_kCenter;	
	FvBoundingBox m_kBB;
	bool m_bCulled;
	bool m_bRefill;
	std::vector<FvEcotype*>	m_kEcotypes;
	int m_iBlockID;
	FvUInt32 m_uiOffset;
	class FvFlora *m_pkFlora;
	FvFloraRenderable *m_pkRenderable;

	friend class FvFloraRenderable;
};

typedef FvFloraBlock* pFloraBlock;

#endif // __FvFloraBlock_H__