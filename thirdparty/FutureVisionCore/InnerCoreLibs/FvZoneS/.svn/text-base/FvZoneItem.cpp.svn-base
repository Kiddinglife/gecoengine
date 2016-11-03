#include "FvZoneItem.h"
#include "FvZone.h"

#include <FvDebug.h>

FvUInt32 FvZoneItemBase::ms_uiInstanceCount		= 0;
FvUInt32 FvZoneItemBase::ms_uiInstanceCountPeak	= 0;

void FvZoneItemBase::LendByBoundingBox( FvZone *pkLender,
									  const FvBoundingBox &kWorldBB )
{
	// FV_GUARD;
	int iAllInOwnChunk = m_pkZone->IsOutsideZone() ? 0 : -1;

	FvZone::piterator pend =pkLender->PEnd();
	for (FvZone::piterator pit =pkLender->PBegin(); pit != pend; pit++)
	{
		if (!pit->HasZone()) continue;

		FvZone *pkConsider = pit->m_pkZone;

		if (!kWorldBB.Intersects(pkConsider->BoundingBox() )) continue;

		if (pkConsider->IsOutsideZone())
		{
			if (iAllInOwnChunk < 0)
			{
				
				FvVector3 bbpts[2] = {kWorldBB.MinBounds(),kWorldBB.MaxBounds() };
				FvVector3 tpoint;	
				int i;		
				for (i = 0; i < 8; i++)
				{
					tpoint.x = bbpts[(i>>0)&1].x;
					tpoint.y = bbpts[(i>>1)&1].y;
					tpoint.z = bbpts[(i>>2)&1].z;
					if (!m_pkZone->Contains( tpoint )) break;
				}
				iAllInOwnChunk = (i == 8);
				
				if (iAllInOwnChunk) break;
			}
		
			FV_ASSERT_DEV( !iAllInOwnChunk );
		}

		if (pkConsider->AddLoanItem( static_cast<FvZoneItem*>(this) ))
		{
			pkConsider->UpdateBoundingBoxes( static_cast<FvZoneItem*>( this ) );
		}
	}
}

void FvZoneItemBase::LendByBoundingBoxShell( FvZone *pkLender, const FvBoundingBox &kWorldBB )
{
	// FV_GUARD;
	int iAllInOwnChunk = m_pkZone->IsOutsideZone() ? 0 : -1;

	FvZone::piterator pend =pkLender->PEnd();
	for (FvZone::piterator pit =pkLender->PBegin(); pit != pend; pit++)
	{
		if (!pit->HasZone()) continue;

		FvZone *pkConsider = pit->m_pkZone;

		if (!kWorldBB.Intersects(pkConsider->BoundingBox() )) continue;

		if (pkConsider->IsOutsideZone())
		{
			continue;
		}

		if (pkConsider->AddLoanItem( static_cast<FvZoneItem*>(this) ))
		{
			pkConsider->UpdateBoundingBoxes( static_cast<FvZoneItem*>( this ) );
		}
	}
}