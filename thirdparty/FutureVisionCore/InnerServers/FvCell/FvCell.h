//{future header message}
#ifndef __FvCell_H__
#define __FvCell_H__

class FvCell;
class FvEntity;

#include "FvCellDefines.h"
#include <algorithm>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <deque>

#include "FvSpace.h"
#include "FvCellAppInterface.h"

#include <FvSmartPointer.h>
#include <FvVector2.h>
#include <FvVector3.h>
#include <FvNetInterfaceMinder.h>
#include <FvServerCommon.h>



class FV_CELL_API FvCell
{
public:
	FvCell(FvSpace& space, FvSpace::CellInfo* pkCellInfo);
	~FvCell();

	static FvCell * FindMessageHandler(const FvNetAddress& srcAddr, FvNetUnpackedMessageHeader& header, FvBinaryIStream& data);

	FvUInt16		CellIdx();
	FvSpaceID		SpaceID();
	FvUInt16		SpaceType();
	bool			IsInCell(const FvVector3& kPos) const;
	FvSpace*		GetSpace() { return &m_kSpace; }
	FvSpace::CellInfo*GetCellInfo() { return m_pkCellInfo; }

	//! internal interface
	void			ShouldOffload( const CellAppInterface::ShouldOffloadArgs & args );
	void			SetRetiringCell( const CellAppInterface::SetRetiringCellArgs & args );
	//! 

	void			AddRealEntity(FvEntity* pkEntity);
	void			DelRealEntity(FvEntity* pkEntity);
	void			EventSyncUpdate();	//! 更新给客户端的消息
	void			TransitionUpdate();	//! 检测迁移和Ghost
	void			AoiAndTrapUpdate();		//! 检测AOI
	float			CellHysteresisSize() { return m_fCellHysteresisSize; }
	void			EntityUpdate();
	void			SendSpaceDataToAllClient(const FvSpaceEntryID& kEntryID, FvUInt16 uiKey, const FvString& kData);

protected:

private:
	FvSpace&	m_kSpace;
	FvSpace::CellInfo* m_pkCellInfo;

	typedef std::vector<FvEntity*> RealEntities;
	RealEntities m_kRealEntities;

	float		m_fCellHysteresisSize;

	FvUInt64	m_uiEntityTickLastTime;
};


#include "FvCell.inl"

#endif // __FvCell_H__
