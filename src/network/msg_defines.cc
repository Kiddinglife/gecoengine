#include "msg_defines.h"
DECLARE_DEBUG_COMPONENT2( "Network", 0 );
stat_entries_t g_stat_entries;
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
		stat::container_type::iterator iter = pStats_->begin();
		stat::container_type::iterator end = pStats_->end();
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