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

#include <thread>
#include "../common/ds/eastl/EASTL/utility.h"
#include "../common/ds/eastl/EASTL/vector.h"
#include "../common/ds/eastl/EASTL/hash_map.h"
#include "../common/ds/eastl/EASTL/string.h"

#include "../common/debugging/gecowatchert.h"
#include "../math/stat_rate_of_change.h"
#include "../common/debugging/timestamp.h"

#define PROFILER_DECLARE( id, name )	int g_profilerSlot_##id = Profiler::instance().declareSlot( name )
#define PROFILER_DECLARE2( id, name, flags )	int g_profilerSlot_##id = Profiler::instance().declareSlot( name, flags )
#define PROFILER_SCOPED( id )	extern int g_profilerSlot_##id; ScopedProfiler scopedProfiler_##id( g_profilerSlot_##id )

#define AUTO_SCOPED_PROFILE( NAME )	static ProfileVal _localProfile( NAME );	ScopedProfile _autoScopedProfile( _localProfile, __FILE__, __LINE__ );
#define SCOPED_PROFILE( PROFILE ) ScopedProfile _scopedProfile( PROFILE, __FILE__, __LINE__ );
#define START_PROFILE( PROFILE ) PROFILE.start();
#define STOP_PROFILE( PROFILE )  PROFILE.stop( __FILE__, __LINE__ );
#define STOP_PROFILE_WITH_CHECK( PROFILE )           if (PROFILE.stop( __FILE__, __LINE__ ))
#define STOP_PROFILE_WITH_DATA( PROFILE, DATA )   PROFILE.stop( __FILE__, __LINE__ , DATA );

class ProfileVal;
class ProfileGroup;

/**
*  A class to wrap up a group of profiles. The grouping associates profiles
*  with each for internal time calculations, i.e. only one profile from a group
*  can be accumulating internal time at once.
*
*  Profiles in the same group must be started and stopped in a stack-like
*  fashion.  Attempting to bridge starts and stops will trigger an assertion.
*/
class GECOAPI ProfileGroup : public geco_watcher_director_t
{
public:
	explicit ProfileGroup(const char * watcherPath = NULL);
	~ProfileGroup();

	typedef eastl::vector< ProfileVal* > Profiles;
	typedef Profiles::iterator iterator;

	iterator begin() { return profiles_.begin(); }
	iterator end() { return profiles_.end(); }

	Profiles & stack() { return stack_; }
	/// adds a new profile to this group.
	void add(ProfileVal& pVal);
	/// This method resets all of the ProfileVal's in this ProfileGroup.
	void reset();

	ProfileVal * pRunningTime() { return profiles_[0]; }
	const ProfileVal * pRunningTime() const { return profiles_[0]; }
	/// sets the timer profile for this group, i.e. the one that is always running and exists to track how long the group has been running for.
	time_stamp_t runningTime() const; // 由此可得到系统profile时间
	/// returns a reference to the default group.
	static ProfileGroup & defaultGroup();

private:
	/// The profiles that are part of this group.
	Profiles profiles_;

	/// The stack of profiles currently executing in this group.
	Profiles stack_;

	/// The watcher subdirectories for this group
	geco_watcher_director_t* pSummaries_;
	geco_watcher_director_t* pDetails_;

	/// The default global group for profiles.
	static ProfileGroup* s_pDefaultGroup_;
};

/**
*	This class is used to profile the performance of parts of the code.
*  as there maybe some other pairs of starts/stops invovled by this start and stop
*/
#include <stdlib.h> 
class GECOAPI ProfileVal
{
private:
	static time_stamp_t s_warningPeriod_;

public:
	ProfileVal(const eastl::string & name = "", ProfileGroup * pGroup = NULL);
	~ProfileVal();

	/// String description of this profile.
	eastl::string	name_;

	/// The profile group this profile belongs to, if any.
	ProfileGroup * pGroup_;

	time_stamp_t		lastTime_;		///< The time the profile was started.
	time_stamp_t		sumTime_;		///< The total time between all start/stops. count_次的总时间
	time_stamp_t		lastIntTime_;	///< The last internal time for this profile.
	time_stamp_t		sumIntTime_;	///< The sum of internal time for this profile.  count_次内部总时间
	uint32		lastQuantity_;	///< The last value passed into stop.
	uint32		sumQuantity_;	///< The total of all values passed into stop.
	uint32		count_;			///< The number of times stop has been called.
	int			inProgress_;	///< Whether the profile is currently timing. 	// 记录第几次处理, 如递归等

	static geco_watcher_base_t& pSummaryWatcher(ProfileVal& profileVal);
	static geco_watcher_base_t& pWatcherStamps(ProfileVal& profileVal);

	static void setWarningPeriod(time_stamp_t warningPeriod);

