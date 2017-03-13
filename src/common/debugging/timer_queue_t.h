/*
* Geco Gaming Company
* All Rights Reserved.
* Copyright (c)  2016 GECOEngine.
*
* GECOEngine is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* GECOEngine is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.

* You should have received a copy of the GNU Lesser General Public License
* along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef TIMER_QUEUE_HPP
#define TIMER_QUEUE_HPP

#include <map>
#include <algorithm>
#include <vector>
#include <functional>

#include "timestamp.h"

// -----------------------------------------------------------------------------
// Section: TimeQueueT DELC
// -----------------------------------------------------------------------------

class TimerHandler;
class TimeQueueBase;

/**
*	This class is the base class for the nodes of the time queue.
*/
class TimeQueueNode
{
public:
	TimeQueueNode(TimeQueueBase & owner,
		TimerHandler * pHandler,
		void * pUserData);

	void cancel();

	void * pUserData() const { return pUserData_; }
	bool isCancelled() const { return state_ == STATE_CANCELLED; }

protected:
	bool isExecuting() const { return state_ == STATE_EXECUTING; }

	/// This enumeration is used to describe the current state of an element on
	/// the queue.
	enum State
	{
		STATE_PENDING,
		STATE_EXECUTING,
		STATE_CANCELLED
	};

	TimeQueueBase & owner_;
	TimerHandler * pHandler_;
	void * pUserData_;

	State state_;
};

/**
*	This class is a handle to a timer added to TimeQueue.
*/
class TimerID
{
public:
	explicit TimerID(TimeQueueNode * pNode = NULL) : pNode_(pNode) {}

	void cancel()
	{
		if (pNode_ != NULL)
		{
			TimeQueueNode* pNode = pNode_;

			pNode_ = NULL;
			pNode->cancel();
		}
	}

	void clearWithoutCancel() { pNode_ = NULL; }

	bool isSet() const { return pNode_ != NULL; }

	friend bool operator==(TimerID h1, TimerID h2);

	// This should only be used by TimeQueueT
	TimeQueueNode * pNode() const { return pNode_; }

private:
	TimeQueueNode * pNode_;
};

inline bool operator==(TimerID h1, TimerID h2)
{
	return h1.pNode_ == h2.pNode_;
}

/**
 * This is an interface which must be derived from in order to receive time queue events.
 * @note as TimerHandler is stored as a pointer in timer queue, so it is a good practise 
 * to  use it as class memeber in firing class object
*/
class TimerHandler
{
public:
	TimerHandler() : numTimesRegistered_(0) {}
	virtual ~TimerHandler()
	{
		GECO_ASSERT_DEV(numTimesRegistered_ == 0);
	};

	/**
	* 	This method is called when a timeout expires.
	*
	* 	@param	handle	The handle returned when the event was added.
	* 	@param	pUser	The user data passed in when the event was added.
	*/
	std::function<void(TimerID id, void * pUser)> handleTimeout;
	std::function<void(TimerID id, void * pUser)> onRelease;

private:
	friend class TimeQueueNode;
	void incTimerRegisterCount() { ++numTimesRegistered_; }
	void decTimerRegisterCount() { --numTimesRegistered_; }
	void release(TimerID handle, void * pUser)
	{
		this->decTimerRegisterCount();
		this->onRelease(handle, pUser);
	}

	int numTimesRegistered_;
};

class TimeQueueBase;


/**
*	This class is the base class for TimeQueueT. It allows TimeQueueNode to have
*	a reference to its owner that is not a template.
*/
class TimeQueueBase
{
public:
	virtual void onCancel() = 0;
};

/**
* 	This class implements a time queue, measured in game ticks. The logic is
* 	basically stolen from Mercury, but it is intended to be used as a low
* 	resolution timer.  Also, timestamps should be synchronised between servers.
*/
template< class TIME_STAMP >
class TimeQueueT : public TimeQueueBase
{
public:
	TimeQueueT();
	~TimeQueueT();

	void clear(bool shouldCallCancel = true);

	/// This is the unit of time used by the time queue
	typedef TIME_STAMP TimeStamp;

	/// Schedule an event
	TimerID	add(TimeStamp startTime, TimeStamp interval,
		TimerHandler* pHandler, void * pUser);

	/// Process all events older than or equal to now
	int		process(TimeStamp now);

	/// Determine whether or not the given handle is legal (slow)
	bool		legal(TimerID handle) const;

