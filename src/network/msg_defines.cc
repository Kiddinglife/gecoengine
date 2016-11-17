#include "msg_defines.h"

DECLARE_DEBUG_COMPONENT2( "Network", 0 );

static stat_entries_t entries_;

/**
 *	This method initialises a single stat instance.
 */
void initRatesOfChangeForStat( uint_stat& stat )
{
	stat_entries_t::const_iterator iter = entries_.begin();
	stat_entries_t::const_iterator end = entries_.end();
	while (iter != end)
	{
		stat.monitorRateOfChange(iter->second);
		++iter;
	}
}

/**
 *	This method initialises a collection of stat instances.
 */
void initRatesOfChangeForStats( const uint_stat::container_type & stats )
{
	uint_stat::container_type::const_iterator iter = stats.begin();
	uint_stat::container_type::const_iterator end = stats.end();
	while (iter != end)
	{
		initRatesOfChangeForStat(*iter);
		++iter;
	}
}

/**
 *	This method adds a Watcher that inspects the total value of a stat.
 */
void addTotalWatcher(geco_watcher_director_t* pWatcher, const char * name, uint_stat& stat )
{
	char buf[ 256 ];
	snprintf( buf, sizeof( buf ), "totals/%s", name );
	//GECO_WATCH(buf, stat, CAST_METHOD_R(uint, uint_stat, total),"HELLO");
	//geco_watcher_base_t* ptr = new method_watcher_t<uint, uint_stat>(stat, &uint_stat::total,NULL,buf);
//	pWatcher->add_watcher( buf, new method_watcher_t<, OBJECT_TYPE>(rObject, getMethod,setMethod, path));
}


connection_stats_t::connection_stats_t(uint connid) :
		connection_id_(connid), pStats_( NULL), numBytesReceived_(pStats_), numPacketsReceived_(
				pStats_), numDuplicatePacketsReceived_(pStats_), numPacketsReceivedOffChannel_(
				pStats_), numBundlesReceived_(pStats_), numMessagesReceived_(
				pStats_), numOverheadBytesReceived_(pStats_), numCorruptedPacketsReceived_(
				pStats_), numCorruptedBundlesReceived_(pStats_), lastGatherTime_(
				0), lastTxQueueSize_(0), lastRxQueueSize_(0), maxTxQueueSize_(
				0), maxRxQueueSize_(0)
//	,mercuryTimer_(),
//	systemTimer_(),
{
//StatWatcherCreator::initRatesOfChangeForStats( *pStats_ );

}

void connection_stats_t::updateStatAverages(double elapsedTime)
{
	if (pStats_)
	{
		uint_stat::container_type::iterator iter = pStats_->begin();
		uint_stat::container_type::iterator end = pStats_->end();
		while (iter != end)
		{
			(*iter).tick(elapsedTime);
			++iter;
		}
	}
}

void connection_stats_t::update_stats()
{
	uint64 currTime = gettimestamp();
	// Wait at least a second.
	if (currTime >= (lastGatherTime_ + stamps_per_sec()))
	{
		// Update stat averages
		double elapsedTime = (currTime - lastGatherTime_)
				/ stamps_per_sec_double();
		this->updateStatAverages(elapsedTime);
		lastGatherTime_ = currTime;

		// @TODO ask channel for lastTxQueueSize_ and lastRxQueueSize_
		//mtra_get_queue_sizes(this->connection_id_,&lastTxQueueSize_,&lastRxQueueSize_ );

		// Warn if the buffers are getting fuller
		if ((lastTxQueueSize_ > maxTxQueueSize_) && (lastTxQueueSize_ > 128*1024))
		{
			WARNING_MSG("send queue peaked at new max (%d bytes)\n",lastTxQueueSize_);
		}
		if ((lastRxQueueSize_ > maxRxQueueSize_) && (lastRxQueueSize_ > 1024*1024))
		{
			WARNING_MSG( "receive queue peaked at new max (%d bytes)\n",lastRxQueueSize_ );
		}

		// update maxs
		maxTxQueueSize_ = std::max( lastTxQueueSize_, maxTxQueueSize_ );
		maxRxQueueSize_ = std::max( lastRxQueueSize_, maxRxQueueSize_ );
	}
}
