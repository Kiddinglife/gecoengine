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

/////////////////////////////////////// stats module  ////////////////////////////////////
#include "../common/ds/eastl/EASTL/utility.h"
#include "../math/stat_rate_of_change.h"
#include "../common/debugging/gecowatchert.h"
#include "../common/ds/eastl/EASTL/vector.h"
/**
 *	This class is used to collect statistics about the connection.
 */
struct network_stats_t
{
	typedef intrusive_stat_rate_of_change_t< uint> stat;
	eastl::intrusive_list<stat>  pStats_;
	geco_watcher_director_t pwatchers_;

	stat numBytesReceived_;
	stat numPacketsReceived_;
	stat numDuplicatePacketsReceived_;
	stat numPacketsReceivedOffChannel_;
	stat numBundlesReceived_;
	stat numMessagesReceived_;
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

	network_stats_t();

	/**
	 *	This method updates the statics associated with this connection
	 *	@TODO register a repeated timer with interval one sec for this function
	 *	instead of test time slap
	 */
	void update_stats();

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
		return numBytesReceived_.getRateOfChange(0) * 8;
	}
	/**
	 *	This method returns the current packets per second received.
	 */
	double PacketsPerSecond() const
	{
		return numPacketsReceived_.getRateOfChange(0);
	}
	/**
	 *	This method returns the current messages per second received.
	 */
	double MessagesPerSecond() const
	{
		return numMessagesReceived_.getRateOfChange();
	}
};
/////////////////////////////////////// stats module ////////////////////////////////////

#endif /* SRC_NETWORK_NETWORKSTATS_H_ */
