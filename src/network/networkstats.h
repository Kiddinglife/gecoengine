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


/*
 * networkstats.h
 *
 *  Created on: 18Nov.,2016
 *      Author: jackiez
 */

#ifndef SRC_NETWORK_NETWORKSTATS_H_
#define SRC_NETWORK_NETWORKSTATS_H_

#include "../common/ds/eastl/EASTL/utility.h"
#include "../math/stat_rate_of_change.h"
#include "../common/debugging/gecowatchert.h"
#include "../common/ds/eastl/EASTL/vector.h"

template <class TYPE>
class stat_watcher_factory_t;
extern stat_watcher_factory_t<uint>& read_uint_stat_watcher_factory();

template <class TYPE>
class stat_watcher_factory_t
{
	typedef eastl::pair< std::string, float > stat_entry_t;
	typedef eastl::vector< stat_entry_t > stat_entries_t;
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
		static cit end = stats.end();
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
		for (stat_entries_t::size_type i = 0; i < count;i++)
		{
			stat_entry_t & entry = mstats_entries_[i];
			geco_snprintf(buf, sizeof(buf), "%s/%sPerSecond", entry.first.c_str(), name);
			ptr = new method_watcher_t<double, intrusive_stat_rate_of_change_t<TYPE> >(fns[i], 0, buf);
			if (pwatchers_->add_watcher(buf, *ptr, NULL))
			{
				geco_snprintf(buf, sizeof(buf), "stat watcher for %s avg val", entry.first.c_str());
				ptr->set_comment("%s avg val");
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
		// no wights, this is nit average stata but the stat itself that is accurate  
		// e.g. numBytesReceivedPerSecond_ with 0 weights is actually the exact bytes received per second
		initRateOfChange("averages/last", 0.f);
		initRateOfChange("averages/short", 60.f);	// 1 minute at 95%
		initRateOfChange("averages/medium", 600.0f);	// 10 minutes at 95%
		initRateOfChange("averages/long", 3600.0f);	// 60 minutes at 95%
	}
};

/**
 *	This class is used to collect statistics about all recv.
 */
struct network_recv_stats_t
{
	typedef intrusive_stat_rate_of_change_t< uint> stat;
	eastl::intrusive_list<stat>  pStats_;
	geco_watcher_director_t pwatchers_;

	stat numBytesReceivedPerSecond_;
	stat numPacketsReceivedPerSecond_;
	stat numDuplicatePacketsReceived_;
	stat numPacketsReceivedOffChannel_;
	stat numBundlesReceived_;
	stat numMessagesReceivedPerSecond_;
	stat numOverheadBytesReceived_;
	stat numCorruptedPacketsReceived_;
	stat numCorruptedBundlesReceived_;

	uint64	lastGatherTime_;
	int		lastTxQueueSize_;
	int		lastRxQueueSize_;
	int		maxTxQueueSize_;
	int		maxRxQueueSize_;

	//ProfileVal	mercuryTimer_;
	//ProfileVal	systemTimer_;

	network_recv_stats_t();

	/**
	 *	This method updates the statics associated with this connection
	 *	@TODO register a repeated timer with interval one sec for this function
	 *	instead of test time slap
	 */
	void update_stats(double elapsedTime);

	/**
	 *	This method updates the moving averages of the collected stats.
	 *	elapsedTime in seconds
	 */
	void updateStatAverages(double elapsedTime);

	/**
	 *	This method returns the current bits per second received.
	 */
	double BitsPerSecond() const
	{
		return numBytesReceivedPerSecond_.getRateOfChange(0) * 8;
	}
	/**
	*	This method returns the current bits per second received.
	*/
	double BytesPerSecond() const
	{
		return numBytesReceivedPerSecond_.getRateOfChange(0);
	}
	/**
	 *	This method returns the current packets per second received.
	 */
	double PacketsPerSecond() const
	{
		return numPacketsReceivedPerSecond_.getRateOfChange(0);
	}
	/**
	 *	This method returns the current messages per second received.
	 */
	double MessagesPerSecond() const
	{
		return numMessagesReceivedPerSecond_.getRateOfChange(0);
	}
};

/**
*	This class is used to collect statistics about all send.
*/
struct network_send_stats_t
{
	static float SEND_STATS_UPDATE_RATE;
	typedef intrusive_stat_rate_of_change_t< uint> stat;
	eastl::intrusive_list<stat>  pStats_;
	geco_watcher_director_t pwatchers_;

	stat numBytesSent_;
	stat numBytesResent_;
	stat numPacketsSent_;
	stat numPacketsResent_;
	stat numPiggybacks_;
	stat numPacketsSentOffChannel_;
	stat numBundlesSent_;
	stat numMessagesSent_;
	stat numReliableMessagesSent_;
	stat numFailedPacketSend_;
	stat numFailedBundleSend_;

	//ProfileVal	mercuryTimer_;
	//ProfileVal	systemTimer_;

	network_send_stats_t();

	/**
	*	This method returns the number of bits per second sent.
	*/
	double bitsPerSecond() const
	{
		return numBytesSent_.getRateOfChange() * 8;
	}


	/**
	*	This method returns the number of packets per second sent.
	*/
	double packetsPerSecond() const
	{
		return numPacketsSent_.getRateOfChange();
	}

	/**
	*	This method returns the number of messages per second sent.
	*/
	double messagesPerSecond() const
	{
		return numMessagesSent_.getRateOfChange();
	}

	/**
	*	This method updates the statics associated with this connection
	*	@TODO register a repeated timer with interval one sec for this function
	*	instead of test time slap
	*/
	void update_stats(double elapsedTime);

	/**
	*	This method updates the moving averages of the collected stats.
	*	elapsedTime in seconds
	*/
	void updateStatAverages(double elapsedTime);
};

#endif /* SRC_NETWORK_NETWORKSTATS_H_ */