	/**
	*	This method starts this profile.
	*/
	void start()
	{
		time_stamp_t now = gettimestamp();
		// 记录第几次处理
		if (inProgress_++ == 0) lastTime_ = now;
		// 如果栈中有对象则自己是从上一个ProfileVal函数进入调用的
		// 我们可以在此得到上一个函数进入到本函数之前的一段时间片
		// 然后将其加入到sumIntTime_
		ProfileGroup::Profiles & stack = pGroup_->stack();
		if (!stack.empty())
		{
			ProfileVal & profile = *stack.back();
			profile.lastIntTime_ = now - profile.lastIntTime_;
			profile.sumIntTime_ += profile.lastIntTime_;
		}
		// push self to stack
		stack.push_back(this);
		//	reset start time
		lastIntTime_ = now;
	}

	/**
	*  This method stops this profile.
	*/
	void stop(uint32 qty = 0)
	{
		time_stamp_t now = gettimestamp();
		// 如果为0则表明自己是调用栈的产生着在此我们可以得到这个函数总共耗费的时间
		if (--inProgress_ == 0)
		{
			lastTime_ = now - lastTime_;
			sumTime_ += lastTime_;
		}

		lastQuantity_ = qty;
		sumQuantity_ += qty;
		++count_;

		ProfileGroup::Profiles & stack = pGroup_->stack();
		GECO_ASSERT(stack.back() == this);
		stack.pop_back();

		// Disable internal time counting for this profile
		// 得到本函数所耗费的时间
		lastIntTime_ = now - lastIntTime_;
		sumIntTime_ += lastIntTime_;

		// Re-enable the internal counter for the frame above this one.
		// 我们需要在此重设上一个函数中的profile对象的最后一次内部时间
		// 使其能够在start时正确得到自调用完本函数之后进入下一个profile函数中时所消耗的时间片段
		if (!stack.empty())
		{
			stack.back()->lastIntTime_ = now;
		}
	}

	/**
	*	This method stops the profile and warns if it took too long.
	*/
	bool stop(const char * filename, int lineNum, uint32 qty = 0)
	{
		this->stop(qty);

		const bool tooLong = this->isTooLong();

		if (tooLong)
		{
			WARNING_MSG("%s:%d: Profile %s took %.2f seconds\n", filename, lineNum, name_.c_str(), lastTime_ / stamps_per_sec_double());
		}
		return tooLong;
	}

	bool isTooLong() const
	{
		return !this->running() &&
			(lastTime_ > s_warningPeriod_);
	}

	/**
	*  This method resets this profile.
	*/
	void reset()
	{
		lastTime_ = 0;
		sumTime_ = 0;
		lastIntTime_ = 0;
		sumIntTime_ = 0;
		lastQuantity_ = 0;
		sumQuantity_ = 0;
		count_ = 0;
		inProgress_ = 0;
	}


	/**
	*	This method returns whether or not this profile is currently running.
	*	That is, start has been called more times than stop.
	*/
	bool running() const
	{
		return inProgress_ > 0;
	}


	time_stamp_t lastTime() const
	{
		return this->running() ? time_stamp_t(0) : lastTime_;
	}

	/**
	*  Returns the readable description of this profile.
	*/
	inline const char * c_str() const { return name_.c_str(); }
	inline double lastTimeInSeconds() const { return stamps2sec(lastTime_); }
	inline double sumTimeInSeconds() const { return stamps2sec(sumTime_); }
	inline double lastIntTimeInSeconds() const { return stamps2sec(lastIntTime_); }
	inline double sumIntTimeInSeconds() const { return stamps2sec(sumIntTime_); }
};

#include <iostream>
/// output stream operator for a ProfileVal.
std::ostream & operator<<(std::ostream  & s, const ProfileVal & val);
///  input stream operator for ProfileVal.
std::istream& operator >> (std::istream  & s, ProfileVal & v);
void read_watcher_value_from_stream(geco_bit_stream_t & result, ProfileVal& value, ushort& valtype, ushort& mode);
void write_watcher_value_to_stream(geco_bit_stream_t & result, const ProfileVal& value, const WatcherMode mode);

/**
*	This structure wraps up a TimeStamp timestamp delta and has an operator defined on it to print it out nicely.
*/
struct GECOAPI NiceTime
{
	/// Constructor
	explicit NiceTime(time_stamp_t t) : t_(t) {}
	time_stamp_t t_;	///< Associated timestamp.
};
/// output stream operator for a NiceTime.
std::stringstream& operator<<(std::stringstream &o, const NiceTime &nt);

