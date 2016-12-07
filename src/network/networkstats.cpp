/*
 * networkstats.cpp
 *
 *  Created on: 18Nov.,2016
 *      Author: jackiez
 */

#include "../common/geco-engine-feature.h"
#include "../common/ultils/ultils.h"
#include "networkstats.h"
#include <iosfwd>

DECLARE_DEBUG_COMPONENT2("networkstats", 0)

// ================= Profiler Definitions Starts ================
Profiler* Profiler::instance_ = NULL;
static const float DAMPING_RATIO = 0.97f;
static void DestroyProfiler()
{
	if (Profiler* profiler = &Profiler::instanceNoCreate()) delete profiler;
}
#ifdef _WIN32
static bool s_resetThreadTimes;
static float s_kernelTime;
static float s_processTime;
void Profiler::addThread(HANDLE thread, const char* name)
{
	ThreadInfo* info = new ThreadInfo;
	info->name_.append_sprintf("Profiler Threads/%s", name);
	info->base_ = 0;
	info->time_ = 0.0f;
	threads_[thread] = info;
	GECO_WATCH(info->name_.c_str(), info->time_, WT_READ_ONLY);
}
#endif
Profiler::Profiler() :
	threadId_(std::this_thread::get_id()),
	prevTime_(gettimestamp()),
	historyFile_(NULL),
	slotNamesWritten_(false),
	frameCount_(0),
	numSlots_(1),
	curSlot_(0),
	slotStackPos_(0)
{
	memset(slots_, 0, sizeof(slots_));
	slots_[0].name_ = "Unaccounted";
    //std::stringstream name;
    //name << threadId_;
	std::hash<std::thread::id> hasher;
	hashedthreadId_ = hasher(threadId_);

#ifdef _WIN32
	GECO_WATCH("Profiler/Unaccounted", slots_[0].curTimeMs_, WT_READ_ONLY);
	GECO_WATCH("Profiler Threads/Reset", s_resetThreadTimes);
	GECO_WATCH("Profiler Threads/Time Kernel", s_kernelTime, WT_READ_ONLY);
	GECO_WATCH("Profiler Threads/Time Process", s_processTime, WT_READ_ONLY);
	addThread(GetCurrentThread(), "Main");
#endif
	instance_ = this;
	atexit(DestroyProfiler);
	DestroyProfiler();
	historyFile_ = fopen("profiler.csv", "w");
	this->begin(0);
}
Profiler::~Profiler()
{
	while (slotStackPos_) this->end();
	closeHistory();
	Profiler::instance_ = NULL;
}
void Profiler::tick()
{
	this->end();
	GECO_ASSERT(slotStackPos_ == 0);

#ifdef _WIN32
	// Thread profiling
	FILETIME dummy;
	FILETIME processKernel;
	FILETIME processUser;
	FILETIME threadKernel;
	FILETIME threadUser;

	static uint64 kernelBase;
	static uint64 processBase;

	// Compute process time
	GetProcessTimes(GetCurrentProcess(), &dummy, &dummy, &processUser, &processKernel);

	uint64 kernelTime = (uint64(processKernel.dwHighDateTime) << 32)
		+ uint64(processKernel.dwLowDateTime);

	uint64 processTime = (uint64(processKernel.dwHighDateTime) << 32)
		+ uint64(processKernel.dwLowDateTime)
		+ (uint64(processUser.dwHighDateTime) << 32)
		+ uint64(processUser.dwLowDateTime);

	s_kernelTime = (float)(double(kernelTime - kernelBase) / 10000000.0);
	s_processTime = (float)(double(processTime - processBase) / 10000000.0);

	if (s_resetThreadTimes)
	{
		kernelBase = kernelTime;
		processBase = processTime;
	}

	// Compute thread times
	for (ThreadInfoMap::iterator it = threads_.begin(); it != threads_.end(); ++it)
	{
		ThreadInfo* info = it->second;

		BOOL b = GetThreadTimes(it->first, &dummy, &dummy, &threadUser, &threadKernel);

		uint64 threadTime = (uint64(threadKernel.dwHighDateTime) << 32)
			+ uint64(threadKernel.dwLowDateTime)
			+ (uint64(threadUser.dwHighDateTime) << 32)
			+ uint64(threadUser.dwLowDateTime);

		info->time_ = (float)(double(threadTime - info->base_) / 10000000.0);

		if (s_resetThreadTimes)
		{
			info->base_ = threadTime;
		}
	}

	s_resetThreadTimes = false;
#endif

	// Update current times for watchers
	double stampsPerMs = stamps_per_sec_double() / 1000.0;

	for (int i = 0; i < numSlots_; i++)
	{
		uint64 slotTime = slots_[i].times_[frameCount_];
		float slotTimeF = (float)((double)slotTime / stampsPerMs);
		slots_[i].curTimeMs_ =
			(DAMPING_RATIO * slots_[i].curTimeMs_) + ((1.f - DAMPING_RATIO) * slotTimeF);

		slots_[i].curCount_ = slots_[i].counts_[frameCount_];
	}

	frameCount_++;

	// Flush
	if (frameCount_ == NUM_FRAMES)
	{
		this->flushHistory();
		frameCount_ = 0;
	}

	// Start next frame
	this->begin(0);
}
int Profiler::declareSlot(const char* name, uint32 flags)
{
	slotNamesWritten_ = false;
	slots_[numSlots_].name_ = name;
	if (flags & FLAG_WATCH)
	{
		char  watcherName[256];
		sprintf(watcherName, "Profiler/%s", name);
		GECO_WATCH(watcherName, slots_[numSlots_].curTimeMs_, WT_READ_ONLY);
		sprintf(watcherName, "Profiler Count/%s", name);
		GECO_WATCH(watcherName, slots_[numSlots_].curCount_, WT_READ_ONLY);
	}
	return numSlots_++;
}
void Profiler::setNewHistory(const char* historyFileName)
{
	closeHistory();
	historyFile_ = fopen(historyFileName, "w");
	slotNamesWritten_ = false;
	frameCount_ = 0;
#ifdef _WIN32
	s_resetThreadTimes = true;
#endif
}
void Profiler::closeHistory()
{
	flushHistory();

	if (historyFile_)
	{
		fclose(historyFile_);
		historyFile_ = NULL;
	}
}
void Profiler::flushHistory()
{
	// Save
	if (historyFile_)
	{
		// Save slot names (twice - for times and counts, skipping unaccounted for counts)
		if (!slotNamesWritten_)
		{
			fprintf(historyFile_, "Total,");

			for (int i = 0; i < numSlots_; i++)
				fprintf(historyFile_, "%s,", slots_[i].name_);

			fprintf(historyFile_, ",");

			for (int i = 1; i < numSlots_; i++)
				fprintf(historyFile_, "%s,", slots_[i].name_);

			fprintf(historyFile_, "\n");

			slotNamesWritten_ = true;
		}

		// Calculate total for each frame
		uint64 frameTotals[NUM_FRAMES];
		for (int i = 0; i < frameCount_; i++)
		{
			frameTotals[i] = 0;
			for (int j = 0; j < numSlots_; j++)
				frameTotals[i] += slots_[j].times_[i];
		}

		double stampsPerMs = stamps_per_sec_double() / 1000.0;

		// Save each frame
		for (int i = 0; i < frameCount_; i++)
		{
			float frameTotal = (float)((double)frameTotals[i] / stampsPerMs);
			fprintf(historyFile_, "%f,", frameTotal);

			for (int j = 0; j < numSlots_; j++)
			{
				uint64 slotTime = slots_[j].times_[i];
				float slotTimeF = (float)((double)slotTime / stampsPerMs);
				fprintf(historyFile_, "%f,", slotTimeF);
			}

			fprintf(historyFile_, ",");

			for (int j = 1; j < numSlots_; j++)
			{
				fprintf(historyFile_, "%d,", slots_[j].counts_[i]);
			}

			fprintf(historyFile_, "\n");
		}
	}

	// Clear
	for (int i = 0; i < frameCount_; i++)
	{
		for (int j = 0; j < numSlots_; j++)
		{
			slots_[j].counts_[i] = 0;
			slots_[j].times_[i] = 0;
		}
	}
}
// ================= Profiler Definitions Ends ================ 


