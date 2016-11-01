#include "FvTimeQueue.h"
#include "FvDebug.h"
#include <FvPowerDefines.h>
#include <functional>

template< class TIME_STAMP >
FvTimeQueueT< TIME_STAMP >::FvTimeQueueT() :
	m_pkProcessingNode( NULL ),
	m_kLastProcessTime( 0 ),
	m_iNumCancelled( 0 )
{
}

template <class TIME_STAMP>
FvTimeQueueT< TIME_STAMP >::~FvTimeQueueT()
{
	this->Clear();
}

template <class TIME_STAMP>
void FvTimeQueueT< TIME_STAMP >::Clear()
{
	typedef Node * const * NodeIter;

	int count = 0;
	while (!m_kTimeQueue.empty())
	{
		unsigned int oldSize = m_kTimeQueue.size();
		NodeIter begin = &m_kTimeQueue.top();
		NodeIter end = begin + oldSize;
		for (NodeIter it = begin; it != end; it++)
			(*it)->Cancel( FvTimeQueueID( *it ) );

		if (oldSize == m_kTimeQueue.size()) break;

		if (++count >= 16)
		{
			FvDPrintfNormal( "FvTimeQueue::~FvTimeQueue: "
				"Unable to cancel whole queue after 16 rounds!\n" );
			break;
		}
	}

	if (!m_kTimeQueue.empty())
	{
		NodeIter begin = &m_kTimeQueue.top();
		NodeIter end = begin + m_kTimeQueue.size();
		for (NodeIter it = begin; it != end; it++)
		{
			delete *it;
		}
	}

	m_kTimeQueue = PriorityQueue();
}

template <class TIME_STAMP>
FvTimeQueueID FvTimeQueueT< TIME_STAMP >::Add( TimeStamp startTime, TimeStamp interval,
						FvTimeQueueHandler* pHandler, void * pUser )
{
	this->CheckTimeSanity( startTime );

	Node * pNode = new Node( startTime, interval, pHandler, pUser );
	m_kTimeQueue.push( pNode );
	return (FvTimeQueueID)pNode;
}

template <class TIME_STAMP>
void FvTimeQueueT< TIME_STAMP >::Cancel( FvTimeQueueID id )
{
	Node * pNode = (Node*)id;
	if (pNode->IsCancelled())
	{
		return;
	}

	pNode->Cancel( id );

	++m_iNumCancelled;

	if (m_iNumCancelled * 2 > int( m_kTimeQueue.size() ))
	{
		this->PurgeCancelledNodes();
	}
}

template <class NODE>
class IsNotCancelled
{
public:
	bool operator()( const NODE * pNode )
	{
		return !pNode->IsCancelled();
	}
};

template <class TIME_STAMP>
void FvTimeQueueT< TIME_STAMP >::PurgeCancelledNodes()
{
	typename PriorityQueue::Container & container = m_kTimeQueue.container();

	typename PriorityQueue::Container::iterator newEnd =
		std::partition( container.begin(), container.end(),
			IsNotCancelled< Node >() );

	for (typename PriorityQueue::Container::iterator iter = newEnd;
		iter != container.end();
		++iter)
	{
		delete *iter;
	}

	m_iNumCancelled -= (container.end() - newEnd);
	container.erase( newEnd, container.end() );
	m_kTimeQueue.heapify();

	FV_ASSERT( (m_iNumCancelled == 0) || (m_iNumCancelled == 1) );
}