	/// Return the number of timestamps until the first node expires.  This will
	/// return 0 if size() == 0, so you must check this first
	TIME_STAMP nextExp(TimeStamp now) const;

	/// Returns the number of timers in the queue
	inline uint32 size() const { return timeQueue_.size(); }

	/// Returns whether the time queue is empty.
	inline bool empty() const { return timeQueue_.empty(); }

	bool		getTimerInfo(TimerID handle,
		TimeStamp &			time,
		TimeStamp &			interval,
		void * &			pUser) const;

	TIME_STAMP timerDeliveryTime(TimerID handle) const;
	TIME_STAMP timerIntervalTime(TimerID handle) const;
	TIME_STAMP & timerIntervalTime(TimerID handle);

private:
	void purgeCancelledNodes();
	void onCancel();

	/// This structure represents one event in the time queue.
	class Node : public TimeQueueNode
	{
	public:
		Node(TimeQueueBase & owner, TimeStamp startTime, TimeStamp interval,
			TimerHandler * pHandler, void * pUser);

		TIME_STAMP time() const { return time_; }
		TIME_STAMP interval() const { return interval_; }
		TIME_STAMP & intervalRef() { return interval_; }

		TIME_STAMP deliveryTime() const;

		void triggerTimer();

	private:
		TimeStamp			time_;
		TimeStamp			interval_;

		Node(const Node &);
		Node & operator=(const Node &);
	};

	/// Comparison object for the priority queue.
	class Comparator
	{
	public:
		bool operator()(const Node* a, const Node* b)
		{
			return a->time() > b->time();
		}
	};

	/**
	*	This class implements a priority queue. std::priority_queue is not used
	*	so that access to the underlying container can be gotten.
	*/
	class PriorityQueue
	{
	public:
		typedef std::vector< Node * > Container;

		typedef typename Container::value_type value_type;
		typedef typename Container::size_type size_type;

		bool empty() const { return container_.empty(); }
		size_type size() const { return container_.size(); }

		const value_type & top() const { return container_.front(); }

		void push(const value_type & x)
		{
			container_.push_back(x);
			std::push_heap(container_.begin(), container_.end(),
				Comparator());
		}

		void pop()
		{
			std::pop_heap(container_.begin(), container_.end(), Comparator());
			container_.pop_back();
		}

		/**
		*	Note: This leaves the queue in a bad state.
		*/
		Node * unsafePopBack()
		{
			Node * pNode = container_.back();
			container_.pop_back();
			return pNode;
		}

		/**
		*	This method returns the underlying container. If this container is
		*	modified, heapify should be called to return the PriorityQueue to
		*	be a valid priority queue.
		*/
		Container & container() { return container_; }

		/**
		*	This method enforces the underlying container to be in a valid heap
		*	ordering.
		*/
		void heapify()
		{
			std::make_heap(container_.begin(), container_.end(),
				Comparator());
		}

	private:
		Container container_;
	};

	PriorityQueue	timeQueue_;
	Node * 			pProcessingNode_;
	TimeStamp 		lastProcessTime_;
	int				numCancelled_;

	// Cannot be copied.
	TimeQueueT(const TimeQueueT &);
	TimeQueueT & operator=(const TimeQueueT &);
};

typedef TimeQueueT< uint32 > TimeQueue;
typedef TimeQueueT< uint64 > TimeQueue64;


// -----------------------------------------------------------------------------
// Section: TimeQueueT IMPL
// -----------------------------------------------------------------------------

/**
*	This is the constructor.
*/
template< class TIME_STAMP >
TimeQueueT< TIME_STAMP >::TimeQueueT() :
	timeQueue_(),
	pProcessingNode_(NULL),
	lastProcessTime_(0),
	numCancelled_(0)
{
}

/**
* 	This is the destructor. It walks the queue and cancels events,
*	then deletes them. If the cancellation of events
*/
template <class TIME_STAMP>
TimeQueueT< TIME_STAMP >::~TimeQueueT()
{
	this->clear();
}


/**
*	This method cancels all events in this queue.
*/
template <class TIME_STAMP>
void TimeQueueT< TIME_STAMP >::clear(bool shouldCallCancel)
{
	// Make sure we don't loop forever
	int maxLoopCount = timeQueue_.size();

	while (!timeQueue_.empty())
	{
		Node * pNode = timeQueue_.unsafePopBack();

		if (!pNode->isCancelled() && shouldCallCancel)
		{
			--numCancelled_; // Since it's removed from the queue
			pNode->cancel();

			if (--maxLoopCount == 0)
			{
				shouldCallCancel = false;
			}
		}
		else if (pNode->isCancelled())
		{
			--numCancelled_;
		}

		delete pNode;
	}

	numCancelled_ = 0;

	// Clear the queue
	timeQueue_ = PriorityQueue();
}


