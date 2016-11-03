//{future header message}
#ifndef __FvHistoryEvent_H__
#define __FvHistoryEvent_H__

#include "FvCellDefines.h"
#include <FvDataDescription.h>
#include <FvNetTypes.h>
#include <FvNetInterfaceMinder.h>
#include <FvObj.h>


class FV_CELL_API FvHistoryEvent
{
public:
	class Level
	{
	public:
		Level() {}
		Level(FvDetailLevel i):m_uiDetail(i) {}
		Level(float f):m_fPriority(f) {}

		union
		{
			float			m_fPriority;
			FvDetailLevel	m_uiDetail;
		};
	};

	FvHistoryEvent(FvNetMessageID msgID, FvEventNumber number,
					const void* msg, int msgLen, Level level,
					FvEntityID iEntityID,
					const FvString* pName=NULL);
	FvHistoryEvent(const FvHistoryEvent& kEvent);
	FvHistoryEvent& operator=(const FvHistoryEvent& kEvent);

	~FvHistoryEvent();

	FvEventNumber	Number() const;
	void			AddToBundle(FvNetBundle& bundle);
	bool			ShouldSend(float threshold, int detailLevel) const;
	bool			IsStateChange() const;
	FvDetailLevel	GetLevel() { return m_kLevel.m_uiDetail; }

public:
	Level					m_kLevel;

	FvNetInterfaceElement	m_kMsgIE;
	FvEventNumber			m_uiNumber;
	char*					m_pcMsg;
	FvInt32					m_iMsgLen;
	const FvString*			m_pkName;
	FvEntityID				m_iEntityID;
};

class FV_CELL_API FvEventHistory
{
public:
	typedef std::deque<FvHistoryEvent*>	Container;

	typedef Container::const_iterator			const_iterator;
	typedef Container::const_reverse_iterator	const_reverse_iterator;

	FvEventHistory();
	~FvEventHistory();

	void					Add(FvHistoryEvent* pEvent);
	void					Trim();
	void					Clear();

	const_iterator			begin() const	{ return m_kContainer.begin(); }
	const_iterator			end() const		{ return m_kContainer.end(); }

	const_reverse_iterator	rbegin() const	{ return m_kContainer.rbegin(); }
	const_reverse_iterator	rend() const	{ return m_kContainer.rend(); }

	bool					empty() const	{ return m_kContainer.empty(); }

private:
	Container				m_kContainer;
	Container::size_type	m_iTrimSize;
};

#include "FvHistoryEvent.inl"


#endif // __FvHistoryEvent_H__
