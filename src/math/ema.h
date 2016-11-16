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

#ifndef __MATH_EMA_HPP
#define __MATH_EMA_HPP

#include <math.h>

/**
*	This class represents an exponentially weighted moving average.
*  @details see http://www.forexabode.com/forex-school/technical-indicators/moving-averages/exponential-moving-average/
*  Current periods’ exponential moving average =  (Previous period's EMA * (1 - Exponent))+(Previous period’s closing price * Exponent)
*  Let’s assume the following to make an example:
*  You wish to calculate daily current EMA while using EMA of 22 periods
*  Let’s say today is 20th April
*  The closing price of April 19th was 135.80
*  EMA of April 19th was 132.60
*  So the exponent = 2 / (22+1) = = 0.08695
*  Hence using the above formula, the EMA of April 20th = (135.80 * 0.08695) + (132.80) * (1 - 0.08695) = 135.41
*
*	The nth sample (given via the sample() method) updates the Current periods’ exponential moving average ()
*	according to: avg(n) = (1 - bias) * sample( n-1 ) + bias * avg( n - 1 )
*
*	This class is entirely inlined.
*/
struct ema_t
{
	/// bias / Exponent
	float bias_;

	/// Current periods’ exponential moving average()
	float average_;

	/**
	*	Constructor.
	*
	*	@param bias 	Determines the exponential bias.
	*	@param initial 	The initial value of the average.
	*/
	explicit ema_t(float bias, float initial = 0.f) :
		bias_(bias),
		average_(initial)
	{}

	/**
	*	Sample a previous period's EMA into the average.
	*
	*	@param value 	Previous period's EMA
	*/
	void sample(float value)
	{
		average_ = (1.f - bias_) * value + bias_ * average_;
	}

	/**
	*	Return the current value of the average.
	*/
	float average() const { return average_; }

	/**
	*	This method calculates a bias required to give the most recent samples the
	*	desired weighting.
	*
	*	For example, if numSamples is 60 and weighting is 0.95, using the resulting
	*	bias in an EMA means that the latest 60 samples account for 95% of the
	*	average and all other samples account for 5%.
	*
	*	@param numSamples The number of samples having the desired weighting.
	*	@weighting The proportion of the average that these samples contribute.
	*/
	static float calculateBiasFromNumSamples(float numSamples,
		float weighting = 0.95f)
	{
		return (numSamples != 0.f) ?
			exp(log(1.f - weighting) / numSamples) :
			0.f;
	}
};

/**
*	This templated class is a helper class for accumulating values, and
*	periodically sampling that value into an ema_t.
*/
template< typename TYPE >
class Accumulatingema_t
{
public:
	/**
	*	Constructor.
	*
	*	@param bias 			Determines the exponential bias.
	*	@param initialAverage	The initial value of the average.
	*	@param initialValue 	The initial value of the accumulated value.
	*/
	Accumulatingema_t(float bias, float initialAverage = 0.f,
		TYPE initialValue = TYPE()) :
		average_(bias, initialAverage),
		accum_(initialValue)
	{
	}


	/**
	*	Accumulated value accessor.
	*/
	TYPE & value() { return accum_; }


	/**
	*	Accumulated value const accessor.
	*/
	const TYPE & value() const { return accum_; }


	/**
	*	Average accessor.
	*/
	float average()	const { return average_.average(); }


	/**
	*	Sample method. This should be called periodically to sample the
	*	accumulated value into the weighted average. The accumulated value
	*	is converted to a float before sampling.
	*
	*	@param shouldReset 		If true, the accumulated value is reset to the
	*							TYPE default (e.g. 0 for numeric TYPEs) after
	*							sampling. If false, it is unchanged.
	*/
	void sample(bool shouldReset = true)
	{
		average_.sample(float(accum_));
		if (shouldReset)
		{
			accum_ = TYPE();
		}
	}

private:
	/// The exponentially weighted average.
	ema_t 	average_;
	/// The accumulated value.
	TYPE 	accum_;
};
#endif // __ema_t_HPP__