/**
*	This singleton class resets all registered ProfileGroups when a nominated ProfileVal is reset.
*/
struct GECOAPI ProfileGroupResetter
{
	ProfileGroupResetter();
	~ProfileGroupResetter();
	/**
	* Records the ProfileVal whose reset should trigger a global reset.
	* Note: If this is in a ProfileGroup, you shouldn't add to the
	* group after you call this function as it could move the memory
	* containing the vector elements around and stuff everything up.*/
	void nominateProfileVal(ProfileVal * pVal = NULL);
	/**
	* Records a ProfileGroup which should be reset when the nominated
	* ProfileVal is. Don't call this when profiles are being reset.*/
	void addProfileGroup(ProfileGroup * pGroup);
	/// Static function to get the singleton instance.
	static ProfileGroupResetter & instance();
	ProfileVal * nominee_;
	eastl::vector< ProfileGroup*>	groups_;
	bool doingReset_;
	/// called by every ProfileVal when it is reset
	void resetIfDesired(ProfileVal & val);
	friend std::istringstream& operator >> (std::istringstream &s, ProfileVal &v);
};
class ScopedProfile
{
public:
	ScopedProfile(ProfileVal & profile, const char * filename, int lineNum) :
		profile_(profile),
		filename_(filename),
		lineNum_(lineNum)
	{
		profile_.start();
	}

	~ScopedProfile()
	{
		profile_.stop(filename_, lineNum_);
	}

private:
	ProfileVal & profile_;
	const char * filename_;
	int lineNum_;
};


class GECOAPI Profiler
{
private:
	enum
	{
		NUM_FRAMES = 64,
		MAX_SLOTS = 256,
		SLOT_STACK_DEPTH = 64,
	};

public:
	enum
	{
		FLAG_WATCH = (1 << 0),
	};

private:
	struct Slot
	{
		const char*			name_;
		float				curTimeMs_;
		int					curCount_;
		uint64				times_[NUM_FRAMES];
		int					counts_[NUM_FRAMES];
	};

#ifdef _WIN32
	struct ThreadInfo
	{
		eastl::string name_;
		uint64 base_;
		float time_;
	};
#endif

public:
	Profiler();
	~Profiler();

	static Profiler&	instance()
	{
		if (!Profiler::instance_) new Profiler;
		return *Profiler::instance_;
	}
	static Profiler&	instanceNoCreate()
	{
		return *Profiler::instance_;
	}
	static Profiler*	instanceNoCreateP() { return instance_; }
	void				tick();

	int					declareSlot(const char* name, uint32 flags = 0);
	///Add to current and push slotId
	void				begin(int slotId)
	{
		if (std::this_thread::get_id() != Profiler::instanceNoCreate().threadId_)
			return;
		this->addTimeToCurrentSlot();
		slotStack_[slotStackPos_++] = curSlot_;
		curSlot_ = slotId;
		slots_[slotId].counts_[frameCount_]++;
	}
	///Add to current and pop
	void				end()
	{
		if (std::this_thread::get_id() != Profiler::instanceNoCreate().threadId_)
			return;
		this->addTimeToCurrentSlot();
		curSlot_ = slotStack_[--slotStackPos_];
	}

#ifdef _WIN32
	void				addThread(HANDLE thread, const char* name);
	void				removeThread(HANDLE thread);
#endif

	void				setNewHistory(const char* historyFileName);
	void				closeHistory();
	void				flushHistory();

private:
	void				addTimeToCurrentSlot()
	{
		uint64 curTime = ::gettimestamp();
		uint64 delta = curTime - prevTime_;
		prevTime_ = curTime;
		slots_[curSlot_].times_[frameCount_] += delta;
	}

private:
	static Profiler* 	instance_;

	std::thread::id		threadId_;
	size_t 				hashedthreadId_;

	uint64				curTime_;
	uint64				prevTime_;

	FILE*				historyFile_;
	bool				slotNamesWritten_;

	int					frameCount_;

	int					numSlots_;
	int					curSlot_;

	int					slotStack_[SLOT_STACK_DEPTH];
	int					slotStackPos_;
	Slot				slots_[MAX_SLOTS];

#ifdef _WIN32
	typedef eastl::hash_map<HANDLE, ThreadInfo*> ThreadInfoMap;
	ThreadInfoMap		threads_;
#endif
};
class GECOAPI ScopedProfiler
{
public:
	ScopedProfiler(int id)
	{
		Profiler::instanceNoCreate().begin(id);
	}

	~ScopedProfiler()
	{
		Profiler::instanceNoCreate().end();
	}
};


template <class TYPE>
class stat_watcher_factory_t;
extern stat_watcher_factory_t<uint>& read_uint_stat_watcher_factory();

template <class TYPE>
class GECOAPI stat_watcher_factory_t
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
		for (auto& stat : stats)
		{
			initRatesOfChangeForStat(stat);
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
		for (stat_entries_t::size_type i = 0; i < count; i++)
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
struct GECOAPI network_recv_stats_t
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
	stat numFilteredBundlesReceived_;

	uint64	lastGatherTime_;
	int		lastTxQueueSize_;
	int		lastRxQueueSize_;
	int		maxTxQueueSize_;
	int		maxRxQueueSize_;

	ProfileVal	mercuryTimer_; // time spent on completed network event cycle llp + ulp
	ProfileVal	systemTimer_; // how much time we spend on system calls eg. read_msg() write_msg()

	network_recv_stats_t();

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
struct GECOAPI network_send_stats_t
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

	ProfileVal	mercuryTimer_;
	ProfileVal	systemTimer_;

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
