
//#include "e:\Work\FuturnVision\trunk\CoreLibs\FvLogicCommon\FvLogBus.h"


template<class State, class GoalState>
AStar<State, GoalState>::AStar() :
	iter_(NULL),
	start_(NULL),
	open_(/* cmp_f(), AStarVector( buffer_ ) */)
{}

template<class State, class GoalState>
AStar<State, GoalState>::~AStar()
{
	set_.reset();
}

namespace
{
	bool updateMaxCount( const int value )
	{
		static int highest = 0;
		static bool printed = 1;

		if (value == -1)
		{
			if (!printed)
			{
				FV_TRACE_MSG( "AStar::search: "
					"New highest open_.size() = %d\n", highest );
				printed = true;
			}
			return true;
		}

		if (value > highest)
		{
			highest = value;
			printed = false;
		}

		if (value > 9999)
			return false;

		return true;
	}
}
//
//template<class State, class GoalState>
//bool AStar<State, GoalState>::search(const State& start, const GoalState& goal, float maxDistance )
//{
//	IntState* pCurrent;
//	IntState* pAdjacency;
//	IntState key;
//	float g, f;
//	bool checkMaxDist = maxDistance > 0.f;
//
//	pCurrent = IntState::alloc();
//	pCurrent->ext = start;
//	pCurrent->g = 0;
//	pCurrent->h = start.DistanceToGoal(goal);
//	pCurrent->f = pCurrent->g + pCurrent->h;
//	pCurrent->pParent = NULL;
//	pCurrent->pChild = NULL;
//	start_ = pCurrent;
//
//	set_.insert(pCurrent);
//	open_.push(pCurrent);
//
//	this->infiniteLoopProblem = false;
//
//	
//	while(!open_.empty())
//	{
//		if (!updateMaxCount(open_.size()))
//		{
//			updateMaxCount(-1);
//			FV_ERROR_MSG( "AStar search has too many open states!\n" );
//			this->infiniteLoopProblem = true;
//		}
//
//		pCurrent = open_.top();
//		open_.pop();
//
//		if(pCurrent->ext.IsGoal(goal))
//		{
//			int safetyCount = 0;
//			while(pCurrent->pParent && (++safetyCount<10000) )
//			{
//				pCurrent->pParent->pChild = pCurrent;
//				pCurrent = pCurrent->pParent;
//			}
//			updateMaxCount(-1);
//			if ( safetyCount < 10000 )
//			{
//				return true;
//			}
//			else
//			{
//				FV_ERROR_MSG( "AStar search entered infinite loop!\n" );
//				this->infiniteLoopProblem = true;
//				return false;
//			}
//		}
//
//		typename State::adjacency_iterator adjIter =
//			pCurrent->ext.AdjacenciesBegin();
//
//		for(; adjIter != pCurrent->ext.AdjacenciesEnd(); ++adjIter)
//		{
//			if(!pCurrent->ext.GetAdjacency( adjIter, key.ext, goal ))
//				continue;
//
//			if(key.ext.Compare(start_->ext) == 0)
//				continue;
//
//			if (checkMaxDist)
//			{
//				if (key.ext.DistanceToGoal( start ) > maxDistance)
//				{
//					continue;
//				}
//			}
//
//			g = pCurrent->g + key.ext.DistanceFromParent();
//			f = g + key.ext.DistanceToGoal( goal );
//
//			IntState* setIter = set_.find( &key );
//
//			if( setIter != NULL )
//			{
//				if(setIter->f <= f)
//				{
//					continue;
//				}
//
//				pAdjacency = setIter;
//				set_.erase(&key);
//
//			}
//			else
//			{
//				pAdjacency = IntState::alloc();
//			}
//			
//			IntState* p = pCurrent;
//
//			while (p && p != pAdjacency)
//			{
//				p = p->pParent;
//			}
//
//			if (p)// p == pAdjacency, pAdjacency 是 pCurrrent的祖先
//			{
//				continue;
//			}
//
//			pAdjacency->ext = key.ext;
//			pAdjacency->g = g;
//
//			pAdjacency->h = pAdjacency->ext.DistanceToGoal(goal);
//
//			pAdjacency->f = f;
//			pAdjacency->pParent = pCurrent;
//			pAdjacency->pChild = NULL;
//			
//			open_.push(pAdjacency);
//		}
//		set_.insert(pCurrent);
//	}
//
//	updateMaxCount(-1);
//	return false;
//}

