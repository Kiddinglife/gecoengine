/*
* Copyright (c) 2016
* Geco Gaming Company
*
* Permission to use, copy, modify, distribute and sell this software
* and its documentation for GECO purpose is hereby granted without fee,
* provided that the above copyright notice appear in all copies and
* that both that copyright notice and this permission notice appear
* in supporting documentation. Geco Gaming makes no
* representations about the suitability of this software for GECO
* purpose.  It is provided "as is" without express or implied warranty.
*
*/

/**
* Created on 22 April 2016 by Jake Zhang
* Reviewed on 07 May 2016 by Jakze Zhang
*/

#ifndef __INCLUDE_PROFILE_H
#define __INCLUDE_PROFILE_H

#include <vector>
#include <string>

#include "../geco-engine-feature.h"
#include "../geco-plateform.h"
#include "../ultils/singleton_base.h"

#include "timestamp.h"
#include "debug.h"

// 由此可得到系统profile时间
extern uint64 runningTime();
struct Profiles;
struct Profile
{
	std::string            strName_;
	Profiles*              pProfiles_;
	int		   inProgress_;  	// 记录第几次处理, 如递归等
	bool                    initWatcher_;

	static time_stamp_t warningPeriod_;
	time_stamp_t		   sumTime_;                       	// count_次的总时间
	time_stamp_t          sumInternalTime_; 	 /// Total time counted
	time_stamp_t          lastTime_;                   /// time after start
	time_stamp_t          lastInternalTime_; 	/// the last internal time

	unsigned int		   lastVal2Stop_;	///< The last value passed into stop.
	unsigned int		   sumAllVals2Stop_;	///< The total of all values passed into stop.
	unsigned int		   stopCounts_;			///< The number of times stop has been called.

	Profile(std::string name = "", Profiles* profiles = NULL);
	virtual ~Profile();

	void start(void);
	void stop(unsigned int qty = 0);
	bool Profile::initializeWatcher(void)
	{
		VERBOSE_MSG("Profile::initializeWatcher()\n");
		return true;
	}
	void Profile::stop1(const char * filename, int lineNum, unsigned int qty = 0)
	{
		this->stop(qty);
		if (isTooLong())
		{
			WARNING_MSG("{%s}:{%d}: Profile {%s} took {:.2f} seconds\n", filename, lineNum, strName_.c_str(), stamps2sec(lastTime_));
		}
	}
	bool Profile::running() const
	{
		return inProgress_ > 0;
	}
	const char * Profile::c_str() const
	{
		return strName_.c_str();
	}
	bool Profile::isTooLong() const
	{
		return !this->running() && (lastTime_ > warningPeriod_);
	}
	const time_stamp_t& Profile::sumTime() const
	{
		return sumTime_;
	}
	const time_stamp_t& Profile::lastIntTime() const
	{
		return lastInternalTime_;
	}
	const time_stamp_t& Profile::sumIntTime() const
	{
		return sumInternalTime_;
	}
	const time_stamp_t Profile::lastTime() const
	{
		return this->running() ? time_stamp_t(0) : lastTime_;
	}
	double Profile::lastTimeInSeconds() const
	{
		return stamps2sec(lastTime_);
	}
	double Profile::sumTimeInSeconds() const
	{
		return stamps2sec(sumTime_);
	}
	double Profile::lastIntTimeInSeconds() const
	{
		return stamps2sec(lastInternalTime_);
	}
	double Profile::sumIntTimeInSeconds() const
	{
		return stamps2sec(sumInternalTime_);
	}
	const char* Profile::name() const
	{
		return this->strName_.c_str();
	}
	unsigned int Profile::count() const
	{
		return stopCounts_;
	}
};

struct Profiles : public base_singleton_t< Profiles >
{
	typedef std::vector<Profile*> ProfilesType;
	typedef ProfilesType::iterator iterator;
	ProfilesType profiles_;
	ProfilesType stack_;
	std::string name_;

	Profiles(std::string name = "");
	virtual ~Profiles();

	void add(Profile * pVal);
	iterator begin() { return profiles_.begin(); }
	iterator end() { return profiles_.end(); }
	Profile* pRunningTime() { return profiles_[0]; }
	const Profile * pRunningTime() const { return profiles_[0]; }
	time_stamp_t runningTime() const;
	inline const char* name()const;
	bool initializeWatcher();
	inline const ProfilesType& profiles()const;
};
struct AutoScopedProfile
{
	AutoScopedProfile(Profile& profile, const char * filename, int lineNum) :
		profile_(profile),
		filename_(filename),
		lineNum_(lineNum)
	{
		profile_.start();
	}

	~AutoScopedProfile()
	{
		profile_.stop1(filename_, lineNum_);
	}

	Profile& profile_;
	const char * filename_;
	int lineNum_;
};

#define START_PROFILE( PROFILE ) PROFILE.start();
#define STOP_PROFILE( PROFILE )	   PROFILE.stop( __FILE__, __LINE__ );

#define AUTO_SCOPED_PROFILE( NAME )	\
static Profile _localProfile( NAME );\
AutoScopedProfile _autoScopedProfile( _localProfile, __FILE__, __LINE__ );

#define SCOPED_PROFILE(PROFILE)\
AutoScopedProfile PROFILE##_AutoScopedProfile(PROFILE, __FILE__, __LINE__);

#define STOP_PROFILE_WITH_CHECK( PROFILE ) if (PROFILE.stop( __FILE__, __LINE__ ))
#define STOP_PROFILE_WITH_DATA( PROFILE, DATA ) PROFILE.stop( __FILE__, __LINE__ , DATA );

#endif