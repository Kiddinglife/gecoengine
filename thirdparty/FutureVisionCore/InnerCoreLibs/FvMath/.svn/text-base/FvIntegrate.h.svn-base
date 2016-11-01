//{future header message}
#ifndef __FvIntegrate_H__
#define __FvIntegrate_H__

//----------------------------------------------------------------------------
template <class Functor, class X_TYPE>
void FvIntegrate(const Functor& F, X_TYPE& x, float h)
{
	X_TYPE A1 = F(0, x);
	X_TYPE xTemp = x + (h/2.f) * A1;

	X_TYPE A2 =  F(h/2.f, xTemp);
	xTemp = x + (h/2.f) * A2;

	X_TYPE A3 = F(h/2.f, xTemp);
	xTemp = x + h * A3;

	X_TYPE A4 = F(h, xTemp);

	x += (h/6.f) * (A1 + A2 + A2 + A3 + A3 + A4);
}
//----------------------------------------------------------------------------
template <class TYPE>
class FvApproachFunctor
{
public:
	FvApproachFunctor(const TYPE & desiredValue,
		float approachRate = 1.f) :
	m_tDesiredValue(desiredValue),
		m_fApproachRate(approachRate){};

	TYPE operator()(float t, const TYPE & x) const
	{
		return m_fApproachRate * (m_tDesiredValue - x);
	}

private:
	const TYPE& m_tDesiredValue;
	float m_fApproachRate;
};
//----------------------------------------------------------------------------
template <class TYPE>
FvApproachFunctor<TYPE> FvMakeApproach(const TYPE& tDesiredValue,
	float fApproachRate = 1.f)
{
	return FvApproachFunctor<TYPE>(tDesiredValue,
		fApproachRate);
}
//----------------------------------------------------------------------------

#endif /* __FvIntegrate_H__ */
