//{future header message}
#ifndef __FvEntityCache_H__
#define __FvEntityCache_H__
/**
#include "FvCellDefines.h"
#include <FvBinaryStream.h>
#include <FvNetInterfaceMinder.h>
#include <FvNetMsgTypes.h>
#include <FvSmartPointer.h>

#include <list>


class FvEntity;
typedef FvSmartPointer<FvEntity> FvEntityPtr;
class FvSpace;


class FV_CELL_API FvEntityCache
{
public:
	static const int MAX_LOD_LEVELS = 4;

	typedef double FvPriority;

	FvEntityCache();
	~FvEntityCache();

	bool		Init(FvEntity* pkEntity, FvPriority fAoiPriority, FvNetChannel* pkChannel);
	bool		UpdatePriorityAndLoD(FvEntity* pkEntity, const FvVector3& kPos, FvPriority fAoiPriority, FvNetChannel* pkChannel);//! 返回FALSE表示出Aoi了
	FvEntity* 	GetEntity() const { return m_spEntity.GetObject(); }
	FvEntityID	GetLostEntityID() { return ms_iLostEntityID; }
	static void SetSpace(FvSpace* pkSpace) { ms_pkSpace = pkSpace; }

	void		SendEnterAoi(FvNetChannel* pkChannel);
	void		SendLeaveAoi(FvNetChannel* pkChannel);
	void		SendLeaveAoi(FvNetChannel* pkChannel, FvEntityID iEntityID);
	void		SendUpdateLod(FvDetailLevel uiFrom, FvDetailLevel uiTo, FvNetChannel* pkChannel);
	void		SendEventUpdate(FvNetChannel* pkChannel);

protected:
	bool		EventNumberLessThan(FvEventNumber a, FvEventNumber b);

private:
	FvEntityPtr			m_spEntity;

	FvEventNumber		m_uiLastEventNumber;
	FvVolatileNumber	m_uiLastVolatileUpdateNumber;

	FvPriority			m_fPriority;
	FvDetailLevel		m_uiDetailLevel;

	FvEventNumber		m_aiLodEventNumbers[ MAX_LOD_LEVELS ];	//! 记录各级Lod EventNumber的用途:
																//! 在遍历事件列表做属性同步/事件更新时,需判断对应Lod的EventNumber,防止重复更新
																//! 如:一共3级Lod,最新的消息id为100,上次遍历的消息id为80
																//! 上次能看到Lod2,消息id为75
																//! 这次新看到Lod1,先做全部更新,消息id为100
																//! 然后遍历消息队列,对Lod1和2做属性同步
																//! 如果不判断各级Lod的消息id,则此时Lod1会重复更新

	friend FvBinaryIStream & operator>>( FvBinaryIStream & stream,
			FvEntityCache & entityCache );
	friend FvBinaryOStream & operator<<( FvBinaryOStream & stream,
			const FvEntityCache & entityCache );

	static FvEntityID	ms_iLostEntityID;
	static FvSpace*		ms_pkSpace;
};

#include "FvEntityCache.inl"
**/

#endif // __FvEntityCache_H__