/**
*	This method adds an event to the time queue. If interval is zero,
*	the event will happen once and will then be deleted. Otherwise,
*	the event will be fired repeatedly.
*
*	@param startTime	Time of the initial event, in game ticks
*	@param interval		Number of game ticks between subsequent events
*	@param pHandler 	Object that is to receive the event
*	@param pUser		User data to be passed with the event.
*	@return				A handle to the new event.
*/
template <class TIME_STAMP>
TimerID TimeQueueT< TIME_STAMP >::add(TimeStamp startTime,
	TimeStamp interval, TimerHandler * pHandler, void * pUser)
{
	Node * pNode = new Node(*this, startTime, interval, pHandler, pUser);
	timeQueue_.push(pNode);

	return TimerID(pNode);
}


/**
*	This method is called when a timer has been cancelled.
*/
template <class TIME_STAMP>
void TimeQueueT< TIME_STAMP >::onCancel()
{
	++numCancelled_;

	// If there are too many cancelled timers in the queue (more than half),
	// these are flushed from the queue immediately.

	if (numCancelled_ * 2 > int(timeQueue_.size()))
	{
		this->purgeCancelledNodes();
	}
}


/**
*	This class is used by purgeCancelledNodes to partition the timers and
*	separate those that have been cancelled.
*/
template <class NODE>
class IsNotCancelled
{
public:
	bool operator()(const NODE * pNode)
	{
		return !pNode->isCancelled();
	}
};


/**
*	This method removes all cancelled timers from the priority queue. Generally,
*	cancelled timers wait until they have reached the top of the queue before
*	being deleted.
*/
template <class TIME_STAMP>
void TimeQueueT< TIME_STAMP >::purgeCancelledNodes()
{
	typename PriorityQueue::Container & container = timeQueue_.container();

	typename PriorityQueue::Container::iterator newEnd =
		std::partition(container.begin(), container.end(),
			IsNotCancelled< Node >());

	for (typename PriorityQueue::Container::iterator iter = newEnd;
		iter != container.end();
		++iter)
	{
		delete *iter;
	}

	const int numPurged = (container.end() - newEnd);
	numCancelled_ -= numPurged;
	// numCancelled_ will be 1 when we're in the middle of processing a
	// once-off timer.
	GECO_ASSERT((numCancelled_ == 0) || (numCancelled_ == 1));

	container.erase(newEnd, container.end());
	timeQueue_.heapify();
}


/**
*	This method processes the time queue and dispatches events.
*	All events with a timestamp earlier than the given one are
*	processed.
*
*	@param now		Process events earlier than or exactly on this.
*
*	@return The number of timers that fired.
*/
template <class TIME_STAMP>
int TimeQueueT< TIME_STAMP >::process(TimeStamp now)
{
	int numFired = 0;

	while ((!timeQueue_.empty()) && (
		timeQueue_.top()->time() <= now ||
		timeQueue_.top()->isCancelled()))
	{
		Node * pNode = pProcessingNode_ = timeQueue_.top();
		timeQueue_.pop();

		if (!pNode->isCancelled())
		{
			++numFired;
			pNode->triggerTimer();
		}

		if (!pNode->isCancelled())
		{
			timeQueue_.push(pNode);
		}
		else
		{
			delete pNode;

			GECO_ASSERT(numCancelled_ > 0);
			--numCancelled_;
		}
	}

	pProcessingNode_ = NULL;
	lastProcessTime_ = now;

	return numFired;
}


/**
*	This method determines whether or not the given handle is legal.
*/
template <class TIME_STAMP>
bool TimeQueueT< TIME_STAMP >::legal(TimerID handle) const
{
	typedef Node * const * NodeIter;

	Node * pNode = static_cast<Node*>(handle.pNode());

	if (pNode == NULL)
	{
		return false;
	}

	if (pNode == pProcessingNode_)
	{
		return true;
	}

	NodeIter begin = &timeQueue_.top();
	NodeIter end = begin + timeQueue_.size();

	for (NodeIter it = begin; it != end; it++)
	{
		if (*it == pNode)
		{
			return true;
		}
	}

	return false;
}

