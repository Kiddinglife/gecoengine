//{future header message}
#include "FvQuadTree.h"
#include <float.h>
#include <FvGlobalMacros.h>

//----------------------------------------------------------------------------
void FvQuadTreeCoord::ClipMin()
{
	x = FV_MAX( 0, x );
	y = FV_MAX( 0, y );
}
//----------------------------------------------------------------------------
void FvQuadTreeCoord::ClipMax(int iDepth)
{
	const int MAX = (1 << iDepth) - 1;
	x = FV_MIN( x, MAX );
	y = FV_MIN( y, MAX );
}
//----------------------------------------------------------------------------
int FvQuadTreeCoord::FindChild(int iDepth) const
{
	const int MASK = (1 << iDepth);

	return ((x & MASK) + 2 * (y & MASK)) >> iDepth;
}
//----------------------------------------------------------------------------
void FvQuadTreeCoord::Offset(int iQuad, int iDepth)
{
	x |= ((iQuad & 0x1) << iDepth);
	y |= ((iQuad > 1) << iDepth);
}
//----------------------------------------------------------------------------
bool FvQuadTreeRange::Fills(int iDepth) const
{
	const int MAX = (1 << iDepth) - 1;
	return m_kLeft == 0 && m_kBottom == 0 &&
		m_kRight == MAX && m_kTop == MAX;
}
//----------------------------------------------------------------------------
bool FvQuadTreeRange::IsValid(int iDepth) const
{
	const int MAX = 1 << iDepth;
	return
		(0 <= m_kLeft) && (m_kLeft <= m_kRight) && (m_kRight < MAX) &&
		(0 <= m_kBottom) && (m_kBottom <= m_kTop) && (m_kTop < MAX) &&
		iDepth >= 0;
}
//----------------------------------------------------------------------------
bool FvQuadTreeRange::InQuad(int iDepth) const
{
	const int MAX = 1 << iDepth;

	return 0 <= m_kRight && 0 <= m_kTop &&
		m_kLeft < MAX && m_kBottom < MAX;
}
//----------------------------------------------------------------------------
void FvQuadTreeRange::Clip(int iDepth)
{
	m_akCorner[0].ClipMin();
	m_akCorner[1].ClipMax(iDepth);
}
//----------------------------------------------------------------------------
