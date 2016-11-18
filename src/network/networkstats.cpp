/*
 * networkstats.cpp
 *
 *  Created on: 18Nov.,2016
 *      Author: jackiez
 */

#include "networkstats.h"

typedef eastl::pair< std::string, float > stat_entry_t;
typedef eastl::vector< stat_entry_t > stat_entries_t;

template <class TYPE>
class stat_watcher_factory_t
{
	stat_entries_t mstats_entries_;

public:
	stat_watcher_factory_t()
	{
		InitStatWatcher();
	}

	/**
	*	This method populates the input watcher with watchers that are
	*	appropriate to inspect the input stat.
	*/
	void AddStatWatchers(geco_watcher_director_t& pwatchers_, const char * name, intrusive_stat_rate_of_change_t< TYPE>& stat)
	{
		addTotalWatcher(&pwatchers_, name, stat);
		addAvgWatchers(&pwatchers_, name, stat);
	}

	/**
	*	This method is used to add a rate of change. Stats that are initialised
	*	with this instance will have an appropriate rate added with the watcher
	*	appropriately named.
	*
	*	@param name The name of the directory to place the Watcher in.
	*/
	void initRateOfChange(const char * name, float numSamples)
	{
		mstats_entries_.push_back(eastl::make_pair(name, numSamples));
	}

	/**
	*	This method initialises a single stat instance.
	*/
	void initRatesOfChangeForStat(intrusive_stat_rate_of_change_t< TYPE>& stat)
	{
		stat_entries_t::iterator iter = mstats_entries_.begin();
		stat_entries_t::iterator end = mstats_entries_.end();
		while (iter != end)
		{
			stat.monitorRateOfChange(iter->second);
			++iter;
		}
	}

	/**
	*	This method initialises a collection of stat instances.
	*/
	void initRatesOfChangeForStats(eastl::intrusive_list<intrusive_stat_rate_of_change_t<TYPE>> & stats)
	{
		typedef typename eastl::intrusive_list<intrusive_stat_rate_of_change_t<TYPE>>::iterator cit;
		cit iter = stats.begin();
		cit end = stats.end();
		while (iter != end)
		{
			initRatesOfChangeForStat(*iter);
			++iter;
		}
	}

private:
	/**
	*	This method adds a Watcher that inspects the total value of a stat.
	*/
	void addTotalWatcher(geco_watcher_director_t* pwatchers_, const char * name, intrusive_stat_rate_of_change_t< TYPE>& stat)
	{
		char buf[256];
		geco_snprintf(buf, sizeof(buf), "totals/%s", name);
		GECO_WATCH(buf, stat, CAST_METHOD_R(TYPE, intrusive_stat_rate_of_change_t<TYPE>, total), "stat watcher for total val", pwatchers_);
	}

	/**
	*	This method adds Watcher instances that inspects the rates of change associated with a stat.
	*/
	void addAvgWatchers(geco_watcher_director_t* pwatchers_, const char * name, intrusive_stat_rate_of_change_t< TYPE>& stat)
	{
		typedef double& (intrusive_stat_rate_of_change_t<TYPE>::*FunctionPtr)();
		FunctionPtr fns[] =
		{
			&intrusive_stat_rate_of_change_t<TYPE>::getRateOfChange0,
			&intrusive_stat_rate_of_change_t<TYPE>::getRateOfChange1,
			&intrusive_stat_rate_of_change_t<TYPE>::getRateOfChange2,
			&intrusive_stat_rate_of_change_t<TYPE>::getRateOfChange3,
			&intrusive_stat_rate_of_change_t<TYPE>::getRateOfChange4, // More than needed
		};

		GECO_ASSERT(sizeof(fns) / sizeof(fns[0]) >= mstats_entries_.size());

		char buf[256];
		geco_watcher_base_t* ptr;
		stat_entries_t::size_type count = mstats_entries_.size();
		for(stat_entries_t::size_type i =0; i < count;i++)
		{
			stat_entry_t & entry = mstats_entries_[i];
			geco_snprintf(buf, sizeof(buf), "%s/%sPerSecond", entry.first.c_str(), name);
			ptr = new method_watcher_t<double, intrusive_stat_rate_of_change_t<TYPE> >(fns[i], 0, buf);
			if (pwatchers_->add_watcher(buf, *ptr, NULL))
			{
				geco_snprintf(buf, sizeof(buf), "stat watcher for %s avg val", entry.first.c_str());
				ptr->set_comment( "%s avg val");
			}
			else
				GECO_EXIT("addAvgWatchers() falied !!");
		}
	}

	/**
	*	This class is the standard for initialising StatWithRatesOfChange instances.
	*/
	void InitStatWatcher()
	{
		initRateOfChange("averages/last", 0.f);// real time no wights
		initRateOfChange("averages/short", 60.f);	// 1 minute at 95%
		initRateOfChange("averages/medium", 600.0f);	// 10 minutes at 95%
		initRateOfChange("averages/long", 3600.0f);	// 60 minutes at 95%
	}
};

stat_watcher_factory_t<uint> uint_stat_watcher_factory_;
stat_watcher_factory_t<float> float_stat_watcher_factory_; //not used

network_stats_t::network_stats_t() :
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
}

void network_stats_t::updateStatAverages(double elapsedTime)
{
	stat::container_type::iterator iter = pStats_.begin();
	stat::container_type::iterator end = pStats_.end();
	while (iter != end)
	{
		(*iter).tick(elapsedTime);
		++iter;
	}
}

void network_stats_t::update_stats()
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
	}
}


