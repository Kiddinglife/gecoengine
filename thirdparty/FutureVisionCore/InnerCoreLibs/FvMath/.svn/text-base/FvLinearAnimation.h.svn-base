#ifndef __FvLinearAnimation_H__
#define __FvLinearAnimation_H__

#include <iostream>

#include <map>

template< class _Ty >
class FvLinearAnimation : public std::map< float, _Ty >
{
public:
	FvLinearAnimation( bool bLoop = false, float fMaxTime = 0 )
	{
		m_bLoop = bLoop;
		m_fMaxTime = fMaxTime;
	}
	~FvLinearAnimation()
	{
	}

	_Ty Animate( float fTime ) const
	{
		if( !size() )
		{
			return _Ty();
		}

		_Ty res;

		if( m_bLoop )
		{
			if( fTime > m_fMaxTime || fTime < 0 )
			{
				fTime -= floorf( fTime / m_fMaxTime ) * m_fMaxTime;
			}

			const_iterator iter = upper_bound( fTime );
			const_iterator iter2 = iter;

			if( iter == begin() )
				iter = end();

			iter--;

			if( iter2 == end() )
			{
				iter2 = begin();
			}


			float firstTime = iter->first;
			_Ty   firstValue = iter->second;

			float lastTime = iter2->first;
			_Ty	  lastValue = iter2->second;

			if( firstTime == fTime )
				return firstValue;

			if( firstTime > lastTime )
				lastTime += m_fMaxTime;

			if( firstTime > fTime )
				fTime += m_fMaxTime;

			float fraction = ( fTime - firstTime ) / ( lastTime - firstTime );
			res = firstValue + ( ( lastValue - firstValue ) * fraction ) ;

		}
		else
		{
			const_iterator iter = this->upper_bound( fTime );

			if( iter == this->end() )
			{
				iter --;
				return iter->second;
			}

			if( iter == this->begin() )
				return iter->second;

			const_iterator iter2 = iter--;

			float firstTime = iter->first;
			_Ty   firstValue = iter->second;

			float lastTime = iter2->first;
			_Ty	  lastValue = iter2->second;

			if( fTime == firstTime )
				return firstValue;

			float fraction = ( fTime - firstTime ) / ( lastTime - firstTime );
			res = firstValue + ( ( lastValue - firstValue ) * fraction );
		}
		return res;
	}

	void AddKey( float fTime, const _Ty &kValue )
	{
		if( m_bLoop )
		{
			if( fTime < m_fMaxTime  && fTime >= 0 )
			{
				(*this)[fTime] = kValue;
			}
		}
		else
		{
			(*this)[fTime] = kValue;
		}
	}

	float GetEndTime( void )
	{	
		if( !m_bLoop )
		{
			if( size() )
				return (--end())->first;
		}
		return m_fMaxTime;
	}

	float GetBeginTime( void )
	{
		if( !m_bLoop )
		{
			if( size() )
				return begin()->first;
		}
		return 0;
	}

	float GetTotalTime( void )
	{
		if( !m_bLoop )
		{
			if( size() )
				return (--end())->first - begin()->first;
		}
		return m_fMaxTime;
	}

	void Loop( bool bState, float fLoopTime )
	{
		m_bLoop = bState;
		m_fMaxTime = fLoopTime;
	}

	void Reset( void )
	{
		Clear();
	}

private:

	bool m_bLoop;
	float m_fMaxTime;

	FvLinearAnimation(const FvLinearAnimation&);
	FvLinearAnimation& operator=(const FvLinearAnimation&);

	friend std::ostream& operator<<(std::ostream&, const FvLinearAnimation&);
};

#endif // __FvLinearAnimation_H__