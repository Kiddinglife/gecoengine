//{future header message}
#ifndef __FvIntrusiveObject_H__
#define __FvIntrusiveObject_H__

#include <vector>
#include <algorithm>

template < class ELEMENT >
class FvIntrusiveObject
{
public:
	typedef std::vector< ELEMENT * > Container;

protected:
	FvIntrusiveObject( Container *& pContainer, bool shouldAdd = true ) :
		m_pkContainer( pContainer )
	{
		if (shouldAdd)
		{
			if (m_pkContainer == NULL)
			{
				m_pkContainer = new Container;
			}

			m_pkContainer->push_back( pThis() );
		}
	}

	virtual ~FvIntrusiveObject()
	{
		if (m_pkContainer != NULL)
		{
			typename FvIntrusiveObject::Container::iterator iter =
				std::find( m_pkContainer->begin(),
				m_pkContainer->end(),
				pThis() );

			m_pkContainer->erase( iter );

			if (m_pkContainer->empty())
			{
				delete m_pkContainer;
				m_pkContainer = NULL;
			}
		}
	}

private:
	ELEMENT *pThis()				{ return (ELEMENT *)this; }
	Container *&m_pkContainer;
};

#endif // __FvIntrusiveObject_H__