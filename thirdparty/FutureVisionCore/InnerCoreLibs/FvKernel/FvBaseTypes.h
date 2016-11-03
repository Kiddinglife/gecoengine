//{future header message}
#ifndef __FvBaseTypes_H__
#define __FvBaseTypes_H__

#include <FvMathPower.h>

#include <string>
typedef std::string FvString;
typedef std::wstring FvWString;

template <class MAP> 
struct FvMapTypes
{
#ifdef FV_WIN32
#	if _MSC_VER >= 1300 // VC7
	typedef typename MAP::mapped_type & _Tref;
#	else /* (_MSC_VER >= 1300) */
	typedef typename MAP::_Tref _Tref;
#	endif /* !(_MSC_VER >= 1300) */
#else /* FV_WIN32 */
	typedef typename MAP::data_type & _Tref;
#endif /* !FV_WIN32 */
};

#define FV_FLOAT_EQUAL(Value1, Value2) \
	(abs(Value1 - Value2) < std::numeric_limits<float>::epsilon())

inline bool FvAlmostEqual( const float fF1, const float fF2, const float fEpsilon = 0.0004f )
{
	return FvFabsf( fF1 - fF2 ) < fEpsilon;
}

inline bool FvAlmostEqual( const double fD1, const double fD2, const double fEpsilon = 0.0004 )
{
	return FvFabs( fD1 - fD2 ) < fEpsilon;
}

inline bool FvAlmostZero( const float fF, const float fEpsilon = 0.0004f )
{
	return fF < fEpsilon && fF > -fEpsilon;
}

inline bool FvAlmostZero( const double fD, const double fEpsilon = 0.0004 )
{
	return fD < fEpsilon && fD > -fEpsilon;
}

template<typename T>
inline bool FvAlmostEqual( const T& kC1, const T& kC2, const float fEpsilon = 0.0004f )
{
	if( kC1.size() != kC2.size() )
		return false;
	typename T::const_iterator iter1 = kC1.begin();
	typename T::const_iterator iter2 = kC2.begin();
	for( ; iter1 != kC1.end(); ++iter1, ++iter2 )
		if( !FvAlmostEqual( *iter1, *iter2, fEpsilon ) )
			return false;
	return true;
}

#define FV_FLOAT_POINT(p) ((float*)p)
#define FV_FLOAT_POINT_CONST(p) ((const float*)p)
#define FV_FLOAT_POINT_THIS ((float*)this)

#endif /* __FvBaseTypes_H__ */