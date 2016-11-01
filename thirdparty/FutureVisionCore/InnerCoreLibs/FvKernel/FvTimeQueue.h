//{future header message}
#ifndef __FvTimeQueue_H__
#define __FvTimeQueue_H__

#include <map>
#include <algorithm>
#include <vector>
#include <FvPrintf.h>
#include <FvPowerDefines.h>

typedef FvUIntPtr FvTimeQueueID;

class FvTimeQueueHandler
{
public:
	virtual ~FvTimeQueueHandler() {};

	virtual void HandleTimeout( FvTimeQueueID id, void * pUser ) = 0;
	virtual void OnRelease( FvTimeQueueID id, void * pUser ) = 0;
};

template< class TIME_STAMP >
class FvTimeQueueT
{
public:
	FvTimeQueueT();
	~FvTimeQueueT();

	void Clear();

	typedef TIME_STAMP TimeStamp;

	FvTimeQueueID Add( TimeStamp startTime, TimeStamp interval,
						FvTimeQueueHandler* pHandler, void * pUser );

	void Cancel( FvTimeQueueID id );

	void Process( TimeStamp now );

	bool Legal( FvTimeQueueID id ) const;

	TIME_STAMP NextEXP( TimeStamp now ) const;

	FV_INLINE FvUInt32 Size() const { return m_kTimeQueue.size(); }

	bool GetTimerInfo( FvTimeQueueID id,
					TimeStamp &			time,
					TimeStamp &			interval,
					FvTimeQueueHandler *&	pHandler,
					void * &			pUser ) const;

	enum State
	{
		STATE_PENDING,
		STATE_EXECUTING,
		STATE_CANCELLED
	};

private:
	void PurgeCancelledNodes();

	class Node
	{
	public:
		Node( TimeStamp startTime, TimeStamp interval,
			FvTimeQueueHandler * pHandler, void * pUser );
		~Node();

		void Cancel( FvTimeQueueID id );

		bool IsCancelled() const	{ return this->m_eState == STATE_CANCELLED; }

		TimeStamp m_kTime;
		TimeStamp m_kInterval;
		State m_eState;
		FvTimeQueueHandler *m_pkHandler;
		void *m_pkUser;

	private:
		Node( const Node & );
		Node & operator=( const Node & );
	};

	class Comparator
	{
	public:
		bool operator()(const Node* a, const Node* b)
		{
			return a->m_kTime > b->m_kTime;
		}
	};

	class PriorityQueue
	{
	public:
		typedef std::vector< Node * > Container;

		typedef typename Container::value_type value_type;
		typedef typename Container::size_type size_type;

		bool empty() const				{ return m_kContainer.empty(); }
		size_type size() const			{ return m_kContainer.size(); }

		const value_type & top() const	{ return m_kContainer.front(); }

		void push( const value_type & x )
		{
			m_kContainer.push_back( x );
			std::push_heap( m_kContainer.begin(), m_kContainer.end(),
					Comparator() );
		}

		void pop()
		{
			std::pop_heap( m_kContainer.begin(), m_kContainer.end(), Comparator() );
			m_kContainer.pop_back();
		}

		Container & container()		{ return m_kContainer; }

		void heapify()
		{
			std::make_heap( m_kContainer.begin(), m_kContainer.end(),
					Comparator() );
		}

	private:
		Container m_kContainer;
	};

	PriorityQueue m_kTimeQueue;
	Node *m_pkProcessingNode;
	TimeStamp m_kLastProcessTime;
	int m_iNumCancelled;

	FvTimeQueueT(const FvTimeQueueT&);
	FvTimeQueueT& operator=(const FvTimeQueueT&);

	void CheckTimeSanity( TimeStamp now );
};

typedef FvTimeQueueT< FvUInt32 > FvTimeQueue;
typedef FvTimeQueueT< FvUInt64 > FvTimeQueue64;

#include "FvTimeQueue.inl"

#endif // __FvTimeQueue_H__
