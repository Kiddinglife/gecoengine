//{future header message}
#ifndef __FvSMA_H__
#define __FvSMA_H__

#include <FvPowerDefines.h>

template<class T>
class FvSMA
{
public:
	FvSMA(int iPeriod);
	FvSMA(const FvSMA<T> &);
	~FvSMA();
	FvSMA&	operator=(const FvSMA<T> &);

	void Append(T value);
	T Average() const;
	T Min() const;
	T Max() const;
	int Period() const {return m_iPeriod;}
	int Count() const {return m_iCount;}
	void Clear();

private:
	int m_iPeriod;
	T* m_ptSamples;
	T m_tTotal;
	int m_iCount;
	int m_iPos;
};
//----------------------------------------------------------------------------
template<class T>
FvSMA<T>::FvSMA(int iPeriod) : m_iPeriod(iPeriod),
	m_tTotal(0), m_iCount(0), m_iPos(0)
{
	FV_ASSERT(iPeriod > 2 && iPeriod < 1000);

	m_ptSamples = new T[m_iPeriod];

	for (int i = 0; i < m_iPeriod; i++)
		m_ptSamples[i] = 0;
}
//----------------------------------------------------------------------------
template<class T>
FvSMA<T>::FvSMA(const FvSMA<T>& rOther) : m_iPeriod(rOther.period_),
	m_iCount(rOther.count_), m_tTotal(rOther.total_), m_iPos(rOther.pos_)
{
	m_ptSamples = new T[m_iPeriod];

	for(int i = 0; i < m_iPeriod; i++)
		m_ptSamples[i] = rOther.samples_[i];
}
//----------------------------------------------------------------------------
template<class T>
FvSMA<T>::~FvSMA()
{
	delete[] m_ptSamples;
	m_ptSamples = NULL;
}
//----------------------------------------------------------------------------
template<class T>
FvSMA<T>& FvSMA<T>::operator=(const FvSMA<T>& rOther)
{
	if (&rOther != this)
	{
		m_iPeriod = rOther.period_;
		m_iCount = rOther.count_;
		m_tTotal = rOther.total_;
		m_iPos = rOther.pos_;

		delete[] m_ptSamples;
		m_ptSamples = new T[m_iPeriod];
	}

	return *this;
}
//----------------------------------------------------------------------------
template<class T>
void FvSMA<T>::Append(T value)
{
	m_tTotal -= m_ptSamples[m_iPos];

	m_ptSamples[m_iPos] = value;
	m_tTotal += value;
	m_iPos = (m_iPos + 1) % m_iPeriod;

	if (m_iCount < m_iPeriod)
		m_iCount++;
}
//----------------------------------------------------------------------------
template<class T>
T FvSMA<T>::Average() const
{
	if (m_iCount)
		return m_tTotal / m_iCount;
	else return 0;
}
//----------------------------------------------------------------------------
template<class T>
T FvSMA<T>::Min() const
{
	T minVal = m_ptSamples[0];

	for (int i = 1; i < m_iCount; i++)
		if (m_ptSamples[i] < minVal)
			minVal = m_ptSamples[i];

	return minVal;
}
//----------------------------------------------------------------------------
template<class T>
T FvSMA<T>::Max() const
{
	T maxVal = m_ptSamples[0];

	for (int i = 1; i < m_iCount; i++)
		if (m_ptSamples[i] > maxVal)
			maxVal = m_ptSamples[i];

	return maxVal;
}
//----------------------------------------------------------------------------
template<class T>
void FvSMA<T>::Clear()
{
	m_iCount = 0;
	m_iPos = 0;
	m_tTotal = 0;

	for (int i = 0; i < m_iPeriod; i++)
		m_ptSamples[i] = 0;
}
//----------------------------------------------------------------------------

#endif /* __FvSMA_H__ */
