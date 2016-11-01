//{future header message}
#ifndef __FvEMA_H__
#define __FvEMA_H__

//----------------------------------------------------------------------------
class FvEMA
{
public:
	FvEMA(float fBias, float fInitial = 0.0f) : m_fBias(fBias),
		m_fAverage(fInitial) {}

	void Sample(float fValue)
	{
		m_fAverage = (1.0f - m_fBias) * fValue + m_fBias * m_fAverage;
	}

	float Average() const {return m_fAverage;}

private:
	float m_fBias;
	float m_fAverage;

};
//----------------------------------------------------------------------------
template< typename TYPE >
class FvAccumulatingEMA
{
public:
	FvAccumulatingEMA(float fBias, float fInitialAverage=0.0f,
		TYPE tInitialValue = TYPE()) : m_kAverage(fBias, fInitialAverage),
		m_tAccum(tInitialValue) {}

	TYPE& Value() {return m_tAccum;}

	const TYPE& Value() const {return m_tAccum;}

	float Average()	const {return m_kAverage.Average();}

	void Sample(bool bShouldReset = true)
	{
		m_kAverage.Sample(float(m_tAccum));
		if (bShouldReset)
		{
			m_tAccum = TYPE();
		}
	}

private:
	FvEMA m_kAverage;
	TYPE m_tAccum;
};
//----------------------------------------------------------------------------

#endif /* __FvEMA_H__ */
