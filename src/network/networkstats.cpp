/*
 * networkstats.cpp
 *
 *  Created on: 18Nov.,2016
 *      Author: jackiez
 */

#include "networkstats.h"

static stat_watcher_factory_t<uint> uint_stat_watcher_factory_;
static stat_watcher_factory_t<float> float_stat_watcher_factory_; //not used

stat_watcher_factory_t<uint>& read_uint_stat_watcher_factory()
{
	return uint_stat_watcher_factory_;
}

network_recv_stats_t::network_recv_stats_t() :
	lastGatherTime_(0),
	lastTxQueueSize_(0),
	lastRxQueueSize_(0),
	maxTxQueueSize_(0),
	maxRxQueueSize_(0)
	//	,mercuryTimer_(),
	//	systemTimer_(),
{
	uint_stat_watcher_factory_.initRatesOfChangeForStats(pStats_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "bytesReceived", this->numBytesReceived_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "bytesReceived", this->numBytesReceived_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "packetsReceived", this->numPacketsReceived_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "duplicatePacketsReceived", this->numDuplicatePacketsReceived_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "packetsReceivedOffChannel", this->numPacketsReceivedOffChannel_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "bundlesReceived", this->numBundlesReceived_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "messagesReceived", this->numMessagesReceived_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "overheadBytesReceived", this->numOverheadBytesReceived_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "corruptedPacketsReceived", this->numCorruptedPacketsReceived_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "corruptedBundlesReceived", this->numCorruptedBundlesReceived_);

	GECO_WATCH("socket/transmitQueue", lastTxQueueSize_, WT_READ_ONLY, "sock stat", &pwatchers_);
	GECO_WATCH("socket/receiveQueue", lastRxQueueSize_, WT_READ_ONLY, "sock stat", &pwatchers_);
	GECO_WATCH("socket/maxTransmitQueue", maxTxQueueSize_, WT_READ_ONLY, "sock stat", &pwatchers_);
	GECO_WATCH("socket/maxReceiveQueue", maxRxQueueSize_, WT_READ_ONLY, "sock stat", &pwatchers_);

	//pwatchers_->add_watcher("timing/mercury", ProfileVal::pWatcherStamps(),&mercuryTimer_);
	//pwatchers_->add_watcher("timing/system", ProfileVal::pWatcherStamps(),&systemTimer_);
	//pwatchers_->add_watcher("timingInSeconds/mercury", ProfileVal::pWatcherSeconds(),&mercuryTimer_);
	//pwatchers_->add_watcher("timingInSeconds/system", ProfileVal::pWatcherSeconds(),&systemTimer_);
	//pwatchers_->add_watcher("timingSummary/mercury", ProfileVal::pSummaryWatcher(),&mercuryTimer_);
	//pwatchers_->add_watcher("timingSummary/system", ProfileVal::pSummaryWatcher(),&systemTimer_);
}

void network_recv_stats_t::updateStatAverages(double elapsedTime)
{
	stat::container_type::iterator iter = pStats_.begin();
	static stat::container_type::iterator end = pStats_.end();
	while (iter != end)
	{
		(*iter).tick(elapsedTime);
		++iter;
	}
}

void network_recv_stats_t::update_stats(double elapsedTime) //seconds
{
	//uint64 currTime = gettimestamp();
	// Wait at least a second. replay on timer cb 
	//if (currTime >= (lastGatherTime_ + stamps_per_sec()))
	//{
	// Update stat averages
	//double elapsedTime = (currTime - lastGatherTime_)
	//	/ stamps_per_sec_double();
	this->updateStatAverages(elapsedTime);
	//lastGatherTime_ = currTime;

	// @TODO ask channel for lastTxQueueSize_ and lastRxQueueSize_
	//mtra_get_queue_sizes(this->connection_id_,&lastTxQueueSize_,&lastRxQueueSize_ );

	// Warn if the buffers are getting fuller
	if ((lastTxQueueSize_ > maxTxQueueSize_) && (lastTxQueueSize_ > 128 * 1024))
	{
		WARNING_MSG("send queue peaked at new max (%d bytes)\n", lastTxQueueSize_);
	}
	if ((lastRxQueueSize_ > maxRxQueueSize_) && (lastRxQueueSize_ > 1024 * 1024))
	{
		WARNING_MSG("receive queue peaked at new max (%d bytes)\n", lastRxQueueSize_);
	}

	// update maxs
	maxTxQueueSize_ = std::max(lastTxQueueSize_, maxTxQueueSize_);
	maxRxQueueSize_ = std::max(lastRxQueueSize_, maxRxQueueSize_);
	//}
}

float network_send_stats_t::SEND_STATS_UPDATE_RATE = 1.0f;
network_send_stats_t::network_send_stats_t() /*:mercuryTimer_(),systemTimer_()*/
{
	uint_stat_watcher_factory_.initRatesOfChangeForStats(pStats_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "bytesSent", numBytesSent_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "bytesResent", numBytesResent_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "packetsSent", numPacketsSent_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "packetsResent", numPacketsResent_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "numPiggybacks", numPiggybacks_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "packetsSentOffChannel", numPacketsSentOffChannel_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "bundlesSent", numBundlesSent_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "messagesSent", numMessagesSent_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "reliableMessagesSent", numReliableMessagesSent_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "failedPacketSends", numFailedPacketSend_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "failedBundleSends", numFailedBundleSend_);

	//pwatchers_->add_watcher("timingInSeconds/mercury", ProfileVal::pWatcherSeconds(),&mercuryTimer_);
	//pwatchers_->add_watcher("timingInSeconds/system", ProfileVal::pWatcherSeconds(),&systemTimer_);
	//pwatchers_->add_watcher("timing/mercury", ProfileVal::pWatcherStamps(),&mercuryTimer_);
	//pwatchers_->add_watcher("timing/system", ProfileVal::pWatcherStamps(),&systemTimer_);
}


void network_send_stats_t::update_stats(double elapsedTime)
{
	this->updateStatAverages(elapsedTime);
}

/**
*	This method is called once a second to update the moving averages.
*/
void network_send_stats_t::updateStatAverages(double elapsedTime)
{
	stat::container_type::iterator iter = pStats_.begin();
	static stat::container_type::iterator end = pStats_.end();
	while (iter != end)
	{
		(*iter).tick(elapsedTime);
		++iter;
	}
}