#include <stdint.h>
// ================= ProfileVal Definitions Starts ================ 
time_stamp_t ProfileVal::s_warningPeriod_(UINT64_MAX);
void ProfileVal::setWarningPeriod(time_stamp_t warningPeriod)
{
	s_warningPeriod_ = warningPeriod;
}
ProfileVal::ProfileVal(const eastl::string& name, ProfileGroup * pGroup) :
	name_(name),
	pGroup_(pGroup)
{
	// Initialise all counters.
	this->reset();

	// At the moment we don't support have values that aren't in a group at
	// all.  Not hard to add support for this if we ever actually need it.
	if (pGroup_ == NULL)
	{
		pGroup_ = &ProfileGroup::defaultGroup();
	}

	if (!name_.empty())
	{
		pGroup_->add(*this);
	}
}
ProfileVal::~ProfileVal()
{
	if (pGroup_ != NULL)
	{
		eastl::remove(pGroup_->begin(), pGroup_->end(), this);
	}
}
/**
*	This method returns the Watcher associated with this class.
*/
geco_watcher_base_t& ProfileVal::pSummaryWatcher(ProfileVal& profileVal)
{
	static geco_watcher_base_t* pWatcher = NULL;
	if (pWatcher == NULL)
	{
		pWatcher = new value_watcher_t< ProfileVal >(profileVal, WT_READ_WRITE);
	}
	return *pWatcher;
}
geco_watcher_base_t& ProfileVal::pWatcherStamps(ProfileVal& profileVal)
{
	static geco_watcher_base_t* pWatcher = NULL;
	if (pWatcher == NULL)
	{
		pWatcher = new geco_watcher_director_t();
		GECO_WATCH("count", profileVal.count_, WT_READ_ONLY, NULL, pWatcher);
		GECO_WATCH("inProgress", profileVal.inProgress_, WT_READ_ONLY, NULL, pWatcher);
		GECO_WATCH("sumQuantity", profileVal.sumQuantity_, WT_READ_ONLY, NULL, pWatcher);
		GECO_WATCH("lastQuantity", profileVal.lastQuantity_, WT_READ_ONLY, NULL, pWatcher);
		GECO_WATCH("sumTime", profileVal.sumTime_.stamp_, WT_READ_ONLY, NULL, pWatcher);
		GECO_WATCH("lastTime", profileVal.lastTime_.stamp_, WT_READ_ONLY, NULL, pWatcher);
		GECO_WATCH("sumIntTime", profileVal.sumIntTime_.stamp_, WT_READ_ONLY, NULL, pWatcher);
		GECO_WATCH("lastIntTime", profileVal.lastIntTime_.stamp_, WT_READ_ONLY, NULL, pWatcher);
	}
	return *pWatcher;
}