/**
*	This method returns the time until the next timer goes off.
*/
template <class TIME_STAMP>
TIME_STAMP TimeQueueT< TIME_STAMP >::nextExp(TimeStamp now) const
{
	if (timeQueue_.empty() ||
		now > timeQueue_.top()->time())
	{
		return 0;
	}

	return timeQueue_.top()->time() - now;
}


/**
*	This method returns information associated with the timer with the input
*	handler.
*/
template <class TIME_STAMP>
bool TimeQueueT< TIME_STAMP >::getTimerInfo(TimerID handle,
	TimeStamp &			time,
	TimeStamp &			interval,
	void * &			pUser) const
{
	Node * pNode = static_cast<Node *>(handle.pNode());

	if (!pNode->isCancelled())
	{
		time = pNode->time();
		interval = pNode->interval();
		pUser = pNode->pUserData();

		return true;
	}

	return false;
}


/**
*	This method returns the time that the given timer handle will be delivered,
*	in timestamps.
*/
template <class TIME_STAMP>
TIME_STAMP
TimeQueueT< TIME_STAMP >::timerDeliveryTime(TimerID handle) const
{
	Node * pNode = static_cast<Node *>(handle.pNode());
	return pNode->deliveryTime();
}


/**
*	This method returns the time between deliveries of the given timer handle,
*	in timestamps.
*/
template <class TIME_STAMP>
TIME_STAMP
TimeQueueT< TIME_STAMP >::timerIntervalTime(TimerID handle) const
{
	Node * pNode = static_cast<Node *>(handle.pNode());
	return pNode->interval();
}


/**
*	This method returns the time between deliveries of the given timer handle,
*	in timestamps. The value returned may be modified.
*/
template <class TIME_STAMP>
TIME_STAMP & TimeQueueT< TIME_STAMP >::timerIntervalTime(TimerID handle)
{
	Node * pNode = static_cast<Node *>(handle.pNode());
	return pNode->intervalRef();
}


// -----------------------------------------------------------------------------
// Section: TimeQueueNode DEFI
// -----------------------------------------------------------------------------

/**
*	Constructor.
*/
inline TimeQueueNode::TimeQueueNode(TimeQueueBase & owner,
	TimerHandler * pHandler, void * pUserData) :
	owner_(owner),
	pHandler_(pHandler),
	pUserData_(pUserData),
	state_(STATE_PENDING)
{
	pHandler->incTimerRegisterCount();
}


/**
*	This method cancels the timer associated with this node.
*/
inline
void TimeQueueNode::cancel()
{
	if (this->isCancelled())
	{
		return;
	}

	GECO_ASSERT((state_ == STATE_PENDING) || (state_ == STATE_EXECUTING));

	state_ = STATE_CANCELLED;

	if (pHandler_)
	{
			pHandler_->release(TimerID(this), pUserData_);
		pHandler_ = NULL;
	}

	owner_.onCancel();
}


// -----------------------------------------------------------------------------
// Section: TimeQueueT::Node
// -----------------------------------------------------------------------------

/**
*	Constructor
*/
template <class TIME_STAMP>
TimeQueueT< TIME_STAMP >::Node::Node(TimeQueueBase & owner,
	TimeStamp startTime, TimeStamp interval,
	TimerHandler * _pHandler, void * _pUser) :
	TimeQueueNode(owner, _pHandler, _pUser),
	time_(startTime),
	interval_(interval)
{
}


/**
*	This method cancels the time queue node.
*/
template <class TIME_STAMP>
TIME_STAMP TimeQueueT< TIME_STAMP >::Node::deliveryTime() const
{
	return this->isExecuting() ? (time_ + interval_) : time_;
}


/**
*	This method triggers the timer assoicated with this node. It also updates
*	the state for repeating timers.
*/
template <class TIME_STAMP>
void TimeQueueT< TIME_STAMP >::Node::triggerTimer()
{
	if (!this->isCancelled())
	{
		state_ = STATE_EXECUTING;

		pHandler_->handleTimeout(TimerID(this), pUserData_);

		if ((interval_ == 0) && !this->isCancelled())
		{
			this->cancel();
		}
	}

	// This event could have been cancelled within the callback.

	if (!this->isCancelled())
	{
		time_ += interval_;
		state_ = STATE_PENDING;
	}
}

#endif
