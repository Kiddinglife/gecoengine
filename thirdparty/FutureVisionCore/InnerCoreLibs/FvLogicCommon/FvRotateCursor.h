
#ifndef __FvRotateCursor_H__
#define __FvRotateCursor_H__

#include "FvLogicCommonDefine.h"

#include <FvKernel.h>
#include <FvProvider.h>
#include <FvDirection3.h>


class FV_LOGIC_COMMON_API FvRotateCursor
{
public:
	FvRotateCursor(void);
	~FvRotateCursor(void);

	void SetAllowAngle(const float fAngle);
	void SetAccordSpd(const float fAngleSpd);
	bool Tick(const float fDeltaTime);
	void FastAccord();
	void SetControlDirecion(const FvSmartPointer<FvProvider1<FvDirection3>>& spProvider);
	const FvSmartPointer<FvProvider1<float>>& FeetDirectionProvider()const;
	const FvSmartPointer<FvProvider1<float>>& BodyDirectionProvider()const;
	float FeetDirection()const;
	float BodyDirection()const;
	float GetCursorYaw()const;

	bool IsActive()const;
	void SetActive(const bool bActive);

	void ForceAccord();

private:

	virtual void OnRoteStart(const FvInt32 iSign){}
	virtual void OnRoteUpdated(){}
	virtual void OnRoteEnd(const FvInt32 iSign){}

	FvInt32 _FomatByMaxAngle(float& fFeetDirection, const float fBodyYaw);

	float m_bAccordSpd;
	FvInt32 m_iAccordSign;
	float m_fAllowAngle;	
	float m_fCursorYaw;	
	bool m_bActive;

	FvSmartPointer<FvProvider1<float>> m_spBodyDirection;
	FvSmartPointer<FvProvider1<float>> m_spFeetDirection;
	FvSmartPointer<FvProvider1<FvDirection3>> m_spControlDirection;
};


template<class T, void (T::*pFuncStart)(const FvInt32), void (T::*pFuncEnd)(const FvInt32), void (T::*pFuncUpdated)()>
class FvTRotateCursor: public FvRotateCursor
{
public:
	FvTRotateCursor(T& kT):m_kT(kT){}
private:
	virtual void OnRoteStart(const FvInt32 iSign)
	{
		(m_kT.*pFuncStart)(iSign);
	}
	virtual void OnRoteEnd(const FvInt32 iSign)
	{
		(m_kT.*pFuncEnd)(iSign);
	}
	virtual void OnRoteUpdated()
	{
		(m_kT.*pFuncUpdated)();
	}

	T& m_kT;
};

#endif //__FvRotateCursor_H__