std::ostream& operator<<(std::ostream &o, const NiceTime &nt)
{
	// Overflows after 12 hours if we don't do this with doubles.
	static double microsecondsPerStamp = 1000000.0 / stamps_per_sec_double();

	double microDouble = ((double)(int64)nt.t_) * microsecondsPerStamp;
	time_stamp_t micros = (uint64)microDouble;

	double truncatedStampDouble = (double)(int64)(nt.t_ - ((uint64)(((double)(int64)micros) / microsecondsPerStamp)));
	uint32 picos = (uint32)(truncatedStampDouble * 1000000.0 *microsecondsPerStamp);

	// phew! Logic of lines above is: turn micros value we have back into
	// stamps, subtract it from the total number of stamps, and turn the
	// result into seconds just like before (except in picoseconds this time).
	// Simpler schemes seem to suffer from floating-point underflow.

	// Now see if micros was rounded and if so square it (hehe :)
	// It bothers me that I have to check for this...
	//	uint32 origPicos = picos;
	if (picos > 1000000)
	{
		uint32 negPicos = 1 + ~picos;
		if (negPicos > 1000000)
		{
			picos = 0;	// sanity check
		}
		else
		{
			picos = negPicos;
			micros--;
		}
	}

	// Well, it all doesn't work anyway. At high values (over 10s say) we
	// still get wild imprecision in the calculation of picos. We shouldn't
	// I'll just leave it here for the moment 'tho. TODO: Fix this.

	if (micros == 0 && picos == 0)
	{
		o << "0us";
		return o;
	}
	if (micros >= 1000000)
	{
		uint32 s = uint32(micros / 1000000);
		if (s >= 60)
		{
			o << (s / 60) << "min ";
		}
		o << (s % 60) << "sec ";
	}

	//o.width( 3 );		o.fill( '0' );		o << std::ios::right;
	//o.form( "%03d", (micros/1000)%1000 ) << " ";
	//o.form( "%03d", micros%1000 ) << ".";
	//o.form( "%03d", (picos/1000)%1000 ) << "us";
	// OKok, I give up on streams!

	char	lastBit[128];
	geco_snprintf(lastBit, sizeof(lastBit), "%03d %03d", (int)(uint32)((micros / 1000) % 1000), (int)(uint32)(micros % 1000));
	o << lastBit;

	//if (micros < 60000000)
	{		// Only display this precision if <1min. Just silly otherwise.
		geco_snprintf(lastBit, sizeof(lastBit), ".%03d", (int)((picos / 1000) % 1000));
		o << lastBit;
	}

	o << "us";
	return o;
}
std::ostream & operator<<(std::ostream  & s, const ProfileVal & val)
{
	time_stamp_t lastTime = val.inProgress_ ? time_stamp_t(gettimestamp() - val.lastTime_) : val.lastTime_;

	s << "Count: " << val.count_;
	if (val.inProgress_)
	{
		s << " inProgress: " << NiceTime(lastTime);
	}
	s << std::endl;

	if (val.count_ == 0)
	{
		return s;
	}

	if (val.sumQuantity_ != 0)
	{
		s << "Quantity: " << val.sumQuantity_ <<
			" (last " << val.lastQuantity_ <<
			", avg " << ((double)val.sumQuantity_) / val.count_ << ')' << std::endl;
	}

	s << "Time: " << NiceTime(val.sumTime_) <<
		" (last " << NiceTime(lastTime) << ')' << std::endl;

	if (val.sumTime_ != 0)
	{
		s << "Time/Count: " << NiceTime(val.sumTime_ / val.count_) << std::endl;

		if (val.sumQuantity_)
		{
			s << "Time/Qty: " <<
				NiceTime(val.sumTime_ / val.sumQuantity_) << std::endl;
		}
	}

	return s;
}
std::istream& operator >> (std::istream  & s, ProfileVal & v)
{
	v.reset();
	ProfileGroupResetter::instance().resetIfDesired(v);
	return s;
}
void read_watcher_value_from_stream(geco_bit_stream_t & result, ProfileVal& value, ushort& valtype, ushort& mode)
{
	//result.Read(valtype);
	//result.Read(mode);
	//static std::string buf;
	//result.Read(buf);
	//std::istringstream iss(buf);
	//iss >> value;
	//buf = "";
}
void write_watcher_value_to_stream(geco_bit_stream_t & result, const ProfileVal& value, const WatcherMode mode)
{
	//result.Write((ushort)WVT_STRING);
	//result.Write((ushort)mode);
	//static std::ostringstream oss;
	//oss << value;
	//result.Write(oss.str());
	//oss.clear();
}
// ================= ProfileVal Definitions Ends ================ 



