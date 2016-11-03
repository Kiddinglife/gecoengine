//{future header message}
#ifndef __FvVectorNoDestructor_H__
#define __FvVectorNoDestructor_H__

#include <vector>

template<class _Ty, class _A = std::allocator<_Ty>>
class FvVectorNoDestructor : public std::vector<_Ty, _A>
{
	typedef std::vector< _Ty, _A > BASE_TYPE;
	typedef typename BASE_TYPE::size_type size_type;

public:
	typedef typename BASE_TYPE::iterator iterator;
	typedef typename BASE_TYPE::const_iterator const_iterator;
	typedef typename BASE_TYPE::reverse_iterator reverse_iterator;
	typedef typename BASE_TYPE::const_reverse_iterator const_reverse_iterator;

	FvVectorNoDestructor() : m_iElements(0)
	{

	}

	void push_back(const _Ty& tElement)
	{
		if(BASE_TYPE::size() <= (size_type)m_iElements)
		{
			BASE_TYPE::push_back(tElement);
			m_iElements = (int)BASE_TYPE::size();
		}
		else
		{
			*(begin() + m_iElements) = tElement;
			m_iElements++;
		}
	}

	iterator end()
	{
		return begin() + m_iElements;
	}

	const_iterator end() const
	{
		return begin() + m_iElements;
	}

	reverse_iterator rbegin()
	{
		return reverse_iterator(begin() + m_iElements);
	}

	const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(begin() + m_iElements);
	}

	reverse_iterator rend()
	{
		return reverse_iterator(begin());
	}

	const_reverse_iterator rend() const
	{
		return const_reverse_iterator(begin());
	}


	size_type size()
	{
		return m_iElements;
	}

	size_type size() const
	{
		return m_iElements;
	}

	bool empty() const
	{
		return m_iElements == 0;
	}

	void resize(size_type kSz)
	{
		if(BASE_TYPE::size() < kSz)
		{
			BASE_TYPE::resize(kSz);
		}

		m_iElements = (int)kSz;
	}

	void clear()
	{
		m_iElements = 0;
	}

	_Ty& back()
	{
		return *(begin() + m_iElements - 1);
	}

	const _Ty& back() const
	{
		return *(begin() + m_iElements - 1);
	}

	void pop_back()
	{
		m_iElements --;
	}

	void erase(typename BASE_TYPE::iterator _P)
	{
		std::copy(_P + 1, end(), _P);
		m_iElements--;
	}

private:
	int m_iElements;
};

#endif /* __FvVectorNoDestructor_H__ */
