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

#ifndef __MATH_ema_t_UPDATE_RATE_STATE_HPP
#define __MATH_ema_t_UPDATE_RATE_STATE_HPP

#include "ema.h"
#include "../common/geco-engine-feature.h"
#include "../common/geco-plateform.h"
#include "../common/debugging/timestamp.h"
#include "../common/debugging/debug.h"
#include "../common/ds/eastl/EASTL/vector.h"
#include "../common/ds/eastl/EASTL/intrusive_list.h"

/**
 *	This class is used to keep a statistic. The rate of change of this statistic is monitored.
 */
template < class TYPE >
class stat_rate_of_change_t
{
public:
	stat_rate_of_change_t() :
		val_(0),
		prev_val_(0),
		averages_()
	{
	}

	/**
	*	This method is used to start the monitoring of the rate of change of this
	*	stat. It does this using an Exponential Moving Average. This is set up so
	*	that the most recent "numSamples" samples makes up "weighting" of the
	*	result.
	*/
	void monitorRateOfChange(float numSamples)
	{
		float bias = ema_t::calculateBiasFromNumSamples(numSamples);
		averages_.push_back(ema_t(bias));
	}

	/**
	*	This method updates the rate of change values using a sample over the
	*	input period.
	*/
	void tick(double deltaTime)
	{
		if (deltaTime <= 0.f)
		{
			return;
		}

		TYPE delta = val_ - prev_val_;
		double avg = delta / deltaTime;
		prev_val_ = val_;

		Averages::iterator iter = averages_.begin();
		static Averages::iterator end = averages_.end();
		while (iter != end)
		{
			iter->sample(float(avg));
			++iter;
		}
	}

	void operator++() { this->add(1); }
	void operator++(int) { this->add(1); }

	void operator--() { this->add(-1); }
	void operator--(int) { this->add(-1); }

	void operator+=(TYPE value) { this->add(value); }
	void operator-=(TYPE value) { this->subtract(value); }

	/**
	*	This method returns the value of this stat.
	*/
	TYPE& total()
	{
		return val_;
	}


	/**
	*	This method returns the value of this stat at the last tick call.
	*/
	void setTotal(TYPE& total)
	{
		val_ = total;
	}

	/**
	*	This method returns a rate of change of this statistic. The rates of change
	*	can vary based on their weightings.
	*/
	double getRateOfChange(int index=0) const
	{
		return averages_[index].average();
	}

	// Workaround for Watchers
	template <int INDEX>
	double getRateOfChangeT() const { return this->getRateOfChange(INDEX); }

	double& getRateOfChange0() { static double val; val = this->getRateOfChange(0); return val;}
	double& getRateOfChange1() { static double val; val = this->getRateOfChange(1); return val;}
	double& getRateOfChange2() { static double val; val = this->getRateOfChange(2); return val;}
	double& getRateOfChange3() { static double val; val = this->getRateOfChange(3); return val;}
	double& getRateOfChange4() { static double val; val = this->getRateOfChange(4); return val;}

	/**
	*	This method changes this stat by the specified amount.
	*/
	void add(TYPE value)
	{
		val_ += value;
	}

	/**
	*	This method reduces this stat by the specified amount.
	*/
	void subtract(TYPE value)
	{
		val_ -= value;
	}

	TYPE val_;
	TYPE prev_val_;

	typedef eastl::vector< ema_t > Averages;
	Averages averages_;
};


/**
 *	This class adds IntrusiveObject functionality to stat_rate_of_change_t.
 */
template < class TYPE >
struct intrusive_stat_rate_of_change_t :
		public eastl::intrusive_list_node,
		public stat_rate_of_change_t< TYPE >
{
	typedef typename eastl::intrusive_list<intrusive_stat_rate_of_change_t<TYPE>> container_type;
};

#endif // STAT_WITH_RATES_OF_CHANGE_HPP