// ================= ProfileGroup Definitions Ends ================ 
ProfileGroup* ProfileGroup::s_pDefaultGroup_ = NULL;
ProfileGroup::ProfileGroup(const char * watcherPath) :
	pSummaries_(new geco_watcher_director_t()),
	pDetails_(new geco_watcher_director_t())
{
	// Make sure timers are set up.
	stamps_per_sec();

	ProfileVal * pRunningTime = new ProfileVal("RunningTime", this);
	pRunningTime->start();

	this->add_watcher("summaries", *pSummaries_);
	this->add_watcher("details", *pDetails_);

	// If we are the default group, then add ourselves to the root of the watcher tree.
	if (watcherPath != NULL)
	{
		geco_watcher_base_t::get_root_watcher().add_watcher(watcherPath, *this);
	}
}
ProfileGroup::~ProfileGroup()
{
	// Clean up the running time profile
	delete this->pRunningTime();
}
/// adds a new profile to this group.
void ProfileGroup::add(ProfileVal& pVal)
{
	profiles_.push_back(&pVal);
	const char * name = pVal.c_str();
	pSummaries_->add_watcher(name, ProfileVal::pSummaryWatcher(pVal));
	pDetails_->add_watcher(name, ProfileVal::pWatcherStamps(pVal));
}
time_stamp_t ProfileGroup::runningTime() const
{
	return gettimestamp() - this->pRunningTime()->lastTime_;
}
void ProfileGroup::reset()
{
	iterator iter = this->begin();
	iterator endd = this->end();
	while (iter != this->end()) { (*iter)->reset();++iter; }
	this->pRunningTime()->start();
}
ProfileGroup & ProfileGroup::defaultGroup()
{
	if (s_pDefaultGroup_ == NULL)
	{
		s_pDefaultGroup_ = new ProfileGroup("profiles");
	}
	return *s_pDefaultGroup_;
}
// ================= ProfileVal Definitions Ends ================ 