template<class State, class GoalState>
bool AStar<State, GoalState>::search(State& start, GoalState& goal, float maxDistance )
{
	IntState* pCurrent;
	IntState* pAdjacency;
	IntState key;
	bool checkMaxDist = maxDistance > 0.f;

	pCurrent = IntState::alloc();
	pCurrent->ext = start;
	pCurrent->g = 0;
	pCurrent->h = start.DistanceToGoal(goal);
	pCurrent->f = pCurrent->g + pCurrent->h;
	pCurrent->pParent = NULL;
	pCurrent->pChild = NULL;
	start_ = pCurrent;

	open_.Push(pCurrent);
	set_.insert(pCurrent);

	this->infiniteLoopProblem = false;


	while(!open_.Empty())
	{
		if (!updateMaxCount(open_.Size()))
		{
			updateMaxCount(-1);
			FV_ERROR_MSG( "AStar search has too many open states!\n" );
			this->infiniteLoopProblem = true;
		}

		pCurrent = open_.Pop();


		if(pCurrent->ext.IsGoal(goal))
		{
			int safetyCount = 0;
			while(pCurrent->pParent && (++safetyCount<10000) )
			{
				pCurrent->pParent->pChild = pCurrent;
				pCurrent = pCurrent->pParent;
			}
			updateMaxCount(-1);
			if ( safetyCount < 10000 )
			{
				return true;
			}
			else
			{
				FV_ERROR_MSG( "AStar search entered infinite loop!\n" );
				this->infiniteLoopProblem = true;
				return false;
			}
		}

		typename State::adjacency_iterator adjIter =
			pCurrent->ext.AdjacenciesBegin();

		//FvLogBus::CritOk("遍历[%d %d] [F%f, G%f, H%f] 临边数量[%d]", pCurrent->ext.GetX(), pCurrent->ext.GetY(), pCurrent->f, pCurrent->g, pCurrent->h, pCurrent->ext.WaypointCount());

		for(; adjIter != pCurrent->ext.AdjacenciesEnd(); ++adjIter)
		{
			if(!pCurrent->ext.GetAdjacency( adjIter, key.ext, goal ))
				continue;

			if(key.ext.Compare(start_->ext) == 0)
				continue;

			if (checkMaxDist)
			{
				if (key.ext.DistanceToGoal( start ) > maxDistance)
				{
					continue;
				}
			}

			const float g = pCurrent->g + key.ext.DistanceFromParent();
			const float f = g + key.ext.DistanceToGoal( goal );

			IntState* setIter = set_.find( &key );
			if( setIter != NULL )
			{
				if(setIter->f <= f)
				{
					continue;
				}
				else
				{
					pAdjacency = setIter;
					set_.erase(&key);
				}
			}
			else
			{
				pAdjacency = IntState::alloc();
			}

			IntState* p = pCurrent;

			pAdjacency->ext = key.ext;
			pAdjacency->g = g;

			pAdjacency->h = pAdjacency->ext.DistanceToGoal(goal);

			pAdjacency->f = f;
			pAdjacency->pParent = pCurrent;
			pAdjacency->pChild = NULL;

			open_.Push(pAdjacency);
			set_.insert(pAdjacency);

			//FvLogBus::CritOk("搜索[%d %d] [F%f, G%f, H%f]", pAdjacency->ext.GetX(), pAdjacency->ext.GetY(), pAdjacency->f, pAdjacency->g, pAdjacency->h);

		}
	}

	updateMaxCount(-1);
	set_.reset();

	return false;
}

template<class State, class GoalState>
State* AStar<State, GoalState>::first()
{
	iter_ = start_;
	return iter_ ? &iter_->ext : NULL;
}

template<class State, class GoalState>
State* AStar<State, GoalState>::next()
{
	if(iter_)
		iter_ = iter_->pChild;
	return iter_ ? &iter_->ext : NULL;
}

template<class State, class GoalState>
void AStar<State, GoalState>::reset()
{
	set_.reset();
	open_.Clear();
	iter_ = NULL;
	start_ = NULL;
}