template <class TIME_STAMP>
void FvTimeQueueT< TIME_STAMP >::Process( TimeStamp now )
{
	this->CheckTimeSanity( now );

	while ((!m_kTimeQueue.empty()) && (
		m_kTimeQueue.top()->m_kTime <= now ||
		m_kTimeQueue.top()->IsCancelled()))
	{
		Node * pNode = m_pkProcessingNode = m_kTimeQueue.top();
		m_kTimeQueue.pop();

		if (!pNode->IsCancelled())
		{
			pNode->m_eState = STATE_EXECUTING;
			pNode->m_pkHandler->HandleTimeout( (FvTimeQueueID)pNode, pNode->m_pkUser );

			if (pNode->m_kInterval == 0)
				this->Cancel( (FvTimeQueueID)pNode );
		}

		if (pNode->IsCancelled())
		{
			FV_ASSERT( m_iNumCancelled > 0 );

			--m_iNumCancelled;
			delete pNode;
		}
		else
		{
			pNode->m_kTime = now + pNode->m_kInterval;
			pNode->m_eState = STATE_PENDING;
			m_kTimeQueue.push( pNode );
		}
	}

	m_pkProcessingNode = NULL;
	m_kLastProcessTime = now;
}

template <class TIME_STAMP>
bool FvTimeQueueT< TIME_STAMP >::Legal( FvTimeQueueID id ) const
{
	typedef Node * const * NodeIter;

	Node * pNode = (Node*)id;

	if (pNode == NULL) return false;
	if (pNode == m_pkProcessingNode) return true;

	NodeIter begin = &m_kTimeQueue.top();
	NodeIter end = begin + m_kTimeQueue.size();
	for (NodeIter it = begin; it != end; it++)
		if (*it == pNode) return true;

	return false;
}

template <class TIME_STAMP>
TIME_STAMP FvTimeQueueT< TIME_STAMP >::NextEXP( TimeStamp now ) const
{
	if (m_kTimeQueue.size() == 0)
		return 0;
	else
		return m_kTimeQueue.top()->m_kTime - now;
}

template <class TIME_STAMP>
bool FvTimeQueueT< TIME_STAMP >::GetTimerInfo( FvTimeQueueID id,
					TimeStamp &			time,
					TimeStamp &			interval,
					FvTimeQueueHandler *&	pHandler,
					void * &			pUser ) const
{
	Node * pNode = (Node*)id;

	if (!pNode->IsCancelled())
	{
		time = pNode->m_kTime;
		interval = pNode->m_kInterval;
		pHandler = pNode->m_pkHandler;
		pUser = pNode->m_pkUser;

		return true;
	}

	return false;
}

template <class TIME_STAMP>
void FvTimeQueueT< TIME_STAMP >::CheckTimeSanity( TimeStamp now )
{
	if (m_kLastProcessTime && now < m_kLastProcessTime && !m_kTimeQueue.empty())
	{
		typename PriorityQueue::Container & container = m_kTimeQueue.container();

		TimeStamp timeOffset = now - container[0]->m_kTime;

		FV_ERROR_MSG( "FvTimeQueue::CheckTimeSanity: Adjusting by "PRIu64"\n",
				   FvUInt64(timeOffset) );

		typename PriorityQueue::Container::iterator newEnd =
			std::partition( container.begin(), container.end(),
				IsNotCancelled< Node >() );

		for (typename PriorityQueue::Container::iterator iter = newEnd;
			iter != container.end();
			++iter)
		{
			(*iter)->m_kTime += timeOffset;
		}
	}
}


template <class TIME_STAMP>
FvTimeQueueT< TIME_STAMP >::Node::Node( TimeStamp _startTime, TimeStamp _interval,
		FvTimeQueueHandler * _pHandler, void * _pUser ) :
	m_kTime( _startTime ),
	m_kInterval( _interval ),
	m_eState( STATE_PENDING ),
	m_pkHandler( _pHandler ),
	m_pkUser( _pUser )
{
}

template <class TIME_STAMP>
FvTimeQueueT< TIME_STAMP >::Node::~Node()
{
}

template <class TIME_STAMP>
void FvTimeQueueT< TIME_STAMP >::Node::Cancel( FvTimeQueueID id )
{
	m_eState = STATE_CANCELLED;

	if (m_pkHandler)
	{
		m_pkHandler->OnRelease( id, m_pkUser );
		m_pkHandler = NULL;
	}
}