// ================= ProfileGroupResetter Definitions Starts ================ 
ProfileGroupResetter::ProfileGroupResetter() :
	nominee_(0),
	groups_(0),
	doingReset_(false)
{
}
ProfileGroupResetter::~ProfileGroupResetter()
{
}
void ProfileGroupResetter::nominateProfileVal(ProfileVal * pVal)
{
	nominee_ = pVal;
}
void ProfileGroupResetter::addProfileGroup(ProfileGroup * pGroup)
{
	if (doingReset_) return;
	groups_.push_back(pGroup);
}
ProfileGroupResetter & ProfileGroupResetter::instance()
{
	static ProfileGroupResetter staticInstance;
	return staticInstance;
}
void ProfileGroupResetter::resetIfDesired(ProfileVal & val)
{
	if (doingReset_) return;
	if (nominee_ != &val) return;
	doingReset_ = true;
	eastl::vector<ProfileGroup*>::iterator iter;
	for (iter = groups_.begin(); iter != groups_.end(); iter++)
	{
		(*iter)->reset();
	}
	doingReset_ = false;
}
// ================= ProfileGroupResetter Definitions Ends ================ 


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
	maxRxQueueSize_(0),
	mercuryTimer_("network_recv_stats_t mercury timer profile"),
	systemTimer_("network_recv_stats_t system timer profile")
{
	uint_stat_watcher_factory_.initRatesOfChangeForStats(pStats_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "bytesReceived", this->numBytesReceivedPerSecond_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "bytesReceived", this->numBytesReceivedPerSecond_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "packetsReceived", this->numPacketsReceivedPerSecond_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "duplicatePacketsReceived", this->numDuplicatePacketsReceived_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "packetsReceivedOffChannel", this->numPacketsReceivedOffChannel_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "bundlesReceived", this->numBundlesReceived_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "messagesReceived", this->numMessagesReceivedPerSecond_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "overheadBytesReceived", this->numOverheadBytesReceived_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "corruptedPacketsReceived", this->numCorruptedPacketsReceived_);
	uint_stat_watcher_factory_.AddStatWatchers(pwatchers_, "corruptedBundlesReceived", this->numCorruptedBundlesReceived_);

	GECO_WATCH("socket/transmitQueue", lastTxQueueSize_, WT_READ_ONLY, "sock stat", &pwatchers_);
	GECO_WATCH("socket/receiveQueue", lastRxQueueSize_, WT_READ_ONLY, "sock stat", &pwatchers_);
	GECO_WATCH("socket/maxTransmitQueue", maxTxQueueSize_, WT_READ_ONLY, "sock stat", &pwatchers_);
	GECO_WATCH("socket/maxReceiveQueue", maxRxQueueSize_, WT_READ_ONLY, "sock stat", &pwatchers_);

	pwatchers_.add_watcher("timing/mercury", ProfileVal::pWatcherStamps(mercuryTimer_));
	pwatchers_.add_watcher("timing/system", ProfileVal::pWatcherStamps(systemTimer_));
	pwatchers_.add_watcher("timingSummary/mercury", ProfileVal::pSummaryWatcher(mercuryTimer_));
	pwatchers_.add_watcher("timingSummary/mercury", ProfileVal::pSummaryWatcher(systemTimer_));
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

void network_recv_stats_t::update_stats() //seconds
{
	uint64 currTime = gettimestamp();
	uint64 elapsedStamp = currTime - lastGatherTime_;

	// Wait at least a second. rely on timer cb 
	if (elapsedStamp >= stamps_per_sec())
	{
		// Update stat averages
		double elapsedTime = elapsedStamp/stamps_per_sec_double();
		this->updateStatAverages(elapsedTime);
		lastGatherTime_ = currTime;
	}

	// @TODO ask channel for lastTxQueueSize_ and lastRxQueueSize_
	//mtra_get_queue_sizes(this->connection_id_,&lastTxQueueSize_,&lastRxQueueSize_ );
	// Warn if the buffers are getting fuller
	//if ((lastTxQueueSize_ > maxTxQueueSize_) && (lastTxQueueSize_ > 128 * 1024))
	//{
	//	WARNING_MSG("send queue peaked at new max (%d bytes)\n", lastTxQueueSize_);
	//}
	//if ((lastRxQueueSize_ > maxRxQueueSize_) && (lastRxQueueSize_ > 1024 * 1024))
	//{
	//	WARNING_MSG("receive queue peaked at new max (%d bytes)\n", lastRxQueueSize_);
	//}
	// update maxs
	//maxTxQueueSize_ = std::max(lastTxQueueSize_, maxTxQueueSize_);
	//maxRxQueueSize_ = std::max(lastRxQueueSize_, maxRxQueueSize_);
}

float network_send_stats_t::SEND_STATS_UPDATE_RATE = 1.0f;
network_send_stats_t::network_send_stats_t() :
	mercuryTimer_("network_recv_stats_t mercury timer profile"),
	systemTimer_("network_recv_stats_t system timer profile")
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

	pwatchers_.add_watcher("timing/mercury", ProfileVal::pWatcherStamps(mercuryTimer_));
	pwatchers_.add_watcher("timing/system", ProfileVal::pWatcherStamps(systemTimer_));
	pwatchers_.add_watcher("timingSummary/mercury", ProfileVal::pSummaryWatcher(mercuryTimer_));
	pwatchers_.add_watcher("timingSummary/mercury", ProfileVal::pSummaryWatcher(systemTimer_));
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

